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

    # Note: it's possible that a smaller number of trials will still be effective here
    # This takes a long time at the moment
    # An alternative could be to use a smaller number of trials and guess 2 characters at a time
    average_delay = 100.times.map do
      start = Time.now
      HTTParty.get("http://localhost:4567/test?file=foo&signature=#{signature + char}")
      Time.now - start
    end.reduce(&:+) / 100

    if longest_delay < average_delay
      longest_delay = average_delay
      next_char = char
    end
  end

  signature += next_char
end

unless signature_valid?(file, signature)
  raise 'Unable to crack signature'
end
