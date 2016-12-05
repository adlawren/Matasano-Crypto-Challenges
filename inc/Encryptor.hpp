#ifndef __ENCRYPTOR_HPP__
#define __ENCRYPTOR_HPP__

#include "ByteSequence.hpp"

class Encryptor {
public:
  static ByteSequence
  getRepeatingKeyXorEncryptedByteSequence(const ByteSequence &byteSequence,
                                          const ByteSequence &key) {
    std::vector<char> encryptedBytes(byteSequence.getBytes());
    auto keyBytes = key.getBytes();

    for (size_t i = 0; i < encryptedBytes.size(); ++i) {
      encryptedBytes[i] ^= keyBytes[i % keyBytes.size()];
    }

    ByteSequence encryptedByteSequence;
    encryptedByteSequence.initializeFromAsciiBytes(encryptedBytes);

    return encryptedByteSequence;
  }
};

#endif // __ENCRYPTOR_HPP__
