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

#ifdef DEBUG_SENSOR_REPORT
# define LOG_SENSOR(sensor) printMillis(Serial); sensor.print(Serial); Serial.println()
#else
# define LOG_SENSOR(sensor)
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

static void updateSensors();
static void handleLoraMessage();

#ifdef DEBUG_SENSOR_VALUES
static void printAllSensors(Stream& stream);
#endif

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN, SONAR_MAX_DISTANCE_CM);

uint32_t nextRunTime = UPDATE_SENSORS_INTERVAL;

GarageCover garageCover = GarageCover(1, "Port", COVER_CLOSED_PIN,
    COVER_OPEN_PIN);
TemperatureSensor temperatureSensor = TemperatureSensor(2, "Temperature", dht);
HumiditySensor humiditySensor = HumiditySensor(3, "Humidity", dht);
DistanceSensor distanceSensor = DistanceSensor(4, "Distance", sonar);
HeightSensor heightSensor = HeightSensor(5, "Height", distanceSensor);
PresenceBinarySensor carPresenceSensor = PresenceBinarySensor(6, "Car",
    heightSensor);

LoRaHandler lora;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.print(F("Lora Garage Node v"));
  printVersion(Serial, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  Serial.print(F(", address="));
  Serial.print(MY_ADDRESS);
  Serial.print(F(", gateway="));
  Serial.println(GATEWAY_ADDRESS);

  if (!lora.begin()) {
    Serial.println(F("Starting LoRa failed!"));
    while (1)
      ;
  }

  uint8_t buffer[LORA_DISCOVERY_MSG_LENGTH];
  lora.sendDiscoveryMsg(garageCover.getDiscoveryMsg(buffer));
  lora.sendDiscoveryMsg(temperatureSensor.getDiscoveryMsg(buffer));
  lora.sendDiscoveryMsg(humiditySensor.getDiscoveryMsg(buffer));
  lora.sendDiscoveryMsg(distanceSensor.getDiscoveryMsg(buffer));
  lora.sendDiscoveryMsg(heightSensor.getDiscoveryMsg(buffer));
  lora.sendDiscoveryMsg(carPresenceSensor.getDiscoveryMsg(buffer));

  dht.begin();
}

void loop() {
  if (lora.loraRx()) {
    handleLoraMessage();
  }

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

static void updateSensors() {
  if (garageCover.update()) {
    LOG_SENSOR(garageCover);
    lora.sendValue(garageCover.getEntityId(),
        static_cast<uint8_t>(garageCover.getState()));
  }

  if (distanceSensor.update()) {
    LOG_SENSOR(distanceSensor);
    lora.sendValue(distanceSensor.getEntityId(), distanceSensor.getValue());
    distanceSensor.setReported();
  }

  if (heightSensor.update()) {
    LOG_SENSOR(heightSensor);
    lora.sendValue(heightSensor.getEntityId(), heightSensor.getValue());
    heightSensor.setReported();
  }

  if (carPresenceSensor.update()) {
    LOG_SENSOR(carPresenceSensor);
    lora.sendValue(carPresenceSensor.getEntityId(),
        carPresenceSensor.getState());
  }

  if (temperatureSensor.update()) {
    LOG_SENSOR(temperatureSensor);
    lora.sendValue(temperatureSensor.getEntityId(),
        temperatureSensor.getValue());
    temperatureSensor.setReported();
  }

  if (humiditySensor.update()) {
    LOG_SENSOR(humiditySensor);
    lora.sendValue(humiditySensor.getEntityId(), humiditySensor.getValue());
    humiditySensor.setReported();
  }
}

static void handleLoraMessage() {
  String msg = "";
  if (msg == "Cover open" || msg == "Cover close" || msg == "Cover stop"
      || msg == "Cover toggle") {

    digitalWrite(COVER_RELAY_PIN, HIGH);
    delay(500);
    digitalWrite(COVER_RELAY_PIN, LOW);
  }
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
