/*
 * LoRaHandler.h
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

/*
 * Discovery message:
 *   Byte 0: Entity Id
 *   Byte 1: Component
 *   Byte 2: Device Class
 *   Byte 3: Unit
 *
 * Value message:
 *   Byte 0: Entity Id
 *   Byte 1-n: Value in big endian style
 */
#pragma once

#include <Arduino.h>

#define MY_ADDRESS 1
#define GATEWAY_ADDRESS 0

#define LORA_HEADER_LENGTH 4
#define LORA_MAX_MESSAGE_LENGTH 51
#define LORA_MAX_PAYLOAD_LENGTH (LORA_MAX_MESSAGE_LENGTH - LORA_HEADER_LENGTH)
#define LORA_DISCOVERY_MSG_LENGTH 4
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
  void sendValue(uint8_t entityId, uint16_t value);
  void setDefaultHeader(LoRaHeaderT* header, uint8_t length);

private:
  void printHeader(const LoRaHeaderT* header);
  void sendHeader(const LoRaHeaderT* header);
  int sendAckIfRequested(const LoRaHeaderT* rx_header);
  int sendMsg(const LoRaMessageT* msg);

  uint8_t mSeqId = { 0 };
};
