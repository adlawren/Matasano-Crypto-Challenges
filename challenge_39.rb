require 'openssl'
require 'pry-byebug'

require_relative 'challenge_33.rb'

class RSA
  attr_reader :p, :q, :n, :e, :d

  def initialize(p: random_prime, q: random_prime, e: 2**16 + 1)
    @p = p
    @q = q
    @n = p * q

    @e = e
    totient = totient(p , q)
    raise 'e and totient are not coprime' unless totient.gcd(@e) == 1

    @d = mod_inv(e, totient)
    unless @e * @d % totient == 1
      raise 'd is not a modular multiplicative inverse of e modulo totient'
    end
  end

  def encrypt(p)
    modexp(base: p, exp: @e, mod: @n)
  end

  def decrypt(c)
    modexp(base: c, exp: @d, mod: @n)
  end

  private

  def random_prime
    OpenSSL::BN.generate_prime(1024).to_i
  end

  def totient(p, q)
    (p - 1) * (q - 1)
  end

  def mod_inv(b, m)
    x, y = extended_euclid(m, b)
    if x > 0
      # In this case y will be less than 0
      # Need x to be negative, otherwise b*y = 1 - x*m is not guaranteed to be congruent to 1 modulo m, which means y will not be a modular multiplicative inverse of b
      # Bezout coefficients are not unique
      # Since x*m + y*b = 1, x*m - k*m*b + y*b + k*m*b = 1 = (x - k*b)*m + (y + k*m)*b
      # Need to find the smallest k for which x - k*b is negative
      k = (x / b) + 1
      y += k*m
    end
    y
  end

  # Compute the Bezout coefficients x & y which satisfy: x*a + y*b = gcd(a, b)
  def extended_euclid(a, b)
    if a < b
      swap_values = true
    end

    if swap_values
      a, b = b, a
    end

    remainders = []
    quotients = []
    a_tmp, b_tmp = a, b
    while a_tmp % b_tmp > 0
      remainders.push(a_tmp % b_tmp)
      quotients.push(a_tmp / b_tmp)

      a_tmp, b_tmp = b_tmp, a_tmp % b_tmp
    end

    x_tmp, y_tmp = 1, -quotients[-1]
    quotients.take(quotients.size - 1).reverse.each do |quotient|
      x_tmp, y_tmp = y_tmp, x_tmp - y_tmp * quotient
    end

    unless x_tmp*a + y_tmp*b == remainders[-1]
      raise 'Failed to compute Bezout coefficients'
    end

    if swap_values
      x_tmp, y_tmp = y_tmp, x_tmp
    end

    return x_tmp, y_tmp
  end
end

1000.times.each do
  openssl_rsa = OpenSSL::PKey::RSA.new(1024)
  rsa = RSA.new(p: openssl_rsa.p.to_i, q: openssl_rsa.q.to_i, e: openssl_rsa.e.to_i)
  unless rsa.d == openssl_rsa.d.to_i
    raise 'RSA didn\'t compute correct private key'
  end
end

rsa = RSA.new

raise 'RSA failed to encrypt/decrypt correctly' unless rsa.decrypt(rsa.encrypt(42))
