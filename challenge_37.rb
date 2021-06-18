require 'pry-byebug'

require_relative 'challenge_33.rb'
require_relative 'challenge_36.rb'

email = 'random.email@domain.com'
password = 'super secret password that is reasonably long so as to be somewhat difficult to guess'

[
  0,
  SRP::N,
  SRP::N**2,
].each do |malicious_client_pub_key|
  srp = SRP.new(email: email, password: password, client_pub_key: malicious_client_pub_key)
  server_key = srp.server_key

  client_secret = OpenSSL::Digest::SHA256.digest(encode_big_endian_int(0))
  client_key = OpenSSL::HMAC.hexdigest("SHA256", client_secret, encode_big_endian_int(srp.salt))

  raise 'Client and server have different HMACs' unless client_key == server_key
end
