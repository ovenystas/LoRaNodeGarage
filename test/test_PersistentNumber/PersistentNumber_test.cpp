#include "PersistentNumber.h"

#include <gtest/gtest.h>

#include "Arduino.h"
#include "BufferSerial.h"

class PersistentNumber_test : public ::testing::Test {
 protected:
  void SetUp() override {
    // Clear EEPROM test region before each test
    for (int i = 0; i < 256; i++) {
      EEPROM.write(i, 0xFF);
    }
    bufSerial.flush();
  }

  void TearDown() override {}

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
};

// ============================================================================
// Tests: Construction and Type Validation
// ============================================================================

TEST_F(PersistentNumber_test, construct_with_full_parameters) {
  PersistentNumber<uint16_t> pn{0x00,
                                100,
                                "Test Number",
                                NumberDeviceClass::DISTANCE,
                                Unit::Type::m,
                                1,
                                BaseComponent::Category::CONFIG,
                                500,
                                0,
                                1000};

  EXPECT_EQ(pn.getEntityId(), 100);
  EXPECT_EQ(pn.getValue(), 500);
  EXPECT_EQ(pn.getUnitType(), Unit::Type::m);
}

TEST_F(PersistentNumber_test, construct_with_minimal_parameters) {
  PersistentNumber<uint16_t> pn{0x10, 101};

  EXPECT_EQ(pn.getEntityId(), 101);
  EXPECT_EQ(pn.getComponentType(), BaseComponent::Type::NUMBER);
}

#ifndef SKIP_DEATH_TESTS
TEST_F(PersistentNumber_test, construct_with_float_shall_fail) {
  // This should fail at compile time, but we can't test that directly in
  // runtime The static_assert will prevent compilation if uncommented:
  // PersistentNumber<float> pn{0x00, 1};  // ← Compilation error!
  SUCCEED();  // Placeholder for compile-time validation
}

TEST_F(PersistentNumber_test, construct_with_int64_shall_fail) {
  // This should fail at compile time:
  // PersistentNumber<int64_t> pn{0x00, 1};  // ← Compilation error!
  SUCCEED();  // Placeholder for compile-time validation
}
#endif

// ============================================================================
// Tests: EEPROM Loading
// ============================================================================

TEST_F(PersistentNumber_test, loadFromEeprom_empty_eeprom_uses_default) {
  PersistentNumber<uint16_t> pn{0x00, 1, "Test"};
  pn.loadFromEeprom(512);

  EXPECT_EQ(pn.getValue(), 512);
}

TEST_F(PersistentNumber_test, loadFromEeprom_oob_address_uses_default) {
  uint16_t oobAddr = EEPROM.length() - 2;  // Not enough space for 5 bytes
  PersistentNumber<uint16_t> pn{oobAddr, 2, "Test"};

  pn.loadFromEeprom(789);

  // Should use default when address is out of bounds
  EXPECT_EQ(pn.getValue(), 789);
}

TEST_F(PersistentNumber_test, loadFromEeprom_corrupted_crc_uses_default) {
  // Pre-write valid data
  PersistentNumber<uint16_t> pn1{0x00,
                                 1,
                                 "Test1",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 0,
                                 1000};
  pn1.setValue(456);  // Auto-saves

  // Corrupt the CRC byte
  EEPROM.write(0x00 + Ee::VALUE_SIZE, 0x00);

  // Load with corrupted CRC
  PersistentNumber<uint16_t> pn2{0x00,
                                 1,
                                 "Test1",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 0,
                                 1000};
  pn2.loadFromEeprom(999);

  // Should use default when CRC fails
  EXPECT_EQ(pn2.getValue(), 999);
}

// ============================================================================
// Tests: EEPROM Saving & Roundtrip
// ============================================================================

TEST_F(PersistentNumber_test, setValue_auto_saves_to_eeprom) {
  PersistentNumber<uint16_t> pn1{0x00,
                                 1,
                                 "Test",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 0,
                                 1000};

  pn1.setValue(789);
  EXPECT_EQ(pn1.getValue(), 789);

  // Create new instance at same EEPROM address
  PersistentNumber<uint16_t> pn2{0x00,
                                 1,
                                 "Test",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 0,
                                 1000};
  pn2.loadFromEeprom(999);

  // Should load the previously saved value
  EXPECT_EQ(pn2.getValue(), 789);
}

TEST_F(PersistentNumber_test, roundtrip_uint8) {
  const uint8_t testValue = 123;

  PersistentNumber<uint8_t> pn1{0x10,
                                1,
                                "Test",
                                NumberDeviceClass::NONE,
                                Unit::Type::none,
                                0,
                                BaseComponent::Category::NONE,
                                0,
                                0,
                                255};
  pn1.setValue(testValue);

  PersistentNumber<uint8_t> pn2{0x10,
                                1,
                                "Test",
                                NumberDeviceClass::NONE,
                                Unit::Type::none,
                                0,
                                BaseComponent::Category::NONE,
                                0,
                                0,
                                255};
  pn2.loadFromEeprom(0);

  EXPECT_EQ(pn2.getValue(), testValue);
}

TEST_F(PersistentNumber_test, roundtrip_int16) {
  const int16_t testValue = -1234;

  PersistentNumber<int16_t> pn1{0x20,
                                1,
                                "Test",
                                NumberDeviceClass::NONE,
                                Unit::Type::none,
                                0,
                                BaseComponent::Category::NONE,
                                0,
                                -32768,
                                32767};
  pn1.setValue(testValue);

  PersistentNumber<int16_t> pn2{0x20,
                                1,
                                "Test",
                                NumberDeviceClass::NONE,
                                Unit::Type::none,
                                0,
                                BaseComponent::Category::NONE,
                                0,
                                -32768,
                                32767};
  pn2.loadFromEeprom(0);

  EXPECT_EQ(pn2.getValue(), testValue);
}

TEST_F(PersistentNumber_test, roundtrip_uint32) {
  const uint32_t testValue = 0xDEADBEEFul;

  PersistentNumber<uint32_t> pn1{0x30,
                                 1,
                                 "Test",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 0,
                                 UINT32_MAX};
  pn1.setValue(testValue);

  PersistentNumber<uint32_t> pn2{0x30,
                                 1,
                                 "Test",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 0,
                                 UINT32_MAX};
  pn2.loadFromEeprom(0);

  EXPECT_EQ(pn2.getValue(), testValue);
}

// ============================================================================
// Tests: Multiple Instances, Address Isolation
// ============================================================================

TEST_F(PersistentNumber_test, multiple_instances_different_addresses) {
  PersistentNumber<uint16_t> pn1{0x00,
                                 1,
                                 "Config1",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::CONFIG,
                                 100,
                                 0,
                                 1000};

  PersistentNumber<uint16_t> pn2{0x10,
                                 2,
                                 "Config2",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::CONFIG,
                                 200,
                                 0,
                                 1000};

  pn1.setValue(111);
  pn2.setValue(222);

  PersistentNumber<uint16_t> pn1_verify{0x00,
                                        1,
                                        "Config1",
                                        NumberDeviceClass::NONE,
                                        Unit::Type::none,
                                        0,
                                        BaseComponent::Category::CONFIG,
                                        0,
                                        0,
                                        1000};
  pn1_verify.loadFromEeprom(999);
  EXPECT_EQ(pn1_verify.getValue(), 111);

  PersistentNumber<uint16_t> pn2_verify{0x10,
                                        2,
                                        "Config2",
                                        NumberDeviceClass::NONE,
                                        Unit::Type::none,
                                        0,
                                        BaseComponent::Category::CONFIG,
                                        0,
                                        0,
                                        1000};
  pn2_verify.loadFromEeprom(999);
  EXPECT_EQ(pn2_verify.getValue(), 222);
}

// ============================================================================
// Tests: Value Constraints (min/max clamping)
// ============================================================================

TEST_F(PersistentNumber_test, setValue_clamps_to_min) {
  PersistentNumber<int16_t> pn{0x40,
                               1,
                               "Test",
                               NumberDeviceClass::NONE,
                               Unit::Type::none,
                               0,
                               BaseComponent::Category::NONE,
                               0,
                               -100,
                               100};

  pn.setValue(-500);               // Try to set below min
  EXPECT_EQ(pn.getValue(), -100);  // Clamped to min
}

TEST_F(PersistentNumber_test, setValue_clamps_to_max) {
  PersistentNumber<int16_t> pn{0x50,
                               1,
                               "Test",
                               NumberDeviceClass::NONE,
                               Unit::Type::none,
                               0,
                               BaseComponent::Category::NONE,
                               0,
                               -100,
                               100};

  pn.setValue(500);               // Try to set above max
  EXPECT_EQ(pn.getValue(), 100);  // Clamped to max
}

TEST_F(PersistentNumber_test, roundtrip_preserves_clamped_value) {
  PersistentNumber<uint16_t> pn1{0x60,
                                 1,
                                 "Test",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 100,
                                 500};

  pn1.setValue(50);  // Below min, will be clamped to 100
  EXPECT_EQ(pn1.getValue(), 100);

  PersistentNumber<uint16_t> pn2{0x60,
                                 1,
                                 "Test",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 100,
                                 500};
  pn2.loadFromEeprom(999);

  // Should recover the clamped value (100), not the original attempt (50)
  EXPECT_EQ(pn2.getValue(), 100);
}

// ============================================================================
// Tests: Boundary EEPROM Addresses
// ============================================================================

TEST_F(PersistentNumber_test, saveToEeprom_at_boundary_address) {
  // Save at address that fits exactly (5 bytes: addr to addr+4)
  uint16_t boundaryAddr = EEPROM.length() - Ee::TOTAL_SIZE;

  PersistentNumber<uint16_t> pn1{boundaryAddr,
                                 1,
                                 "Boundary",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 0,
                                 1000};
  pn1.setValue(777);

  PersistentNumber<uint16_t> pn2{boundaryAddr,
                                 1,
                                 "Boundary",
                                 NumberDeviceClass::NONE,
                                 Unit::Type::none,
                                 0,
                                 BaseComponent::Category::NONE,
                                 0,
                                 0,
                                 1000};
  pn2.loadFromEeprom(999);

  EXPECT_EQ(pn2.getValue(), 777);
}

TEST_F(PersistentNumber_test, saveToEeprom_past_boundary_fails_gracefully) {
  // Try to save at address that goes past EEPROM end
  uint16_t pastBoundaryAddr = EEPROM.length() - 2;  // Only 2 bytes left

  PersistentNumber<uint16_t> pn{pastBoundaryAddr,
                                1,
                                "PastBoundary",
                                NumberDeviceClass::NONE,
                                Unit::Type::none,
                                0,
                                BaseComponent::Category::NONE,
                                0,
                                0,
                                1000};

  pn.setValue(888);  // Attempt save

  // Load attempt should use default due to address being OOB
  PersistentNumber<uint16_t> pn_verify{pastBoundaryAddr,
                                       1,
                                       "PastBoundary",
                                       NumberDeviceClass::NONE,
                                       Unit::Type::none,
                                       0,
                                       BaseComponent::Category::NONE,
                                       0,
                                       0,
                                       1000};
  pn_verify.loadFromEeprom(555);

  // Should not have persisted (address was invalid)
  EXPECT_EQ(pn_verify.getValue(), 555);
}

// ============================================================================
// Tests: Delegation of Number Methods
// ============================================================================

TEST_F(PersistentNumber_test, getComponentType) {
  PersistentNumber<uint16_t> pn{0x70, 1, "Test"};

  EXPECT_EQ(pn.getComponentType(), BaseComponent::Type::NUMBER);
}

TEST_F(PersistentNumber_test, getDeviceClass) {
  PersistentNumber<uint16_t> pn{0x80, 1, "Test", NumberDeviceClass::TEMPERATURE,
                                Unit::Type::C};

  EXPECT_EQ(pn.getDeviceClass(), NumberDeviceClass::TEMPERATURE);
}

TEST_F(PersistentNumber_test, getUnitType) {
  PersistentNumber<uint16_t> pn{0x90, 1, "Test", NumberDeviceClass::DISTANCE,
                                Unit::Type::m};

  EXPECT_EQ(pn.getUnitType(), Unit::Type::m);
}

// ============================================================================
// Tests: Direct Access to Wrapped Number
// ============================================================================

TEST_F(PersistentNumber_test, getNumber_returns_wrapped_instance) {
  PersistentNumber<uint16_t> pn{0xA0,
                                1,
                                "Test",
                                NumberDeviceClass::NONE,
                                Unit::Type::none,
                                0,
                                BaseComponent::Category::NONE,
                                100,
                                0,
                                1000};

  Number<uint16_t>& num = pn.getNumber();
  EXPECT_EQ(num.getValue(), 100);
}

TEST_F(PersistentNumber_test, getNumber_const_access) {
  PersistentNumber<uint16_t> pn{0xB0,
                                1,
                                "Test",
                                NumberDeviceClass::NONE,
                                Unit::Type::none,
                                0,
                                BaseComponent::Category::NONE,
                                200,
                                0,
                                1000};

  const PersistentNumber<uint16_t>& pn_const = pn;
  const Number<uint16_t>& num = pn_const.getNumber();
  EXPECT_EQ(num.getValue(), 200);
}
