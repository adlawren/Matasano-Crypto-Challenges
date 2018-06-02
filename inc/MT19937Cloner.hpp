#ifndef __MT19937_CLONER_HPP__
#define __MT19937_CLONER_HPP__

#include <cmath>
#include <vector>

#include "MT19937RandomNumberGenerator.hpp"

class MT19937Cloner {
 public:
  static MT19937RandomNumberGenerator cloneMT19937RandomNumberGenerator(
      MT19937RandomNumberGenerator& rng) {
    const unsigned periodLength = 624;

    std::vector<unsigned> stateArray;
    for (unsigned i = 0; i < periodLength; ++i) {
      unsigned nextRandomNumber = rng.getNextRandomNumber();

      // Reverse tempering operations
      unsigned tmp = rightShiftUntemper(nextRandomNumber, rng.L);
      tmp = leftShiftUntemper(tmp, rng.T, rng.C);
      tmp = leftShiftUntemper(tmp, rng.S, rng.B);
      unsigned untemperedRandomNumber = rightShiftUntemper(tmp, rng.U, rng.D);

      stateArray.push_back(untemperedRandomNumber);
    }

    MT19937RandomNumberGenerator newRng(stateArray);
    return std::move(newRng);
  }

 private:
  static unsigned shiftBitsRight(unsigned val, unsigned bitCount) {
    if (bitCount < 32) {
      return val >> bitCount;
    } else {
      // Shift by 32 bits - assume no sign extension
      return 0;
    }
  }

  static unsigned shiftBitsLeft(unsigned val, unsigned bitCount) {
    if (bitCount < 32) {
      return val << bitCount;
    } else {
      // Shift by 32 bits
      return 0;
    }
  }

  static unsigned rightShiftUntemper(unsigned val, unsigned shiftAmount,
                                     unsigned bitmask = 0xFFFFFFFF) {
    const unsigned MAX_UNSIGNED = 0xFFFFFFFF;

    unsigned iterationCount = static_cast<unsigned>(
        ceil((sizeof(val) * 8) / static_cast<float>(shiftAmount)));

    unsigned result = val;
    for (unsigned i = 0; i < iterationCount; ++i) {
      result ^= ((result >> shiftAmount) & bitmask) &
                shiftBitsRight(MAX_UNSIGNED, shiftAmount * i);
      result &= ~shiftBitsRight(MAX_UNSIGNED, shiftAmount * (i + 1));
      result |= shiftBitsRight(MAX_UNSIGNED, shiftAmount * (i + 1)) & val;
    }

    return result;
  }

  static unsigned leftShiftUntemper(unsigned val, unsigned shiftAmount,
                                    unsigned bitmask) {
    const unsigned MAX_UNSIGNED = 0xFFFFFFFF;

    unsigned iterationCount = static_cast<unsigned>(
        ceil((sizeof(val) * 8) / static_cast<float>(shiftAmount)));

    unsigned result = val;
    for (unsigned i = 0; i < iterationCount; ++i) {
      result ^= ((result << shiftAmount) & bitmask) &
                shiftBitsLeft(MAX_UNSIGNED, shiftAmount * i);
      result &= ~shiftBitsLeft(MAX_UNSIGNED, shiftAmount * (i + 1));
      result |= shiftBitsLeft(MAX_UNSIGNED, shiftAmount * (i + 1)) & val;
    }

    return result;
  }

  friend class MT19937ClonerTestBootstrap;
};

#endif  // __MT19937_CLONER_HPP__
