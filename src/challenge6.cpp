#include <fstream>
#include <vector>

#include "ByteSequence.hpp"
#include "Decryptor.hpp"
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
      Decryptor::getKeyFromRepeatingKeyXorEncryptedByteSequence(
          fileByteSequence);

  auto plaintextByteSequence =
      Encryptor::getRepeatingKeyXorEncryptedByteSequence(fileByteSequence,
                                                         keyByteSequence);

  plaintextByteSequence.printAsciiString();

  return 0;
}
