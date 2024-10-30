#pragma once
#include "algorithm_library/interface/interface.h"
#include "framework/framework.h"

// Upscale a 2D float array with a factor in horizontal and vertical direction
//
// author: Kristian Timm Andersen

struct Upscale2DConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        int factorHorizontal = 4;
        int factorVertical = 4;
        DEFINE_TUNABLE_COEFFICIENTS(factorHorizontal, factorVertical)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static Eigen::ArrayXXf initInput(const Coefficients &c)
    {
        return Eigen::ArrayXXf::Random(100, 100); // arbitrary number of samples
    }

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c)
    {
        return Eigen::ArrayXXf::Zero(c.factorVertical * (input.rows() - 1) + 1, c.factorHorizontal * (input.cols() - 1) + 1);
    }

    static bool validInput(Input input, const Coefficients &c) { return input.allFinite() && (input.rows() > 0) && (input.cols() > 0); }

    static bool validOutput(Output output, const Coefficients &c)
    {
        return output.allFinite() && (output.rows() >= c.factorVertical) && (output.cols() >= c.factorHorizontal);
    }
};

// upscale a 2D float array using linear interpolation
//
// author: Kristian Timm Andersen
class Upscale2DLinear : public AlgorithmImplementation<Upscale2DConfiguration, Upscale2DLinear>
{
  public:
    Upscale2DLinear(Coefficients c = Coefficients()) : BaseAlgorithm{c}
    {
        interpolateVertical = Eigen::VectorXf::LinSpaced(c.factorVertical, 1, 1.f / c.factorVertical);
        interpolateHorizontal = Eigen::VectorXf::LinSpaced(c.factorHorizontal, 1, 1.f / c.factorHorizontal);
    }

  private:
    void processAlgorithm(Input input, Output output)
    {
        const int cols = input.cols();
        const int colsm1 = cols - 1;
        const int rows = input.rows();
        const int rowsm1 = rows - 1;
        for (auto iCol = 0; iCol < cols; iCol++)
        {
            // linear interpolation across rows
            for (auto iRow = 0; iRow < rowsm1; iRow++)
            {
                for (auto i = 0; i < C.factorVertical; i++)
                {
                    output(i + iRow * C.factorVertical, iCol * C.factorHorizontal) =
                        (input(iRow, iCol) - input(iRow + 1, iCol)) * interpolateVertical(i) + input(iRow + 1, iCol);
                }
            }
            output(rowsm1 * C.factorVertical, iCol * C.factorHorizontal) = input(rowsm1, iCol);
        }

        // linear interpolation across first col
        for (auto iHor = 1; iHor < C.factorHorizontal; iHor++)
        {
            output.col(iHor) = output.col(0) * interpolateHorizontal(iHor);
        }
        for (auto iCol = 1; iCol < colsm1; iCol++)
        {
            for (auto iHor = 1; iHor < C.factorHorizontal; iHor++)
            {
                // linear interpolation across cols
                output.col(iCol * C.factorHorizontal + iHor) = output.col(iCol * C.factorHorizontal) * interpolateHorizontal(iHor);
                // backwards linear interpolation across cols
                output.col(iCol * C.factorHorizontal - iHor) += output.col(iCol * C.factorHorizontal + iHor);
            }
        }
        // backwards linear interpolation across last col
        for (auto iHor = 1; iHor < C.factorHorizontal; iHor++)
        {
            output.col((colsm1 - 1) * C.factorHorizontal + iHor) += output.col(colsm1 * C.factorHorizontal) * interpolateHorizontal(C.factorHorizontal - iHor);
        }
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = interpolateVertical.getDynamicMemorySize();
        size += interpolateHorizontal.getDynamicMemorySize();
        return size;
    }

    Eigen::VectorXf interpolateVertical;
    Eigen::VectorXf interpolateHorizontal;
    friend BaseAlgorithm;
};