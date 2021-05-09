require 'pry-byebug'

# There's a couple of approaches that would potentially work here:
# - Crack the keystream in the appropriate section of the ciphertext, in the positions where you want to flip the bits, XOR the keystream bytes w/ the target bytes and set the ciphertext bytes to the resulting bytes
# - Flip bits in the N applicable positions until you get the target bytes
#   - The problem w/ this approach is that you'd need to try all combinations of the 256 possible values at each position; there's 256^N possibilities, which is too expensive
# Using the former approach below

crypt = CryptCTR.new

def profile_for(crypt:, user_data:)
  hash = {
    comment1: 'cooking%20MCs',
    userdata: user_data,
    comment2: '%20like%20a%20pound%20of%20bacon'
  }
  Profile2.new(hash).encode(crypt: crypt)
end

target_user_data = 'abc;role=admin'
crib_user_data = 'a' * target_user_data.size

def find_user_data_position(&block)
  encoded_profile1 = yield('a')
  encoded_profile2 = yield('b')
  encoded_profile2.bytes.take_while.with_index { |byte, index| encoded_profile1.bytes[index] == byte }.size
end


user_data_position = find_user_data_position do |plaintext|
  profile_for(crypt: crypt, user_data: plaintext)
end

encoded_profile = profile_for(crypt: crypt, user_data: crib_user_data)
cracked_keystream = xor(encoded_profile.slice(user_data_position, crib_user_data.size), crib_user_data)
encoded_profile[user_data_position...user_data_position + target_user_data.size] = xor(cracked_keystream, target_user_data)

p "Is admin profile: #{Profile2.decode(encoded: encoded_profile, crypt: crypt).is_admin?}"
