require 'pry-byebug'

require_relative 'challenge_21.rb'

mt = MT19937.new(seed: Random.new(Time.now.to_i).rand(2**32 - 1))

rands = []
624.times do
  rands.push(mt.rand)
end

def untemper_right_shift(y, shift_length, mask)
  w = 32
  max_word = 2**w - 1
  iterations = (w / shift_length) + (w % shift_length > 0 ? 1 : 0)
  untempered = 0
  iterations.times do |iteration|
    next_mask = max_word >> (iteration * shift_length)
    next_mask ^= max_word >> ((iteration + 1) * shift_length)
    untempered |= (y ^ ((untempered >> shift_length) & mask)) & next_mask
  end
  untempered
end

def untemper_left_shift(y, shift_length, mask)
  w = 32
  max_word = 2**w - 1
  iterations = (w / shift_length) + (w % shift_length > 0 ? 1 : 0)
  untempered = 0
  iterations.times do |iteration|
    next_mask = (max_word << (iteration * shift_length)) & max_word
    next_mask ^= (max_word << ((iteration + 1) * shift_length)) & max_word
    untempered |= (y ^ ((untempered << shift_length) & mask)) & next_mask
  end
  untempered
end

def untemper(z)
  u = 11
  d = 'FFFFFFFF'.to_i(16)
  s = 7
  b = '9D2C5680'.to_i(16)
  t = 15
  c = 'EFC60000'.to_i(16)
  l = 18

  tmp = untemper_right_shift(z, l, 'ffffffff'.to_i(16))
  tmp = untemper_left_shift(tmp, t, c)
  tmp = untemper_left_shift(tmp, s, b)
  untemper_right_shift(tmp, u, d)
end

x = rands.map do |rand|
  untemper(rand)
end

cloned_mt = MT19937.new(x: x)

10000.times do |iteration|
  rand = mt.rand
  cloned_rand = cloned_mt.rand
  raise "RNG returned unexpected value: #{cloned_rand} vs #{rand} (expected). Iteration: #{iteration}" unless rand == cloned_rand
end
