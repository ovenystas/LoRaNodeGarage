#include "GarageCover.h"

#include <gtest/gtest.h>

#include "Unit.h"
#include "mocks/Arduino.h"
#include "mocks/BufferSerial.h"

using ::testing::Return;

class GarageCover_test : public ::testing::Test {
 protected:
  void SetUp() override {
    strBuf[0] = '\0';
    pSerial = new BufferSerial(256);
    pArduinoMock = arduinoMockInstance();
    EXPECT_CALL(*pArduinoMock, pinMode(11, INPUT_PULLUP));
    EXPECT_CALL(*pArduinoMock, pinMode(12, INPUT_PULLUP));
    EXPECT_CALL(*pArduinoMock, pinMode(13, OUTPUT));
    EXPECT_CALL(*pArduinoMock, digitalWrite(13, LOW));
    eeprom_clear();

    pGc = new GarageCover(91, "GarageCover", 11, 12, 13);
  }

  void TearDown() override {
    delete pGc;
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
  ArduinoMock* pArduinoMock;
  GarageCover* pGc;
};

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

TEST_F(GarageCover_test, callService_unknown) {
  pGc->mCover.setState(CoverState::closed);
  expectCalls_relayActivatedNoTimes(pArduinoMock);
  pGc->callService(4);  // unknown
}

TEST_F(GarageCover_test, getConfigItemValuesMsg) {
  ConfigItemValueT items[1];

  EXPECT_EQ(pGc->getConfigItemValues(items, sizeof(items) / sizeof(items[0])),
            1);

  EXPECT_EQ(items[0].configId, 0);
  EXPECT_EQ(items[0].value,
            GarageCoverConstants::CONFIG_REPORT_INTERVAL_DEFAULT);
}

TEST_F(GarageCover_test, getDeviceClass) {
  EXPECT_EQ(pGc->mCover.getDeviceClass(), CoverDeviceClass::garage);
}

TEST_F(GarageCover_test, getDiscoveryItem) {
  DiscoveryItemT item;

  pGc->getDiscoveryItem(&item);

  EXPECT_EQ(item.entity.entityId, 91);
  EXPECT_EQ(item.entity.componentType,
            static_cast<uint8_t>(BaseComponent::Type::cover));
  EXPECT_EQ(item.entity.deviceClass,
            static_cast<uint8_t>(CoverDeviceClass::garage));
  EXPECT_EQ(item.entity.unit, static_cast<uint8_t>(Unit::Type::none));
  EXPECT_FALSE(item.entity.isSigned);
  EXPECT_EQ(item.entity.sizeCode, 0);
  EXPECT_EQ(item.entity.precision, 0);

  EXPECT_EQ(item.numberOfConfigItems, 1);

  EXPECT_EQ(item.configItems[0].configId, 0);
  EXPECT_EQ(item.configItems[0].unit, static_cast<uint8_t>(Unit::Type::s));
  EXPECT_FALSE(item.configItems[0].isSigned);
  EXPECT_EQ(item.configItems[0].sizeCode, sizeof(uint16_t) / 2);
  EXPECT_EQ(item.configItems[0].precision, 0);
}

TEST_F(GarageCover_test, getEntityId) { EXPECT_EQ(pGc->getEntityId(), 91); }

TEST_F(GarageCover_test, getValueItem) {
  ValueItemT item;

  pGc->getValueItem(&item);

  EXPECT_EQ(item.entityId, 91);
  EXPECT_EQ(item.value, 0);
}

TEST_F(GarageCover_test, print) {
  const char* expectStr = "GarageCover: closed";
  EXPECT_CALL(*pArduinoMock, pinMode(11, INPUT_PULLUP));
  EXPECT_CALL(*pArduinoMock, pinMode(12, INPUT_PULLUP));
  EXPECT_CALL(*pArduinoMock, pinMode(13, OUTPUT));
  EXPECT_CALL(*pArduinoMock, digitalWrite(13, LOW));
  GarageCover gc = GarageCover(91, "GarageCover", 11, 12, 13);

  EXPECT_EQ(gc.print(*pSerial), 19);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

TEST_F(GarageCover_test, print_service_open) {
  const char* expectStr =
      "GarageCover: Service open called when in state closed";
  EXPECT_CALL(*pArduinoMock, pinMode(11, INPUT_PULLUP));
  EXPECT_CALL(*pArduinoMock, pinMode(12, INPUT_PULLUP));
  EXPECT_CALL(*pArduinoMock, pinMode(13, OUTPUT));
  EXPECT_CALL(*pArduinoMock, digitalWrite(13, LOW));
  GarageCover gc = GarageCover(91, "GarageCover", 11, 12, 13);

  EXPECT_EQ(gc.print(*pSerial, static_cast<uint8_t>(CoverService::open)), 53);

  bufSerReadStr();
  EXPECT_STREQ(strBuf, expectStr);
}

// No config items
TEST_F(GarageCover_test, setConfigs) {
  const ConfigItemValueT item = {1, 0xABBACAFE};
  EXPECT_FALSE(pGc->setConfigItemValues(&item, 1));
}

TEST_F(GarageCover_test, setReported) {
  EXPECT_CALL(*pArduinoMock, millis()).Times(1);
  pGc->setReported();
}

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

TEST_F(GarageCover_test, update_closed_both_pins_low_shall_stay_in_same_state) {
  pGc->mCover.setState(CoverState::closed);
  EXPECT_CALL(*pArduinoMock, digitalRead(11)).WillOnce(Return(LOW));
  EXPECT_CALL(*pArduinoMock, digitalRead(12)).WillOnce(Return(LOW));
  EXPECT_EQ(pGc->update(), false);
  EXPECT_EQ(pGc->mCover.getState(), CoverState::closed);
}

TEST_F(GarageCover_test, isReportDue) {
  EXPECT_CALL(*pArduinoMock, millis()).WillOnce(Return(0));

  // Newly constructed shall be true
  EXPECT_TRUE(pGc->isReportDue());

  // Shall be set to false when setReported() is called
  pGc->setReported();
  EXPECT_FALSE(pGc->isReportDue());
}
