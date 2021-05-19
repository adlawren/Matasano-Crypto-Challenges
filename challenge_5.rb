require 'pry-byebug'

require_relative 'challenge_1.rb'
require_relative 'challenge_2.rb'

module Crypt
  def self.repeating_key_xor(plaintext, key)
    plaintext.bytes.each_slice(key.size).map do |slice|
      xor(slice.pack('C*'), key)
    end.join('')
  end
end

plaintext = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal"
p encode_hex(Crypt.repeating_key_xor(plaintext, 'ICE'))
