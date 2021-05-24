require 'openssl'
require 'pry-byebug'

require_relative 'challenge_10.rb'
require_relative 'challenge_33.rb'

class CryptDH
  def initialize(secret:)
    @secret = secret
  end

  def encrypt(plaintext)
    crypt_cbc = CryptCBC.new(key: OpenSSL::Digest::SHA1.new.digest(@secret)[0..15])
    crypt_cbc.encrypt(plaintext) + crypt_cbc.iv
  end

  def decrypt(ciphertext)
    iv = ciphertext.slice(ciphertext.size - 16, 16)
    ciphertext = ciphertext.slice(0, ciphertext.size - 16)
    crypt_cbc = CryptCBC.new(key: OpenSSL::Digest::SHA1.new.digest(@secret)[0..15], iv: iv)
    crypt_cbc.decrypt(ciphertext)
  end
end

crypt = CryptDH.new(secret: SecureRandom.rand(2**2048 - 1).to_s(16))
plaintext = 'abcdefghijklmnopqrstuvwxyz'
if crypt.decrypt(crypt.encrypt(plaintext)) != plaintext
  raise 'CryptDH returned incorrect plaintext'
end

mitm_a_pub = DH::P
mitm_b_pub = DH::P

a = DH.new
b = DH.new

a_secret = a.secret(mitm_a_pub)
b_secret = b.secret(mitm_b_pub)

test_message = 'This is a test message'

a_crypt = CryptDH.new(secret: a_secret)
a_ciphertext = a_crypt.encrypt(test_message)

b_crypt = CryptDH.new(secret: b_secret)
b_ciphertext = b_crypt.encrypt(test_message)

# B/c the 'public keys' used by both parties are multiples of p, their secrets will be 0
mitm_crypt = CryptDH.new(secret: encode_big_endian_int(0))

raise 'MITM didn\'t crack secrets' unless mitm_crypt.decrypt(a_ciphertext) == test_message
raise 'MITM didn\'t crack secrets' unless mitm_crypt.decrypt(b_ciphertext) == test_message
