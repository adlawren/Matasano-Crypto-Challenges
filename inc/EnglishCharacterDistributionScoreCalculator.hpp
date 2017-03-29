#ifndef __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_HPP__
#define __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_HPP__

#include <cmath>
#include <map>
#include <vector>

#include "ByteSequence.hpp"
#include "EnglishMonogramRelativeFrequencyProvider.hpp"
#include "EnglishBigramRelativeFrequencyProvider.hpp"
#include "EnglishTrigramRelativeFrequencyProvider.hpp"

class EnglishCharacterDistributionScoreCalculator {
 public:
  static float calculateByteSequenceMonogramFrequencyScore(
      const ByteSequence &byteSequence) {
    float alphabeticCharacterCount =
        static_cast<float>(getAlphabeticCharacterCount(byteSequence));
    float totalCharacterCount = static_cast<float>(byteSequence.getByteCount());

    std::map<ByteSequence, float> monogramRelativeFrequencyMap;

    ByteSequence lowerCaseAsciiByteSequence =
        byteSequence.getLowerCaseAsciiByteSequence();
    for (unsigned i = 0; i < lowerCaseAsciiByteSequence.getByteCount(); ++i) {
      ByteSequence monogramByteSequence =
          lowerCaseAsciiByteSequence.getSubSequence(i, 1);
      if (isAlphabeticAsciiByteSequence(monogramByteSequence)) {
        monogramRelativeFrequencyMap[monogramByteSequence] +=
            1 / alphabeticCharacterCount;
      } else if (isWhitelistedMonogramByteSequence(monogramByteSequence)) {
        monogramRelativeFrequencyMap[monogramByteSequence] +=
            0.5f / totalCharacterCount;
      } else if (!isSpaceMonogramByteSequence(monogramByteSequence)) {
        monogramRelativeFrequencyMap[monogramByteSequence] +=
            1 / totalCharacterCount;
      }
    }

    float preScore = 0.0;
    for (auto it1 = monogramRelativeFrequencyMap.begin();
         it1 != monogramRelativeFrequencyMap.end(); ++it1) {
      if (EnglishMonogramRelativeFrequencyProvider::getInstance()
              .hasMonogramRelativeFrequency(it1->first)) {
        preScore +=
            std::abs(it1->second -
                     EnglishMonogramRelativeFrequencyProvider::getInstance()
                         .getMonogramRelativeFrequency(it1->first));
      } else {
        preScore += it1->second;  // equivalent to: it1->second - 0.0
      }
    }

    return 1 / preScore;
  }

  static float calculateByteSequenceBigramFrequencyScore(
      const ByteSequence &byteSequence) {
    float alphabeticBigramCount =
        static_cast<float>(getAlphabeticBigramCount(byteSequence));

    std::map<ByteSequence, float> bigramRelativeFrequencyMap;

    ByteSequence lowerCaseAsciiByteSequence =
        byteSequence.getLowerCaseAsciiByteSequence();
    for (unsigned i = 0; i < lowerCaseAsciiByteSequence.getByteCount() - 1;
         ++i) {
      ByteSequence bigramByteSequence =
          lowerCaseAsciiByteSequence.getSubSequence(i, 2);

      if (bigramByteSequence.isAlphabeticAsciiByteSequence()) {
        bigramRelativeFrequencyMap[bigramByteSequence] +=
            1 / alphabeticBigramCount;
      }
    }

    float preScore = 0.0;
    for (auto it1 = bigramRelativeFrequencyMap.begin();
         it1 != bigramRelativeFrequencyMap.end(); ++it1) {
      if (EnglishBigramRelativeFrequencyProvider::getInstance()
              .hasBigramRelativeFrequency(it1->first)) {
        preScore +=
            std::abs(it1->second -
                     EnglishBigramRelativeFrequencyProvider::getInstance()
                         .getBigramRelativeFrequency(it1->first));
      } else {
        preScore += it1->second;  // equivalent to: it1->second - 0.0
      }
    }

    return 1 / preScore;
  }

  static float calculateByteSequenceTrigramFrequencyScore(
      const ByteSequence &byteSequence) {
    float alphabeticTrigramCount =
        static_cast<float>(getAlphabeticTrigramCount(byteSequence));

    std::map<ByteSequence, float> trigramRelativeFrequencyMap;

    ByteSequence lowerCaseAsciiByteSequence =
        byteSequence.getLowerCaseAsciiByteSequence();
    for (unsigned i = 0; i < lowerCaseAsciiByteSequence.getByteCount() - 2;
         ++i) {
      ByteSequence trigramByteSequence =
          lowerCaseAsciiByteSequence.getSubSequence(i, 3);

      if (trigramByteSequence.isAlphabeticAsciiByteSequence()) {
        trigramRelativeFrequencyMap[trigramByteSequence] +=
            1 / alphabeticTrigramCount;
      }
    }

    float preScore = 0.0;
    for (auto it1 = trigramRelativeFrequencyMap.begin();
         it1 != trigramRelativeFrequencyMap.end(); ++it1) {
      if (EnglishTrigramRelativeFrequencyProvider::getInstance()
              .hasTrigramRelativeFrequency(it1->first)) {
        preScore +=
            std::abs(it1->second -
                     EnglishTrigramRelativeFrequencyProvider::getInstance()
                         .getTrigramRelativeFrequency(it1->first));
      } else {
        preScore += it1->second;  // equivalent to: it1->second - 0.0
      }
    }

    return 1 / preScore;
  }

 private:
  static size_t getAlphabeticBigramCount(const ByteSequence &byteSequence) {
    size_t alphabeticBigramCount = 0;
    for (unsigned i = 0; i < byteSequence.getByteCount() - 1; ++i) {
      if (byteSequence.getSubSequence(i, 2).isAlphabeticAsciiByteSequence())
        ++alphabeticBigramCount;
    }

    return alphabeticBigramCount;
  }

  static size_t getAlphabeticCharacterCount(const ByteSequence &byteSequence) {
    size_t alphabeticCharacterCount = 0;

    auto bytes = byteSequence.getBytes();
    for (auto nextCharacter : bytes) {
      if (nextCharacter >= 'a' && nextCharacter <= 'z') {
        ++alphabeticCharacterCount;
        continue;
      }

      if (nextCharacter >= 'A' && nextCharacter <= 'Z') {
        ++alphabeticCharacterCount;
        continue;
      }
    }

    return alphabeticCharacterCount;
  }

  static size_t getAlphabeticTrigramCount(const ByteSequence &byteSequence) {
    size_t alphabeticTrigramCount = 0;
    for (unsigned i = 0; i < byteSequence.getByteCount() - 2; ++i) {
      if (byteSequence.getSubSequence(i, 3).isAlphabeticAsciiByteSequence())
        ++alphabeticTrigramCount;
    }

    return alphabeticTrigramCount;
  }

  static bool isAlphabeticAsciiByteSequence(const ByteSequence &byteSequence) {
    return getAlphabeticCharacterCount(byteSequence) ==
           byteSequence.getByteCount();
  }

  static bool isSpaceMonogramByteSequence(
      const ByteSequence &monogramByteSequence) {
    assert(monogramByteSequence.getByteCount() == 1);

    return monogramByteSequence.getBytes()[0] == ' ';
  }

  static bool isWhitelistedMonogramByteSequence(
      const ByteSequence &monogramByteSequence) {
    assert(monogramByteSequence.getByteCount() == 1);

    switch (monogramByteSequence.getBytes()[0]) {
      case '!':
      case '\"':
      case '\'':
      case '.':
      case ':':
      case ';':
      case ',':
      case '?':
        return true;
      default:
        return false;
    }
  }
};

#endif  // __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_HPP__
