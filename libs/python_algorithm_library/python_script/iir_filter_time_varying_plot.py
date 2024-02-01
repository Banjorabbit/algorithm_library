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
plt.plot(10*np.log10(filterTVPow)+10*np.log10(filterTVPow1)+10*np.log10(filterTVPow2)+10*np.log10(filterTVPow3)+10*np.log10(filterTVPow4)+10*np.log10(filterTVPow5)+10*np.log10(filterTVPow6))
plt.ylabel('Power (dB)')
plt.grid()


# plot frequency responses of the IIRFilterCascadeTimeVarying class and check the power frequency response matches the sum of the individual filters above
filterCTV = pal.IIRFilterCascadeTimeVarying()
cCTV = filterCTV.getCoefficients()
cCTV['nSos'] = 7
filterCTV.setCoefficients(cCTV)
filterType = filterCTV.getFilterTypes()
filterType[0] = 0
filterType[1] = 1
filterType[2] = 2
filterType[3] = 3
filterType[4] = 4
filterType[5] = 5
filterType[6] = 6
filterCTV.setFilterTypes(filterType)
filterType = filterCTV.getFilterTypes() #{0,1,2,3,4,5,6}
print("Filter type: ", filterType)

filterCTVPow = filterCTV.getPowerFrequencyResponse(257, [cutoff,cutoff,cutoff,cutoff,cutoff,cutoff,cutoff], [gain,gain,gain,gain,gain,gain,gain], [resonance,resonance,resonance,resonance,resonance,resonance,resonance])
plt.plot(10*np.log10(filterCTVPow))
plt.show()