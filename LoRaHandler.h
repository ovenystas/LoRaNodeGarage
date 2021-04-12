/*
 * LoRaHandler.h
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

/*
 * Discovery message:
 *   Byte 0: Entity Id (0-254, 255 is reserved for broadcast)
 *   Byte 1: Component (Component::Type)
 *   Byte 2: Device Class (BinarySensor::DeviceClass, Cover::DeviceClass or Sensor::DeviceClass)
 *   Byte 3: Unit (Unit::TypeE)
 *   Byte 4: Precision (Number of decimals 0-3)
 *
 * Value message:
 *   Byte 0: Entity Id
 *   Byte 1-n: Value in big endian style
 */
#pragma once

#include <Arduino.h>

#define DEBUG_LORA_MESSAGE

#define MY_ADDRESS 1
#define GATEWAY_ADDRESS 0

#define LORA_HEADER_LENGTH 4
#define LORA_MAX_MESSAGE_LENGTH 51
#define LORA_MAX_PAYLOAD_LENGTH (LORA_MAX_MESSAGE_LENGTH - LORA_HEADER_LENGTH)
#define LORA_DISCOVERY_MSG_LENGTH 5
#define LORA_FREQUENCY 868e6

#define FLAGS_DISCOVERY 0x20
#define FLAGS_REQ_ACK 0x40
#define FLAGS_ACK 0x80

typedef struct {
  uint8_t dst;
  uint8_t src;
  uint8_t id;
  uint8_t flags;
  uint8_t len;
} LoRaHeaderT;

typedef struct {
  LoRaHeaderT header;
  uint8_t payload[LORA_MAX_PAYLOAD_LENGTH];
} LoRaMessageT;

class LoRaHandler {
public:
  int begin();
  int loraRx();
  int loraTx();
  void sendDiscoveryMsg(const uint8_t* buffer);
  void sendValue(uint8_t entityId, uint8_t value);
  void sendValue(uint8_t entityId, int8_t value) {
    sendValue(entityId, static_cast<uint8_t>(value));
  }
  void sendValue(uint8_t entityId, bool state) {
    sendValue(entityId, static_cast<uint8_t>(state));
  }
  void sendValue(uint8_t entityId, uint16_t value);
  void sendValue(uint8_t entityId, int16_t value) {
    sendValue(entityId, static_cast<uint16_t>(value));
  }
  void setDefaultHeader(LoRaHeaderT* header, uint8_t length);

private:
  void printMessage(const LoRaMessageT* msg);
  void printHeader(const LoRaHeaderT* header);
  void printPayload(const uint8_t* payload, uint8_t len);
  void sendHeader(const LoRaHeaderT* header);
  int sendAckIfRequested(const LoRaHeaderT* rx_header);
  int sendMsg(const LoRaMessageT* msg);

  uint8_t mSeqId = { 0 };
};
