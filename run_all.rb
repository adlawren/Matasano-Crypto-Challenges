# Omit challenges which take a long time
(1..20).each { |v| load "challenge_#{v}.rb" unless [4,6,17].include?(v) }
