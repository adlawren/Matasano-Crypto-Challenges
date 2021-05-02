require 'pry-byebug'

class MT19937
  def initialize(seed = Time.now.to_i)
    @w = 32
    @n = 624
    @m = 397
    @r = 31
    @a = '9908B0DF'.to_i(16)
    @u = 11
    @d = 'FFFFFFFF'.to_i(16)
    @s = 7
    @b = '9D2C5680'.to_i(16)
    @t = 15
    @c = 'EFC60000'.to_i(16)
    @l = 18
    @f = 1812433253

    @max_word = 2**@w - 1
    @lower_bitmask = @max_word >> (@w - @r)
    # Need to & w/ max_word b/c this isn't a 32 bit value, it needs to be
    # converted into one
    @upper_bitmask = (@max_word << @r) & @max_word
    @k = 0

    @x = [seed]
    (@n - 1).times do |index|
      i = index + 1

      # These intermediate values may also be larger than the max_word,
      # so they need to be and-ed w/ max_word
      tmp = @f * (@x[i - 1] ^ (@x[i - 1] >> (@w - 2)))
      tmp &= @max_word
      tmp += i
      tmp &= @max_word

      @x.push(tmp)
    end
  end

  def twist_transform(x)
    if (x & 1) == 1
      (x >> 1) ^ @a
    else
      x >> 1
    end
  end

  def rand
    @x.push(@x[@k + @m] ^ twist_transform((@x[@k] & @upper_bitmask) | (@x[@k + 1] & @lower_bitmask)))

    x = @x[@k + @n]
    y = x ^ ((x >> @u) & @d)
    y = y ^ ((y << @s) & @b)
    y = y ^ ((y << @t) & @c)
    z = y ^ (y >> @l)

    @k += 1

    return z
  end
end

(0..10).each do |seed|
  mt19937 = MT19937.new(seed)
  r = Random.new(seed)
  1000.times do |iteration|
    rand = mt19937.rand
    expected = r.rand(2**32 - 1)
    if rand != expected
      raise "RNG returned unexpected value: #{rand} vs #{expected} (expected). Seed: #{seed}, iteration: #{iteration}"
    end
  end
end
