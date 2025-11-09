#include "BufferSerial.h"

#include <assert.h>

#include <cstdlib>

// #include "Arduino.h"

BufferSerial::BufferSerial(size_t buffer_size) : _buffer_size{buffer_size} {
  _buffer = static_cast<unsigned char*>(malloc(buffer_size));
  assert(_buffer);
}

BufferSerial::~BufferSerial() {
  // clear any received data
  _buffer_head = _buffer_tail;

  // Free buffer memory
  if (_buffer) {
    free(_buffer);
  }
}

int BufferSerial::available(void) {
  return (static_cast<unsigned int>(_buffer_size + _buffer_head -
                                    _buffer_tail)) %
         _buffer_size;
}

int BufferSerial::peek(void) {
  if (_buffer_head == _buffer_tail) {
    return -1;
  } else {
    return _buffer[_buffer_tail];
  }
}

int BufferSerial::read(void) {
  // if the head isn't ahead of the tail, we don't have any characters
  if (_buffer_head == _buffer_tail) {
    return -1;
  } else {
    unsigned char c = _buffer[_buffer_tail];
    _buffer_tail = static_cast<size_t>(_buffer_tail + 1) % _buffer_size;
    return c;
  }
}

int BufferSerial::availableForWrite(void) {
  size_t head = _buffer_head;
  size_t tail = _buffer_tail;

  if (head >= tail) {
    return _buffer_size - 1 - head + tail;
  }
  return tail - head - 1;
}

void BufferSerial::flush() { _buffer_head = _buffer_tail; }

size_t BufferSerial::write(uint8_t c) {
  size_t i = (_buffer_head + 1) % _buffer_size;

  assert(i != _buffer_tail);

  _buffer[_buffer_head] = c;
  _buffer_head = i;

  return 1;
}
