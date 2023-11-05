#pragma once
#include "framework/framework.h"
#include "algorithm_library/spline.h"

// SplineCubic interpolates the set of data points xGiven/yGiven using cubic splines evaluted at xDesired.
// The inputs must be ordered in increasing order or the algorithm will give nonsense output.
//
// author: Kristian Timm Andersen

class SplineCubic : public AlgorithmImplementation<SplineConfiguration, SplineCubic>
{
public:
    SplineCubic(Coefficients c = Coefficients()) :
        AlgorithmImplementation<SplineConfiguration, SplineCubic>{ c }
    { }

    inline void processOn(Input x, Output y)
    {
        using namespace Eigen;
        // for each spline
        for (auto channel = 0; channel < x.xGiven.cols(); channel++)
        {
            Map<const ArrayXf> input(x.xGiven.col(channel).data(), x.xGiven.rows());
            Map<const ArrayXf> output(x.yGiven.col(channel).data(), x.yGiven.rows());
            Map<const ArrayXf> xDesired(x.xDesired.col(channel).data(), x.xDesired.rows());

            // mk is tangent in starting points
            ArrayXf mk(input.rows());
            auto d = input.rows() - 2;
            mk.segment(1, d) = (1 - P.sFactor) * (output.segment(2, d) - output.segment(0, d)) / (input.segment(2, d) - input.segment(0, d));
            mk.head(1) = (output(1) - output(0)) / (input(1) - input(0));
            mk.tail(1) = (output(output.rows() - 1) - output(output.rows() - 2)) / (input(input.rows() - 1) - input(input.rows() - 2));

            // find index of interpolated points where they cross starting points
            int I = 1;
            ArrayXi index(input.rows());
            index(0) = 0;
            for (int i = 0; i < xDesired.rows(); i++)
            {
                while (xDesired(i) > input(I) && I < input.rows())
                {
                    index(I) = i;
                    I += 1;
                }
                assert((i > 0) ? xDesired(i - 1) < xDesired(i) : true); // ASSERT: desired x-values must be ordered in increasing order
            }
            index.tail(index.rows() - I) = static_cast<int>(xDesired.rows());

            // interpolation
            for (auto i = 0; i < input.rows() - 1; i++)
            {
                const float iDiff = input(i + 1) - input(i);
                const int idxDiff = index(i + 1) - index(i);
                const float oDiff = output(i + 1) - output(i);
                // T is interpolated points between two starting points normalized to the interval (0,1)
                const ArrayXf T = (xDesired.segment(index(i), idxDiff) - input(i)) / iDiff;
                // Interpolation function written as a polynomial of T, i.e. y = x + T*(x1 + T*(x2 + T*(x3)));
                y.block(index(i), channel, idxDiff, 1) = output(i) + T * (iDiff * mk(i) + T * ((3.f * (oDiff)-iDiff * (2.f * mk(i) + mk(i + 1))) + T * (-2.f * oDiff + iDiff * (mk(i) + mk(i + 1)))));

                assert(input(i) < input(i + 1)); // ASSERT: input x-values must be ordered in increasing order
            }
        }
    }
};