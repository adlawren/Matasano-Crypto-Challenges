#ifndef __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_HPP__
#define __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_HPP__

#include <cmath>
#include <map>
#include <vector>

class CharacterFrequencyScoreCalculator {
public:
  static float
  calculateByteVectorFrequencyScore(const std::vector<char> &bytes) {
    float alphabeticCharacterCount = (float)getAlphabeticCharacterCount(bytes);

    auto frequencyMap = createEmptyFrequencyMap();
    for (auto nextCharacter : bytes) {
      nextCharacter = getLowerCaseAsciiByte(nextCharacter);

      if (frequencyMap.find(nextCharacter) != frequencyMap.end()) {
        frequencyMap[nextCharacter] += 1 / alphabeticCharacterCount;
      } else if (isWhitelistedCharacter(nextCharacter)) {
        frequencyMap[nextCharacter] += 0.5f / (float)bytes.size();
      } else if (!isSpace(nextCharacter)) {
        frequencyMap[nextCharacter] += 1 / (float)bytes.size();
      }
    }

    auto naturalFrequencyMap = createNaturalFrequencyMap();

    float preScore = 0.0;
    for (auto it1 = frequencyMap.begin(); it1 != frequencyMap.end(); ++it1) {
      preScore += std::abs(it1->second - naturalFrequencyMap[it1->first]);
    }

    return 1 / preScore;
  }

private:
  static std::vector<float>
  createFrequencyVector(const std::map<char, float> &frequencyMap) {
    std::vector<float> frequencyVector;
    for (auto pair : frequencyMap) {
      frequencyVector.push_back(pair.second);
    }

    return frequencyVector;
  }

  static std::map<char, float> createNaturalFrequencyMap() {
    std::map<char, float> naturalFrequencyMap;

    naturalFrequencyMap['a'] = 0.08167;
    naturalFrequencyMap['b'] = 0.01492;
    naturalFrequencyMap['c'] = 0.02782;
    naturalFrequencyMap['d'] = 0.04253;
    naturalFrequencyMap['e'] = 0.12702;

    naturalFrequencyMap['f'] = 0.02228;
    naturalFrequencyMap['g'] = 0.02015;
    naturalFrequencyMap['h'] = 0.06094;
    naturalFrequencyMap['i'] = 0.06966;
    naturalFrequencyMap['j'] = 0.00153;

    naturalFrequencyMap['k'] = 0.00772;
    naturalFrequencyMap['l'] = 0.04025;
    naturalFrequencyMap['m'] = 0.02406;
    naturalFrequencyMap['n'] = 0.06749;
    naturalFrequencyMap['o'] = 0.07507;

    naturalFrequencyMap['p'] = 0.01929;
    naturalFrequencyMap['q'] = 0.00095;
    naturalFrequencyMap['r'] = 0.05987;
    naturalFrequencyMap['s'] = 0.06327;
    naturalFrequencyMap['t'] = 0.09056;

    naturalFrequencyMap['u'] = 0.02758;
    naturalFrequencyMap['v'] = 0.00978;
    naturalFrequencyMap['w'] = 0.02360;
    naturalFrequencyMap['x'] = 0.00150;
    naturalFrequencyMap['y'] = 0.01974;

    naturalFrequencyMap['z'] = 0.00074;

    return naturalFrequencyMap;
  }

  static std::map<char, float> createEmptyFrequencyMap() {
    std::map<char, float> emptyFrequencyMap;

    emptyFrequencyMap['a'] = 0.0;
    emptyFrequencyMap['b'] = 0.0;
    emptyFrequencyMap['c'] = 0.0;
    emptyFrequencyMap['d'] = 0.0;
    emptyFrequencyMap['e'] = 0.0;

    emptyFrequencyMap['f'] = 0.0;
    emptyFrequencyMap['g'] = 0.0;
    emptyFrequencyMap['h'] = 0.0;
    emptyFrequencyMap['i'] = 0.0;
    emptyFrequencyMap['j'] = 0.0;

    emptyFrequencyMap['k'] = 0.0;
    emptyFrequencyMap['l'] = 0.0;
    emptyFrequencyMap['m'] = 0.0;
    emptyFrequencyMap['n'] = 0.0;
    emptyFrequencyMap['o'] = 0.0;

    emptyFrequencyMap['p'] = 0.0;
    emptyFrequencyMap['q'] = 0.0;
    emptyFrequencyMap['r'] = 0.0;
    emptyFrequencyMap['s'] = 0.0;
    emptyFrequencyMap['t'] = 0.0;

    emptyFrequencyMap['u'] = 0.0;
    emptyFrequencyMap['v'] = 0.0;
    emptyFrequencyMap['w'] = 0.0;
    emptyFrequencyMap['x'] = 0.0;
    emptyFrequencyMap['y'] = 0.0;

    emptyFrequencyMap['z'] = 0.0;

    return emptyFrequencyMap;
  }

  static size_t getAlphabeticCharacterCount(const std::vector<char> &bytes) {
    size_t alphaNumericCharacterCount = 0;
    for (auto nextCharacter : bytes) {
      if (nextCharacter >= 'a' && nextCharacter <= 'z') {
        ++alphaNumericCharacterCount;
        continue;
      }

      if (nextCharacter >= 'A' && nextCharacter <= 'Z') {
        ++alphaNumericCharacterCount;
        continue;
      }
    }

    return alphaNumericCharacterCount;
  }

  static char getLowerCaseAsciiByte(char asciiByte) {
    if (asciiByte >= 'A' && asciiByte <= 'Z') {
      return asciiByte - 'A' + 'a';
    }

    return asciiByte;
  }

  static bool isSpace(char c) { return c == ' '; }

  static bool isWhitelistedCharacter(char c) {
    switch (c) {
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

#endif // __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_HPP__
