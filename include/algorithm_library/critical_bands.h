#pragma once
#include "interface/interface.h"

// Convert power spectrum into critical bands using either sum, mean or max
//
// The class has a public inverse method.
//
// author: Kristian Timm Andersen


struct CriticalBandsConfiguration : public Configuration<I::Real2D, O::Real2D>
{
    struct Coefficients
    {
        int nBands = 257;
        float sampleRate = 44.1e3f;
        DEFINE_TUNABLE_COEFFICIENTS(nBands, sampleRate)
    };

	template<typename Talgo>
	struct Test
	{
		Eigen::ArrayXXf xPower;
        Eigen::ArrayXXf yPower;

		Test()
		{
            int nChannels = 2;
            auto c = Coefficients();
            xPower = Eigen::ArrayXXf::Random(c.nBands, nChannels).abs2();
            yPower.resize(c.nBands, nChannels);
		}

		void processAlgorithm(Talgo& algo) { algo.process(xPower, yPower); }
		bool isTestOutputFinite() const { return yPower.allFinite(); }
	};
};

struct CriticalBandsSumConfiguration : public CriticalBandsConfiguration {};
struct CriticalBandsMeanConfiguration : public CriticalBandsConfiguration {};
struct CriticalBandsMaxConfiguration : public CriticalBandsConfiguration {};

class CriticalBandsSum : public Algorithm<CriticalBandsSumConfiguration>
{
public:
	CriticalBandsSum() = default;
    CriticalBandsSum(const Coefficients& c);
    
    static int getNCriticalBands(float sampleRate);
	static Eigen::ArrayXf getCenterFrequencies(float sampleRate);
	static Eigen::ArrayXf getCornerFrequencies(float sampleRate);
	void inverse(I::Real2D xPower, O::Real2D yPower);
};

class CriticalBandsMean : public Algorithm<CriticalBandsMeanConfiguration>
{
public:
	CriticalBandsMean() = default;
    CriticalBandsMean(const Coefficients& c);
    
    static int getNCriticalBands(float sampleRate);
	static Eigen::ArrayXf getCenterFrequencies(float sampleRate);
	static Eigen::ArrayXf getCornerFrequencies(float sampleRate);
	void inverse(I::Real2D xPower, O::Real2D yPower);
};

class CriticalBandsMax : public Algorithm<CriticalBandsMaxConfiguration>
{
public:
	CriticalBandsMax() = default;
    CriticalBandsMax(const Coefficients& c);
    
    static int getNCriticalBands(float sampleRate);
	static Eigen::ArrayXf getCenterFrequencies(float sampleRate);
	static Eigen::ArrayXf getCornerFrequencies(float sampleRate);
	void inverse(I::Real2D xPower, O::Real2D yPower);
};
