#ifndef __BYTE_SEQUENCE_TESTS_HPP__
#define __BYTE_SEQUENCE_TESTS_HPP__

#include <cassert>
#include <string>
#include <vector>

#include "ByteSequence.hpp"

class ByteSequenceTests
{
public:
  static void runTests()
  {
    shouldGetDecodedHexEncodedAsciiBytes();
    shouldGetHexEncodedAsciiBytes();
    shouldGetBase64EncodedAsciiBytes();
  }

private:
  static void shouldGetDecodedHexEncodedAsciiBytes()
  {
    std::string s = "68656c6c6f";
    std::vector<char> asciiBytes(&s[0], &s[ s.size() ]);
    std::vector<char> expectedDecodedBytes = { 104, 101, 108, 108, 111 };

    auto decodedBytes =
      ByteSequence::getDecodedHexEncodedAsciiBytes(asciiBytes);

    assert(decodedBytes == expectedDecodedBytes);
  }

  static void shouldGetHexEncodedAsciiBytes()
  {
    std::string s = "68656c6c6f";
    std::vector<char> expectedAsciiBytes(&s[0], &s[ s.size() ]);

    auto decodedBytes =
      ByteSequence::getDecodedHexEncodedAsciiBytes(expectedAsciiBytes);
    auto encodedBytes = ByteSequence::getHexEncodedAsciiBytes(decodedBytes);

    assert(encodedBytes == expectedAsciiBytes);
  }

  static void shouldGetBase64EncodedAsciiBytes()
  {
    std::string s1 = "68656c6c6f";
    std::vector<char> hexEncodedAsciiBytes(&s1[0], &s1[ s1.size() ]);
    std::string s2 = "AGhlbGxv";
    std::vector<char> expectedAsciiBytes(&s2[0], &s2[ s2.size() ]);

    auto decodedBytes =
      ByteSequence::getDecodedHexEncodedAsciiBytes(hexEncodedAsciiBytes);
    auto encodedBytes = ByteSequence::getBase64EncodedAsciiBytes(decodedBytes);

    assert(encodedBytes == expectedAsciiBytes);
  }
};

#endif // __BYTE_SEQUENCE_TESTS_HPP__
