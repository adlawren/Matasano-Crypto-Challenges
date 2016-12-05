#include <fstream>
#include <string>
#include <vector>

#include "ByteSequence.hpp"
#include "CharacterFrequencyScoreCalculator.hpp"
#include "Decryptor.hpp"

int main(int argc, char *argv[]) {
  ByteSequence bestPlaintextByteSequence;
  float maxScore = 0.0;

  std::ifstream fileStream("challenge_4_ciphertext.txt");

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    ByteSequence nextByteSequence;
    nextByteSequence.initializeFromHexEncodedAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));

    auto nextKey = Decryptor::getKeyFromSingleCharacterXorEncryptedByteSequence(
        nextByteSequence);

    ByteSequence keyByteSequence;
    keyByteSequence.initializeFromAsciiBytes(
        std::vector<char>(nextByteSequence.getByteCount(), nextKey));

    auto nextPlaintextByteSequence =
        nextByteSequence.getXoredByteSequence(keyByteSequence);

    auto plainTextScore =
        CharacterFrequencyScoreCalculator::calculateByteVectorFrequencyScore(
            nextPlaintextByteSequence.getBytes());
    if (plainTextScore > maxScore) {
      bestPlaintextByteSequence = nextPlaintextByteSequence;
      maxScore = plainTextScore;
    }
  }

  bestPlaintextByteSequence.printAsciiString();

  return 0;
}
