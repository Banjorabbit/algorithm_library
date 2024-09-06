#include "algorithm_library/spectral_compressor.h"
#include "delay/circular_buffer.h"
#include "framework/framework.h"
#include "spectral_compressor/spectral_compressor_wola.h"
#include "spectral_compressor/spectral_selector.h"

// Spectral compressor using an adaptive selection of spectral compressors
//
// author: Kristian Timm Andersen
class SpectralCompressorAdaptive : public AlgorithmImplementation<SpectralCompressorConfiguration, SpectralCompressorAdaptive>
{
  public:
    SpectralCompressorAdaptive(Coefficients c = Coefficients())
        : BaseAlgorithm{c}, spectralCompressorShort({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize / 16}),
          spectralCompressorMedium({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize / 4}),
          spectralCompressorLong({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize}),
          spectralSelector({.nChannels = c.nChannels, .nStreams = 3, .bufferSize = c.bufferSize / 16})
    {
        spectralBufferShort.resize(c.bufferSize / 16, c.nChannels);
        spectralBufferMedium.resize(c.bufferSize / 4, c.nChannels);
        spectralBufferLong.resize(c.bufferSize, c.nChannels);
        resetVariables();
        onParametersChanged();
    }

    SpectralCompressorWOLA spectralCompressorShort;
    SpectralCompressorWOLA spectralCompressorMedium;
    SpectralCompressorWOLA spectralCompressorLong;
    SpectralSelector spectralSelector;

    DEFINE_MEMBER_ALGORITHMS(spectralCompressorShort, spectralCompressorMedium, spectralCompressorLong, spectralSelector)

    float getDelaySamples() const { return C.bufferSize * 3; }

  private:
    void inline processAlgorithm(Input input, Output output)
    {
        spectralCompressorLong.process(input, spectralBufferLong);
        for (auto iMedium = 0; iMedium < 4; iMedium++)
        {
            spectralCompressorMedium.process(input.middleRows(iMedium * C.bufferSize / 4, C.bufferSize / 4), spectralBufferMedium);
            for (auto iShort = 0; iShort < 4; iShort++)
            {

                spectralCompressorShort.process(input.middleRows(iMedium * C.bufferSize / 4 + iShort * C.bufferSize / 16, C.bufferSize / 16), spectralBufferShort);
                spectralSelector.process(spectralBufferShort, spectralBufferMedium.middleRows(iShort * C.bufferSize / 16, C.bufferSize / 16),
                                         spectralBufferLong.middleRows(iMedium * C.bufferSize / 4 + iShort * C.bufferSize / 16, C.bufferSize / 16),
                                         output.middleRows(iMedium * C.bufferSize / 4 + iShort * C.bufferSize / 16, C.bufferSize / 16));
            }
        }
    }

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