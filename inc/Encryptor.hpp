#ifndef __ENCRYPTOR_HPP__
#define __ENCRYPTOR_HPP__

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include "ByteSequence.hpp"

class Encryptor {
public:
  static ByteSequence getAES128BitCBCModeEncryptedByteSequence(
      const ByteSequence &byteSequence, const ByteSequence &keyByteSequence,
      const ByteSequence &initializationVectorByteSequence) {
    const size_t blockSize = 16;

    assert(byteSequence.getByteCount() % blockSize == 0);

    ByteSequence previousCiphertextBlockByteSequence;
    previousCiphertextBlockByteSequence.initializeFromAsciiBytes(
        initializationVectorByteSequence.getBytes());

    ByteSequence encryptedByteSequence;
    for (unsigned i = 0; i < byteSequence.getByteCount() / blockSize; ++i) {
      auto blockByteSequence =
          byteSequence.getSubSequence(i * blockSize, blockSize);

      auto xoredBlockblockByteSequence = blockByteSequence.getXoredByteSequence(
          previousCiphertextBlockByteSequence);

      auto encryptedBlockByteSequence =
          Encryptor::getAES128BitECBModeEncryptedByteSequence(
              xoredBlockblockByteSequence, keyByteSequence);

      encryptedByteSequence.appendAsciiBytes(
          encryptedBlockByteSequence.getBytes());

      previousCiphertextBlockByteSequence.initializeFromAsciiBytes(
          encryptedBlockByteSequence.getBytes());
    }

    return encryptedByteSequence;
  }

  static ByteSequence getAES128BitECBModeEncryptedByteSequence(
      const ByteSequence &byteSequence, const ByteSequence &keyByteSequence) {
    const size_t blockSize = 16;

    assert(byteSequence.getByteCount() % blockSize == 0);
    assert(keyByteSequence.getByteCount() == blockSize);

    ByteSequence ciphertextByteSequence;

    unsigned blockCount = byteSequence.getByteCount() / blockSize;
    for (unsigned i = 0; i < blockCount; ++i) {
      auto nextBlockByteSequence =
          byteSequence.getSubSequence(i * blockSize, blockSize);

      auto nextBlockCiphertextByteSequence =
          get16ByteAES128BitECBModeEncryptedByteSequence(nextBlockByteSequence,
                                                         keyByteSequence);

      ciphertextByteSequence.appendAsciiBytes(
          nextBlockCiphertextByteSequence.getBytes());
    }

    return ciphertextByteSequence;
  }

  static ByteSequence
  getAES128BitECBModeEncryptedByteSequenceWithAppendedPlaintext(
      const ByteSequence &plaintextByteSequence,
      const ByteSequence &keyByteSequence,
      const ByteSequence &frontPlaintextByteSequence,
      const ByteSequence &backPlaintextByteSequence) {
    const unsigned blockSize = 16;

    assert(blockSize == keyByteSequence.getByteCount());

    ByteSequence joinedPlaintextByteSequence;
    joinedPlaintextByteSequence.appendAsciiBytes(
        frontPlaintextByteSequence.getBytes());
    joinedPlaintextByteSequence.appendAsciiBytes(
        plaintextByteSequence.getBytes());
    joinedPlaintextByteSequence.appendAsciiBytes(
        backPlaintextByteSequence.getBytes());

    if (joinedPlaintextByteSequence.getByteCount() % blockSize != 0) {
      joinedPlaintextByteSequence =
          joinedPlaintextByteSequence.getPaddedByteSequence(blockSize);
    }

    return getAES128BitECBModeEncryptedByteSequence(joinedPlaintextByteSequence,
                                                    keyByteSequence);
  }

  static ByteSequence
  getRepeatingKeyXorEncryptedByteSequence(const ByteSequence &byteSequence,
                                          const ByteSequence &keyByteSequence) {
    std::vector<char> encryptedBytes(byteSequence.getBytes());
    auto keyBytes = keyByteSequence.getBytes();

    for (size_t i = 0; i < encryptedBytes.size(); ++i) {
      encryptedBytes[i] ^= keyBytes[i % keyBytes.size()];
    }

    ByteSequence encryptedByteSequence;
    encryptedByteSequence.initializeFromAsciiBytes(encryptedBytes);

    return encryptedByteSequence;
  }

private:
  static ByteSequence get16ByteAES128BitECBModeEncryptedByteSequence(
      const ByteSequence &byteSequence, const ByteSequence &keyByteSequence) {
    const size_t blockSize = 16;

    assert(byteSequence.getByteCount() == blockSize);
    assert(keyByteSequence.getByteCount() == blockSize);

    SSL_load_error_strings();

    std::basic_string<unsigned char> textString(blockSize, 0);
    for (size_t i = 0; i < blockSize; ++i) {
      textString[i] = (unsigned char)byteSequence.getBytes()[i];
    }

    std::basic_string<unsigned char> keyString(blockSize, 0);
    for (size_t i = 0; i < blockSize; ++i) {
      keyString[i] = (unsigned char)keyByteSequence.getBytes()[i];
    }

    std::vector<char> cipherTextByteVector;

    std::basic_string<unsigned char> ciphertextString;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, keyString.c_str(), NULL);
    EVP_CIPHER_CTX_set_padding(&ctx, false);

    unsigned char buffer[1024], *pointer = buffer;
    int outlen;
    EVP_EncryptUpdate(&ctx, pointer, &outlen, textString.c_str(),
                      textString.length());

    pointer += outlen;
    EVP_EncryptFinal_ex(&ctx, pointer, &outlen);

    pointer += outlen;
    ciphertextString =
        std::basic_string<unsigned char>(buffer, pointer - buffer);

    ByteSequence ciphertextByteSequence;
    ciphertextByteSequence.initializeFromAsciiBytes(std::vector<char>(
        &ciphertextString[0], &ciphertextString[ciphertextString.length()]));

    return ciphertextByteSequence;
  }
};

#endif // __ENCRYPTOR_HPP__
