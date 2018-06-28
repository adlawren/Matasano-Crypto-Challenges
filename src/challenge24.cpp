#include <cassert>
#include <iostream>
#include <limits>
#include <random>

#include "ByteSequence.hpp"
#include "Cracker.hpp"
#include "Encryptor.hpp"

const unsigned MAX_RANDOM_BYTE_COUNT = 100;

ByteSequence getPrefixedCiphertextByteSequence(
    const ByteSequence& plaintextByteSequence, int randomPrefixByteCount,
    uint16_t seed) {
  ByteSequence randomByteSequence =
      ByteSequence::getRandomByteSequence(randomPrefixByteCount);

  ByteSequence prefixedPlaintextByteSequence;
  prefixedPlaintextByteSequence.appendAsciiBytes(randomByteSequence.getBytes());
  prefixedPlaintextByteSequence.appendAsciiBytes(
      plaintextByteSequence.getBytes());

  return Encryptor::getMT19937EncryptedByteSequence(
      prefixedPlaintextByteSequence, seed);
}

int main() {
  srand(time(0));

  // Crack MT19937 encrypted byte sequence
  const unsigned cribByteSequenceLength = 16;
  ByteSequence cribByteSequence;
  for (unsigned i = 0; i < cribByteSequenceLength; ++i) {
    cribByteSequence.appendAsciiBytes(std::vector<char>{'A'});
  }

  int randomPrefixByteCount = rand() % MAX_RANDOM_BYTE_COUNT;
  uint16_t seed = static_cast<uint16_t>(rand());
  auto prefixedCiphertextByteSequence = getPrefixedCiphertextByteSequence(
      cribByteSequence, randomPrefixByteCount, seed);

  uint16_t crackedSeed = Cracker::get16BitSeedFromMT19937CiphertextByteSequence(
      prefixedCiphertextByteSequence, cribByteSequence);

  assert(crackedSeed == seed);

  auto crackedByteSequence = Encryptor::getMT19937EncryptedByteSequence(
      prefixedCiphertextByteSequence, crackedSeed);

  std::cout << "Cracked MT19937 encrypted ciphertext:" << std::endl;
  crackedByteSequence.printHexEncodedAsciiString();

  // Determine if MT19937 generated password token was seeded using current time
  unsigned seedFromCurrentTime = time(0);
  ByteSequence passwordTokenByteSequence =
      Encryptor::getMT19937GeneratedPasswordTokenByteSequence(
          seedFromCurrentTime);
  assert(Cracker::isMT19937GeneratedPasswordTokenByteSequence(
      passwordTokenByteSequence));

  return 0;
}
