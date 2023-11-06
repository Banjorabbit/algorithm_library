#pragma once
#include "framework/framework.h"
#include "algorithm_library/critical_bands.h"

// bark scale
namespace BarkScale
{
    constexpr static float frequencyCornersArrayHz[25] = { 20.f, 100.f, 200.f, 300.f, 400.f, 510.f, 630.f, 770.f, 920.f, 1080.f, 1270.f, 1480.f, 1720.f, 2000.f, 2320.f, 2700.f, 3150.f, 3700.f, 4400.f, 5300.f, 6400.f, 7700.f, 9500.f, 12000.f, 15500.f };
    constexpr static float frequencyCentersArrayHz[24] = { 50.f,150.f,250.f,350.f,450.f,570.f,700.f,840.f,1000.f,1170.f,1370.f,1600.f,1850.f,2150.f,2500.f,2900.f,3400.f,4000.f,4800.f,5800.f,7000.f, 8500.f, 10500.f, 13500.f }; 

    inline int getNCriticalBands(float sampleRate)
    {
        // calculate number of critical bands
        Eigen::Map<const Eigen::ArrayXf> frequencyCornersHz(frequencyCornersArrayHz, 25);
        return (frequencyCornersHz <= sampleRate / 2).cast<int>().sum() - 1;
    }

    inline Eigen::ArrayXf getCenterFrequencies(float sampleRate) 
    {
        return Eigen::Map<const Eigen::ArrayXf>(frequencyCentersArrayHz, getNCriticalBands(sampleRate));
    }

    inline Eigen::Map<const Eigen::ArrayXf> getCornerFrequencies(float sampleRate)
    {
        return Eigen::Map<const Eigen::ArrayXf>(frequencyCornersArrayHz, getNCriticalBands(sampleRate) + 1);
    }
}

// Common CriticalBandsBark class that following classes inherit from and specialize
class CriticalBandsBark
{
public:
    CriticalBandsBark(CriticalBandsConfiguration::Coefficients c = CriticalBandsConfiguration::Coefficients())
    {
        // calculate number of critical bands
        nBandsCritical = BarkScale::getNCriticalBands(c.sampleRate);
    
        // convert Frequency corners to FFT indices
        Eigen::Map<const Eigen::ArrayXf> frequencyCornersHz(BarkScale::frequencyCornersArrayHz, 25);
        auto nFFT = (c.nBands - 1) * 2; // number of FFT points
        Eigen::ArrayXi frequencyCornersIndex = (frequencyCornersHz * nFFT / c.sampleRate).round().cast<int>();
        indexStart = frequencyCornersIndex.head(nBandsCritical);
        nBandsSum = (frequencyCornersIndex.segment(1, nBandsCritical) - indexStart).cwiseMax(1);
        indexEnd = indexStart(nBandsCritical - 1) + nBandsSum(nBandsCritical - 1);
    }

    inline void inverse(I::Real2D xPower, O::Real2D yPower, const int nBands)
    {
        assert(xPower.rows() == nBandsCritical);

        for (auto channel = 0; channel < xPower.cols(); channel++)
        {
            yPower.block(0, channel, indexStart(0), 1).setConstant(xPower(0, channel));
            for (auto i = 0; i < nBandsCritical; i++)
            {
                yPower.block(indexStart(i), channel, nBandsSum(i), 1).setConstant(xPower(i, channel));
            }
            yPower.block(indexEnd, channel, nBands - indexEnd, 1).setConstant(xPower(nBandsCritical - 1, channel));
        }
    }

protected:
    size_t getDynamicSizeVariables() const
    { 
        size_t size = indexStart.getDynamicMemorySize();
        size += nBandsSum.getDynamicMemorySize();
        return size;
    }

    int nBandsCritical, indexEnd;
    Eigen::ArrayXi indexStart;
    Eigen::ArrayXi nBandsSum;
};

// --------------------------------------------------- CriticalBandsBarkSum ----------------------------------------------------------------

class CriticalBandsBarkSum : public CriticalBandsBark, public AlgorithmImplementation<CriticalBandsSumConfiguration, CriticalBandsBarkSum>
{
public:
    CriticalBandsBarkSum(Coefficients c =  Coefficients()) :
        CriticalBandsBark(c), 
        AlgorithmImplementation<CriticalBandsSumConfiguration, CriticalBandsBarkSum>{ c }
    { }

    inline void processOn(Input xPower, Output yPower)
    {
       for (auto channel = 0; channel < xPower.cols(); channel++)
        {
            for (auto i = 0; i < nBandsCritical; i++)
            {
                // sum in a critical band
                yPower(i, channel) = xPower.block(indexStart(i), channel, nBandsSum(i), 1).sum();
            }
        }
    }

    inline void inverse(I::Real2D xPower, O::Real2D yPower) { CriticalBandsBark::inverse(xPower, yPower, C.nBands); }

private:
    size_t getDynamicSizeVariables() const final { return CriticalBandsBark::getDynamicSizeVariables(); }
};

// --------------------------------------------------- CriticalBandsBarkMax ----------------------------------------------------------------

class CriticalBandsBarkMax : public CriticalBandsBark, public AlgorithmImplementation<CriticalBandsMaxConfiguration, CriticalBandsBarkMax>
{
public:

    CriticalBandsBarkMax(Coefficients c =  Coefficients()) :
        CriticalBandsBark(c), 
        AlgorithmImplementation<CriticalBandsMaxConfiguration, CriticalBandsBarkMax>{ c }
    { }

    inline void processOn(Input xPower, Output yPower)
    {
       for (auto channel = 0; channel < xPower.cols(); channel++)
        {
            for (auto i = 0; i < nBandsCritical; i++)
            {
                // find max in a critical band
                yPower(i, channel) = xPower.block(indexStart(i), channel, nBandsSum(i), 1).maxCoeff();
            }
        }
    }

    inline void inverse(I::Real2D xPower, O::Real2D yPower) { CriticalBandsBark::inverse(xPower, yPower, C.nBands); }

private:
    size_t getDynamicSizeVariables() const final { return CriticalBandsBark::getDynamicSizeVariables(); }
};

// --------------------------------------------------- CriticalBandsBarkMean ----------------------------------------------------------------

class CriticalBandsBarkMean : public CriticalBandsBark, public AlgorithmImplementation<CriticalBandsMeanConfiguration, CriticalBandsBarkMean>
{
public:

    CriticalBandsBarkMean(Coefficients c =  Coefficients()) :
        CriticalBandsBark(c), 
        AlgorithmImplementation<CriticalBandsMeanConfiguration, CriticalBandsBarkMean>{ c }
    { }

    inline void processOn(Input xPower, Output yPower)
    {
       for (auto channel = 0; channel < xPower.cols(); channel++)
        {
            for (auto i = 0; i < nBandsCritical; i++)
            {
                // mean in a critical band
                yPower(i, channel) = xPower.block(indexStart(i), channel, nBandsSum(i), 1).mean();
            }
        }
    }

    inline void inverse(I::Real2D xPower, O::Real2D yPower) { CriticalBandsBark::inverse(xPower, yPower, C.nBands); }

private:
    size_t getDynamicSizeVariables() const final { return CriticalBandsBark::getDynamicSizeVariables(); }
};