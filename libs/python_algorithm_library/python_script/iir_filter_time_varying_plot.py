
import PythonAlgorithmLibrary as pal
import numpy as np
import matplotlib.pyplot as plt

# plot frequency responses of the IIRFilterTimeVarying class

filterTV = pal.IIRFilterTimeVarying()
pTV = filterTV.getParameters()
print(filterTV)
print(pTV)

# set filter and plot coefficients
cutoff = np.tan(np.pi * 4000/16000)
gain = .25
resonance = .7071
filterTVSos = filterTV.getFilter(cutoff, gain, resonance)
print(filterTVSos)

# get frequency responses for each type of filter
filterTVPow = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = "HighPass"
filterTV.setParameters(pTV)
filterTVPow1 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = "BandPass"
filterTV.setParameters(pTV)
filterTVPow2 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = "BandStop"
filterTV.setParameters(pTV)
filterTVPow3 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = "Peaking"
filterTV.setParameters(pTV)
filterTVPow4 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = "LowShelf"
filterTV.setParameters(pTV)
filterTVPow5 = filterTV.getPowerFrequencyResponse(257, cutoff, gain, resonance)

pTV["filterType"] = "HighShelf"
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
plt.plot(10*np.log10(filterTVPow)+10*np.log10(filterTVPow1)+10*np.log10(filterTVPow2)+10*np.log10(filterTVPow3)+10*np.log10(filterTVPow4)+10*np.log10(filterTVPow5)+10*np.log10(filterTVPow6))
plt.ylabel('Power (dB)')
plt.grid()


# plot frequency responses of the IIRFilterCascadeTimeVarying class and check the power frequency response matches the sum of the individual filters above
filterCTV = pal.IIRFilterCascadeTimeVarying()
cCTV = filterCTV.getCoefficients()
cCTV['nSos'] = 7
filterCTV.setCoefficients(cCTV)
filterType = filterCTV.getFilterTypes()
print("Filter type initial: ", filterType)
filterType[0] = "LowPass"
filterType[1] = "HighPass"
filterType[2] = "BandPass"
filterType[3] = "BandStop"
filterType[4] = "Peaking"
filterType[5] = "LowShelf"
filterType[6] = "HighShelf"
filterCTV.setFilterTypes(filterType)
filterType = filterCTV.getFilterTypes() 
print("Filter type: ", filterType)

filterCTVPow = filterCTV.getPowerFrequencyResponse(257, [cutoff,cutoff,cutoff,cutoff,cutoff,cutoff,cutoff], [gain,gain,gain,gain,gain,gain,gain], [resonance,resonance,resonance,resonance,resonance,resonance,resonance])
plt.plot(10*np.log10(filterCTVPow))
plt.show()