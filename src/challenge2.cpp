#include <iostream>
#include <string>
#include <vector>

#include "ByteSequence.hpp"

int main(int argc, char *argv[]) {
  std::string s1 = "1c0111001f010100061a024b53535009181c";
  ByteSequence byteSequence1;
  byteSequence1.initializeFromHexEncodedAsciiBytes(
      std::vector<char>(&s1[0], &s1[s1.size()]));

  std::string s2 = "686974207468652062756c6c277320657965";
  ByteSequence byteSequence2;
  byteSequence2.initializeFromHexEncodedAsciiBytes(
      std::vector<char>(&s2[0], &s2[s2.size()]));

  auto byteSequence3 = byteSequence1.getXoredByteSequence(byteSequence2);

  byteSequence3.printHexEncodedAsciiString();

  return 0;
}
