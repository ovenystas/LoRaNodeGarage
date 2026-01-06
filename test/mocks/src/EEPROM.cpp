#include "EEPROM.h"

static uint8_t EEPROM_mem[E2END + 1];

uint8_t eeprom_read_byte(int index) {
  assert(index >= 0);
  assert(index <= E2END);

  return EEPROM_mem[index];
}

void eeprom_write_byte(int index, uint8_t __value) {
  assert(index >= 0);
  assert(index <= E2END);

  EEPROM_mem[index] = __value;
}

void eeprom_clear(uint8_t val) {
  for (uint16_t i = 0; i <= E2END; i++) {
    EEPROM_mem[i] = val;
  }
}
