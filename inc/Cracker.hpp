#ifndef __CRACKER_HPP__
#define __CRACKER_HPP__

#include <algorithm>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include "ByteSequence.hpp"
#include "CharacterFrequencyScoreCalculator.hpp"
#include "Encryptor.hpp"

class Cracker {
public:
  static char getKeyFromSingleCharacterXorEncryptedByteSequence(
      const ByteSequence &encryptedByteSequence) {
    char key = 0;
    float maxScore = 0.0;

    for (auto nextCharacter = 0; nextCharacter < 256; ++nextCharacter) {
      ByteSequence singleCharacterByteSequence;
      singleCharacterByteSequence.initializeFromAsciiBytes(std::vector<char>(
          encryptedByteSequence.getByteCount(), nextCharacter));

      auto xoredByteSequence = encryptedByteSequence.getXoredByteSequence(
          singleCharacterByteSequence);

      auto score =
          CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
              xoredByteSequence.getBytes());
      if (score > maxScore) {
        key = nextCharacter;
        maxScore = score;
      }
    }

    return key;
  }

  static ByteSequence getKeyFromRepeatingKeyXorEncryptedByteSequence(
      const ByteSequence &encryptedByteSequence, int maxKeySizeGuess,
      int maxKeySizeGuessCount) {
    std::map<unsigned, float> keySizeToNormalizedHammingDistanceMap;
    for (auto i = 2; i < maxKeySizeGuess; ++i) {
      auto blockCount = std::div(encryptedByteSequence.getByteCount(), i).quot;

      auto minimumHammingDistance = 0.0f;

      for (auto j = 0; j < blockCount - 1; ++j) {
        ByteSequence byteSequence1 =
            encryptedByteSequence.getSubSequence(j * i, i);
        ByteSequence byteSequence2 =
            encryptedByteSequence.getSubSequence((j + 1) * i, i);

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

    auto keySizeGuesses =
        std::min((int)keySizeToNormalizedHammingDistanceMapPairs.size(),
                 maxKeySizeGuessCount);

    std::vector<int> keySizes(keySizeGuesses);
    for (auto i = 0; i < maxKeySizeGuessCount; ++i) {
      keySizes[i] = keySizeToNormalizedHammingDistanceMapPairs[i].first;
    }

    std::vector<ByteSequence> keys(keySizes.size());
    for (size_t i = 0; i < keySizes.size(); ++i) {
      auto keySize = keySizes[i];

      std::vector<ByteSequence> fileContentBlocks;
      for (size_t j = 0; j < encryptedByteSequence.getByteCount();
           j += keySize) {
        auto blockLength = j + keySize < encryptedByteSequence.getByteCount()
                               ? keySize
                               : encryptedByteSequence.getByteCount() - j;

        ByteSequence blockByteSequence =
            encryptedByteSequence.getSubSequence(j, blockLength);
        fileContentBlocks.push_back(blockByteSequence);
      }

      std::vector<ByteSequence> singleCharacterXoredBlocks(keySize,
                                                           ByteSequence());
      for (auto blockByteSequence : fileContentBlocks) {
        const auto blockBytes = blockByteSequence.getBytes();
        for (size_t i = 0; i < blockBytes.size(); ++i) {
          singleCharacterXoredBlocks[i].appendAsciiBytes(
              std::vector<char>{blockBytes[i]});
        }
      }

      std::vector<char> keyVector;
      for (auto singleCharacterXoredBlock : singleCharacterXoredBlocks) {
        auto nextKey =
            Cracker::getKeyFromSingleCharacterXorEncryptedByteSequence(
                singleCharacterXoredBlock);

        keyVector.push_back(nextKey);
      }

      keys[i].initializeFromAsciiBytes(keyVector);
    }

    auto bestScore = 0.0f;
    ByteSequence bestKeyByteSequence;
    for (auto key : keys) {
      auto decryptedByteSequence =
          Encryptor::getRepeatingKeyXorEncryptedByteSequence(
              encryptedByteSequence, key);

      auto score =
          CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
              decryptedByteSequence.getBytes());
      if (score > bestScore) {
        bestScore = score;
        bestKeyByteSequence = key;
      }
    }

    return bestKeyByteSequence;
  }
};

#endif // __CRACKER_HPP__
