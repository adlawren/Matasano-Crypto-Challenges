#include <fstream>

#include "ByteSequence.hpp"
#include "Decryptor.hpp"
#include "Encryptor.hpp"

ByteSequence editCTREncryptedCiphertextByteSequence(
    const ByteSequence& ciphertextByteSequence,
    const ByteSequence& keyByteSequence, unsigned offset,
    const ByteSequence& newByteSequence) {
  ByteSequence newPlaintextByteSequence =
      Decryptor::decryptAES128BitCTRModeEncryptedByteSequence(
          ciphertextByteSequence, keyByteSequence);

  newPlaintextByteSequence.editSubSequence(offset, newByteSequence);

  return Encryptor::getAES128BitCTRModeEncryptedByteSequence(
      newPlaintextByteSequence, keyByteSequence);
}

ByteSequence getCTRKeyStreamByteSequence(
    const ByteSequence& ciphertextByteSequence,
    const ByteSequence& keyByteSequence, unsigned offset, unsigned byteCount) {
  ByteSequence knownPlaintextByteSequence;
  knownPlaintextByteSequence.initializeFromAsciiBytes(
      std::vector<char>(byteCount, 0));

  ByteSequence newCiphertextByteSequence =
      editCTREncryptedCiphertextByteSequence(ciphertextByteSequence,
                                             keyByteSequence, offset,
                                             knownPlaintextByteSequence);

  ByteSequence ciphertextSubSequence =
      newCiphertextByteSequence.getSubSequence(offset, byteCount);

  return ciphertextSubSequence.getXoredByteSequence(knownPlaintextByteSequence);
}

int main() {
  ByteSequence cbcEncryptedCiphertextByteSequence;

  std::ifstream fileStream("challenge_7_ciphertext.txt");

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    cbcEncryptedCiphertextByteSequence.appendBase64EncodedAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));
  }

  std::string keyString = "YELLOW SUBMARINE";

  ByteSequence cbcKeyByteSequence;
  cbcKeyByteSequence.initializeFromAsciiBytes(
      std::vector<char>(&keyString[0], &keyString[keyString.size()]));

  auto plaintextByteSequence =
      Decryptor::decryptAES128BitECBModeEncryptedByteSequence(
          cbcEncryptedCiphertextByteSequence, cbcKeyByteSequence);

  ByteSequence randomKeyByteSequence = ByteSequence::getRandomByteSequence(16);

  ByteSequence ciphertextByteSequence =
      Encryptor::getAES128BitCTRModeEncryptedByteSequence(
          plaintextByteSequence, randomKeyByteSequence);

  ByteSequence crackedKeyStreamByteSequence =
      getCTRKeyStreamByteSequence(ciphertextByteSequence, randomKeyByteSequence,
                                  0, ciphertextByteSequence.getByteCount());

  ByteSequence crackedPlaintextByteSequence =
      ciphertextByteSequence.getXoredByteSequence(crackedKeyStreamByteSequence);
  crackedPlaintextByteSequence.printAsciiString();

  return 0;
}
