/*
 * LoRaHandler.h
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

/*
 * Ping message:
 *   Byte 0: RSSI in ping request message
 *
 * Discovery message:
 *   Byte 0:    Number of entities (1-9)
 *   Byte 1:    Entity Id (0-254, 255 is reserved for broadcast)
 *   Byte 2:    Component (Component::Type)
 *   Byte 3:    Device Class (From BinarySensor, Cover or Sensor)
 *   Byte 4:    Unit (Unit::TypeE)
 *   Byte 5:    High nibble: Size (1, 2 or 4 bytes)
 *              Low nibble:  Precision (Number of decimals 0-3)
 *   Byte 6-10: Repeat Byte 1-5 for second entity
 *   Byte 11-n: Repeat Byte 1-5 for third to n entity
 *
 * Value message:
 *   Byte 0:       Number of entities (1-k)
 *   Byte 1:       Entity Id
 *   Byte 2-n:     Value in big endian style (1,2 or 4 bytes)
 *   Byte (n+1)-m: Repeat Byte 2-n for second entity
 *
 * Config message:
 *   Byte 0: ?
 *
 */
#pragma once

#include <Arduino.h>

#include "util.h"

#define DEBUG_LORA_MESSAGE

#define MY_ADDRESS 1
#define GATEWAY_ADDRESS 0

#define LORA_FREQUENCY 868e6
#define LORA_MAX_MESSAGE_LENGTH 51

#define FLAGS_REQ_ACK 0x40
#define FLAGS_ACK 0x80

struct LoRaHeaderT {
  uint8_t dst;
  uint8_t src;
  uint8_t id;
  uint8_t flags;
  uint8_t len;
} __attribute__((packed, aligned(1)));

#define LORA_HEADER_LENGTH sizeof(LoRaHeaderT)
#define LORA_MAX_PAYLOAD_LENGTH (LORA_MAX_MESSAGE_LENGTH - LORA_HEADER_LENGTH)

struct LoRaTxMessageT {
  LoRaHeaderT header;
  uint8_t payload[LORA_MAX_PAYLOAD_LENGTH];
} __attribute__((packed, aligned(1)));

struct LoRaRxMessageT {
  LoRaHeaderT header;
  int8_t rssi;
  uint8_t payload[LORA_MAX_PAYLOAD_LENGTH];
} __attribute__((packed, aligned(1)));

struct LoRaDiscoveryItemT {
  uint8_t entityId;
  uint8_t component;
  uint8_t deviceClass;
  uint8_t unit;
  uint8_t size:4;
  uint8_t precision:4;
} __attribute__((packed, aligned(1)));

#define LORA_DISCOVERY_ITEM_LENGTH sizeof(LoRaDiscoveryItemT)
#define LORA_DISCOVERY_ITEMS_MAX ((LORA_MAX_PAYLOAD_LENGTH - 1) / LORA_DISCOVERY_ITEM_LENGTH)

struct LoRaDiscoveryPayloadT {
  uint8_t numberOfEntities;
  LoRaDiscoveryItemT entity[LORA_DISCOVERY_ITEMS_MAX];
} __attribute__((packed, aligned(1)));

template <typename T>
struct LoRaValueItemT {
  uint8_t entityId;
  T value;
} __attribute__((packed, aligned(1)));

struct LoRaValuePayloadT {
  uint8_t numberOfEntities;
  uint8_t subPayload[LORA_MAX_PAYLOAD_LENGTH - sizeof(numberOfEntities)];;
} __attribute__((packed, aligned(1)));

class LoRaHandler {
public:
  enum class MsgType : uint8_t {
    ping_req = 0,
    ping_msg,
    discovery_req,
    discovery_msg,
    value_req,
    value_msg,
    config_req,
    config_msg,
    service_req
  };

  const uint8_t msgTypeMask = 0x0f;

  friend inline uint8_t& operator |=(uint8_t& a, const MsgType b) {
    return (a |= static_cast<uint8_t>(b));
  }

  typedef void (*OnDiscoveryReqMsgFunc)(void);
  typedef void (*OnValueReqMsgFunc)(void);
  typedef void (*OnConfigReqMsgFunc)(void);
  typedef void (*OnServiceReqMsgFunc)(void);

  int begin();
  int begin(OnDiscoveryReqMsgFunc onDiscoveryReqMsgFunc,
      OnValueReqMsgFunc onValueReqMsgFunc,
      OnConfigReqMsgFunc onConfigReqMsgFunc,
      OnServiceReqMsgFunc onServiceReqMsgFunc);

  int loraRx();
  int loraTx();

  int8_t parseMsg(LoRaRxMessageT& rxMsg);

  void beginDiscoveryMsg();
  void endMsg();
  void addDiscoveryItem(const uint8_t* buffer);
  void addDiscoveryItem(const LoRaDiscoveryItemT* item);

  void beginValueMsg();

  template <typename T>
  void addValueItem(const uint8_t entityId, const T value) {
    LoRaValuePayloadT* payload = reinterpret_cast<LoRaValuePayloadT*>(mMsgTx.payload);
    if (mMsgTx.header.len +  sizeof(LoRaValueItemT<T>) >= LORA_MAX_PAYLOAD_LENGTH - 1) {
      LoRaValueItemT<T>* entity = reinterpret_cast<LoRaValueItemT<T>*>(&payload->subPayload[1 + mMsgTx.header.len]);
      entity->entityId = entityId;
      entity->value = hton(value);
      payload->numberOfEntities++;
      mMsgTx.header.len += sizeof(LoRaValueItemT<T>);
    }
  }

  void setDefaultHeader(LoRaHeaderT* header, uint8_t length);

private:
  void printMessage(const LoRaTxMessageT* msg);
  void printHeader(const LoRaHeaderT* header);
  void printPayload(const uint8_t* payload, uint8_t len);

  int sendAckIfRequested(const LoRaHeaderT* rx_header);
  void sendMsg(const LoRaTxMessageT* msg);
  void sendPing(const LoRaHeaderT* rxHeader, int8_t rssi);

  uint8_t mSeqId = { 0 };
  LoRaTxMessageT mMsgTx;

  OnDiscoveryReqMsgFunc mOnDiscoveryReqMsgFunc = { nullptr };
  OnValueReqMsgFunc mOnValueReqMsgFunc = { nullptr };
  OnConfigReqMsgFunc mOnConfigReqMsgFunc = { nullptr };
  OnServiceReqMsgFunc mOnServiceReqMsgFunc = { nullptr };
};
