require 'pry-byebug'

class MD4
  # Source: https://datatracker.ietf.org/doc/html/rfc1186#section-3
  def initialize(a: 0x67452301, b: 0xefcdab89, c: 0x98badcfe, d: 0x10325476)
    @a = a
    @b = b
    @c = c
    @d = d
  end

  def digest(data, padding: padding(data))
    a = @a
    b = @b
    c = @c
    d = @d

    data_with_padding = data + padding
    data_with_padding.chars.each_slice(64) do |block|
      x = block.each_slice(4).map { |bytes| bytes.join('').unpack('L').first }

      prev_a = a
      prev_b = b
      prev_c = c
      prev_d = d

      # Round 1
      a = ff(a, b, c, d, 0, 3, x)
      d = ff(d, a, b, c, 1, 7, x)
      c = ff(c, d, a, b, 2, 11, x)
      b = ff(b, c, d, a, 3, 19, x)
      a = ff(a, b, c, d, 4, 3, x)
      d = ff(d, a, b, c, 5, 7, x)
      c = ff(c, d, a, b, 6, 11, x)
      b = ff(b, c, d, a, 7, 19, x)
      a = ff(a, b, c, d, 8, 3, x)
      d = ff(d, a, b, c, 9, 7, x)
      c = ff(c, d, a, b, 10, 11, x)
      b = ff(b, c, d, a, 11, 19, x)
      a = ff(a, b, c, d, 12, 3, x)
      d = ff(d, a, b, c, 13, 7, x)
      c = ff(c, d, a, b, 14, 11, x)
      b = ff(b, c, d, a, 15, 19, x)

      # Round 2
      a = gg(a, b, c, d, 0, 3, x)
      d = gg(d, a, b, c, 4, 5, x)
      c = gg(c, d, a, b, 8, 9, x)
      b = gg(b, c, d, a, 12, 13, x)
      a = gg(a, b, c, d, 1, 3, x)
      d = gg(d, a, b, c, 5, 5, x)
      c = gg(c, d, a, b, 9, 9, x)
      b = gg(b, c, d, a, 13, 13, x)
      a = gg(a, b, c, d, 2, 3, x)
      d = gg(d, a, b, c, 6, 5, x)
      c = gg(c, d, a, b, 10, 9, x)
      b = gg(b, c, d, a, 14, 13, x)
      a = gg(a, b, c, d, 3, 3, x)
      d = gg(d, a, b, c, 7, 5, x)
      c = gg(c, d, a, b, 11, 9, x)
      b = gg(b, c, d, a, 15, 13, x)

      # Round 3
      a = hh(a, b, c, d, 0, 3, x)
      d = hh(d, a, b, c, 8, 9, x)
      c = hh(c, d, a, b, 4, 11, x)
      b = hh(b, c, d, a, 12, 15, x)
      a = hh(a, b, c, d, 2, 3, x)
      d = hh(d, a, b, c, 10, 9, x)
      c = hh(c, d, a, b, 6, 11, x)
      b = hh(b, c, d, a, 14, 15, x)
      a = hh(a, b, c, d, 1, 3, x)
      d = hh(d, a, b, c, 9, 9, x)
      c = hh(c, d, a, b, 5, 11, x)
      b = hh(b, c, d, a, 13, 15, x)
      a = hh(a, b, c, d, 3, 3, x)
      d = hh(d, a, b, c, 11, 9, x)
      c = hh(c, d, a, b, 7, 11, x)
      b = hh(b, c, d, a, 15, 15, x)

      a += prev_a
      a &= max_32bit
      b += prev_b
      b &= max_32bit
      c += prev_c
      c &= max_32bit
      d += prev_d
      d &= max_32bit
    end

    encode_hex([a].pack('L') + [b].pack('L') + [c].pack('L') + [d].pack('L'))
  end

  def padding(data, offset: 0, data_size: data.to_s.size)
    str = data.to_s + [0x80].pack('C*')
    size = str.size + offset
    padding = if size % 64 > (64 - 8)
                Array.new((64 - (size % 64)) + (64 - 8), 0)
              else
                Array.new((64 - 8) - (size % 64), 0)
              end.pack('C*') + [(data_size + offset) * 8].pack('Q')
    [0x80].pack('C*') + padding
  end

  private def f(x, y, z)
    (x & y) | (~x & z)
  end

  private def g(x, y, z)
    (x & y) | (x & z) | (y & z)
  end

  private def h(x, y, z)
    x ^ y ^ z
  end

  private def ff(a, b, c, d, i, s, x)
    tmp = a + f(b, c, d) + x[i]
    tmp &= max_32bit
    left_rotate_32bit(tmp, s)
  end

  private def gg(a, b, c, d, i, s, x)
    tmp = a + g(b, c, d) + x[i] + 0x5A827999
    tmp &= max_32bit
    left_rotate_32bit(tmp, s)
  end

  private def hh(a, b, c, d, i, s, x)
    tmp = a + h(b, c, d) + x[i] + 0x6ED9EBA1
    tmp &= max_32bit
    left_rotate_32bit(tmp, s)
  end

  # Source: https://stackoverflow.com/questions/10134805/bitwise-rotate-left-function/10134877#10134877
  private def left_rotate_32bit(val, shift)
    ((val << shift) & max_32bit) | (val >> (32 - shift))
  end

  private def max_32bit
    2**32 - 1
  end
end

m = MD4.new
1000.times do |size|
  text = '0' * size
  hash = m.digest(text)
  expected_hash = OpenSSL::Digest::MD4.new(text).to_s
  if hash != expected_hash
    raise "MD4 digest is not returning the expected hash: #{hash} vs #{expected_hash} (expected), input size: #{size}"
  end
end

class SignedMessage2
  def initialize(message, key = SecureRandom.alphanumeric(128))
    @message = message
    @key = key
  end

  def to_s
    "message: #{@message}, signature: #{signature}"
  end

  def self.from_s(str, key)
    message = str.match(/^message: .+, signature: /).to_s.delete_prefix('message: ').delete_suffix(', signature: ')
    signature = str.match(/, signature: .+$/).to_s.delete_prefix(', signature: ')
    signed_message = new(message, key)
    if signed_message.signature != signature
      raise "Invalid signature"
    end
    signed_message
  end

  def signature
    MD4.new.digest(@key + @message)
  end
end

key = SecureRandom.alphanumeric(128)
signed_message = SignedMessage2.new("comment1=cooking%20MCs;userdata=foo;comment2=%20like%20a%20pound%20of%20bacon", key)

p SignedMessage2.from_s(signed_message.to_s, key).to_s

text = signed_message.to_s

message = text.match(/^message: .+, signature: /).to_s.delete_prefix('message: ').delete_suffix(', signature: ')
signature = text.match(/, signature: .+$/).to_s.delete_prefix(', signature: ')

def md4_registers(digest)
  a, b, c, d = decode_hex(digest).chars.each_slice(4).map { |bytes| bytes.join('').unpack('L').first }
  { a: a, b: b, c: c, d: d }
end

md4 = MD4.new(md4_registers(signature))

new_signed_message = nil
(0..200).each do |key_size_guess|
  additional_text = ";admin=true"
  padding = md4.padding(message, offset: key_size_guess)
  new_message = message + padding + additional_text
  new_padding = md4.padding(additional_text, data_size: additional_text.size + message.size + padding.size + key_size_guess)
  new_signature = md4.digest(additional_text, padding: new_padding)
  new_text = "message: #{new_message}, signature: #{new_signature}"

  begin
    new_signed_message = SignedMessage2.from_s(new_text, key)
  rescue
    next
  else
    break
  end
end

raise "Unable to forge signature" if new_signed_message.nil?

p "Forged message: #{new_signed_message.to_s}"
