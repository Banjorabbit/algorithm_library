#pragma once
#include "interface/interface.h"

// Spline interpolates the set of data points xGiven/yGiven evaluted at xDesired.
// The inputs must be ordered in increasing x order or the algorithm will give nonsense output.
//
// author: Kristian Timm Andersen

struct SplineConfiguration
{
    struct Input
    {
        I::Real2D xGiven;
        I::Real2D yGiven;
        I::Real2D xDesired;
    };

    using Output = O::Real2D;

    struct Coefficients { DEFINE_NO_TUNABLE_COEFFICIENTS };
    
    struct Parameters
    {
        float sFactor = 0.f;
        DEFINE_TUNABLE_PARAMETERS(sFactor)
    };

    static auto validInput(Input input, const Coefficients& c) { return (input.xDesired.cols() == input.xGiven.cols()) && (input.xDesired.cols() == input.yGiven.cols()) && (input.xGiven.rows() == input.yGiven.rows()); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(input.xDesired.rows(), input.xDesired.cols()); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf xGiven;
        Eigen::ArrayXXf yGiven;
        Eigen::ArrayXXf xDesired;
        Eigen::ArrayXXf yDesired;
        int nOS, nChannels;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            int n = 10; // number of input samples per channel
            nOS = 100; // number of output samples per channel
            nChannels = 2;
            xGiven = Eigen::ArrayXf::LinSpaced(n, 0, n - 1).replicate(1, nChannels);
            yGiven.resize(n, nChannels);
            yGiven.setRandom();
            xDesired = Eigen::ArrayXf::LinSpaced(nOS, 0, n - 1).replicate(1, nChannels);
            yDesired = initOutput({xGiven, yGiven, xDesired}, c);
        }

        inline void processAlgorithm() { algo.process({ xGiven, yGiven, xDesired }, yDesired); }
        bool isTestOutputValid() const { return yDesired.allFinite() && (yDesired.rows() == nOS) && (yDesired.cols() == nChannels); }
    };
};

class Spline : public Algorithm<SplineConfiguration>
{ 
public:
    Spline() = default;
    Spline(const Coefficients& c);
};