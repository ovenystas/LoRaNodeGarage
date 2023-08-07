// clang-format off
/*
 * Header:
 *   Byte 0:  dst - Destination address
 *   Byte 1:  src - Source address
 *   Byte 2:  id  - Message sequence id
 *   Byte 3:  flags
 *     Bit7: Acknowledge response
 *     Bit6: Acknowledge request
 *     Bit5: Reserved
 *     Bit4: Reserved
 *     Bit3-0: Message type
 *       0: ping_req
 *       1: ping_msg
 *       2: discovery_req
 *       3: discovery_msg
 *       4: value_req
 *       5: value_msg
 *       6: config_req
 *       7: config_msg
 *       8: configSet_req
 *       9: service_req
 *       10-15: Reserved
 *   Byte 4:  len - Length of payload in bytes
 *
 * Payloads:
 *   Ping request:
 *     Empty payload
 *
 *   Ping message:
 *     Byte 0: RSSI in ping request message
 *
 *   Discovery request:
 *     Byte 0:   EntityId (0-254, 255 is request discovery messages from all
 *               entities)
 *
 *   Discovery message:
 *     Byte 0:    Entity Id (0-254, 255 is reserved for broadcast)
 *     Byte 1:    BaseComponent Type - 0=BinarySensor, 1=Sensor, 2=Cover)
 *     Byte 2:    Device Class - BinarySensorDeviceClass, SensorDeviceClass or CoverDeviceClass
 *     Byte 3:    Unit (Unit::TypeE)
 *     Byte 4:    Bit 4: 0=UnSigned, 1=Signed
 *                Bit 3-2: Size (0=1 byte, 1=2 bytes or 2=4 bytes)
 *                Bit 1-0:  Precision (Number of decimals 0-3)
 *     Byte 5:    Number of config items (0-?)
 *     Byte 6:    Config Id (0-254, 255 is reserved for all)
 *     Byte 7:    Unit (Unit::TypeE)
 *     Byte 8:    Bit 4: 0=UnSigned, 1=Signed
 *                Bit 3-2: Size (0=1 byte, 1=2 bytes or 2=4 bytes)
 *                Bit 1-0:  Precision (Number of decimals 0-3)
 *     Byte 9-m:  Repeat byte 6-8 for more config items until payload is full
 *
 *   Value request:
 *     Byte 0:    EntityId (0-254, 255 is request values from all entities)
 *
 *   Value message:
 *     Byte 0:    Number of entities
 *     Byte 1:    Entity Id (0-254, 255 is reserved for broadcast)
 *     Byte 2-5:  Value in big endian (4 bytes)
 *     Byte 6-m:  Repeat byte 1-5 for more entities until payload is full
 *
 *   Config request:
 *     Byte 0:    EntityId (0-254, 255 is request configs from all entities)
 *
 *   Config message:
 *     Byte 0:    EntityId (0-254, 255 is reserved for broadcast)
 *     Byte 1:    Number of configs (1-?)
 *     Byte 2:    ConfigId
 *     Byte 3-6:  Value in big endian (4 bytes)
 *     Byte 7-m:  Repeat Byte 2-6 for more configs until payload is full
 *
 *   ConfigSet request:
 *     Same as Config message.
 *
 *   Service request
 *     Byte 0:    EntityId
 *     Byte 1:    Service
 */
// clang-format on
#pragma once

#include <Arduino.h>
#include <LoRa.h>
#include <Stream.h>

#include "AirTime.h"
#include "Types.h"
#include "Util.h"

#define DEBUG_LORA_MESSAGE

#define LORA_FREQUENCY 868e6
#define LORA_MAX_MESSAGE_LENGTH 51

#define FLAGS_REQ_ACK 0x40
#define FLAGS_ACK 0x80

#define AIRTIME_LIMIT_PERCENT 1
#define AIRTIME_LIMIT_PPM (AIRTIME_LIMIT_PERCENT * 10000)

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

struct LoRaValuePayloadT {
  uint8_t numberOfEntities;
  ValueItemT valueItems[(LORA_MAX_PAYLOAD_LENGTH - sizeof(numberOfEntities)) /
                        ValueItemT::size()];

  size_t size() const {
    return sizeof(numberOfEntities) + ValueItemT::size() * numberOfEntities;
  }

  size_t toByteArray(uint8_t* buf, size_t length) const {
    if (length < size()) {
      return 0;
    }

    buf[0] = numberOfEntities;

    size_t n = 1;
    for (uint8_t i = 0; i < numberOfEntities; i++) {
      n += valueItems[i].toByteArray(&buf[n], length - n);
    }

    return n;
  }

  uint8_t fromByteArray(const uint8_t* buf, size_t length) {
    if (length < 1 + buf[0] * ValueItemT::size()) {
      return 0;
    }

    numberOfEntities = buf[0];

    size_t n = 1;
    for (uint8_t i = 0; i < numberOfEntities; i++) {
      n += valueItems[i].fromByteArray(&buf[n], length - n);
    }
    return n;
  }
};

struct LoRaServiceItemT {
  uint8_t entityId;
  uint8_t service;

  uint8_t fromByteArray(const uint8_t* buf, size_t length) {
    if (length != 2) {
      return 0;
    }

    entityId = buf[0];
    service = buf[1];
    return 1;
  }
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
    configSet_req,
    service_req
  };

  LoRaHandler(LoRaClass& loRa, Stream& stream, uint8_t gatewayAddress,
              uint8_t myAddress)
      : mLoRa{loRa},
        mStream{stream},
        mGatewayAddress{gatewayAddress},
        mMyAddress{myAddress} {}

  friend inline uint8_t& operator|=(uint8_t& a, const MsgType b) {
    return (a |= static_cast<uint8_t>(b));
  }

  using OnDiscoveryReqMsgFunc = void (*)(uint8_t);
  using OnValueReqMsgFunc = void (*)(uint8_t);
  using OnConfigReqMsgFunc = void (*)(uint8_t);
  using OnConfigSetReqMsgFunc = void (*)(const ConfigValuePayloadT&);
  using OnServiceReqMsgFunc = void (*)(const LoRaServiceItemT&);

  int begin(OnDiscoveryReqMsgFunc onDiscoveryReqMsgFunc = nullptr,
            OnValueReqMsgFunc onValueReqMsgFunc = nullptr,
            OnConfigReqMsgFunc onConfigReqMsgFunc = nullptr,
            OnConfigSetReqMsgFunc onConfigSetReqMsgFunc = nullptr,
            OnServiceReqMsgFunc onServiceReqMsgFunc = nullptr);

  int loraRx();

  void beginDiscoveryMsg();
  void addDiscoveryItem(const DiscoveryItemT* item);

  void beginValueMsg();
  void addValueItem(const ValueItemT* item);

  void beginConfigsValueMsg(uint8_t entityId);
  void addConfigItemValues(const ConfigItemValueT* items, uint8_t length);

  void endMsg();

  void setDefaultHeader(LoRaHeaderT* header);

 private:
  static constexpr uint8_t msgTypeMask = 0x0f;

  void printMessage(const LoRaTxMessageT* msg);
  void printHeader(const LoRaHeaderT* header);
  void printPayload(const uint8_t* payload, uint8_t len);

  int8_t parseMsg(const LoRaRxMessageT& rxMsg);

  bool isAckRequested(const LoRaHeaderT* rxHeader) const {
    return (rxHeader->flags & FLAGS_REQ_ACK) != 0;
  }

  void sendAck(const LoRaHeaderT* rx_header);
  void sendMsg(const LoRaTxMessageT* msg);
  void sendPing(const uint8_t toAddr, int8_t rssi);

  LoRaClass& mLoRa;
  Stream& mStream;
  const uint8_t mGatewayAddress;
  const uint8_t mMyAddress;
  uint8_t mSeqId{};
  LoRaTxMessageT mMsgTx{};
  AirTime airTime{AirTime(AIRTIME_LIMIT_PPM)};

  OnDiscoveryReqMsgFunc mOnDiscoveryReqMsgFunc{nullptr};
  OnValueReqMsgFunc mOnValueReqMsgFunc{nullptr};
  OnConfigReqMsgFunc mOnConfigReqMsgFunc{nullptr};
  OnConfigSetReqMsgFunc mOnConfigSetReqMsgFunc{nullptr};
  OnServiceReqMsgFunc mOnServiceReqMsgFunc{nullptr};
};
