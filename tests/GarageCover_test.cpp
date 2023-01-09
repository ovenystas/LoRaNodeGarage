#include "../GarageCover.h"

#include <gtest/gtest.h>

#include "../Unit.h"
#include "mocks/Arduino.h"

using ::testing::ElementsAre;
using ::testing::Return;

class GarageCover_test : public ::testing::Test {
 protected:
  void SetUp() override {
    arduinoMock = arduinoMockInstance();
    EXPECT_CALL(*arduinoMock, pinMode(11, INPUT_PULLUP));
    EXPECT_CALL(*arduinoMock, pinMode(12, INPUT_PULLUP));
    EXPECT_CALL(*arduinoMock, pinMode(13, OUTPUT));
    EXPECT_CALL(*arduinoMock, digitalWrite(13, LOW));
    pGc = new GarageCover(91, "GarageCover", 11, 12, 13);
  }

  void TearDown() override {
    delete pGc;
    releaseArduinoMock();
  }

  ArduinoMock* arduinoMock;
  GarageCover* pGc;
};

TEST_F(GarageCover_test, update_closing_closed_closed) {
  pGc->setState(Cover::State::closing);
  EXPECT_CALL(*arduinoMock, digitalRead(11))
      .WillOnce(Return(LOW))
      .WillOnce(Return(LOW));
  EXPECT_CALL(*arduinoMock, digitalRead(12))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_EQ(pGc->update(), true);
  EXPECT_EQ(pGc->getState(), Cover::State::closed);
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->getState(), Cover::State::closed);
}

TEST_F(GarageCover_test, update_opening_open_open) {
  pGc->setState(Cover::State::opening);
  EXPECT_CALL(*arduinoMock, digitalRead(11))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_CALL(*arduinoMock, digitalRead(12))
      .WillOnce(Return(LOW))
      .WillOnce(Return(LOW));
  EXPECT_EQ(pGc->update(), true);
  EXPECT_EQ(pGc->getState(), Cover::State::open);
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->getState(), Cover::State::open);
}

TEST_F(GarageCover_test, update_closed_opening_opening) {
  pGc->setState(Cover::State::closed);
  EXPECT_CALL(*arduinoMock, digitalRead(11))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_CALL(*arduinoMock, digitalRead(12))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_EQ(pGc->update(), true);
  EXPECT_EQ(pGc->getState(), Cover::State::opening);
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->getState(), Cover::State::opening);
}

TEST_F(GarageCover_test, update_open_closing_closing) {
  pGc->setState(Cover::State::open);
  EXPECT_CALL(*arduinoMock, digitalRead(11))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_CALL(*arduinoMock, digitalRead(12))
      .WillOnce(Return(HIGH))
      .WillOnce(Return(HIGH));
  EXPECT_EQ(pGc->update(), true);
  EXPECT_EQ(pGc->getState(), Cover::State::closing);
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->getState(), Cover::State::closing);
}

TEST_F(GarageCover_test, getDiscoveryMsg) {
  uint8_t buf[6] = {};
  EXPECT_EQ(pGc->getDiscoveryMsg(buf), 6);
  EXPECT_THAT(buf,
              ElementsAre(91, static_cast<uint8_t>(Component::Type::cover),
                          static_cast<uint8_t>(Cover::DeviceClass::garage),
                          static_cast<uint8_t>(Unit::Type::none), (1 << 4) | 0,
                          0));  // No config items
}

TEST_F(GarageCover_test, getDeviceClass) {
  EXPECT_EQ(pGc->getDeviceClass(), Cover::DeviceClass::garage);
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

void expectCalls_relayActivatedNoTimes(ArduinoMock* arduinoMock) {
  EXPECT_CALL(*arduinoMock, digitalWrite(13, HIGH)).Times(0);
  EXPECT_CALL(*arduinoMock, digitalWrite(13, LOW)).Times(0);
  EXPECT_CALL(*arduinoMock, delay(500)).Times(0);
}

void expectCalls_relayActivatedOneTime(ArduinoMock* arduinoMock) {
  EXPECT_CALL(*arduinoMock, digitalWrite(13, HIGH)).Times(1);
  EXPECT_CALL(*arduinoMock, digitalWrite(13, LOW)).Times(1);
  EXPECT_CALL(*arduinoMock, delay(500)).Times(1);
}

void expectCalls_relayActivatedTwoTimes(ArduinoMock* arduinoMock) {
  EXPECT_CALL(*arduinoMock, digitalWrite(13, HIGH)).Times(2);
  EXPECT_CALL(*arduinoMock, digitalWrite(13, LOW)).Times(2);
  EXPECT_CALL(*arduinoMock, delay(500)).Times(3);
}

TEST_F(GarageCover_test, callService_open_when_closed_then_relayOneTime) {
  pGc->setState(Cover::State::closed);
  expectCalls_relayActivatedOneTime(arduinoMock);
  pGc->callService(Cover::Service::open);
}

TEST_F(GarageCover_test, callService_open_when_closing_then_relayTwoTimes) {
  pGc->setState(Cover::State::closing);
  expectCalls_relayActivatedTwoTimes(arduinoMock);
  pGc->callService(Cover::Service::open);
}

TEST_F(GarageCover_test, callService_open_when_open_then_relayNoTime) {
  pGc->setState(Cover::State::open);
  expectCalls_relayActivatedNoTimes(arduinoMock);
  pGc->callService(Cover::Service::open);
}

TEST_F(GarageCover_test, callService_open_when_opening_then_relayNoTime) {
  pGc->setState(Cover::State::opening);
  expectCalls_relayActivatedNoTimes(arduinoMock);
  pGc->callService(Cover::Service::open);
}

TEST_F(GarageCover_test, callService_close_when_open_then_relayOneTime) {
  pGc->setState(Cover::State::open);
  expectCalls_relayActivatedOneTime(arduinoMock);
  pGc->callService(Cover::Service::close);
}

TEST_F(GarageCover_test, callService_close_when_opening_then_relayTwoTimes) {
  pGc->setState(Cover::State::opening);
  expectCalls_relayActivatedTwoTimes(arduinoMock);
  pGc->callService(Cover::Service::close);
}

TEST_F(GarageCover_test, callService_close_when_closed_then_relayNoTime) {
  pGc->setState(Cover::State::open);
  expectCalls_relayActivatedNoTimes(arduinoMock);
  pGc->callService(Cover::Service::open);
}

TEST_F(GarageCover_test, callService_close_when_closing_then_relayNoTime) {
  pGc->setState(Cover::State::closing);
  expectCalls_relayActivatedNoTimes(arduinoMock);
  pGc->callService(Cover::Service::close);
}

TEST_F(GarageCover_test, callService_stop_when_opening_then_relayOneTime) {
  pGc->setState(Cover::State::opening);
  expectCalls_relayActivatedOneTime(arduinoMock);
  pGc->callService(Cover::Service::stop);
}

TEST_F(GarageCover_test, callService_stop_when_closing_then_relayOneTime) {
  pGc->setState(Cover::State::closing);
  expectCalls_relayActivatedOneTime(arduinoMock);
  pGc->callService(Cover::Service::stop);
}

TEST_F(GarageCover_test, callService_stop_when_closed_then_relayNoTime) {
  pGc->setState(Cover::State::closed);
  expectCalls_relayActivatedNoTimes(arduinoMock);
  pGc->callService(Cover::Service::stop);
}

TEST_F(GarageCover_test, callService_stop_when_open_then_relayNoTime) {
  pGc->setState(Cover::State::open);
  expectCalls_relayActivatedNoTimes(arduinoMock);
  pGc->callService(Cover::Service::stop);
}

TEST_F(GarageCover_test, callService_toggle_when_open_then_relayOneTime) {
  pGc->setState(Cover::State::open);
  expectCalls_relayActivatedOneTime(arduinoMock);
  pGc->callService(Cover::Service::toggle);
}

TEST_F(GarageCover_test, callService_toggle_when_closed_then_relayOneTime) {
  pGc->setState(Cover::State::closed);
  expectCalls_relayActivatedOneTime(arduinoMock);
  pGc->callService(Cover::Service::toggle);
}

TEST_F(GarageCover_test, callService_toggle_when_opening_then_relayOwoTimes) {
  pGc->setState(Cover::State::opening);
  expectCalls_relayActivatedTwoTimes(arduinoMock);
  pGc->callService(Cover::Service::toggle);
}

TEST_F(GarageCover_test, callService_toggle_when_closing_then_relayOwoTimes) {
  pGc->setState(Cover::State::closing);
  expectCalls_relayActivatedTwoTimes(arduinoMock);
  pGc->callService(Cover::Service::toggle);
}
