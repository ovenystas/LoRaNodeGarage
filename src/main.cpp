/**
 * LoraDeviceGarage
 *
 * A LoRa wireless device that can sense if a Car is present in a garage,
 * sense the state of the garage door (open, closed, opening and closing),
 * sense temperature and humidity levels.
 *
 * Author:
 *   Ove Nystås
 *
 * Arduino board:
 *   - Arduino Pro Mini, ATmega328P, 3.3 V, 8 MHz
 *
 * External libraries used:
 *   Adafruit BusIO by Adafruit v1.17.4
 *   Adafruit Sensor by Adafruit v1.1.
 *   LoRa by Sandeep Mistry v0.8.0
 *   NewPing by Tim Eckel v1.9.7
 *   CRC by Rob Tillaart v1.0.3
 *   Crypto by Rhys Weatherley v0.4.0
 */

/* TODO:
 * - Improve encryption of messages
 * - Use message sequence frCnt in LoRa header
 */

// External libraries
#include <AES.h>      // Crypto
#include <CTR.h>      // Crypto
#include <NewPing.h>  // NewPing

// Arduino libraries
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>

// Local includes
#include "AHT20.h"
#include "AHTReader.h"
#include "Component.h"
#include "Device.h"
#include "DistanceSensor.h"
#include "EeAdressMap.h"
#include "GarageCover.h"
#include "HeightSensor.h"
#include "HumiditySensor.h"
#include "LoRaHandler.h"
#include "PresenceBinarySensor.h"
#include "TemperatureSensor.h"
#include "Util.h"

// Version information
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 7

// Baud rate for serial UART communication
#define SERIAL_BAUD_RATE 115200

// Increment when breaking changes of configuration are made.
#define CONFIG_MAGIC 0x00

// Debug flags
#define DEBUG_SENSOR_VALUES true
#define DEBUG_SENSOR_REPORT false
#define DEBUG_SERVICE true

// Debugging helper macros
#if (DEBUG_SENSOR_REPORT)
#define LOG_SENSOR(sensor) \
  printMillis(Serial);     \
  (sensor)->printTo(Serial);
#else
#define LOG_SENSOR(sensor)
#endif

#if (DEBUG_SERVICE)
#define LOG_SERVICE(component, service) \
  printMillis(Serial);                  \
  (component)->printTo(Serial, (service));
#else
#define LOG_SERVICE(component, service)
#endif

// NewPing Sonar configuration
#define SONAR_TRIGGER_PIN 7
#define SONAR_ECHO_PIN 6
#define SONAR_MAX_DISTANCE_CM 300

// Cover configuration
#define COVER_OPEN_PIN 8
#define COVER_CLOSED_PIN 9
#define COVER_RELAY_PIN A0

// AHT20 configuration
#define AHT20_SENSOR_ENABLED true

// LoRa configuration
#define LORA_ENABLED true
#define LORA_RESET_PIN 5
#define LORA_SS_PIN LORA_DEFAULT_SS_PIN
#define LORA_DIO0_PIN LORA_DEFAULT_DIO0_PIN
#define LORA_MY_ADDRESS 1
#define LORA_GATEWAY_ADDRESS 0

// Sensor update interval
#define UPDATE_SENSORS_INTERVAL 1000

// Local constants
// ----------------------------------------------------------------
// Names of components (stored in flash memory to save RAM)
const char garageCoverName[] PROGMEM = "Port";
const char temperatureSensorName[] PROGMEM = "Temperature";
const char humiditySensorName[] PROGMEM = "Humidity";
const char distanceSensorName[] PROGMEM = "Distance";
const char heightSensorName[] PROGMEM = "Height";
const char carPresenceSensorName[] PROGMEM = "Car Presence";

// Entity IDs for components
constexpr uint8_t ENTITY_ID_GARAGE_COVER = 0;

constexpr uint8_t ENTITY_ID_TEMPERATURE_SENSOR =
    ENTITY_ID_GARAGE_COVER + GarageCover::getEntityCount();

constexpr uint8_t ENTITY_ID_HUMIDITY_SENSOR =
    ENTITY_ID_TEMPERATURE_SENSOR + TemperatureSensor::getEntityCount();

constexpr uint8_t ENTITY_ID_DISTANCE_SENSOR =
    ENTITY_ID_HUMIDITY_SENSOR + HumiditySensor::getEntityCount();

constexpr uint8_t ENTITY_ID_HEIGHT_SENSOR =
    ENTITY_ID_DISTANCE_SENSOR + DistanceSensor::getEntityCount();

constexpr uint8_t ENTITY_ID_CAR_PRESENCE_SENSOR =
    ENTITY_ID_HEIGHT_SENSOR + HeightSensor::getEntityCount();

// Local variables
// ----------------------------------------------------------------
AHT20 aht;
AHTReader ahtReader(aht);
NewPing sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN, SONAR_MAX_DISTANCE_CM);

uint32_t lastRunTime = 0;

// Components
GarageCover garageCover = GarageCover(0, garageCoverName, COVER_CLOSED_PIN,
                                      COVER_OPEN_PIN, COVER_RELAY_PIN);

TemperatureSensor temperatureSensor = TemperatureSensor(
    ENTITY_ID_TEMPERATURE_SENSOR, temperatureSensorName, ahtReader);

HumiditySensor humiditySensor =
    HumiditySensor(ENTITY_ID_HUMIDITY_SENSOR, humiditySensorName, ahtReader);

DistanceSensor distanceSensor =
    DistanceSensor(ENTITY_ID_DISTANCE_SENSOR, distanceSensorName, sonar);

HeightSensor heightSensor = HeightSensor(
    ENTITY_ID_HEIGHT_SENSOR, heightSensorName, distanceSensor.getSensor());

PresenceBinarySensor carPresenceSensor =
    PresenceBinarySensor(ENTITY_ID_CAR_PRESENCE_SENSOR, carPresenceSensorName,
                         heightSensor.getSensor());

// Array of all components for easy iteration
IComponent* components[] = {&garageCover,    &temperatureSensor,
                            &humiditySensor, &distanceSensor,
                            &heightSensor,   &carPresenceSensor};

// Device instance that holds all components and provides helper functions to
// access them
Device device = Device(components, sizeof(components) / sizeof(components[0]));

// Crypto for encrypting LoRa messages
// CTR<AES128> ctrAes128;

// AES key and IV for CTR mode (stored in flash memory to save RAM)
const byte AES_KEY[16] PROGMEM = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                  0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                                  0x0C, 0x0D, 0x0E, 0x0F};
const byte CTR_IV[16] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x01};

// LoRa handler for sending and receiving LoRa messages
LoRaHandler lora(LoRa, LORA_GATEWAY_ADDRESS, LORA_MY_ADDRESS /*, &ctrAes128*/);

// Local function declarations
// ----------------------------------------------------------------
void onDiscoveryReqMsg(uint8_t entityId);
void onValueReqMsg(void);
void onValueSetReqMsg(const ValueItemT& item);
void onServiceReqMsg(const LoRaServiceItemT& item);
int freeRam();

// Local function definitions
// ----------------------------------------------------------------
static void loadConfigValuesForAllComponents() {
  for (uint8_t i = 0; i < device.getSize(); i++) {
    IComponent* c = device.getComponent(i);
    if (c == nullptr) {
      continue;
    }

    c->loadConfigValues();
  }
}

static void updateSensors() {
  // printMillis(Serial);
  // Serial.println("Updating sensors");

#if (AHT20_SENSOR_ENABLED)
  ahtReader.update();
#endif

  for (uint8_t i = 0; i < device.getSize(); i++) {
    IComponent* c = device.getComponent(i);
    if (c == nullptr) {
      continue;
    }

    if (c->update()) {
      LOG_SENSOR(c);
      // c->setReported();
    }
  }
}

static bool isReportDue() {
  uint8_t reportDueCount = 0;

  for (uint8_t i = 0; i < device.getSize(); i++) {
    IComponent* c = device.getComponent(i);
    if (c == nullptr) {
      continue;
    }

    if (c->isReportDue()) {
      if (reportDueCount == 0) {
        printMillis(Serial);
        Serial.print(F("Report is due for entityIds: "));
      } else {
        Serial.print(F(", "));
      }
      Serial.print(c->getEntityId());
      reportDueCount++;
    }
  }

  const bool isReportDue = reportDueCount > 0;

  if (isReportDue) {
    Serial.println();
  }

  return isReportDue;
}

static void sendSensorValueForComponent(IComponent* component) {
  if (component == nullptr) {
    return;
  }

  lora.beginValueMsg();

  ValueItemT item;
  component->getValueItem(item);
  lora.addValueItem(item);
  component->setReported();

  lora.endMsg();
}

static void sendSensorValueForAllComponents() {
  lora.beginValueMsg();

  for (uint8_t i = 0; i < device.getSize(); i++) {
    IComponent* c = device.getComponent(i);
    if (c == nullptr) {
      continue;
    }

    ValueItemT item;
    c->getValueItem(item);
    lora.addValueItem(item);
    c->setReported();
  }

  lora.endMsg();

  printMillis(Serial);
  Serial.println(F("Sent sensor values for all components"));
}

static void sendSensorValueForComponentsWhereReportIsDue() {
  lora.beginValueMsg();

  uint8_t itemsAdded = 0;
  for (uint8_t i = 0; i < device.getSize(); i++) {
    IComponent* c = device.getComponent(i);
    if (c == nullptr) {
      continue;
    }

    if (c->isReportDue()) {
      ValueItemT item;
      c->getValueItem(item);
      lora.addValueItem(item);
      itemsAdded++;
      c->setReported();
    }
  }

  if (itemsAdded > 0) {
    lora.endMsg();
  }
}

void sendSensorValueForEntity(uint8_t entityId) {
  IComponent* c = device.getComponentByEntityId(entityId);
  sendSensorValueForComponent(c);
}

static void sendConfigValues(const IComponent* component) {
  if (component == nullptr) {
    return;
  }

  uint8_t numberOfConfigValues = component->getNumConfigItems();
  if (numberOfConfigValues == 0) {
    return;
  }

  lora.beginValueMsg();
  for (uint8_t i = 0; i < numberOfConfigValues; i++) {
    ValueItemT item;
    component->getConfigValue(item, i);
    lora.addValueItem(item);
  }
  lora.endMsg();
}

static void sendConfigValuesForAllComponents() {
  for (uint8_t i = 0; i < device.getSize(); i++) {
    const IComponent* c = device.getComponent(i);
    sendConfigValues(c);
    delay(500);
  }
}

void sendConfigValuesForEntity(uint8_t entityId) {
  const IComponent* c = device.getComponentByEntityId(entityId);
  sendConfigValues(c);
}

static void sendDiscoveryMsg(DiscoveryEntityT& discovery_entity) {
  printMillis(Serial);
  Serial.print(F("Sending discovery for "));
  Serial.print(discovery_entity.entityId);
  Serial.print(':');
  Serial.println(
      reinterpret_cast<const __FlashStringHelper*>(discovery_entity.name));

  lora.beginDiscoveryMsg();
  lora.addDiscoveryEntity(discovery_entity);
  lora.endMsg();
}

static void sendDiscoveryMsgForComponent(const IComponent* component) {
  if (component == nullptr) {
    return;
  }

  const uint8_t numEntities = component->getNumEntities();

  for (uint8_t i = 0; i < numEntities; i++) {
    if (i > 0) {
      delay(500);
    }

    DiscoveryEntityT discovery_entity;
    if (component->getDiscoveryEntity(discovery_entity, i)) {
      sendDiscoveryMsg(discovery_entity);
    }
  }
}

static void sendDiscoveryMsgForAllComponents() {
  for (uint8_t i = 0; i < device.getSize(); i++) {
    if (i > 0) {
      delay(500);
    }

    const IComponent* c = device.getComponent(i);
    sendDiscoveryMsgForComponent(c);
  }
}

static void sendDiscoveryMsgForEntity(uint8_t entityId) {
  const IComponent* c = device.getComponentByEntityId(entityId);
  sendDiscoveryMsgForComponent(c);
}

static void printWelcomeMsg() {
  printMillis(Serial);
  Serial.print(F("LoRa Garage Device v"));
  printVersion(Serial, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  Serial.print(F(", Address="));
  Serial.print(LORA_MY_ADDRESS);
  Serial.print(F(", Gateway="));
  Serial.println(LORA_GATEWAY_ADDRESS);
}

static void eraseEepromConfig() {
  printMillis(Serial);
  Serial.print(F("Erasing EEPROM"));

  for (uint16_t i = 0; i <= E2END; i++) {
    EEPROM.write(i, 0xFF);
    if (i % 64 == 0) {
      Serial.print('.');
    }
  }

  Serial.print(F("Done!"));
}

static void checkEepromConfig() {
  printMillis(Serial);

  uint8_t configMagicEe = EEPROM.read(EE_ADDRESS_CONFIG_MAGIC);

  if (configMagicEe != CONFIG_MAGIC) {
    Serial.print(F("WRN: Config magic word in EEPROM "));
    printHex(Serial, configMagicEe);
    Serial.print(F(" != "));
    printHex(Serial, static_cast<uint8_t>(CONFIG_MAGIC));
    Serial.print('!');
    eraseEepromConfig();
  } else {
    Serial.print(F("Config magic word in EEPROM OK: "));
    printHex(Serial, configMagicEe);
  }

  Serial.println();
}

#if (DEBUG_SENSOR_VALUES)
static void printAllSensors(Print& p) { device.printTo(p); }
#endif

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(100);  // Allow serial connection to stabilize

  printWelcomeMsg();

  checkEepromConfig();

  printMillis(Serial);
  Serial.println(F("Loading config values..."));
  loadConfigValuesForAllComponents();
  EEPROM.write(EE_ADDRESS_CONFIG_MAGIC, CONFIG_MAGIC);

#if (AHT20_SENSOR_ENABLED)
  printMillis(Serial);
  Serial.print(F("Initializing AHT reader..."));
  ahtReader.begin();
#else
  printMillis(Serial);
  Serial.println(F("AHT20 sensor is disabled"));
#endif

#if (LORA_ENABLED)
  // ctrAes128.clear();
  // ctrAes128.setCounterSize(4);
  // ctrAes128.setKey(AES_KEY, ctrAes128.keySize());
  // ctrAes128.setIV(CTR_IV, ctrAes128.ivSize());

  printMillis(Serial);
  Serial.print(F("Starting LoRa..."));

  LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!lora.begin(&onDiscoveryReqMsg, &onValueReqMsg, &onValueSetReqMsg,
                  &onServiceReqMsg)) {
    Serial.println(F("ERROR: Starting LoRa failed!"));
    while (1) {
      delay(1000);
    }
  } else {
    printMillis(Serial);
    Serial.print(F("LoRa started"));
  }

  printMillis(Serial);
  Serial.println(F("Sending discovery for all components"));
  sendDiscoveryMsgForAllComponents();
  printMillis(Serial);
  Serial.println(F("Discovery sent for all components"));
#else
  printMillis(Serial);
  Serial.println(F("LoRa is disabled"));
#endif

  printMillis(Serial);
  Serial.println(F("Setup complete, starting loop"));
}

void loop() {
  auto curMillis = millis();

  if (curMillis - lastRunTime >= UPDATE_SENSORS_INTERVAL) {
    lastRunTime += UPDATE_SENSORS_INTERVAL;
    updateSensors();

#if (DEBUG_SENSOR_VALUES)
    printMillis(Serial);
    printAllSensors(Serial);
#endif

#if (LORA_ENABLED)
    if (isReportDue()) {
      sendSensorValueForAllComponents();
    }
#endif
  }

#if (LORA_ENABLED)
  // FIXME: This crashes sometimes
  (void)lora.loraRx();
#endif
}

void onDiscoveryReqMsg(uint8_t entityId) {
  if (entityId == 255) {
    sendDiscoveryMsgForAllComponents();
    return;
  }
  sendDiscoveryMsgForEntity(entityId);
}

void onValueReqMsg(void) { sendSensorValueForAllComponents(); }

void onValueSetReqMsg(const ValueItemT& valueItem) {
  IComponent* c = device.getComponentByEntityId(valueItem.entityId);
  if (c) {
    c->setValueItem(valueItem);
  }
}

void onServiceReqMsg(const LoRaServiceItemT& item) {
  IComponent* c = device.getComponentByEntityId(item.entityId);
  if (c) {
    LOG_SERVICE(c, item.service);
    c->callService(item.service);
  }
}
