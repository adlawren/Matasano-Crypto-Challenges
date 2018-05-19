class MT19937RandomNumberGenerator {
 public:
  MT19937RandomNumberGenerator(unsigned seed) : k_(0) {
    largestWord_ = getLargestWord();
    upperBitsBitmask_ = getUpperBitmask(largestWord_);
    lowerBitsBitmask_ = getLowerBitmask(largestWord_);

    initializeSeries(static_cast<uint32_t>(seed));
  }

  unsigned getNextRandomNumber() {
    uint32_t tmp = 0;

    // Copy the (W - R) upper bits of x-sub-k
    tmp |= (xSeries_[k_] & upperBitsBitmask_);

    // Copy the R lower bits of x-sub-k-plus-one
    tmp |= (xSeries_[k_ + 1] & lowerBitsBitmask_);

    // Multiply by twist transformation matrix
    if (tmp & 0x01) {
      // x-sub-zero, the lowest order bit, is 1
      tmp = (tmp >> 1) ^ A;
    } else {
      // x-sub-zero, the lowest order bit, is 0
      tmp = tmp >> 1;
    }

    xSeries_.push_back(tmp ^ xSeries_[k_ + M]);

    // Compute the next value returned from the algorithm
    uint32_t tmp2 = xSeries_[k_ + N] ^ ((xSeries_[k_ + N] >> U) & D);
    tmp2 = tmp2 ^ ((tmp2 << S) & B);
    tmp2 = tmp2 ^ ((tmp2 << T) & C);
    unsigned result = tmp2 ^ (tmp2 >> L);

    // Increment state
    ++k_;

    return result;
  }

 private:
  // Parameters
  static const uint32_t W = 32, N = 624, M = 397, R = 31;
  static const uint32_t A = 0x9908B0DF;
  static const uint32_t U = 11, D = 0xFFFFFFFF;
  static const uint32_t S = 7, B = 0x9D2C5680;
  static const uint32_t T = 15, C = 0xEFC60000;
  static const uint32_t L = 18;
  static const uint32_t F = 1812433253;

  // 2 raised to the power of W; a word containing ones
  uint32_t largestWord_;

  // The bitmask with the W - R upper bits set to one, and the remaining bits
  // set to zero
  uint32_t upperBitsBitmask_;

  // The bitmask with the R lower bits set to one, and the remaining bits
  // set to zero
  uint32_t lowerBitsBitmask_;

  // The kth index of the next value in the x series
  unsigned k_;

  std::vector<uint32_t> xSeries_;

  uint32_t getLargestWord() {
    // Compute the largest word
    uint32_t largestWord = 1;
    for (unsigned i = 0; i < W - 1; ++i) {
      largestWord = largestWord << 1;
      largestWord |= 0x01;
    }

    return largestWord;
  }

  uint32_t getUpperBitmask(uint32_t largestWord) {
    uint32_t upperBitsBitmask = (largestWord << R);
    return upperBitsBitmask;
  }

  uint32_t getLowerBitmask(uint32_t largestWord) {
    uint32_t upperBitsBitmask = (largestWord << R);

    uint32_t lowerBitsBitmask = upperBitsBitmask ^ largestWord;
    return lowerBitsBitmask;
  }

  void initializeSeries(uint32_t seed) {
    xSeries_.push_back(seed);
    for (unsigned i = 1; i < N; ++i) {
      uint32_t tmp = xSeries_[i - 1] ^ (xSeries_[i - 1] >> (W - 2));
      xSeries_.push_back(F * tmp + i);
    }
  }
};
