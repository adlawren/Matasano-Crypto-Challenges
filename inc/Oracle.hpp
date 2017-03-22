#ifndef __ORACLE_HPP__
#define __ORACLE_HPP__

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <map>

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

  static std::string getAESEncryptionModeString(
      AES_ENCRYPTION_MODE aesEncryptionMode) {
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

  static AES_ENCRYPTION_MODE getAESEncryptionMode(
      const std::string aesEncryptionModeString) {
    if (aesEncryptionModeString.compare(ECB_AES_ENCRYPTION_MODE_STRING) == 0) {
      return AES_ENCRYPTION_MODE::ECB;
    }

    if (aesEncryptionModeString.compare(CBC_AES_ENCRYPTION_MODE_STRING) == 0) {
      return AES_ENCRYPTION_MODE::CBC;
    }

    return AES_ENCRYPTION_MODE::UNKNOWN;
  }

  static unsigned getCiphertextBlockSize(
      const ByteSequence &ciphertextByteSequence, unsigned maxKeySizeGuess) {
    std::map<unsigned, float> keySizeToNormalizedHammingDistanceMap;
    for (int i = 2; i < (int)maxKeySizeGuess; ++i) {
      auto blockCount = std::div(ciphertextByteSequence.getByteCount(), i).quot;

      auto minimumHammingDistance = 0.0f;

      for (auto j = 0; j < blockCount - 1; ++j) {
        ByteSequence byteSequence1 =
            ciphertextByteSequence.getSubSequence(j * i, i);
        ByteSequence byteSequence2 =
            ciphertextByteSequence.getSubSequence((j + 1) * i, i);

        auto normalizedHammingDistance =
            byteSequence1.getHammingDistance(byteSequence2) / float(i);

        minimumHammingDistance += normalizedHammingDistance;
      }

      keySizeToNormalizedHammingDistanceMap[i] =
          minimumHammingDistance / (float)blockCount;
    }

    std::vector<std::pair<int, float>>
        keySizeToNormalizedHammingDistanceMapPairs(
            keySizeToNormalizedHammingDistanceMap.begin(),
            keySizeToNormalizedHammingDistanceMap.end());

    std::sort(
        keySizeToNormalizedHammingDistanceMapPairs.begin(),
        keySizeToNormalizedHammingDistanceMapPairs.end(),
        [](const std::pair<int, float> &p1, const std::pair<int, float> &p2) {
          return p1.second < p2.second;
        });

    return keySizeToNormalizedHammingDistanceMapPairs[0].first;
  }

  static unsigned getAES128BitECBModeEncryptedBackMysteryPlaintextLength(
      std::function<ByteSequence(const ByteSequence &)> encryptionFunction,
      unsigned blockSize, unsigned frontMysteryPlaintextByteCount = 0) {
    ByteSequence userByteSequence;
    userByteSequence.initializeFromAsciiBytes(std::vector<char>(1, 0));

    int firstCiphertextByteCount =
            encryptionFunction(userByteSequence).getByteCount(),
        nextCiphertextByteCount = -1;
    while (nextCiphertextByteCount <= firstCiphertextByteCount) {
      userByteSequence.appendAsciiBytes(std::vector<char>(1, 0));

      nextCiphertextByteCount =
          encryptionFunction(userByteSequence).getByteCount();
    }

    return firstCiphertextByteCount - (userByteSequence.getByteCount() - 1 +
                                       frontMysteryPlaintextByteCount);
  }

  static unsigned getAES128BitECBModeEncryptedFrontMysteryPlaintextLength(
      std::function<ByteSequence(const ByteSequence &)> encryptionFunction,
      unsigned blockSize) {
    const unsigned repeatedBlockFrequency = 10;

    auto randomByteSequence = ByteSequence::getRandomByteSequence(blockSize);

    ByteSequence userByteSequence;
    for (unsigned i = 0; i < repeatedBlockFrequency; ++i) {
      userByteSequence.appendAsciiBytes(randomByteSequence.getBytes());
    }

    auto nextCiphertextByteSequence = encryptionFunction(userByteSequence);

    int repeatedCiphertextStartIndex = -1, nextIndex = 0;
    while ((repeatedCiphertextStartIndex =
                getRepeatedCiphertextBlockIndexWithFrequencyN(
                    nextCiphertextByteSequence, blockSize,
                    repeatedBlockFrequency)) == -1) {
      userByteSequence.appendAsciiBytes(std::vector<char>(
          1, randomByteSequence
                 .getBytes()[nextIndex++ % randomByteSequence.getByteCount()]));

      nextCiphertextByteSequence = encryptionFunction(userByteSequence);
    }

    return ((repeatedCiphertextStartIndex + repeatedBlockFrequency) *
            blockSize) -
           userByteSequence.getByteCount();
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

  static ByteSequence stripPKCSPadding(const ByteSequence &byteSequence) {
    unsigned hypothesizedPaddingAmount =
        byteSequence.getBytes()[byteSequence.getByteCount() - 1];

    if (byteSequence.getByteCount() < hypothesizedPaddingAmount ||
        hypothesizedPaddingAmount == 0) {
      throw std::runtime_error("Invalid PKCS padding");
    }

    for (unsigned i = 0; i < hypothesizedPaddingAmount; ++i) {
      if (byteSequence.getBytes()[byteSequence.getByteCount() - 1 - i] !=
          (char)hypothesizedPaddingAmount) {
        throw std::runtime_error("Invalid PKCS padding");
      }
    }

    ByteSequence strippedByteSequence;
    strippedByteSequence.initializeFromAsciiBytes(byteSequence.getBytes());

    return strippedByteSequence.getSubSequence(
        0, strippedByteSequence.getByteCount() - hypothesizedPaddingAmount);
  }

 private:
  static int randomNumberGenerationSeed;

  static float getAverageNormalizedHammingDistance(
      const ByteSequence &byteSequence, unsigned blockSize) {
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

  // TODO: Rename/update this
  static int getRepeatedCiphertextBlockIndexWithFrequencyN(
      const ByteSequence &ciphertextByteSequence, unsigned blockSize,
      unsigned frequency) {
    assert(ciphertextByteSequence.getByteCount() % blockSize == 0);

    std::map<ByteSequence, std::pair<unsigned, unsigned>>
        byteSequenceIndexAndFrequencyMap;

    unsigned blockCount = ciphertextByteSequence.getByteCount() / blockSize;
    for (unsigned blockIndex = 0; blockIndex < blockCount; ++blockIndex) {
      auto nextSubsequence = ciphertextByteSequence.getSubSequence(
          blockIndex * blockSize, blockSize);

      if (byteSequenceIndexAndFrequencyMap.find(nextSubsequence) ==
          byteSequenceIndexAndFrequencyMap.end()) {
        byteSequenceIndexAndFrequencyMap[nextSubsequence].first = blockIndex;
        byteSequenceIndexAndFrequencyMap[nextSubsequence].second = 1;
      } else {
        byteSequenceIndexAndFrequencyMap[nextSubsequence].second += 1;
      }
    }

    for (auto byteSequenceFrequencyPair : byteSequenceIndexAndFrequencyMap) {
      if (byteSequenceFrequencyPair.second.second == frequency) {
        return byteSequenceFrequencyPair.second.first;
      }
    }

    return -1;
  }
};

int Oracle::randomNumberGenerationSeed = std::time(0);

const float Oracle::ECB_AVERAGE_NORMALIZED_HAMMING_DISTANCE_THRESHOLD =
    2.3f;  // 2.20312f

#endif  // __ORACLE_HPP__
