#ifndef __CRACKER_HPP__
#define __CRACKER_HPP__

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <algorithm>
#include <cstring>
#include <functional>

#include "BoundedBuffer.hpp"
#include "ByteSequence.hpp"
#include "EnglishCharacterDistributionScoreCalculator.hpp"

#include "Encryptor.hpp"
#include "Oracle.hpp"

class Cracker {
 public:
  static uint16_t get16BitSeedFromMT19937CiphertextByteSequence(
      const ByteSequence &ciphertextByteSequence,
      const ByteSequence &cribByteSequence) {
    unsigned maxSeed = std::numeric_limits<char16_t>::max();
    for (unsigned nextSeed = 0; nextSeed < maxSeed; ++nextSeed) {
      auto decryptedByteSequence = Encryptor::getMT19937EncryptedByteSequence(
          ciphertextByteSequence, nextSeed);
      if (decryptedByteSequence.containsSubSequence(cribByteSequence))
        return nextSeed;
    }

    throw std::runtime_error("Error: could not crack MT19937 seed");
  }

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

      auto score = EnglishCharacterDistributionScoreCalculator::
          calculateByteSequenceMonogramFrequencyScore(xoredByteSequence);
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
          minimumHammingDistance / static_cast<float>(blockCount);
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

    auto keySizeGuesses = std::min(
        static_cast<int>(keySizeToNormalizedHammingDistanceMapPairs.size()),
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

      auto score = EnglishCharacterDistributionScoreCalculator::
          calculateByteSequenceMonogramFrequencyScore(decryptedByteSequence);
      if (score > bestScore) {
        bestScore = score;
        bestKeyByteSequence = key;
      }
    }

    return bestKeyByteSequence;
  }

  static ByteSequence getSizedKeyFromRepeatingKeyXorEncryptedByteSequence(
      const ByteSequence &encryptedByteSequence, unsigned keySize) {
    std::vector<ByteSequence> ciphertextColumnByteSequences(keySize);
    for (unsigned i = 0; i < encryptedByteSequence.getByteCount();
         i += keySize) {
      auto subSequence = encryptedByteSequence.getSubSequence(i, i + keySize);
      for (unsigned j = 0; j < keySize; ++j) {
        ciphertextColumnByteSequences[j].appendAsciiBytes(
            std::vector<char>{subSequence.getBytes()[j]});
      }
    }

    // Crack the keystream bytes
    ByteSequence crackedKeyStreamByteSequence;
    for (auto ciphertextColumnByteSequence : ciphertextColumnByteSequences) {
      char keyByte = getKeyFromSingleCharacterXorEncryptedByteSequence(
          ciphertextColumnByteSequence);
      crackedKeyStreamByteSequence.appendAsciiBytes(std::vector<char>{keyByte});
    }

    return crackedKeyStreamByteSequence;
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

  static char getNextKeystreamByteFromFixedNonceStreamEncryptedByteSequences(
      const std::vector<ByteSequence> &ciphertextByteSequences,
      const ByteSequence &existingCrackedKeystreamByteSequence) {
    // Validate ciphertext byte sequence lengths
    for (auto ciphertextByteSequence : ciphertextByteSequences) {
      assert(ciphertextByteSequence.getByteCount() ==
             existingCrackedKeystreamByteSequence.getByteCount() + 1);
    }

    // Construct the ciphertext "column"
    //  - i.e. the list of the last bytes from each ciphertext byte sequence
    ByteSequence ciphertextColumnByteSequence;
    for (auto ciphertextByteSequence : ciphertextByteSequences) {
      ciphertextColumnByteSequence.appendAsciiBytes(
          std::vector<char>{ciphertextByteSequence.getBytes().back()});
    }

    char key = 0;
    float maxScore = 0.0;
    for (auto nextCharacter = 0; nextCharacter < 256; ++nextCharacter) {
      ByteSequence singleCharacterByteSequence;
      singleCharacterByteSequence.initializeFromAsciiBytes(std::vector<char>(
          ciphertextColumnByteSequence.getByteCount(), nextCharacter));

      auto xoredByteSequence =
          ciphertextColumnByteSequence.getXoredByteSequence(
              singleCharacterByteSequence);

      auto score = EnglishCharacterDistributionScoreCalculator::
          calculateByteSequenceMonogramFrequencyScore(xoredByteSequence);

      //// Score the cracked byte sequences that would be generated if this key
      //// were used
      ByteSequence updatedKeystreamByteSequence =
          existingCrackedKeystreamByteSequence;
      updatedKeystreamByteSequence.appendAsciiBytes(
          std::vector<char>{static_cast<char>(nextCharacter)});

      // Compute the cracked byte sequences that would be generated if this key
      // were used
      std::vector<ByteSequence> crackedPlaintextByteSequenceGuesses;
      for (auto ciphertextByteSequence : ciphertextByteSequences) {
        crackedPlaintextByteSequenceGuesses.push_back(
            ciphertextByteSequence.getXoredByteSequence(
                updatedKeystreamByteSequence));
      }

      // Score the monograms in the cracked byte sequences
      for (auto ciphertextByteSequence : ciphertextByteSequences) {
        score += EnglishCharacterDistributionScoreCalculator::
            calculateByteSequenceMonogramFrequencyScore(
                ciphertextByteSequence.getXoredByteSequence(
                    updatedKeystreamByteSequence));
      }

      // Score the bigrams in the cracked byte sequences
      float bigramScoreScalingFactor = 10.0f;
      for (auto crackedPlaintextByteSequenceGuess :
           crackedPlaintextByteSequenceGuesses) {
        score += EnglishCharacterDistributionScoreCalculator::
                     calculateByteSequenceBigramFrequencyScore(
                         crackedPlaintextByteSequenceGuess) *
                 bigramScoreScalingFactor;
      }

      // Score the trigrams in the cracked byte sequences
      float trigramScoreScalingFactor = bigramScoreScalingFactor * 10.0f;
      for (auto crackedPlaintextByteSequenceGuess :
           crackedPlaintextByteSequenceGuesses) {
        score += EnglishCharacterDistributionScoreCalculator::
                     calculateByteSequenceTrigramFrequencyScore(
                         crackedPlaintextByteSequenceGuess) *
                 trigramScoreScalingFactor;
      }

      if (score > maxScore) {
        key = nextCharacter;
        maxScore = score;
      }
    }

    return key;
  }

  static bool isMT19937GeneratedPasswordTokenByteSequence(
      const ByteSequence &passwordTokenByteSequence) {
    const unsigned GUESS_COUNT = 100000;

    unsigned nextSeedGuess = time(0);
    for (unsigned i = 0; i < GUESS_COUNT; ++i) {
      ByteSequence passwordTokenByteSequenceGuess =
          Encryptor::getMT19937GeneratedPasswordTokenByteSequence(
              nextSeedGuess);
      if (passwordTokenByteSequence == passwordTokenByteSequenceGuess)
        return true;
      --nextSeedGuess;
    }

    return false;
  }
};

#endif  // __CRACKER_HPP__
