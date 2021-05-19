require 'pry-byebug'
require 'securerandom'

require_relative 'challenge_7.rb'
require_relative 'challenge_18.rb'

plaintext = File.readlines('25.txt').join('')
unknown = CryptECB.new(key: 'YELLOW SUBMARINE').decrypt(decode_base64(File.readlines('25.txt').map(&:strip).join('')))

def edit(ciphertext:, crypt:, offset:, new_plaintext:)
  plaintext = crypt.decrypt(ciphertext)
  plaintext[offset...offset + new_plaintext.size] = new_plaintext
  crypt.encrypt(plaintext)
end

key = SecureRandom.random_bytes(16)
nonce = SecureRandom.random_bytes(8)
crypt = CryptCTR.new(key: key, nonce: nonce)

plaintext = 'this is a reasonably long sentence, it needs to be fairly long to ensure that things are working properly'
ciphertext = crypt.encrypt(unknown)

# There's a few different approaches that could work here:
# - Guess every possible byte in each position, compare to ciphertext, find match
# - Edit ciphertext at a position to a known value, XOR the known value w/ the ciphertext to get keystream byte, XOR keystream w/ original ciphertext
# Using the latter approach below

new_plaintext = 'a' * ciphertext.size
new_ciphertext = edit(ciphertext: ciphertext, crypt: crypt, offset: 0, new_plaintext: new_plaintext)
cracked_keystream = xor(new_ciphertext, new_plaintext)
cracked_plaintext = xor(ciphertext, cracked_keystream)

p cracked_plaintext
