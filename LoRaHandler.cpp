/*
 * LoRaHandler.cpp
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

#include "LoRaHandler.h"

#include <Arduino.h>
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

  return mLoRa.begin(LORA_FREQUENCY);
}

int LoRaHandler::loraRx() {
  // try to parse packet
  int packetSize = mLoRa.parsePacket();
  if (packetSize) {
    // received a packet
    mStream.print(F("Received packet '"));

    // read packet

    LoRaRxMessageT rxMsg;
    rxMsg.header.dst = mLoRa.read();
    rxMsg.header.src = mLoRa.read();
    rxMsg.header.id = mLoRa.read();
    rxMsg.header.flags = mLoRa.read();
    rxMsg.header.len = mLoRa.read();

    printHeader(&rxMsg.header);
    mStream.print(',');

    uint8_t i = 0;
    while (mLoRa.available()) {
      uint8_t rxByte = static_cast<uint8_t>(mLoRa.read());
      rxMsg.payload[i] = rxByte;
      mStream.print(rxByte);
    }

    // print RSSI of packet
    mStream.print(F("' with RSSI "));
    rxMsg.rssi = static_cast<int8_t>(mLoRa.packetRssi());
    mStream.print(rxMsg.rssi);

    if (sendAckIfRequested(&rxMsg.header) == 1) {
      mStream.print(F(", ACK sent"));
    }
    mStream.println();

    if (parseMsg(rxMsg) == -1) {
      mStream.println(F("Error: Failed to parse msg"));
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
  mStream.print("H:");
  printHeader(&msg->header);
  mStream.print(" P:");
  printPayload(msg->payload, msg->header.len);
  mStream.println();
}

void LoRaHandler::printHeader(const LoRaHeaderT* header) {
  printHex(mStream, header->dst);
  mStream.print(' ');
  printHex(mStream, header->src);
  mStream.print(' ');
  printHex(mStream, header->id);
  mStream.print(' ');
  printHex(mStream, header->flags);
  mStream.print(' ');
  printHex(mStream, header->len);
}

void LoRaHandler::printPayload(const uint8_t* payload, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    printHex(mStream, payload[i]);
    if (i < len - 1) {
      mStream.print(' ');
    }
  }
}

void LoRaHandler::sendMsg(const LoRaTxMessageT* msg) {
#ifdef DEBUG_LORA_MESSAGE
  printMillis(mStream);
  mStream.print(F("LoRaTx: "));
  printMessage(msg);
#endif

  mLoRa.beginPacket();
  mLoRa.write(reinterpret_cast<const uint8_t*>(msg),
              sizeof(msg->header) + msg->header.len);
  mLoRa.endPacket();

  mSeqId++;
}

int LoRaHandler::sendAckIfRequested(const LoRaHeaderT* rxHeader) {
  if (rxHeader->flags & FLAGS_REQ_ACK) {
    LoRaTxMessageT msg;
    msg.header.dst = rxHeader->src;
    msg.header.src = mMyAddress;
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
  header->dst = mGatewayAddress;
  header->src = mMyAddress;
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

void LoRaHandler::beginConfigsValueMsg(uint8_t entityId) {
  setDefaultHeader(&mMsgTx.header);
  mMsgTx.header.flags |= MsgType::config_msg;
  LoRaConfigValuePayloadT* payload =
      reinterpret_cast<LoRaConfigValuePayloadT*>(mMsgTx.payload);
  payload->entityId = entityId;
  payload->numberOfConfigs = 0;
  mMsgTx.header.len += 2;
}

void LoRaHandler::addConfigItemValues(const uint8_t* buffer, uint8_t length) {
  if (mMsgTx.header.len + length <= LORA_MAX_PAYLOAD_LENGTH - 1) {
    memcpy(&mMsgTx.payload[mMsgTx.header.len], buffer, length);
    mMsgTx.header.len += length;

    LoRaConfigValuePayloadT* payload =
        reinterpret_cast<LoRaConfigValuePayloadT*>(mMsgTx.payload);
    payload->numberOfConfigs++;
  }
}
