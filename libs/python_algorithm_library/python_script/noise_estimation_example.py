import PythonAlgorithmLibrary as pal
import numpy as np
import matplotlib.pyplot as plt

c = pal.NoiseEstimation().getCoefficients()
c['nChannels'] = 1
ne = pal.NoiseEstimation(c)
print(ne)

nFrames = 1000
x = np.random.rand(c['nBands'], nFrames)
x[:,300:400] = x[:,300:400] * .1

x[:,800:900] = x[:,800:900] * .1
x = x * x # power
y = np.zeros((c['nBands'], nFrames))

for i in np.arange(nFrames):
    z = ne.process(x[:,i])
    y[:,i] = z[:,0]

plt.subplot(2,1,1)
plt.imshow(10*np.log10(x),aspect='auto', vmin = -40, vmax = 0)
plt.colorbar()
plt.subplot(2,1,2)
plt.imshow(10*np.log10(y),aspect='auto', vmin = -40, vmax = 0)
plt.colorbar()
plt.show()
