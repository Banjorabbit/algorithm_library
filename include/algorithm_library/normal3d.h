#pragma once
#include "interface/interface.h"


// Calculate the 3-dimensional normal vector of a plane for each point in the plane. 
// The input is a 2-dimensional matrix containing the z-value of each point in a x/y grid
// The output is a 2-dimensional matrix, where the 1st output dimension is 3 times the size of the 1st input dimension to hold the interleaved 3-dimensional normal vector.
//
// author: Kristian Timm Andersen

// --------------------------------------------- Filterbank Analysis ----------------------------------------------------------------

struct Normal3dConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        int nValuesX = 257;
        DEFINE_TUNABLE_COEFFICIENTS(nValuesX)
    };

    struct Parameters
    {
        float distance1 = 31.25f; // distance between values along 1st dimension
        float distance2 = 8.f; // distance between values along 2nd dimension
        DEFINE_TUNABLE_PARAMETERS(distance1, distance2)
    };

    static auto validInput(Input input, const Coefficients& c) { return (input.rows() == c.nValuesX) && (input.cols() > 0); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(3 * c.nValuesX, input.cols()); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        int nValuesY, nValuesX;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nValuesX = c.nValuesX;
            nValuesY = 10;
            input.resize(nValuesX, nValuesY);
            input.setRandom();
            output = initOutput(input, c);
        }
    
        inline void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const { return output.allFinite() && (output.cols() == nValuesY) && (output.rows() == 3 * nValuesX); }
    };
};

class Normal3d : public Algorithm<Normal3dConfiguration>
{
public:
    Normal3d() = default;
    Normal3d(const Coefficients& c);
};
