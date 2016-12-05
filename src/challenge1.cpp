#include <cstdlib>
#include <fstream>
#include <vector>

#include "ByteSequence.hpp"

int main(int argc, char *argv[]) {
  ByteSequence fileByteSequence;

  std::ifstream fileStream("challenge_1_string.txt");

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    fileByteSequence.appendHexEncodedAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));
  }

  fileByteSequence.printBase64EncodedString();

  return 0;
}
