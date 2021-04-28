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

  sendAllLoraDiscoveryMsgs();
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
    sendAllLoraDiscoveryMsgs();
    return;
  }

  sendLoraDiscoveryMsg(entityId);
}

static void addValueItem(BinarySensor& binarySensor) {
  lora.addValueItem(binarySensor.getEntityId(), binarySensor.getState());
}

static void addValueItem(Cover& cover) {
  lora.addValueItem(cover.getEntityId(), static_cast<uint8_t>(cover.getState()));
}

template <typename T>
static void addValueItem(Sensor<T>& sensor) {
  lora.addValueItem(sensor.getEntityId(), sensor.getValue());
}

void sendSensorValue(uint8_t entityId) {
  lora.beginValueMsg();

  switch (entityId) {
    case 0:
      addValueItem(garageCover);
      garageCover.setReported();
      break;

    case 1:
      addValueItem(distanceSensor);
      distanceSensor.setReported();
      break;

    case 2:
      addValueItem(heightSensor);
      heightSensor.setReported();
      break;

    case 3:
      addValueItem(carPresenceSensor);
      carPresenceSensor.setReported();
      break;

    case 4:
      addValueItem(temperatureSensor);
      temperatureSensor.setReported();
      break;

    case 5:
      addValueItem(humiditySensor);
      humiditySensor.setReported();
      break;

    default:
      return;
  }

  lora.endMsg();
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
  bool valueAdded = false;

  lora.beginValueMsg();

  if (garageCover.update()) {
    LOG_SENSOR(garageCover);
    lora.addValueItem(garageCover.getEntityId(),
        static_cast<uint8_t>(garageCover.getState()));
    garageCover.setReported();
    valueAdded = true;
  }

  if (distanceSensor.update()) {
    LOG_SENSOR(distanceSensor);
    lora.addValueItem(distanceSensor.getEntityId(),
        distanceSensor.getValue());
    distanceSensor.setReported();
    valueAdded = true;
  }

  if (heightSensor.update()) {
    LOG_SENSOR(heightSensor);
    lora.addValueItem(heightSensor.getEntityId(),
        heightSensor.getValue());
    heightSensor.setReported();
    valueAdded = true;
  }

  if (carPresenceSensor.update()) {
    LOG_SENSOR(carPresenceSensor);
    lora.addValueItem(carPresenceSensor.getEntityId(),
        carPresenceSensor.getState());
    carPresenceSensor.setReported();
    valueAdded = true;
  }

  if (temperatureSensor.update()) {
    LOG_SENSOR(temperatureSensor);
    lora.addValueItem(temperatureSensor.getEntityId(),
        temperatureSensor.getValue());
    temperatureSensor.setReported();
    valueAdded = true;
  }

  if (humiditySensor.update()) {
    LOG_SENSOR(humiditySensor);
    lora.addValueItem(humiditySensor.getEntityId(),
        humiditySensor.getValue());
    humiditySensor.setReported();
    valueAdded = true;
  }

  if (valueAdded) {
    lora.endMsg();
  }
}

static void sendAllSensorValues() {
  lora.beginValueMsg();

  addValueItem(garageCover);
  garageCover.setReported();

  addValueItem(distanceSensor);
  distanceSensor.setReported();

  addValueItem(heightSensor);
  heightSensor.setReported();

  addValueItem(carPresenceSensor);
  carPresenceSensor.setReported();

  addValueItem(temperatureSensor);
  temperatureSensor.setReported();

  addValueItem(humiditySensor);
  humiditySensor.setReported();

  lora.endMsg();
}

static void sendAllConfigValues(uint8_t entityId) {
  lora.beginConfigsValueMsg(entityId);
  // TODO: Add Config values from all configs in entityId
  lora.endMsg();
}

static void sendAllLoraDiscoveryMsgs() {
  uint8_t buffer[max(LORA_DISCOVERY_ITEM_LENGTH, LORA_CONFIG_ITEMS_MAX * LORA_CONFIG_ITEM_LENGTH)];

  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(buffer, garageCover.getDiscoveryMsg(buffer));
  lora.endMsg();

  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(buffer, temperatureSensor.getDiscoveryMsg(buffer));
  lora.endMsg();

  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(buffer, humiditySensor.getDiscoveryMsg(buffer));
  lora.endMsg();

  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(buffer, distanceSensor.getDiscoveryMsg(buffer));
  lora.endMsg();

  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(buffer, heightSensor.getDiscoveryMsg(buffer));
  lora.endMsg();

  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(buffer, carPresenceSensor.getDiscoveryMsg(buffer));
  lora.endMsg();
}

void sendLoraDiscoveryMsg(uint8_t entityId) {
  uint8_t buffer[max(LORA_DISCOVERY_ITEM_LENGTH, LORA_CONFIG_ITEMS_MAX * LORA_CONFIG_ITEM_LENGTH)];

  lora.beginDiscoveryMsg();

  switch (entityId) {
    case 0:
      lora.addDiscoveryItem(buffer, garageCover.getDiscoveryMsg(buffer));
      break;

    case 1:
      lora.addDiscoveryItem(buffer, distanceSensor.getDiscoveryMsg(buffer));
      break;

    case 2:
      lora.addDiscoveryItem(buffer, heightSensor.getDiscoveryMsg(buffer));
      break;

    case 3:
      lora.addDiscoveryItem(buffer, carPresenceSensor.getDiscoveryMsg(buffer));
      break;

    case 4:
      lora.addDiscoveryItem(buffer, temperatureSensor.getDiscoveryMsg(buffer));
      break;

    case 5:
      lora.addDiscoveryItem(buffer, humiditySensor.getDiscoveryMsg(buffer));
      break;

    default:
      return;
  }

  lora.endMsg();
}

#ifdef DEBUG_SENSOR_VALUES
static void printAllSensors(Stream& stream) {
  garageCover.print(stream);
  stream.print(", ");
  distanceSensor.print(stream);
  stream.print(", ");
  heightSensor.print(stream);
  stream.print(", ");
  carPresenceSensor.print(stream);
  stream.print(", ");
  temperatureSensor.print(stream);
  stream.print(", ");
  humiditySensor.print(stream);
  stream.println();
}
#endif
