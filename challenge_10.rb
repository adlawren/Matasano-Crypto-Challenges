require 'openssl'
require 'pry-byebug'
require 'securerandom'

require_relative 'challenge_1.rb'
require_relative 'challenge_2.rb'
require_relative 'challenge_9.rb'

class CryptCBC
  def initialize(key: SecureRandom.random_bytes(16), iv: SecureRandom.random_bytes(16))
    @key = key
    @iv = iv
  end

  def encrypt(plaintext)
    cipher = OpenSSL::Cipher.new('AES-128-ECB')
    cipher.padding = 0
    cipher.encrypt
    cipher.key = @key

    plaintext = add_padding(plaintext)
    prev_ciphertext = ''
    plaintext.bytes.each_slice(16).with_index.map do |block, idx|
      if idx == 0
        pre_encrypted_block = xor(block.pack('C*'), @iv)
      else
        pre_encrypted_block = xor(block.pack('C*'), prev_ciphertext)
      end
      prev_ciphertext = cipher.update(pre_encrypted_block)  + cipher.final
    end.join('')
  end

  def decrypt(ciphertext)
    cipher = OpenSSL::Cipher.new('AES-128-ECB')
    cipher.padding = 0
    cipher.decrypt
    cipher.key = @key

    ciphertext.bytes.each_slice(16).with_index.map do |block, idx|
      pre_decrypted_block = cipher.update(block.pack('C*')) + cipher.final

      if idx == 0
        plaintext = xor(pre_decrypted_block, @iv)
      else
        plaintext = xor(pre_decrypted_block, ciphertext.slice(idx * 16 - 16, 16))
      end

      if idx == ciphertext.size / 16 - 1
        plaintext = remove_padding(plaintext)
      end

      plaintext
    end.join('')
  end
end

ciphertexts = File.readlines('10.txt')

ciphertext = decode_base64(ciphertexts.map(&:strip).join(''))

cbc = CryptCBC.new(key: 'YELLOW SUBMARINE', iv: Array.new(16, 0).pack('C*'))
p cbc.decrypt(ciphertext)
