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

static constexpr uint8_t msgTypeMask = 0x0f;

int16_t LoRaHandler::begin(OnDiscoveryReqMsgFunc onDiscoveryReqMsgFunc,
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

int16_t LoRaHandler::loraRx() {
  // try to parse packet
  int16_t packetSize = mLoRa.parsePacket();
  if (packetSize <= 0) {
    return 0;
  }

  // received a packet
  Serial.print(F("Received packet '"));

  // read packet
  uint8_t buf[LORA_MAX_MESSAGE_LENGTH];
  for (int16_t i = 0; i < packetSize; i++) {
    auto b = mLoRa.read();
    if (b < 0) {
      return -1;
    }
    buf[i] = b;
  }

  // Decrypt packet
  if (mCipher) {
    mCipher->decrypt(&buf[3], &buf[3], packetSize - 3);
  }

  // Parse header
  LoRaRxMessageT rxMsg;
  rxMsg.header.fromByteArray(buf);

  // Print message as HEX
  printArray(Serial, buf, LORA_HEADER_LENGTH, HEX);
  Serial.print(',');
  printArray(Serial, &buf[LORA_HEADER_LENGTH], rxMsg.header.len, HEX);

  // print RSSI of packet
  Serial.print(F(" with RSSI "));
  rxMsg.rssi = static_cast<int16_t>(mLoRa.packetRssi());
  Serial.print(rxMsg.rssi);

  // Check if it is addressed to me
  if (rxMsg.header.dst != mMyAddress) {
    Serial.println(F(", not for me, drop msg."));
    return 0;
  }

  // Check frame counter
  if (rxMsg.header.frCnt != mDownlinkFrCnt) {
    Serial.print(F(", unexpected frame counter, received "));
    Serial.print(rxMsg.header.frCnt);
    Serial.print(F(" != expected "));
    Serial.print(mDownlinkFrCnt);
    return -1;
  }

  // Send ack if requested
  if (isAckRequested(&rxMsg.header)) {
    Serial.println(F("', sending ACK"));
    sendAck(&rxMsg.header);
  } else {
    Serial.println('\'');
  }

  // Parse message
  if (parseMsg(rxMsg, &buf[LORA_HEADER_LENGTH]) == -1) {
    Serial.println(F("Error: Failed to parse msg"));
    return -1;
  }

  mDownlinkFrCnt++;

  return packetSize;
}

int8_t LoRaHandler::parseMsg(const LoRaRxMessageT& rxMsg, uint8_t* payload) {
  switch (rxMsg.header.flags.msgType) {
    case LoRaMsgType::ping_req:
      sendPing(rxMsg.header.src, rxMsg.rssi);
      break;

    case LoRaMsgType::discovery_req:
      if (mOnDiscoveryReqMsgFunc) {
        uint8_t entityId = payload[0];
        mOnDiscoveryReqMsgFunc(entityId);
      }
      break;

    case LoRaMsgType::value_req:
      if (mOnValueReqMsgFunc) {
        uint8_t entityId = payload[0];
        mOnValueReqMsgFunc(entityId);
      }
      break;

    case LoRaMsgType::config_req:
      if (mOnConfigReqMsgFunc) {
        uint8_t entityId = payload[0];
        mOnConfigReqMsgFunc(entityId);
      }
      break;

    case LoRaMsgType::configSet_req:
      if (mOnConfigSetReqMsgFunc) {
        ConfigValuePayloadT cfgValPayload;
        cfgValPayload.fromByteArray(payload, rxMsg.header.len);
        mOnConfigSetReqMsgFunc(cfgValPayload);
      }
      break;

    case LoRaMsgType::service_req:
      if (mOnServiceReqMsgFunc) {
        if (rxMsg.header.len == sizeof(LoRaServiceItemT)) {
          LoRaServiceItemT serviceItem;
          serviceItem.fromByteArray(payload, rxMsg.header.len);
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
  Serial.print(F("H: "));
  uint8_t buf[LORA_HEADER_LENGTH];
  msg->header.toByteArray(buf);
  printArray(Serial, buf, LORA_HEADER_LENGTH, HEX);

  Serial.print(F(" P: "));
  if (msg->header.len == 0) {
    Serial.print("--");
  } else {
    printArray(Serial, msg->payload, msg->header.len, HEX);
  }
  Serial.println();
}

void LoRaHandler::sendMsg(const LoRaTxMessageT* msg) {
#ifdef DEBUG_LORA_MESSAGE
  printMillis(Serial);
  Serial.print(F("LoRaTx: "));
  printMessage(msg);
#endif

  if (airTime.isLimitReached()) {
    Serial.println(F("AirTime limit reached! Not sending."));
    return;
  }

  uint8_t buf[LORA_MAX_MESSAGE_LENGTH];
  uint8_t n = msg->header.toByteArray(buf);
  memcpy(&buf[n], msg->payload, msg->header.len);
  n += msg->header.len;

  if (mCipher) {
    mCipher->encrypt(&buf[3], &buf[3], n - 3);
  }

  (void)mLoRa.beginPacket();
  (void)mLoRa.write(buf, n);

  uint32_t sendStartTime = millis();
  (void)mLoRa.endPacket();
  uint32_t sendEndTime = millis();

  airTime.update(sendStartTime, sendEndTime);

  printMillis(Serial);
  Serial.print(F("AirTime: "));
  Serial.print(airTime.getTime_ms());
  Serial.print(F(" ms, "));
  Serial.print(airTime.getTime_ppm());
  Serial.println(F(" ppm"));

  mUplinkFrCnt++;
}

void LoRaHandler::sendAck(const LoRaHeaderT* rxHeader) {
  LoRaTxMessageT msg;
  msg.header.dst = rxHeader->src;
  msg.header.src = mMyAddress;
  msg.header.frCnt = static_cast<uint16_t>(mUplinkFrCnt & 0Xff);
  msg.header.flags.fromByte(0);
  msg.header.flags.ack_response = true;
  msg.header.flags.msgType = rxHeader->flags.msgType;
  msg.header.len = 0;
  sendMsg(&msg);
}

void LoRaHandler::sendPing(const uint8_t toAddr, int8_t rssi) {
  LoRaTxMessageT msg;
  setDefaultHeader(msg.header);
  msg.header.dst = toAddr;
  msg.header.flags.msgType = LoRaMsgType::ping_msg;
  msg.payload[0] = rssi;
  msg.header.len++;
  sendMsg(&msg);
}

void LoRaHandler::setDefaultHeader(LoRaHeaderT& header) {
  header.dst = mGatewayAddress;
  header.src = mMyAddress;
  header.frCnt = static_cast<uint16_t>(mUplinkFrCnt & 0xFFFF);
  header.flags.fromByte(0);
  header.flags.ack_request = true;
  header.len = 0;
}

void LoRaHandler::beginDiscoveryMsg() {
  setDefaultHeader(mMsgTx.header);
  mMsgTx.header.flags.msgType = LoRaMsgType::discovery_msg;
}

void LoRaHandler::endMsg() { sendMsg(&mMsgTx); }

void LoRaHandler::addDiscoveryItem(const DiscoveryItemT* item) {
  size_t length = item->toByteArray(&mMsgTx.payload[mMsgTx.header.len],
                                    sizeof(mMsgTx.payload) - mMsgTx.header.len);
  mMsgTx.header.len += length;
}

void LoRaHandler::beginValueMsg() {
  setDefaultHeader(mMsgTx.header);
  mMsgTx.header.flags.msgType = LoRaMsgType::value_msg;

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
  setDefaultHeader(mMsgTx.header);
  mMsgTx.header.flags.msgType = LoRaMsgType::config_msg;

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
