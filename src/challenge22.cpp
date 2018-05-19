#include <ctime>
#include <iostream>
#include <random>

#include "MT19937RandomNumberGenerator.hpp"

static const unsigned TRIAL_LIMIT = 1000000;

unsigned getRandomTimeOffset(unsigned timeOffsetLowerLimit,
                             unsigned timeOffsetUpperLimit) {
  return static_cast<unsigned>(((rand() / float(RAND_MAX)) *
                                (timeOffsetUpperLimit - timeOffsetLowerLimit)) +
                               timeOffsetLowerLimit);
}

unsigned getFirstMT19937RandomNumberWithTimeOffsetSeed(
    unsigned timeOffsetLowerLimit, unsigned timeOffsetUpperLimit) {
  // Get current UNIX timestamp
  unsigned timestamp = time(nullptr);

  // Simulate two 40 - 1000 second offsets
  timestamp += getRandomTimeOffset(timeOffsetLowerLimit, timeOffsetUpperLimit);
  timestamp += getRandomTimeOffset(timeOffsetLowerLimit, timeOffsetUpperLimit);

  MT19937RandomNumberGenerator rng(timestamp);
  return rng.getNextRandomNumber();
}

int main() {
  static const unsigned TIME_OFFSET_LOWER_LIMIT = 40;
  static const unsigned TIME_OFFSET_UPPER_LIMIT = 1000;

  // Determine the seed based on the RNG output
  unsigned firstRandomNumber = getFirstMT19937RandomNumberWithTimeOffsetSeed(
      TIME_OFFSET_LOWER_LIMIT, TIME_OFFSET_UPPER_LIMIT);

  // Get current UNIX timestamp
  unsigned currentTimestamp = time(nullptr);

  // Add maximum possible offsets to current timestamp, to accommodate the
  // timestamp simulated earlier
  currentTimestamp += 2 * TIME_OFFSET_UPPER_LIMIT;

  // Guess seed by decrementing current timestamp
  unsigned trialCount = 0;
  for (unsigned nextSeedGuess = currentTimestamp; trialCount < TRIAL_LIMIT;
       --nextSeedGuess, ++trialCount) {
    MT19937RandomNumberGenerator rng(nextSeedGuess);
    if (firstRandomNumber == rng.getNextRandomNumber()) {
      std::cout << "Cracked seed: " << nextSeedGuess << std::endl;
      break;
    }
  }

  if (trialCount == TRIAL_LIMIT) {
    std::cout << "Failed to determine RNG seed" << std::endl;
  }

  return 0;
}
