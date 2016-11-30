#ifndef __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_TESTS_HPP__
#define __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_TESTS_HPP__

#include <cassert>
#include <string>
#include <vector>

#include "CharacterFrequencyScoreCalculator.hpp"

class CharacterFrequencyScoreCalculatorTests
{
public:
  static void runTests()
  {
    shouldCalculateByteVectorFrequencyScore();
    shouldCalculateLargerFrequencyScoreForValidSentence();
  }

private:
  static void shouldCalculateByteVectorFrequencyScore()
  {
    std::string s = "This is an english sentence";
    std::vector<char> bytes(&s[0], &s[ s.size() ]);

    auto score =
      CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
        bytes);

    assert(score > 0.0);
  }

  static void shouldCalculateLargerFrequencyScoreForValidSentence()
  {
    std::string s1 = "This is an english sentence, fact";
    std::vector<char> bytes1(&s1[0], &s1[ s1.size() ]);
    std::string s2 = "Ieeacdm*GI-y*fcao*k*zedn*el*hkied";
    std::vector<char> bytes2(&s2[0], &s2[ s2.size() ]);

    auto score1 =
      CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
        bytes1);
    auto score2 =
      CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
        bytes2);

    assert(score1 > score2);
  }
};

#endif // __ENGLISH_CHARACTER_FREQUENCY_SCORE_CALCULATOR_TESTS_HPP__
