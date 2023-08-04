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
  if (packetSize <= 0) {
    return 0;
  }

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
  while (mLoRa.available() && i < (LORA_MAX_PAYLOAD_LENGTH - 2)) {
    uint8_t rxByte = static_cast<uint8_t>(mLoRa.read());
    rxMsg.payload[i++] = rxByte;
    mStream.print(rxByte);
  }

  // print RSSI of packet
  mStream.print(F("' with RSSI "));
  rxMsg.rssi = static_cast<int8_t>(mLoRa.packetRssi());
  mStream.print(rxMsg.rssi);

  if (rxMsg.header.dst != mMyAddress) {
    mStream.println(F(", not for me, drop msg."));
    return 0;
  }

  if (isAckRequested(&rxMsg.header)) {
    mStream.println(F(", sending ACK"));
    sendAck(&rxMsg.header);
  } else {
    mStream.println();
  }

  if (parseMsg(rxMsg) == -1) {
    mStream.println(F("Error: Failed to parse msg"));
    return -1;
  }

  return packetSize;
}

int8_t LoRaHandler::parseMsg(const LoRaRxMessageT& rxMsg) {
  const MsgType msgType =
      static_cast<MsgType>(rxMsg.header.flags & msgTypeMask);

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
        ConfigValuePayloadT cfgValPayload;
        cfgValPayload.fromByteArray(rxMsg.payload, rxMsg.header.len);
        mOnConfigSetReqMsgFunc(cfgValPayload);
      }
      break;

    case MsgType::service_req:
      if (mOnServiceReqMsgFunc) {
        if (rxMsg.header.len == sizeof(LoRaServiceItemT)) {
          LoRaServiceItemT serviceItem;
          serviceItem.fromByteArray(rxMsg.payload, rxMsg.header.len);
          mOnServiceReqMsgFunc(serviceItem);
        }
      }
      break;

    default:
      return -1;
  }
  return 0;
}

void LoRaHandler::printMessage(const LoRaTxMessageT* msg) {
  mStream.print(F("H: "));
  printHeader(&msg->header);
  mStream.print(F(" P: "));
  if (msg->header.len == 0) {
    mStream.print("--");
  } else {
    printPayload(msg->payload, msg->header.len);
  }
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

  if (airTime.isLimitReached()) {
    mStream.println(F("AirTime limit reached! Not sending."));
    return;
  }

  (void)mLoRa.beginPacket();
  (void)mLoRa.write(reinterpret_cast<const uint8_t*>(msg),
                    sizeof(msg->header) + msg->header.len);

  uint32_t sendStartTime = millis();
  (void)mLoRa.endPacket();
  uint32_t sendEndTime = millis();
  airTime.update(sendStartTime, sendEndTime);

  mSeqId++;
}

void LoRaHandler::sendAck(const LoRaHeaderT* rxHeader) {
  LoRaTxMessageT msg;
  msg.header.dst = rxHeader->src;
  msg.header.src = mMyAddress;
  msg.header.id = rxHeader->id;
  msg.header.flags = (rxHeader->flags & msgTypeMask) | FLAGS_ACK;
  msg.header.len = 0;
  sendMsg(&msg);
}

void LoRaHandler::sendPing(const uint8_t toAddr, int8_t rssi) {
  LoRaTxMessageT msg;
  setDefaultHeader(&msg.header);
  msg.header.dst = toAddr;
  msg.header.flags |= MsgType::ping_msg;
  msg.payload[0] = rssi;
  msg.header.len++;
  sendMsg(&msg);
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

void LoRaHandler::addDiscoveryItem(const DiscoveryItemT* item) {
  size_t length = item->toByteArray(&mMsgTx.payload[mMsgTx.header.len],
                                    sizeof(mMsgTx.payload) - mMsgTx.header.len);
  mMsgTx.header.len += length;
}

void LoRaHandler::beginValueMsg() {
  setDefaultHeader(&mMsgTx.header);
  mMsgTx.header.flags |= MsgType::value_msg;

  LoRaValuePayloadT payload;
  payload.numberOfEntities = 0;

  mMsgTx.header.len =
      payload.toByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));
}

void LoRaHandler::addValueItem(const ValueItemT* item) {
  LoRaValuePayloadT payload;
  payload.fromByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));

  payload.valueItems[payload.numberOfEntities++] = *item;
  mMsgTx.header.len += item->size();

  payload.toByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));
}

void LoRaHandler::beginConfigsValueMsg(uint8_t entityId) {
  setDefaultHeader(&mMsgTx.header);
  mMsgTx.header.flags |= MsgType::config_msg;

  ConfigValuePayloadT payload;
  payload.entityId = entityId;
  payload.numberOfConfigs = 0;

  payload.toByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));

  mMsgTx.header.len += 2;
}

void LoRaHandler::addConfigItemValues(const ConfigItemValueT* items,
                                      uint8_t length) {
  ConfigValuePayloadT payload;
  payload.fromByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));

  for (uint8_t i = 0; i < length; i++) {
    payload.configValues[i] = items[i];
    payload.numberOfConfigs++;
    mMsgTx.header.len += payload.configValues[0].size();
  }

  payload.toByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));
}
