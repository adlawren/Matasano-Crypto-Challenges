#ifndef __ENCRYPTOR_HPP__
#define __ENCRYPTOR_HPP__

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include "ByteSequence.hpp"

class Encryptor {
public:
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

  static ByteSequence getAES128BitECBModeEncryptedByteSequence(
      const ByteSequence &byteSequence, const ByteSequence &keyByteSequence) {
    SSL_load_error_strings();

    std::basic_string<unsigned char> text(
        (const unsigned char *)&(byteSequence.getBytes()[0]));
    std::basic_string<unsigned char> keyString(
        (const unsigned char *)&(keyByteSequence.getBytes()[0]));

    std::vector<char> cipherTextByteVector;

    std::basic_string<unsigned char> ciphertextString;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, keyString.c_str(), NULL);
    EVP_CIPHER_CTX_set_padding(&ctx, false);

    std::basic_string<unsigned char> testString(
        (const unsigned char *)&(byteSequence.getBytes()[0]));

    unsigned char buffer[1024], *pointer = buffer;
    int outlen;
    EVP_EncryptUpdate(&ctx, pointer, &outlen, testString.c_str(),
                      testString.length());

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
