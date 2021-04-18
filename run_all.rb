# Omit challenges 4 & 6 b/c they take a long time
(1..14).each { |v| load "challenge_#{v}.rb" unless [4,6].include?(v) }
