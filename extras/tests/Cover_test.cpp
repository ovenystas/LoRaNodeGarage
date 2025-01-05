#include "Cover.h"

#include <gtest/gtest.h>
#include <string.h>

#include "Types.h"
#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class Cover_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pC = new Cover(34, "Cover");
    strBuf[0] = '\0';
  }

  void TearDown() override { delete pC; }

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
  Cover* pC;
};

TEST_F(Cover_test, getComponentType) {
  EXPECT_EQ(pC->getComponentType(), BaseComponent::Type::cover);
}

TEST_F(Cover_test, getDeviceClass) {
  EXPECT_EQ(pC->getDeviceClass(), CoverDeviceClass::none);
}

TEST_F(Cover_test, getDiscoveryEntityItem) {
  DiscoveryEntityItemT item;

  pC->getDiscoveryEntityItem(&item);

  EXPECT_EQ(item.entityId, 34);
  EXPECT_EQ(item.componentType,
            static_cast<uint8_t>(BaseComponent::Type::cover));
  EXPECT_EQ(item.deviceClass, static_cast<uint8_t>(CoverDeviceClass::none));
  EXPECT_EQ(item.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_FALSE(item.isSigned);
  EXPECT_EQ(item.sizeCode, 0);
  EXPECT_EQ(item.precision, 0);
}

TEST_F(Cover_test, getEntityId) { EXPECT_EQ(pC->getEntityId(), 34); }

TEST_F(Cover_test, getState) {
  EXPECT_EQ(pC->getState(), CoverState::closed);

  pC->setState(CoverState::opening);
  EXPECT_EQ(pC->getState(), CoverState::opening);

  pC->setState(CoverState::open);
  EXPECT_EQ(pC->getState(), CoverState::open);

  pC->setState(CoverState::closing);
  EXPECT_EQ(pC->getState(), CoverState::closing);
}

TEST_F(Cover_test, getValueItem) {
  ValueItemT item;
  pC->setState(CoverState::closed);
  pC->getValueItem(&item);
  EXPECT_EQ(item.entityId, 34);
  EXPECT_EQ(item.value, static_cast<uint32_t>(CoverState::closed));

  pC->setState(CoverState::opening);
  pC->getValueItem(&item);
  EXPECT_EQ(item.entityId, 34);
  EXPECT_EQ(item.value, static_cast<uint32_t>(CoverState::opening));

  pC->setState(CoverState::open);
  pC->getValueItem(&item);
  EXPECT_EQ(item.entityId, 34);
  EXPECT_EQ(item.value, static_cast<uint32_t>(CoverState::open));

  pC->setState(CoverState::closing);
  pC->getValueItem(&item);
  EXPECT_EQ(item.entityId, 34);
  EXPECT_EQ(item.value, static_cast<uint32_t>(CoverState::closing));
}

TEST_F(Cover_test, setReported_isDiffLastReportedState_timeSinceLastReport) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis())
      .WillOnce(Return(0))
      .WillOnce(Return(10000))
      .WillOnce(Return(20500))
      .WillOnce(Return(35999));
  pC->setReported();
  EXPECT_EQ(pC->isDiffLastReportedState(), false);
  EXPECT_EQ(pC->timeSinceLastReport(), 10);
  pC->setState(CoverState::opening);
  EXPECT_EQ(pC->isDiffLastReportedState(), true);
  pC->setReported();
  EXPECT_EQ(pC->timeSinceLastReport(), 15);
  releaseArduinoMock();
}

TEST_F(Cover_test, print_when_state_is_closed) {
  const char* expectStr = "Cover: closed";
  pC->setState(CoverState::closed);

  size_t printedChars = pC->printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_opening) {
  const char* expectStr = "Cover: opening";
  pC->setState(CoverState::opening);

  size_t printedChars = pC->printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_open) {
  const char* expectStr = "Cover: open";
  pC->setState(CoverState::open);

  size_t printedChars = pC->printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_closing) {
  const char* expectStr = "Cover: closing";
  pC->setState(CoverState::closing);

  size_t printedChars = pC->printTo(Serial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_closed_and_service_open_is_called) {
  const char* expectStr = "Cover: Service open called when in state closed";
  pC->setState(CoverState::closed);

  size_t printedChars =
      pC->printTo(Serial, static_cast<uint8_t>(CoverService::open));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_open_and_service_close_is_called) {
  const char* expectStr = "Cover: Service close called when in state open";
  pC->setState(CoverState::open);

  size_t printedChars =
      pC->printTo(Serial, static_cast<uint8_t>(CoverService::close));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_opening_and_service_stop_is_called) {
  const char* expectStr = "Cover: Service stop called when in state opening";
  pC->setState(CoverState::opening);

  size_t printedChars =
      pC->printTo(Serial, static_cast<uint8_t>(CoverService::stop));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_closing_and_service_toggle_is_called) {
  const char* expectStr = "Cover: Service toggle called when in state closing";
  pC->setState(CoverState::closing);

  size_t printedChars =
      pC->printTo(Serial, static_cast<uint8_t>(CoverService::toggle));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_closed_and_service_unknown_is_called) {
  const char* expectStr = "Cover: Service unknown called when in state closed";
  pC->setState(CoverState::closed);

  size_t printedChars =
      pC->printTo(Serial, static_cast<uint8_t>(CoverService::unknown));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, serviceDecode) {
  EXPECT_EQ(pC->serviceDecode(0), CoverService::open);
  EXPECT_EQ(pC->serviceDecode(1), CoverService::close);
  EXPECT_EQ(pC->serviceDecode(2), CoverService::stop);
  EXPECT_EQ(pC->serviceDecode(3), CoverService::toggle);
  EXPECT_EQ(pC->serviceDecode(4), CoverService::unknown);
  EXPECT_EQ(pC->serviceDecode(5), CoverService::unknown);
}

TEST_F(Cover_test, isReportDue) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, millis()).WillOnce(Return(0));

  // Newly constructed shall be true
  EXPECT_TRUE(pC->isReportDue());

  // Shall be set to false when setReported() is called
  pC->setReported();
  EXPECT_FALSE(pC->isReportDue());

  // Shall set it with setIsReportDue()
  pC->setIsReportDue(true);
  EXPECT_TRUE(pC->isReportDue());
  pC->setIsReportDue(false);
  EXPECT_FALSE(pC->isReportDue());

  releaseArduinoMock();
}
