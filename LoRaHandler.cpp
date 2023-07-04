/*
 * LoRaHandler.cpp
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

#include "LoRaHandler.h"

#include <LoRa.h>  // LoRa by Sandeep Mistry v0.8.0

#include "Util.h"

int LoRaHandler::begin(OnDiscoveryReqMsgFunc onDiscoveryReqMsgFunc,
                       OnValueReqMsgFunc onValueReqMsgFunc,
                       OnConfigReqMsgFunc onConfigReqMsgFunc,
                       OnConfigSetReqMsgFunc onConfigSetReqMsgFunc,
                       OnServiceReqMsgFunc onServiceReqMsgFunc) {
  mOnDiscoveryReqMsgFunc = onDiscoveryReqMsgFunc;
  mOnValueReqMsgFunc = onValueReqMsgFunc;
  mOnConfigReqMsgFunc = onConfigReqMsgFunc;
  mOnConfigSetReqMsgFunc = onConfigSetReqMsgFunc;
  mOnServiceReqMsgFunc = onServiceReqMsgFunc;

  return LoRa.begin(LORA_FREQUENCY);
}

int LoRaHandler::loraRx() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print(F("Received packet '"));

    // read packet

    LoRaRxMessageT rxMsg;
    rxMsg.header.dst = LoRa.read();
    rxMsg.header.src = LoRa.read();
    rxMsg.header.id = LoRa.read();
    rxMsg.header.flags = LoRa.read();
    rxMsg.header.len = LoRa.read();

    printHeader(&rxMsg.header);
    Serial.print(',');

    uint8_t i = 0;
    while (LoRa.available()) {
      uint8_t rxByte = static_cast<uint8_t>(LoRa.read());
      rxMsg.payload[i] = rxByte;
      Serial.print(rxByte);
    }

    // print RSSI of packet
    Serial.print(F("' with RSSI "));
    rxMsg.rssi = static_cast<int8_t>(LoRa.packetRssi());
    Serial.print(rxMsg.rssi);

    if (sendAckIfRequested(&rxMsg.header) == 1) {
      Serial.print(F(", ACK sent"));
    }
    Serial.println();

    if (parseMsg(rxMsg) == -1) {
      Serial.println(F("Error: Failed to parse msg"));
      return -1;
    }
  }

  return packetSize;
}

int8_t LoRaHandler::parseMsg(LoRaRxMessageT& rxMsg) {
  MsgType msgType = static_cast<MsgType>(rxMsg.header.flags & msgTypeMask);

  switch (msgType) {
    case MsgType::ping_req:
      sendPing(rxMsg.header.src, rxMsg.rssi);
      break;

    case MsgType::discovery_req:
      if (mOnDiscoveryReqMsgFunc) {
        uint8_t entityId = rxMsg.payload[0];
        mOnDiscoveryReqMsgFunc(entityId);
      }
      break;

    case MsgType::value_req:
      if (mOnValueReqMsgFunc) {
        uint8_t entityId = rxMsg.payload[0];
        mOnValueReqMsgFunc(entityId);
      }
      break;

    case MsgType::config_req:
      if (mOnConfigReqMsgFunc) {
        uint8_t entityId = rxMsg.payload[0];
        mOnConfigReqMsgFunc(entityId);
      }
      break;

    case MsgType::configSet_req:
      if (mOnConfigSetReqMsgFunc) {
        LoRaConfigValuePayloadT* payload =
            reinterpret_cast<LoRaConfigValuePayloadT*>(rxMsg.payload);
        mOnConfigSetReqMsgFunc(*payload);
      }
      break;

    case MsgType::service_req:
      if (mOnServiceReqMsgFunc) {
        if (rxMsg.header.len == sizeof(LoRaServiceItemT)) {
          LoRaServiceItemT* item =
              reinterpret_cast<LoRaServiceItemT*>(rxMsg.payload);
          mOnServiceReqMsgFunc(*item);
        }
      }
      break;

    default:
      return -1;
  }
  return 0;
}

void LoRaHandler::printMessage(const LoRaTxMessageT* msg) {
  Serial.print("H:");
  printHeader(&msg->header);
  Serial.print(" P:");
  printPayload(msg->payload, msg->header.len);
  Serial.println();
}

void LoRaHandler::printHeader(const LoRaHeaderT* header) {
  printHex(Serial, header->dst);
  Serial.print(' ');
  printHex(Serial, header->src);
  Serial.print(' ');
  printHex(Serial, header->id);
  Serial.print(' ');
  printHex(Serial, header->flags);
  Serial.print(' ');
  printHex(Serial, header->len);
}

void LoRaHandler::printPayload(const uint8_t* payload, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    printHex(Serial, payload[i]);
    if (i < len - 1) {
      Serial.print(' ');
    }
  }
}

void LoRaHandler::sendMsg(const LoRaTxMessageT* msg) {
#ifdef DEBUG_LORA_MESSAGE
  printMillis(Serial);
  Serial.print(F("LoRaTx: "));
  printMessage(msg);
#endif

  LoRa.beginPacket();
  LoRa.write(reinterpret_cast<uint8_t*>(&msg),
             sizeof(msg->header) + msg->header.len);
  LoRa.endPacket();

  mSeqId++;
}

int LoRaHandler::sendAckIfRequested(const LoRaHeaderT* rxHeader) {
  if (rxHeader->flags & FLAGS_REQ_ACK) {
    LoRaTxMessageT msg;
    msg.header.dst = rxHeader->src;
    msg.header.src = MY_ADDRESS;
    msg.header.id = rxHeader->id;
    msg.header.flags = (rxHeader->flags & msgTypeMask) | FLAGS_ACK;
    msg.header.len = 0;
    sendMsg(&msg);
    return 1;
  }
  return 0;
}

void LoRaHandler::sendPing(const uint8_t toAddr, int8_t rssi) {
  LoRaTxMessageT msg;
  setDefaultHeader(&msg.header);
  msg.header.dst = toAddr;
  msg.header.flags |= MsgType::ping_msg;
  msg.payload[0] = rssi;
  msg.header.len++;
}

void LoRaHandler::setDefaultHeader(LoRaHeaderT* header) {
  header->dst = GATEWAY_ADDRESS;
  header->src = MY_ADDRESS;
  header->id = mSeqId;
  header->flags = FLAGS_REQ_ACK;
  header->len = 0;
}

void LoRaHandler::beginDiscoveryMsg() {
  setDefaultHeader(&mMsgTx.header);
  mMsgTx.header.flags |= MsgType::discovery_msg;
}

void LoRaHandler::endMsg() { sendMsg(&mMsgTx); }

void LoRaHandler::addDiscoveryItem(const uint8_t* buffer, uint8_t length) {
  if (mMsgTx.header.len == 0) {
    memcpy(mMsgTx.payload, buffer, length);
    mMsgTx.header.len += length;
  }
}

void LoRaHandler::beginValueMsg() {
  setDefaultHeader(&mMsgTx.header);
  mMsgTx.header.flags |= MsgType::value_msg;
  reinterpret_cast<LoRaValuePayloadT*>(mMsgTx.payload)->numberOfEntities = 0;
  mMsgTx.header.len++;
}

void LoRaHandler::addValueItem(const uint8_t* buffer, uint8_t length) {
  if (mMsgTx.header.len + length <= LORA_MAX_PAYLOAD_LENGTH - 1) {
    memcpy(&mMsgTx.payload[mMsgTx.header.len], buffer, length);
    mMsgTx.header.len += length;

    LoRaValuePayloadT* payload =
        reinterpret_cast<LoRaValuePayloadT*>(mMsgTx.payload);
    payload->numberOfEntities++;
  }
}

void LoRaHandler::beginConfigsValueMsg() {
  setDefaultHeader(&mMsgTx.header);
  mMsgTx.header.flags |= MsgType::config_msg;
}

void LoRaHandler::addConfigItemValues(const uint8_t* buffer, uint8_t length) {
  if (mMsgTx.header.len == 0) {
    memcpy(mMsgTx.payload, buffer, length);
    mMsgTx.header.len += length;
  }
}
