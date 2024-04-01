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
#ifdef DEBUG_LORA_MESSAGE
  printMillis(Serial);
  Serial.print(F("LoRaRx: '"));
#endif

  // read packet
  uint8_t buf[LORA_MAX_MESSAGE_LENGTH];
  for (int16_t i = 0; i < packetSize; i++) {
    auto b = mLoRa.read();
    if (b < 0) {
      return -1;
    }
    buf[i] = b;
  }

  uint8_t payload_length = packetSize - LORA_HEADER_LENGTH;

  // Decrypt packet
  if (mCipher) {
    mCipher->decrypt(&buf[LORA_HEADER_LENGTH - 1], &buf[LORA_HEADER_LENGTH - 1],
                     payload_length + 1);
  }

  // Parse header
  LoRaRxMessageT rxMsg;
  rxMsg.header.fromByteArray(buf);
  rxMsg.payload_length = payload_length;

#ifdef DEBUG_LORA_MESSAGE
  // Print message as HEX
  printArray(Serial, buf, LORA_HEADER_LENGTH, HEX);
  Serial.print(", ");
  printArray(Serial, &buf[LORA_HEADER_LENGTH], rxMsg.payload_length, HEX);

  // print RSSI of packet
  Serial.print(F("' with RSSI "));
#endif
  rxMsg.rssi = static_cast<int16_t>(mLoRa.packetRssi());
#ifdef DEBUG_LORA_MESSAGE
  Serial.print(rxMsg.rssi);
#endif

  // Check if it is addressed to me
  if (rxMsg.header.dst != mMyAddress) {
#ifdef DEBUG_LORA_MESSAGE
    Serial.println(F(", not for me, drop msg."));
#endif
    return 0;
  }

  // Check msg id
  // mMsgIdDown += 1;
  // if (rxMsg.header.id != mMsgIdDown) {
  //   Serial.print(F(", unexpected msg id "));
  //   Serial.print(rxMsg.header.id);
  //   Serial.print(F(" != "));
  //   Serial.println(mMsgIdDown);
  //   return -1;
  // }

  // Send ack if requested
  if (isAckRequest(&rxMsg.header)) {
#ifdef DEBUG_LORA_MESSAGE
    Serial.println(F(", sending ACK"));
#endif
    sendAck(&rxMsg.header);
  }

  if (isAckResponse(&rxMsg.header)) {
#ifdef DEBUG_LORA_MESSAGE
    Serial.println(F(", ACK response"));
#endif
    return 0;
  }

  // Parse message
  if (parseMsg(rxMsg, &buf[LORA_HEADER_LENGTH]) == -1) {
#ifdef DEBUG_LORA_MESSAGE
    Serial.println(F(", Error: Failed to parse msg"));
#endif
    return -1;
  }

  // Serial.println();

  return packetSize;
}

int8_t LoRaHandler::parseMsg(const LoRaRxMessageT& rxMsg, uint8_t* payload) {
  // printMillis(Serial);
  // Serial.print(F("Parsing msg type="));
  // Serial.println(static_cast<uint8_t>(rxMsg.header.flags.msgType));

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
        mOnValueReqMsgFunc();
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
        cfgValPayload.fromByteArray(payload, rxMsg.payload_length);
        mOnConfigSetReqMsgFunc(cfgValPayload);
      }
      break;

      // s 1 0 0
      // DEBUG: Sending b'\x00\x00' to 1 (lora.py:197)
      // send b'\x00\x00' to 1

      // [66926] LoRaRx: '01 00 0D 49, 00 00 70 74 88 77' with RSSI -47, sending
      // ACK [66930] LoRaTx: H: 00 01 0D 89 P: (ACK) [67010] AirTime: 897 ms,
      // 249 ppm [67012] Parsing msg type=9 DBG srvReq: wrong length, len=6,
      // exp=2

    case LoRaMsgType::service_req:
      if (mOnServiceReqMsgFunc) {
        LoRaServiceItemT serviceItem;
        serviceItem.fromByteArray(payload, rxMsg.payload_length);
        mOnServiceReqMsgFunc(serviceItem);
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
  if (msg->payload_length == 0) {
    Serial.print(F("--"));
  } else if (msg->header.flags.ack_response and msg->payload_length == '!') {
    Serial.print(F("(ACK)"));
  } else {
    printArray(Serial, msg->payload, msg->payload_length, HEX);
  }
  Serial.println();
}

void LoRaHandler::sendMsg(const LoRaTxMessageT* msg) {
#ifdef DEBUG_LORA_MESSAGE
  printMillis(Serial);
  Serial.print(F("LoRaTx: "));
  printMessage(msg);
#endif

  if (mAirTime.isLimitReached()) {
    Serial.println(F("AirTime limit reached! Not sending."));
    return;
  }

  uint8_t buf[LORA_MAX_MESSAGE_LENGTH];
  uint8_t n = msg->header.toByteArray(buf);
  memcpy(&buf[n], msg->payload, msg->payload_length);
  n += msg->payload_length;

  if (mCipher) {
    mCipher->encrypt(&buf[LORA_HEADER_LENGTH - 1], &buf[LORA_HEADER_LENGTH - 1],
                     n - LORA_HEADER_LENGTH + 1);
  }

  (void)mLoRa.beginPacket();
  (void)mLoRa.write(buf, n);

  uint32_t sendStartTime = millis();
  (void)mLoRa.endPacket();
  uint32_t sendEndTime = millis();

  mAirTime.update(sendStartTime, sendEndTime);

  printMillis(Serial);
  Serial.print(F("AirTime: "));
  Serial.print(mAirTime.getTime_ms());
  Serial.print(F(" ms, "));
  Serial.print(mAirTime.getTime_ppm());
  Serial.println(F(" ppm"));
}

void LoRaHandler::sendAck(const LoRaHeaderT* rxHeader) {
  LoRaTxMessageT msg;
  msg.header.dst = rxHeader->src;
  msg.header.src = mMyAddress;
  msg.header.id = rxHeader->id;
  msg.header.flags.fromByte(0);
  msg.header.flags.ack_response = true;
  msg.header.flags.msgType = rxHeader->flags.msgType;
  msg.payload_length = 1;
  msg.payload[0] = '!';  // ACK is special and has an ! as payload.
  sendMsg(&msg);
}

void LoRaHandler::sendPing(const uint8_t toAddr, int16_t rssi) {
  LoRaTxMessageT msg;
  setDefaultHeader(msg.header);
  msg.payload_length = 0;
  msg.header.dst = toAddr;
  msg.header.id = ++mMsgIdUp;
  msg.header.flags.msgType = LoRaMsgType::ping_msg;
  int16_t* pValue_n = reinterpret_cast<int16_t*>(&msg.payload[0]);
  *pValue_n = hton(rssi);
  msg.payload_length += 2;
  sendMsg(&msg);
}

void LoRaHandler::setDefaultHeader(LoRaHeaderT& header) {
  header.dst = mGatewayAddress;
  header.src = mMyAddress;
  header.id = 0;
  header.flags.fromByte(0);
  header.flags.ack_request = true;
}

void LoRaHandler::beginDiscoveryMsg() {
  setDefaultHeader(mMsgTx.header);
  mMsgTx.payload_length = 0;
  mMsgTx.header.flags.msgType = LoRaMsgType::discovery_msg;
}

void LoRaHandler::endMsg() {
  mMsgTx.header.id = ++mMsgIdUp;
  sendMsg(&mMsgTx);
}

void LoRaHandler::addDiscoveryItem(const DiscoveryItemT* item) {
  size_t length =
      item->toByteArray(&mMsgTx.payload[mMsgTx.payload_length],
                        sizeof(mMsgTx.payload) - mMsgTx.payload_length);
  if (length == 0) {
    Serial.println(F("Err: DiscItemLen=0"));
  }
  mMsgTx.payload_length += length;
}

void LoRaHandler::beginValueMsg() {
  setDefaultHeader(mMsgTx.header);
  mMsgTx.payload_length = 0;
  mMsgTx.header.flags.msgType = LoRaMsgType::value_msg;

  LoRaValuePayloadT payload;
  payload.numberOfEntities = 0;

  mMsgTx.payload_length =
      payload.toByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));
}

void LoRaHandler::addValueItem(const ValueItemT* item) {
  LoRaValuePayloadT payload;
  payload.fromByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));

  payload.valueItems[payload.numberOfEntities++] = *item;
  mMsgTx.payload_length += item->size();

  payload.toByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));
}

void LoRaHandler::beginConfigsValueMsg(uint8_t entityId) {
  setDefaultHeader(mMsgTx.header);
  mMsgTx.payload_length = 0;
  mMsgTx.header.flags.msgType = LoRaMsgType::config_msg;

  ConfigValuePayloadT payload;
  payload.entityId = entityId;
  payload.numberOfConfigs = 0;

  payload.toByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));

  mMsgTx.payload_length += 2;
}

void LoRaHandler::addConfigItemValues(const ConfigItemValueT* items,
                                      uint8_t length) {
  ConfigValuePayloadT payload;
  payload.fromByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));

  for (uint8_t i = 0; i < length; i++) {
    payload.configValues[i] = items[i];
    payload.numberOfConfigs++;
    mMsgTx.payload_length += payload.configValues[0].size();
  }

  payload.toByteArray(mMsgTx.payload, sizeof(mMsgTx.payload));
}
