#pragma once
#include "algorithm_library/spectral_compressor.h"
#include "delay/circular_buffer.h"
#include "framework/framework.h"
#include "framework/vector_algo.h"
#include "spectral_compressor/spectral_compressor_wola.h"
#include "spectral_compressor/spectral_selector.h"

// Spectral compressor using an adaptive selection of spectral compressors
//
// author: Kristian Timm Andersen
class SpectralCompressorAdaptive : public AlgorithmImplementation<SpectralCompressorConfiguration, SpectralCompressorAdaptive>
{
  public:
    SpectralCompressorAdaptive(Coefficients c = {.nChannels = 2, .sampleRate = 48000.f, .bufferSize = 4096})
        : BaseAlgorithm{c}, 
          delayShort({.nChannels = c.nChannels, .delayLength = 3 * (c.bufferSize - c.bufferSize / 16)}),
          delayMedium({.nChannels = c.nChannels, .delayLength = 3 * (c.bufferSize - c.bufferSize / 4)}), 
          spectralCompressorShort({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize / 16}),
          spectralCompressorMedium({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize / 4}),
          spectralCompressorLong({.nChannels = c.nChannels, .sampleRate = c.sampleRate, .bufferSize = c.bufferSize}),
          spectralSelector({.nChannels = 3 * c.nChannels, .nStreams = 3, .bufferSize = c.bufferSize / 16})
    {
        spectralBuffer.resize(c.bufferSize, 3 * c.nChannels);
        resetVariables();
    }

    CircularBuffer delayShort;
    CircularBuffer delayMedium;
    SpectralCompressorWOLA spectralCompressorShort;
    SpectralCompressorWOLA spectralCompressorMedium;
    SpectralCompressorWOLA spectralCompressorLong;
    SpectralSelector spectralSelector;

    DEFINE_MEMBER_ALGORITHMS(delayShort, delayMedium, spectralCompressorShort, spectralCompressorMedium, spectralCompressorLong, spectralSelector)

    float getDelaySamples() const { return static_cast<float>(C.bufferSize * 3); }

  private:
    void inline processAlgorithm(Input input, Output output)
    {
        delayMedium.process(input, spectralBuffer.middleCols(C.nChannels, C.nChannels)); // delay so delayMedium + spectralCompressorMedium has delay equal to spectralCompressorLong
        delayShort.process(input, spectralBuffer.rightCols(C.nChannels)); // delay so delayShort + spectralCompressorShort has delay equal to spectralCompressorLong
        spectralCompressorLong.process(input, spectralBuffer.leftCols(C.nChannels));
        for (auto iMedium = 0; iMedium < 4; iMedium++)
        {
            // spectralCompressor can share input/output buffer
            spectralCompressorMedium.process(spectralBuffer.block(iMedium * C.bufferSize / 4, C.nChannels, C.bufferSize / 4, C.nChannels),
                                             spectralBuffer.block(iMedium * C.bufferSize / 4, C.nChannels, C.bufferSize / 4, C.nChannels));
            for (auto iShort = 0; iShort < 4; iShort++)
            {
                // spectralCompressor can share input/output buffer
                spectralCompressorShort.process(
                    spectralBuffer.block(iMedium * C.bufferSize / 4 + iShort * C.bufferSize / 16, 2 * C.nChannels, C.bufferSize / 16, C.nChannels),
                    spectralBuffer.block(iMedium * C.bufferSize / 4 + iShort * C.bufferSize / 16, 2 * C.nChannels, C.bufferSize / 16, C.nChannels));

                spectralSelector.process(spectralBuffer.middleRows(iMedium * C.bufferSize / 4 + iShort * C.bufferSize / 16, C.bufferSize / 16),
                                         output.middleRows(iMedium * C.bufferSize / 4 + iShort * C.bufferSize / 16, C.bufferSize / 16));
            }
        }
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = spectralBuffer.getDynamicMemorySize();
        return size;
    }

    void resetVariables() final { spectralBuffer.setZero(); }

    Eigen::ArrayXXf spectralBuffer;

    friend BaseAlgorithm;
};