/*
 *  Created on: 26 feb. 2021
 *      Author: oveny
 */

#include <Arduino.h>
#include <GarageCover.h>
#include <PresenceBinarySensor.h>
#include <SPI.h>
#include <DHT.h>
#include <NewPing.h>

#include "LoRaHandler.h"
#include "TemperatureSensor.h"
#include "HumiditySensor.h"
#include "DistanceSensor.h"
#include "HeightSensor.h"
#include "Node.h"
#include "Util.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 2

#define DEBUG_SENSOR_VALUES
#define DEBUG_SENSOR_REPORT
#define DEBUG_COVER

#ifdef DEBUG_SENSOR_REPORT
# define LOG_SENSOR(sensor) printMillis(Serial); (sensor).print(Serial); Serial.println()
#else
# define LOG_SENSOR(sensor)
#endif

#ifdef DEBUG_COVER
# define LOG_COVER_SERVICE(cover, service) printMillis(Serial); (cover).print(Serial, (service)); Serial.println()
#else
# define LOG_COVER_SERVICE(sensor, service)
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

#ifdef DEBUG_SENSOR_VALUES
static void printAllSensors(Stream& stream);
#endif

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN, SONAR_MAX_DISTANCE_CM);

uint32_t nextRunTime = UPDATE_SENSORS_INTERVAL;

GarageCover garageCover = GarageCover(1, "Port",
    COVER_CLOSED_PIN, COVER_OPEN_PIN, COVER_RELAY_PIN);
TemperatureSensor temperatureSensor = TemperatureSensor(2, "Temperature", dht);
HumiditySensor humiditySensor = HumiditySensor(3, "Humidity", dht);
DistanceSensor distanceSensor = DistanceSensor(4, "Distance", sonar);
HeightSensor heightSensor = HeightSensor(5, "Height", distanceSensor);
PresenceBinarySensor carPresenceSensor = PresenceBinarySensor(6, "Car",
    heightSensor);

Node node = Node(&garageCover, &temperatureSensor, &humiditySensor,
    &distanceSensor, &heightSensor, &carPresenceSensor);

LoRaHandler lora;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // Do nothing
  }

  Serial.print(F("LoRa Garage Node v"));
  printVersion(Serial, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  Serial.print(F(", address="));
  Serial.print(MY_ADDRESS);
  Serial.print(F(", gateway="));
  Serial.println(GATEWAY_ADDRESS);

  if (!lora.begin(&onDiscoveryReqMsg, &onValueReqMsg, &onConfigReqMsg,
      &onConfigSetReqMsg, &onServiceReqMsg)) {
    Serial.println(F("Starting LoRa failed!"));
    while (1) {
      // Do nothing
    }
  }

  sendAllDiscoveryMsgs();
  dht.begin();
}

void loop() {
  lora.loraRx();

  auto curMillis = millis();
  if (curMillis >= nextRunTime) {
    nextRunTime += UPDATE_SENSORS_INTERVAL;

    updateSensors();

#ifdef DEBUG_SENSOR_VALUES
    printMillis(Serial);
    printAllSensors(Serial);
#endif
  }
}

void onDiscoveryReqMsg(uint8_t entityId) {
  if (entityId == 255) {
    sendAllDiscoveryMsgs();
    return;
  }
  sendDiscoveryMsg(entityId);
}

void onValueReqMsg(uint8_t entityId) {
  if (entityId == 255) {
    sendAllSensorValues();
    return;
  }

  sendSensorValue(entityId);
}

void onConfigReqMsg(uint8_t entityId) {
  sendAllConfigValues(entityId);
}

void onConfigSetReqMsg(uint8_t entityId) {
  (void)entityId;
  // TODO: Set configuration of entityId
}

void onServiceReqMsg(const LoRaServiceItemT& item) {
  if (item.entityId == garageCover.getEntityId()) {
    Cover::Service service = static_cast<Cover::Service>(item.service);
    LOG_COVER_SERVICE(garageCover, service);
    garageCover.callService(service);
  }
}

static void updateSensors() {
  uint8_t buffer[1 + sizeof(uint32_t)];
  bool valueAdded = false;

  lora.beginValueMsg();

  for (uint8_t i = 0; i < node.getSize(); i++) {
    Component* c = node.getComponent(i);

    if (c->update()) {
      LOG_SENSOR(*c);

      uint8_t length = c->getValueMsg(buffer);

      if (length) {
        lora.addValueItem(buffer, length);
      }

      c->setReported();

      valueAdded = true;
    }
  }

  if (valueAdded) {
    lora.endMsg();
  }
}

static void sendAllSensorValues() {
  uint8_t buffer[1 + sizeof(uint32_t)];
  bool valueAdded = false;

  lora.beginValueMsg();

  for (uint8_t i = 0; i < node.getSize(); i++) {
    uint8_t length = node.getValueMsg(buffer, i);

    if (length) {
      lora.addValueItem(buffer, length);
      valueAdded = true;
    }
  }

  if (valueAdded) {
    lora.endMsg();
  }
}

void sendSensorValue(uint8_t entityId) {
  uint8_t buffer[1 + sizeof(uint32_t)];

  Component* c = node.getComponentByEntityId(entityId);

  uint8_t length = c->getValueMsg(buffer);

  if (length) {
    lora.beginValueMsg();
    lora.addValueItem(buffer, length);
    c->setReported();
    lora.endMsg();
  }
}

static void sendAllConfigValues(uint8_t entityId) {
  uint8_t buffer[LORA_MAX_PAYLOAD_LENGTH];

  uint8_t length = node.getConfigItemValuesMsg(buffer, entityId);

  if (length) {
    lora.beginConfigsValueMsg();
    lora.addConfigItemValues(buffer, length);
    lora.endMsg();
  }
}

static void sendAllDiscoveryMsgs() {
  uint8_t buffer[LORA_MAX_PAYLOAD_LENGTH];

  for (uint8_t i = 0; i < 6; i++) {
    uint8_t length = node.getDiscoveryMsg(buffer, i);

    if (length) {
      lora.beginDiscoveryMsg();
      lora.addDiscoveryItem(buffer, length);
      lora.endMsg();
    }
  }
}

void sendDiscoveryMsg(uint8_t entityId) {
  uint8_t buffer[LORA_MAX_PAYLOAD_LENGTH];

  uint8_t length = node.getDiscoveryMsgByEntityId(buffer, entityId);

  if (length) {
    lora.beginDiscoveryMsg();
    lora.addDiscoveryItem(buffer, length);
    lora.endMsg();
  }
}

#ifdef DEBUG_SENSOR_VALUES
static void printAllSensors(Stream& stream) {
  node.print(stream);
}
#endif
