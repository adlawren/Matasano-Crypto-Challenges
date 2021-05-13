require 'pry-byebug'

class SHA1
  # Source: https://en.wikipedia.org/wiki/SHA-1#SHA-1_pseudocode
  def initialize(
        h0: 0x67452301,
        h1: 0xEFCDAB89,
        h2: 0x98BADCFE,
        h3: 0x10325476,
        h4: 0xC3D2E1F0)
    @h0 = h0
    @h1 = h1
    @h2 = h2
    @h3 = h3
    @h4 = h4
  end

  def digest(data, padding: padding(data))
    h0 = @h0
    h1 = @h1
    h2 = @h2
    h3 = @h3
    h4 = @h4

    data_with_padding = data.to_s + padding
    data_with_padding.chars.each_slice(64) do |chunk|
      words = chunk.each_slice(4).map { |bytes| bytes.join('').unpack('L>').first }
      (16..79).each do |index|
        words.push(left_rotate_32bit(words[index - 3] ^ words[index - 8] ^ words[index - 14] ^ words[index - 16], 1))
      end

      a = h0
      b = h1
      c = h2
      d = h3
      e = h4

      (0..79).each do |index|
        if index < 20
          f = (b & c) | ((~b) & d)
          k = 0x5A827999
        elsif index > 19 && index < 40
          f = b ^ c ^ d
          k = 0x6ED9EBA1
        elsif index > 39 && index < 60
          f = (b & c) | (b & d) | (c & d)
          k = 0x8F1BBCDC
        elsif index > 59 && index < 80
          f = b ^ c ^ d
          k = 0xCA62C1D6
        end

        tmp = left_rotate_32bit(a, 5) + f + e + k + words[index]
        e = d
        d = c
        c = left_rotate_32bit(b, 30)
        b = a
        a = tmp & max_32bit
      end

      h0 = (h0 + a) & max_32bit
      h1 = (h1 + b) & max_32bit
      h2 = (h2 + c) & max_32bit
      h3 = (h3 + d) & max_32bit
      h4 = (h4 + e) & max_32bit
    end

    ((h0 << 128) | (h1 << 96) | (h2 << 64) | (h3 << 32) | h4).to_s(16).rjust(40, '0')
  end

  def padding(data, offset: 0, data_size: data.to_s.size)
    str = data.to_s + [0x80].pack('C*')
    size = str.size + offset
    padding = if size % 64 > (64 - 8)
                Array.new((64 - (size % 64)) + (64 - 8), 0)
              else
                Array.new((64 - 8) - (size % 64), 0)
              end.pack('C*') + [(data_size + offset) * 8].pack('Q>')
    [0x80].pack('C*') + padding
  end

  # Source: https://stackoverflow.com/questions/10134805/bitwise-rotate-left-function/10134877#10134877
  private def left_rotate_32bit(val, shift)
    ((val << shift) & max_32bit) | (val >> (32 - shift))
  end

  private def max_32bit
    2**32 - 1
  end
end

s = SHA1.new
1000.times do |size|
  text = '0' * size
  hash = s.digest(text)
  expected_hash = OpenSSL::Digest::SHA1.new(text).to_s
  if hash != expected_hash
    raise "SHA1 digest is not returning the expected hash: #{hash} vs #{expected_hash} (expected), input size: #{size}"
  end
end

class SignedMessage
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
    SHA1.new.digest(@key + @message)
  end
end

key = SecureRandom.alphanumeric(128)
signed_message = SignedMessage.new('abc', key)

p SignedMessage.from_s(signed_message.to_s, key).to_s
