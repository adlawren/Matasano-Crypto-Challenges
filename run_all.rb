(1..25).each do |v|
  # Omit challenges which take a long time
  load "challenge_#{v}.rb" unless [4,6,17,20,22,24].include?(v)
end
