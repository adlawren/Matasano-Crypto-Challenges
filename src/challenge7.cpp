#include <fstream>
#include <vector>

#include "ByteSequence.hpp"
#include "Decryptor.hpp"

int main(int argc, char *argv[]) {
  ByteSequence fileByteSequence;

  std::ifstream fileStream("challenge_7_ciphertext.txt");

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    fileByteSequence.appendBase64EncodedAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));
  }

  std::string keyString = "YELLOW SUBMARINE";

  ByteSequence keyByteSequence;
  keyByteSequence.initializeFromAsciiBytes(
      std::vector<char>(&keyString[0], &keyString[keyString.size()]));

  auto decryptedByteSequence =
      Decryptor::decryptAES128BitECBModeEncryptedByteSequence(fileByteSequence,
                                                              keyByteSequence);

  decryptedByteSequence.printAsciiString();

  return 0;
}
