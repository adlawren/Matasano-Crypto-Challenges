def add_padding(text, block_size = 16)
  padding_value = block_size - (text.size % block_size)
  text + Array.new(padding_value, padding_value).pack('C*')
end

def valid_padding?(text)
  padding_value = text[-1].bytes.first
  text.bytes.last(padding_value).any? do |padding_byte|
    padding_byte != padding_value
  end
end

def remove_padding(text)
  if valid_padding?(text)
    raise 'Invalid padding'
  end
  padding_value = text[-1].bytes.first
  text[0...text.size - padding_value]
end

p add_padding('YELLOW SUBMARINE', 20)
p remove_padding(add_padding('YELLOW SUBMARINE', 20))
