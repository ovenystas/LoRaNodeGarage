#pragma once

#include <inttypes.h>

#include "Stream.h"

class BufferSerial : public Stream {
 protected:
  volatile size_t _buffer_head{0};
  volatile size_t _buffer_tail{0};

  size_t _buffer_size;

  unsigned char* _buffer;

 public:
  explicit BufferSerial(size_t buffer_size);
  virtual ~BufferSerial();

  virtual int available(void) override;
  virtual int peek(void) override;
  virtual int read(void) override;
  virtual int availableForWrite(void) override;
  virtual void flush(void) override;
  virtual size_t write(uint8_t c) override;

  // inline size_t write(unsigned long n) { return write((uint8_t)n); }
  // inline size_t write(long n) { return write((uint8_t)n); }
  // inline size_t write(unsigned int n) { return write((uint8_t)n); }
  // inline size_t write(int n) { return write((uint8_t)n); }
  using Print::write;  // pull in write(str) and write(buf, size) from Print
  // operator bool() { return true; }
};
