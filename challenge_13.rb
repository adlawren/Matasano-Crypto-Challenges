require 'pry-byebug'

class Profile
  def initialize(**args)
    @args = args
  end

  def is_admin?
    @args[:role] == 'admin'
  end

  def to_s
    URI.encode_www_form(@args)
  end

  def self.from_s(str)
    hash = URI.decode_www_form(str).to_h
    new_hash = hash.keys.inject({}) do |new_hash, key|
      new_hash.merge({ key.to_sym => hash[key] })
    end
    new(new_hash)
  end

  def encode(crypt:)
    crypt.encrypt(to_s)
  end

  def self.decode(encoded:, crypt:)
    from_s(crypt.decrypt(encoded))
  end
end

crypt = CryptECB.new

encode_proc = Proc.new do |pt|
  Profile.new(email: pt, uid: 10, role: 'user').encode(crypt: crypt)
end

block_size = find_block_size(&encode_proc)

def prepended_size(&block)
  # Note: this will only be accurate if there is NO appended text
  appended_size(&block)
end

def common_prefix_size(s1, s2)
  s2.each_char.take_while.with_index do |char, idx|
    s1[idx] == char
  end.size
end

def ecb_prepended_size(&block)
  block_size = find_block_size(&block)
  prev_ct = yield('a')
  prev_common_prefix_size = common_prefix_size(yield(''), prev_ct)
  size = (2..).find do |size|
    ct = yield('a' * size)
    common_prefix_size = common_prefix_size(prev_ct, ct)
    if common_prefix_size >= prev_common_prefix_size + block_size
      true
    else
      prev_ct = ct
      prev_common_prefix_size = common_prefix_size
      false
    end
  end
  (prev_common_prefix_size + block_size) - (size - 1)
end

def ecb_appended_size(&block)
  prepended_size = ecb_prepended_size(&block)

  # appended_size will return prepended_size + appended_size;
  # it only returns the appended_size if the prepended_size is 0,
  # otherwise it returns the sum of the two
  appended_size(&block) - prepended_size
end

prepended_size = ecb_prepended_size do |pt|
  Profile.new(email: pt, uid: 10, role: 'user').encode(crypt: crypt)
end

appended_size = ecb_appended_size do |pt|
  Profile.new(email: pt, uid: 10, role: 'user').encode(crypt: crypt)
end

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

p "Is admin profile: #{Profile.decode(encoded: encoded_admin_profile, crypt: crypt).is_admin?}"
