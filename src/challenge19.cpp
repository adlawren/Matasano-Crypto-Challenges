#include <iostream>
#include <map>

#include "Encryptor.hpp"
#include "Cracker.hpp"
#include "Decryptor.hpp"

static const unsigned BLOCK_SIZE = 16;

static const unsigned MINIMUM_COLUMN_SIZE = 10;  // Note: this is a guess

static ByteSequence randomGlobalKeyByteSequence;

/*
  DISCLAIMER: This doesn't completely work

  The approach is to crack the ("vertical") "columns" of ciphertext, where each
  column i contains the ith character from each ciphertext string. Note that not
  all of the ciphertext strings are the same length, so the columns progresively
  get shorter as i increases.

  The algorithm (understandably) breaks down when cracking the final columns of
  ciphertext characters, since these columns are very short.

  Augmenting the column-cracking process by incorporating scoring algorithms
  which look for common bigrams and trigrams helped somewhat. Ciphertext columns
  of length 10 or greater are cracked using the single-character-xor cracking
  algorithm exclusively. When cracking the columns shorter than 10 characters,
  scores from bigrams and trigrams (update: currently just using bigram scores;
  see Cracker.hpp) are incorporated when found during the evaluation of
  keystream guesses.

  The longest ciphertext column shorter that 10 characters (6 characters in
  lenghth) was sucessfully cracked using this approach (note that it could not
  be successfully cracked using the single-character-xor cracking algorithm,
  applied to columns of length 10 or greater, successfully). However the smaller
  ciphertext columns (of length 3 and smaller) could not be cracked using the
  same code (note that one of the columns of length 3 could be decrypted, but
  that success was considered to be a fluke).
*/

int main(int argc, char* argv[]) {
  // Initialize random global key
  randomGlobalKeyByteSequence = ByteSequence::getRandomByteSequence(BLOCK_SIZE);

  std::vector<std::string> base64EncodedStrings = {
      "SSBoYXZlIG1ldCB0aGVtIGF0IGNsb3NlIG9mIGRheQ==",
      "Q29taW5nIHdpdGggdml2aWQgZmFjZXM=",
      "RnJvbSBjb3VudGVyIG9yIGRlc2sgYW1vbmcgZ3JleQ==",
      "RWlnaHRlZW50aC1jZW50dXJ5IGhvdXNlcy4=",
      "SSBoYXZlIHBhc3NlZCB3aXRoIGEgbm9kIG9mIHRoZSBoZWFk",
      "T3IgcG9saXRlIG1lYW5pbmdsZXNzIHdvcmRzLA==",
      "T3IgaGF2ZSBsaW5nZXJlZCBhd2hpbGUgYW5kIHNhaWQ=",
      "UG9saXRlIG1lYW5pbmdsZXNzIHdvcmRzLA==",
      "QW5kIHRob3VnaHQgYmVmb3JlIEkgaGFkIGRvbmU=",
      "T2YgYSBtb2NraW5nIHRhbGUgb3IgYSBnaWJl",
      "VG8gcGxlYXNlIGEgY29tcGFuaW9u",
      "QXJvdW5kIHRoZSBmaXJlIGF0IHRoZSBjbHViLA==",
      "QmVpbmcgY2VydGFpbiB0aGF0IHRoZXkgYW5kIEk=",
      "QnV0IGxpdmVkIHdoZXJlIG1vdGxleSBpcyB3b3JuOg==",
      "QWxsIGNoYW5nZWQsIGNoYW5nZWQgdXR0ZXJseTo=",
      "QSB0ZXJyaWJsZSBiZWF1dHkgaXMgYm9ybi4=",
      "VGhhdCB3b21hbidzIGRheXMgd2VyZSBzcGVudA==",
      "SW4gaWdub3JhbnQgZ29vZCB3aWxsLA==",
      "SGVyIG5pZ2h0cyBpbiBhcmd1bWVudA==",
      "VW50aWwgaGVyIHZvaWNlIGdyZXcgc2hyaWxsLg==",
      "V2hhdCB2b2ljZSBtb3JlIHN3ZWV0IHRoYW4gaGVycw==",
      "V2hlbiB5b3VuZyBhbmQgYmVhdXRpZnVsLA==",
      "U2hlIHJvZGUgdG8gaGFycmllcnM/",
      "VGhpcyBtYW4gaGFkIGtlcHQgYSBzY2hvb2w=",
      "QW5kIHJvZGUgb3VyIHdpbmdlZCBob3JzZS4=",
      "VGhpcyBvdGhlciBoaXMgaGVscGVyIGFuZCBmcmllbmQ=",
      "V2FzIGNvbWluZyBpbnRvIGhpcyBmb3JjZTs=",
      "SGUgbWlnaHQgaGF2ZSB3b24gZmFtZSBpbiB0aGUgZW5kLA==",
      "U28gc2Vuc2l0aXZlIGhpcyBuYXR1cmUgc2VlbWVkLA==",
      "U28gZGFyaW5nIGFuZCBzd2VldCBoaXMgdGhvdWdodC4=",
      "VGhpcyBvdGhlciBtYW4gSSBoYWQgZHJlYW1lZA==",
      "QSBkcnVua2VuLCB2YWluLWdsb3Jpb3VzIGxvdXQu",
      "SGUgaGFkIGRvbmUgbW9zdCBiaXR0ZXIgd3Jvbmc=",
      "VG8gc29tZSB3aG8gYXJlIG5lYXIgbXkgaGVhcnQs",
      "WWV0IEkgbnVtYmVyIGhpbSBpbiB0aGUgc29uZzs=",
      "SGUsIHRvbywgaGFzIHJlc2lnbmVkIGhpcyBwYXJ0",
      "SW4gdGhlIGNhc3VhbCBjb21lZHk7",
      "SGUsIHRvbywgaGFzIGJlZW4gY2hhbmdlZCBpbiBoaXMgdHVybiw=",
      "VHJhbnNmb3JtZWQgdXR0ZXJseTo=",
      "QSB0ZXJyaWJsZSBiZWF1dHkgaXMgYm9ybi4="};

  std::vector<ByteSequence> ciphertextByteSequences;
  std::vector<ByteSequence> plaintextByteSequences;  // todo: remove

  std::cout << "Plaintext byte sequences:" << std::endl;
  for (auto plaintextString : base64EncodedStrings) {
    ByteSequence plaintextByteSequence;
    plaintextByteSequence.initializeFromBase64EncodedAsciiBytes(
        std::vector<char>(&plaintextString[0],
                          &plaintextString[plaintextString.size()]));

    // todo: remove
    plaintextByteSequence.printAsciiString();
    plaintextByteSequences.push_back(plaintextByteSequence);

    ciphertextByteSequences.push_back(
        Encryptor::getAES128BitCTRModeEncryptedByteSequence(
            plaintextByteSequence, randomGlobalKeyByteSequence));
  }

  std::cout << std::endl;

  // todo: remove
  //// Print plaintext columns
  unsigned maxPlaintextByteSequenceLength = 0;
  for (auto plaintextByteSequence : plaintextByteSequences) {
    if (plaintextByteSequence.getByteCount() > maxPlaintextByteSequenceLength)
      maxPlaintextByteSequenceLength = plaintextByteSequence.getByteCount();
  }


  // todo: remove
  std::cout << "Plaintext columns:" << std::endl;
  std::vector<ByteSequence> plaintextColumnByteSequences;
  for (unsigned i = 0; i < maxPlaintextByteSequenceLength; ++i) {
    ByteSequence plaintextColumnByteSequence;
    for (auto plaintextByteSequence : plaintextByteSequences) {
      if (i < plaintextByteSequence.getByteCount()) {
        plaintextColumnByteSequence.appendAsciiBytes(
            std::vector<char>{plaintextByteSequence.getBytes()[i]});
      }
    }

    plaintextColumnByteSequence.printAsciiString();
    plaintextColumnByteSequences.push_back(plaintextColumnByteSequence);
  }

  std::cout << std::endl;

  // Construct ciphertext columns
  std::vector<ByteSequence> ciphertextColumnByteSequences;
  unsigned limit = maxPlaintextByteSequenceLength;
  for (unsigned i = 0; i < limit; ++i) {
    ByteSequence nextCiphertextColumnByteSequence;
    for (auto ciphertextByteSequence : ciphertextByteSequences) {
      if (i < ciphertextByteSequence.getByteCount()) {
        nextCiphertextColumnByteSequence.appendAsciiBytes(
            std::vector<char>{ciphertextByteSequence.getBytes()[i]});
      }
    }

    if (nextCiphertextColumnByteSequence.getByteCount() < MINIMUM_COLUMN_SIZE)
      break;

    ciphertextColumnByteSequences.push_back(nextCiphertextColumnByteSequence);
  }

  // Crack the keystream bytes
  ByteSequence crackedKeyStreamByteSequence;
  for (auto ciphertextColumnByteSequence : ciphertextColumnByteSequences) {
    char keyByte = Cracker::getKeyFromSingleCharacterXorEncryptedByteSequence(
        ciphertextColumnByteSequence);
    crackedKeyStreamByteSequence.appendAsciiBytes(std::vector<char>{keyByte});
  }

  std::vector<ByteSequence> crackedPlaintextByteSequences;
  for (auto ciphertextByteSequence : ciphertextByteSequences) {
    ByteSequence crackedPlaintextByteSequence;
    for (unsigned i = 0; i < crackedKeyStreamByteSequence.getByteCount(); ++i) {
      if (i < ciphertextByteSequence.getByteCount()) {
        crackedPlaintextByteSequence.appendAsciiBytes(std::vector<char>{
            static_cast<char>(ciphertextByteSequence.getBytes()[i] ^
                              crackedKeyStreamByteSequence.getBytes()[i])});
      }
    }

    crackedPlaintextByteSequences.push_back(crackedPlaintextByteSequence);
  }

  // todo: remove
  // Construct and print cracked plaintext byte sequence columns
  std::cout << "Cracked plaintext column byte sequences:" << std::endl;
  for (unsigned i = 0; i < ciphertextColumnByteSequences.size(); ++i) {
    ByteSequence crackedPlaintextColumnByteSequence;
    for (auto crackedPlaintextByteSequence : crackedPlaintextByteSequences) {
      if (i < crackedPlaintextByteSequence.getByteCount()) {
        crackedPlaintextColumnByteSequence.appendAsciiBytes(
            std::vector<char>{crackedPlaintextByteSequence.getBytes()[i]});
      }
    }

    if (crackedPlaintextColumnByteSequence.getLowerCaseAsciiByteSequence() ==
        plaintextColumnByteSequences[i].getLowerCaseAsciiByteSequence()) {
      std::cout << "Correct!! (byte count: "
                << crackedPlaintextColumnByteSequence.getByteCount() << ")"
                << std::endl;
    } else {
      std::cout << "INCORRECT" << std::endl;
      std::cout << "--------------------" << std::endl;
      crackedPlaintextColumnByteSequence.printAsciiString();
      std::cout << "vs." << std::endl;
      plaintextColumnByteSequences[i].printAsciiString();
      std::cout << "Byte count: "
                << crackedPlaintextColumnByteSequence.getByteCount()
                << std::endl;
      std::cout << "--------------------" << std::endl;
    }
  }

  std::cout << std::endl;

  // Crack the remaining keystream bytes
  int remainingColumnCount =
      maxPlaintextByteSequenceLength - ciphertextColumnByteSequences.size();

  // todo: remove
  std::cout << "Remaining column count: " << remainingColumnCount << std::endl;

  for (int i = 0; i < remainingColumnCount; ++i) {
    std::vector<ByteSequence> nextCiphertextByteSequences;
    for (auto ciphertextByteSequence : ciphertextByteSequences) {
      if (ciphertextByteSequence.getByteCount() >
          ciphertextColumnByteSequences.size() + i) {
        nextCiphertextByteSequences.push_back(
            ciphertextByteSequence.getSubSequence(
                0, ciphertextColumnByteSequences.size() + i + 1));
      }
    }

    /*
    std::cout << "Cracking " << nextCiphertextByteSequences.size()
              << " ciphertext byte sequences" << std::endl;

    std::cout << "Sizes:" << std::endl;
    for (auto nextCiphertextByteSequence : nextCiphertextByteSequences) {
      std::cout << nextCiphertextByteSequence.getByteCount() << std::endl;
    }
    */

    char nextCrackedKeyStreamByte =
        Cracker::getNextKeystreamByteFromFixedNonceStreamEncryptedByteSequences(
            nextCiphertextByteSequences, crackedKeyStreamByteSequence);
    crackedKeyStreamByteSequence.appendAsciiBytes(
        std::vector<char>{nextCrackedKeyStreamByte});
  }

  std::cout << std::endl;

  std::vector<ByteSequence> updatedCrackedPlaintextByteSequences;
  for (auto ciphertextByteSequence : ciphertextByteSequences) {
    ByteSequence crackedPlaintextByteSequence;
    for (unsigned i = 0; i < crackedKeyStreamByteSequence.getByteCount(); ++i) {
      if (i < ciphertextByteSequence.getByteCount()) {
        crackedPlaintextByteSequence.appendAsciiBytes(std::vector<char>{
            static_cast<char>(ciphertextByteSequence.getBytes()[i] ^
                              crackedKeyStreamByteSequence.getBytes()[i])});
      }
    }

    updatedCrackedPlaintextByteSequences.push_back(
        crackedPlaintextByteSequence);
  }


  // todo: remove
  std::cout << "Updated cracked plaintext column byte sequences:" << std::endl;
  for (unsigned i = 0; i < maxPlaintextByteSequenceLength; ++i) {
    ByteSequence updatedCrackedPlaintextColumnByteSequence;
    for (auto updatedCrackedPlaintextByteSequence :
         updatedCrackedPlaintextByteSequences) {
      if (i < updatedCrackedPlaintextByteSequence.getByteCount()) {
        updatedCrackedPlaintextColumnByteSequence.appendAsciiBytes(
            std::vector<char>{
                updatedCrackedPlaintextByteSequence.getBytes()[i]});
      }
    }

    if (updatedCrackedPlaintextColumnByteSequence
            .getLowerCaseAsciiByteSequence() ==
        plaintextColumnByteSequences[i].getLowerCaseAsciiByteSequence()) {
      std::cout << "Correct!! (byte count: "
                << updatedCrackedPlaintextColumnByteSequence.getByteCount()
                << ")" << std::endl;
    } else {
      std::cout << "INCORRECT" << std::endl;
      std::cout << "--------------------" << std::endl;
      updatedCrackedPlaintextColumnByteSequence.printAsciiString();
      std::cout << "vs." << std::endl;
      plaintextColumnByteSequences[i].printAsciiString();
      std::cout << "Byte count: "
                << updatedCrackedPlaintextColumnByteSequence.getByteCount()
                << std::endl;
      std::cout << "--------------------" << std::endl;
    }
  }

  std::cout << std::endl;

  // Print cracked plaintext byte sequences
  std::cout << "Final cracked plaintext byte sequences:" << std::endl;
  for (auto updatedCrackedPlaintextByteSequence :
       updatedCrackedPlaintextByteSequences) {
    updatedCrackedPlaintextByteSequence.printAsciiString();
  }

  return 0;
}
