/**
 * LoraNodeGarage
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
 *   - SparkFun SAMD21 Mini
 *     Install:
 *       - Arduino SAMD Baoards
 *       - SparkFun SAMD Boards
 *
 * External libraries used:
 *   DHT sensor library by Adafruit v1.4.4
 *   LoRa by Sandeep Mistry v0.8.0
 *   NewPing by Tim Eckel v1.9.7
 *   CRC by Rob Tillaart v1.0.3
 *   Crypto by Rhys Weatherley v0.4.0
 *   For SAMD21:
 *     - FlashStorage (To emulate GenericEEPROM)
 */

/* TODO:
 * - Improve encryption of messages
 * - Use configuration measurement interval to update sensors (or remove that
 *   config)
 * - Use message sequence frCnt in LoRa header
 */
#include <AES.h>  // Crypto by Rhys Weatherley
#include <Arduino.h>
#include <CTR.h>      // Crypto by Rhys Weatherley
#include <DHT.h>      // DHT sensor library by Adafruit
#include <NewPing.h>  // NewPing by Tim Eckel
#include <SPI.h>

#include "Component.h"
#include "DistanceSensor.h"
#include "EeAdressMap.h"
#include "GarageCover.h"
#include "GenericEEPROM.h"
#include "HeightSensor.h"
#include "HumiditySensor.h"
#include "LoRaHandler.h"
#include "Node.h"
#include "PresenceBinarySensor.h"
#include "TemperatureSensor.h"
#include "Util.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 7

// Increment when breaking changes of configuration are made.
#define CONFIG_MAGIC 0x00

//#define DEBUG_SENSOR_VALUES
#define DEBUG_SENSOR_REPORT
#define DEBUG_SERVICE

#ifdef DEBUG_SENSOR_REPORT
#define LOG_SENSOR(sensor)   \
  printMillis(Serial);       \
  (sensor)->printTo(Serial);
#else
#define LOG_SENSOR(sensor)
#endif

#ifdef DEBUG_SERVICE
#define LOG_SERVICE(component, service)    \
  printMillis(Serial);                     \
  (component)->printTo(Serial, (service));
#else
#define LOG_SERVICE(component, service)
#endif

#define DHTPIN 4
#define DHTTYPE DHT22

#define SONAR_TRIGGER_PIN 7
#define SONAR_ECHO_PIN 6
#define SONAR_MAX_DISTANCE_CM 300

#define COVER_OPEN_PIN 8
#define COVER_CLOSED_PIN 9
#define COVER_RELAY_PIN A0

#define LORA_RESET_PIN 5

#define UPDATE_SENSORS_INTERVAL 1000

#define LORA_MY_ADDRESS 1
#define LORA_GATEWAY_ADDRESS 0

#define NUMBER_OF_COMPONENTS 6

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN, SONAR_MAX_DISTANCE_CM);

uint32_t lastRunTime = 0;

const char garageCoverName[] PROGMEM = "Port";
const char temperatureSensorName[] PROGMEM = "Temp";
const char humiditySensorName[] PROGMEM = "Hum";
const char distanceSensorName[] PROGMEM = "Dist";
const char heightSensorName[] PROGMEM = "Height";
const char carPresenceSensorName[] PROGMEM = "Car";

GarageCover garageCover = GarageCover(0, garageCoverName, COVER_CLOSED_PIN,
                                      COVER_OPEN_PIN, COVER_RELAY_PIN);
TemperatureSensor temperatureSensor =
    TemperatureSensor(1, temperatureSensorName, dht);
HumiditySensor humiditySensor = HumiditySensor(2, humiditySensorName, dht);
DistanceSensor distanceSensor = DistanceSensor(3, distanceSensorName, sonar);
HeightSensor heightSensor =
    HeightSensor(4, heightSensorName, distanceSensor.getSensor());
PresenceBinarySensor carPresenceSensor =
    PresenceBinarySensor(5, carPresenceSensorName, heightSensor.getSensor());

IComponent* components[NUMBER_OF_COMPONENTS] = {
    &garageCover,    &temperatureSensor, &humiditySensor,
    &distanceSensor, &heightSensor,      &carPresenceSensor};

Node node = Node(components, sizeof(components) / sizeof(components[0]));

CTR<AES128> ctrAes128;

LoRaHandler lora(LoRa, LORA_GATEWAY_ADDRESS, LORA_MY_ADDRESS /*, &ctrAes128*/);

const byte AES_KEY[16] PROGMEM = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                  0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                                  0x0C, 0x0D, 0x0E, 0x0F};
const byte CTR_IV[16] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x01};

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // Do nothing
  }

  printWelcomeMsg();

  uint8_t configMagicEe = GenericEEPROM.read(EE_ADDRESS_CONFIG_MAGIC);
  if (configMagicEe != CONFIG_MAGIC) {
    Serial.print(F("\nConfig magic word in EEPROM "));
    printHex(Serial, configMagicEe);
    Serial.print(F(" != "));
    printHex(Serial, static_cast<uint8_t>(CONFIG_MAGIC));
    Serial.print('!');
    Serial.print(F("\nErasing EEPROM"));
    for (uint16_t i = 0; i <= E2END; i++) {
      GenericEEPROM.write(i, 0xFF);
      if (i % 64 == 0) {
        Serial.print('.');
      }
    }
    Serial.print(F("Done!"));
  }
  loadConfigValuesForAllComponents();
  GenericEEPROM.write(EE_ADDRESS_CONFIG_MAGIC, CONFIG_MAGIC);
  GenericEEPROM.commit();

  dht.begin();

  ctrAes128.clear();
  ctrAes128.setCounterSize(4);
  ctrAes128.setKey(AES_KEY, ctrAes128.keySize());
  ctrAes128.setIV(CTR_IV, ctrAes128.ivSize());

  lora.setPins(LORA_DEFAULT_SS_PIN, LORA_RESET_PIN, LORA_DEFAULT_DIO0_PIN);
  if (!lora.begin(&onDiscoveryReqMsg, &onValueReqMsg, &onConfigReqMsg,
                  &onConfigSetReqMsg, &onServiceReqMsg)) {
    Serial.print(F("\nStarting LoRa failed!"));
    while (1) {
      // Do nothing
    }
  }

  sendDiscoveryMsgForAllComponents();
}

void loop() {
  auto curMillis = millis();
  if (curMillis - lastRunTime >= UPDATE_SENSORS_INTERVAL) {
    lastRunTime += UPDATE_SENSORS_INTERVAL;

    updateSensors();

#ifdef DEBUG_SENSOR_VALUES
    printMillis(Serial);
    printAllSensors(Serial);
#endif

    if (isReportDue()) {
      sendSensorValueForAllComponents();
    }
  }

  (void)lora.loraRx();
}

void onDiscoveryReqMsg(uint8_t entityId) {
  if (entityId == 255) {
    sendDiscoveryMsgForAllComponents();
    return;
  }
  sendDiscoveryMsgForEntity(entityId);
}

void onValueReqMsg(void) { sendSensorValueForAllComponents(); }

void onConfigReqMsg(uint8_t entityId) {
  if (entityId == 255) {
    sendConfigValuesForAllComponents();
    return;
  }
  sendConfigValuesForEntity(entityId);
}

void onConfigSetReqMsg(const ConfigValuePayloadT& payload) {
  IComponent* c = node.getComponentByEntityId(payload.entityId);
  if (c) {
    c->setConfigItemValues(payload.configValues, payload.numberOfConfigs);
  }
}

void onServiceReqMsg(const LoRaServiceItemT& item) {
  IComponent* c = node.getComponentByEntityId(item.entityId);
  if (c) {
    LOG_SERVICE(c, item.service);
    c->callService(item.service);
  }
}

static void loadConfigValuesForAllComponents() {
  for (uint8_t i = 0; i < node.getSize(); i++) {
    IComponent* c = node.getComponent(i);
    c->loadConfigValues();
  }
}

static void updateSensors() {
  Serial.print('.');

  for (uint8_t i = 0; i < node.getSize(); i++) {
    IComponent* c = node.getComponent(i);

    if (c->update()) {
      LOG_SENSOR(c);
      c->setReported();
    }
  }
}

static bool isReportDue() {
  for (uint8_t i = 0; i < node.getSize(); i++) {
    IComponent* c = node.getComponent(i);
    if (c != nullptr && c->isReportDue()) {
      return true;
    }
  }
  return false;
}

static void sendSensorValue(IComponent* component) {
  if (component == nullptr) {
    return;
  }

  lora.beginValueMsg();

  ValueItemT item;
  component->getValueItem(&item);
  lora.addValueItem(item);
  component->setReported();

  lora.endMsg();
}

static void sendSensorValueForAllComponents() {
  lora.beginValueMsg();

  for (uint8_t i = 0; i < node.getSize(); i++) {
    IComponent* c = node.getComponent(i);
    if (c != nullptr) {
      ValueItemT item;
      c->getValueItem(&item);
      lora.addValueItem(item);
      c->setReported();
    }
  }

  lora.endMsg();
}

static void sendSensorValueForComponentsWhereReportIsDue() {
  lora.beginValueMsg();

  uint8_t itemsAdded = 0;
  for (uint8_t i = 0; i < node.getSize(); i++) {
    IComponent* c = node.getComponent(i);
    if (c != nullptr && c->isReportDue()) {
      ValueItemT item;
      c->getValueItem(&item);
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
  IComponent* c = node.getComponentByEntityId(entityId);
  sendSensorValue(c);
}

static void sendConfigValues(const IComponent* component) {
  if (component == nullptr) {
    return;
  }

  ConfigItemValueT items[13];
  uint8_t numberOfConfigValues = component->getConfigItemValues(items, 13);

  lora.beginConfigsValueMsg(component->getEntityId());
  if (numberOfConfigValues > 0) {
    lora.addConfigItemValues(items, numberOfConfigValues);
  }
  lora.endMsg();
}

static void sendConfigValuesForAllComponents() {
  for (uint8_t i = 0; i < node.getSize(); i++) {
    const IComponent* c = node.getComponent(i);
    sendConfigValues(c);
    delay(500);
  }
}

void sendConfigValuesForEntity(uint8_t entityId) {
  const IComponent* c = node.getComponentByEntityId(entityId);
  sendConfigValues(c);
}

static void sendDiscoveryMsg(const IComponent* component) {
  if (component == nullptr) {
    return;
  }

  DiscoveryItemT item;
  component->getDiscoveryItem(&item);

  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(item);
  lora.endMsg();
}

static void sendDiscoveryMsgForAllComponents() {
  for (uint8_t i = 0; i < node.getSize(); i++) {
    const IComponent* c = node.getComponent(i);
    sendDiscoveryMsg(c);
    delay(500);
  }
}

void sendDiscoveryMsgForEntity(uint8_t entityId) {
  const IComponent* c = node.getComponentByEntityId(entityId);
  sendDiscoveryMsg(c);
}

void printWelcomeMsg() {
  Serial.print(F("LoRa Garage Node v"));
  printVersion(Serial, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  Serial.print(F(", Address="));
  Serial.print(LORA_MY_ADDRESS);
  Serial.print(F(", Gateway="));
  Serial.print(LORA_GATEWAY_ADDRESS);
}

#ifdef DEBUG_SENSOR_VALUES
static void printAllSensors(Print& p) { node.printTo(p); }
#endif
