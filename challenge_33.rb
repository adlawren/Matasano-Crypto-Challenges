require 'openssl'
require 'pry-byebug'
require 'securerandom'

require_relative 'challenge_1.rb'

def modexp(base:, exp:, mod:)
  # Source: https://en.wikipedia.org/wiki/Modular_exponentiation#Pseudocode
  res = 1
  base_square_mod = (base**1) % mod
  while exp > 0
    if exp & 1 == 0
      # (base**(0*(2**i))) % mod = 1
      res = (1 * res) % mod
    elsif
      # (base**(1*(2**i))) % mod
      res = (base_square_mod * res) % mod
    end
    exp = exp >> 1
    base_square_mod = (base_square_mod * base_square_mod) % mod
  end
  res
end

base = 3
exp = 5
7.times do |index|
  mod = index + 1
  raise 'modexp returned wrong value' unless modexp(base: base, exp: exp, mod: mod) == (base**exp) % mod
end

class DH
  # Source: https://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange#Cryptographic_explanation

  # Note: this value is 192 bytes (1536 bits) long
  P = 0xffffffffffffffffc90fdaa22168c234c4c6628b80dc1cd129024e088a67cc74020bbea63b139b22514a08798e3404ddef9519b3cd3a431b302b0a6df25f14374fe1356d6d51c245e485b576625e7ec6f44c42e9a637ed6b0bff5cb6f406b7edee386bfb5a899fa5ae9f24117c4b1fe649286651ece45b3dc2007cb8a163bf0598da48361c55d39a69163fa8fd24cf5f83655d23dca3ad961c62f356208552bb9ed529077096966d670c354e4abc9804f1746c08ca237327ffffffffffffffff
  G = 2

  attr_reader :priv_key, :pub_key

  def initialize(p: P, g: G, priv_key: nil)
    @p = p
    @g = g

    p_bit_length = @p.to_s(2).size

    if priv_key.nil?
      @priv_key = SecureRandom.rand(2**p_bit_length - 1)
    else
      @priv_key = priv_key % (2**p_bit_length - 1)
    end
    @pub_key = modexp(base: @g, exp: @priv_key, mod: @p)
  end

  def secret(pub_key)
    if pub_key.is_a?(self.class)
      pub_key = pub_key.pub_key
    elsif !pub_key.is_a?(Integer)
      raise ArgumentError.new('Unsupported class for public key')
    end

    modexp = modexp(base: pub_key, exp: @priv_key, mod: @p)
    modexp_hex = modexp.to_s(16)
    if modexp_hex.size % 2 != 0
      modexp_hex = "0#{modexp_hex}"
    end
    modexp_hex.chars.each_slice(2).map do |chars|
      decode_hex(chars.join(''))
    end.join('')
  end
end

a_pkey = OpenSSL::PKey::DH.new(1024)
b_pkey = OpenSSL::PKey::DH.new(a_pkey.to_der)
b_pkey.generate_key!
a_key = a_pkey.compute_key(b_pkey.pub_key)
b_key = b_pkey.compute_key(a_pkey.pub_key)

a = DH.new(p: a_pkey.p.to_i, g: a_pkey.g.to_i, priv_key: a_pkey.priv_key.to_i)
b = DH.new(p: a_pkey.p.to_i, g: a_pkey.g.to_i, priv_key: b_pkey.priv_key.to_i)

a_secret = a.secret(b)
b_secret = b.secret(a)

raise 'Diffie-Hellman not returning correct values' unless a_key == a_secret
raise 'Diffie-Hellman not returning correct values' unless b_key == b_secret

raise 'Secrets are not equal' unless a_secret == b_secret
