#include "LoRaHandler.h"

#include <AES.h>
#include <CTR.h>
#include <gtest/gtest.h>

#include "Arduino.h"
#include "BufferSerial.h"
#include "LoRa.h"
#include "Types.h"

#define LoRa (*LoRaHandler_test::pLoRaMock)
#define LORA_GATEWAY 1
#define LORA_MY_ADDRESS 2

using ::testing::_;
using ::testing::DoAll;
using ::testing::HasSubstr;
using ::testing::Invoke;
using ::testing::IsSupersetOf;
using ::testing::Return;
using ::testing::SaveArg;

static LoRaTxMessageT loraTxMsg;
typedef struct {
  uint8_t buf[LORA_MAX_MESSAGE_LENGTH] = {};
  size_t length{};
} LoRaBufT;

static LoRaBufT loraTxBuf;

struct EncryptedMsg {
  EncryptedMsg() {}

  uint8_t mockLoRaRead() { return buffer.buf[i++]; }

  size_t i{};
  LoRaBufT buffer;
};

static void loraReadBuf(const uint8_t* buf, size_t size) {
  uint8_t n = loraTxMsg.header.fromByteArray(&buf[0]);
  EXPECT_EQ(size, n + loraTxMsg.header.len);
  memcpy(loraTxMsg.payload, &buf[n], loraTxMsg.header.len);
}

static void loraReadRawBuf(const uint8_t* buf, size_t size) {
  EXPECT_LE(size, LORA_MAX_MESSAGE_LENGTH);
  memcpy(loraTxBuf.buf, buf, size);
  loraTxBuf.length = size;
}

bool FakeCallbackFunc_called;
uint8_t FakeCallbackFunc_entityId;

void FakeCallbackFunc(uint8_t entityId) {
  FakeCallbackFunc_called = true;
  FakeCallbackFunc_entityId = entityId;
}

ConfigValuePayloadT FakeCallbackFunc_config;

void FakeConfigCallbackFunc(const ConfigValuePayloadT& item) {
  FakeCallbackFunc_called = true;
  memcpy(&FakeCallbackFunc_config, &item, sizeof(FakeCallbackFunc_config));
}

LoRaServiceItemT FakeCallbackFunc_service;

void FakeServiceCallbackFunc(const LoRaServiceItemT& item) {
  FakeCallbackFunc_called = true;
  memcpy(&FakeCallbackFunc_service, &item, sizeof(FakeCallbackFunc_service));
}

class LoRaHandler_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    pLoRaMock = new LoRaMock();
    pLH = new LoRaHandler(*pLoRaMock, LORA_GATEWAY, LORA_MY_ADDRESS);
    strBuf[0] = '\0';
    Serial.flush();
    FakeCallbackFunc_called = false;
    FakeCallbackFunc_entityId = 0;
    memset(&FakeCallbackFunc_config, 0, sizeof(FakeCallbackFunc_config));
    memset(&FakeCallbackFunc_service, 0, sizeof(FakeCallbackFunc_service));
  }

  void TearDown() override {
    delete pLH;
    delete pLoRaMock;
    releaseArduinoMock();
  }

  void bufSerReadStr() {
    size_t i = 0;
    while (Serial.available()) {
      int c = Serial.read();
      if (c < 0) {
        break;
      }
      strBuf[i++] = static_cast<char>(c);
    }
    strBuf[i] = '\0';
  }

  char strBuf[256];
  LoRaHandler* pLH;
  ArduinoMock* pArduinoMock;

 public:
  LoRaMock* pLoRaMock;
};

TEST(LoRaHandler_LoRaServiceItemT_test, fromByteArray) {
  const uint8_t buf[] = {1, 2};
  LoRaServiceItemT item;

  EXPECT_EQ(item.fromByteArray(buf, sizeof(buf)), 1);
}

TEST(LoRaHandler_LoRaServiceItemT_test, fromByteArray_invalid_length) {
  const uint8_t buf[] = {1, 2};
  LoRaServiceItemT item;

  EXPECT_EQ(item.fromByteArray(buf, sizeof(buf) - 1), 0);
}

TEST_F(LoRaHandler_test, begin) {
  EXPECT_CALL(*pLoRaMock, begin(868e6)).WillOnce(Return(1));

  pLH->begin();
}

TEST_F(LoRaHandler_test, configsValueMsg) {
  const uint8_t entityId = 55;
  const ConfigItemValueT items[2] = {{1, 0x11}, {2, 0x11223344}};
  const size_t payloadSize = sizeof(ConfigValuePayloadT::entityId) +
                             sizeof(ConfigValuePayloadT::numberOfConfigs) +
                             items[0].size() * 2;
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + payloadSize;
  const uint8_t expectedPayload[] = {55,   2, 1,    0x00, 0x00, 0x00,
                                     0x11, 2, 0x11, 0x22, 0x33, 0x44};
  size_t writeSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(SaveArg<1>(&writeSize), Invoke(loraReadBuf),
                      Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(61500))         // Print
      .WillOnce(Return(61501))         // AirTime update
      .WillOnce(Return(61502))         // Send start
      .WillRepeatedly(Return(61602));  // Send end and debug print of AirTime

  pLH->beginConfigsValueMsg(entityId);
  pLH->addConfigItemValues(items, 2);
  pLH->endMsg();

  bufSerReadStr();
  // clang-format off
  EXPECT_STREQ(strBuf,
    "[61500] LoRaTx: H: 01 02 00 47 0C P: 37 02 01 00 00 00 11 02 11 22 33 44\r\n[61602] AirTime: 100 ms, 28 ppm\r\n");
  // clang-format on

  EXPECT_EQ(writeSize, expectedMsgSize);

  EXPECT_EQ(loraTxMsg.header.dst, 1);
  EXPECT_EQ(loraTxMsg.header.src, 2);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_TRUE(loraTxMsg.header.flags.ack_request);
  EXPECT_EQ(loraTxMsg.header.flags.msgType, LoRaMsgType::config_msg);
  EXPECT_EQ(loraTxMsg.header.len, payloadSize);

  EXPECT_THAT(loraTxMsg.payload, IsSupersetOf(expectedPayload));
}

TEST_F(LoRaHandler_test, discoveryMsg) {
  const DiscoveryItemT item = {{123, 1, 2, 3, 3, 2, 1, 0},
                               2,
                               {{6, 16, 3, 2, 1, 0}, {7, 17, 2, 1, 0, 0}}};
  const size_t payloadSize = item.size();
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + payloadSize;
  size_t writeSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(SaveArg<1>(&writeSize), Invoke(loraReadBuf),
                      Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(100))         // Print
      .WillOnce(Return(101))         // AirTime update
      .WillOnce(Return(102))         // Send start
      .WillRepeatedly(Return(202));  // Send end and debug print of AirTime

  pLH->beginDiscoveryMsg();
  pLH->addDiscoveryItem((&item));
  pLH->endMsg();

  bufSerReadStr();
  // clang-format off
  EXPECT_STREQ(strBuf,
    "[100] LoRaTx: H: 01 02 00 43 0C P: 7B 01 02 03 1B 02 06 10 1B 07 11 06\r\n[202] AirTime: 100 ms, 28 ppm\r\n");
  // clang-format on

  EXPECT_EQ(writeSize, expectedMsgSize);

  EXPECT_EQ(loraTxMsg.header.dst, 1);
  EXPECT_EQ(loraTxMsg.header.src, 2);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_TRUE(loraTxMsg.header.flags.ack_request);
  EXPECT_EQ(loraTxMsg.header.flags.msgType, LoRaMsgType::discovery_msg);
  EXPECT_EQ(loraTxMsg.header.len, payloadSize);

  const DiscoveryItemT* pDi =
      reinterpret_cast<DiscoveryItemT*>(&loraTxMsg.payload);
  EXPECT_EQ(pDi->entity, item.entity);
  EXPECT_EQ(pDi->numberOfConfigItems, item.numberOfConfigItems);
  EXPECT_EQ(pDi->configItems[0], item.configItems[0]);
  EXPECT_EQ(pDi->configItems[1], item.configItems[1]);
}

TEST_F(LoRaHandler_test, loraRx_no_packet_shall_do_nothing) {
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(0));

  EXPECT_EQ(pLH->loraRx(), 0);
}

TEST_F(LoRaHandler_test, loraRx_other_dst_shall_do_nothing) {
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(LORA_HEADER_LENGTH));
  EXPECT_CALL(*pLoRaMock, read())
      .WillOnce(Return(LORA_MY_ADDRESS + 1))  // dst
      .WillOnce(Return(LORA_GATEWAY))         // src
      .WillOnce(Return(0))                    // id
      .WillOnce(Return(0))                    // flags
      .WillOnce(Return(0));                   // len
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(-111));

  EXPECT_EQ(pLH->loraRx(), 0);

  bufSerReadStr();
  EXPECT_THAT(strBuf, HasSubstr("not for me, drop msg"));
}

TEST_F(LoRaHandler_test, loraRx_unknown_msgType_shall_do_nothing) {
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(LORA_HEADER_LENGTH));
  EXPECT_CALL(*pLoRaMock, read())
      .WillOnce(Return(LORA_MY_ADDRESS))  // dst
      .WillOnce(Return(LORA_GATEWAY))     // src
      .WillOnce(Return(0))                // id
      .WillOnce(Return(0x0F))             // flags
      .WillOnce(Return(0));               // len
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(-111));

  EXPECT_EQ(pLH->loraRx(), -1);

  bufSerReadStr();
  EXPECT_THAT(strBuf, HasSubstr("Error: Failed to parse msg"));
}

TEST_F(LoRaHandler_test, loraRx_ping_req_no_ack_shall_send_ping_msg) {
  int8_t rssi = -111;

  // RX
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(LORA_HEADER_LENGTH));
  EXPECT_CALL(*pLoRaMock, read())
      .WillOnce(Return(LORA_MY_ADDRESS))                              // dst
      .WillOnce(Return(LORA_GATEWAY))                                 // src
      .WillOnce(Return(0))                                            // id
      .WillOnce(Return(static_cast<uint8_t>(LoRaMsgType::ping_req)))  // flags
      .WillOnce(Return(0));                                           // len
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // TX
  const size_t expectedPayloadSize = 1;
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + expectedPayloadSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(0)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(Invoke(loraReadBuf), Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(100))         // Print
      .WillOnce(Return(101))         // AirTime update
      .WillOnce(Return(102))         // Send start
      .WillRepeatedly(Return(202));  // Send end and debug print of AirTime

  // Recieve msg
  EXPECT_EQ(pLH->loraRx(), LORA_HEADER_LENGTH);

  bufSerReadStr();
  EXPECT_THAT(strBuf,
              IsSupersetOf("[100] LoRaTx: H: 01 02 00 41 01 P: 91\r\n[202] "
                           "AirTime: 100 ms, 28 ppm\r\n"));

  // Check TX header
  EXPECT_EQ(loraTxMsg.header.dst, LORA_GATEWAY);
  EXPECT_EQ(loraTxMsg.header.src, LORA_MY_ADDRESS);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_TRUE(loraTxMsg.header.flags.ack_request);
  EXPECT_EQ(loraTxMsg.header.flags.msgType, LoRaMsgType::ping_msg);
  EXPECT_EQ(loraTxMsg.header.len, expectedPayloadSize);

  // Check TX item
  EXPECT_EQ(static_cast<int8_t>(loraTxMsg.payload[0]), rssi);
}

TEST_F(LoRaHandler_test,
       loraRx_discovery_req_no_ack_shall_call_OnDiscoveryReqMsgFunc) {
  const uint8_t rxMsgSize = LORA_HEADER_LENGTH + 1;
  const int8_t rssi = -111;
  const uint8_t entityId = 56;

  // Begin
  EXPECT_CALL(*pLoRaMock, begin(_)).WillOnce(Return(1));

  // RX
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(rxMsgSize));
  EXPECT_CALL(*pLoRaMock, read())
      // Header
      .WillOnce(Return(LORA_MY_ADDRESS))  // dst
      .WillOnce(Return(LORA_GATEWAY))     // src
      .WillOnce(Return(0))                // id
      .WillOnce(
          Return(static_cast<uint8_t>(LoRaMsgType::discovery_req)))  // flags
      .WillOnce(Return(1))                                           // len
      // Payload
      .WillOnce(Return(entityId));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // Begin and recieve msg
  pLH->begin(FakeCallbackFunc, nullptr, nullptr, nullptr, nullptr);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  // Check callback function
  EXPECT_TRUE(FakeCallbackFunc_called);
  EXPECT_EQ(FakeCallbackFunc_entityId, entityId);
}

TEST_F(
    LoRaHandler_test,
    loraRx_discovery_req_with_ack_shall_send_ack_and_call_OnDiscoveryReqMsgFunc) {
  const uint8_t rxMsgSize = LORA_HEADER_LENGTH + 1;
  const int8_t rssi = -111;
  const uint8_t entityId = 56;

  // Begin
  EXPECT_CALL(*pLoRaMock, begin(_)).WillOnce(Return(1));

  // RX
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(rxMsgSize));
  EXPECT_CALL(*pLoRaMock, read())
      // Header
      .WillOnce(Return(LORA_MY_ADDRESS))  // dst
      .WillOnce(Return(LORA_GATEWAY))     // src
      .WillOnce(Return(0))                // id
      .WillOnce(
          Return(1 << FLAGS_REQ_ACK_SHIFT |
                 static_cast<uint8_t>(LoRaMsgType::discovery_req)))  // flags
      .WillOnce(Return(1))                                           // len
      // Payload
      .WillOnce(Return(entityId));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // TX
  const size_t expectedPayloadSize = 0;
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + expectedPayloadSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(0)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(Invoke(loraReadBuf), Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(100))         // Print
      .WillOnce(Return(101))         // AirTime update
      .WillOnce(Return(102))         // Send start
      .WillRepeatedly(Return(202));  // Send end and debug print of AirTime

  // Begin and recieve msg
  pLH->begin(FakeCallbackFunc, nullptr, nullptr, nullptr, nullptr);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  bufSerReadStr();
  EXPECT_THAT(strBuf,
              IsSupersetOf("[100] LoRaTx: H: 01 02 00 82 00 P: --\r\n[202] "
                           "AirTime: 100 ms, 28 ppm\r\n"));

  // Check TX header
  EXPECT_EQ(loraTxMsg.header.dst, LORA_GATEWAY);
  EXPECT_EQ(loraTxMsg.header.src, LORA_MY_ADDRESS);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_FALSE(loraTxMsg.header.flags.ack_request);
  EXPECT_TRUE(loraTxMsg.header.flags.ack_response);
  EXPECT_EQ(loraTxMsg.header.flags.msgType, LoRaMsgType::discovery_req);
  EXPECT_EQ(loraTxMsg.header.len, expectedPayloadSize);

  // Check callback function
  EXPECT_TRUE(FakeCallbackFunc_called);
  EXPECT_EQ(FakeCallbackFunc_entityId, entityId);
}

TEST_F(LoRaHandler_test, loraRx_value_req_no_ack_shall_call_OnValueReqMsgFunc) {
  const uint8_t rxMsgSize = LORA_HEADER_LENGTH + 1;
  const int8_t rssi = -111;
  const uint8_t entityId = 56;

  // Begin
  EXPECT_CALL(*pLoRaMock, begin(_)).WillOnce(Return(1));

  // RX
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(rxMsgSize));
  EXPECT_CALL(*pLoRaMock, read())
      // Header
      .WillOnce(Return(LORA_MY_ADDRESS))                               // dst
      .WillOnce(Return(LORA_GATEWAY))                                  // src
      .WillOnce(Return(0))                                             // id
      .WillOnce(Return(static_cast<uint8_t>(LoRaMsgType::value_req)))  // flags
      .WillOnce(Return(1))                                             // len
      // Payload
      .WillOnce(Return(entityId));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // Begin and recieve msg
  pLH->begin(nullptr, FakeCallbackFunc, nullptr, nullptr, nullptr);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  // Check callback function
  EXPECT_TRUE(FakeCallbackFunc_called);
  EXPECT_EQ(FakeCallbackFunc_entityId, entityId);
}

TEST_F(LoRaHandler_test,
       loraRx_config_req_no_ack_shall_call_OnConfigReqMsgFunc) {
  const uint8_t rxMsgSize = LORA_HEADER_LENGTH + 1;
  const int8_t rssi = -111;
  const uint8_t entityId = 56;

  // Begin
  EXPECT_CALL(*pLoRaMock, begin(_)).WillOnce(Return(1));

  // RX
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(rxMsgSize));
  EXPECT_CALL(*pLoRaMock, read())
      // Header
      .WillOnce(Return(LORA_MY_ADDRESS))                                // dst
      .WillOnce(Return(LORA_GATEWAY))                                   // src
      .WillOnce(Return(0))                                              // id
      .WillOnce(Return(static_cast<uint8_t>(LoRaMsgType::config_req)))  // flags
      .WillOnce(Return(1))                                              // len
      // Payload
      .WillOnce(Return(entityId));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // Begin and recieve msg
  pLH->begin(nullptr, nullptr, FakeCallbackFunc, nullptr, nullptr);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  // Check callback function
  EXPECT_TRUE(FakeCallbackFunc_called);
  EXPECT_EQ(FakeCallbackFunc_entityId, entityId);
}

TEST_F(LoRaHandler_test,
       loraRx_configSet_req_no_ack_shall_call_OnConfigSetReqMsgFunc) {
  const ConfigValuePayloadT item = {56, 1, {{2, 0x11223344}}};
  const uint8_t rxMsgSize = LORA_HEADER_LENGTH + 7;
  const int8_t rssi = -111;

  // Begin
  EXPECT_CALL(*pLoRaMock, begin(_)).WillOnce(Return(1));

  // RX
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(rxMsgSize));
  EXPECT_CALL(*pLoRaMock, read())
      // Header
      .WillOnce(Return(LORA_MY_ADDRESS))  // dst
      .WillOnce(Return(LORA_GATEWAY))     // src
      .WillOnce(Return(0))                // id
      .WillOnce(
          Return(static_cast<uint8_t>(LoRaMsgType::configSet_req)))  // flags
      .WillOnce(Return(7))                                           // len
      // Payload
      .WillOnce(Return(item.entityId))
      .WillOnce(Return(item.numberOfConfigs))
      .WillOnce(Return(item.configValues[0].configId))
      .WillOnce(Return(0x11))
      .WillOnce(Return(0x22))
      .WillOnce(Return(0x33))
      .WillOnce(Return(0x44));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // Begin and recieve msg
  pLH->begin(nullptr, nullptr, nullptr, FakeConfigCallbackFunc, nullptr);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  // Check callback function
  EXPECT_TRUE(FakeCallbackFunc_called);
  EXPECT_EQ(FakeCallbackFunc_config.entityId, item.entityId);
  EXPECT_EQ(FakeCallbackFunc_config.numberOfConfigs, item.numberOfConfigs);
  EXPECT_EQ(FakeCallbackFunc_config.configValues[0].configId,
            item.configValues[0].configId);
  EXPECT_EQ(FakeCallbackFunc_config.configValues[0].value,
            item.configValues[0].value);
}

TEST_F(LoRaHandler_test,
       loraRx_service_req_no_ack_shall_call_OnServiceReqMsgFunc) {
  const LoRaServiceItemT item = {56, 1};
  const uint8_t rxMsgSize = LORA_HEADER_LENGTH + sizeof(item);
  const int8_t rssi = -111;

  // Begin
  EXPECT_CALL(*pLoRaMock, begin(_)).WillOnce(Return(1));

  // RX
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(rxMsgSize));
  EXPECT_CALL(*pLoRaMock, read())
      // Header
      .WillOnce(Return(LORA_MY_ADDRESS))  // dst
      .WillOnce(Return(LORA_GATEWAY))     // src
      .WillOnce(Return(0))                // id
      .WillOnce(
          Return(static_cast<uint8_t>(LoRaMsgType::service_req)))  // flags
      .WillOnce(Return(sizeof(item)))                              // len
      // Payload
      .WillOnce(Return(item.entityId))
      .WillOnce(Return(item.service));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // Begin and recieve msg
  pLH->begin(nullptr, nullptr, nullptr, nullptr, FakeServiceCallbackFunc);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  // Check callback function
  EXPECT_TRUE(FakeCallbackFunc_called);
  EXPECT_EQ(FakeCallbackFunc_service.entityId, item.entityId);
  EXPECT_EQ(FakeCallbackFunc_service.service, item.service);
}

// TODO: More Examples of real packets

TEST_F(LoRaHandler_test, setDefaultHeader) {
  LoRaHeaderT header;

  pLH->setDefaultHeader(header);

  EXPECT_EQ(header.dst, 1);
  EXPECT_EQ(header.src, 2);
  EXPECT_EQ(header.id, 0);
  EXPECT_TRUE(header.flags.ack_request);
  EXPECT_EQ(header.len, 0);
}

// TODO: Not according to protocol, Only one or more values in one message?
TEST_F(LoRaHandler_test, valueMsg) {
  const ValueItemT item1 = ValueItemT(123, 0x01);
  const ValueItemT item2 = ValueItemT(124, 0x11223344);
  const size_t payloadSize =
      sizeof(LoRaValuePayloadT::numberOfEntities) + item1.size() + item2.size();
  const uint8_t expectedPayload[] = {2,   123,  0x00, 0x00, 0x00, 0x01,
                                     124, 0x11, 0x22, 0x33, 0x44};
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + payloadSize;
  size_t writeSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(SaveArg<1>(&writeSize), Invoke(loraReadBuf),
                      Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillOnce(Return(61500))         // Print
      .WillOnce(Return(61501))         // AirTime update
      .WillOnce(Return(61502))         // Send start
      .WillRepeatedly(Return(61602));  // Send end and debug print of AirTime

  pLH->beginValueMsg();
  pLH->addValueItem(&item1);
  pLH->addValueItem(&item2);
  pLH->endMsg();

  bufSerReadStr();
  // clang-format off
  EXPECT_STREQ(
      strBuf,
      "[61500] LoRaTx: H: 01 02 00 45 0B P: 02 7B 00 00 00 01 7C 11 22 33 44\r\n[61602] AirTime: 100 ms, 28 ppm\r\n");
  // clang-format off

  EXPECT_EQ(writeSize, expectedMsgSize);

  EXPECT_EQ(loraTxMsg.header.dst, 1);
  EXPECT_EQ(loraTxMsg.header.src, 2);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_TRUE(loraTxMsg.header.flags.ack_request);
  EXPECT_EQ(loraTxMsg.header.flags.msgType, LoRaMsgType::value_msg);
  EXPECT_EQ(loraTxMsg.header.len, payloadSize);

  EXPECT_THAT(loraTxMsg.payload, IsSupersetOf(expectedPayload));
}

TEST_F(LoRaHandler_test, encrypted_msg) {
  const int8_t rssi = -111;
  const byte AES_KEY[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                  0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                                  0x0C, 0x0D, 0x0E, 0x0F};
  const byte CTR_IV[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

  CTR<AES128> ctrAes128_tester;
  ctrAes128_tester.clear();
  ctrAes128_tester.setCounterSize(4);
  ctrAes128_tester.setKey(AES_KEY, ctrAes128_tester.keySize());
  ctrAes128_tester.setIV(CTR_IV, ctrAes128_tester.ivSize());

  CTR<AES128> ctrAes128;
  ctrAes128.clear();
  ctrAes128.setCounterSize(4);
  ctrAes128.setKey(AES_KEY, ctrAes128.keySize());
  ctrAes128.setIV(CTR_IV, ctrAes128.ivSize());

  LoRaHandler lora(*pLoRaMock, LORA_GATEWAY, LORA_MY_ADDRESS, &ctrAes128);

  const uint8_t pingReqMsg[LORA_HEADER_LENGTH] =
    {LORA_MY_ADDRESS, LORA_GATEWAY, 0, static_cast<uint8_t>(LoRaMsgType::ping_req), 0};
  EncryptedMsg encryptedPingReqMsg;
  ctrAes128_tester.encrypt(encryptedPingReqMsg.buffer.buf, pingReqMsg, LORA_HEADER_LENGTH);
  encryptedPingReqMsg.buffer.length = LORA_HEADER_LENGTH;

  // RX
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(LORA_HEADER_LENGTH));
  EXPECT_CALL(*pLoRaMock, read())
      .Times(LORA_HEADER_LENGTH)
      .WillRepeatedly(Invoke(&encryptedPingReqMsg, &EncryptedMsg::mockLoRaRead));
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // TX
  const size_t expectedPayloadSize = 1;
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + expectedPayloadSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(0)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(Invoke(loraReadRawBuf), Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis())
      .WillRepeatedly(Return(0));

  // Recieve msg
  EXPECT_EQ(lora.loraRx(), LORA_HEADER_LENGTH);

  bufSerReadStr();
  EXPECT_THAT(strBuf, IsSupersetOf("[0] LoRaTx: H: 01 02 00 41 01 P: 91\r\n[0] "
                               "AirTime: 0 ms, 0 ppm\r\n"));

  uint8_t decryptedPingRespMsg[LORA_MAX_MESSAGE_LENGTH];
  ctrAes128_tester.decrypt(decryptedPingRespMsg, loraTxBuf.buf, loraTxBuf.length);

  LoRaHeaderT header;
  header.fromByteArray(&decryptedPingRespMsg[0]);

  // Check TX header
  EXPECT_EQ(header.dst, LORA_GATEWAY);
  EXPECT_EQ(header.src, LORA_MY_ADDRESS);
  EXPECT_EQ(header.id, 0);
  EXPECT_TRUE(header.flags.ack_request);
  EXPECT_EQ(header.flags.msgType, LoRaMsgType::ping_msg);
  EXPECT_EQ(header.len, expectedPayloadSize);

  // Check TX item
  EXPECT_EQ(static_cast<int8_t>(decryptedPingRespMsg[5]), rssi);
}
