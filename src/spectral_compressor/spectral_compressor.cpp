#include "spectral_compressor/spectral_compressor_wola.h"

DEFINE_ALGORITHM_CONSTRUCTOR(SpectralCompressor, SpectralCompressorWOLA, SpectralCompressorConfiguration)

float SpectralCompressor::getDelaySamples() const { return static_cast<SpectralCompressorWOLASingleBufferImpl *>(pimpl.get())->algo.getDelaySamples(); }