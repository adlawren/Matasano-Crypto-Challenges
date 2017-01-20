#include <fstream>
#include <iostream>
#include <vector>

#include "ByteSequence.hpp"
#include "Encryptor.hpp"
#include "Decryptor.hpp"

int main(int argc, char *argv[]) {
  std::ifstream fileStream("challenge_10_ciphertext.txt");

  ByteSequence byteSequence;

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    byteSequence.appendBase64EncodedAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));
  }

  std::string keyString = "YELLOW SUBMARINE";
  ByteSequence keyByteSequence;
  keyByteSequence.initializeFromAsciiBytes(
      std::vector<char>(&keyString[0], &keyString[keyString.size()]));

  auto decryptedByteSequence =
      Decryptor::decryptAES128BitCBCModeEncryptedByteSequence(byteSequence,
                                                              keyByteSequence);

  decryptedByteSequence.printAsciiString();

  return 0;
}
