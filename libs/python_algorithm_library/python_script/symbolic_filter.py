from sympy import *
from sympy import together
init_printing()

# This is an experimental 2nd order filter. The idea is to allow any transfer function (unlike state-variable filters where the denominator is fixed) and also allow the coefficients to be change in real-time without artifacts.
#
# author: Kristian Timm Andersen


a,b,c,d,h,m,p,q,z,x,y = symbols('a,b,c,d,h,m,p,q,z,x,y',real=True)

# equations that describe the filter topology
eq0 = b*x + c*y -p*h - d # d is delay unit in 1st order transposed filter with x as input and y as output. the term p*h is feedback path from next 1st order filter
eq1 = a*x + d*z - y # y is output from 1st order transposed filter with x as input and d as delay unit
eq2 = y + m - h # h is delay unit in 1st order transposed filter with y as input and m as output. The filter has no coefficients
eq3 = y + h*z +q*x - m # m is output from 1st order transposed filter with y as input and h as delay unit. The term q*x is feedforward path from before prevous 1st order filter

# solve set of equations and print equation for output (m) as a function of input (x)
sol = solve([eq0,eq1,eq2,eq3],[d,y,h,m])
print("Transfer function of output: \n", sol[m])

# factorize and split into numerator and denominator
Num1, Den1 = fraction(factor(sol[m]))

# perform simplification
Num1 = simplify(collect(expand(Num1),z))
Den1 = simplify(collect(expand(Den1),z))

Num1 = Num1.args[1] # get rid of x

# print resulting numerator and denominator
print("Numerator of transfer function (m/x): \n", Num1)
print("Denominator of transfer function (m/x): \n", Den1)

# print coefficient for each term as a factor of z
print("Coefficients of transfer function (aa + bb*z + cc*z**2)/(dd + ee*z + ff*z**2):")
print("aa: ", Num1.coeff(z,0))
print("bb: ", Num1.coeff(z,1))
print("cc: ", Num1.coeff(z,2))
print("dd: ", Den1.coeff(z,0))
print("ee: ", Den1.coeff(z,1))
print("ff: ", Den1.coeff(z,2))

# find expressions for calculating the filter coefficients in new filter topology as a function of standard filter topology
aa,bb,cc,dd,ee,ff = symbols('aa,bb,cc,dd,ee,ff',real=True)
eq10 = Num1.coeff(z,0) - aa
eq11 = Num1.coeff(z,1) - bb
eq12 = Num1.coeff(z,2) - cc
eq13 = Den1.coeff(z,0) - dd
eq14 = Den1.coeff(z,1) - ee
eq15 = Den1.coeff(z,2) - ff

solsol = solve([eq10,eq11,eq12,eq13,eq14,eq15],[a,b,c,p,q])
print("Coefficients in filter topology as a function of coefficients in standard filter topology:")
print("a: ", solsol[0][0])
print("b: ", solsol[0][1])
print("c: ", solsol[0][2])
print("p: ", solsol[0][3])
print("q: ", solsol[0][4])
