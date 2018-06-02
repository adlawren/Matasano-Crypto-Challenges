#include <cassert>
#include <iostream>
#include <random>

#include "MT19937Cloner.hpp"

const unsigned OUTPUT_VERIFICATION_PERIOD = 1000;

class MT19937ClonerTestBootstrap {
 public:
  MT19937ClonerTestBootstrap() {
  }

  void runTests() {
    // Note: It's assumed that the shift amount in the tempering operation will
    // not exceed 31
    rightShiftUntemperTest(0x00000001, 2);
    rightShiftUntemperTest(0x00000001, 4);
    rightShiftUntemperTest(0x00000001, 8);
    rightShiftUntemperTest(0x00000001, 15);
    rightShiftUntemperTest(0x00000001, 16);
    rightShiftUntemperTest(0x00000001, 17);
    rightShiftUntemperTest(0x00000001, 31);

    rightShiftUntemperTest(0x10101010, 2);
    rightShiftUntemperTest(0x10101010, 4);
    rightShiftUntemperTest(0x10101010, 8);
    rightShiftUntemperTest(0x10101010, 15);
    rightShiftUntemperTest(0x10101010, 16);
    rightShiftUntemperTest(0x10101010, 17);
    rightShiftUntemperTest(0x10101010, 31);

    rightShiftUntemperTest(0x00000001, 2, 0x00000001);
    rightShiftUntemperTest(0x00000001, 4, 0x00000001);
    rightShiftUntemperTest(0x00000001, 8, 0x00000001);
    rightShiftUntemperTest(0x00000001, 15, 0x00000001);
    rightShiftUntemperTest(0x00000001, 16, 0x00000001);
    rightShiftUntemperTest(0x00000001, 17, 0x00000001);
    rightShiftUntemperTest(0x00000001, 31, 0x00000001);

    rightShiftUntemperTest(0x10101010, 2, 0x01010101);
    rightShiftUntemperTest(0x10101010, 4, 0x01010101);
    rightShiftUntemperTest(0x10101010, 8, 0x01010101);
    rightShiftUntemperTest(0x10101010, 15, 0x01010101);
    rightShiftUntemperTest(0x10101010, 16, 0x01010101);
    rightShiftUntemperTest(0x10101010, 17, 0x01010101);
    rightShiftUntemperTest(0x10101010, 31, 0x01010101);

    leftShiftUntemperTest(0x00000001, 2);
    leftShiftUntemperTest(0x00000001, 4);
    leftShiftUntemperTest(0x00000001, 8);
    leftShiftUntemperTest(0x00000001, 15);
    leftShiftUntemperTest(0x00000001, 16);
    leftShiftUntemperTest(0x00000001, 17);
    leftShiftUntemperTest(0x00000001, 31);

    leftShiftUntemperTest(0x10101010, 2);
    leftShiftUntemperTest(0x10101010, 4);
    leftShiftUntemperTest(0x10101010, 8);
    leftShiftUntemperTest(0x10101010, 15);
    leftShiftUntemperTest(0x10101010, 16);
    leftShiftUntemperTest(0x10101010, 17);
    leftShiftUntemperTest(0x10101010, 31);

    leftShiftUntemperTest(0x00000001, 2, 0x00000001);
    leftShiftUntemperTest(0x00000001, 4, 0x00000001);
    leftShiftUntemperTest(0x00000001, 8, 0x00000001);
    leftShiftUntemperTest(0x00000001, 15, 0x00000001);
    leftShiftUntemperTest(0x00000001, 16, 0x00000001);
    leftShiftUntemperTest(0x00000001, 17, 0x00000001);
    leftShiftUntemperTest(0x00000001, 31, 0x00000001);

    leftShiftUntemperTest(0x10101010, 2, 0x01010101);
    leftShiftUntemperTest(0x10101010, 4, 0x01010101);
    leftShiftUntemperTest(0x10101010, 8, 0x01010101);
    leftShiftUntemperTest(0x10101010, 15, 0x01010101);
    leftShiftUntemperTest(0x10101010, 16, 0x01010101);
    leftShiftUntemperTest(0x10101010, 17, 0x01010101);
    leftShiftUntemperTest(0x10101010, 31, 0x01010101);
  }

 private:
  static unsigned rightShiftTemper(unsigned val, unsigned shiftAmount,
                                   unsigned bitmask = 0xFFFFFFFF) {
    return val ^ ((val >> shiftAmount) & bitmask);
  }

  static unsigned leftShiftTemper(unsigned val, unsigned shiftAmount,
                                  unsigned bitmask) {
    return val ^ ((val << shiftAmount) & bitmask);
  }

  void rightShiftUntemperTest(unsigned val, unsigned shiftAmount,
                              unsigned bitmask = 0xFFFFFFFF) {
    unsigned tempered = rightShiftTemper(val, shiftAmount, bitmask);
    unsigned untempered =
        MT19937Cloner::rightShiftUntemper(tempered, shiftAmount, bitmask);

    assert(val == untempered);
  }

  void leftShiftUntemperTest(unsigned val, unsigned shiftAmount,
                             unsigned bitmask = 0xFFFFFFFF) {
    unsigned tempered = leftShiftTemper(val, shiftAmount, bitmask);
    unsigned untempered =
        MT19937Cloner::leftShiftUntemper(tempered, shiftAmount, bitmask);

    assert(val == untempered);
  }
};


int main() {
  // Run Cloner tests
  MT19937ClonerTestBootstrap clonerTestBootstrap;
  clonerTestBootstrap.runTests();

  srand(time(0));
  MT19937RandomNumberGenerator rng(rand());

  MT19937RandomNumberGenerator clonedRng =
      MT19937Cloner::cloneMT19937RandomNumberGenerator(rng);

  // Verify that the output from clone matches the output from the original
  for (unsigned i = 0; i < OUTPUT_VERIFICATION_PERIOD; ++i) {
    assert(rng.getNextRandomNumber() == clonedRng.getNextRandomNumber());
  }

  std::cout << "All test cases passed!!" << std::endl;

  return 0;
}
