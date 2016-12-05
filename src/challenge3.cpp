#include <fstream>
#include <string>
#include <vector>

#include "ByteSequence.hpp"
#include "Decryptor.hpp"

int main(int argc, char *argv[]) {
  ByteSequence fileByteSequence;

  std::ifstream fileStream("challenge_3_ciphertext.txt");

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    fileByteSequence.appendHexEncodedAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));
  }

  auto key = Decryptor::getKeyFromSingleCharacterXorEncryptedByteSequence(
      fileByteSequence);

  ByteSequence keyByteSequence;
  keyByteSequence.initializeFromAsciiBytes(
      std::vector<char>(fileByteSequence.getByteCount(), key));

  auto plaintextByteSequence =
      fileByteSequence.getXoredByteSequence(keyByteSequence);

  plaintextByteSequence.printAsciiString();

  return 0;
}
