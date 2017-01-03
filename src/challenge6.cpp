#include <fstream>
#include <vector>

#include "ByteSequence.hpp"
#include "Cracker.hpp"
#include "Encryptor.hpp"

int main(int argc, char *argv[]) {
  ByteSequence fileByteSequence;

  std::ifstream fileStream("challenge_6_ciphertext.txt");

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    fileByteSequence.appendBase64EncodedAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));
  }

  auto keyByteSequence =
      Cracker::getKeyFromRepeatingKeyXorEncryptedByteSequence(fileByteSequence,
                                                              100, 3);

  auto plaintextByteSequence =
      Encryptor::getRepeatingKeyXorEncryptedByteSequence(fileByteSequence,
                                                         keyByteSequence);

  plaintextByteSequence.printAsciiString();

  return 0;
}
