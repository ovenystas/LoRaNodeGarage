#include "LoRaHandler.h"

#include <gtest/gtest.h>

#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"
#include "mocks/LoRa.h"

#define LoRa (*LoRaHandler_test::pLoRaMock)

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::IsSupersetOf;
using ::testing::Return;
using ::testing::SaveArg;

static LoRaTxMessageT loraTxMsg;

static void loraReadBuf(const uint8_t* buf, size_t size) {
  memcpy(&loraTxMsg, buf, size);
}

class LoRaHandler_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pSerial = new BufferSerial(256);
    pLoRaMock = new LoRaMock();
    pLH = new LoRaHandler(*pLoRaMock, *pSerial, 1, 2);
    strBuf[0] = '\0';
  }

  void TearDown() override {
    delete pLH;
    delete pLoRaMock;
    delete pSerial;
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

 public:
  LoRaMock* pLoRaMock;
};

TEST_F(LoRaHandler_test, begin) {
  EXPECT_CALL(*pLoRaMock, begin(868e6)).WillOnce(Return(1));

  pLH->begin();
}

TEST_F(LoRaHandler_test, loraRx_no_packet) {
  EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(0));

  EXPECT_EQ(pLH->loraRx(), 0);
}

// TODO: Examples of real packets
// TEST_F(LoRaHandler_test, loraRx_no_packet) {
//   EXPECT_CALL(*pLoRaMock, parsePacket(0)).WillOnce(Return(0));
//   EXPECT_EQ(pLH->loraRx(), 0);
// }

TEST_F(LoRaHandler_test, setDefaultHeader) {
  LoRaHeaderT header;

  pLH->setDefaultHeader(&header);

  EXPECT_EQ(header.dst, 1);
  EXPECT_EQ(header.src, 2);
  EXPECT_EQ(header.id, 0);
  EXPECT_EQ(header.flags, FLAGS_REQ_ACK);
  EXPECT_EQ(header.len, 0);
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

  pLH->beginValueMsg();
  pLH->addValueItem(reinterpret_cast<const uint8_t*>(&expectedItem1),
                    sizeof(expectedItem1));
  pLH->addValueItem(reinterpret_cast<const uint8_t*>(&expectedItem2),
                    sizeof(expectedItem2));
  pLH->endMsg();

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

  pLH->beginConfigsValueMsg(55);
  pLH->addConfigItemValues(reinterpret_cast<const uint8_t*>(&expectedItem1),
                           sizeof(expectedItem1));
  pLH->addConfigItemValues(reinterpret_cast<const uint8_t*>(&expectedItem2),
                           sizeof(expectedItem2));
  pLH->endMsg();

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
