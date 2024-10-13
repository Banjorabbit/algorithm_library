
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
filterTVSos = filterTV.getSosFilter(cutoff, gain, resonance)
print(filterTVSos)

# get frequency responses for each type of filter
nBands = 2049
filterPowerSpectrum = pal.FilterPowerSpectrum()
cFPS = filterPowerSpectrum.getCoefficients()
cFPS['nBands'] = nBands
filterPowerSpectrum.setCoefficients(cFPS)

filterTVPow = filterPowerSpectrum.process(filterTVSos)

pTV["filterType"] = "HighPass"
filterTV.setParameters(pTV)
filterTVSos = filterTV.getSosFilter(cutoff, gain, resonance)
filterTVPow1 = filterPowerSpectrum.process(filterTVSos)

pTV["filterType"] = "BandPass"
filterTV.setParameters(pTV)
filterTVSos = filterTV.getSosFilter(cutoff, gain, resonance)
filterTVPow2 = filterPowerSpectrum.process(filterTVSos)

pTV["filterType"] = "BandStop"
filterTV.setParameters(pTV)
filterTVSos = filterTV.getSosFilter(cutoff, gain, resonance)
filterTVPow3 = filterPowerSpectrum.process(filterTVSos)

pTV["filterType"] = "Peaking"
filterTV.setParameters(pTV)
filterTVSos = filterTV.getSosFilter(cutoff, gain, resonance)
filterTVPow4 = filterPowerSpectrum.process(filterTVSos)

pTV["filterType"] = "LowShelf"
filterTV.setParameters(pTV)
filterTVSos = filterTV.getSosFilter(cutoff, gain, resonance)
filterTVPow5 = filterPowerSpectrum.process(filterTVSos)

pTV["filterType"] = "HighShelf"
filterTV.setParameters(pTV)
filterTVSos = filterTV.getSosFilter(cutoff, gain, resonance)
filterTVPow6 = filterPowerSpectrum.process(filterTVSos)

filterTVPowSum = filterTVPow * filterTVPow1 * filterTVPow2 * filterTVPow3 * filterTVPow4 * filterTVPow5 * filterTVPow6

# plot frequency responses of the IIRFilterCascadeTimeVarying class
smv = 1e-10
plt.plot(10*np.log10(filterTVPow+smv))
plt.plot(10*np.log10(filterTVPow1+smv))
plt.plot(10*np.log10(filterTVPow2+smv))
plt.plot(10*np.log10(filterTVPow3+smv))
plt.plot(10*np.log10(filterTVPow4+smv))
plt.plot(10*np.log10(filterTVPow5+smv))
plt.plot(10*np.log10(filterTVPow6+smv))
plt.plot(10*np.log10(filterTVPowSum))
plt.ylabel('Power (dB)')
plt.grid()

# Combine filters into one cascaded filter and check the power frequency response matches the sum of the individual filters above
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

sosCascade = filterCTV.getSosFilter([cutoff,cutoff,cutoff,cutoff,cutoff,cutoff,cutoff], [gain,gain,gain,gain,gain,gain,gain], [resonance,resonance,resonance,resonance,resonance,resonance,resonance])
filterCTVPow = filterPowerSpectrum.process(sosCascade)
plt.plot(10*np.log10(filterCTVPow))

print("error: ", np.sum((filterTVPowSum-filterCTVPow)**2) / np.sum((filterCTVPow)**2))

plt.show()