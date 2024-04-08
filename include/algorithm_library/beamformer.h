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

    static std::tuple<Eigen::ArrayXXcf, bool> initInput(const Coefficients& c) { return std::make_tuple(Eigen::ArrayXXcf::Random(c.nBands, c.nChannels), true); } // complex frequency spectrum and activity flag

    static Eigen::ArrayXcf initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXcf::Zero(c.nBands); } // complex frequency spectrum

    static bool validInput(Input input, const Coefficients& c) 
    { 
        return (input.xFreq.rows() == c.nBands) && (input.xFreq.cols() == c.nChannels) && input.xFreq.allFinite();
    }

    static bool validOutput(Output output, const Coefficients& c) 
    { 
        return (output.rows() == c.nBands) && output.allFinite();
    }
    
    

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXcf xFreq;
        bool speechActivity;
        int nBands;
        Eigen::ArrayXcf yFreq;

        Example() : Example(Coefficients()) {}
        Example(Coefficients c): algo(c)
        {
            nBands = c.nBands;
            xFreq = Eigen::ArrayXXcf::Random(nBands, c.nChannels);
            speechActivity = true;
            yFreq = initOutput({xFreq, speechActivity}, c);
        }

        void processAlgorithm() { algo.process({xFreq, speechActivity}, yFreq); }
        bool isExampleOutputValid() const { return yFreq.allFinite() && (yFreq.rows() == nBands); }
    };  
};

class Beamformer : public Algorithm<BeamformerConfiguration>
{
public:
    Beamformer() = default;
    Beamformer(const Coefficients& c);
};