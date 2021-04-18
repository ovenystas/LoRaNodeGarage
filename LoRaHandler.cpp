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

    if (sendAckIfRequested(&rx_header) == 1) {
      Serial.print(", ACK sent");
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
  if (header->flags < 0x10) {
    Serial.print('0');
  }
  Serial.print(header->flags, HEX);
  Serial.print(',');
  Serial.print(header->len);
}

void LoRaHandler::printPayload(const uint8_t* payload, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    if (payload[i] < 0x10) {
      Serial.print('0');
    }
    Serial.print(payload[i], HEX);
    if (i < len - 1) {
      Serial.print(' ');
    }
  }
}

void LoRaHandler::sendMsg(const LoRaMessageT* msg) {
#ifdef DEBUG_LORA_MESSAGE
  printMillis(Serial);
  Serial.print(F("LoRaTx: "));
  printMessage(msg);
#endif

  LoRa.beginPacket();
  LoRa.write(reinterpret_cast<uint8_t*>(&msg), sizeof(msg->header) + msg->header.len);
  LoRa.endPacket();
}

int LoRaHandler::sendAckIfRequested(const LoRaHeaderT* rxHeader) {
  if (rxHeader->flags & FLAGS_REQ_ACK) {
    LoRaMessageT msg;
    msg.header.dst = rxHeader->src;
    msg.header.src = MY_ADDRESS;
    msg.header.id = rxHeader->id;
    msg.header.flags = FLAGS_ACK;
    msg.payload[0] = '!';
    sendMsg(&msg);
    return 1;
  }
  return 0;
}

void LoRaHandler::setDefaultHeader(LoRaHeaderT* header, uint8_t length) {
  header->dst = GATEWAY_ADDRESS;
  header->src = MY_ADDRESS;
  header->id = mSeqId++;
  header->flags = FLAGS_REQ_ACK;
  header->len = length;
}

void LoRaHandler::beginDiscoveryMsg() {
  setDefaultHeader(&mMsgTx.header, 1);
  mMsgTx.header.flags |= MsgType::discovery_msg;
  reinterpret_cast<LoRaDiscoveryPayloadT*>(mMsgTx.payload)->numberOfEntities = 0;
}

void LoRaHandler::endMsg() {
  sendMsg(&mMsgTx);
}

void LoRaHandler::addDiscoveryItem(const uint8_t* buffer) {
  LoRaDiscoveryPayloadT* payload = reinterpret_cast<LoRaDiscoveryPayloadT*>(mMsgTx.payload);
  if (payload->numberOfEntities < LORA_DISCOVERY_ITEMS_MAX)
  {
    LoRaDiscoveryItemT entity = payload->entity[payload->numberOfEntities];
    memcpy(&entity, buffer, LORA_DISCOVERY_ITEM_LENGTH);
    payload->numberOfEntities++;
    mMsgTx.header.len += LORA_DISCOVERY_ITEM_LENGTH;
  }
}

void LoRaHandler::addDiscoveryItem(const LoRaDiscoveryItemT* item) {
  LoRaDiscoveryPayloadT* payload = reinterpret_cast<LoRaDiscoveryPayloadT*>(mMsgTx.payload);
  if (payload->numberOfEntities < LORA_DISCOVERY_ITEMS_MAX)
  {
    LoRaDiscoveryItemT entity = payload->entity[payload->numberOfEntities];
    entity = *item;
    payload->numberOfEntities++;
    mMsgTx.header.len += LORA_DISCOVERY_ITEM_LENGTH;
  }
}

void LoRaHandler::beginValueMsg() {
  setDefaultHeader(&mMsgTx.header, 1);
  mMsgTx.header.flags |= MsgType::value_msg;
  reinterpret_cast<LoRaValuePayloadT*>(mMsgTx.payload)->numberOfEntities = 0;
}
