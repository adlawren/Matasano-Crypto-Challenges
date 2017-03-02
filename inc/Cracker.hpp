#ifndef __CRACKER_HPP__
#define __CRACKER_HPP__

#include <algorithm>
#include <functional>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include "BoundedBuffer.hpp"
#include "ByteSequence.hpp"
#include "CharacterFrequencyScoreCalculator.hpp"
#include "Encryptor.hpp"
#include "Oracle.hpp"

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

  static ByteSequence getAES128BitECBModeEncryptedMysteryPlaintextByteSequence(
      std::function<ByteSequence(const ByteSequence &)> encryptionFunction) {
    static const char MAX_CHARACTER_GUESS = 127;

    // Determine block size
    unsigned computedBlockSize = 0;
    {
      const unsigned userByteSequenceLength = 1024;

      ByteSequence userByteSequence;
      userByteSequence.initializeFromAsciiBytes(
          std::vector<char>(userByteSequenceLength, 0));

      // Assert that the ciphertext is AES ECB mode encrypted
      auto ciphertextByteSequence = encryptionFunction(userByteSequence);

      assert(Oracle::isAES128BitECBModeCipherText(
          ciphertextByteSequence,
          Oracle::ECB_AVERAGE_NORMALIZED_HAMMING_DISTANCE_THRESHOLD));

      computedBlockSize =
          Oracle::getCiphertextBlockSize(ciphertextByteSequence, 20);
    }

    unsigned frontMysteryPlaintextByteCount =
        Oracle::getAES128BitECBModeEncryptedFrontMysteryPlaintextLength(
            encryptionFunction, computedBlockSize);

    unsigned backMysteryPlaintextByteCount =
        Oracle::getAES128BitECBModeEncryptedBackMysteryPlaintextLength(
            encryptionFunction, computedBlockSize,
            frontMysteryPlaintextByteCount);

    ByteSequence decryptedMysteryPlaintext;
    {
      // Bounded buffer associated with the last (block size - 1) worth
      // of decrypted plaintext bytes
      BoundedBuffer<char> lastDecryptedPlaintextBytesBuffer(computedBlockSize -
                                                            1);

      for (unsigned mysteryPlaintextByteIndex = 0;
           mysteryPlaintextByteIndex < backMysteryPlaintextByteCount;
           ++mysteryPlaintextByteIndex) {
        // Compute the expected ciphertext
        unsigned nextUserByteSequenceLength =
            computedBlockSize - 1 -
            (mysteryPlaintextByteIndex % computedBlockSize);

        ByteSequence userByteSequence;
        userByteSequence.appendAsciiBytes(std::vector<char>(
            computedBlockSize -
                (frontMysteryPlaintextByteCount % computedBlockSize),
            0));
        userByteSequence.appendAsciiBytes(
            std::vector<char>(nextUserByteSequenceLength, 0));

        auto expectedCiphertextByteSequence =
            encryptionFunction(userByteSequence)
                .getSubSequence(
                    ((frontMysteryPlaintextByteCount / computedBlockSize) + 1 +
                     (mysteryPlaintextByteIndex / computedBlockSize)) *
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

        char nextCharacter = 0;
        for (; nextCharacter < MAX_CHARACTER_GUESS; ++nextCharacter) {
          ByteSequence nextPlaintextByteSequenceGuess;
          nextPlaintextByteSequenceGuess.appendAsciiBytes(std::vector<char>(
              computedBlockSize -
                  (frontMysteryPlaintextByteCount % computedBlockSize),
              0));
          nextPlaintextByteSequenceGuess.appendAsciiBytes(
              userByteSequence.getBytes());
          nextPlaintextByteSequenceGuess.appendAsciiBytes(
              std::vector<char>(1, nextCharacter));

          auto nextCiphertextByteSequenceGuess =
              encryptionFunction(nextPlaintextByteSequenceGuess)
                  .getSubSequence(
                      ((frontMysteryPlaintextByteCount / computedBlockSize) +
                       1) *
                          computedBlockSize,
                      computedBlockSize);

          if (nextCiphertextByteSequenceGuess ==
              expectedCiphertextByteSequence) {
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

    return decryptedMysteryPlaintext;
  }
};

#endif // __CRACKER_HPP__
