require 'pry-byebug'
require 'securerandom'

require_relative 'challenge_10.rb'

key = SecureRandom.random_bytes(16)
iv = SecureRandom.random_bytes(16)

def padding_valid?(ct:, key:, iv:)
  begin
    CryptCBC.new(key: key, iv: iv).decrypt(ct)
  rescue
    false
  else
    true
  end
end

def crack_padding_oracle(ct:, iv:, block_size:, &block)
  # Crack the first block
  first_block = ct.slice(0, block_size)
  cracked_bytes = []
  (0..15).reverse_each do |byte_idx|
    expected_padding = block_size - byte_idx
    padding = cracked_bytes.zip(iv.bytes.last(cracked_bytes.size)).map do |cracked_byte, block_byte|
      cracked_byte ^ expected_padding ^ block_byte
    end
    guesses = (0..255).select do |guess|
      first_block[first_block.size - block_size * 2...first_block.size - block_size] = iv.slice(0, byte_idx) + [guess].pack('C*') + padding.pack('C*')
      yield(first_block, iv)
    end

    # The existing byte may incidentally result in valid padding;
    # only select it if there isn't an alternative
    if guesses.size > 1
      guesses.delete(iv.unpack('C*')[byte_idx])
    end

    cracked_bytes = [guesses.first ^ expected_padding ^ iv.unpack('C*')[byte_idx]] + cracked_bytes
  end

  block_count = ct.size / block_size
  cracked_bytes.pack('C*') + (2..block_count).map do |block_idx|
    ct.slice(0, block_idx * block_size)
  end.map do |ct_slice|
    cracked_bytes = []
    second_last_block = ct_slice.slice(ct_slice.size - block_size * 2, block_size)
    (0..15).reverse_each do |byte_idx|
      expected_padding = block_size - byte_idx
      padding = cracked_bytes.zip(second_last_block.bytes.last(cracked_bytes.size)).map do |cracked_byte, block_byte|
        cracked_byte ^ expected_padding ^ block_byte
      end
      guesses = (0..255).select do |guess|
        ct_slice[ct_slice.size - block_size * 2...ct_slice.size - block_size] = second_last_block.slice(0, byte_idx) + [guess].pack('C*') + padding.pack('C*')
        yield(ct_slice, iv)
      end

      # The existing byte may incidentally result in valid padding;
      # only select it if there isn't an alternative
      if guesses.size > 1
        guesses.delete(second_last_block[byte_idx].unpack('C*').first)
      end

      cracked_bytes = [guesses.first ^ expected_padding ^ second_last_block.unpack('C*')[byte_idx]] + cracked_bytes
    end
    cracked_bytes.pack('C*')
  end.join('')
end

[
  "MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc=",
  "MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic=",
  "MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw==",
  "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg==",
  "MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl",
  "MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA==",
  "MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw==",
  "MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8=",
  "MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g=",
  "MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93",
].each do |unknown|
  ct = CryptCBC.new(key: key, iv: iv).encrypt(decode_base64(unknown))
  cracked = crack_padding_oracle(ct: ct, iv: iv, block_size: 16) do |ct, iv|
    padding_valid?(ct: ct, key: key, iv: iv)
  end
  p cracked
end
