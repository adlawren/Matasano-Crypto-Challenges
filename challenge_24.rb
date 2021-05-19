require 'pry-byebug'

require_relative 'challenge_2.rb'
require_relative 'challenge_21.rb'

class CryptMT19937
  def initialize(seed = Time.now.to_i)
    @seed = seed
  end

  def encrypt(plaintext)
    mt = MT19937.new(seed: @seed)
    plaintext.bytes.map do |byte|
      xor([byte].pack('C*'), [mt.rand].pack('C*'))
    end.join('')
  end

  def decrypt(ciphertext)
    mt = MT19937.new(seed: @seed)
    ciphertext.bytes.map do |byte|
      xor([byte].pack('C*'), [mt.rand].pack('C*'))
    end.join('')
  end
end

r = Random.new(Time.now.to_i)
seed = r.rand(2**16 - 1) # 16 bit maximum
crypt = CryptMT19937.new(seed)

plaintext = 19.times.map { r.rand(2**32 - 1) }.pack('C*')
decrypted_ciphertext = crypt.decrypt(crypt.encrypt(plaintext))
if plaintext != decrypted_ciphertext
  raise "MT19937 cipher isn't working: expected #{plaintext} after decryption, got #{decrypted_ciphertext}"
end

def random_prefix(unknown)
  max_prefix_size = 16
  r = Random.new(Time.now.to_i)
  r.rand(max_prefix_size).times.map { r.rand(2**8 - 1) }.pack('C*') + unknown
end

known_plaintext = 'a' * 14
plaintext = random_prefix(known_plaintext)
ciphertext = crypt.encrypt(plaintext)

cracked_seed = (0...2**16).find do |seed|
  crypt = CryptMT19937.new(seed)
  crypt.decrypt(ciphertext).include?(plaintext)
end

raise "Unable to crack seed" unless crypt.instance_variable_get(:@seed) == cracked_seed

def random_alphabetized_token(seed = nil)
  seed = seed.nil? ? Time.now.to_i : seed
  mt = MT19937.new(seed: seed)
  password_reset_token = 128.times.map do
    # Alphabetize
    (mt.rand % 26) + 97
  end.pack('C*')
end

token = random_alphabetized_token

current_time_seconds = Time.now.to_i
seeds = 100.times.map { |index| current_time_seconds - index }

cracked_seed = seeds.find do |seed|
  mt = MT19937.new(seed: seed)
  128.times.map do
    # Alphabetize
    (mt.rand % 26) + 97
  end.pack('C*') == token
end

raise "Unable to crack seed" unless random_alphabetized_token(cracked_seed) == token
