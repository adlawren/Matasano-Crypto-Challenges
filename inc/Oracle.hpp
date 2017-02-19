#ifndef __ORACLE_HPP__
#define __ORACLE_HPP__

#include <cassert>
#include <cstdlib>
#include <ctime>

#include "ByteSequence.hpp"
#include "Encryptor.hpp"

typedef enum AES_ENCRYPTION_MODE { ECB, CBC, UNKNOWN } AES_ENCRYPTION_MODE;

static const std::string ECB_AES_ENCRYPTION_MODE_STRING = "ECB";
static const std::string CBC_AES_ENCRYPTION_MODE_STRING = "CBC";

struct AES128BitRandomModeEncryptedByteSequenceResult {
  ByteSequence ciphertextByteSequence;
  AES_ENCRYPTION_MODE encryptionMode;
};

class Oracle {
public:
  static const float ECB_AVERAGE_NORMALIZED_HAMMING_DISTANCE_THRESHOLD;

  static AES128BitRandomModeEncryptedByteSequenceResult
  getAES128BitRandomModeEncryptedByteSequence(
      const ByteSequence &byteSequence, const ByteSequence &keyByteSequence) {
    const size_t blockSize = 16;

    AES128BitRandomModeEncryptedByteSequenceResult result;

    // Use a new seed every function call
    std::srand(randomNumberGenerationSeed++);

    // Pad the beginning and end of the byte sequence with 5 - 10 random bytes
    auto bytes = byteSequence.getBytes();

    {
      int randomByteCount = 5 + (rand() % 5);
      auto randomByteSequence = getRandomByteSequence(randomByteCount);

      bytes.insert(std::begin(bytes), std::begin(randomByteSequence.getBytes()),
                   std::end(randomByteSequence.getBytes()));
    }

    {
      int randomByteCount = 5 + (rand() % 5);
      auto randomByteSequence = getRandomByteSequence(randomByteCount);

      bytes.insert(std::end(bytes), std::begin(randomByteSequence.getBytes()),
                   std::end(randomByteSequence.getBytes()));
    }

    ByteSequence updatedByteSequence;
    updatedByteSequence.initializeFromAsciiBytes(bytes);

    auto paddedByteSequence =
        updatedByteSequence.getPaddedByteSequence(blockSize);

    int randomValue = rand() % 2;
    if (randomValue == 0) {
      auto ciphertextByteSequence =
          Encryptor::getAES128BitECBModeEncryptedByteSequence(
              paddedByteSequence, keyByteSequence);
      result.ciphertextByteSequence = ciphertextByteSequence;

      result.encryptionMode = AES_ENCRYPTION_MODE::ECB;
    } else {
      auto initializationVectorByteSequence = getRandomByteSequence(blockSize);

      auto ciphertextByteSequence =
          Encryptor::getAES128BitCBCModeEncryptedByteSequence(
              paddedByteSequence, keyByteSequence,
              initializationVectorByteSequence);

      result.ciphertextByteSequence = ciphertextByteSequence;

      result.encryptionMode = AES_ENCRYPTION_MODE::CBC;
    }

    return result;
  }

  static std::string
  getAESEncryptionModeString(AES_ENCRYPTION_MODE aesEncryptionMode) {
    switch (aesEncryptionMode) {
    case AES_ENCRYPTION_MODE::ECB:
      return ECB_AES_ENCRYPTION_MODE_STRING;
    case AES_ENCRYPTION_MODE::CBC:
      return CBC_AES_ENCRYPTION_MODE_STRING;
    default:
      break;
    }

    return std::string("Unknown AES encryption mode");
  }

  static AES_ENCRYPTION_MODE
  getAESEncryptionMode(const std::string aesEncryptionModeString) {
    if (aesEncryptionModeString.compare(ECB_AES_ENCRYPTION_MODE_STRING) == 0) {
      return AES_ENCRYPTION_MODE::ECB;
    }

    if (aesEncryptionModeString.compare(CBC_AES_ENCRYPTION_MODE_STRING) == 0) {
      return AES_ENCRYPTION_MODE::CBC;
    }

    return AES_ENCRYPTION_MODE::UNKNOWN;
  }

  static bool isAES128BitECBModeCipherText(
      const ByteSequence &ciphertextByteSequence,
      float ecbAverageNormalizedHammingDancingThreshold) {
    const unsigned blockSize = 16;

    auto averageNormalizedHammingDistance =
        getAverageNormalizedHammingDistance(ciphertextByteSequence, blockSize);

    if (averageNormalizedHammingDistance >
        ecbAverageNormalizedHammingDancingThreshold) {
      return false;
    }

    return true;
  }

private:
  static int randomNumberGenerationSeed;

  static float
  getAverageNormalizedHammingDistance(const ByteSequence &byteSequence,
                                      unsigned blockSize) {
    assert(byteSequence.getByteCount() % blockSize == 0);

    auto averageHammingDistance = 0.0f;

    auto blockCount = byteSequence.getByteCount() / blockSize;
    for (unsigned i = 1; i < blockCount; ++i) {
      auto subByteSequence1 =
          byteSequence.getSubSequence((i - 1) * blockSize, blockSize);
      auto subByteSequence2 =
          byteSequence.getSubSequence(i * blockSize, blockSize);

      auto normalizedHammingDistance =
          (float)subByteSequence1.getHammingDistance(subByteSequence2) /
          float(blockSize);

      averageHammingDistance += normalizedHammingDistance / float(blockCount);
    }

    return averageHammingDistance;
  }

  static ByteSequence getRandomByteSequence(unsigned byteCount) {
    const unsigned maxByteValue = 128;
    const unsigned randomByteCount = byteCount;

    std::srand(std::time(0));

    std::vector<char> randomBytes;
    for (unsigned i = 0; i < randomByteCount; ++i) {
      randomBytes.push_back(rand() % maxByteValue);
    }

    ByteSequence randomByteSequence;
    randomByteSequence.initializeFromAsciiBytes(randomBytes);

    return randomByteSequence;
  }
};

int Oracle::randomNumberGenerationSeed = std::time(0);

const float Oracle::ECB_AVERAGE_NORMALIZED_HAMMING_DISTANCE_THRESHOLD =
    2.3f; // 2.20312f

#endif // __ORACLE_HPP__
