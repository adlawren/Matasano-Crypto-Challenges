#include <vector>

#include "ByteSequence.hpp"

ByteSequence stripValidPKCSPadding(const ByteSequence& byteSequence) {
  int hypothesizedPaddingAmount =
      byteSequence.getBytes()[byteSequence.getByteCount() - 1];

  for (int i = 0; i < hypothesizedPaddingAmount; ++i) {
    if (byteSequence.getBytes()[byteSequence.getByteCount() - 1 - i] !=
        hypothesizedPaddingAmount) {
      throw std::runtime_error("Invalid PKCS padding");
    }
  }


  ByteSequence strippedByteSequence;
  strippedByteSequence.initializeFromAsciiBytes(byteSequence.getBytes());

  return strippedByteSequence.getSubSequence(
      0, strippedByteSequence.getByteCount() - hypothesizedPaddingAmount);
}

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
      auto strippedByteSequence = stripValidPKCSPadding(byteSequence);

      std::cout << "Stripped byte sequence:" << std::endl;
      strippedByteSequence.printHexEncodedAsciiString();
    } catch (std::runtime_error& re) {
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
      auto strippedByteSequence = stripValidPKCSPadding(byteSequence);

      std::cout << "Stripped byte sequence:" << std::endl;
      strippedByteSequence.printHexEncodedAsciiString();
    } catch (std::runtime_error& re) {
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
      auto strippedByteSequence = stripValidPKCSPadding(byteSequence);

      std::cout << "Stripped byte sequence:" << std::endl;
      strippedByteSequence.printHexEncodedAsciiString();
    } catch (std::runtime_error& re) {
      std::cout << "Invalid PKCS padding" << std::endl;
    }
  }

  return 0;
}
