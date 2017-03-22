#include <vector>

#include "ByteSequence.hpp"
#include "Oracle.hpp"

int main(int argc, char* argv[]) {
  std::string testString = "ICE ICE BABY";

  {
    ByteSequence byteSequence;
    byteSequence.initializeFromAsciiBytes(
        std::vector<char>(&testString[0], &testString[testString.size()]));
    byteSequence.appendAsciiBytes(std::vector<char>(4, 4));

    std::cout << "Byte sequence:" << std::endl;
    byteSequence.printHexEncodedAsciiString();

    try {
      auto strippedByteSequence = Oracle::stripPKCSPadding(byteSequence);

      std::cout << "Stripped byte sequence:" << std::endl;
      strippedByteSequence.printHexEncodedAsciiString();
    } catch (const std::runtime_error& re) {
      std::cout << "Invalid PKCS padding" << std::endl;
    }
  }

  {
    ByteSequence byteSequence;
    byteSequence.initializeFromAsciiBytes(
        std::vector<char>(&testString[0], &testString[testString.size()]));
    byteSequence.appendAsciiBytes(std::vector<char>(4, 5));

    std::cout << "Byte sequence:" << std::endl;
    byteSequence.printHexEncodedAsciiString();

    try {
      auto strippedByteSequence = Oracle::stripPKCSPadding(byteSequence);

      std::cout << "Stripped byte sequence:" << std::endl;
      strippedByteSequence.printHexEncodedAsciiString();
    } catch (const std::runtime_error& re) {
      std::cout << "Invalid PKCS padding" << std::endl;
    }
  }

  {
    ByteSequence byteSequence;
    byteSequence.initializeFromAsciiBytes(
        std::vector<char>(&testString[0], &testString[testString.size()]));
    byteSequence.appendAsciiBytes(std::vector<char>{1, 2, 3, 4});

    std::cout << "Byte sequence:" << std::endl;
    byteSequence.printHexEncodedAsciiString();

    try {
      auto strippedByteSequence = Oracle::stripPKCSPadding(byteSequence);

      std::cout << "Stripped byte sequence:" << std::endl;
      strippedByteSequence.printHexEncodedAsciiString();
    } catch (const std::runtime_error& re) {
      std::cout << "Invalid PKCS padding" << std::endl;
    }
  }

  return 0;
}
