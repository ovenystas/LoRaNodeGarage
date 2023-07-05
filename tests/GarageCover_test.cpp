#include "GarageCover.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"

using ::testing::ElementsAre;
using ::testing::Return;

class GarageCover_test : public ::testing::Test {
 protected:
  void SetUp() override {
    pArduinoMock = arduinoMockInstance();
    EXPECT_CALL(*pArduinoMock, pinMode(11, INPUT_PULLUP));
    EXPECT_CALL(*pArduinoMock, pinMode(12, INPUT_PULLUP));
    EXPECT_CALL(*pArduinoMock, pinMode(13, OUTPUT));
    EXPECT_CALL(*pArduinoMock, digitalWrite(13, LOW));
    pGc = new GarageCover(91, "GarageCover", 11, 12, 13);
  }

  void TearDown() override {
    delete pGc;
    releaseArduinoMock();
  }

  ArduinoMock* pArduinoMock;
  GarageCover* pGc;
};

TEST_F(GarageCover_test, update_closing_closed_closed) {
  pGc->mCover.setState(CoverState::closing);
  EXPECT_CALL(*pArduinoMock, digitalRead(11))
      .WillOnce(Return(LOW))
      .WillOnce(Return(LOW));
  EXPECT_CALL(*pArduinoMock, digitalRead(12))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_EQ(pGc->update(), true);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::closed);
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::closed);
}

TEST_F(GarageCover_test, update_opening_open_open) {
  pGc->mCover.setState(CoverState::opening);
  EXPECT_CALL(*pArduinoMock, digitalRead(11))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_CALL(*pArduinoMock, digitalRead(12))
      .WillOnce(Return(LOW))
      .WillOnce(Return(LOW));
  EXPECT_EQ(pGc->update(), true);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::open);
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::open);
}

TEST_F(GarageCover_test, update_closed_opening_opening) {
  pGc->mCover.setState(CoverState::closed);
  EXPECT_CALL(*pArduinoMock, digitalRead(11))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_CALL(*pArduinoMock, digitalRead(12))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_EQ(pGc->update(), true);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::opening);
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::opening);
}

TEST_F(GarageCover_test, update_open_closing_closing) {
  pGc->mCover.setState(CoverState::open);
  EXPECT_CALL(*pArduinoMock, digitalRead(11))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_CALL(*pArduinoMock, digitalRead(12))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_EQ(pGc->update(), true);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::closing);
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::closing);
}

TEST_F(GarageCover_test, getDiscoveryMsg) {
  uint8_t buf[6] = {};
  EXPECT_EQ(pGc->getDiscoveryMsg(buf), 6);
  EXPECT_THAT(buf,
              ElementsAre(91, static_cast<uint8_t>(BaseComponent::Type::cover),
                          static_cast<uint8_t>(CoverDeviceClass::garage),
                          static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0,
                          0));  // No config items
}

TEST_F(GarageCover_test, getDeviceClass) {
  EXPECT_EQ(pGc->mCover.getDeviceClass(), CoverDeviceClass::garage);
}

// No config items
TEST_F(GarageCover_test, getConfigItemValuesMsg) {
  EXPECT_EQ(pGc->getConfigItemValuesMsg(nullptr), 0);
}

// No config items
TEST_F(GarageCover_test, setConfigs) {
  pGc->setConfigs(0, nullptr);
  SUCCEED();
}

void expectCalls_relayActivatedNoTimes(ArduinoMock* pArduinoMock) {
  EXPECT_CALL(*pArduinoMock, digitalWrite(13, HIGH)).Times(0);
  EXPECT_CALL(*pArduinoMock, digitalWrite(13, LOW)).Times(0);
  EXPECT_CALL(*pArduinoMock, delay(500)).Times(0);
}

void expectCalls_relayActivatedOneTime(ArduinoMock* pArduinoMock) {
  EXPECT_CALL(*pArduinoMock, digitalWrite(13, HIGH)).Times(1);
  EXPECT_CALL(*pArduinoMock, digitalWrite(13, LOW)).Times(1);
  EXPECT_CALL(*pArduinoMock, delay(500)).Times(1);
}

void expectCalls_relayActivatedTwoTimes(ArduinoMock* pArduinoMock) {
  EXPECT_CALL(*pArduinoMock, digitalWrite(13, HIGH)).Times(2);
  EXPECT_CALL(*pArduinoMock, digitalWrite(13, LOW)).Times(2);
  EXPECT_CALL(*pArduinoMock, delay(500)).Times(3);
}

TEST_F(GarageCover_test, callService_open_when_closed_then_relayOneTime) {
  pGc->mCover.setState(CoverState::closed);
  expectCalls_relayActivatedOneTime(pArduinoMock);
  pGc->callService(0);  // open
}

TEST_F(GarageCover_test, callService_open_when_closing_then_relayTwoTimes) {
  pGc->mCover.setState(CoverState::closing);
  expectCalls_relayActivatedTwoTimes(pArduinoMock);
  pGc->callService(0);  // open
}

TEST_F(GarageCover_test, callService_open_when_open_then_relayNoTime) {
  pGc->mCover.setState(CoverState::open);
  expectCalls_relayActivatedNoTimes(pArduinoMock);
  pGc->callService(0);  // open
}

TEST_F(GarageCover_test, callService_open_when_opening_then_relayNoTime) {
  pGc->mCover.setState(CoverState::opening);
  expectCalls_relayActivatedNoTimes(pArduinoMock);
  pGc->callService(0);  // open
}

TEST_F(GarageCover_test, callService_close_when_open_then_relayOneTime) {
  pGc->mCover.setState(CoverState::open);
  expectCalls_relayActivatedOneTime(pArduinoMock);
  pGc->callService(1);  // close
}

TEST_F(GarageCover_test, callService_close_when_opening_then_relayTwoTimes) {
  pGc->mCover.setState(CoverState::opening);
  expectCalls_relayActivatedTwoTimes(pArduinoMock);
  pGc->callService(1);  // close
}

TEST_F(GarageCover_test, callService_close_when_closed_then_relayNoTime) {
  pGc->mCover.setState(CoverState::closed);
  expectCalls_relayActivatedNoTimes(pArduinoMock);
  pGc->callService(1);  // close
}

TEST_F(GarageCover_test, callService_close_when_closing_then_relayNoTime) {
  pGc->mCover.setState(CoverState::closing);
  expectCalls_relayActivatedNoTimes(pArduinoMock);
  pGc->callService(1);  // close
}

TEST_F(GarageCover_test, callService_stop_when_opening_then_relayOneTime) {
  pGc->mCover.setState(CoverState::opening);
  expectCalls_relayActivatedOneTime(pArduinoMock);
  pGc->callService(2);  // stop
}

TEST_F(GarageCover_test, callService_stop_when_closing_then_relayOneTime) {
  pGc->mCover.setState(CoverState::closing);
  expectCalls_relayActivatedOneTime(pArduinoMock);
  pGc->callService(2);  // stop
}

TEST_F(GarageCover_test, callService_stop_when_closed_then_relayNoTime) {
  pGc->mCover.setState(CoverState::closed);
  expectCalls_relayActivatedNoTimes(pArduinoMock);
  pGc->callService(2);  // stop
}

TEST_F(GarageCover_test, callService_stop_when_open_then_relayNoTime) {
  pGc->mCover.setState(CoverState::open);
  expectCalls_relayActivatedNoTimes(pArduinoMock);
  pGc->callService(2);  // stop
}

TEST_F(GarageCover_test, callService_toggle_when_open_then_relayOneTime) {
  pGc->mCover.setState(CoverState::open);
  expectCalls_relayActivatedOneTime(pArduinoMock);
  pGc->callService(3);  // toggle
}

TEST_F(GarageCover_test, callService_toggle_when_closed_then_relayOneTime) {
  pGc->mCover.setState(CoverState::closed);
  expectCalls_relayActivatedOneTime(pArduinoMock);
  pGc->callService(3);  // toggle
}

TEST_F(GarageCover_test, callService_toggle_when_opening_then_relayOwoTimes) {
  pGc->mCover.setState(CoverState::opening);
  expectCalls_relayActivatedTwoTimes(pArduinoMock);
  pGc->callService(3);  // toggle
}

TEST_F(GarageCover_test, callService_toggle_when_closing_then_relayOwoTimes) {
  pGc->mCover.setState(CoverState::closing);
  expectCalls_relayActivatedTwoTimes(pArduinoMock);
  pGc->callService(3);  // toggle
}
