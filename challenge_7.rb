require 'openssl'

class CryptECB
  def initialize(key: SecureRandom.random_bytes(16))
    @key = key
  end

  def encrypt(plaintext)
    cipher = OpenSSL::Cipher.new('AES-128-ECB')
    cipher.encrypt
    cipher.key = @key
    cipher.update(plaintext) + cipher.final
  end

  def decrypt(ciphertext)
    cipher = OpenSSL::Cipher.new('AES-128-ECB')
    cipher.decrypt
    cipher.key = @key
    cipher.update(ciphertext) + cipher.final
  end
end

ciphertexts = File.readlines('7.txt')
ciphertext = decode_base64(ciphertexts.map(&:strip).join(''))

c = CryptECB.new(key: 'YELLOW SUBMARINE')
p c.decrypt(ciphertext)
