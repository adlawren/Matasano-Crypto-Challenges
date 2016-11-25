#ifndef __BYTE_SEQUENCE_HPP__
#define __BYTE_SEQUENCE_HPP__

#include <iostream>
#include <memory>
#include <vector>

class ByteSequence
{
public:
  void initializeFromHexEncodedAsciiBytes(const std::vector<char>& bytes)
  {
    _bytes.reset(new std::vector<char>(getDecodedHexEncodedAsciiBytes(bytes)));
  }

  const std::vector<char>& getBytes() const { return *_bytes.get(); };

  void printAsciiString() const
  {
    printVector<char>(*_bytes.get());
  }

  void printBase64EncodedString() const
  {
    auto base64EncodedBytes = getBase64EncodedAsciiBytes(*_bytes.get());

    printVector<char>(base64EncodedBytes);
  }

  void printHexEncodedAsciiString() const
  {
    auto hexEncodedBytes = getHexEncodedAsciiBytes(*_bytes.get());

    printVector<char>(hexEncodedBytes);
  }

private:
  std::unique_ptr<std::vector<char>> _bytes;

  static char convertAsciiByteToHexByte(char ascii)
  {
    if (ascii >= '0' && ascii <= '9' )
    {
      return ascii - '0';
    }

    if (ascii >= 'a' && ascii <= 'f' )
    {
      return 10 + ascii - 'a';
    }

    return 0;
  }

  static char convertBase64ByteToAsciiByte(char base64)
  {
    if (base64 >= 0 && base64 < 26)
    {
      return base64 + 'A';
    }

    if (base64 >= 26 && base64 < 52)
    {
      return base64 - 26 + 'a';
    }

    if (base64 >= 52 && base64 < 62)
    {
      return base64 - 52 + '0';
    }

    if (base64 == 63) return '+';
    if (base64 == 64) return '/';

    return 0;
  }


  static char convertHexByteToAsciiByte(char hex)
  {
    if (hex >= 0 && hex < 10)
    {
      return hex + '0';
    }

    if (hex >= 10 && hex < 16)
    {
      return hex - 10 + 'a';
    }

    return 0;
  }

  static std::vector<char> getBase64EncodedAsciiBytes(
    const std::vector<char>& bytes)
  {
    std::vector<char> paddedBytes(bytes);

    size_t unpaddedSize = bytes.size();
    if (unpaddedSize % 3 != 0)
    {
      for (auto i = 0; i < 3 - (unpaddedSize % 3); ++i)
      {
        paddedBytes.insert(paddedBytes.begin(), 0);
      }
    }

    std::vector<char> base64EncodedBytes(
      std::div(paddedBytes.size(), 3).quot * 4);
    for (auto i = 0, j = 0; i < base64EncodedBytes.size(); i += 4, j += 3)
    {
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

    return base64EncodedBytes;
  }

  static std::vector<char> getDecodedHexEncodedAsciiBytes(
    const std::vector<char>& hexEncodedBytes)
  {
    auto paddedHexEncodedBytes = hexEncodedBytes;
    if (paddedHexEncodedBytes.size() % 2 != 0)
    {
      paddedHexEncodedBytes.insert(paddedHexEncodedBytes.begin(), 0);
    }

    std::vector<char> decodedBytes(
      std::div(paddedHexEncodedBytes.size(), 2).quot);
    for (auto i = 0, j = 0; i < decodedBytes.size(); ++i, j += 2)
    {
      decodedBytes[i] =
        convertAsciiByteToHexByte(paddedHexEncodedBytes[j]) << 4;
      decodedBytes[i] |=
        convertAsciiByteToHexByte(paddedHexEncodedBytes[j + 1]);
    }

    return decodedBytes;
  }

  static std::vector<char> getHexEncodedAsciiBytes(
    const std::vector<char>& bytes)
  {
    std::vector<char> hexEncodedBytes(bytes.size() * 2);
    for (auto i = 0, j = 0; i < hexEncodedBytes.size(); i += 2, ++j)
    {
      hexEncodedBytes[i] = convertHexByteToAsciiByte(
        0b00001111 & bytes[j] >> 4);
      hexEncodedBytes[i + 1] = convertHexByteToAsciiByte(0b00001111 & bytes[j]);
    }

    return hexEncodedBytes;
  }

  template <typename T> void printVector(const std::vector<T>& v) const
  {
    for (auto it = v.begin(); it != v.end(); ++it)
    {
      std::cout << *it;
    }

     std::cout << std::endl;
  }

  friend class ByteSequenceTests;
};

#endif // __BYTE_SEQUENCE_HPP__
