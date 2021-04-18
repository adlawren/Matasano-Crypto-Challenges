require 'securerandom'
require 'pry-byebug'

unknown = decode_base64("Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK")

r = Random.new
prefix = SecureRandom.random_bytes(16 + r.rand(48))

key = SecureRandom.random_bytes(16)

def ecb_append_unknown2(prefix, text, unknown, key)
  c = CryptECB.new(key: key)
  c.encrypt(prefix + text + unknown)
end

c = CryptECB.new(key: key)

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

block_size = find_block_size do |pt|
  ecb_append_unknown2(prefix, pt, unknown, key)
end

p "Block size: #{block_size}"

ct = c.encrypt('0' * 64)

p "Is ECB-encrypted: #{is_ecb?(ct)}"

def crack_ecb_appended2(&block)
  block_size = find_block_size(&block)
  cracked = ''
  prepended_size = ecb_prepended_size(&block)
  bumper_text = 'a' * (block_size  - (prepended_size % block_size))
  throwaway_size = prepended_size + bumper_text.size
  block_count = yield(bumper_text).size / block_size
  block_count -= throwaway_size / block_size
  (1...block_count).each do |block_number|
    (0..15).reverse_each do |size|
      ct = yield(bumper_text + 'a' * size).slice(throwaway_size, block_size * block_number)
      cracked_byte = (0..255).find do |guess|
        ct == yield(bumper_text + 'a' * size + cracked + guess.chr).slice(throwaway_size, block_size * block_number)
      end.chr
      cracked += cracked_byte
    end
  end
  appended_size = ecb_appended_size(&block)
  remaining_bytes = appended_size % block_size
  (15 - remaining_bytes + 1..15).reverse_each do |size|
    ct = yield(bumper_text + 'a' * size).slice(throwaway_size, block_size * block_count)
    cracked_byte = (0..255).find do |guess|
      ct == yield(bumper_text + 'a' * size + cracked + guess.chr).slice(throwaway_size, block_size * block_count)
    end.chr
    cracked += cracked_byte
  end
  cracked
end

cracked = crack_ecb_appended2 do |pt|
  ecb_append_unknown2(prefix, pt, unknown, key)
end

p cracked
