from timeit import default_timer as timer
import PythonAlgorithmLibrary as pal
import numpy as np
import matplotlib.pyplot as plt

spec = pal.Spectrogram()
x = np.random.rand(128)

start = timer()
y = spec.process(x)
end = timer()

#print(y)
print(spec.validInput(x))
print(y.size)
print(end-start)


cFilter = pal.FilterMinMax().getCoefficients()
cFilter['filterLength'] = 3
cFilter['nChannels'] = 2
filter = pal.FilterMinMax(cFilter)
sFilter = filter.getSetup()
print(cFilter)
print(sFilter)
print(filter)
x = np.array([[1,2,3,4,5,6,7,8,9,10],[1,2,3,4,5,6,7,8,9,10]]).transpose()
print(x.shape)
print(filter.validInput(x))

start = timer()
(ymin,ymax) = filter.process(x)
end = timer()

print(ymin.transpose())
print(ymax.transpose())
print(end-start)

spline = pal.Spline()
xGiven = np.arange(0, 10)
yGiven = np.random.rand(10)
xDesired = np.arange(0,9, 0.1)

print(xGiven.shape)
print(yGiven.shape)
print(xDesired.shape)
print(spline.validInput(xGiven,yGiven,xDesired))

y = spline.process(xGiven,yGiven,xDesired)

plt.plot(xGiven,yGiven)
plt.plot(xDesired,y)
plt.show()
