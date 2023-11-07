#pragma once
#include "interface/interface.h"

struct BeamformerConfiguration
{
    struct Input
    {
        I::Complex2D xFreq;
        I::Boolean speechActivity;
    };

    using Output = O::Complex;

    struct Coefficients
    {
        int nChannels = 4;
        float filterbankRate = 125.f;
        int nBands = 257;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, filterbankRate, nBands)
    };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input.xFreq.rows() == c.nBands) && (input.xFreq.cols() == c.nChannels);
    }
    
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXcf(c.nBands); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXcf xFreq;
        bool speechActivity;
        Eigen::ArrayXcf yFreq;

        Test() : Test(Coefficients()) {}
        Test(Coefficients c): algo(c)
        {
            xFreq = Eigen::ArrayXXcf::Random(c.nBands, c.nChannels);
            speechActivity = true;
            yFreq = initOutput({xFreq, speechActivity}, c);
        }

        void processAlgorithm() { algo.process({xFreq, speechActivity}, yFreq); }
        bool isTestOutputFinite() const { return yFreq.allFinite(); }
    };  
};

class Beamformer : public Algorithm<BeamformerConfiguration>
{
public:
    Beamformer() = default;
    Beamformer(const Coefficients& c);
};