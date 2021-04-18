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
