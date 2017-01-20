#ifndef __DECRYPTOR_HPP__
#define __DECRYPTOR_HPP__

#include <algorithm>

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include "ByteSequence.hpp"
#include "Encryptor.hpp"

class Decryptor {
public:
  static ByteSequence decryptAES128BitCBCModeEncryptedByteSequence(
      const ByteSequence &byteSequence, const ByteSequence &keyByteSequence) {
    const size_t blockSize = 16;

    assert(byteSequence.getByteCount() % blockSize == 0);

    ByteSequence decryptedByteSequence, previousCiphertextBlockByteSequence;
    for (int i = byteSequence.getByteCount() / blockSize - 1; i >= 0; --i) {
      if (i > 0) {
        previousCiphertextBlockByteSequence.initializeFromAsciiBytes(
            byteSequence.getSubSequence((i - 1) * blockSize, blockSize)
                .getBytes());
      } else {
        previousCiphertextBlockByteSequence.initializeFromAsciiBytes(
            std::vector<char>(blockSize,
                              0)); // Initialization Vector
      }

      auto blockByteSequence =
          byteSequence.getSubSequence(i * blockSize, blockSize);

      auto decryptedBlockByteSequence =
          Decryptor::decryptAES128BitECBModeEncryptedByteSequence(
              blockByteSequence, keyByteSequence);

      auto xoredBlockblockByteSequence =
          decryptedBlockByteSequence.getXoredByteSequence(
              previousCiphertextBlockByteSequence);

      auto decrytpedBytes = xoredBlockblockByteSequence.getBytes();

      std::reverse(decrytpedBytes.begin(), decrytpedBytes.end());
      decryptedByteSequence.appendAsciiBytes(decrytpedBytes);
    }

    auto decryptedBytes = decryptedByteSequence.getBytes();

    std::reverse(decryptedBytes.begin(), decryptedBytes.end());

    ByteSequence reversedDecryptedByteSequence;
    reversedDecryptedByteSequence.initializeFromAsciiBytes(decryptedBytes);

    return reversedDecryptedByteSequence;
  }

  static ByteSequence decryptAES128BitECBModeEncryptedByteSequence(
      const ByteSequence &encryptedByteSequence,
      const ByteSequence &keyByteSequence) {
    const size_t blockSize = 16;

    assert(encryptedByteSequence.getByteCount() % blockSize == 0);
    assert(keyByteSequence.getByteCount() == blockSize);

    SSL_load_error_strings();

    std::basic_string<unsigned char> keyString(
        (const unsigned char *)&(keyByteSequence.getBytes()[0]));
    std::basic_string<unsigned char> plaintextString;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_DecryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, keyString.c_str(), NULL);
    EVP_CIPHER_CTX_set_padding(&ctx, false);

    std::basic_string<unsigned char> ciphertextString(blockSize, 0);
    for (size_t i = 0; i < blockSize; ++i) {
      ciphertextString[i] = encryptedByteSequence.getBytes()[i];
    }

    unsigned char buffer[1024], *pointer = buffer;
    int outlen;
    EVP_DecryptUpdate(&ctx, pointer, &outlen, ciphertextString.c_str(),
                      ciphertextString.length());

    pointer += outlen;
    EVP_DecryptFinal_ex(&ctx, pointer, &outlen);

    pointer += outlen;
    plaintextString =
        std::basic_string<unsigned char>(buffer, pointer - buffer);

    ByteSequence plaintextByteSequence;
    plaintextByteSequence.initializeFromAsciiBytes(std::vector<char>(
        &plaintextString[0], &plaintextString[plaintextString.length()]));

    return plaintextByteSequence;
  }
};

#endif // __DECRYPTOR_HPP__
