#pragma once

#ifdef ARDUINO_ARCH_SAMD
#include <FlashAsEEPROM.h>
#else
#include <EEPROM.h>
#endif

#ifndef E2END
#define E2END (EEPROM_EMULATION_SIZE - 1)
#endif

class GenericEEPROMClass : public EEPROMClass {
 public:
  GenericEEPROMClass() : EEPROMClass() {}

  // Functionality to 'get' and 'put' objects to and from EEPROM.
  template <typename T>
  T &get(int idx, T &t) {
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&t);
    for (int count = sizeof(T); count > 0; count--) {
      *ptr++ = EEPROMClass::read(idx++);
    }
    return t;
  }

  template <typename T>
  const T &put(int idx, const T &t) {
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&t);
    for (int count = sizeof(T); count > 0; count--) {
      EEPROMClass::update(idx++, *ptr++);
    }
    return t;
  }

#ifndef ARDUINO_ARCH_SAMD
  void commit() {
    // Do nothing
  }
#endif
};

extern GenericEEPROMClass GenericEEPROM;
