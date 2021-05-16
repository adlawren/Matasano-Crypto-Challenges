(1..32).each do |v|
  load "challenge_#{v}.rb" unless [4,6,17,20,22,24,31,32].include?(v) # Omit challenges which take a long time
end
