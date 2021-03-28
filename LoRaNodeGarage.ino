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

#define DEBUG_SENSOR_VALUES
#define DEBUG_SENSOR_REPORT

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
static void printSensors(Stream& stream);
static void handleLoraMessage();

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN, SONAR_MAX_DISTANCE_CM);

uint32_t nextRunTime = UPDATE_SENSORS_INTERVAL;

GarageCover garageCover = GarageCover(1, COVER_CLOSED_PIN, COVER_OPEN_PIN);
TemperatureSensor temperatureSensor = TemperatureSensor(2, dht);
HumiditySensor humiditySensor = HumiditySensor(3, dht);
DistanceSensor distanceSensor = DistanceSensor(4, sonar);
HeightSensor heightSensor = HeightSensor(5, distanceSensor);
PresenceBinarySensor carPresenceSensor = PresenceBinarySensor(6, heightSensor);

LoRaHandler lora;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.print(F("LoRa Node, address="));
  Serial.print(MY_ADDRESS);
  Serial.print(F(", gateway="));
  Serial.println(GATEWAY_ADDRESS);

  if (!lora.begin()) {
    Serial.println(F("Starting LoRa failed!"));
    while (1);
  }

  uint8_t buffer[LORA_DISCOVERY_MSG_LENGTH];
  lora.sendDiscoveryMsg(garageCover.getDiscoveryMsg(buffer)),
  lora.sendDiscoveryMsg(temperatureSensor.getDiscoveryMsg(buffer));
  lora.sendDiscoveryMsg(humiditySensor.getDiscoveryMsg(buffer));
  lora.sendDiscoveryMsg(heightSensor.getDiscoveryMsg(buffer));
  lora.sendDiscoveryMsg(distanceSensor.getDiscoveryMsg(buffer));
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
    printSensors(Serial);
#endif
  }
}

static void updateSensors() {
  if (garageCover.update()) {
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("GarageCover: "));
    Serial.println(garageCover.getStateName());
#endif
    lora.sendValue(garageCover.getEntityId(),
        static_cast<uint8_t>(garageCover.getState()));
  }

  if (distanceSensor.update()) {
    DistanceT distance = distanceSensor.getValue();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("Distance: "));
    Serial.print(distance);
    Serial.println("cm");
#endif
    lora.sendValue(distanceSensor.getEntityId(),
        static_cast<uint16_t>(distance));
    distanceSensor.setReported();
  }

  if (heightSensor.update()) {
    HeightT height = heightSensor.getValue();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("Height: "));
    Serial.print(height);
    Serial.println("cm");
#endif
    lora.sendValue(heightSensor.getEntityId(),
        static_cast<uint16_t>(height));
    heightSensor.setReported();
  }

  if (carPresenceSensor.update()) {
    bool carHome = carPresenceSensor.getState();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("CarPresence: "));
    Serial.println(carHome);
#endif
    lora.sendValue(carPresenceSensor.getEntityId(),
        static_cast<uint8_t>(carHome));
  }

  if (temperatureSensor.update()) {
    TemperatureT temperature = temperatureSensor.getValue();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("Temperature: "));
    Serial.print(temperature / 10);
    Serial.print('.');
    Serial.print(temperature % 10);
    Serial.println('C');
#endif
    lora.sendValue(temperatureSensor.getEntityId(),
        static_cast<uint16_t>(temperature));
    temperatureSensor.setReported();
  }

  if (humiditySensor.update()) {
    HumidityT humidity = humiditySensor.getValue();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println('%');
#endif
    lora.sendValue(humiditySensor.getEntityId(),
        static_cast<uint16_t>(humidity));
    humiditySensor.setReported();
  }
}

static void printSensors(Stream& stream) {
  stream.print(F("Cov:"));
  stream.print(garageCover.getStateName());
  stream.print(F(" Dis:"));
  stream.print(distanceSensor.getValue());
  stream.print(F("cm Hei:"));
  stream.print(heightSensor.getValue());
  stream.print(F("cm Pre:"));
  stream.print(carPresenceSensor.getState());
  stream.print(F(" Tem:"));
  auto temp = temperatureSensor.getValue();
  stream.print(temp / 10);
  stream.print('.');
  stream.print(temp % 10);
  stream.print(F("C Hum:"));
  stream.print(humiditySensor.getValue());
  stream.println('%');
}

static void handleLoraMessage() {
  String msg = "";
  if (msg == "Cover open" ||
      msg == "Cover close" ||
      msg == "Cover stop" ||
      msg == "Cover toggle") {
    digitalWrite(COVER_RELAY_PIN, HIGH);
    delay(500);
    digitalWrite(COVER_RELAY_PIN, LOW);
  }
}
