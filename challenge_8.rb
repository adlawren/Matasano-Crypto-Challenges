require 'pry-byebug'

require_relative 'challenge_1.rb'

def count_duplicate_blocks(ciphertext)
  block_counts = Hash.new(0)
  ciphertext.bytes.each_slice(16).each do |block|
    block_counts[block] += 1
  end

  block_counts.reject { |k, v| v < 2 }.size
end

ciphertexts = File.readlines('8.txt')

res = ciphertexts.map(&:strip).map do |ciphertext|
  ciphertext = decode_hex(ciphertext)
  {
    ciphertext: ciphertext,
    duplicate_block_count: count_duplicate_blocks(ciphertext),
  }
end

p res.sort_by { |h| h[:duplicate_block_count] }.reverse.first[:ciphertext]

