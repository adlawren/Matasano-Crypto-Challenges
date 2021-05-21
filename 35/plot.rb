require 'unicode_plot'

require_relative '../challenge_33.rb'

# Can't use this in the plot; too big
p_large = 0xffffffffffffffffc90fdaa22168c234c4c6628b80dc1cd129024e088a67cc74020bbea63b139b22514a08798e3404ddef9519b3cd3a431b302b0a6df25f14374fe1356d6d51c245e485b576625e7ec6f44c42e9a637ed6b0bff5cb6f406b7edee386bfb5a899fa5ae9f24117c4b1fe649286651ece45b3dc2007cb8a163bf0598da48361c55d39a69163fa8fd24cf5f83655d23dca3ad961c62f356208552bb9ed529077096966d670c354e4abc9804f1746c08ca237327ffffffffffffffff
p = 27
g = 2

x = 50.times.to_a
modexp = x.map { |x_pos| modexp(base: g, exp: x_pos, mod: p) }
modexp_mitm = x.map { |x_pos| modexp(base: p - 1, exp: x_pos, mod: p) }

UnicodePlot.lineplot(x, modexp, name: "modexp(g, x, p)", width: 40, height: 10).render

# Much less curvature
UnicodePlot.lineplot(x, modexp_mitm, name: "modexp(p - 1, x, p)").render

unique_modexp = modexp_mitm.uniq
p "#{unique_modexp.size} unique modexp values:" # Only 2 unique values: 1, p - 1
# Explanation:
# Example: p = 7, modexp(6, x, 7)
# Note: a*b % m = [(a % m) * (b % m)] % m
# x = 1: 6 % 7 = 6
# x = 2: [(6 % 7) * 6] % 7 = 36 % 7 = 1
# x = 3: [(6 % 7) * 1] % 7 = 6
# x = 4: [(6 % 7) * 6] % 7 = 36 % 7 = 1
# Observation: (p - 1) ^ 2 % p = 1, why?
# (p - 1) ^ 2 = p^2 - 2p + 1
# p^2 is a multiple of p, 2p is a multiple of p
# (multiple of p - multiple of p + 1) % p = 1
unique_modexp.map { |modexp| p modexp }

modexp_mitm2 = x.map do |x_pos|
  modexp(base: p_large - 1, exp: x_pos, mod: p_large)
end

unique_modexp2 = modexp_mitm2.uniq
p "#{unique_modexp2.size} unique modexp values:"
unique_modexp2.map { |modexp| p modexp }
