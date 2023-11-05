#pragma once
#include "framework/framework.h"
#include "algorithm_library/min_phase_spectrum.h"
#include "fft/fft_real.h"

// --------------------------------------------------- MinPhaseSpectrumCepstrum ----------------------------------------------------------------

class MinPhaseSpectrumCepstrum : public AlgorithmImplementation<MinPhaseSpectrumConfiguration, MinPhaseSpectrumCepstrum>
{
public:
    MinPhaseSpectrumCepstrum(Coefficients c =  Coefficients()) :
        AlgorithmImplementation<MinPhaseSpectrumConfiguration, MinPhaseSpectrumCepstrum>{ c },
        fft({ 2 * (C.nBands - 1) })
    { }

    FFTReal fft;
    DEFINE_MEMBER_ALGORITHMS(fft)

    inline void processOn(Input magnitude, Output spectrum)
    {
        using namespace std::complex_literals;

        for (auto channel = 0; channel < magnitude.cols(); channel++)
        {
            // calculate cepstrum
            Eigen::ArrayXcf xLog = magnitude.col(channel).max(P.minMagnitude).log().cast<std::complex<float>>();
            Eigen::ArrayXf xCepstrum((C.nBands - 1) * 2);
            fft.inverse(xLog, xCepstrum);
            // fold
            xCepstrum.segment(1, C.nBands - 2) += xCepstrum.segment(C.nBands, C.nBands - 2).colwise().reverse();
            xCepstrum.segment(C.nBands, C.nBands - 2) = 0.f;
            // convert back
            fft.process(xCepstrum, xLog);
            spectrum.col(channel) = xLog.real().exp() * (xLog.imag().cos() + 1.if*xLog.imag().sin()); //complex exponential exp(x+iy) = exp(x)*(cos(y)+i*sin(y)) is much faster according to profiling
        }
    }

};
