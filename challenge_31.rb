require 'httparty'
require 'pry-byebug'

file = 'foo'

def signature_valid?(file, signature)
  HTTParty.get("http://localhost:4567/test?file=foo&signature=#{signature}").parsed_response == "Signature valid"
end

signature = ''
(0..39).each do
  next_char = nil
  longest_delay = Float::MIN
  (0..15).each do |guess|
    char = guess.to_s(16)

    start = Time.now
    HTTParty.get("http://localhost:4567/test?file=foo&signature=#{signature + char}")
    delay = Time.now - start

    if longest_delay < delay
      longest_delay = delay
      next_char = char
    end
  end

  signature += next_char
end

unless signature_valid?(file, signature)
  raise 'Unable to crack signature'
end
