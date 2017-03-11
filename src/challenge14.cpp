#include <functional>
#include <iostream>

#include "ByteSequence.hpp"
#include "Cracker.hpp"
#include "Encryptor.hpp"

static const unsigned RANDOM_GLOBAL_KEY_SIZE = 16;

static ByteSequence randomGlobalKeyByteSequence;

int main(int argc, char *argv[]) {
  // Initialize random global key
  randomGlobalKeyByteSequence =
      ByteSequence::getRandomByteSequence(RANDOM_GLOBAL_KEY_SIZE);

  std::string base64EncodedBackMysteryPlaintext =
      "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4g"
      "YmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQg"
      "eW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";

  const unsigned maxRandomPrependedByteCount = 100;

  srand(time(0));
  unsigned randomPrependedByteCount = rand() % maxRandomPrependedByteCount;
  std::cout << "Random prepended byte count: " << randomPrependedByteCount
            << std::endl;

  ByteSequence frontMysteryPlaintextByteSequence =
                   ByteSequence::getRandomByteSequence(
                       randomPrependedByteCount),
               backMysteryPlaintextByteSequence;
  backMysteryPlaintextByteSequence.initializeFromBase64EncodedAsciiBytes(
      std::vector<char>(
          &base64EncodedBackMysteryPlaintext[0],
          &base64EncodedBackMysteryPlaintext[base64EncodedBackMysteryPlaintext
                                                 .size()]));

  auto encryptionFunction = std::bind(
      Encryptor::getAES128BitECBModeEncryptedByteSequenceWithAppendedPlaintext,
      std::placeholders::_1, randomGlobalKeyByteSequence,
      frontMysteryPlaintextByteSequence, backMysteryPlaintextByteSequence);

  auto decryptedMysteryPlaintext =
      Cracker::getAES128BitECBModeEncryptedMysteryPlaintextByteSequence(
          encryptionFunction);

  std::cout << "Decrypted mystery plaintext:" << std::endl;
  decryptedMysteryPlaintext.printAsciiString();
  std::cout << "Decrypted mystery plaintext byte count: "
            << decryptedMysteryPlaintext.getByteCount() << std::endl;

  return 0;
}
