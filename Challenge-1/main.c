#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <cassert>
#include <cstdlib>
#include <cstring>

// Source: http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format/3208376#3208376
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

// Parameters
const size_t BUFFER_LENGTH = 256;

void printVectorBinary(const std::vector<char>& v)
{
  for (auto it = v.begin(); it != v.end(); ++it)
  {
    printf("| " BYTE_TO_BINARY_PATTERN " |", BYTE_TO_BINARY(*it));
  }

   printf("\n");
}

void printVector(const std::vector<char>& v)
{
  for (auto it = v.begin(); it != v.end(); ++it)
  {
    std::cout << *it;
  }

   std::cout << std::endl;
}

char asciiToHex(char ascii)
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

void asciiToHex(std::vector<char>& asciiBytes)
{
  for (auto it = asciiBytes.begin(); it != asciiBytes.end(); ++it)
  {
    *it = asciiToHex(*it);
  }
}

char asciiToBase64(char ascii)
{
  if (ascii >= 'A' && ascii <= 'Z' )
  {
    return ascii - 'A';
  }

  if (ascii >= 'a' && ascii <= 'z' )
  {
    return 26 +  ascii - 'a';
  }

  if (ascii >= '0' && ascii <= '9' )
  {
    return 52 +  ascii - '0';
  }

  if (ascii == '+') return 53;
  if (ascii == '/') return 54;

  return 0;
}

void asciiToBase64(std::vector<char>& asciiBytes)
{
  for (auto it = asciiBytes.begin(); it != asciiBytes.end(); ++it)
  {
    *it = asciiToBase64(*it);
  }
}

char base64ToAscii(char base64)
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

void base64ToAscii(std::vector<char>& asciiBytes)
{
  for (auto it = asciiBytes.begin(); it != asciiBytes.end(); ++it)
  {
    *it = base64ToAscii(*it);
  }
}

void hexToBase64(const std::vector<char>& hexBytes,
  std::vector<char>& base64Bytes)
{
  for (auto i = 0, j = 0; i < hexBytes.size(); i += 3, j += 2)
  {
    base64Bytes[j] = hexBytes[i + 1] >> 2;
    base64Bytes[j] |= hexBytes[i] << 2;
    base64Bytes[j + 1] = (hexBytes[i + 1] & 0x03) << 4;
    base64Bytes[j + 1] |= hexBytes[i + 2];
  }
}

int main(int argc, char* argv[])
{
  char buffer[BUFFER_LENGTH];
  int bytesRead;

  scanf("%s%n", buffer, &bytesRead);

  std::vector<char> bytes(&buffer[0], &buffer[bytesRead]);
  asciiToHex(bytes);

  if (bytes.size() % 3 != 0)
  {
    const int unpaddedSize = bytes.size();
    for (auto i = 0; i < (3 - (unpaddedSize % 3)); ++i) bytes.push_back(0);
  }

  std::vector<char> base64Bytes(std::div(bytes.size(), 3).quot * 2);

  hexToBase64(bytes, base64Bytes);
  base64ToAscii(base64Bytes);

  printVector(base64Bytes);

  return 0;
}
