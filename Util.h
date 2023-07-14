/*
 *  Created on: 28 feb. 2021
 *      Author: oveny
 */

#pragma once

#include <Arduino.h>
#include <Stream.h>

// TODO: Handle wrap-around after 49 days.
#define seconds() (millis() / 1000)

#define lowWord(ul) (static_cast<uint16_t>((ul)&0xffff))
#define highWord(ul) (static_cast<uint16_t>((ul) >> 16))

#define htons(x) ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0xFF))
#define ntohs(x) htons(x)

#define htonl(x)                                            \
  (((x) << 24 & 0xFF000000UL) | ((x) << 8 & 0x00FF0000UL) | \
   ((x) >> 8 & 0x0000FF00UL) | ((x) >> 24 & 0x000000FFUL))
#define ntohl(x) htonl(x)

inline uint8_t hton(uint8_t v) { return v; }
inline int8_t hton(int8_t v) { return v; }
inline uint16_t hton(uint16_t v) { return htons(v); }
inline int16_t hton(int16_t v) { return htons(v); }
inline uint32_t hton(uint32_t v) { return htonl(v); }
inline int32_t hton(int32_t v) { return htonl(v); }

inline uint8_t ntoh(uint8_t v) { return v; }
inline int8_t ntoh(int8_t v) { return v; }
inline uint16_t ntoh(uint16_t v) { return ntohs(v); }
inline int16_t ntoh(int16_t v) { return ntohs(v); }
inline uint32_t ntoh(uint32_t v) { return ntohl(v); }
inline int32_t ntoh(int32_t v) { return ntohl(v); }

void printMillis(Stream& stream);
void printUptime(Stream& stream);
void printVersion(Stream& stream, int major, int minor, int patch);
void printHex(Stream& stream, uint8_t value, bool prefix = false);
void printHex(Stream& stream, uint16_t value, bool prefix = false);
void printHex(Stream& stream, uint32_t value, bool prefix = false);
