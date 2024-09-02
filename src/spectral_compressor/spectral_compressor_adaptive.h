#include "algorithm_library/spectral_compressor.h"
#include "delay/circular_buffer.h"
#include "framework/framework.h"
#include "spectral_compressor/spectral_compressor_wola.h"

// Spectral compressor using an adaptive selection of spectral compressors
//
// author: Kristian Timm Andersen
class SpectralCompressorAdaptive : public AlgorithmImplementation<SpectralCompressorConfiguration, SpectralCompressorAdaptive>
{
  public:
    SpectralCompressorAdaptive(Coefficients c = Coefficients())
        : BaseAlgorithm{c}, spectralCompressorShort({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize / 4}),
          spectralCompressorMedium({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize}),
          spectralCompressorLong({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize * 4})
    {
        spectralBufferShort.resize(c.bufferSize / 4, c.nChannels);
        spectralBufferMedium.resize(c.bufferSize, c.nChannels);
        spectralBufferLong.resize(c.bufferSize * 4, c.nChannels);
        resetVariables();
        onParametersChanged();
    }

    SpectralCompressorWOLA spectralCompressorShort;
    SpectralCompressorWOLA spectralCompressorMedium;
    SpectralCompressorWOLA spectralCompressorLong;

    DEFINE_MEMBER_ALGORITHMS(spectralCompressorShort, spectralCompressorMedium, spectralCompressorLong)

    float getDelaySamples() const { return C.bufferSize * 3; }

  private:
    void inline processAlgorithm(Input input, Output output) {}

    void onParametersChanged() {}

    size_t getDynamicSizeVariables() const final
    {
        size_t size = spectralBufferShort.getDynamicMemorySize();
        size += spectralBufferMedium.getDynamicMemorySize();
        size += spectralBufferLong.getDynamicMemorySize();
        return size;
    }

    void resetVariables() final
    {
        spectralBufferShort.setZero();
        spectralBufferMedium.setZero();
        spectralBufferLong.setZero();
    }

    Eigen::ArrayXXf spectralBufferShort;
    Eigen::ArrayXXf spectralBufferMedium;
    Eigen::ArrayXXf spectralBufferLong;

    friend BaseAlgorithm;
};