#ifndef __ENGLISH_MONOGRAM_RELATIVE_FREQUENCY_PROVIDER_HPP__
#define __ENGLISH_MONOGRAM_RELATIVE_FREQUENCY_PROVIDER_HPP__

#include <map>

#include "ByteSequence.hpp"

// todo: make an abstract "EnglishRelativeFrequencyProvider" class which the
// monogram, bigram and trigram classes all inherit from?
class EnglishMonogramRelativeFrequencyProvider {
 public:
  static EnglishMonogramRelativeFrequencyProvider& getInstance() {
    static EnglishMonogramRelativeFrequencyProvider instance;
    return instance;
  }

  // todo: replace all assertions in this project with exceptions
  float getMonogramRelativeFrequency(
      const ByteSequence& monogramByteSequence) const {
    assert(monogramByteSequence.getByteCount() == 1);

    return _monogramRelativeFrequencyMap.at(monogramByteSequence.getBytes()[0]);
  }

  bool hasMonogramRelativeFrequency(
      const ByteSequence& monogramByteSequence) const {
    assert(monogramByteSequence.getByteCount() == 1);

    return _monogramRelativeFrequencyMap.find(
               monogramByteSequence.getBytes()[0]) !=
           _monogramRelativeFrequencyMap.end();
  }

 private:
  static const std::map<char, float> _monogramRelativeFrequencyMap;

  EnglishMonogramRelativeFrequencyProvider() {
  }

  static const std::map<char, float> createMonogramRelativeFrequencyMap() {
    std::map<char, float> monogramRelativeFrequencyMap;

    monogramRelativeFrequencyMap['a'] = 0.08167;
    monogramRelativeFrequencyMap['b'] = 0.01492;
    monogramRelativeFrequencyMap['c'] = 0.02782;
    monogramRelativeFrequencyMap['d'] = 0.04253;
    monogramRelativeFrequencyMap['e'] = 0.12702;

    monogramRelativeFrequencyMap['f'] = 0.02228;
    monogramRelativeFrequencyMap['g'] = 0.02015;
    monogramRelativeFrequencyMap['h'] = 0.06094;
    monogramRelativeFrequencyMap['i'] = 0.06966;
    monogramRelativeFrequencyMap['j'] = 0.00153;

    monogramRelativeFrequencyMap['k'] = 0.00772;
    monogramRelativeFrequencyMap['l'] = 0.04025;
    monogramRelativeFrequencyMap['m'] = 0.02406;
    monogramRelativeFrequencyMap['n'] = 0.06749;
    monogramRelativeFrequencyMap['o'] = 0.07507;

    monogramRelativeFrequencyMap['p'] = 0.01929;
    monogramRelativeFrequencyMap['q'] = 0.00095;
    monogramRelativeFrequencyMap['r'] = 0.05987;
    monogramRelativeFrequencyMap['s'] = 0.06327;
    monogramRelativeFrequencyMap['t'] = 0.09056;

    monogramRelativeFrequencyMap['u'] = 0.02758;
    monogramRelativeFrequencyMap['v'] = 0.00978;
    monogramRelativeFrequencyMap['w'] = 0.02360;
    monogramRelativeFrequencyMap['x'] = 0.00150;
    monogramRelativeFrequencyMap['y'] = 0.01974;

    monogramRelativeFrequencyMap['z'] = 0.00074;

    return monogramRelativeFrequencyMap;
  }
};

const std::map<char, float>
    EnglishMonogramRelativeFrequencyProvider::_monogramRelativeFrequencyMap =
        EnglishMonogramRelativeFrequencyProvider::
            createMonogramRelativeFrequencyMap();

#endif  // __ENGLISH_MONOGRAM_RELATIVE_FREQUENCY_PROVIDER_HPP__
