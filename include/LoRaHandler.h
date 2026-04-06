// clang-format off
/*
 * Header:
 *   Byte 0:    dst - Destination address
 *   Byte 1:    src - Source address
 *   Byte 2:    id  - Message sequence id
 *   Byte 3:    flags
 *     Bit 7:     Acknowledge response
 *     Bit 6:     Acknowledge request
 *     Bit 5:     Reserved for future use
 *     Bit 4:     Reserved for future use
 *     Bit 3-0:   Message type
 *       0:     ping_req
 *       1:     ping_msg
 *       2:     discovery_req
 *       3:     discovery_msg
 *       4:     value_req
 *       5:     value_msg
 *       6:     valueSet_req
 *       7:     service_req
 *       8-15:  Reserved for future message types
 *
 * Payloads:
 *   Ping request:
 *     Empty payload
 *
 *   Ping message:
 *     Byte 0-1:   RSSI of ping request message as big endian (2 bytes)
 *
 *   Discovery request:
 *     Byte 0:     EntityId (0-254, 255 means all entities)
 *
 *   Discovery message:
 *     Byte 0:      Entity Id (0-254)
 *     Byte 1:      Entity Domain - BaseComponent::Type, Examples: BINARY_SENSOR, SENSOR, COVER
 *     Byte 2:      Device Class - BinarySensorDeviceClass, SensorDeviceClass or CoverDeviceClass
 *     Byte 3:      Category - BaseComponent::Category, 0=NONE, 1=CONFIG, 2=DIAGNOSTIC
 *     Byte 4:      Unit (Unit::TypeE)
 *     Byte 5:      Format 
 *     Byte 6-9:    Minimum value in big endian (4 bytes)
 *     Byte 10-13:  Maximum value in big endian (4 bytes)
 *     Byte 14-m:   Entity Name as zero-terminated string
 *
 *     Format byte:
 *       Bit 7-5:  Reserved (zeroes)
 *       Bit 4:    0=UnSigned, 1=Signed
 *       Bit 3-2:  Size (0=1 byte, 1=2 bytes or 2=4 bytes)
 *       Bit 1-0:  Precision (Number of decimals 0-3)
 *
 *   Value request:
 *     Byte 0:    Entity Id (0-254, 255 means all entities)
 *
 *   Value message:
 *     Byte 0:    Number of entities
 *     Byte 1-m:  Array of value items
 *
 *     Value item:
 *       Byte 1:    Entity Id (0-254)
 *       Byte 2-5:  Value in big endian (4 bytes)
 *
 *   Value set request:
 *     Byte 0:    Number of entities
 *     Byte 1-m:  Array of value items
 *
 *     Value item:
 *       Byte 1:    Entity Id (0-254)
 *       Byte 2-5:  Value in big endian (4 bytes)
 *
 *   Service request
 *     Byte 0:    EntityId (0-254)
 *     Byte 1:    Service
 *
 * For encrypted messages, header byte 0-2 (dst, src and id) are sent in plain text
 * while byte 3-5 and whole payload is encrypted.
 */
// clang-format on
#pragma once

#include <Arduino.h>
#include <Cipher.h>
#include <LoRa.h>
#include <Stream.h>

#include "AirTime.h"
#include "Types.h"
#include "Util.h"

#define DEBUG_LORA_MESSAGE
// #define LORA_DRY_RUN

#define LORA_FREQUENCY 868e6
#define LORA_MAX_MESSAGE_LENGTH 150

#define LORA_HEADER_LENGTH 4
#define LORA_MAX_PAYLOAD_LENGTH (LORA_MAX_MESSAGE_LENGTH - LORA_HEADER_LENGTH)

#define FLAGS_ACK_MASK 0x80
#define FLAGS_ACK_SHIFT 7
#define FLAGS_REQ_ACK_MASK 0x40
#define FLAGS_REQ_ACK_SHIFT 6
#define FLAGS_MSG_TYPE_MASK 0x0F
#define FLAGS_MSG_TYPE_SHIFT 0

#define AIRTIME_LIMIT_PERCENT 1
#define AIRTIME_LIMIT_PPM (AIRTIME_LIMIT_PERCENT * 10000)

enum class LoRaMsgType : uint8_t {
  ping_req,
  ping_msg,
  discovery_req,
  discovery_msg,
  value_req,
  value_msg,
  valueSet_req,
  service_req
};

struct LoRaHeaderFlagsT {
  bool ack_response{false};
  bool ack_request{false};
  LoRaMsgType msgType{LoRaMsgType::ping_req};

  uint8_t fromByte(const uint8_t b) {
    ack_response = (b & FLAGS_ACK_MASK) != 0;
    ack_request = (b & FLAGS_REQ_ACK_MASK) != 0;
    msgType = static_cast<LoRaMsgType>(b & FLAGS_MSG_TYPE_MASK);
    return 1;
  }

  uint8_t toByte(uint8_t* b) const {
    uint8_t lb = (ack_response << FLAGS_ACK_SHIFT);
    lb |= (ack_request << FLAGS_REQ_ACK_SHIFT);
    lb |= (static_cast<uint8_t>(msgType) << FLAGS_MSG_TYPE_SHIFT);
    *b = lb;
    return 1;
  }
};

struct LoRaHeaderT {
  uint8_t dst{};
  uint8_t src{};
  uint8_t id{};
  LoRaHeaderFlagsT flags;

  uint8_t fromByteArray(const uint8_t* buf) {
    dst = buf[0];
    src = buf[1];
    id = buf[2];
    flags.fromByte(buf[3]);
    return 4;
  }

  uint8_t toByteArray(uint8_t* buf) const {
    buf[0] = dst;
    buf[1] = src;
    buf[2] = id;
    flags.toByte(&buf[3]);
    return 4;
  }
};

struct LoRaTxMessageT {
  LoRaHeaderT header;
  // cppcheck-suppress unusedStructMember // false positive
  uint8_t payload_length;
  // cppcheck-suppress unusedStructMember // false positive
  uint8_t payload[LORA_MAX_PAYLOAD_LENGTH]{};
};

struct LoRaRxMessageT {
  LoRaHeaderT header;
  // cppcheck-suppress unusedStructMember // false positive
  uint8_t payload_length;
  // cppcheck-suppress unusedStructMember // false positive
  int16_t rssi{};
};

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
    if (length < 2) {
      return 0;
    }

    entityId = buf[0];
    service = buf[1];
    return 1;
  }
} __attribute__((packed, aligned(1)));

class LoRaHandler {
 public:
  LoRaHandler(LoRaClass& loRa, uint8_t gatewayAddress, uint8_t myAddress,
              Cipher* cipher = nullptr)
      : mLoRa{loRa},
        mGatewayAddress{gatewayAddress},
        mMyAddress{myAddress},
        mCipher{cipher} {}

  using OnDiscoveryReqMsgFunc = void (*)(uint8_t);
  using OnValueReqMsgFunc = void (*)(void);
  using OnValueSetReqMsgFunc = void (*)(const ValueItemT&);
  using OnServiceReqMsgFunc = void (*)(const LoRaServiceItemT&);

  int16_t begin(OnDiscoveryReqMsgFunc onDiscoveryReqMsgFunc = nullptr,
                OnValueReqMsgFunc onValueReqMsgFunc = nullptr,
                OnValueSetReqMsgFunc onValueSetReqMsgFunc = nullptr,
                OnServiceReqMsgFunc onServiceReqMsgFunc = nullptr);

  int16_t loraRx();

  void beginDiscoveryMsg();
  void addDiscoveryEntity(const DiscoveryEntityT& item);

  void beginValueMsg();
  void addValueItem(const ValueItemT& item);

  void endMsg();

  void setDefaultHeader(LoRaHeaderT& header);

 private:
  void printMessage(const LoRaTxMessageT& msg);

  int8_t parseMsg(const LoRaRxMessageT& rxMsg, uint8_t* payload);

  bool isAckRequest(const LoRaHeaderT& rxHeader) const {
    return rxHeader.flags.ack_request != 0;
  }

  bool isAckResponse(const LoRaHeaderT& rxHeader) const {
    return rxHeader.flags.ack_response != 0;
  }

  void sendAck(const LoRaHeaderT& rx_header);
  void sendMsg(const LoRaTxMessageT& msg);
  void sendPing(const uint8_t toAddr, int16_t rssi);

  LoRaClass& mLoRa;
  const uint8_t mGatewayAddress;
  const uint8_t mMyAddress;
  Cipher* mCipher;
  uint8_t mMsgIdUp{};
  uint8_t mMsgIdDown{};
  LoRaTxMessageT mMsgTx{};
  uint8_t mBuffer[LORA_MAX_MESSAGE_LENGTH]{};  // Shared buffer for RX and TX
  AirTime mAirTime{AirTime(AIRTIME_LIMIT_PPM)};

  OnDiscoveryReqMsgFunc mOnDiscoveryReqMsgFunc{nullptr};
  OnValueReqMsgFunc mOnValueReqMsgFunc{nullptr};
  OnValueSetReqMsgFunc mOnValueSetReqMsgFunc{nullptr};
  OnServiceReqMsgFunc mOnServiceReqMsgFunc{nullptr};
};
