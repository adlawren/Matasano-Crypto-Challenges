def decode_hex(s)
  [s].pack('H*')
end

def encode_hex(s)
  s.unpack('H*').first
end

def decode_base64(s)
  s.unpack('m0').first
end

def encode_base64(s)
  [s].pack('m0')
end

p encode_base64(decode_hex('49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d'))
