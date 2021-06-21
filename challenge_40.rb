require_relative 'challenge_39.rb'

# Newton's method, where f(x) = x^3 - n
# Initial guess is chosen s.t. it has similar magnitude to that of the root,
# on the basis that the bit length of the cube of a number is probably roughly 3x the bit length of the number
# Ex. (2^16)^3 = 2^16 * 2^16 * 2^16 = 2^48, 48 / 3 = 16
# From testing in the console, 6 iterations were insufficient, 7 were sufficient. Roughly doubling to 10 for safety
# This is needed b/c trying to raise the congruence to (1.0 / 3) returns Infinity, presumably b/c the number is too large. For example:
# [2] pry(main)> congruence
# => 189837433609418179357377570251545466627545277150445603779909176485267452159751753625583825821776318034766950192705777772777063324826759013601216710038832859041142200695376209519123883176627929985539575379375911781762337789595774814009161130694270997988012652243290023410194433525651644119899480587191463240636990621065672211666884651201533632336732525933098292879413808318869020979976889041698250086395391994541231364828391520805645440281650163112041477584424812744896467033903593616971636162460858847414851139156653805135538063045617848951119854526406344320960100431417686301989778406944610673245321850985274136165514564303197527221651602541918697598974977113217422877148441415428515132180787790563514294996088683213652244360811768743965578949792727767877278025603162633071469311186485360527532233017980259035725011839827139994401836143240748590765502604427887439948546104107642306942365843336319693787695342648566857978201
# [3] pry(main)> congruence**(1.0 / 3)
# => Infinity
# References:
# - https://math.stackexchange.com/questions/575281/how-to-calculate-square-root-or-cube-root/575295#575295
# - https://en.wikipedia.org/wiki/Cube_root#Numerical_methods
# - https://math.stackexchange.com/questions/787019/what-initial-guess-is-used-for-finding-n-th-root-using-newton-raphson-method/787041#787041
def cube_root(x, iterations: 20, guess: 2 ** (Math.log2(x).round / 3))
  root = guess
  iterations.times do
    root = ((x / (root ** 2)) + (2 * root)) / 3
    return root if (root ** 3) == x
  end
  root
end

rsa1 = RSA.new(e: 3)
rsa2 = RSA.new(e: 3)
rsa3 = RSA.new(e: 3)

100.times do
  m = Random.rand(2**1024)

  c1 = rsa1.encrypt(m)
  c2 = rsa2.encrypt(m)
  c3 = rsa3.encrypt(m)

  modulus = rsa1.n * rsa2.n * rsa3.n
  congruence =
    ((c1 * rsa2.n * rsa3.n * mod_inv(rsa2.n * rsa3.n, rsa1.n)) +
     (c2 * rsa1.n * rsa3.n * mod_inv(rsa1.n * rsa3.n, rsa2.n)) +
     (c3 * rsa1.n * rsa2.n * mod_inv(rsa1.n * rsa2.n, rsa3.n))) % modulus

  cracked_m = cube_root(congruence)

  unless cracked_m == m
    raise 'Failed to crack message'
  end
end
