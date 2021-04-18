require 'pry-byebug'

def strip_padding(pt)
  padding_size = pt.chars.last.ord
  raise 'Padding invalid' unless pt.bytes.last(padding_size).all? do |byte|
    byte == padding_size
  end
  pt.chars.take(pt.size - padding_size).join('')
end

p strip_padding("ICE ICE BABY\x04\x04\x04\x04")
#p strip_padding("ICE ICE BABY\x05\x05\x05\x05")
#p strip_padding("ICE ICE BABY\x01\x02\x03\x04")
