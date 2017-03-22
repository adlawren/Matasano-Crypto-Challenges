#include <ctime>
#include <iostream>

#include "Encryptor.hpp"
#include "Decryptor.hpp"
#include "Oracle.hpp"

typedef struct CBCEncryptionResult {
  ByteSequence ciphertextByteSequence, initializationVectorByteSequence;
} CBCEncryptionResult;

static const unsigned BLOCK_SIZE = 16;
static const unsigned MAX_BYTE_VALUE = 256;

static ByteSequence randomGlobalKeyByteSequence;

bool isPlaintextPaddingValid(
    const ByteSequence& ciphertextByteSequence,
    const ByteSequence& keyByteSequence,
    const ByteSequence& initializationVectorByteSequence) {
  auto plaintextByteSequence =
      Decryptor::decryptAES128BitCBCModeEncryptedByteSequence(
          ciphertextByteSequence, keyByteSequence,
          initializationVectorByteSequence);

  try {
    Oracle::stripPKCSPadding(plaintextByteSequence);
    return true;
  } catch (std::runtime_error& re) {
    return false;
  }
}

CBCEncryptionResult CBCEncryptionFunction(const ByteSequence& keyByteSequence) {
  static int randomNumberGenerationSeed = -1;
  if (randomNumberGenerationSeed < 0) {
    randomNumberGenerationSeed = time(NULL);
    srand(randomNumberGenerationSeed);
  }

  std::vector<std::string> randomStrings = {
      "MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc=",
      "MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBp"
      "bic=",
      "MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw==",
      "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg==",
      "MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl",
      "MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA==",
      "MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw==",
      "MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8=",
      "MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g=",
      "MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93"};

  std::vector<ByteSequence> randomByteSequences;
  for (auto randomString : randomStrings) {
    ByteSequence nextRandomByteSequence;
    nextRandomByteSequence.initializeFromBase64EncodedAsciiBytes(
        std::vector<char>(&randomString[0],
                          &randomString[randomString.size()]));

    randomByteSequences.push_back(nextRandomByteSequence);
  }

  auto paddedByteSequence =
      randomByteSequences[rand() % randomByteSequences.size()]
          .getPaddedByteSequence(BLOCK_SIZE);

  auto initializationVectorByteSequence =
      ByteSequence::getRandomByteSequence(BLOCK_SIZE);

  auto ciphertextByteSequence =
      Encryptor::getAES128BitCBCModeEncryptedByteSequence(
          paddedByteSequence, keyByteSequence,
          initializationVectorByteSequence);

  CBCEncryptionResult cbcEncryptionResult;
  cbcEncryptionResult.ciphertextByteSequence = ciphertextByteSequence;
  cbcEncryptionResult.initializationVectorByteSequence =
      initializationVectorByteSequence;

  return cbcEncryptionResult;
}

int main(int argc, char* argv[]) {
  // Initialize random global key byte sequence
  randomGlobalKeyByteSequence = ByteSequence::getRandomByteSequence(BLOCK_SIZE);

  auto cbcEncryptionResult = CBCEncryptionFunction(randomGlobalKeyByteSequence);

  ByteSequence decryptedByteSequence;

  unsigned blockCount =
      cbcEncryptionResult.ciphertextByteSequence.getByteCount() / BLOCK_SIZE;
  for (unsigned i = 0; i < blockCount; ++i) {
    ByteSequence blockByteSequence =
        cbcEncryptionResult.ciphertextByteSequence.getSubSequence(
            i * BLOCK_SIZE, BLOCK_SIZE);
    ByteSequence decryptedBytesByteSequence;
    decryptedBytesByteSequence.initializeFromAsciiBytes(
        std::vector<char>(BLOCK_SIZE, 0));
    for (int j = BLOCK_SIZE - 1; j >= 0; --j) {
      ByteSequence temporaryInitializationVectorByteSequence;
      temporaryInitializationVectorByteSequence.appendAsciiBytes(
          std::vector<char>(
              BLOCK_SIZE - decryptedBytesByteSequence.getByteCount(), 0));
      temporaryInitializationVectorByteSequence.appendAsciiBytes(
          decryptedBytesByteSequence.getBytes());

      ByteSequence paddingBitmaskByteSequence;
      paddingBitmaskByteSequence.appendAsciiBytes(std::vector<char>(j + 1, 0));
      paddingBitmaskByteSequence.appendAsciiBytes(
          std::vector<char>(BLOCK_SIZE - j - 1, BLOCK_SIZE - j));

      assert(paddingBitmaskByteSequence.getByteCount() == BLOCK_SIZE);
      assert(temporaryInitializationVectorByteSequence.getByteCount() ==
             BLOCK_SIZE);

      auto xoredTemporaryInitializationVectorByteSequence =
          temporaryInitializationVectorByteSequence.getXoredByteSequence(
              paddingBitmaskByteSequence);

      char decryptedByte = 0;
      for (unsigned k = 0; k < MAX_BYTE_VALUE; ++k) {
        xoredTemporaryInitializationVectorByteSequence.setByteAtIndex(j, k);

        if (isPlaintextPaddingValid(
                blockByteSequence, randomGlobalKeyByteSequence,
                xoredTemporaryInitializationVectorByteSequence)) {
          decryptedByte = k ^ (BLOCK_SIZE - j);
          break;
        }
      }

      decryptedBytesByteSequence.setByteAtIndex(j, decryptedByte);
    }

    // Append next decrypted block to decrypted byte sequence
    ByteSequence xoredDecryptedBytesByteSequence;
    if (i > 0) {
      xoredDecryptedBytesByteSequence =
          decryptedBytesByteSequence.getXoredByteSequence(
              cbcEncryptionResult.ciphertextByteSequence.getSubSequence(
                  (i - 1) * BLOCK_SIZE, BLOCK_SIZE));
    } else {
      xoredDecryptedBytesByteSequence =
          decryptedBytesByteSequence.getXoredByteSequence(
              cbcEncryptionResult.initializationVectorByteSequence);
    }

    decryptedByteSequence.appendAsciiBytes(
        xoredDecryptedBytesByteSequence.getBytes());
  }

  std::cout << "Decrypted string:" << std::endl;
  decryptedByteSequence.printAsciiString();
  decryptedByteSequence.printHexEncodedAsciiString();

  auto strippedDecryptedByteSequence =
      Oracle::stripPKCSPadding(decryptedByteSequence);

  std::cout << "Stripped decrypted string:" << std::endl;
  strippedDecryptedByteSequence.printAsciiString();
  strippedDecryptedByteSequence.printHexEncodedAsciiString();

  return 0;
}