require 'securerandom'

require_relative 'challenge_7.rb'
require_relative 'challenge_8.rb'
require_relative 'challenge_10.rb'

text = Array.new(64, 0).pack('C*')

def rand_encrypt(text)
  key = SecureRandom.random_bytes(16)
  iv = SecureRandom.random_bytes(16)

  if SecureRandom.rand(2) == 0
    c = CryptECB.new(key: key)
    c.encrypt(text)
  else
    c = CryptCBC.new(key: key, iv: iv)
    c.encrypt(text)
  end
end

def is_ecb?(ciphertext)
  count_duplicate_blocks(ciphertext) > 0
end

(0..10).each do |idx|
  ct = rand_encrypt(text)
  p "Is ECB? #{is_ecb?(ct)}"
end
