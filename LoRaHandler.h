/*
 * LoRaHandler.h
 *
 *  Created on: 25 feb. 2021
 *      Author: oveny
 */

/*
 * Ping request:
 *   Empty payload
 *
 * Ping message:
 *   Byte 0: RSSI in ping request message
 *
 * Discovery request:
 *   Byte 0:   EntityId (0-254, 255 is request discovery messages from all entities)
 *
 * Discovery message:
 *   Byte 0:    Entity Id (0-254, 255 is reserved for broadcast)
 *   Byte 1:    Component (Component::Type)
 *   Byte 2:    Device Class (From BinarySensor, Cover or Sensor)
 *   Byte 3:    Unit (Unit::TypeE)
 *   Byte 4:    High nibble: Size (1, 2 or 4 bytes)
 *              Low nibble:  Precision (Number of decimals 0-3)
 *   Byte 5:    Number of config items (0-?)
 *   Byte 6:    Config Id (0-254, 255 is reserved for all)
 *   Byte 7:    Unit (Unit::TypeE)
 *   Byte 8:    High nibble: Size (1, 2 or 4 bytes)
 *              Low nibble:  Precision (Number of decimals 0-3)
 *   Byte 9-11: Repeat byte 6-8 for second config item
 *
 * Value request:
 *   Byte 0:   EntityId (0-254, 255 is request values from all entities)
 *
 * Value message:
 *   Byte 0:       Number of entities (1-k)
 *   Byte 1:       Entity Id
 *   Byte 2-n:     Value in big endian style (1, 2 or 4 bytes)
 *   Byte (n+1)-m: Repeat Byte 2-n for second entity
 *
 * Config request:
 *   Byte 0:   EntityId
 *
 * Config message:
 *   Byte 0:       EntityId
 *   Byte 1:       Number of configs (1-?)
 *   Byte 2:       ConfigId
 *   Byte 3-n:     Value
 *   Byte (n+1)-m: Repeat Byte 3-n for second config
 *
 * Service request
 *   Byte 0: EntityId
 *   Byte 1: Service
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
  uint8_t size :4;
  uint8_t precision :4;
} __attribute__((packed, aligned(1)));

#define LORA_DISCOVERY_ITEM_LENGTH sizeof(LoRaDiscoveryItemT)

struct LoRaConfigItemT {
  uint8_t configId;
  uint8_t unit;
  uint8_t size :4;
  uint8_t precision :4;
} __attribute__((packed, aligned(1)));

#define LORA_CONFIG_ITEM_LENGTH sizeof(LoRaConfigItemT)
#define LORA_CONFIG_ITEMS_MAX ((LORA_MAX_PAYLOAD_LENGTH - LORA_DISCOVERY_ITEM_LENGTH - 1) / LORA_CONFIG_ITEM_LENGTH)

struct LoRaConfigPayloadT {
  uint8_t numberOfConfigs;
  LoRaConfigItemT configItems[LORA_CONFIG_ITEMS_MAX];
} __attribute__((packed, aligned(1)));

struct LoRaDiscoveryPayloadT {
  LoRaDiscoveryItemT entity;
  LoRaConfigPayloadT config;
} __attribute__((packed, aligned(1)));

template<typename T>
struct LoRaValueItemT {
  uint8_t entityId;
  T value;
} __attribute__((packed, aligned(1)));

struct LoRaValuePayloadT {
  uint8_t numberOfEntities;
  uint8_t subPayload[LORA_MAX_PAYLOAD_LENGTH - sizeof(numberOfEntities)];
} __attribute__((packed, aligned(1)));

template<typename T>
struct LoRaConfigValueItemT {
  uint8_t configId;
  T value;
} __attribute__((packed, aligned(1)));

struct LoRaConfigValuePayloadT {
  uint8_t entityId;
  uint8_t numberOfConfigs;
  uint8_t subPayload[LORA_MAX_PAYLOAD_LENGTH - sizeof(entityId) - sizeof(numberOfConfigs)];
} __attribute__((packed, aligned(1)));

struct LoRaServiceItemT {
  uint8_t entityId;
  uint8_t service;
} __attribute__((packed, aligned(1)));

class LoRaHandler {
public:
  enum class MsgType {
    ping_req,
    ping_msg,
    discovery_req,
    discovery_msg,
    value_req,
    value_msg,
    config_req,
    config_msg,
    service_req
  };

  friend inline uint8_t& operator |=(uint8_t& a, const MsgType b) {
    return (a |= static_cast<uint8_t>(b));
  }

  using OnDiscoveryReqMsgFunc = void (*)(uint8_t);
  using OnValueReqMsgFunc = void (*)(uint8_t);
  using OnConfigReqMsgFunc = void (*)(uint8_t);
  using OnServiceReqMsgFunc = void (*)(const LoRaServiceItemT&);

  int begin(OnDiscoveryReqMsgFunc onDiscoveryReqMsgFunc = nullptr,
      OnValueReqMsgFunc onValueReqMsgFunc = nullptr,
      OnConfigReqMsgFunc onConfigReqMsgFunc = nullptr,
      OnServiceReqMsgFunc onServiceReqMsgFunc = nullptr);

  int loraRx();
  int loraTx();

  void beginDiscoveryMsg();
  void addDiscoveryItem(const uint8_t* buffer, uint8_t length);

  void beginValueMsg();

  template<typename T>
  void addValueItem(uint8_t entityId, T value) {
    LoRaValuePayloadT* payload =
        reinterpret_cast<LoRaValuePayloadT*>(mMsgTx.payload);

    if (mMsgTx.header.len + sizeof(LoRaValueItemT<T>)
        >= LORA_MAX_PAYLOAD_LENGTH - 1) {

      LoRaValueItemT<T>* item = reinterpret_cast<LoRaValueItemT<T>*>(
              &payload->subPayload[mMsgTx.header.len]);

      item->entityId = entityId;
      item->value = hton(value);
      payload->numberOfEntities++;
      mMsgTx.header.len += sizeof(LoRaValueItemT<T> );
    }
  }

  void beginConfigsValueMsg(uint8_t entityId);

  template<typename T>
  void addConfigValueItem(uint8_t configId, T value) {
    LoRaConfigValuePayloadT* payload =
        reinterpret_cast<LoRaConfigValuePayloadT*>(mMsgTx.payload);

    if (mMsgTx.header.len + sizeof(LoRaValueItemT<T>)
        >= LORA_MAX_PAYLOAD_LENGTH - 2) {

      LoRaConfigValueItemT<T>* item = reinterpret_cast<LoRaConfigValueItemT<T>*>(
              &payload->subPayload[mMsgTx.header.len]);

      item->configId = configId;
      item->value = hton(value);
      payload->numberOfConfigs++;
      mMsgTx.header.len += sizeof(LoRaConfigValueItemT<T> );
    }
  }

  void endMsg();

  void setDefaultHeader(LoRaHeaderT* header);

private:
  static constexpr uint8_t msgTypeMask = 0x0f;

  void printMessage(const LoRaTxMessageT* msg);
  void printHeader(const LoRaHeaderT* header);
  void printPayload(const uint8_t* payload, uint8_t len);

  int8_t parseMsg(LoRaRxMessageT& rxMsg);

  int sendAckIfRequested(const LoRaHeaderT* rx_header);
  void sendMsg(const LoRaTxMessageT* msg);
  void sendPing(const uint8_t toAddr, int8_t rssi);

  uint8_t mSeqId = { 0 };
  LoRaTxMessageT mMsgTx;

  OnDiscoveryReqMsgFunc mOnDiscoveryReqMsgFunc = { nullptr };
  OnValueReqMsgFunc mOnValueReqMsgFunc = { nullptr };
  OnConfigReqMsgFunc mOnConfigReqMsgFunc = { nullptr };
  OnServiceReqMsgFunc mOnServiceReqMsgFunc = { nullptr };
};
