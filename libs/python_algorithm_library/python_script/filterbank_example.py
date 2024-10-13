import PythonAlgorithmLibrary as pal
import numpy as np
import matplotlib.pyplot as plt

filterbankAnalysis = pal.FilterbankAnalysis()
filterbankSynthesis = pal.FilterbankSynthesis()

cA = filterbankAnalysis.getCoefficients()
bufferSize = cA['bufferSize']
cA['nChannels'] = 1
filterbankAnalysis.setCoefficients(cA)

cS = filterbankSynthesis.getCoefficients()
cS['nChannels'] = 1
filterbankSynthesis.setCoefficients(cS)

nFrames = 30
x = np.zeros(nFrames * bufferSize)
x[225] = 1

print("Standard Filterbank: ")
print(filterbankAnalysis)
print(filterbankSynthesis)

zStandard = np.zeros(nFrames * bufferSize)
for i in np.arange(nFrames):
    y = filterbankAnalysis.process(x[i*bufferSize:(i+1)*bufferSize])
    y[25] = 0
    zStandard[i*bufferSize:(i+1)*bufferSize] = filterbankSynthesis.process(y)[:,0]


cA['filterbankType'] = 'Wola'
cS['filterbankType'] = 'Wola'

filterbankAnalysis.setCoefficients(cA)
filterbankSynthesis.setCoefficients(cS)
print("High Quality Filterbank: ")
print(filterbankAnalysis)
print(filterbankSynthesis)

zHQ = np.zeros(nFrames * bufferSize)
for i in np.arange(nFrames):
    y = filterbankAnalysis.process(x[i*bufferSize:(i+1)*bufferSize])
    y[25] = 0
    zHQ[i*bufferSize:(i+1)*bufferSize] = filterbankSynthesis.process(y)[:,0]


plt.style.use('dark_background')
plt.subplot(2,1,1)
plt.plot(x)
plt.plot(zStandard)
plt.plot(zHQ)
plt.xlim(0, 1500)
plt.xlabel('samples')
plt.title('Impulse reponse')
plt.grid(linestyle='--',linewidth=1)

freq = np.linspace(0, 1, x.size)
plt.subplot(2,1,2)
plt.plot(freq,20*np.log(abs(np.fft.fft(x))))
plt.plot(freq,20*np.log(np.maximum(abs(np.fft.fft(zStandard)),1e-8)))
plt.plot(freq,20*np.log(np.maximum(abs(np.fft.fft(zHQ)),1e-8)))
plt.xlim(0, 0.2)
plt.xlabel('Normalized frequency')
plt.ylabel('power (dB)')
plt.title('Frequency domain of impulse reponse')
plt.grid(linestyle='--',linewidth=1)

plt.show()