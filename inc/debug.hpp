/*
 * Some useful tools for debugging
 */

#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#include <iostream>
#include <map>
#include <vector>

namespace debug {

// Source:
// http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format/3208376#3208376
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
  (byte & 0x80 ? '1' : '0'), (byte & 0x40 ? '1' : '0'),                        \
      (byte & 0x20 ? '1' : '0'), (byte & 0x10 ? '1' : '0'),                    \
      (byte & 0x08 ? '1' : '0'), (byte & 0x04 ? '1' : '0'),                    \
      (byte & 0x02 ? '1' : '0'), (byte & 0x01 ? '1' : '0')

void printBinaryVector(const std::vector<char> &v) {
  for (auto it = v.begin(); it != v.end(); ++it) {
    printf("| " BYTE_TO_BINARY_PATTERN " |", BYTE_TO_BINARY(*it));
  }

  printf("\n");
}

void printString(const std::string &s) { std::cout << s << std::endl; }

template <typename T> void printVector(const std::vector<T> &v) {
  for (auto it = v.begin(); it != v.end(); ++it) {
    std::cout << *it;
  }

  std::cout << std::endl;
}

template <typename T, typename U>
static void printMap(const std::map<T, U> &map) {
  for (auto it = map.begin(); it != map.end(); ++it) {
    std::cout << "Key: \'" << it->first << "\', Value: " << it->second
              << std::endl;
  }
}
}
#endif // __DEBUG_HPP__
