def add_padding(text, block_size = 16)
  padding_value = block_size - (text.size % block_size)
  text + Array.new(padding_value, padding_value).pack('C*')
end

def valid_padding?(text)
  padding_value = text.bytes.last
  text.bytes.last(padding_value).all? do |padding_byte|
    padding_byte == padding_value
  end
end

def remove_padding(text)
  unless valid_padding?(text)
    raise 'Invalid padding'
  end
  padding_size = text.bytes.last
  text.slice(0, text.size - padding_size)
end

p add_padding('YELLOW SUBMARINE', 20)
p remove_padding(add_padding('YELLOW SUBMARINE', 20))
