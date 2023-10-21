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
cS['gain'] = 2/3
filterbankSynthesis.setCoefficients(cS)

nFrames = 30
x = np.zeros(nFrames * bufferSize)
x[225] = 1

filterbankAnalysis.setStandardFilterbank(bufferSize)
filterbankSynthesis.setStandardFilterbank(bufferSize)
print("Standard Filterbank: ")
print(filterbankAnalysis)
print(filterbankSynthesis)

zStandard = np.zeros(nFrames * bufferSize)
for i in np.arange(nFrames):
    y = filterbankAnalysis.process(x[i*bufferSize:(i+1)*bufferSize])
    y[25] = 0
    zStandard[i*bufferSize:(i+1)*bufferSize] = filterbankSynthesis.process(y)[:,0]


filterbankAnalysis.setHighQualityFilterbank(bufferSize)
filterbankSynthesis.setHighQualityFilterbank(bufferSize)
print("High Quality Filterbank: ")
print(filterbankAnalysis)
print(filterbankSynthesis)

zHQ = np.zeros(nFrames * bufferSize)
for i in np.arange(nFrames):
    y = filterbankAnalysis.process(x[i*bufferSize:(i+1)*bufferSize])
    y[25] = 0
    zHQ[i*bufferSize:(i+1)*bufferSize] = filterbankSynthesis.process(y)[:,0]


bufferSize = int(bufferSize/4)
filterbankAnalysis.setLowDelayFilterbank(bufferSize)
filterbankSynthesis.setLowDelayFilterbank(bufferSize)
print("Low Delay Filterbank: ")
print(filterbankAnalysis)
print(filterbankSynthesis)

zLD = np.zeros(nFrames * bufferSize*4)
for i in np.arange(nFrames):
    y = filterbankAnalysis.process(x[i*bufferSize:(i+1)*bufferSize])
    y[25] = 0
    zLD[i*bufferSize:(i+1)*bufferSize] = filterbankSynthesis.process(y)[:,0]


plt.style.use('dark_background')
plt.subplot(2,1,1)
plt.plot(x)
plt.plot(zStandard)
plt.plot(zHQ)
plt.plot(zLD)
plt.xlim(0, 1500)
plt.xlabel('samples')
plt.title('Impulse reponse')
plt.grid(linestyle='--',linewidth=1)

freq = np.linspace(0, 1, x.size)
plt.subplot(2,1,2)
plt.plot(freq,20*np.log(abs(np.fft.fft(x))))
plt.plot(freq,20*np.log(np.maximum(abs(np.fft.fft(zStandard)),1e-8)))
plt.plot(freq,20*np.log(np.maximum(abs(np.fft.fft(zHQ)),1e-8)))
plt.plot(freq,20*np.log(np.maximum(abs(np.fft.fft(zLD)),1e-8)))
plt.xlim(0, 0.2)
plt.xlabel('Normalized frequency')
plt.ylabel('power (dB)')
plt.title('Frequency domain of impulse reponse')
plt.grid(linestyle='--',linewidth=1)

plt.show()