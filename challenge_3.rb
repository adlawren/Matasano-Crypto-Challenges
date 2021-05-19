require 'pry-byebug'

require_relative 'challenge_1.rb'
require_relative 'challenge_2.rb'

def english_monogram_frequency_score(s)
  monogram_frequencies = {
    a: 0.08167,
    b: 0.01492,
    c: 0.02782,
    d: 0.04253,
    e: 0.12702,

    f: 0.02228,
    g: 0.02015,
    h: 0.06094,
    i: 0.06966,
    j: 0.00153,

    k: 0.00772,
    l: 0.04025,
    m: 0.02406,
    n: 0.06749,
    o: 0.07507,

    p: 0.01929,
    q: 0.00095,
    r: 0.05987,
    s: 0.06327,
    t: 0.09056,

    u: 0.02758,
    v: 0.00978,
    w: 0.02360,
    x: 0.00150,
    y: 0.01974,

    z: 0.00074,
  }
  expected_frequencies = Hash.new(0.0)
  expected_frequencies.merge!(monogram_frequencies)

  alphabetic_count = s.each_char.filter do |c|
    c.match?(/[A-Za-z]/)
  end.size

  frequencies = Hash.new(0.0)
  s.downcase.each_char do |c|
    if c.match?(/[A-Za-z]/)
      frequencies[c.to_sym] += 1.0 / alphabetic_count
    elsif c.match?(/[!"'\.:;,\?]/)
      frequencies[c.to_sym] += 0.5 / s.size
    elsif c != ' ' # Don't penalize spaces, b/c they're really common
      frequencies[c.to_sym] += 1.0 / s.size
    end
  end

  1 / frequencies.map do |c, f|
    f - expected_frequencies[c]
  end.map(&:abs).reduce(&:+)
end

def crack_single_char_xor_key(s)
  (0..255).map do |guess|
    key = Array.new(s.size, guess).pack('C*')
    cracked = xor(s, key)
    score = english_monogram_frequency_score(cracked)
    {
      key: key,
      score: score,
    }
  end.sort_by { |h| h[:score] }.reverse.first[:key]
end

ciphertext = decode_hex('1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736')
cracked_key = crack_single_char_xor_key(ciphertext)
p xor(ciphertext, cracked_key)
