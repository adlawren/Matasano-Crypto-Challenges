#include <fstream>
#include <string>
#include <vector>

#include "ByteSequence.hpp"
#include "CharacterFrequencyScoreCalculator.hpp"
#include "Encryptor.hpp"

int main(int argc, char *argv[]) {
  std::string text = "Burning 'em, if you ain't quick and nimble\nI go crazy "
                     "when I hear a cymbal";
  ByteSequence textByteSequence;
  textByteSequence.initializeFromAsciiBytes(
      std::vector<char>(&text[0], &text[text.size()]));

  std::string key = "ICE";
  ByteSequence keyByteSequence;
  keyByteSequence.initializeFromAsciiBytes(
      std::vector<char>(&key[0], &key[key.size()]));

  auto encryptedByteSequence =
      Encryptor::getRepeatingKeyXorEncryptedByteSequence(textByteSequence,
                                                         keyByteSequence);

  encryptedByteSequence.printHexEncodedAsciiString();

  return 0;
}
