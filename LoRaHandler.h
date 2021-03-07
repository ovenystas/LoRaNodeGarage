/*
 * LoRaHandler.h
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <Arduino.h>

#define MY_ADDRESS 10
#define GATEWAY_ADDRESS 2

#define LORA_HEADER_LENGTH 4
#define LORA_MAX_MESSAGE_LENGTH 51
#define LORA_MAX_PAYLOAD_LENGTH (LORA_MAX_MESSAGE_LENGTH - LORA_HEADER_LENGTH)
#define LORA_FREQUENCY 868E6

#define FLAGS_ACK 0x80
#define FLAGS_REQ_ACK 0x40

typedef struct {
  uint8_t dst;
  uint8_t src;
  uint8_t id;
  uint8_t flags;
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
  void handleLoRaMessage();

private:
  void printHeader(LoRaHeaderT header);
  void sendHeader(LoRaHeaderT header);
  int sendAckIfRequested(LoRaHeaderT rx_header);
};
