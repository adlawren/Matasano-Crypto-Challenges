#ifndef __BYTE_SEQUENCE_HPP__
#define __BYTE_SEQUENCE_HPP__

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class ByteSequence {
 public:
  static ByteSequence getRandomByteSequence(unsigned byteCount) {
    const unsigned maxByteValue = 128;

    static int randomNumberGenerationSeed = time(0);
    srand(randomNumberGenerationSeed++);

    std::vector<char> randomBytes;
    for (unsigned i = 0; i < byteCount; ++i) {
      randomBytes.push_back(rand() % maxByteValue);
    }

    ByteSequence randomByteSequence;
    randomByteSequence.initializeFromAsciiBytes(randomBytes);

    return randomByteSequence;
  }

  static std::vector<ByteSequence> getSplitSingleByteDelimitedByteSequences(
      const ByteSequence &byteSequence, char delimeter) {
    if (byteSequence.getByteCount() == 0) {
      return std::vector<ByteSequence>{byteSequence};
    }

    int delimeterIndex = byteSequence.getByteIndex(delimeter);
    if (delimeterIndex < 0) {
      return std::vector<ByteSequence>{byteSequence};
    } else {
      std::vector<ByteSequence> delimitedByteSequences;

      delimitedByteSequences.push_back(
          byteSequence.getSubSequence(0, delimeterIndex));

      if (static_cast<unsigned>(delimeterIndex + 1) <
          byteSequence.getByteCount()) {
        auto otherdelimitedByteSequences =
            getSplitSingleByteDelimitedByteSequences(
                byteSequence.getSubSequence(delimeterIndex + 1), delimeter);

        delimitedByteSequences.insert(std::end(delimitedByteSequences),
                                      std::begin(otherdelimitedByteSequences),
                                      std::end(otherdelimitedByteSequences));
      }

      return delimitedByteSequences;
    }
  }

  static std::vector<std::pair<ByteSequence, ByteSequence>>
  getParsedTupleListFromEncodedByteSequence(
      const ByteSequence &encodedByteSequence, char primaryDelimiter,
      char secondaryDelimiter) {
    std::vector<std::pair<ByteSequence, ByteSequence>> byteSequenceTupleList;

    std::vector<ByteSequence> primaryDelimitedByteSequences =
        getSplitSingleByteDelimitedByteSequences(encodedByteSequence,
                                                 primaryDelimiter);
    for (auto primaryDelimitedByteSequence : primaryDelimitedByteSequences) {
      auto secondaryDelimitedByteSequences =
          getSplitSingleByteDelimitedByteSequences(primaryDelimitedByteSequence,
                                                   secondaryDelimiter);

      assert(secondaryDelimitedByteSequences.size() > 0);

      if (secondaryDelimitedByteSequences.size() == 1) {
        ByteSequence emptyByteSequence;
        byteSequenceTupleList.push_back(std::pair<ByteSequence, ByteSequence>(
            secondaryDelimitedByteSequences[0], emptyByteSequence));
      } else {
        byteSequenceTupleList.push_back(std::pair<ByteSequence, ByteSequence>(
            secondaryDelimitedByteSequences[0],
            secondaryDelimitedByteSequences[1]));
      }
    }

    return byteSequenceTupleList;
  }

  ByteSequence() {
    _bytes.reset(new std::vector<char>());
  }

  ByteSequence(const ByteSequence &byteSequence) {
    _bytes.reset(new std::vector<char>(*byteSequence._bytes.get()));
  }

  void appendAsciiBytes(const std::vector<char> &bytes) {
    _bytes.get()->insert(std::end(*_bytes.get()), std::begin(bytes),
                         std::end(bytes));
  }

  void appendBase64EncodedAsciiBytes(const std::vector<char> &bytes) {
    auto decodedBytes = getDecodedBase64EncodedAsciiBytes(bytes);
    _bytes.get()->insert(std::end(*_bytes.get()), std::begin(decodedBytes),
                         std::end(decodedBytes));
  }

  void appendHexEncodedAsciiBytes(const std::vector<char> &bytes) {
    auto decodedBytes = getDecodedHexEncodedAsciiBytes(bytes);
    _bytes.get()->insert(std::end(*_bytes.get()), std::begin(decodedBytes),
                         std::end(decodedBytes));
  }

  void initializeFromAsciiBytes(const std::vector<char> &bytes) {
    _bytes.reset(new std::vector<char>(bytes));
  }

  void initializeFromBase64EncodedAsciiBytes(const std::vector<char> &bytes) {
    _bytes.reset(
        new std::vector<char>(getDecodedBase64EncodedAsciiBytes(bytes)));
  }

  void initializeFromHexEncodedAsciiBytes(const std::vector<char> &bytes) {
    _bytes.reset(new std::vector<char>(getDecodedHexEncodedAsciiBytes(bytes)));
  }

  std::string getAsciiString() const {
    auto byteVector = *_bytes.get();
    std::string str(&byteVector[0], &byteVector[byteVector.size()]);

    return str;
  }

  char getByteAtIndex(unsigned index) const {
    auto byteVector = *_bytes.get();
    assert(index < byteVector.size());

    return byteVector[index];
  }

  int getByteIndex(char byte) const {
    for (unsigned i = 0; i < _bytes.get()->size(); ++i) {
      if ((*_bytes.get())[i] == byte) {
        return i;
      }
    }

    return -1;
  }

  const std::vector<char> &getBytes() const {
    return *_bytes.get();
  };

  size_t getByteCount() const {
    return _bytes.get()->size();
  }

  std::string getHexEncodedAsciiString() const {
    auto hexEncodedByteVector = getHexEncodedAsciiBytes(*_bytes.get());
    std::string str(&hexEncodedByteVector[0],
                    &hexEncodedByteVector[hexEncodedByteVector.size()]);

    return str;
  }

  unsigned getHammingDistance(const ByteSequence &byteSequence) const {
    auto otherBytes = byteSequence.getBytes();

    auto bitCount = 0;
    for (auto it1 = _bytes.get()->begin(), it2 = otherBytes.begin();
         it1 != _bytes.get()->end(); ++it1, ++it2) {
      bitCount += computeHammingDistance(*it1, *it2);
    }

    return bitCount;
  }

  ByteSequence getLowerCaseAsciiByteSequence() const {
    ByteSequence lowerCaseByteSequence;
    for (auto nextByte : *_bytes) {
      lowerCaseByteSequence.appendAsciiBytes(
          std::vector<char>{convertAsciiByteToLowerCaseAsciiByte(nextByte)});
    }

    return lowerCaseByteSequence;
  }

  ByteSequence getPaddedByteSequence(unsigned blockByteSize) const {
    ByteSequence paddedByteSequence;
    paddedByteSequence.initializeFromAsciiBytes(*_bytes.get());

    auto missingByteCount =
        blockByteSize - paddedByteSequence.getByteCount() % blockByteSize;
    if (missingByteCount != 0) {
      paddedByteSequence.appendAsciiBytes(
          std::vector<char>(missingByteCount, missingByteCount));
    }

    return paddedByteSequence;
  }

  ByteSequence getSubSequence(unsigned index, int length = -1) const {
    assert(index < _bytes.get()->size());
    auto it1 = _bytes.get()->begin() + index;

    auto it2 = length > 0 ? _bytes.get()->begin() + index + length
                          : _bytes.get()->end();

    ByteSequence subSequence;
    subSequence.initializeFromAsciiBytes(std::vector<char>(it1, it2));

    return subSequence;
  }

  ByteSequence getXoredByteSequence(const ByteSequence &byteSequence) const {
    auto xoredBytes = getXoredBytes(*_bytes.get(), *byteSequence._bytes.get());

    ByteSequence xoredByteSequence;
    xoredByteSequence._bytes.reset(new std::vector<char>(xoredBytes));

    return xoredByteSequence;
  }

  bool isAlphabeticAsciiByteSequence() const {
    for (char byte : *_bytes) {
      if (!isAlphabeticAsciiByte(byte))
        return false;
    }

    return true;
  }

  ByteSequence &operator=(const ByteSequence &byteSequence) {
    _bytes.reset(new std::vector<char>(*byteSequence._bytes.get()));

    return *this;
  }

  bool operator==(const ByteSequence &byteSequence) const {
    assert(_bytes.get()->size() == byteSequence.getByteCount());

    auto otherBytes = *byteSequence._bytes.get();
    for (unsigned i = 0; i < _bytes.get()->size(); ++i) {
      if (otherBytes[i] != (*_bytes.get())[i]) {
        return false;
      }
    }

    return true;
  }

  bool operator!=(const ByteSequence &byteSequence) const {
    return !operator==(byteSequence);
  }

  bool operator<(const ByteSequence &rhs) const {
    assert(getByteCount() == rhs.getByteCount());

    auto lhsBytes = getBytes(), rhsBytes = rhs.getBytes();
    for (unsigned i = 0; i < getByteCount(); ++i) {
      if (lhsBytes[i] < rhsBytes[i]) {
        return true;
      } else if (lhsBytes[i] > rhsBytes[i]) {
        return false;
      }
    }

    return false;
  }

  void printAsciiString() const {
    printVector<char>(*_bytes.get());
  }

  void printBase64EncodedString() const {
    auto base64EncodedBytes = getBase64EncodedAsciiBytes(*_bytes.get());

    printVector<char>(base64EncodedBytes);
  }

  void printHexEncodedAsciiString() const {
    auto hexEncodedBytes = getHexEncodedAsciiBytes(*_bytes.get());

    printVector<char>(hexEncodedBytes);
  }

  void setByteAtIndex(unsigned index, char newByteValue) {
    assert(index < _bytes.get()->size());

    (*_bytes.get())[index] = newByteValue;
  }

 private:
  std::unique_ptr<std::vector<char>> _bytes;

  static unsigned computeHammingDistance(char byte1, char byte2) {
    auto xoredByte = byte1 ^ byte2;

    auto distance = 0;
    for (auto i = 0; i < 8; ++i) {
      if (0b00000001 & xoredByte >> i) {
        ++distance;
      }
    }

    return distance;
  }

  static char convertAsciiByteToBase64Byte(char ascii) {
    if (ascii >= 'A' && ascii <= 'Z') {
      return ascii - 'A';
    }

    if (ascii >= 'a' && ascii <= 'z') {
      return ascii - 'a' + 26;
    }

    if (ascii >= '0' && ascii <= '9') {
      return ascii - '0' + 52;
    }

    if (ascii == '+')
      return 62;
    if (ascii == '/')
      return 63;

    return ascii;
  }

  static char convertAsciiByteToHexByte(char ascii) {
    if (ascii >= '0' && ascii <= '9') {
      return ascii - '0';
    }

    if (ascii >= 'a' && ascii <= 'f') {
      return 10 + ascii - 'a';
    }

    return ascii;
  }

  static char convertAsciiByteToLowerCaseAsciiByte(char ascii) {
    if (ascii >= 'A' && ascii <= 'Z') {
      return ascii - 'A' + 'a';
    }

    return ascii;
  }

  static char convertBase64ByteToAsciiByte(char base64) {
    if (base64 >= 0 && base64 < 26) {
      return base64 + 'A';
    }

    if (base64 >= 26 && base64 < 52) {
      return base64 - 26 + 'a';
    }

    if (base64 >= 52 && base64 < 62) {
      return base64 - 52 + '0';
    }

    if (base64 == 62)
      return '+';
    if (base64 == 63)
      return '/';

    return base64;
  }

  static char convertHexByteToAsciiByte(char hex) {
    if (hex >= 0 && hex < 10) {
      return hex + '0';
    }

    if (hex >= 10 && hex < 16) {
      return hex - 10 + 'a';
    }

    return hex;
  }

  static bool isAlphabeticAsciiByte(char ascii) {
    if (ascii >= 'a' && ascii <= 'z')
      return true;
    if (ascii >= 'A' && ascii <= 'Z')
      return true;

    return false;
  }

  static std::vector<char> getBase64EncodedAsciiBytes(
      const std::vector<char> &bytes) {
    std::vector<char> paddedBytes(bytes);

    const size_t unpaddedSize = bytes.size();
    if (unpaddedSize % 3 != 0) {
      for (size_t i = 0; i < 3 - (unpaddedSize % 3); ++i) {
        paddedBytes.insert(paddedBytes.end(), 0);
      }
    }

    std::vector<char> base64EncodedBytes(std::div(paddedBytes.size(), 3).quot *
                                         4);
    for (size_t i = 0, j = 0; i < base64EncodedBytes.size(); i += 4, j += 3) {
      base64EncodedBytes[i] = 0b00111111 & paddedBytes[j] >> 2;
      base64EncodedBytes[i] =
          convertBase64ByteToAsciiByte(base64EncodedBytes[i]);

      base64EncodedBytes[i + 1] = 0b00001111 & paddedBytes[j + 1] >> 4;
      base64EncodedBytes[i + 1] |= 0b00110000 & paddedBytes[j] << 4;
      base64EncodedBytes[i + 1] =
          convertBase64ByteToAsciiByte(base64EncodedBytes[i + 1]);

      base64EncodedBytes[i + 2] = 0b00111100 & paddedBytes[j + 1] << 2;
      base64EncodedBytes[i + 2] |= 0b00000011 & paddedBytes[j + 2] >> 6;
      base64EncodedBytes[i + 2] =
          convertBase64ByteToAsciiByte(base64EncodedBytes[i + 2]);

      base64EncodedBytes[i + 3] = 0b00111111 & paddedBytes[j + 2];
      base64EncodedBytes[i + 3] =
          convertBase64ByteToAsciiByte(base64EncodedBytes[i + 3]);
    }

    if (unpaddedSize % 3 != 0) {
      for (size_t i = 0; i < 3 - (unpaddedSize % 3); ++i) {
        base64EncodedBytes[base64EncodedBytes.size() - (i + 1)] = '=';
      }
    }

    return base64EncodedBytes;
  }

  static std::vector<char> getDecodedBase64EncodedAsciiBytes(
      const std::vector<char> &base64EncodedBytes) {
    assert(base64EncodedBytes.size() % 4 == 0);

    std::vector<char> decodedBytes(std::div(base64EncodedBytes.size(), 4).quot *
                                   3);
    for (size_t i = 0, j = 0; i < decodedBytes.size(); i += 3, j += 4) {
      decodedBytes[i] = convertAsciiByteToBase64Byte(base64EncodedBytes[j])
                        << 2;
      decodedBytes[i] |=
          0b00000011 &
          convertAsciiByteToBase64Byte(base64EncodedBytes[j + 1]) >> 4;

      decodedBytes[i + 1] =
          0b11110000 &
          convertAsciiByteToBase64Byte(base64EncodedBytes[j + 1]) << 4;
      decodedBytes[i + 1] |=
          0b00001111 &
          convertAsciiByteToBase64Byte(base64EncodedBytes[j + 2]) >> 2;

      decodedBytes[i + 2] =
          0b11000000 &
          convertAsciiByteToBase64Byte(base64EncodedBytes[j + 2]) << 6;
      decodedBytes[i + 2] |=
          0b00111111 & convertAsciiByteToBase64Byte(base64EncodedBytes[j + 3]);
    }

    auto paddingByteCount = 0;
    for (auto character : base64EncodedBytes) {
      if (character == '=') {
        ++paddingByteCount;
      }
    }

    decodedBytes.resize(decodedBytes.size() - paddingByteCount);

    return decodedBytes;
  }

  static std::vector<char> getDecodedHexEncodedAsciiBytes(
      const std::vector<char> &hexEncodedBytes) {
    assert(hexEncodedBytes.size() % 2 == 0);

    std::vector<char> decodedBytes(std::div(hexEncodedBytes.size(), 2).quot);
    for (size_t i = 0, j = 0; i < decodedBytes.size(); ++i, j += 2) {
      decodedBytes[i] = convertAsciiByteToHexByte(hexEncodedBytes[j]) << 4;
      decodedBytes[i] |= convertAsciiByteToHexByte(hexEncodedBytes[j + 1]);
    }

    return decodedBytes;
  }

  static std::vector<char> getHexEncodedAsciiBytes(
      const std::vector<char> &bytes) {
    std::vector<char> hexEncodedBytes(bytes.size() * 2);
    for (size_t i = 0, j = 0; i < hexEncodedBytes.size(); i += 2, ++j) {
      hexEncodedBytes[i] =
          convertHexByteToAsciiByte(0b00001111 & bytes[j] >> 4);
      hexEncodedBytes[i + 1] = convertHexByteToAsciiByte(0b00001111 & bytes[j]);
    }

    return hexEncodedBytes;
  }

  static std::vector<char> getXoredBytes(const std::vector<char> &bytes1,
                                         const std::vector<char> &bytes2) {
    assert(bytes1.size() == bytes2.size());

    std::vector<char> xoredBytes(bytes1.size());
    for (size_t i = 0; i < xoredBytes.size(); ++i) {
      xoredBytes[i] = bytes1[i] ^ bytes2[i];
    }

    return xoredBytes;
  }

  template <typename T>
  void printVector(const std::vector<T> &v) const {
    for (auto it = v.begin(); it != v.end(); ++it) {
      std::cout << *it;
    }

    std::cout << std::endl;
  }
};

#endif  // __BYTE_SEQUENCE_HPP__
