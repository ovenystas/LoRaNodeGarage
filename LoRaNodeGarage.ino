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
      &onServiceReqMsg)) {
    Serial.println(F("Starting LoRa failed!"));
    while (1) {
      // Do nothing
    }
  }

  sendLoraDiscoveryMsg();
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

void onDiscoveryReqMsg() {
  sendLoraDiscoveryMsg();
}

void onValueReqMsg() {
  sendAllSensorValues();
}

void onConfigReqMsg() {
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
    valueAdded = true;
  }

  if (distanceSensor.update()) {
    LOG_SENSOR(distanceSensor);
    lora.addValueItem(distanceSensor.getEntityId(), distanceSensor.getValue());
    distanceSensor.setReported();
    valueAdded = true;
  }

  if (heightSensor.update()) {
    LOG_SENSOR(heightSensor);
    lora.addValueItem(heightSensor.getEntityId(), heightSensor.getValue());
    heightSensor.setReported();
    valueAdded = true;
  }

  if (carPresenceSensor.update()) {
    LOG_SENSOR(carPresenceSensor);
    lora.addValueItem(carPresenceSensor.getEntityId(),
        carPresenceSensor.getState());
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
    lora.addValueItem(humiditySensor.getEntityId(), humiditySensor.getValue());
    humiditySensor.setReported();
    valueAdded = true;
  }

  if (valueAdded) {
    lora.endMsg();
  }
}

static void sendAllSensorValues() {
  lora.beginValueMsg();

  lora.addValueItem(garageCover.getEntityId(),
      static_cast<uint8_t>(garageCover.getState()));

  lora.addValueItem(distanceSensor.getEntityId(), distanceSensor.getValue());
  distanceSensor.setReported();

  lora.addValueItem(heightSensor.getEntityId(), heightSensor.getValue());
  heightSensor.setReported();

  lora.addValueItem(carPresenceSensor.getEntityId(),
      carPresenceSensor.getState());

  lora.addValueItem(temperatureSensor.getEntityId(),
      temperatureSensor.getValue());
  temperatureSensor.setReported();

  lora.addValueItem(humiditySensor.getEntityId(), humiditySensor.getValue());
  humiditySensor.setReported();

  lora.endMsg();
}

static void sendLoraDiscoveryMsg() {
  uint8_t buffer[LORA_DISCOVERY_ITEM_LENGTH];
  lora.beginDiscoveryMsg();
  lora.addDiscoveryItem(garageCover.getDiscoveryMsg(buffer));
  lora.addDiscoveryItem(temperatureSensor.getDiscoveryMsg(buffer));
  lora.addDiscoveryItem(humiditySensor.getDiscoveryMsg(buffer));
  lora.addDiscoveryItem(distanceSensor.getDiscoveryMsg(buffer));
  lora.addDiscoveryItem(heightSensor.getDiscoveryMsg(buffer));
  lora.addDiscoveryItem(carPresenceSensor.getDiscoveryMsg(buffer));
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
