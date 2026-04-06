#include "AHT20.h"

#include "Arduino.h"

bool AHT20::begin() {
  delay(20);  // 20 ms to power up

  if (!i2c_dev.begin()) {
    return false;
  }

  {
    const uint8_t cmd[1] = {AHT20_CMD_SOFTRESET};
    if (!i2c_dev.write(cmd, 1)) {
      return false;
    }
  }
  delay(20);

  while (getStatus() & AHT20_STATUS_BUSY) {
    delay(10);
  }

  {
    const uint8_t cmd[3] = {AHT20_CMD_CALIBRATE, 0x08, 0x00};
    i2c_dev.write(cmd, 3);  // may not 'succeed' on newer AHT20s
  }

  while (getStatus() & AHT20_STATUS_BUSY) {
    delay(10);
  }
  if (!(getStatus() & AHT20_STATUS_CALIBRATED)) {
    return false;
  }

  return true;
}

uint8_t AHT20::getStatus(void) {
  uint8_t ret;
  if (!i2c_dev.read(&ret, 1)) {
    return 0xFF;
  }

  return ret;
}

bool AHT20::getMeasurements(float *humidity, float *temperature) {
  const uint8_t cmd[3] = {AHT20_CMD_TRIGGER, 0x33, 0};
  if (!i2c_dev.write(cmd, 3)) {
    return false;
  }

  while (getStatus() & AHT20_STATUS_BUSY) {
    delay(10);
  }

  uint8_t data[6];
  if (!i2c_dev.read(data, 6)) {
    return false;
  }

  const uint32_t h_data =
      ((uint32_t)data[1] << 12) | ((uint16_t)data[2] << 4) | (data[3] >> 4);
  *humidity = ((float)h_data * 100) / 0x100000;

  const uint32_t t_data =
      (uint32_t)(data[3] & 0x0F) << 16 | ((uint16_t)data[4] << 8) | data[5];
  *temperature = ((float)t_data * 200 / 0x100000) - 50;

  return true;
}
