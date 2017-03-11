#include <cassert>
#include <iostream>
#include <map>
#include <vector>

#include "ByteSequence.hpp"
#include "Cracker.hpp"
#include "Encryptor.hpp"
#include "Decryptor.hpp"

typedef std::map<std::string, std::string> ParsedObject;

static const unsigned BLOCK_SIZE = 16;
static const unsigned RANDOM_GLOBAL_KEY_SIZE = 16;

static ByteSequence randomGlobalKeyByteSequence;

std::vector<std::string> splitString(const std::string &str,
                                     const std::string &deilimeter) {
  if (str.size() == 0) {
    return std::vector<std::string>{str};
  }

  if (str.find(deilimeter) == std::string::npos) {
    return std::vector<std::string>{str};
  } else {
    std::vector<std::string> delimitedStrings;

    delimitedStrings.push_back(str.substr(0, str.find(deilimeter)));

    auto otherDelimitedStrings = splitString(
        str.substr(str.find(deilimeter) + 1, str.size()), deilimeter);

    delimitedStrings.insert(std::end(delimitedStrings),
                            std::begin(otherDelimitedStrings),
                            std::end(otherDelimitedStrings));

    return delimitedStrings;
  }
}

ParsedObject
getParsedObjectFromEncodedString(const std::string &encodedString) {
  ParsedObject parsedObject;

  auto ampersandDelimitedStrings = splitString(encodedString, "&");
  for (auto ampersandDelimitedString : ampersandDelimitedStrings) {
    auto equalsDelimitedStrings = splitString(ampersandDelimitedString, "=");

    assert(equalsDelimitedStrings.size() > 0);

    if (equalsDelimitedStrings.size() == 1) {
      parsedObject[equalsDelimitedStrings[0]] = "";
    } else {
      parsedObject[equalsDelimitedStrings[0]] = equalsDelimitedStrings[1];
    }
  }

  return parsedObject;
}

ByteSequence profileFor(const ByteSequence &userEmailByteSequence) {
  // Remove meta characters ('&' and '=')
  auto userEmailBytes = userEmailByteSequence.getBytes();
  userEmailBytes.erase(
      std::remove(userEmailBytes.begin(), userEmailBytes.end(), '&'),
      std::end(userEmailBytes));
  userEmailBytes.erase(
      std::remove(userEmailBytes.begin(), userEmailBytes.end(), '='),
      std::end(userEmailBytes));

  // Build profile string
  ByteSequence profileByteSequence;

  std::string emailKey = "email=";
  profileByteSequence.appendAsciiBytes(
      std::vector<char>(&emailKey[0], &emailKey[emailKey.size()]));

  profileByteSequence.appendAsciiBytes(userEmailByteSequence.getBytes());

  std::string profileData = "&uid=10&role=user";
  profileByteSequence.appendAsciiBytes(
      std::vector<char>(&profileData[0], &profileData[profileData.size()]));

  return profileByteSequence;
}

ByteSequence getAES128BitECBModeEncryptedByteSequenceFromUserProfile(
    const ByteSequence &encodedUserProfileByteSequence,
    const ByteSequence &keyByteSequence) {
  if (encodedUserProfileByteSequence.getByteCount() % BLOCK_SIZE != 0) {
    auto paddedByteSequence =
        encodedUserProfileByteSequence.getPaddedByteSequence(BLOCK_SIZE);

    return Encryptor::getAES128BitECBModeEncryptedByteSequence(
        paddedByteSequence, keyByteSequence);
  } else {
    return Encryptor::getAES128BitECBModeEncryptedByteSequence(
        encodedUserProfileByteSequence, keyByteSequence);
  }
}

ParsedObject getDecryptedAndParsedAES128BitECBModeEncryptedUserProfile(
    const ByteSequence &encodedUserProfileByteSequence,
    const ByteSequence &keyByteSequence) {

  auto decryptedUserProfileByteSequence =
      Decryptor::decryptAES128BitECBModeEncryptedByteSequence(
          encodedUserProfileByteSequence, keyByteSequence);

  auto encodedUserProfileBytes = decryptedUserProfileByteSequence.getBytes();
  std::string encodedUserProfile =
      std::string(&encodedUserProfileBytes[0],
                  &encodedUserProfileBytes[encodedUserProfileBytes.size()]);

  return getParsedObjectFromEncodedString(encodedUserProfile);
}

int main(int argc, char *agv[]) {
  // Initialize random global key
  randomGlobalKeyByteSequence =
      ByteSequence::getRandomByteSequence(RANDOM_GLOBAL_KEY_SIZE);

  auto encryptionFunction = [&](const ByteSequence &plaintextByteSequence) {
    auto userProfileByteSequence = profileFor(plaintextByteSequence);

    return getAES128BitECBModeEncryptedByteSequenceFromUserProfile(
        userProfileByteSequence, randomGlobalKeyByteSequence);
  };

  std::string userEmail = "test@something.com";
  ByteSequence userEmailByteSequence;
  userEmailByteSequence.initializeFromAsciiBytes(
      std::vector<char>(&userEmail[0], &userEmail[userEmail.size()]));

  auto userProfileByteSequence = profileFor(userEmailByteSequence);

  auto encryptedUserProfileByteSequence =
      encryptionFunction(userEmailByteSequence);

  {
    auto decryptedParsedObject =
        getDecryptedAndParsedAES128BitECBModeEncryptedUserProfile(
            encryptedUserProfileByteSequence, randomGlobalKeyByteSequence);

    std::cout << "Before:" << std::endl;
    for (auto keyValuePair : decryptedParsedObject) {
      std::cout << keyValuePair.first << " : " << keyValuePair.second
                << std::endl;
    }
  }

  auto decryptedMysteryPlaintext =
      Cracker::getAES128BitECBModeEncryptedMysteryPlaintextByteSequence(
          encryptionFunction);

  std::cout << "Decrypted mystery plaintext:" << std::endl;
  decryptedMysteryPlaintext.printAsciiString();

  unsigned crackedProfileDataLength = decryptedMysteryPlaintext.getByteCount();

  unsigned computedBlockSize = 0;
  {
    const unsigned userByteSequenceLength = 1024;

    ByteSequence userByteSequence;
    userByteSequence.initializeFromAsciiBytes(
        std::vector<char>(userByteSequenceLength, 0));

    auto ciphertextByteSequence = encryptionFunction(userByteSequence);
    computedBlockSize =
        Oracle::getCiphertextBlockSize(ciphertextByteSequence, 20);
  }

  auto frontMysteryPlaintextByteCount =
      Oracle::getAES128BitECBModeEncryptedFrontMysteryPlaintextLength(
          encryptionFunction, computedBlockSize);

  // Determine what the ciphertext block for 'admin{trailing null bytes}' looks
  // like
  ByteSequence adminCiphertextBlockByteSequence;
  {
    ByteSequence userByteSequence;
    userByteSequence.appendAsciiBytes(std::vector<char>(
        computedBlockSize -
            (frontMysteryPlaintextByteCount % computedBlockSize),
        0));

    std::string adminString = "admin";
    userByteSequence.appendAsciiBytes(
        std::vector<char>(&adminString[0], &adminString[adminString.size()]));

    userByteSequence.appendAsciiBytes(std::vector<char>(
        computedBlockSize - (adminString.size() % computedBlockSize), 0));

    auto ciphertextByteSequence = encryptionFunction(userByteSequence);

    adminCiphertextBlockByteSequence = ciphertextByteSequence.getSubSequence(
        ((frontMysteryPlaintextByteCount / computedBlockSize) + 1) *
            computedBlockSize,
        computedBlockSize);
  }

  // Create a ciphertext where the last block consists exclusively of 'user{PKCS
  // #7 padding}'
  ByteSequence paddedCiphertextByteSequence;
  {
    ByteSequence userEmailPlaintextByteSequence;
    userEmailPlaintextByteSequence.appendAsciiBytes(
        std::vector<char>(&userEmail[0], &userEmail[userEmail.size()]));
    userEmailPlaintextByteSequence.appendAsciiBytes(std::vector<char>(
        computedBlockSize -
            ((frontMysteryPlaintextByteCount + (crackedProfileDataLength - 4) +
              userEmailPlaintextByteSequence.getByteCount()) %
             computedBlockSize),
        0));

    paddedCiphertextByteSequence =
        encryptionFunction(userEmailPlaintextByteSequence);
  }

  // Replace the last block of the ciphertext with the admin ciphertext block
  auto trimmedCiphertextByteSequence =
      paddedCiphertextByteSequence.getSubSequence(
          0, paddedCiphertextByteSequence.getByteCount() - computedBlockSize);
  trimmedCiphertextByteSequence.appendAsciiBytes(
      adminCiphertextBlockByteSequence.getBytes());
  encryptedUserProfileByteSequence = trimmedCiphertextByteSequence;

  {
    auto decryptedParsedObject =
        getDecryptedAndParsedAES128BitECBModeEncryptedUserProfile(
            encryptedUserProfileByteSequence, randomGlobalKeyByteSequence);

    std::cout << "After:" << std::endl;
    for (auto keyValuePair : decryptedParsedObject) {
      std::cout << keyValuePair.first << " : " << keyValuePair.second
                << std::endl;
    }
  }

  return 0;
}
