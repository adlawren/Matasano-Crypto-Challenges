require 'pry-byebug'

# Differences from the original solution:
# - Using 'role=admin' as admin tuple instead of 'admin=true'

crypt = CryptCBC.new

class Profile2
  def initialize(**args)
    @args = args
  end

  def is_admin?
    @args[:role] == 'admin'
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
    from_s(crypt.decrypt(encoded))
  end
end

hash = {
  comment1: 'cooking%20MCs',
  userdata: 'abc0role0admin',
  comment2: '%20like%20a%20pound%20of%20bacon'
}
encoded_profile = Profile2.new(hash).encode(crypt: crypt)

# Convert the first '0' into an ';', and the second '0' into a '='
# First '0' is in 36th position
# Second '0' is in 41st position

original_byte = encoded_profile[19].bytes.first
new_byte = ';'.ord ^ '0'.ord ^ original_byte
encoded_profile[19] = new_byte.chr

original_byte2 = encoded_profile[24].bytes.first
new_byte2 = '='.ord ^ '0'.ord ^ original_byte2
encoded_profile[24] = new_byte2.chr

p "Is admin profile: #{Profile2.decode(encoded: encoded_profile, crypt: crypt).is_admin?}"
