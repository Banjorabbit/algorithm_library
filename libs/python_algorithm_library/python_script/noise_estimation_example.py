import PythonAlgorithmLibrary as pal
import numpy as np
import matplotlib.pyplot as plt

c = pal.NoiseEstimation().getCoefficients()
c['nChannels'] = 1
ne = pal.NoiseEstimation(c)
print(ne)

nFrames = 10000
x = np.random.rand(c['nBands'], nFrames)
x[:,3000:4000] = x[:,3000:4000] * .1

x[:,8000:9000] = x[:,8000:9000] * .1
y = np.zeros((c['nBands'], nFrames))
activity = np.zeros((c['nBands'], nFrames))

for i in np.arange(nFrames):
    z = ne.process(x[:,i])
    y[:,i] = z[0][:,0]
    activity[:,i] = z[1][:,0]

plt.subplot(2,1,1)
plt.imshow(x,aspect='auto')
plt.subplot(2,1,2)
plt.imshow(y,aspect='auto')
plt.show()
