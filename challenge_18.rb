require 'pry-byebug'

class CryptCTR
  def initialize(key: SecureRandom.random_bytes(16), nonce: SecureRandom.random_bytes(8))
    @key = key
    @nonce = nonce
  end

  def encrypt(plaintext)
    xor_with_keystream(plaintext)
  end

  def decrypt(ciphertext)
    xor_with_keystream(ciphertext)
  end

  private def xor_with_keystream(text)
    cipher = OpenSSL::Cipher.new('AES-128-ECB')
    cipher.padding = 0
    cipher.encrypt
    cipher.key = @key

    block_count = text.size / 16
    (0..block_count).map do |block_idx|
      block_idx_str = block_idx.to_s(16).each_char.map { |char| char.to_i }.pack('C*')
      block_idx_str = [0].pack('C*') * (8 - block_idx_str.size) + block_idx_str
      keystream_plaintext = @nonce + block_idx_str.reverse
      keystream = cipher.update(keystream_plaintext) + cipher.final
      xor(text.slice(block_idx * 16, 16), keystream)
    end.join('')
  end
end

key = 'YELLOW SUBMARINE'
nonce = [0].pack('C*') * 8
unknown = decode_base64('L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==')

crypt = CryptCTR.new(key: key, nonce: nonce)
p crypt.decrypt(unknown)
