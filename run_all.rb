# Omit challenges which take a long time
(1..23).each { |v| load "challenge_#{v}.rb" unless [4,6,17,20,22].include?(v) }
