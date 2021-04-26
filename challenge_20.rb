require 'pry-byebug'

crypt = CryptCTR.new(nonce: Array.new(8, 0).pack('C*'))

ciphertexts = File.readlines('20.txt').map do |unknown|
  crypt.encrypt(decode_base64(unknown.strip))
end

smallest_ciphertext_size = ciphertexts.map(&:size).min
ciphertext_columns = smallest_ciphertext_size.times.map { '' }
ciphertexts.map do |ciphertext|
  ciphertext.chars.take(smallest_ciphertext_size).each.with_index do |char, idx|
    ciphertext_columns[idx] += char
  end
end

cracked_keystream = ciphertext_columns.map do |ciphertext_column|
  crack_single_char_xor_key(ciphertext_column).chars.first
end.join('')

ciphertexts.each do |ciphertext|
  p xor(ciphertext.slice(0, cracked_keystream.size), cracked_keystream)
end

