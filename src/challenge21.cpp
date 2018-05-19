#include <iostream>
#include <random>  // used for testing

#include "MT19937RandomNumberGenerator.hpp"

static const unsigned MAX_TEST_SEED = 100;
static const unsigned SEED_VERIFICATION_PERIOD = 100;

int main() {
  // test code
  for (unsigned next_seed = 0; next_seed < MAX_TEST_SEED; ++next_seed) {
    std::mt19937 mt(next_seed);
    MT19937RandomNumberGenerator mtrng(next_seed);
    for (unsigned i = 0; i < SEED_VERIFICATION_PERIOD; ++i) {
      unsigned next_expected_random_number = mt(),
               next_random_number = mtrng.getNextRandomNumber();

      if (next_random_number != next_expected_random_number) {
        std::cout << "ERROR: RNG OUTPUT INCORRECT; TEST FAILED" << std::endl;
        std::cout << "Test seed: " << next_seed << ", iteration number: " << i
                  << std::endl;
        std::cout << "Expected random number: " << next_expected_random_number
                  << ", generated random number: " << next_random_number
                  << std::endl;
        return -1;
      }
    }
  }

  std::cout << "All test cases passed!!" << std::endl;

  return 0;
}
