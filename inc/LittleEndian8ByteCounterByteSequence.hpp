#ifndef __LITTLE_ENDIAN_EIGHT_BYTE_COUNTER_BYTE_SEQUENCE_HPP__
#define __LITTLE_ENDIAN_EIGHT_BYTE_COUNTER_BYTE_SEQUENCE_HPP__

#include <vector>

#include "ByteSequence.hpp"

class LittleEndian8ByteCounterByteSequence {
 public:
  LittleEndian8ByteCounterByteSequence() {
    counterByteSequence.initializeFromAsciiBytes(
        std::vector<char>(byteSequenceSize, 0));
  }

  LittleEndian8ByteCounterByteSequence(const ByteSequence& byteSequence) {
    assert(byteSequence.getByteCount() == byteSequenceSize);

    counterByteSequence.initializeFromAsciiBytes(byteSequence.getBytes());
  }

  ByteSequence getByteSequence() const {
    return counterByteSequence;
  }

  LittleEndian8ByteCounterByteSequence
  getIncrementedLittleEndian8ByteCounterByteSequence() const {
    unsigned long numericalEquivalent = 0;
    for (unsigned byteIndex = 0; byteIndex < counterByteSequence.getByteCount();
         ++byteIndex) {
      long temp = (unsigned long)counterByteSequence.getBytes()[byteIndex];
      temp = temp << (8 * byteIndex);

      long bitmask = 0xff;
      bitmask = bitmask << (8 * byteIndex);

      numericalEquivalent ^= temp & bitmask;
    }

    numericalEquivalent += 1;

    std::vector<char> newBytes(byteSequenceSize, 0);
    for (unsigned byteIndex = 0; byteIndex < counterByteSequence.getByteCount();
         ++byteIndex) {
      long temp = 0xff;
      temp = temp << (8 * (counterByteSequence.getByteCount() - byteIndex));
      temp &= numericalEquivalent;
      newBytes[byteIndex] =
          temp >> (8 * (counterByteSequence.getByteCount() - byteIndex));
    }

    ByteSequence newByteSequence;
    newByteSequence.initializeFromAsciiBytes(newBytes);

    LittleEndian8ByteCounterByteSequence
        newLittleEndian8ByteCounterByteSequence(newByteSequence);

    return newLittleEndian8ByteCounterByteSequence;
  }

  LittleEndian8ByteCounterByteSequence& operator=(
      const LittleEndian8ByteCounterByteSequence&
          littleEndian8ByteCounterByteSequence) {
    counterByteSequence.initializeFromAsciiBytes(
        littleEndian8ByteCounterByteSequence.getByteSequence().getBytes());

    return *this;
  }

 private:
  const unsigned byteSequenceSize = 8;

  ByteSequence counterByteSequence;
};

#endif  // __LITTLE_ENDIAN_EIGHT_BYTE_COUNTER_BYTE_SEQUENCE_HPP__