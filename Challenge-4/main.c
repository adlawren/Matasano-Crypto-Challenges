#include <fstream>
#include <string>
#include <vector>

#include "ByteSequence.hpp"
#include "CharacterFrequencyScoreCalculator.hpp"

int main(int argc, char* argv[])
{
  ByteSequence decryptedBestByteSequence;
  float maxScore = 0.0;

  std::ifstream fileStream("file.txt");

  std::string nextLine;
  while (std::getline(fileStream, nextLine))
  {
    ByteSequence byteSequence1;
    byteSequence1.initializeFromHexEncodedAsciiBytes(
      std::vector<char>(&nextLine[0], &nextLine[ nextLine.size() ]));

    ByteSequence bestSubSequence;
    float maxSubScore = 0.0;

    for (unsigned char nextCharacter = 0; nextCharacter < 128; ++nextCharacter)
    {
      ByteSequence byteSequence2;
      byteSequence2.initializeFromAsciiBytes(
        std::vector<char>(byteSequence1.getByteCount(), nextCharacter));

      auto xoredByteSequence =
        byteSequence1.getXoredByteSequence(byteSequence2);

      bool byteSequenceHasInvalidCharacter = false;
      for (auto byte : xoredByteSequence.getBytes())
      {
        if (byte < 0)
        {
          byteSequenceHasInvalidCharacter = true;
          break;
        }
      }

      auto score = !byteSequenceHasInvalidCharacter
        ? CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
          xoredByteSequence.getBytes())
        : 0.0;

      if (score > maxSubScore)
      {
        bestSubSequence = xoredByteSequence;
        maxSubScore = score;
      }
    }

    if (maxSubScore > maxScore)
    {
      decryptedBestByteSequence = bestSubSequence;
      maxScore = maxSubScore;
    }
  }

  decryptedBestByteSequence.printAsciiString();

  return 0;
}
