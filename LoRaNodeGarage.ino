/*
 *  Created on: 26 feb. 2021
 *      Author: oveny
 *
 * Board: Arduino Pro Mini, 8 MHz, 3.3 V
 * Libraries: DHT sensor library by Adafruit v1.4.4
 *            NewPing by Tim Eckel v1.9.7
 *            LoRa by Sandeep Mistry v0.8.0
 */

/* TODO:
 * - Store configs in EEPROM
 * - Handle millis() wrap-around
 */

#include <Arduino.h>
#include <DHT.h>  // DHT sensor library by Adafruit
#include <LoRa.h>
#include <NewPing.h>  // NewPing by Tim Eckel
#include <SPI.h>

#include "Component.h"
#include "DistanceSensor.h"
#include "GarageCover.h"
#include "HeightSensor.h"
#include "HumiditySensor.h"
#include "LoRaHandler.h"
#include "Node.h"
#include "PresenceBinarySensor.h"
#include "TemperatureSensor.h"
#include "Util.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 6

#define DEBUG_SENSOR_VALUES
#define DEBUG_SENSOR_REPORT
#define DEBUG_SERVICE

#ifdef DEBUG_SENSOR_REPORT
#define LOG_SENSOR(sensor) \
  printMillis(Serial);     \
  (sensor)->print(Serial); \
  Serial.println()
#else
#define LOG_SENSOR(sensor)
#endif

#ifdef DEBUG_SERVICE
#define LOG_SERVICE(component, service)  \
  printMillis(Serial);                   \
  (component)->print(Serial, (service)); \
  Serial.println()
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

#define UPDATE_SENSORS_INTERVAL 1000

#define LORA_MY_ADDRESS 1
#define LORA_GATEWAY_ADDRESS 0

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN, SONAR_MAX_DISTANCE_CM);

uint32_t nextRunTime = UPDATE_SENSORS_INTERVAL;

GarageCover garageCover =
    GarageCover(0, "Port", COVER_CLOSED_PIN, COVER_OPEN_PIN, COVER_RELAY_PIN);
TemperatureSensor temperatureSensor = TemperatureSensor(1, "Temperature", dht);
HumiditySensor humiditySensor = HumiditySensor(2, "Humidity", dht);
DistanceSensor distanceSensor = DistanceSensor(3, "Distance", sonar);
HeightSensor heightSensor =
    HeightSensor(4, "Height", distanceSensor.getSensor());
PresenceBinarySensor carPresenceSensor =
    PresenceBinarySensor(5, "Car", heightSensor.getSensor());

IComponent* components[6] = {&garageCover,    &temperatureSensor,
                             &humiditySensor, &distanceSensor,
                             &heightSensor,   &carPresenceSensor};

Node node = Node(components, sizeof(components) / sizeof(components[0]));

LoRaHandler lora(LoRa, Serial, LORA_GATEWAY_ADDRESS, LORA_MY_ADDRESS);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // Do nothing
  }

  printWelcomeMsg();

  if (!lora.begin(&onDiscoveryReqMsg, &onValueReqMsg, &onConfigReqMsg,
                  &onConfigSetReqMsg, &onServiceReqMsg)) {
    Serial.println(F("Starting LoRa failed!"));
    while (1) {
      // Do nothing
    }
  }

  sendDiscoveryMsgForAllComponents();

  dht.begin();
}

void loop() {
  (void)lora.loraRx();

  auto curMillis = millis();
  if (curMillis >= nextRunTime) {
    nextRunTime += UPDATE_SENSORS_INTERVAL;

    updateSensors();

#ifdef DEBUG_SENSOR_VALUES
    printUptime(Serial);
    printAllSensors(Serial);
#endif
  }
}

void onDiscoveryReqMsg(uint8_t entityId) {
  if (entityId == 255) {
    sendDiscoveryMsgForAllComponents();
    return;
  }
  sendDiscoveryMsgForEntity(entityId);
}

void onValueReqMsg(uint8_t entityId) {
  if (entityId == 255) {
    sendSensorValueForAllComponents();
    return;
  }
  sendSensorValueForEntity(entityId);
}

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

static void updateSensors() {
  for (uint8_t i = 0; i < node.getSize(); i++) {
    IComponent* c = node.getComponent(i);

    if (c->update()) {
      LOG_SENSOR(c);
      sendSensorValue(c);
    }
  }
}

static void sendSensorValue(IComponent* component) {
  if (component == nullptr) {
    return;
  }

  lora.beginValueMsg();

  ValueItemT item;
  component->getValueItem(&item);
  lora.addValueItem(&item);
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
      lora.addValueItem(&item);
      c->setReported();
    }
  }

  lora.endMsg();
}

void sendSensorValueForEntity(uint8_t entityId) {
  IComponent* c = node.getComponentByEntityId(entityId);
  sendSensorValue(c);
}

static void sendConfigValues(IComponent* component) {
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
    IComponent* c = node.getComponent(i);
    sendConfigValues(c);
  }
}

void sendConfigValuesForEntity(uint8_t entityId) {
  IComponent* c = node.getComponentByEntityId(entityId);
  sendConfigValues(c);
}

static void sendDiscoveryMsg(IComponent* component) {
  if (component == nullptr) {
    return;
  }

  DiscoveryItemT item;
  component->getDiscoveryItem(&item);

  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(&item);
  lora.endMsg();
}

static void sendDiscoveryMsgForAllComponents() {
  for (uint8_t i = 0; i < node.getSize(); i++) {
    IComponent* c = node.getComponent(i);
    sendDiscoveryMsg(c);
  }
}

void sendDiscoveryMsgForEntity(uint8_t entityId) {
  IComponent* c = node.getComponentByEntityId(entityId);
  sendDiscoveryMsg(c);
}

void printWelcomeMsg() {
  Serial.print(F("LoRa Garage Node v"));
  printVersion(Serial, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  Serial.print(F(", address="));
  Serial.print(LORA_MY_ADDRESS);
  Serial.print(F(", gateway="));
  Serial.println(LORA_GATEWAY_ADDRESS);
}

#ifdef DEBUG_SENSOR_VALUES
static void printAllSensors(Stream& stream) { node.print(stream); }
#endif
