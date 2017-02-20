#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <map>

#include "BoundedBuffer.hpp"
#include "ByteSequence.hpp"
#include "Encryptor.hpp"
#include "Oracle.hpp"

static const unsigned RANDOM_GLOBAL_KEY_SIZE = 16;
static const char MAX_CHARACTER_GUESS = 127;

static ByteSequence randomGlobalKeyByteSequence;

static int randomNumberGenerationSeed = time(0);

ByteSequence getRandomByteSequence(unsigned byteCount) {
  const unsigned maxByteValue = 128;

  srand(randomNumberGenerationSeed++);

  std::vector<char> randomBytes;
  for (unsigned i = 0; i < byteCount; ++i) {
    randomBytes.push_back(rand() % maxByteValue);
  }

  ByteSequence randomByteSequence;
  randomByteSequence.initializeFromAsciiBytes(randomBytes);

  return randomByteSequence;
}

ByteSequence getAES128BitECBModeEncryptedByteSequenceWithMysteryPlaintext(
    const ByteSequence &plaintextByteSequence,
    const ByteSequence &keyByteSequence) {
  const unsigned blockSize = 16;

  // std::string mysteryPlainTextString =
  //     "This is a text!!This is a text!!This is a";

  std::string mysteryPlainTextBase64EncodedString =
      "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4g"
      "YmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQg"
      "eW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";

  ByteSequence mysteryPlaintextByteSequence;
  mysteryPlaintextByteSequence.initializeFromBase64EncodedAsciiBytes(
      std::vector<char>(&mysteryPlainTextBase64EncodedString[0],
                        &mysteryPlainTextBase64EncodedString
                            [mysteryPlainTextBase64EncodedString.size()]));

  // mysteryPlaintextByteSequence.initializeFromAsciiBytes(std::vector<char>(
  //     &mysteryPlainTextString[0],
  //     &mysteryPlainTextString[mysteryPlainTextString.size()]));

  ByteSequence joinedPlaintextByteSequence;
  joinedPlaintextByteSequence.appendAsciiBytes(
      plaintextByteSequence.getBytes());
  joinedPlaintextByteSequence.appendAsciiBytes(
      mysteryPlaintextByteSequence.getBytes());

  if (joinedPlaintextByteSequence.getByteCount() % blockSize != 0) {
    joinedPlaintextByteSequence =
        joinedPlaintextByteSequence.getPaddedByteSequence(blockSize);
  }

  return Encryptor::getAES128BitECBModeEncryptedByteSequence(
      joinedPlaintextByteSequence, keyByteSequence);
}

int main(int argc, char *argv[]) {
  // Initialize random global key
  randomGlobalKeyByteSequence = getRandomByteSequence(RANDOM_GLOBAL_KEY_SIZE);

  // Determine block size
  unsigned computedBlockSize = 0, mysteryPlaintextByteCount = 0;
  {
    ByteSequence userByteSequence;
    userByteSequence.initializeFromAsciiBytes(std::vector<char>(1, 0));

    int firstCiphertextByteCount =
            getAES128BitECBModeEncryptedByteSequenceWithMysteryPlaintext(
                userByteSequence, randomGlobalKeyByteSequence)
                .getByteCount(),
        nextCiphertextByteCount = -1;
    while (nextCiphertextByteCount <= firstCiphertextByteCount) {
      auto test = getAES128BitECBModeEncryptedByteSequenceWithMysteryPlaintext(
          userByteSequence, randomGlobalKeyByteSequence);

      nextCiphertextByteCount =
          getAES128BitECBModeEncryptedByteSequenceWithMysteryPlaintext(
              userByteSequence, randomGlobalKeyByteSequence)
              .getByteCount();

      userByteSequence.appendAsciiBytes(std::vector<char>(1, 0));
    }

    computedBlockSize = nextCiphertextByteCount - firstCiphertextByteCount;

    mysteryPlaintextByteCount =
        firstCiphertextByteCount - (userByteSequence.getByteCount() - 1);
  }

  std::cout << "Computed block size: " << computedBlockSize << std::endl;

  std::cout << "Mystery plaintext length: " << mysteryPlaintextByteCount
            << std::endl;

  // Assert that the ciphertext is AES ECB mode encrypted
  {
    const unsigned userByteSequenceLength = 1024;

    ByteSequence userByteSequence;
    userByteSequence.initializeFromAsciiBytes(
        std::vector<char>(userByteSequenceLength, 0));

    auto ciphertextByteSequence =
        getAES128BitECBModeEncryptedByteSequenceWithMysteryPlaintext(
            userByteSequence, randomGlobalKeyByteSequence);

    assert(Oracle::isAES128BitECBModeCipherText(
        ciphertextByteSequence,
        Oracle::ECB_AVERAGE_NORMALIZED_HAMMING_DISTANCE_THRESHOLD));
  }

  std::cout << "AES ECB mode ciphertext confirmed" << std::endl;

  ByteSequence decryptedMysteryPlaintext;
  {
    // Bounded buffer associated with the last (block size - 1) worth
    // of decrypted plaintext bytes
    BoundedBuffer<char> lastDecryptedPlaintextBytesBuffer(computedBlockSize -
                                                          1);

    for (unsigned mysteryPlaintextByteIndex = 0;
         mysteryPlaintextByteIndex < mysteryPlaintextByteCount;
         ++mysteryPlaintextByteIndex) {
      // Compute the expected ciphertext
      ByteSequence userByteSequence;
      userByteSequence.initializeFromAsciiBytes(
          std::vector<char>(computedBlockSize - 1 -
                                (mysteryPlaintextByteIndex % computedBlockSize),
                            0));

      auto expectedCiphertextByteSequence =
          getAES128BitECBModeEncryptedByteSequenceWithMysteryPlaintext(
              userByteSequence, randomGlobalKeyByteSequence)
              .getSubSequence((mysteryPlaintextByteIndex / computedBlockSize) *
                                  computedBlockSize,
                              computedBlockSize);

      // Enumerate the possible ciphertexts
      userByteSequence.initializeFromAsciiBytes(std::vector<char>(0));
      if (decryptedMysteryPlaintext.getByteCount() < computedBlockSize - 1) {
        userByteSequence.appendAsciiBytes(std::vector<char>(
            computedBlockSize - decryptedMysteryPlaintext.getByteCount() - 1,
            0));
      }

      unsigned decryptedByteCount =
          decryptedMysteryPlaintext.getByteCount() < computedBlockSize - 1
              ? decryptedMysteryPlaintext.getByteCount()
              : computedBlockSize - 1;

      auto it = lastDecryptedPlaintextBytesBuffer.begin();
      std::vector<char> decryptedBytes;
      for (unsigned i = 0; i < decryptedByteCount; ++i, ++it) {
        decryptedBytes.push_back(*it);
      }

      std::reverse(decryptedBytes.begin(), decryptedBytes.end());
      userByteSequence.appendAsciiBytes(decryptedBytes);

      assert(userByteSequence.getByteCount() == computedBlockSize - 1);

      char nextCharacter = 0;
      for (; nextCharacter < MAX_CHARACTER_GUESS; ++nextCharacter) {
        ByteSequence nextPlaintextByteSequenceGuess;
        nextPlaintextByteSequenceGuess.initializeFromAsciiBytes(
            userByteSequence.getBytes());
        nextPlaintextByteSequenceGuess.appendAsciiBytes(
            std::vector<char>(1, nextCharacter));

        auto nextCiphertextByteSequenceGuess =
            getAES128BitECBModeEncryptedByteSequenceWithMysteryPlaintext(
                nextPlaintextByteSequenceGuess, randomGlobalKeyByteSequence)
                .getSubSequence(0, computedBlockSize);

        if (nextCiphertextByteSequenceGuess == expectedCiphertextByteSequence) {
          char nextDecryptedMysteryPlaintextCharacter = nextCharacter;

          decryptedMysteryPlaintext.appendAsciiBytes(
              std::vector<char>(1, nextDecryptedMysteryPlaintextCharacter));

          lastDecryptedPlaintextBytesBuffer.push(
              nextDecryptedMysteryPlaintextCharacter);

          break;
        }
      }

      // Ensure a guess matched the expected ciphertext
      assert(nextCharacter != 127);
    }
  }

  std::cout << "Decrypted mystery plaintext:" << std::endl;
  decryptedMysteryPlaintext.printAsciiString();
  std::cout << "Decrypted mystery plaintext byte count: "
            << decryptedMysteryPlaintext.getByteCount() << std::endl;

  return 0;
}
