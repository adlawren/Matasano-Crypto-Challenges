require 'pry-byebug'

require_relative 'challenge_34.rb'

[
  {
    mitm_g: 1,
    expected_secret: [1].pack('C'),
  },
  {
    mitm_g: DH::P,
    expected_secret: [0].pack('C'),
  },
  {
    mitm_g: DH::P - 1,
    expected_secret: [
      [1].pack('C'),
      (DH::P - 1).to_s(16).chars.each_slice(2).map { |chars| decode_hex(chars.join('')) }.join(''),
    ], # Could be one or the other, see 35/plot.rb for details
  },
].each do |test_case|
  mitm_g = test_case[:mitm_g]
  expected_secret = test_case[:expected_secret]

  a = DH.new(g: mitm_g)
  b = DH.new(g: mitm_g)

  a_secret = a.secret(b)
  b_secret = b.secret(a)

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
