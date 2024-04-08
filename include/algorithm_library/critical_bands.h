#pragma once
#include "interface/interface.h"

// Convert power spectrum into critical bands using either sum, mean or max
//
// The class has a public inverse method.
//
// author: Kristian Timm Andersen

struct CriticalBandsConfiguration
{

    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        int nBands = 257;
        float sampleRate = 44.1e3f;
        DEFINE_TUNABLE_COEFFICIENTS(nBands, sampleRate)
    };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static int getNCriticalBands(float sampleRate);

    static Eigen::ArrayXXf initInput(const Coefficients& c) { return Eigen::ArrayXXf::Random(c.nBands, 2).abs2(); } // power spectrum. Number of channels can be arbitrary

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf::Zero(getNCriticalBands(c.sampleRate), input.cols()); } // power spectrum in critical bands

    static bool validInput(Input input, const Coefficients& c) 
    { 
        return (input.rows() == c.nBands) && (input.cols() > 0) && (input >= 0).all();
    }
    
    static bool validOutput(Output output, const Coefficients& c) 
    { 
        return (output.rows() == getNCriticalBands(c.sampleRate)) && (output.cols() > 0) && (output >= 0).all();
    }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        int nChannels = 2;
        int nCriticalBands;
        Eigen::ArrayXXf xPower;
        Eigen::ArrayXXf yPower;

        Example() : Example(Coefficients()) {}
        Example(Coefficients c): algo(c)
        {
            xPower = Eigen::ArrayXXf::Random(c.nBands, nChannels).abs2();
            yPower = initOutput(xPower, c);
            nCriticalBands = algo.getNCriticalBands(c.sampleRate);
        }

        void processAlgorithm() { algo.process(xPower, yPower); }
        bool isExampleOutputValid() const 
        {
            bool test = yPower.allFinite();
            test &= (yPower >= 0).all();
            test &= (yPower.cols() == nChannels) && (yPower.rows() == nCriticalBands);
            return test; 
        }
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
