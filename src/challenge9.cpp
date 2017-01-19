#include <vector>

#include "ByteSequence.hpp"

int main(int argc, char *argv[]) {
  std::string s = "YELLOW SUBMARINE";

  ByteSequence byteSequence;
  byteSequence.initializeFromAsciiBytes(std::vector<char>(&s[0], &s[s.size()]));

  size_t paddingCount = 20;

  if (byteSequence.getByteCount() % paddingCount != 0) {
    byteSequence.appendAsciiBytes(
        std::vector<char>(paddingCount - byteSequence.getByteCount(),
                          paddingCount - byteSequence.getByteCount()));
  }

  std::cout << "Ascii:" << std::endl;
  byteSequence.printAsciiString();

  std::cout << "Hex encoded:" << std::endl;
  byteSequence.printHexEncodedAsciiString();

  return 0;
}
