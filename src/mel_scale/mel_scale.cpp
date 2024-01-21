#include "mel_scale/mel_scale_spectrogram.h"

DEFINE_ALGORITHM_CONSTRUCTOR(MelScale, MelScaleSpectrogram, MelScaleConfiguration)

Eigen::ArrayXf MelScale::getCornerFrequencies() const
{
    return  static_cast<MelScaleSpectrogramSingleBufferImpl*>(pimpl.get())->algo.getCornerFrequencies();
}