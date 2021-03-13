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

#define UPDATE_SENSORS_INTERVAL 1000

static void updateSensors();
static void printSensors(Stream& stream);

DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN);

byte id = 1;
uint32_t nextRunTime = UPDATE_SENSORS_INTERVAL;

GarageCover garageCover = GarageCover(COVER_CLOSED_PIN, COVER_OPEN_PIN);
TemperatureSensor temperatureSensor = TemperatureSensor(dht);
HumiditySensor humiditySensor = HumiditySensor(dht);
DistanceSensor distanceSensor = DistanceSensor(sonar);
HeightSensor heightSensor = HeightSensor(distanceSensor);
PresenceBinarySensor carPresenceSensor = PresenceBinarySensor(heightSensor);
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

  dht.begin();
}

void loop() {
  if (lora.loraRx()) {
    lora.handleLoRaMessage();
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

  //lora.loraTx();
}

static void updateSensors() {
  if (garageCover.update()) {
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("GarageCover: "));
    Serial.println(garageCover.getStateName());
#endif
    //lora.send(state);
  }

  if (distanceSensor.update()) {
    auto distance = distanceSensor.getValue();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("Distance: "));
    Serial.print(distance);
    Serial.println("cm");
#endif
    //lora.send(distance);
    distanceSensor.setReported();
  }

  if (heightSensor.update()) {
    auto height = heightSensor.getValue();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("Height: "));
    Serial.print(height);
    Serial.println("cm");
#endif
    //lora.send(height);
    heightSensor.setReported();
  }

  if (carPresenceSensor.update()) {
    auto carHome = carPresenceSensor.getState();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("CarPresence: "));
    Serial.println(carHome);
#endif
    //lora.send(carHome);
  }

  if (temperatureSensor.update()) {
    auto temperature = temperatureSensor.getValue();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("Temperature: "));
    Serial.print(temperature / 10);
    Serial.print('.');
    Serial.print(temperature % 10);
    Serial.println('C');
#endif
    //lora.send(temperature);
    temperatureSensor.setReported();
  }

  if (humiditySensor.update()) {
    auto humidity = humiditySensor.getValue();
#ifdef DEBUG_SENSOR_REPORT
    printMillis(Serial);
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println('%');
#endif
    //lora.send(humidity);
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
