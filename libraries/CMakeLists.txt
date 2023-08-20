include_directories(.)

add_library(
  crypto_ctr_aes128 STATIC EXCLUDE_FROM_ALL
  arduinolibs/libraries/Crypto/AES128.cpp
  arduinolibs/libraries/Crypto/AESCommon.cpp
  arduinolibs/libraries/Crypto/BlockCipher.cpp
  arduinolibs/libraries/Crypto/Cipher.cpp
  arduinolibs/libraries/Crypto/Crypto.cpp
  arduinolibs/libraries/Crypto/CTR.cpp)

add_library(crc_crc8 STATIC EXCLUDE_FROM_ALL
            CRC/src/CRC8.cpp CRC/src/CrcFastReverse.cpp Arduino.cpp)
