require 'pry-byebug'
require 'securerandom'
require 'sinatra'

require_relative '../challenge_1.rb'
require_relative '../challenge_2.rb'

class HMAC
  # Sources:
  # - https://en.wikipedia.org/wiki/HMAC#Implementation
  # - https://datatracker.ietf.org/doc/html/rfc2104#section-2
  def digest(key:, data:)
    block_size = 64 # bytes
    output_size = 20 # bytes

    if key.size > block_size
      key = decode_hex(digest_class.new(key).to_s)
    end

    if key.size < block_size
      key = key.ljust(block_size, 0.chr)
    end

    outer_key_padding = xor(key, Array.new(block_size, 0x5c).pack('C*'))
    inner_key_padding = xor(key, Array.new(block_size, 0x36).pack('C*'))

    digest_class.new(outer_key_padding + decode_hex(digest_class.new(inner_key_padding + data).to_s)).to_s
  end

  private def digest_class
    OpenSSL::Digest::SHA1
  end
end

key = SecureRandom.random_bytes(128)
1000.times do |size|
  data = SecureRandom.alphanumeric(size)
  mac = HMAC.new.digest(key: key, data: data)
  expected_mac = OpenSSL::HMAC.hexdigest('sha1', key, data)
  if mac != expected_mac
    raise "Unexpected MAC: #{mac}, expected: #{expected_mac}. Input: #{data}"
  end
end

def signature_valid?(key, file, signature)
  HMAC.new.digest(key: key, data: file).chars.each.with_index do |char, index|
    if char != signature[index]
      return false
    end

    sleep(50 / 1000.to_f) # 50 milliseconds
  end

  true
end

key = SecureRandom.random_bytes(128)
get '/test' do
  file = params['file']
  signature = params['signature']

  if signature_valid?(key, file, signature)
    "Signature valid"
  else
    "Signature invalid"
  end
end
