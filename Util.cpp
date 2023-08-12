/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include "Util.h"

#include <Arduino.h>

void printUptime(Stream& stream) {
  auto t = millis();

  uint16_t hours = t / 3600000;
  t %= 3600000;

  uint8_t minutes = t / 60000;
  t %= 60000;

  uint8_t seconds = t / 1000;
  t %= 1000;

  uint16_t milliseconds = t;

  stream.print('[');
  stream.print(hours);
  stream.print(':');
  if (minutes < 10) {
    stream.print('0');
  }
  stream.print(minutes);
  stream.print(':');
  if (seconds < 10) {
    stream.print('0');
  }
  stream.print(seconds);
  stream.print('.');
  if (milliseconds < 100) {
    stream.print('0');
  }
  if (milliseconds < 10) {
    stream.print('0');
  }
  stream.print(milliseconds);
  stream.print("] ");
}

void printMillis(Stream& stream) {
  auto t = millis();

  stream.print('[');
  stream.print(t);
  stream.print("] ");
}

void printVersion(Stream& stream, uint8_t major, uint8_t minor, uint8_t patch) {
  stream.print(major);
  stream.print('.');
  stream.print(minor);
  stream.print('.');
  stream.print(patch);
}

void printHex(Stream& stream, uint8_t value, bool prefix) {
  if (prefix) {
    stream.print("0x");
  }
  if (value < 0x10) {
    stream.print('0');
  }
  stream.print(value, HEX);
}

void printHex(Stream& stream, uint16_t value, bool prefix) {
  if (prefix) {
    stream.print("0x");
  }
  if (value < 0x1000) {
    stream.print('0');
  }
  if (value < 0x100) {
    stream.print('0');
  }
  if (value < 0x10) {
    stream.print('0');
  }
  stream.print(value, HEX);
}

void printHex(Stream& stream, uint32_t value, bool prefix) {
  if (prefix) {
    stream.print("0x");
  }
  if (value < 0x10000000) {
    stream.print('0');
  }
  if (value < 0x1000000) {
    stream.print('0');
  }
  if (value < 0x100000) {
    stream.print('0');
  }
  if (value < 0x10000) {
    stream.print('0');
  }
  if (value < 0x1000) {
    stream.print('0');
  }
  if (value < 0x100) {
    stream.print('0');
  }
  if (value < 0x10) {
    stream.print('0');
  }
  stream.print(value, HEX);
}
