#include <fstream>
#include <iostream>
#include <limits>

#include "ByteSequence.hpp"
#include "Cracker.hpp"

int main() {
  std::ifstream fileStream("challenge_20_ciphertext.txt");

  std::vector<ByteSequence> ciphertextByteSequences;

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    ByteSequence nextCiphertextByteSequence;
    nextCiphertextByteSequence.initializeFromBase64EncodedAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));

    ciphertextByteSequences.push_back(nextCiphertextByteSequence);
  }

  unsigned shortestByteSequenceLength = std::numeric_limits<unsigned>::max();
  for (auto ciphertextByteSequence : ciphertextByteSequences) {
    if (ciphertextByteSequence.getByteCount() < shortestByteSequenceLength) {
      shortestByteSequenceLength = ciphertextByteSequence.getByteCount();
    }
  }

  ByteSequence concatenatedCiphertextByteSequence;
  for (auto ciphertextByteSequence : ciphertextByteSequences) {
    concatenatedCiphertextByteSequence.appendAsciiBytes(
        ciphertextByteSequence.getSubSequence(0, shortestByteSequenceLength)
            .getBytes());
  }

  auto repeatedKeyXorKey =
      Cracker::getSizedKeyFromRepeatingKeyXorEncryptedByteSequence(
          concatenatedCiphertextByteSequence, shortestByteSequenceLength);

  // for (unsigned i = 0; i < concatenatedCiphertextByteSequence.getByteCount()
  // /
  //                              shortestByteSequenceLength;
  //      i += shortestByteSequenceLength) {
  for (unsigned i = 0; i < concatenatedCiphertextByteSequence.getByteCount();
       i += shortestByteSequenceLength) {
    auto trimmedCiphertextByteSequence =
        concatenatedCiphertextByteSequence.getSubSequence(
            i, shortestByteSequenceLength);

    // std::cout << repeatedKeyXorKey.getByteCount() << " vs. "
    //           << trimmedCiphertextByteSequence.getByteCount() << std::endl;

    auto crackedPlaintextByteSequence =
        trimmedCiphertextByteSequence.getXoredByteSequence(repeatedKeyXorKey);

    crackedPlaintextByteSequence.printAsciiString();
  }

  return 0;
}
