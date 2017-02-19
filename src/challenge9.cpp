#include <vector>

#include "ByteSequence.hpp"

int main(int argc, char *argv[]) {
  std::string s = "YELLOW SUBMARINE";

  ByteSequence byteSequence;
  byteSequence.initializeFromAsciiBytes(std::vector<char>(&s[0], &s[s.size()]));

  auto paddedByteSequence = byteSequence.getPaddedByteSequence(20);

  std::cout << "Ascii:" << std::endl;
  paddedByteSequence.printAsciiString();

  std::cout << "Hex encoded:" << std::endl;
  paddedByteSequence.printHexEncodedAsciiString();

  return 0;
}
