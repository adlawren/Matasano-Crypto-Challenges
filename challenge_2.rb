def xor(s1, s2)
  s1.bytes.zip(s2.bytes).map do |b1, b2|
    b1 ^ b2
  end.pack('C*')
end

p xor(['1c0111001f010100061a024b53535009181c'].pack('H*'),
      ['686974207468652062756c6c277320657965'].pack('H*')).unpack('H*').first
