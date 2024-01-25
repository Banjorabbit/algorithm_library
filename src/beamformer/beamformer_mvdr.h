#pragma once
#include "algorithm_library/beamformer.h"
#include "framework/framework.h"

class BeamformerMVDR : public AlgorithmImplementation<BeamformerConfiguration, BeamformerMVDR>
{
public:
    BeamformerMVDR(const Coefficients& c = Coefficients()) :
        AlgorithmImplementation<BeamformerConfiguration, BeamformerMVDR>{c}
    {
        filterUpdatesPerFrame = static_cast<int>(c.nBands * filterUpdateRate / c.filterbankRate);
        covarianceUpdateLambda = 1.f - expf(-1.f / (c.filterbankRate * covarianceUpdateTConstant));

        filter.resize(c.nBands, c.nChannels);
        eigenVectors.resize(c.nChannels, c.nChannels);
		filter.resize(c.nBands, c.nChannels);
		Rx.resize(c.nBands);
		for (auto& rx : Rx) { rx.resize(c.nChannels, c.nChannels); }
		Rn.resize(c.nBands);
		for (auto& rn : Rn) { rn.resize(c.nChannels, c.nChannels); }
		Rxn.resize(c.nChannels, c.nChannels);
		eigenSolver = Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXcf>(c.nChannels);

        resetVariables();
		eigenSolver.compute(Rx[0], Rn[0]); // run once to make sure calculation works
    }

    void processOn(Input input, Output yFreq)
	{
		bool activityFlag = input.speechActivity;

		switch (speechDecision)
		{
		case SPEECH: activityFlag = true; break;
		case NOISE: activityFlag = false; break;
		default: break;
		}

		if (speechDecision != FREEZE_UPDATE) { covarianceUpdate({input.xFreq, activityFlag}); }

		for (auto i = 0;i < filterUpdatesPerFrame;i++)
		{
			calculateFilter();
			currentBand++;
			if (currentBand >= C.nBands) { currentBand = 0; }
		}

		yFreq = (input.xFreq * filter).rowwise().sum(); // this has been profiled to be just as fast as multiplying with ones or summing in a for-loop
	}

    enum SpeechUpdateDecisions { NOISE, SPEECH, INPUT, FREEZE_UPDATE };
    void setSpeechDecision(SpeechUpdateDecisions sd) { speechDecision = sd; }
    SpeechUpdateDecisions getSpeechDecision() const { return speechDecision; }

private:

    void covarianceUpdate(Input input)
	{
		for (auto band = 0;band < C.nBands; band++)
		{
			// only calculate lower triangular covariance matrix, since that is all that is used by eigensolver
			// Full matrix is: Rxn = input.xFreq.matrix().row(i).transpose() * input.xFreq.matrix().row(i).conjugate();
			for (auto channel = 0;channel < C.nChannels; channel++)
			{
				Rxn.block(channel, channel, C.nChannels - channel, 1) = input.xFreq.block(band,channel,1,C.nChannels-channel).transpose().matrix() * std::conj(input.xFreq(band, channel));
			}
			Rx[band] += covarianceUpdateLambda * (Rxn - Rx[band]);
			if (!input.speechActivity) { Rn[band] += covarianceUpdateLambda * (Rxn - Rn[band]); }
		}
	}

    void calculateFilter()
	{
		Rx[currentBand] += 1e-16f*Eigen::MatrixXcf::Identity(C.nChannels, C.nChannels);
		Rn[currentBand] += 1e-17f*Eigen::MatrixXcf::Identity(C.nChannels, C.nChannels);
		eigenSolver.compute(Rx[currentBand], Rn[currentBand]);
		eigenVectors = eigenSolver.eigenvectors();
		Eigen::MatrixXcf invEigen = eigenVectors.inverse();

		// calculate max_snr beamformer with mic 0 as reference and put resulting conjugated beamformer for current band into filter 
		filter.row(currentBand) = (eigenVectors.col(C.nChannels - 1) * invEigen(C.nChannels-1,0)).adjoint();		
	}


    void resetVariables() final 
    { 
        currentBand = 0; 
        for (auto i = 0u; i < Rx.size(); i++)
		{
			Rx[i].setZero();
			Rx[i](0,0) = 1e-5f;
			Rn[i].setZero();
		}
        eigenVectors.setZero();
		filter.setZero();
		filter.col(0) = 1;

		Rxn.setZero();
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = filter.getDynamicMemorySize();
		for (auto& rx : Rx) { size += rx.getDynamicMemorySize(); }
		for (auto& rn : Rn) { size += rn.getDynamicMemorySize(); }
		size += eigenVectors.getDynamicMemorySize();
		size += Rxn.getDynamicMemorySize();
        return size;
    }

	SpeechUpdateDecisions speechDecision = INPUT;
    static constexpr float filterUpdateRate = 8.f; // how many times per second is the filter updated
    static constexpr float covarianceUpdateTConstant = 5.f; // covariance update smoothing time constant in seconds
    int filterUpdatesPerFrame;
    float covarianceUpdateLambda;
    int currentBand;
    Eigen::ArrayXXcf filter;
    std::vector<Eigen::MatrixXcf> Rx, Rn;
    Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXcf> eigenSolver;
	Eigen::MatrixXcf eigenVectors, Rxn;    
};