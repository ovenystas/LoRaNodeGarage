#include "Cover.h"

#include <gtest/gtest.h>
#include <string.h>

#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::Return;

char strBuf[256];

size_t appendStrBuf(const char* str) {
  strcat(strBuf, str);
  return (strlen(str));
}

class Cover_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pSerial = new BufferSerial(256);
    pC = new Cover(34, "Cover");
    strBuf[0] = '\0';
  }

  void TearDown() override {
    delete pC;
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
  Cover* pC;
};

TEST_F(Cover_test, getState) {
  EXPECT_EQ(pC->getState(), CoverState::closed);
  pC->setState(CoverState::opening);
  EXPECT_EQ(pC->getState(), CoverState::opening);
  pC->setState(CoverState::open);
  EXPECT_EQ(pC->getState(), CoverState::open);
  pC->setState(CoverState::closing);
  EXPECT_EQ(pC->getState(), CoverState::closing);
}

TEST_F(Cover_test, getStateName_of_current_state) {
  EXPECT_STREQ(pC->getStateName(), "closed");
  pC->setState(CoverState::opening);
  EXPECT_STREQ(pC->getStateName(), "opening");
  pC->setState(CoverState::open);
  EXPECT_STREQ(pC->getStateName(), "open");
  pC->setState(CoverState::closing);
  EXPECT_STREQ(pC->getStateName(), "closing");
}

TEST_F(Cover_test, getStateName_of_state_as_arg) {
  EXPECT_STREQ(pC->getStateName(CoverState::closed), "closed");
  EXPECT_STREQ(pC->getStateName(CoverState::opening), "opening");
  EXPECT_STREQ(pC->getStateName(CoverState::open), "open");
  EXPECT_STREQ(pC->getStateName(CoverState::closing), "closing");
}

TEST_F(Cover_test, getServiceName_of_service_as_arg) {
  EXPECT_STREQ(pC->getServiceName(CoverService::close), "close");
  EXPECT_STREQ(pC->getServiceName(CoverService::open), "open");
  EXPECT_STREQ(pC->getServiceName(CoverService::stop), "stop");
  EXPECT_STREQ(pC->getServiceName(CoverService::toggle), "toggle");
}

TEST_F(Cover_test, getComponentType) {
  EXPECT_EQ(pC->getComponentType(), BaseComponent::Type::cover);
}

TEST_F(Cover_test, getDeviceClass) {
  EXPECT_EQ(pC->getDeviceClass(), CoverDeviceClass::none);
}

TEST_F(Cover_test, getDiscoveryMsg) {
  uint8_t buf[5] = {};
  EXPECT_EQ(pC->getDiscoveryMsg(buf), 5);
  EXPECT_THAT(
      buf, ElementsAre(34, static_cast<uint8_t>(BaseComponent::Type::cover),
                       static_cast<uint8_t>(CoverDeviceClass::none),
                       static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0));
}

TEST_F(Cover_test, getValueMsg) {
  uint8_t buf[2] = {};
  EXPECT_EQ(pC->getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(CoverState::closed)));
  pC->setState(CoverState::opening);
  EXPECT_EQ(pC->getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(CoverState::opening)));
  pC->setState(CoverState::open);
  EXPECT_EQ(pC->getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(CoverState::open)));
  pC->setState(CoverState::closing);
  EXPECT_EQ(pC->getValueMsg(buf), 2);
  EXPECT_THAT(buf, ElementsAre(34, static_cast<uint8_t>(CoverState::closing)));
}

TEST_F(Cover_test, setReported) {
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

  size_t printedChars = pC->print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_opening) {
  const char* expectStr = "Cover: opening";
  pC->setState(CoverState::opening);

  size_t printedChars = pC->print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_open) {
  const char* expectStr = "Cover: open";
  pC->setState(CoverState::open);

  size_t printedChars = pC->print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_closing) {
  const char* expectStr = "Cover: closing";
  pC->setState(CoverState::closing);

  size_t printedChars = pC->print(*pSerial);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_closed_and_service_open_is_called) {
  const char* expectStr = "Cover: Service open called when in state closed";
  pC->setState(CoverState::closed);

  size_t printedChars =
      pC->print(*pSerial, static_cast<uint8_t>(CoverService::open));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_open_and_service_close_is_called) {
  const char* expectStr = "Cover: Service close called when in state open";
  pC->setState(CoverState::open);

  size_t printedChars =
      pC->print(*pSerial, static_cast<uint8_t>(CoverService::close));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_opening_and_service_stop_is_called) {
  const char* expectStr = "Cover: Service stop called when in state opening";
  pC->setState(CoverState::opening);

  size_t printedChars =
      pC->print(*pSerial, static_cast<uint8_t>(CoverService::stop));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_closing_and_service_toggle_is_called) {
  const char* expectStr = "Cover: Service toggle called when in state closing";
  pC->setState(CoverState::closing);

  size_t printedChars =
      pC->print(*pSerial, static_cast<uint8_t>(CoverService::toggle));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}

TEST_F(Cover_test, print_when_state_is_closed_and_service_unknown_is_called) {
  const char* expectStr = "Cover: Service unknown called when in state closed";
  pC->setState(CoverState::closed);

  size_t printedChars =
      pC->print(*pSerial, static_cast<uint8_t>(CoverService::unknown));

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
  EXPECT_EQ(printedChars, strlen(expectStr));
}
