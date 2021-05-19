require 'pry-byebug'

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

# Note: this value is 192 bytes (1536 bits) long
p = 0xffffffffffffffffc90fdaa22168c234c4c6628b80dc1cd129024e088a67cc74020bbea63b139b22514a08798e3404ddef9519b3cd3a431b302b0a6df25f14374fe1356d6d51c245e485b576625e7ec6f44c42e9a637ed6b0bff5cb6f406b7edee386bfb5a899fa5ae9f24117c4b1fe649286651ece45b3dc2007cb8a163bf0598da48361c55d39a69163fa8fd24cf5f83655d23dca3ad961c62f356208552bb9ed529077096966d670c354e4abc9804f1746c08ca237327ffffffffffffffff
p_bit_length = p.to_s(2).size
g = 2

a_priv = SecureRandom.rand(2**p_bit_length - 1)
a_pub = modexp(base: g, exp: a_priv, mod: p)

b_priv = SecureRandom.rand(2**p_bit_length - 1)
b_pub = modexp(base: g, exp: b_priv, mod: p)

a_secret = modexp(base: b_pub, exp: a_priv, mod: p)
b_secret = modexp(base: a_pub, exp: b_priv, mod: p)

raise 'Secrets are not equal' unless a_secret == b_secret
