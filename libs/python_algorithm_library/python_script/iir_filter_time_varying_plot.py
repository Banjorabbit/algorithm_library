import PythonAlgorithmLibrary as pal
import numpy as np
import matplotlib.pyplot as plt

# plot frequency responses of the IIRFilterTimeVarying class

filterTV = pal.IIRFilterTimeVarying()
pTV = filterTV.getParameters()
print(filterTV)
print(pTV)

# set filter and plot coefficients
cutoff = 4000
gain = .25
resonance = .7071
filterTVSos = filterTV.getFilter(cutoff, gain, resonance)
print(filterTVSos)

# get frequency responses for each type of filter
filterTVPow = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = 1
filterTV.setParameters(pTV)
filterTVPow1 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = 2
filterTV.setParameters(pTV)
filterTVPow2 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = 3
filterTV.setParameters(pTV)
filterTVPow3 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = 4
filterTV.setParameters(pTV)
filterTVPow4 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = 5
filterTV.setParameters(pTV)
filterTVPow5 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = 6
filterTV.setParameters(pTV)
filterTVPow6 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

plt.plot(10*np.log10(filterTVPow))
plt.plot(10*np.log10(filterTVPow))
plt.plot(10*np.log10(filterTVPow1))
plt.plot(10*np.log10(filterTVPow2))
plt.plot(10*np.log10(filterTVPow3))
plt.plot(10*np.log10(filterTVPow4))
plt.plot(10*np.log10(filterTVPow5))
plt.plot(10*np.log10(filterTVPow6))
plt.ylabel('Power (dB)')
plt.grid()
plt.show()
