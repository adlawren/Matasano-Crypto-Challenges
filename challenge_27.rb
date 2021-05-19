require 'pry-byebug'
require 'securerandom'

require_relative 'challenge_10.rb'

key = SecureRandom.random_bytes(16)
crypt = CryptCBC.new(key: key, iv: key)

class Profile3
  def initialize(**args)
    @args = args
  end

  def to_s
    @args.inject('') { |str, tuple| str += "#{tuple[0]}=#{tuple[1]};" }
  end

  def self.from_s(str)
    args = str.split(';').map do |str|
      tuple = str.split('=')
      tuple[0] = tuple[0].to_sym
      tuple
    end.select do |tuple|
      tuple.size == 2
    end.to_h
    new(args)
  end

  def encode(crypt:)
    crypt.encrypt(to_s)
  end

  def self.decode(encoded:, crypt:)
    decrypted = crypt.decrypt(encoded)
    if decrypted.chars.reject { |char| is_valid?(char) }.size > 0
      raise "Invalid plaintext: #{decrypted}"
    end
    from_s(decrypted)
  end

  class << self
    private def is_valid?(char)
      return false if char < ' '
      return false if char > '~'
      true
    end
  end
end

def profile_for2(crypt:, user_data:)
  hash = {
    comment1: 'cooking%20MCs',
    userdata: user_data,
    comment2: '%20like%20a%20pound%20of%20bacon'
  }
  Profile3.new(hash).encode(crypt: crypt)
end

encoded_profile = profile_for2(crypt: crypt, user_data: 'a' * 16 * 3) # 3 blocks

encoded_profile[16...32] = ([0] * 16).pack('C*')
encoded_profile[32...48] = encoded_profile[0...16]

begin
  Profile3.decode(encoded: encoded_profile, crypt: crypt)
rescue
  decrypted = $!.to_s.delete_prefix('Invalid plaintext: ')
  cracked_key = xor(decrypted[0...16], decrypted[32...48])
  raise "Unable to crack key" unless cracked_key == key
end
