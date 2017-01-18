#ifndef __DECRYPTOR_HPP__
#define __DECRYPTOR_HPP__

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include "ByteSequence.hpp"
#include "Encryptor.hpp"

class Decryptor {
public:
  static ByteSequence decryptAES128BitECBModeEncryptedByteSequence(
      const ByteSequence &encryptedByteSequence,
      const ByteSequence &keyByteSequence) {
    SSL_load_error_strings();

    std::basic_string<unsigned char> keyString(
        (const unsigned char *)&(keyByteSequence.getBytes()[0]));
    std::basic_string<unsigned char> plaintextString;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_DecryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, keyString.c_str(), NULL);
    EVP_CIPHER_CTX_set_padding(&ctx, false);

    std::basic_string<unsigned char> testString(
        (const unsigned char *)&(encryptedByteSequence.getBytes()[0]));

    unsigned char buffer[1024], *pointer = buffer;
    int outlen;
    EVP_DecryptUpdate(&ctx, pointer, &outlen, testString.c_str(),
                      testString.length());

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
