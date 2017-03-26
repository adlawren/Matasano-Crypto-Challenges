#include <iostream>

#include "Encryptor.hpp"
#include "Decryptor.hpp"

static const unsigned BLOCK_SIZE = 16;

static ByteSequence randomGlobalKeyByteSequence;

int main(int argc, char* argv[]) {
  // Initialize random global key
  randomGlobalKeyByteSequence = ByteSequence::getRandomByteSequence(BLOCK_SIZE);

  {
    std::string ciphtertextString =
        "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/"
        "kXX0KSvoOLSFQ==";

    ByteSequence ciphertextByteSequence;
    ciphertextByteSequence.initializeFromBase64EncodedAsciiBytes(
        std::vector<char>(&ciphtertextString[0],
                          &ciphtertextString[ciphtertextString.size()]));

    std::cout << "Ciphertext string length:"
              << ciphertextByteSequence.getByteCount() << std::endl;

    std::string keyString = "YELLOW SUBMARINE";

    ByteSequence keyByteSequence;
    keyByteSequence.initializeFromAsciiBytes(
        std::vector<char>(&keyString[0], &keyString[keyString.size()]));

    auto plaintextByteSequence =
        Decryptor::decryptAES128BitCTRModeEncryptedByteSequence(
            ciphertextByteSequence, keyByteSequence);

    std::cout << "Decrypted string:" << std::endl;
    plaintextByteSequence.printAsciiString();

    std::cout << "Decrypted string length:"
              << plaintextByteSequence.getByteCount() << std::endl;
  }

  {
    std::string plaintextString =
        "This is a test string, and ya. That's pretty much it.";

    ByteSequence plaintextByteSequence;
    plaintextByteSequence.initializeFromAsciiBytes(std::vector<char>(
        &plaintextString[0], &plaintextString[plaintextString.size()]));

    std::cout << "Plaintext string length:"
              << plaintextByteSequence.getByteCount() << std::endl;

    auto ciphertextByteSequence =
        Encryptor::getAES128BitCTRModeEncryptedByteSequence(
            plaintextByteSequence, randomGlobalKeyByteSequence);

    std::cout << "Ciphertext byte count:"
              << ciphertextByteSequence.getByteCount() << std::endl;

    auto decryptedByteSequence =
        Decryptor::decryptAES128BitCTRModeEncryptedByteSequence(
            ciphertextByteSequence, randomGlobalKeyByteSequence);

    std::cout << "Decrypted string:" << std::endl;
    decryptedByteSequence.printAsciiString();

    std::cout << "Decrypted string length:"
              << decryptedByteSequence.getByteCount() << std::endl;
  }

  return 0;
}