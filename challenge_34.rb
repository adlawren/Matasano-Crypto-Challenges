require 'openssl'
require 'pry-byebug'

require_relative 'challenge_10.rb'
require_relative 'challenge_33.rb'

p = 0xffffffffffffffffc90fdaa22168c234c4c6628b80dc1cd129024e088a67cc74020bbea63b139b22514a08798e3404ddef9519b3cd3a431b302b0a6df25f14374fe1356d6d51c245e485b576625e7ec6f44c42e9a637ed6b0bff5cb6f406b7edee386bfb5a899fa5ae9f24117c4b1fe649286651ece45b3dc2007cb8a163bf0598da48361c55d39a69163fa8fd24cf5f83655d23dca3ad961c62f356208552bb9ed529077096966d670c354e4abc9804f1746c08ca237327ffffffffffffffff
p_bit_length = p.to_s(2).size
g = 2

class CryptDH
  def initialize(secret:)
    @secret = secret.to_s
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

crypt = CryptDH.new(secret: SecureRandom.rand(2**p_bit_length - 1))
plaintext = 'abcdefghijklmnopqrstuvwxyz'
if crypt.decrypt(crypt.encrypt(plaintext)) != plaintext
  raise 'CryptDH returned incorrect plaintext'
end

a_priv = SecureRandom.rand(2**p_bit_length - 1)
mitm_a_pub = p

b_priv = SecureRandom.rand(2**p_bit_length - 1)
mitm_b_pub = p

a_secret = modexp(base: mitm_b_pub, exp: a_priv, mod: p)
b_secret = modexp(base: mitm_a_pub, exp: b_priv, mod: p)

test_message = 'This is a test message'

a_crypt = CryptDH.new(secret: a_secret)
a_ciphertext = a_crypt.encrypt(test_message)

b_crypt = CryptDH.new(secret: b_secret)
b_ciphertext = b_crypt.encrypt(test_message)

# B/c the 'public keys' used by both parties are multiples of p, their secrets will be 0
mitm_crypt = CryptDH.new(secret: 0)

raise 'MITM didn\'t crack secrets' unless mitm_crypt.decrypt(a_ciphertext) == test_message
raise 'MITM didn\'t crack secrets' unless mitm_crypt.decrypt(b_ciphertext) == test_message
