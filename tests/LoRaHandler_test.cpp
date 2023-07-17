#include "LoRaHandler.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"
#include "mocks/LoRa.h"

#define LoRa (*LoRaHandler_test::pLoRaMock)
#define LORA_GATEWAY 1
#define LORA_MY_ADDRESS 2

using ::testing::_;
using ::testing::DoAll;
using ::testing::HasSubstr;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::SaveArg;

static LoRaTxMessageT loraTxMsg;

static void loraReadBuf(const uint8_t* buf, size_t size) {
  memcpy(&loraTxMsg, buf, size);
}

bool FakeCallbackFunc_called;
uint8_t FakeCallbackFunc_entityId;

void FakeCallbackFunc(uint8_t entityId) {
  FakeCallbackFunc_called = true;
  FakeCallbackFunc_entityId = entityId;
}

LoRaConfigValuePayloadT FakeCallbackFunc_config;

void FakeConfigCallbackFunc(const LoRaConfigValuePayloadT& payload) {
  FakeCallbackFunc_called = true;
  memcpy(&FakeCallbackFunc_config, &payload, sizeof(FakeCallbackFunc_config));
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
    pSerial = new BufferSerial(256);
    pLoRaMock = new LoRaMock();
    pLH = new LoRaHandler(*pLoRaMock, *pSerial, LORA_GATEWAY, LORA_MY_ADDRESS);
    strBuf[0] = '\0';
    FakeCallbackFunc_called = false;
    FakeCallbackFunc_entityId = 0;
    memset(&FakeCallbackFunc_config, 0, sizeof(FakeCallbackFunc_config));
    memset(&FakeCallbackFunc_service, 0, sizeof(FakeCallbackFunc_service));
  }

  void TearDown() override {
    delete pLH;
    delete pLoRaMock;
    delete pSerial;
    releaseArduinoMock();
  }

  void bufSerReadStr() {
    size_t i = 0;
    while (pSerial->available()) {
      int c = pSerial->read();
      if (c < 0) {
        break;
      }
      strBuf[i++] = static_cast<char>(c);
    }
    strBuf[i] = '\0';
  }

  char strBuf[256];
  BufferSerial* pSerial;
  LoRaHandler* pLH;
  ArduinoMock* pArduinoMock;

 public:
  LoRaMock* pLoRaMock;
};

TEST_F(LoRaHandler_test, begin) {
  EXPECT_CALL(*pLoRaMock, begin(868e6)).WillOnce(Return(1));

  pLH->begin();
}

TEST_F(LoRaHandler_test, configsValueMsg) {
  const LoRaConfigItemValueT<int8_t> expectedItem1 = {123, 0x01};
  const LoRaConfigItemValueT<int32_t> expectedItem2 = {124, 0x10111213};
  const size_t payloadSize = sizeof(LoRaConfigValuePayloadT::entityId) +
                             sizeof(LoRaConfigValuePayloadT::numberOfConfigs) +
                             sizeof(expectedItem1) + sizeof(expectedItem2);
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + payloadSize;
  size_t writeSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(SaveArg<1>(&writeSize), Invoke(loraReadBuf),
                      Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(61500));

  pLH->beginConfigsValueMsg(55);
  pLH->addConfigItemValues(reinterpret_cast<const uint8_t*>(&expectedItem1),
                           sizeof(expectedItem1));
  pLH->addConfigItemValues(reinterpret_cast<const uint8_t*>(&expectedItem2),
                           sizeof(expectedItem2));
  pLH->endMsg();

  bufSerReadStr();
  EXPECT_STREQ(
      strBuf,
      "[61500] LoRaTx: H: 01 02 00 47 09 P: 37 02 7B 01 7C 10 11 12 13\r\n");

  EXPECT_EQ(writeSize, expectedMsgSize);

  EXPECT_EQ(loraTxMsg.header.dst, 1);
  EXPECT_EQ(loraTxMsg.header.src, 2);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_EQ(
      loraTxMsg.header.flags,
      FLAGS_REQ_ACK | static_cast<uint8_t>(LoRaHandler::MsgType::config_msg));
  EXPECT_EQ(loraTxMsg.header.len, payloadSize);

  const LoRaConfigValuePayloadT* pCvp =
      reinterpret_cast<LoRaConfigValuePayloadT*>(&loraTxMsg.payload);
  EXPECT_EQ(pCvp->entityId, 55);
  EXPECT_EQ(pCvp->numberOfConfigs, 2);

  const LoRaConfigItemValueT<int8_t>* pItem1 =
      reinterpret_cast<const LoRaConfigItemValueT<int8_t>*>(
          &pCvp->subPayload[0]);
  EXPECT_EQ(*pItem1, expectedItem1);

  const LoRaConfigItemValueT<int32_t>* pItem2 =
      reinterpret_cast<const LoRaConfigItemValueT<int32_t>*>(
          &pCvp->subPayload[sizeof(expectedItem1)]);
  EXPECT_EQ(*pItem2, expectedItem2);
}

TEST_F(LoRaHandler_test, discoveryMsg) {
  const LoRaDiscoveryPayloadT payload = {{123, 1, 2, 3, 4, 2},
                                         {2, {{6, 16, 4, 0}, {7, 17, 2, 1}}}};
  const size_t payloadSize =
      sizeof(payload.entity) + sizeof(payload.config.numberOfConfigs) +
      payload.config.numberOfConfigs * sizeof(payload.config.configItems[0]);
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + payloadSize;
  size_t writeSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(SaveArg<1>(&writeSize), Invoke(loraReadBuf),
                      Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));

  pLH->beginDiscoveryMsg();
  pLH->addDiscoveryItem(reinterpret_cast<const uint8_t*>(&payload),
                        payloadSize);
  pLH->endMsg();

  EXPECT_EQ(writeSize, expectedMsgSize);

  EXPECT_EQ(loraTxMsg.header.dst, 1);
  EXPECT_EQ(loraTxMsg.header.src, 2);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_EQ(loraTxMsg.header.flags,
            FLAGS_REQ_ACK |
                static_cast<uint8_t>(LoRaHandler::MsgType::discovery_msg));
  EXPECT_EQ(loraTxMsg.header.len, payloadSize);

  const LoRaDiscoveryPayloadT* pDp =
      reinterpret_cast<LoRaDiscoveryPayloadT*>(&loraTxMsg.payload);
  EXPECT_EQ(*pDp, payload);
}

TEST_F(LoRaHandler_test, LoRaConfigPayloadT_equal) {
  const LoRaConfigPayloadT expected = {1, {10, 11, 1, 2}};
  LoRaConfigPayloadT actual;
  memcpy(&actual, &expected, sizeof(actual));

  EXPECT_EQ(actual, expected);
}

TEST_F(LoRaHandler_test, LoRaConfigPayloadT_numberOfConfigs_not_equal) {
  const LoRaConfigPayloadT expected = {1, {10, 11, 1, 2}};
  LoRaConfigPayloadT actual;
  memcpy(&actual, &expected, sizeof(actual));
  actual.numberOfConfigs++;

  EXPECT_NE(actual, expected);
}

TEST_F(LoRaHandler_test, LoRaConfigPayloadT_too_many_configs) {
  const LoRaConfigPayloadT expected = {LORA_CONFIG_ITEMS_MAX + 1,
                                       {10, 11, 1, 2}};
  LoRaConfigPayloadT actual;
  memcpy(&actual, &expected, sizeof(actual));

  EXPECT_NE(actual, expected);
}

TEST_F(LoRaHandler_test, LoRaConfigPayloadT_configItem_not_equal) {
  const LoRaConfigPayloadT expected = {1, {10, 11, 1, 2}};
  LoRaConfigPayloadT actual;
  memcpy(&actual, &expected, sizeof(actual));
  actual.configItems[0].configId++;

  EXPECT_NE(actual, expected);
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
  EXPECT_CALL(*pLoRaMock, available()).WillOnce(Return(0));
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
  EXPECT_CALL(*pLoRaMock, available()).WillOnce(Return(0));
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
      .WillOnce(Return(LORA_MY_ADDRESS))  // dst
      .WillOnce(Return(LORA_GATEWAY))     // src
      .WillOnce(Return(0))                // id
      .WillOnce(Return(
          static_cast<uint8_t>(LoRaHandler::MsgType::ping_req)))  // flags
      .WillOnce(Return(0));                                       // len
  EXPECT_CALL(*pLoRaMock, available()).WillOnce(Return(0));
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // TX
  const size_t expectedPayloadSize = 1;
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + expectedPayloadSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(0)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(Invoke(loraReadBuf), Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));

  // Recieve msg
  EXPECT_EQ(pLH->loraRx(), LORA_HEADER_LENGTH);

  // Check TX header
  EXPECT_EQ(loraTxMsg.header.dst, LORA_GATEWAY);
  EXPECT_EQ(loraTxMsg.header.src, LORA_MY_ADDRESS);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_EQ(
      loraTxMsg.header.flags,
      FLAGS_REQ_ACK | static_cast<uint8_t>(LoRaHandler::MsgType::ping_msg));
  EXPECT_EQ(loraTxMsg.header.len, expectedPayloadSize);

  // Check TX payload
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
      .WillOnce(Return(
          static_cast<uint8_t>(LoRaHandler::MsgType::discovery_req)))  // flags
      .WillOnce(Return(1))                                             // len
      // Payload
      .WillOnce(Return(entityId));
  EXPECT_CALL(*pLoRaMock, available()).WillOnce(Return(1)).WillOnce(Return(0));
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
      .WillOnce(Return(
          FLAGS_REQ_ACK |
          static_cast<uint8_t>(LoRaHandler::MsgType::discovery_req)))  // flags
      .WillOnce(Return(1))                                             // len
      // Payload
      .WillOnce(Return(entityId));
  EXPECT_CALL(*pLoRaMock, available()).WillOnce(Return(1)).WillOnce(Return(0));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // TX
  const size_t expectedPayloadSize = 0;
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + expectedPayloadSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(0)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(Invoke(loraReadBuf), Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));

  // Begin and recieve msg
  pLH->begin(FakeCallbackFunc, nullptr, nullptr, nullptr, nullptr);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  // Check TX header
  EXPECT_EQ(loraTxMsg.header.dst, LORA_GATEWAY);
  EXPECT_EQ(loraTxMsg.header.src, LORA_MY_ADDRESS);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_EQ(
      loraTxMsg.header.flags,
      FLAGS_ACK | static_cast<uint8_t>(LoRaHandler::MsgType::discovery_req));
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
      .WillOnce(Return(LORA_MY_ADDRESS))  // dst
      .WillOnce(Return(LORA_GATEWAY))     // src
      .WillOnce(Return(0))                // id
      .WillOnce(Return(
          static_cast<uint8_t>(LoRaHandler::MsgType::value_req)))  // flags
      .WillOnce(Return(1))                                         // len
      // Payload
      .WillOnce(Return(entityId));
  EXPECT_CALL(*pLoRaMock, available()).WillOnce(Return(1)).WillOnce(Return(0));
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
       loraRx_value_req_no_ack_shall_call_OnConfigReqMsgFunc) {
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
      .WillOnce(Return(
          static_cast<uint8_t>(LoRaHandler::MsgType::config_req)))  // flags
      .WillOnce(Return(1))                                          // len
      // Payload
      .WillOnce(Return(entityId));
  EXPECT_CALL(*pLoRaMock, available()).WillOnce(Return(1)).WillOnce(Return(0));
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
       loraRx_value_req_no_ack_shall_call_OnConfigSetReqMsgFunc) {
  const LoRaConfigValuePayloadT payload = {56, 1, {2, 3, 4, 5, 6}};
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
      .WillOnce(Return(
          static_cast<uint8_t>(LoRaHandler::MsgType::configSet_req)))  // flags
      .WillOnce(Return(7))                                             // len
      // Payload
      .WillOnce(Return(payload.entityId))
      .WillOnce(Return(payload.numberOfConfigs))
      .WillOnce(Return(payload.subPayload[0]))
      .WillOnce(Return(payload.subPayload[1]))
      .WillOnce(Return(payload.subPayload[2]))
      .WillOnce(Return(payload.subPayload[3]))
      .WillOnce(Return(payload.subPayload[4]));
  EXPECT_CALL(*pLoRaMock, available())
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(0));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // Begin and recieve msg
  pLH->begin(nullptr, nullptr, nullptr, FakeConfigCallbackFunc, nullptr);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  // Check callback function
  EXPECT_TRUE(FakeCallbackFunc_called);
  EXPECT_EQ(FakeCallbackFunc_config.entityId, payload.entityId);
  EXPECT_EQ(FakeCallbackFunc_config.numberOfConfigs, payload.numberOfConfigs);
  EXPECT_EQ(FakeCallbackFunc_config.subPayload[0], payload.subPayload[0]);
  EXPECT_EQ(FakeCallbackFunc_config.subPayload[1], payload.subPayload[1]);
  EXPECT_EQ(FakeCallbackFunc_config.subPayload[2], payload.subPayload[2]);
  EXPECT_EQ(FakeCallbackFunc_config.subPayload[3], payload.subPayload[3]);
  EXPECT_EQ(FakeCallbackFunc_config.subPayload[4], payload.subPayload[4]);
}

TEST_F(LoRaHandler_test,
       loraRx_value_req_no_ack_shall_call_OnServiceReqMsgFunc) {
  const LoRaServiceItemT payload = {56, 1};
  const uint8_t rxMsgSize = LORA_HEADER_LENGTH + sizeof(payload);
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
      .WillOnce(Return(
          static_cast<uint8_t>(LoRaHandler::MsgType::service_req)))  // flags
      .WillOnce(Return(sizeof(payload)))                             // len
      // Payload
      .WillOnce(Return(payload.entityId))
      .WillOnce(Return(payload.service));
  EXPECT_CALL(*pLoRaMock, available())
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(0));
  // RX-RSSI
  EXPECT_CALL(*pLoRaMock, packetRssi()).WillOnce(Return(rssi));

  // Begin and recieve msg
  pLH->begin(nullptr, nullptr, nullptr, nullptr, FakeServiceCallbackFunc);
  EXPECT_EQ(pLH->loraRx(), rxMsgSize);

  // Check callback function
  EXPECT_TRUE(FakeCallbackFunc_called);
  EXPECT_EQ(FakeCallbackFunc_service.entityId, payload.entityId);
  EXPECT_EQ(FakeCallbackFunc_service.service, payload.service);
}

// TODO: More Examples of real packets

TEST_F(LoRaHandler_test, setDefaultHeader) {
  LoRaHeaderT header;

  pLH->setDefaultHeader(&header);

  EXPECT_EQ(header.dst, 1);
  EXPECT_EQ(header.src, 2);
  EXPECT_EQ(header.id, 0);
  EXPECT_EQ(header.flags, FLAGS_REQ_ACK);
  EXPECT_EQ(header.len, 0);
}

// TODO: Not according to protocol, Only one or more values in one message?
TEST_F(LoRaHandler_test, valueMsg) {
  const LoRaValueItemT<int8_t> expectedItem1 = {123, 0x01};
  const LoRaValueItemT<int32_t> expectedItem2 = {124, 0x10111213};
  const size_t payloadSize = sizeof(LoRaValuePayloadT::numberOfEntities) +
                             sizeof(expectedItem1) + sizeof(expectedItem2);
  const size_t expectedMsgSize = LORA_HEADER_LENGTH + payloadSize;
  size_t writeSize;
  EXPECT_CALL(*pLoRaMock, beginPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pLoRaMock, write(_, expectedMsgSize))
      .WillOnce(DoAll(SaveArg<1>(&writeSize), Invoke(loraReadBuf),
                      Return(expectedMsgSize)));
  EXPECT_CALL(*pLoRaMock, endPacket(false)).WillOnce(Return(1));
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(61500));

  pLH->beginValueMsg();
  pLH->addValueItem(reinterpret_cast<const uint8_t*>(&expectedItem1),
                    sizeof(expectedItem1));
  pLH->addValueItem(reinterpret_cast<const uint8_t*>(&expectedItem2),
                    sizeof(expectedItem2));
  pLH->endMsg();

  bufSerReadStr();
  EXPECT_STREQ(
      strBuf,
      "[61500] LoRaTx: H: 01 02 00 45 08 P: 02 7B 01 7C 13 12 11 10\r\n");

  EXPECT_EQ(writeSize, expectedMsgSize);

  EXPECT_EQ(loraTxMsg.header.dst, 1);
  EXPECT_EQ(loraTxMsg.header.src, 2);
  EXPECT_EQ(loraTxMsg.header.id, 0);
  EXPECT_EQ(
      loraTxMsg.header.flags,
      FLAGS_REQ_ACK | static_cast<uint8_t>(LoRaHandler::MsgType::value_msg));
  EXPECT_EQ(loraTxMsg.header.len, payloadSize);

  const LoRaValuePayloadT* pVp =
      reinterpret_cast<LoRaValuePayloadT*>(&loraTxMsg.payload);
  EXPECT_EQ(pVp->numberOfEntities, 2);

  const LoRaValueItemT<int8_t>* pItem1 =
      reinterpret_cast<const LoRaValueItemT<int8_t>*>(&pVp->subPayload[0]);
  EXPECT_EQ(*pItem1, expectedItem1);

  const LoRaValueItemT<int32_t>* pItem2 =
      reinterpret_cast<const LoRaValueItemT<int32_t>*>(
          &pVp->subPayload[sizeof(expectedItem1)]);
  EXPECT_EQ(*pItem2, expectedItem2);
}
