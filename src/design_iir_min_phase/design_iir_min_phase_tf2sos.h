#pragma once
#include "framework/framework.h"
#include "algorithm_library/design_iir_min_phase.h"
#include <unsupported/Eigen/Polynomials>
#include "min_phase_spectrum/min_phase_spectrum_cepstrum.h"
#include "fft/fft_real.h"

// Design minimum phase IIR filter from a magnitude spectrum. The IIR filter is factorized in a cascade of 2nd order sections.
//
// author: Kristian Timm Andersen
class DesignIIRMinPhaseTF2SOS : public AlgorithmImplementation<DesignIIRMinPhaseConfiguration, DesignIIRMinPhaseTF2SOS>
{
public:
    DesignIIRMinPhaseTF2SOS(const DesignIIRMinPhaseConfiguration::Coefficients& c = Coefficients()) : 
        AlgorithmImplementation<DesignIIRMinPhaseConfiguration, DesignIIRMinPhaseTF2SOS>(c),
        minPhaseCalculator({c.nBands}),
        fft({2*(c.nBands-1)})
    {
        fftSize = 2 * (c.nBands-1);
        R.resize(2 * c.nOrder + 1, 2 * c.nOrder + 1);
        R.block(c.nOrder, c.nOrder, c.nOrder+1, c.nOrder+1).setIdentity();
        weight.resize(c.nBands);
        xFreq.resize(C.nBands);
        xPow.resize(C.nBands);
        xTime.resize(fftSize);
        Vd.resize(2 * C.nOrder + 1);
        th.resize(2 * C.nOrder + 1);
        A.resize(C.nOrder + 1);
        B.resize(C.nOrder + 1);
        if (c.weightType == c.MELSCALE)
        {
            weight(0) = 1.6f;
			Eigen::ArrayXf freq = Eigen::ArrayXf::LinSpaced(c.nBands - 1, 1, static_cast<float>(c.nBands - 1)) * c.sampleRate / fftSize;
			weight.tail(c.nBands - 1) = 2595.f*(1.f + freq / 700).log10() / freq;
        }
        else
        {
            weight.setOnes();
        }
    }

    MinPhaseSpectrumCepstrum minPhaseCalculator;
    FFTReal fft;
    DEFINE_MEMBER_ALGORITHMS(minPhaseCalculator, fft)

    int getNSos() const { return Configuration::getNSos(C.nOrder); }

    // convert transfer function to second order sections
    void TF2SOS(I::Real tf, O::Real2D sos, O::Float gain)
    {
        const auto N = tf.size() - 1;
		const auto N1 = N - 1;

		//// This code is equivalent to the PolynomialSolver (with different ordering of roots), but is slightly slower when profiling. If this is used then Eigen::EigenSolver<Eigen::MatrixXf> needs to be a member in class
		//Eigen::MatrixXf companionMatrix = Eigen::MatrixXf::Zero(N, N);
		//companionMatrix.diagonal(-1) = Eigen::VectorXf::Ones(N - 1);
		//companionMatrix.row(0) = -tf.tail(N).transpose() / tf(0);
		//D.EigenSolver.compute(companionMatrix);
		//Eigen::VectorXcf poles = D.EigenSolver.eigenvalues();

		polynomialsolver.compute(tf.reverse().matrix());
		Eigen::VectorXcf roots = polynomialsolver.roots();
        Eigen::ArrayXf rootsPow = roots.array().abs2();

		Eigen::VectorXcf rootsStable(N);
		auto start = 0;
		auto end = N1;
		for (auto i = 0; i < N;i++)
		{
			if (roots(i).imag() == 0)
			{
				if (rootsPow(i) > 1.f) { rootsStable(end) = std::real(roots(i)) / rootsPow(i); }
				else { rootsStable(end) = roots(i); }
				end--;
			}
			else
			{
				if (rootsPow(i) > 1.f) { rootsStable(start) = roots(i) / rootsPow(i); }
				else { rootsStable(start) = roots(i); }
				start++;
			}
		}

		auto nSections = sos.cols();
		sos.row(0).setOnes();
		for (auto i = 0; i < N - 1; i += 2)
		{
			sos(1, i / 2) = -std::real(rootsStable(i) + rootsStable(i + 1));
			sos(2, i / 2) = std::real(rootsStable(i) * rootsStable(i + 1));
		}
		if (N & 1) // if odd number of roots
		{
			sos(1, nSections - 1) = -std::real(rootsStable(N1));
			sos(2, nSections - 1) = 0.f;
		}
		float gainSOS = sos.colwise().sum().prod();
		float gainA = tf.sum();
		gain = gainA / gainSOS;
    }

private:

        void processOn(Input magnitudeSpectrum, Output output)
    {
        assert(magnitudeSpectrum.rows() == C.nBands);
        minPhaseCalculator.process(magnitudeSpectrum, xFreq);

        // lower left and upper right corner
        xPow = xFreq * weight;
        fft.inverse(xPow, xTime);

        for (auto i = 0; i < C.nOrder; i++)
        {
            const auto r2 = std::max(C.nOrder - i, 0);
            const auto r1 = C.nOrder + 1 - r2;
            R.block(C.nOrder, i, r1, 1) = -xTime.segment(fftSize - 1 - i, r1);
			R.block(C.nOrder + r1, i, r2, 1) = -xTime.head(r2);
        }
        // upper right corner
        R.block(0, C.nOrder, C.nOrder, C.nOrder + 1) = R.block(C.nOrder, 0, C.nOrder + 1, C.nOrder).transpose();
		Vd.tail(C.nOrder + 1) = xTime.head(C.nOrder + 1);

        // upper left corner
        xPow *= xFreq.conjugate();
		fft.inverse(xPow, xTime);
		for (auto i = 0; i < C.nOrder; i++)
		{
			const auto r2 = C.nOrder - i;
			const auto r1 = C.nOrder - r2;
			R.block(i, i, r2, 1) = xTime.head(r2);
			R.block(0, i, r1, 1) = xTime.segment(1, r1).reverse();
		}
		Vd.head(C.nOrder) = -xTime.segment(1, C.nOrder);

        th = R.llt().solve(Vd);

		A(0) = 1.f;
		A.tail(C.nOrder) = th.head(C.nOrder);
		B = th.tail(C.nOrder + 1);

		// convert to sos matrix and gain
		TF2SOS(B, output.sos.topRows(3), output.gain);
		float gainDen;
		TF2SOS(A, output.sos.bottomRows(3), gainDen);
		output.gain /= gainDen;
    }
    
    size_t getDynamicSizeVariables() const final
    {
        size_t size = weight.getDynamicMemorySize();
        size += R.getDynamicMemorySize();
        size += xFreq.getDynamicMemorySize();
        size += xPow.getDynamicMemorySize();
        size += xTime.getDynamicMemorySize();
        size += Vd.getDynamicMemorySize();
        size += th.getDynamicMemorySize();
        size += A.getDynamicMemorySize();
        size += B.getDynamicMemorySize();
        return size;
    }

    int fftSize;
    Eigen::ArrayXcf xFreq;
    Eigen::ArrayXf weight;
    Eigen::MatrixXf R;
    Eigen::PolynomialSolver<float, Eigen::Dynamic> polynomialsolver;
    Eigen::ArrayXcf xPow;
    Eigen::ArrayXf xTime;
    Eigen::VectorXf Vd;
    Eigen::VectorXf th;
    Eigen::ArrayXf A;
    Eigen::ArrayXf B;

    friend AlgorithmImplementation<DesignIIRMinPhaseConfiguration, DesignIIRMinPhaseTF2SOS>;
};