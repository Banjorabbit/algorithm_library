from sympy import *

c0,c1,c2,c3,h,l,b,x,x1,x2,d1,d2,z, bstop, gain, peak, lshelf, hshelf = symbols('c0 c1 c2 c3 h l b x x1 x2 d1 d2 z bstop gain peak lshelf hshelf', real=True)

eq0 = c0*(x - d2 * z - c3 * d1 * z) - h
eq1 = c1 * h + d1 * z - b
eq2 = c2 * b + d2 * z - l
eq3 = c1 * h + b - d1
eq4 = c2 * b + l - d2

eq5 = x - b - bstop
eq6 = x + (gain-1)*b - peak
eq7 = gain * (b + h) + l - lshelf
eq8 = gain * (b + l) + h - hshelf

sol = solve([eq0,eq1,eq2,eq3,eq4,eq5,eq6,eq7,eq8],[h,l,b,d1,d2,bstop,peak,lshelf,hshelf])
print(sol)
print("solution for l: ",sol[l])
Num,Den = fraction(factor(sol[l]))
Num1 = simplify(collect(expand(Num),z))
Den1 = simplify(collect(expand(Den),z))

# print resulting numerator and denominator
print("Numerator of transfer function (m/x): \n", Num1)
print("Denominator of transfer function (m/x): \n", Den1)


print("solution for b: ",sol[b])
Num,Den = fraction(factor(sol[b]))
Num2 = simplify(collect(expand(Num),z))
Den2 = simplify(collect(expand(Den),z))

# print resulting numerator and denominator
print("Numerator of transfer function (m/x): \n", Num2)
print("Denominator of transfer function (m/x): \n", Den2)

print("solution for h: ",sol[h])
Num,Den = fraction(factor(sol[h]))
Num3 = simplify(collect(expand(Num),z))
Den3 = simplify(collect(expand(Den),z))

# print resulting numerator and denominator
print("Numerator of transfer function (m/x): \n", Num3)
print("Denominator of transfer function (m/x): \n", Den3)


print("solution for bstop: ",sol[bstop])
Num,Den = fraction(factor(sol[bstop]))
Num4 = simplify(collect(expand(Num),z))
Den4 = simplify(collect(expand(Den),z))

# print resulting numerator and denominator
print("Numerator of transfer function (m/x): \n", Num4)
print("Denominator of transfer function (m/x): \n", Den4)

print("solution for peak: ",sol[peak])
Num,Den = fraction(factor(sol[peak]))
Num5 = simplify(collect(expand(Num),z))
Den5 = simplify(collect(expand(Den),z))

# print resulting numerator and denominator
print("Numerator of transfer function (m/x): \n", Num5)
print("Denominator of transfer function (m/x): \n", Den5)

print("solution for lshelf: ",sol[lshelf])
Num,Den = fraction(factor(sol[lshelf]))
Num6 = simplify(collect(expand(Num),z))
Den6 = simplify(collect(expand(Den),z))

# print resulting numerator and denominator
print("Numerator of transfer function (m/x): \n", Num6)
print("Denominator of transfer function (m/x): \n", Den6)

print("solution for hshelf: ",sol[hshelf])
Num,Den = fraction(factor(sol[hshelf]))
Num7 = simplify(collect(expand(Num),z))
Den7 = simplify(collect(expand(Den),z))

# print resulting numerator and denominator
print("Numerator of transfer function (m/x): \n", Num7)
print("Denominator of transfer function (m/x): \n", Den7)

# find cutoff, resonance, cLP, cBP, cHP from second-order-section representation
cutoff, resonance, c0, c1, c2, c3 = symbols('cutoff, resonance, c0, c1, c2, c3', real = True)

eq10 = cutoff / resonance - c1
eq11 = cutoff * resonance - c2
eq12 = c2 + 1 - c3
eq13 = 1 / (1+c1*(c2+1)) - c0

cLP, cBP, cHP, b0, b1, b2, a1, a2 = symbols('cLP, cBP, cHP, b0, b1, b2, a1, a2', real=True)

eq15 = cLP*c0*c1*c2 + cBP*c0*c1 + cHP*c0 - b0
eq16 = cLP*2*c0*c1*c2 -2*cHP*c0 - b1
eq17 = cLP*c0*c1*c2 - cBP*c0*c1 + cHP*c0 - b2

eq18 = 2*(c0*c1*c2 + c0*c1*(c2+1) - 1) - a1
eq19 = (2*c0*c1*c2 - 2*c0*c1*(c2+1) + 1) - a2
 
solc = solve([eq10,eq11,eq13,eq15,eq16,eq17,eq18,eq19],[c0,c1,c2,cLP,cBP,cHP,cutoff,resonance])
print(solc)

