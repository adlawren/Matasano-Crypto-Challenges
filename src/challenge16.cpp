#include <iostream>

#include "Encryptor.hpp"
#include "Decryptor.hpp"

static const unsigned BLOCK_SIZE = 16;

ByteSequence randomGlobalKeyByteSequence, randomGlobalInitializationVector;

bool isAdminCiphertextByteSequence(
    const ByteSequence& ciphertextByteSequence,
    std::function<ByteSequence(const ByteSequence&)> decryptionFunction) {
  auto decryptedByteSequence = decryptionFunction(ciphertextByteSequence);

  // Look for the 'admin=true' tuple
  std::string adminKeyString = "admin";
  ByteSequence adminKeyByteSequence;
  adminKeyByteSequence.initializeFromAsciiBytes(std::vector<char>(
      &adminKeyString[0], &adminKeyString[adminKeyString.size()]));

  std::string adminValueString = "true";
  ByteSequence adminValueByteSequence;
  adminValueByteSequence.initializeFromAsciiBytes(std::vector<char>(
      &adminValueString[0], &adminValueString[adminValueString.size()]));

  auto tupleList = ByteSequence::getParsedTupleListFromEncodedByteSequence(
      decryptedByteSequence, ';', '=');

  for (auto tuple : tupleList) {
    if (tuple.first.getByteCount() != adminKeyByteSequence.getByteCount()) {
      continue;
    }

    if (tuple.first != adminKeyByteSequence) {
      continue;
    }

    if (tuple.second.getByteCount() != adminValueByteSequence.getByteCount()) {
      continue;
    }

    if (tuple.second != adminValueByteSequence) {
      continue;
    }

    return true;
  }

  return false;
}

ByteSequence getBitFlippedMetaCharacterBitMaskByteSequence(
    const ByteSequence& byteSequence, const std::vector<char>& metaCharacters) {
  ByteSequence bitmaskByteSequence;

  for (auto nextByte : byteSequence.getBytes()) {
    if (std::find(metaCharacters.begin(), metaCharacters.end(), nextByte) !=
        metaCharacters.end()) {
      bitmaskByteSequence.appendAsciiBytes(std::vector<char>{1});
    } else {
      bitmaskByteSequence.appendAsciiBytes(std::vector<char>{0});
    }
  }

  return bitmaskByteSequence;
}

ByteSequence getConstraintVerifiedPlaintextByteSequence(
    const ByteSequence& userByteSequence,
    const ByteSequence& bitmaskByteSequence,
    std::function<ByteSequence(const ByteSequence&)> encryptionFunction,
    std::function<bool(const ByteSequence&)> constraintVerificationFunction) {
  // NOTE: This is assumed at present
  const unsigned blockSize = 16;

  assert(bitmaskByteSequence.getByteCount() == blockSize);

  for (unsigned i = 0; i < blockSize; ++i) {
    // Encrypt the user byte sequence, offset by i
    ByteSequence plaintextByteSequence;
    plaintextByteSequence.appendAsciiBytes(std::vector<char>(i, 1));
    plaintextByteSequence.appendAsciiBytes(userByteSequence.getBytes());

    auto ciphertextByteSequence = encryptionFunction(plaintextByteSequence);

    // For each block of the cipher text
    unsigned ciphertextBlockCount =
        ciphertextByteSequence.getByteCount() / blockSize;
    for (unsigned j = 0; j < ciphertextBlockCount; ++j) {
      // Flip the bits according to the bit mask byte sequence (i.e. XOR the
      // block with the bit mask byte sequence)
      ByteSequence fullLengthBitmaskByteSequence;
      fullLengthBitmaskByteSequence.appendAsciiBytes(
          std::vector<char>(j * blockSize, 0));
      fullLengthBitmaskByteSequence.appendAsciiBytes(
          bitmaskByteSequence.getBytes());
      fullLengthBitmaskByteSequence.appendAsciiBytes(
          std::vector<char>((ciphertextBlockCount - j - 1) * blockSize, 0));

      auto bitFlippedCiphertextByteSequence =
          ciphertextByteSequence.getXoredByteSequence(
              fullLengthBitmaskByteSequence);

      // Check if the ciphertext is an admin ciphertext
      if (constraintVerificationFunction(bitFlippedCiphertextByteSequence)) {
        return plaintextByteSequence;
      }
    }
  }

  throw std::runtime_error(
      "Failed to find a constraint verified plaintext byte sequence");
}

int main(int argc, char* argv[]) {
  randomGlobalKeyByteSequence = ByteSequence::getRandomByteSequence(BLOCK_SIZE);
  randomGlobalInitializationVector =
      ByteSequence::getRandomByteSequence(BLOCK_SIZE);

  auto encryptionFunction = [&](const ByteSequence& userByteSequence) {
    // Remove meta characters (';' and '=')
    auto userBytes = userByteSequence.getBytes();
    userBytes.erase(std::remove(userBytes.begin(), userBytes.end(), ';'),
                    std::end(userBytes));
    userBytes.erase(std::remove(userBytes.begin(), userBytes.end(), '='),
                    std::end(userBytes));

    ByteSequence joinedByteSequence;

    std::string prependedString = "comment1=cooking%20MCs;userdata=";
    joinedByteSequence.appendAsciiBytes(std::vector<char>(
        &prependedString[0], &prependedString[prependedString.size()]));

    joinedByteSequence.appendAsciiBytes(userBytes);

    std::string appendedString = ";comment2=%20like%20a%20pound%20of%20bacon";
    joinedByteSequence.appendAsciiBytes(std::vector<char>(
        &appendedString[0], &appendedString[appendedString.size()]));

    if (joinedByteSequence.getByteCount() % BLOCK_SIZE != 0) {
      auto paddedByteSequence =
          joinedByteSequence.getPaddedByteSequence(BLOCK_SIZE);

      return Encryptor::getAES128BitCBCModeEncryptedByteSequence(
          paddedByteSequence, randomGlobalKeyByteSequence,
          randomGlobalInitializationVector);
    } else {
      return Encryptor::getAES128BitCBCModeEncryptedByteSequence(
          joinedByteSequence, randomGlobalKeyByteSequence,
          randomGlobalInitializationVector);
    }
  };

  auto decryptionFunction = [&](const ByteSequence& ciphertextByteSequence) {
    return Decryptor::decryptAES128BitCBCModeEncryptedByteSequence(
        ciphertextByteSequence, randomGlobalKeyByteSequence,
        randomGlobalInitializationVector);
  };

  // Initialize user byte sequence
  std::string userString = "asdfg;admin=true";
  ByteSequence userByteSequence;
  userByteSequence.initializeFromAsciiBytes(
      std::vector<char>(&userString[0], &userString[userString.size()]));

  // Initialize bit mask byte sequence
  auto bitmaskByteSequence = getBitFlippedMetaCharacterBitMaskByteSequence(
      userByteSequence, std::vector<char>{';', '='});

  // Flip the bits of the meta characters
  auto bitFlippedUserByteSequence =
      userByteSequence.getXoredByteSequence(bitmaskByteSequence);

  auto constraintVerificationFunction = std::bind(
      isAdminCiphertextByteSequence, std::placeholders::_1, decryptionFunction);

  auto constraintVerifiedPlaintextByteSequence =
      getConstraintVerifiedPlaintextByteSequence(
          bitFlippedUserByteSequence, bitmaskByteSequence, encryptionFunction,
          constraintVerificationFunction);

  std::cout << "Ascii encoded constraint verified plaintext byte sequence:"
            << std::endl;
  constraintVerifiedPlaintextByteSequence.printAsciiString();
  std::cout << "Hex encoded constraint verified plaintext byte sequence:"
            << std::endl;
  constraintVerifiedPlaintextByteSequence.printHexEncodedAsciiString();

  return 0;
}