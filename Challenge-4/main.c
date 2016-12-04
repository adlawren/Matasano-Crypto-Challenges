#include <ctime>

#include <fstream>
#include <string>
#include <vector>

#include "ByteSequence.hpp"
#include "CharacterFrequencyScoreCalculator.hpp"

void generateTestFile()
{
  ByteSequence bs1;

  std::string s = "This a string containing texts";
  bs1.initializeFromAsciiBytes(std::vector<char>(&s[0], &s[ s.size() ]));
  // bs1.printAsciiString();
  // bs1.printHexEncodedAsciiString();

  ByteSequence bs2;

  std::vector<char> characterVector(30, 'x');
  bs2.initializeFromAsciiBytes(characterVector);

  bs1 = bs1.getXoredByteSequence(bs2);

  std::ofstream file("file3.txt");

  file << bs1.getHexEncodedAsciiString() << std::endl;

  std::srand(std::time(0));
  for (auto i = 0; i < 1024; ++i)
  {
    std::vector<char> v;
    for (auto j = 0; j < 30; ++j)
    {
      v.push_back(std::rand());
    }

    ByteSequence bs3;
    bs3.initializeFromAsciiBytes(v);

    file << bs3.getHexEncodedAsciiString() << std::endl;
  }
}

void runTest()
{
  std::string s = "This a string containing texts";

  ByteSequence bs1;
  bs1.initializeFromAsciiBytes(std::vector<char>(&s[0], &s[ s.size() ]));
  bs1.printAsciiString();

  std::string s2 =
    "0d263bd6f8f6329323486bbe31cf2449985452d75941fa4e65c1a2089c4f";

  ByteSequence bs2;
  bs2.initializeFromHexEncodedAsciiBytes(
    std::vector<char>(&s2[0], &s2[ s2.size() ]));
  bs2.printAsciiString();

  auto score1 =
    CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
      bs1.getBytes());

  std::cout << score1 << std::endl;

  auto score2 =
    CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
      bs2.getBytes());

  std::cout << score2 << std::endl;
}

int main(int argc, char* argv[])
{
  runTest();

  return 0;

  // generateTestFile();

  static int test = 0;

  ByteSequence bestByteSequence, decryptedByteSequence;
  float maxScore = 0.0;

  std::ifstream fileStream("file.txt");
  // std::ifstream fileStream("file3.txt");

  std::string nextLine;
  while (std::getline(fileStream, nextLine))
  {
    ByteSequence byteSequence1;
    byteSequence1.initializeFromHexEncodedAsciiBytes(
      std::vector<char>(&nextLine[0], &nextLine[ nextLine.size() ]));

    // byteSequence1.printHexEncodedAsciiString();

    ByteSequence bestSubSequence;
    float maxSubScore = 0.0;

    for (unsigned char nextCharacter = 0; nextCharacter < 128; ++nextCharacter)
    {
      ++test;

      ByteSequence byteSequence2;
      byteSequence2.initializeFromAsciiBytes(
        std::vector<char>(byteSequence1.getByteCount(), nextCharacter));

      // byteSequence2.printAsciiString();

      auto xoredByteSequence =
        byteSequence1.getXoredByteSequence(byteSequence2);

      bool byteSequenceHasInvalidCharacter = false;
      for (auto byte : xoredByteSequence.getBytes())
      {
        if (byte < 0)
        {
          --test;

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
      bestByteSequence = byteSequence1;
      decryptedByteSequence = bestSubSequence;

      maxScore = maxSubScore;
    }
  }

  bestByteSequence.printHexEncodedAsciiString();
  bestByteSequence.printAsciiString();
  decryptedByteSequence.printHexEncodedAsciiString();
  decryptedByteSequence.printAsciiString();

  std::cout << maxScore << std::endl;

  std::cout << test << std::endl;

  // runTest();

  return 0;
}
