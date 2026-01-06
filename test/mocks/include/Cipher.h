#pragma once

#include <gmock/gmock.h>

class Cipher
{
public:
    virtual size_t keySize() const = 0;
    virtual size_t ivSize() const = 0;

    virtual bool setKey(const uint8_t *key, size_t len) = 0;
    virtual bool setIV(const uint8_t *iv, size_t len) = 0;

    virtual void encrypt(uint8_t *output, const uint8_t *input, size_t len) = 0;
    virtual void decrypt(uint8_t *output, const uint8_t *input, size_t len) = 0;

    virtual void clear() = 0;
};

class CipherMock : public Cipher {
 public:
  virtual ~CipherMock() = default;
  MOCK_METHOD(size_t, keySize, (), (const));
  MOCK_METHOD(size_t, ivSize, (), (const));
  MOCK_METHOD(bool, setKey, (const uint8_t*, size_t));
  MOCK_METHOD(bool, setIV, (const uint8_t*, size_t));
  MOCK_METHOD(void, encrypt, (uint8_t*, const uint8_t*, size_t));
  MOCK_METHOD(void, decrypt, (uint8_t*, const uint8_t*, size_t));
  MOCK_METHOD(void, clear, ());
};
