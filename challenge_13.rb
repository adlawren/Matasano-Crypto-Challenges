require 'pry-byebug'

class Profile
  def initialize(email: email, id: id, role: role)
    @email = email
    @id = id
    @role = role
  end

  def encode(key: key)
    c = CryptECB.new(key: key)
    c.encrypt(URI.encode_www_form({email: @email, uid: @id, role: @role}))
  end

  def self.decode(encoded: encoded, key: key)
    c = CryptECB.new(key: key)
    hash = URI.decode_www_form(c.decrypt(encoded)).to_h
    new(email: hash['email'].strip, id: hash['uid'].strip, role: hash['role'].strip)
  end
end

key = SecureRandom.random_bytes(16)

encode_proc = Proc.new do |pt|
  Profile.new(email: pt, id: 10, role: 'user').encode(key: key)
end

block_size = find_block_size(&encode_proc)

# TODO: see if there's a way to do this without making these assumptions
prepended_size = 6
appended_size = 17

wrapper_text_size = prepended_size + appended_size

# Make and concatenate the following encrypted blocks:
# 'email=abc@gmail.'
# 'com&uid=10&role=' -> a block w/ 'role=' at the end
# 'admin&uid=10&rol' -> a block which starts w/ 'admin'
# '=user\v\v\v\v\v\v\v\v\v\v\v' -> a block which starts w/ '='

email = encode_proc.call('a' * (block_size - prepended_size)).slice(0, block_size)

role_equals = encode_proc.call('a' * (block_size - (wrapper_text_size % block_size) + 'user'.size))
role_equals = role_equals.slice(role_equals.size - block_size * 2, block_size)

bumper_text = 'a' * (block_size - prepended_size)
admin = encode_proc.call(bumper_text + 'admin').slice(prepended_size + bumper_text.size, block_size)

equals_user = encode_proc.call('a' * (block_size - (wrapper_text_size % block_size) + '=user'.size))
equals_user = equals_user.slice(equals_user.size - block_size, block_size)

encoded_admin_profile = email + role_equals + admin + equals_user

p Profile.decode(encoded: encoded_admin_profile, key: key)
