require 'pry-byebug'

require_relative 'challenge_21.rb'

seconds = Random.new(Time.now.to_i).rand(15)

mt = MT19937.new(seed: Time.now.to_i + seconds)
rand = mt.rand

sleep(15)

cracked_seed = nil
current_time_seconds = Time.now.to_i
30.times do |index|
  seed = current_time_seconds - index
  mt2 = MT19937.new(seed: seed)
  if mt2.rand == rand
    cracked_seed = seed
    break
  end
end

p "Cracked seed: #{cracked_seed}"
