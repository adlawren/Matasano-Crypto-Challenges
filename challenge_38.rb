require 'pry-byebug'

require_relative 'challenge_33.rb'

# Why does this attack depend on a simplified version of SRP?
# In the original protocol, after the client has sent its HMAC to the server, the server can't create a new HMAC using a different password and guarantee that the client would have generated the same HMAC for that password, b/c the parameters used by the client to generate its HMAC depended on parameters created by the server which were derived from a specific password (specifically the 'B' & 'u' parameters). These parameters are not derived from a specific password in this simplified version of the protocol

class SRP2
  # References:
  # - https://en.wikipedia.org/wiki/Secure_Remote_Password_protocol#Example_code_in_Python
  # - https://sources.debian.org/src/openssl/1.1.1k-1/crypto/srp/srp_lib.c/#L176-L221
  # - https://sources.debian.org/src/openssl/1.1.1k-1/crypto/srp/srp_lib.c/#L58-L86

  attr_reader :client_key, :server_key

  N = DH::P
  G = 2
  K = 3

  def initialize
    @salt = rand_salt
    @u = rand_u

    @client_priv_key = rand_priv_key
    @server_priv_key = rand_priv_key
  end

  def generate_keys!(email:, password:)
    xH = encode_hex(OpenSSL::Digest::SHA256.digest(encode_big_endian_int(@salt) + password))
    x = xH.to_i(16)

    v = modexp(base: G, exp: x, mod: N)

    client_pub_key = pub_key(@client_priv_key)
    server_pub_key = pub_key(@server_priv_key)

    @client_key = generate_client_key(
      salt: @salt,
      x: x,
      u: @u,
      client_priv_key: @client_priv_key,
      server_pub_key: server_pub_key)
    @server_key = generate_server_key(
      salt: @salt,
      v: v,
      u: @u,
      server_priv_key: @server_priv_key,
      client_pub_key: client_pub_key)
  end

  private def generate_client_key(salt:, x:, u:, client_priv_key:, server_pub_key:)
    secret = modexp(base: server_pub_key, exp: client_priv_key + ((u * x) % N) % N, mod: N)
    key = OpenSSL::Digest::SHA256.digest(encode_big_endian_int(secret))

    OpenSSL::HMAC.hexdigest("SHA256", key, encode_big_endian_int(salt))
  end

  private def generate_server_key(salt:, v:, u:, server_priv_key:, client_pub_key:)
    secret = modexp(base: client_pub_key * modexp(base: v, exp: u, mod: N) % N, exp: server_priv_key, mod: N)
    key = OpenSSL::Digest::SHA256.digest(encode_big_endian_int(secret))

    server_key = OpenSSL::HMAC.hexdigest("SHA256", key, encode_big_endian_int(salt))
  end

  private def rand_salt
    SecureRandom.rand(2**256 - 1)
  end

  private def rand_u
    SecureRandom.rand(2**128 - 1)
  end

  private def rand_priv_key
    n_bit_length = N.to_s(2).size
    SecureRandom.rand(2**n_bit_length - 1)
  end

  private def pub_key(priv_key)
    modexp(base: G, exp: priv_key, mod: N)
  end
end

email = 'random.email@domain.com'
password = 'super secret password that is reasonably long so as to be somewhat difficult to guess'

srp = SRP2.new

srp.generate_keys!(email: email, password: password)
client_key = srp.client_key
server_key = srp.server_key

raise 'Client and server have different HMACs' unless client_key == server_key

srp.generate_keys!(email: email, password: 'some other password')

raise 'SRP returning identical HMACs for different passwords' unless client_key != srp.client_key

srp.generate_keys!(email: email, password: password)

raise 'SRP not returning identical keys for same password' unless client_key == srp.client_key

password_list = [
  'password',
  'Password',
  'password1',
  'Password1',
  'password1!',
  'Password1!',
  'passw0rd',
  'Passw0rd',
  'passw0rd!',
  'Passw0rd!',
]

known_password = password_list.sample

srp = SRP2.new
srp.generate_keys!(email: email, password: known_password)
client_key = srp.client_key

cracked_password = password_list.find do |password_guess|
  srp.generate_keys!(email: email, password: password_guess)
  srp.client_key == client_key
end

raise 'Unable to crack password' unless cracked_password == known_password
