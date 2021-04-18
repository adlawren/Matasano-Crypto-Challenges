require 'securerandom'
require 'pry-byebug'

unknown = decode_base64("Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK")

key = SecureRandom.random_bytes(16)

def ecb_append_unknown(text, unknown, key)
  c = CryptECB.new(key: key)
  c.encrypt(text + unknown)
end

c = CryptECB.new(key: key)

def find_block_size
  prev_size = yield('').size
  size = (1..).find do |size|
    yield('0' * size).size > prev_size
  end

  yield('0' * size).size - prev_size
end

def appended_size(&block)
  # Note: this will only be accurate if there is NO prepended text
  prev_size = yield('').size
  size = (1..).find do |size|
    yield('0' * size).size > prev_size
  end
  prev_size - size
end

block_size = find_block_size do |pt|
  ecb_append_unknown(pt, unknown, key)
end

p "Block size: #{block_size}"

ct = c.encrypt('0' * 64)

p "Is ECB-encrypted: #{is_ecb?(ct)}"

def crack_ecb_appended(&block)
  block_size = find_block_size(&block)
  cracked = ''
  block_count = yield('').size / block_size
  (1...block_count).each do |block_number|
    (0..15).reverse_each do |size|
      ct = yield('a' * size).slice(0, block_size * block_number)
      cracked_byte = (0..255).find do |guess|
        ct == yield('a' * size + cracked + guess.chr).slice(0, block_size * block_number)
      end.chr
      cracked += cracked_byte
    end
  end
  appended_size = appended_size(&block)
  remaining_bytes = appended_size % block_size
  (15 - remaining_bytes + 1..15).reverse_each do |size|
    ct = yield('a' * size).slice(0, block_size * block_count)
    cracked_byte = (0..255).find do |guess|
      ct == yield('a' * size + cracked + guess.chr).slice(0, block_size * block_count)
    end.chr
    cracked += cracked_byte
  end
  cracked
end

cracked = crack_ecb_appended do |pt|
  ecb_append_unknown(pt, unknown, key)
end

p cracked
