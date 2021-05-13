require 'pry-byebug'

key = SecureRandom.alphanumeric(128)
text = SignedMessage.new("comment1=cooking%20MCs;userdata=foo;comment2=%20like%20a%20pound%20of%20bacon", key).to_s

message = text.match(/^message: .+, signature: /).to_s.delete_prefix('message: ').delete_suffix(', signature: ')
signature = text.match(/, signature: .+$/).to_s.delete_prefix(', signature: ')

def sha1_registers(digest)
  max_32bit = 2**32 - 1
  digest_i = digest.to_i(16)
  {
    h0: (digest_i >> 128) & max_32bit,
    h1: (digest_i >> 96) & max_32bit,
    h2: (digest_i >> 64) & max_32bit,
    h3: (digest_i >> 32) & max_32bit,
    h4: digest_i & max_32bit,
  }
end

sha1 = SHA1.new(sha1_registers(signature))

new_signed_message = nil
(0..200).each do |key_size_guess|
  additional_text = ";admin=true"
  padding = sha1.padding(message, offset: key_size_guess)
  new_message = message + padding + additional_text
  new_padding = sha1.padding(additional_text, data_size: additional_text.size + message.size + padding.size + key_size_guess)
  new_signature = sha1.digest(additional_text, padding: new_padding)
  new_text = "message: #{new_message}, signature: #{new_signature}"

  begin
    new_signed_message = SignedMessage.from_s(new_text, key)
  rescue
    next
  else
    break
  end
end

raise "Unable to forge signature" if new_signed_message.nil?

p "Forged message: #{new_signed_message.to_s}"
