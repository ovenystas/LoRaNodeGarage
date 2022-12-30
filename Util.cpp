/*
 *  Created on: 1 mars 2021
 *      Author: oveny
 */

#include "Util.h"

#include <Arduino.h>

void printUptime(Stream& stream) {
  auto t = millis();

  uint16_t h = t / 3600000;
  t = t % 3600000;

  uint8_t m = t / 60000;
  t = t % 60000;

  uint8_t s = t / 60;

  uint8_t ms = t % 1000;

  stream.print('[');
  stream.print(h);
  stream.print(':');
  stream.print(m);
  stream.print(':');
  stream.print(s);
  stream.print('.');
  stream.print(ms);
  stream.print("] ");
}

void printMillis(Stream& stream) {
  auto t = millis();

  stream.print('[');
  stream.print(t);
  stream.print("] ");
}

void printVersion(Stream& stream, int major, int minor, int patch) {
  stream.print(major);
  stream.print('.');
  stream.print(minor);
  stream.print('.');
  stream.print(patch);
  stream.print('.');
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
