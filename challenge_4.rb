require 'pry-byebug'

require_relative 'challenge_1.rb'
require_relative 'challenge_3.rb'

ciphertexts = File.readlines('4.txt')

ciphertext = ciphertexts.map(&:strip).map do |ciphertext|
  ciphertext = decode_hex(ciphertext)
  cracked_key = crack_single_char_xor_key(ciphertext)
  cracked_plaintext = xor(ciphertext, cracked_key)
  score = english_monogram_frequency_score(cracked_plaintext)
  {
    plaintext: cracked_key,
    score: score,
  }
end.sort_by { |h| h[:score] }.reverse.first[:plaintext]

p ciphertext
