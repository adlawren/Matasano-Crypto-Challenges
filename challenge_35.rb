require 'pry-byebug'

require_relative 'challenge_34.rb'

p = 0xffffffffffffffffc90fdaa22168c234c4c6628b80dc1cd129024e088a67cc74020bbea63b139b22514a08798e3404ddef9519b3cd3a431b302b0a6df25f14374fe1356d6d51c245e485b576625e7ec6f44c42e9a637ed6b0bff5cb6f406b7edee386bfb5a899fa5ae9f24117c4b1fe649286651ece45b3dc2007cb8a163bf0598da48361c55d39a69163fa8fd24cf5f83655d23dca3ad961c62f356208552bb9ed529077096966d670c354e4abc9804f1746c08ca237327ffffffffffffffff
p_bit_length = p.to_s(2).size

[
  {
    mitm_g: 1,
    expected_secret: 1,
  },
  {
    mitm_g: p,
    expected_secret: 0,
  },
  {
    mitm_g: p - 1,
    expected_secret: [1, p - 1], # Could be one or the other, see 35/plot.rb for details
  },
].each do |test_case|
  mitm_g = test_case[:mitm_g]
  expected_secret = test_case[:expected_secret]

  a_priv = SecureRandom.rand(2**p_bit_length - 1)
  a_pub = modexp(base: mitm_g, exp: a_priv, mod: p)

  b_priv = SecureRandom.rand(2**p_bit_length - 1)
  b_pub = modexp(base: mitm_g, exp: b_priv, mod: p)

  a_secret = modexp(base: b_pub, exp: a_priv, mod: p)
  b_secret = modexp(base: a_pub, exp: b_priv, mod: p)

  test_message = 'This is a test message'

  a_crypt = CryptDH.new(secret: a_secret)
  a_ciphertext = a_crypt.encrypt(test_message)

  b_crypt = CryptDH.new(secret: b_secret)
  b_ciphertext = b_crypt.encrypt(test_message)

  if expected_secret.is_a?(Array)
    any_secret_valid_a = expected_secret.map do |secret|
      mitm_crypt = CryptDH.new(secret: secret)
      begin
        mitm_crypt.decrypt(a_ciphertext) == test_message
      rescue
        false
      end
    end.any?
    any_secret_valid_b = expected_secret.map do |secret|
      mitm_crypt = CryptDH.new(secret: secret)
      begin
        mitm_crypt.decrypt(b_ciphertext) == test_message
      rescue
        false
      end
    end.any?

    raise 'MITM didn\'t crack secrets' unless any_secret_valid_a
    raise 'MITM didn\'t crack secrets' unless any_secret_valid_b
  else
    mitm_crypt = CryptDH.new(secret: expected_secret)

    raise 'MITM didn\'t crack secrets' unless mitm_crypt.decrypt(a_ciphertext) == test_message
    raise 'MITM didn\'t crack secrets' unless mitm_crypt.decrypt(b_ciphertext) == test_message
  end
end
