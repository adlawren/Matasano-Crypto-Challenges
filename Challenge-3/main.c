#include <string>
#include <vector>

#include "ByteSequence.hpp"
#include "CharacterFrequencyScoreCalculator.hpp"

int main(int argc, char* argv[])
{
  std::string s1 =
    "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
  ByteSequence byteSequence1;
  byteSequence1.initializeFromHexEncodedAsciiBytes(
    std::vector<char>(&s1[0], &s1[ s1.size() ]));

  ByteSequence bestByteSequence;
  float maxScore = 0.0;
  char key = 0;

  for (auto nextCharacter = 0; nextCharacter < 256; ++nextCharacter)
  {
    ByteSequence byteSequence2;
    byteSequence2.initializeFromAsciiBytes(
      std::vector<char>(byteSequence1.getByteCount(), nextCharacter));

    auto xoredByteSequence = byteSequence1.getXoredByteSequence(byteSequence2);

    auto score =
      CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
        xoredByteSequence.getBytes());
    if (score > maxScore)
    {
      bestByteSequence = xoredByteSequence;
      maxScore = score;
      key = nextCharacter;
    }
  }

  bestByteSequence.printAsciiString();

  return 0;
}
