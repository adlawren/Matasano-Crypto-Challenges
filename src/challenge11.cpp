#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "ByteSequence.hpp"
#include "Encryptor.hpp"
#include "Oracle.hpp"

static const std::string generatedCiphertextFilename =
    "challenge_11_generated_ciphertext.txt";

static int randomNumberGenerationSeed = time(0);

ByteSequence getRandomByteSequence(unsigned byteCount) {
  const unsigned maxByteValue = 128;

  srand(randomNumberGenerationSeed++);

  std::vector<char> randomBytes;
  for (unsigned i = 0; i < byteCount; ++i) {
    randomBytes.push_back(rand() % maxByteValue);
  }

  ByteSequence randomByteSequence;
  randomByteSequence.initializeFromAsciiBytes(randomBytes);

  return randomByteSequence;
}

void generateAnnotatedRandomCiphertextByteSequenceFile() {
  const unsigned plaintextByteSequenceLength = 100,
                 plaintextByteSequenceCount = 1000, keyByteLength = 16;

  std::ofstream outputFileStream(generatedCiphertextFilename);

  for (unsigned i = 0; i < plaintextByteSequenceCount; ++i) {
    auto nextRandomByteSequence =
        getRandomByteSequence(plaintextByteSequenceLength);
    auto nextRandomKeyByteSequence = getRandomByteSequence(keyByteLength);

    auto randomCiphertextByteSequence =
        Oracle::getAES128BitRandomModeEncryptedByteSequence(
            nextRandomByteSequence, nextRandomKeyByteSequence);

    outputFileStream << randomCiphertextByteSequence.ciphertextByteSequence
                            .getHexEncodedAsciiString()
                     << std::endl;
    outputFileStream << Oracle::getAESEncryptionModeString(
                            randomCiphertextByteSequence.encryptionMode)
                     << std::endl;
  }
}

std::vector<std::pair<ByteSequence, AES_ENCRYPTION_MODE>>
parseGeneratedCiphertextFile() {
  std::vector<std::pair<ByteSequence, AES_ENCRYPTION_MODE>> parsedResults;

  std::ifstream fileStream(generatedCiphertextFilename);

  unsigned currentLineIndex = 0;

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    if ((currentLineIndex++ % 2) == 0) {
      ByteSequence nextCiphertextByteSequence;
      nextCiphertextByteSequence.initializeFromAsciiBytes(
          std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));

      parsedResults.push_back(std::pair<ByteSequence, AES_ENCRYPTION_MODE>(
          nextCiphertextByteSequence, AES_ENCRYPTION_MODE::UNKNOWN));
    } else {
      parsedResults[currentLineIndex / 2 - 1].second =
          Oracle::getAESEncryptionMode(nextLine);
    }
  }

  return parsedResults;
}

int main(int argc, char *argv[]) {
  auto res = parseGeneratedCiphertextFile();

  float thresholdGuess = 0.0f;
  const float thresholdGuessDelta = 0.1f;

  unsigned minErrorCount = std::numeric_limits<int>::max();
  while (true) {
    unsigned errorCount = 0;
    for (auto next : res) {
      if (Oracle::isAES128BitECBModeCipherText(next.first, thresholdGuess) &&
          next.second == AES_ENCRYPTION_MODE::ECB) {
        continue;
      } else if (!Oracle::isAES128BitECBModeCipherText(next.first,
                                                       thresholdGuess) &&
                 next.second == AES_ENCRYPTION_MODE::CBC) {
        continue;
      }

      errorCount += 1;
    }

    if (errorCount > minErrorCount) {
      break;
    }

    minErrorCount = errorCount;
    thresholdGuess += thresholdGuessDelta;
  }

  std::cout << "Threshold: " << thresholdGuess - thresholdGuessDelta
            << std::endl;

  return 0;
}
