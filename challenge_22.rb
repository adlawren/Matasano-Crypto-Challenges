require 'pry-byebug'

seconds = Random.new(Time.now.to_i).rand(15)

mt = MT19937.new(Time.now.to_i + seconds)
rand = mt.rand

sleep(15)

cracked_seed = nil
30.times do |index|
  seed = Time.now.to_i - index
  mt2 = MT19937.new(seed)
  if mt2.rand == rand
    cracked_seed = seed
    break
  end
end

p "Cracked seed: #{cracked_seed}"
