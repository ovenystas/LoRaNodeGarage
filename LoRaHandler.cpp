/*
 * LoRaHandler.cpp
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

#include <LoRa.h>

#include "LoRaHandler.h"
#include "Util.h"

int LoRaHandler::begin() {
  return LoRa.begin(LORA_FREQUENCY);
}

int LoRaHandler::loraRx() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet

    LoRaHeaderT rx_header;
    rx_header.dst = LoRa.read();
    rx_header.src = LoRa.read();
    rx_header.id = LoRa.read();
    rx_header.flags = LoRa.read();

    printHeader(&rx_header);
    Serial.print(',');

    while (LoRa.available()) {
      Serial.print(static_cast<char>(LoRa.read()));
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.print(LoRa.packetRssi());

    int ackSentStatus = sendAckIfRequested(&rx_header);
    if (ackSentStatus == 1) {
      Serial.print(", ACK sent");
    }
    else if (ackSentStatus == 0) {
      Serial.print(", ACK send failed");
    }
    else {
      // Do nothing
    }
    Serial.println();
  }
  return packetSize;
}

void LoRaHandler::printMessage(const LoRaMessageT* msg) {
  Serial.print("H:");
  printHeader(&msg->header);
  Serial.print(" P:");
  printPayload(msg->payload, msg->header.len);
  Serial.println();
}

void LoRaHandler::printHeader(const LoRaHeaderT* header) {
  Serial.print(header->dst);
  Serial.print(',');
  Serial.print(header->src);
  Serial.print(',');
  Serial.print(header->id);
  Serial.print(",0x");
  Serial.print(header->flags, HEX);
  Serial.print(',');
  Serial.print(header->len);
}

void LoRaHandler::printPayload(const uint8_t* payload, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    Serial.print(payload[i], HEX);
    Serial.print(' ');
  }
}

void LoRaHandler::sendHeader(const LoRaHeaderT* header) {
  LoRa.write(header->dst);
  LoRa.write(header->src);
  LoRa.write(header->id);
  LoRa.write(header->flags);
  LoRa.write(header->len);
}

int LoRaHandler::sendMsg(const LoRaMessageT* msg) {
#ifdef DEBUG_LORA_MESSAGE
  printMillis(Serial);
  Serial.print(F("LoRaTx: "));
  printMessage(msg);
#endif

  LoRa.beginPacket();
  sendHeader(&msg->header);
  LoRa.write(msg->payload, msg->header.len);
  return LoRa.endPacket();
}

int LoRaHandler::sendAckIfRequested(const LoRaHeaderT* rx_header) {
  if (rx_header->flags & FLAGS_REQ_ACK) {
    LoRaHeaderT tx_header;
    tx_header.dst = rx_header->src;
    tx_header.src = MY_ADDRESS;
    tx_header.id = rx_header->id;
    tx_header.flags = FLAGS_ACK;

    LoRa.beginPacket();
    sendHeader(&tx_header);
    LoRa.print('!');

    return LoRa.endPacket();
  }
  return 2;
}

void LoRaHandler::setDefaultHeader(LoRaHeaderT* header, uint8_t length) {
  header->dst = GATEWAY_ADDRESS;
  header->src = MY_ADDRESS;
  header->id = mSeqId++;
  header->flags = FLAGS_REQ_ACK;
  header->len = length;
}

void LoRaHandler::sendDiscoveryMsg(const uint8_t* buffer) {
  LoRaMessageT msg;
  setDefaultHeader(&msg.header, LORA_DISCOVERY_MSG_LENGTH);
  msg.header.flags |= FLAGS_DISCOVERY;
  memcpy(msg.payload, buffer, LORA_DISCOVERY_MSG_LENGTH);
  sendMsg(&msg);
}

void LoRaHandler::sendValue(uint8_t entityId, uint8_t value) {
  LoRaMessageT msg;
  setDefaultHeader(&msg.header, 1 + sizeof(value));
  msg.payload[0] = entityId;
  msg.payload[1] = value;
  sendMsg(&msg);
}

void LoRaHandler::sendValue(uint8_t entityId, uint16_t value) {
  LoRaMessageT msg;
  setDefaultHeader(&msg.header, 1 + sizeof(value));
  msg.payload[0] = entityId;
  msg.payload[1] = highByte(value);
  msg.payload[2] = lowByte(value);
  sendMsg(&msg);
}
