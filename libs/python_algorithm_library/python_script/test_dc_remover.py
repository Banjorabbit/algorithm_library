import PythonAlgorithmLibrary as pal
import numpy as np
import matplotlib.pyplot as plt


n = 10000
x = np.random.normal(0,1,n)
print(x.shape)
dcRemover = pal.DCRemover()

s = dcRemover.getSetup()
s['parameters']['cutoffFrequency'] = 100
s['coefficients']['nChannels'] = 1
dcRemover.setSetup(s)

y = dcRemover.process(x)
print(y.shape)
s['parameters']['cutoffFrequency'] = 5000
dcRemover.setSetup(s)

print(dcRemover)
print(dcRemover.validInput(x))
y2 = dcRemover.process(x)

X = np.fft.fft(x)
Y = np.fft.fft(y[:,0])
Y2 = np.fft.fft(y2[:,0])

plt.subplot(2,1,1)
plt.plot(x)
plt.plot(y)
plt.plot(y2)
plt.title('time input')

plt.subplot(2,1,2)
plt.plot(20*np.log10(abs(X)))
plt.plot(20*np.log10(abs(Y)))
plt.plot(20*np.log10(abs(Y2)))
plt.ylim(0, 50)
plt.xlabel('Frequency (bin number)')
plt.ylabel('Power (dB)')
plt.show()
