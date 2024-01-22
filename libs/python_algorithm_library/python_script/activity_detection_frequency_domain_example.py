import PythonAlgorithmLibrary as pal
import numpy as np
import matplotlib.pyplot as plt

c = pal.NoiseEstimation().getCoefficients()
c['nChannels'] = 1
vad = pal.ActivityDetectionFrequencyDomain(c)
ne = pal.NoiseEstimation(c)
print(ne)

nFrames = 1000
x = np.random.rand(c['nBands'], nFrames)
x[:,300:400] = x[:,300:400] * .1

x[:,800:900] = x[:,800:900] * .1
x = x * x # power
y = np.zeros((c['nBands'], nFrames))
activity = np.zeros(nFrames)

for i in np.arange(nFrames):
    vact = vad.process(x[:,i])
    z = ne.process(x[:,i])
    y[:,i] = z[0][:,0]
    activity[i] = vact

plt.subplot(3,1,1)
plt.imshow(10*np.log10(x),aspect='auto', vmin = -40, vmax = 0)
plt.colorbar()
plt.subplot(3,1,2)
plt.imshow(10*np.log10(y),aspect='auto', vmin = -40, vmax = 0)
plt.colorbar()
plt.subplot(3,1,3)
plt.plot(activity)
plt.show()

