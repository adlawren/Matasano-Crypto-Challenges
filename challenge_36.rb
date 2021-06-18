require 'openssl'
require 'pry-byebug'
require 'securerandom'

require_relative 'challenge_33.rb'

class SRP
  # References:
  # - https://en.wikipedia.org/wiki/Secure_Remote_Password_protocol#Example_code_in_Python
  # - https://sources.debian.org/src/openssl/1.1.1k-1/crypto/srp/srp_lib.c/#L176-L221
  # - https://sources.debian.org/src/openssl/1.1.1k-1/crypto/srp/srp_lib.c/#L58-L86

  attr_reader :client_key, :server_key

  N = DH::P
  G = 2
  K = 3

  def initialize(email:, password:, client_pub_key: nil, server_pub_key: nil)
    x = x(salt: salt, password: password)
    v = v(x: x)

    client_priv_key = rand_priv_key
    if client_pub_key.nil?
      client_pub_key = client_pub_key(client_priv_key)
    end

    server_priv_key = rand_priv_key
    if server_pub_key.nil?
      server_pub_key = server_pub_key(priv_key: server_priv_key, v: v)
    end

    u = u(client_pub_key: client_pub_key, server_pub_key: server_pub_key)

    @client_key = generate_client_key(
      x: x,
      u: u,
      client_priv_key: client_priv_key,
      server_pub_key: server_pub_key)
    @server_key = generate_server_key(
      v: v,
      u: u,
      server_priv_key: server_priv_key,
      client_pub_key: client_pub_key)
  end

  private def generate_client_key(x:, u:, client_priv_key:, server_pub_key:)
    secret = modexp(base: server_pub_key - ((K * modexp(base: G, exp: x, mod: N)) % N) % N, exp: client_priv_key + u * x, mod: N)
    key = OpenSSL::Digest::SHA256.digest(encode_big_endian_int(secret))

    OpenSSL::HMAC.hexdigest("SHA256", key, encode_big_endian_int(salt))
  end

  private def generate_server_key(v:, u:, server_priv_key:, client_pub_key:)
    secret = modexp(base: client_pub_key * modexp(base: v, exp: u, mod: N) % N, exp: server_priv_key, mod: N)
    key = OpenSSL::Digest::SHA256.digest(encode_big_endian_int(secret))

    server_key = OpenSSL::HMAC.hexdigest("SHA256", key, encode_big_endian_int(salt))
  end

  def salt
    @salt ||= SecureRandom.rand(2**256 - 1)
  end

  private def x(salt:, password:)
    xH = OpenSSL::Digest::SHA256.digest(encode_big_endian_int(salt) + password)
    @x ||= xH.to_i(16)
  end

  private def v(x:)
    @v ||= modexp(base: G, exp: x, mod: N)
  end

  private def u(client_pub_key:, server_pub_key:)
    uH = OpenSSL::Digest::SHA256.digest(encode_big_endian_int(client_pub_key) + encode_big_endian_int(server_pub_key))
    @u ||= uH.to_i(16)
  end

  private def rand_priv_key
    n_bit_length = N.to_s(2).size
    SecureRandom.rand(2**n_bit_length - 1)
  end

  private def client_pub_key(priv_key)
    @client_pub_key ||= modexp(base: G, exp: priv_key, mod: N)
  end

  private def server_pub_key(priv_key:, v:)
    @server_pub_key ||= ((K*v % N) + modexp(base: G, exp: priv_key, mod: N)) % N
  end
end

email = 'random.email@domain.com'
password = 'super secret password that is reasonably long so as to be somewhat difficult to guess'

srp = SRP.new(email: email, password: password)
client_key = srp.client_key
server_key = srp.server_key

raise 'Client and server have different HMACs' unless client_key == server_key
