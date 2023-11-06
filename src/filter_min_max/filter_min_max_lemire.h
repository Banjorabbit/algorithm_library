#pragma once
#include "framework/framework.h"
#include "algorithm_library/filter_min_max.h"

using namespace Eigen;

// StreamingMinMax finds the minimum and maximum value over a
// window with length C.Length for each new sample. It requires on
// average no more than 3 comparisons per sample. The algorithm uses 2
// double-ended queues for the minimum and maximum indices. A delay line
// is also used internally since in a true streaming application you need 
// to be able to call the algorithm succesively with new frames (or just 1 new sample),
// and you are not guaranteed that the input frame is as long as C.Length. 
// To be able to preallocate, the queues have been implemented as circular buffers. 
//
// A symmetric version of StreamingMinMax has been implemented below called "Filter". 
// It might be necessary to call the public ResetInitialValues function before Process, 
// if certain initial conditions are required. Also versions that only find Min/Max have been implemented.
//
// ref: Daniel Lemire, STREAMING MAXIMUM - MINIMUM FILTER USING NO MORE THAN THREE COMPARISONS PER ELEMENT
//
// author : Kristian Timm Andersen


class StreamingMinMaxLemire : public AlgorithmImplementation<StreamingMinMaxConfiguration, StreamingMinMaxLemire>
{
public:
    StreamingMinMaxLemire(Coefficients c = Coefficients()) :
        AlgorithmImplementation<StreamingMinMaxConfiguration, StreamingMinMaxLemire>{ c }
    {
        minIndex.resize(C.filterLength, C.nChannels);
        maxIndex.resize(C.filterLength, C.nChannels);
        maxValue.resize(C.filterLength, C.nChannels);
        minValue.resize(C.filterLength, C.nChannels);
        inputOld.resize(C.nChannels);
        upperFrontIndex.resize(C.nChannels);
        upperEndIndex.resize(C.nChannels);
        lowerFrontIndex.resize(C.nChannels);
        lowerEndIndex.resize(C.nChannels);
        resetVariables();
    }

    void processOn(Input input, Output output)
    {
        for (auto channel = 0; channel < C.nChannels; channel++)
        {
            int uf = upperFrontIndex(channel);
            int ue = upperEndIndex(channel);
            int lf = lowerFrontIndex(channel);
            int le = lowerEndIndex(channel);
            for (auto i = 0; i < input.rows(); i++)
            {
                if (input(i, channel) > inputOld(channel))
                {
                    minIndex(le, channel) = i - 1;
                    minValue(le, channel) = inputOld(channel);
                    le += 1;
                    if (le == C.filterLength) { le = 0; }
                    if (i == C.filterLength + minIndex(lf, channel))
                    {
                        lf += 1;
                        if (lf == C.filterLength) { lf = 0; }
                    }
                    while (uf != ue)
                    {
                        auto up = ue - 1;
                        if (up < 0) { up = C.filterLength - 1; }
                        if (input(i, channel) <= maxValue(up, channel))
                        {
                            if (i == C.filterLength + maxIndex(uf, channel))
                            {
                                uf += 1;
                                if (uf == C.filterLength) { uf = 0; }
                            }
                            break;
                        }
                        ue = up;
                    }
                }
                else
                {
                    maxIndex(ue, channel) = i - 1;
                    maxValue(ue, channel) = inputOld(channel);
                    ue = (ue + 1) % C.filterLength;
                    if (i == C.filterLength + maxIndex(uf, channel))
                    {
                        uf += 1;
                        if (uf == C.filterLength) { uf = 0; }
                    }
                    while (lf != le)
                    {
                        auto lp = le - 1;
                        if (lp < 0) { lp = C.filterLength - 1; }
                        if (input(i, channel) >= minValue(lp, channel))
                        {
                            if (i == C.filterLength + minIndex(lf, channel))
                            {
                                lf += 1;
                                if (lf == C.filterLength) { lf = 0; }
                            }
                            break;
                        }
                        le = lp;
                    }
                }
                if (uf == ue) { output.maxValue(i, channel) = input(i, channel); }
                else { output.maxValue(i, channel) = maxValue(uf, channel); }
                if (lf == le) { output.minValue(i, channel) = input(i, channel); }
                else { output.minValue(i, channel) = minValue(lf, channel); }
                inputOld(channel) = input(i, channel);
            }
            upperFrontIndex(channel) = uf;
            upperEndIndex(channel) = ue;
            lowerFrontIndex(channel) = lf;
            lowerEndIndex(channel) = le;
        }
        // update Index to allow next Process() to continue as if they are successive frames
        maxIndex -= static_cast<int>(input.rows());
        minIndex -= static_cast<int>(input.rows());
    }

    void resetInitialValue(const float iOld) 
    { 
        reset(); 
        inputOld.setConstant(iOld); 
    }

    void resetInitialValue(I::Real iOld)
    { 
        if (iOld.size() == inputOld.size())
        {
            reset();
            inputOld = iOld; 
        }
    }

private:

    size_t getDynamicSizeVariables() const final
    {
        auto size = minIndex.getDynamicMemorySize();
        size += maxIndex.getDynamicMemorySize();
        size += maxValue.getDynamicMemorySize();
        size += minValue.getDynamicMemorySize();
        size += inputOld.getDynamicMemorySize();
        size += upperFrontIndex.getDynamicMemorySize();
        size += upperEndIndex.getDynamicMemorySize();
        size += lowerFrontIndex.getDynamicMemorySize();
        size += lowerEndIndex.getDynamicMemorySize();
        return size;
    }

    void resetVariables() final 
    {
        minIndex.setZero();
        maxIndex.setZero();
        maxValue.setZero();
        minValue.setZero();
        inputOld.setZero();
        upperFrontIndex.setZero();
        upperEndIndex.setZero();
        lowerFrontIndex.setZero();
        lowerEndIndex.setZero();
    }

    ArrayXXi minIndex;
    ArrayXXi maxIndex;
    ArrayXXf maxValue;
    ArrayXXf minValue;
    ArrayXf inputOld;
    ArrayXi upperFrontIndex, upperEndIndex, lowerFrontIndex, lowerEndIndex;
};

class FilterMinMaxLemire : public AlgorithmImplementation<FilterMinMaxConfiguration, FilterMinMaxLemire>
{
public:
    FilterMinMaxLemire(Coefficients c = Coefficients()) :
        AlgorithmImplementation<FilterMinMaxConfiguration, FilterMinMaxLemire>{ c },
        streaming{c}
    {
        wHalf = (c.filterLength - 1) / 2;
    }

    StreamingMinMaxLemire streaming;
    DEFINE_MEMBER_ALGORITHMS(streaming)

    void processOn(Input input, Output output)
    {
        streaming.resetInitialValue(input.row(0).transpose());
        // this output is discarded and is only used to update internal values
        streaming.process(input.topRows(wHalf), { output.minValue.topRows(wHalf), output.maxValue.topRows(wHalf) });
        // This is the shifted streaming filter, which creates a symmetric window
        ArrayXXf xSymmetric(input.rows(), input.cols());
        xSymmetric.topRows(input.rows() - wHalf) = input.bottomRows(input.rows() - wHalf);
        xSymmetric.bottomRows(wHalf) = input.bottomRows<1>().replicate(wHalf, 1);
        streaming.process(xSymmetric, output);
    }

    void resetInitialValue(const float iOld) { streaming.resetInitialValue(iOld); }
    void resetInitialValue(I::Real iOld) { streaming.resetInitialValue(iOld); }

private:

    int wHalf;
};

class StreamingMaxLemire : public AlgorithmImplementation<StreamingMaxConfiguration, StreamingMaxLemire>
{
public:
    StreamingMaxLemire(Coefficients c = Coefficients()) :
        AlgorithmImplementation<StreamingMaxConfiguration, StreamingMaxLemire>{ c }
    {
        maxIndex.resize(C.filterLength, C.nChannels);
        maxValue.resize(C.filterLength, C.nChannels);
        inputOld.resize(C.nChannels);
        upperFrontIndex.resize(C.nChannels);
        upperEndIndex.resize(C.nChannels);
        resetVariables();
    }

    void processOn(Input input, Output output)
    {
        for (auto channel = 0; channel < C.nChannels; channel++)
        {
            int uf = upperFrontIndex(channel);
            int ue = upperEndIndex(channel);
            for (auto i = 0; i < input.rows(); i++)
            {
                if (input(i, channel) > inputOld(channel))
                {
                    while (uf != ue)
                    {
                        auto up = ue - 1;
                        if (up < 0) { up = C.filterLength - 1; }
                        if (input(i, channel) <= maxValue(up, channel))
                        {
                            if (i == C.filterLength + maxIndex(uf, channel))
                            {
                                uf += 1;
                                if (uf == C.filterLength) { uf = 0; }
                            }
                            break;
                        }
                        ue = up;
                    }
                }
                else
                {
                    maxIndex(ue, channel) = i - 1;
                    maxValue(ue, channel) = inputOld(channel);
                    ue = (ue + 1) % C.filterLength;
                    if (i == C.filterLength + maxIndex(uf, channel))
                    {
                        uf += 1;
                        if (uf == C.filterLength) { uf = 0; }
                    }
                }
                if (uf == ue) { output(i, channel) = input(i, channel); }
                else { output(i, channel) = maxValue(uf, channel); }
                inputOld(channel) = input(i, channel);
            }
            upperFrontIndex(channel) = uf;
           upperEndIndex(channel) = ue;
        }
        // update Index to allow next Process() to continue as if they are successive frames
        maxIndex -= static_cast<int>(input.rows());
    }

    void resetInitialValue(const float iOld) 
    { 
        reset(); 
        inputOld.setConstant(iOld); 
    }

    void resetInitialValue(I::Real iOld)
    { 
        if (iOld.size() == inputOld.size())
        {
            reset();
            inputOld = iOld; 
        }
    }

private:

    size_t getDynamicSizeVariables() const final
    {
        auto size = maxIndex.getDynamicMemorySize();
        size += maxValue.getDynamicMemorySize();
        size += inputOld.getDynamicMemorySize();
        size += upperFrontIndex.getDynamicMemorySize();
        size += upperEndIndex.getDynamicMemorySize();
        return size;
    }

    void resetVariables() final 
    {
        maxIndex.setZero();
        maxValue.setZero();
        inputOld.setZero();
        upperFrontIndex.setZero();
        upperEndIndex.setZero();
    }

    ArrayXXi maxIndex;
    ArrayXXf maxValue;
    ArrayXf inputOld;
    ArrayXi upperFrontIndex, upperEndIndex;
};

class StreamingMinLemire : public AlgorithmImplementation<StreamingMinConfiguration, StreamingMinLemire>
{
public:
    StreamingMinLemire(Coefficients c = Coefficients()) :
        AlgorithmImplementation<StreamingMinConfiguration, StreamingMinLemire>{ c }
    {
        minIndex.resize(C.filterLength, C.nChannels);
        minValue.resize(C.filterLength, C.nChannels);
        inputOld.resize(C.nChannels);
        lowerFrontIndex.resize(C.nChannels);
        lowerEndIndex.resize(C.nChannels);
        resetVariables();
    }

    void processOn(Input input, Output output)
    {
        for (auto channel = 0; channel < C.nChannels; channel++)
        {
            int uf = lowerFrontIndex(channel);
            int ue = lowerEndIndex(channel);
            for (auto i = 0; i < input.rows(); i++)
            {
                if (input(i, channel) > inputOld(channel))
                {
                    minIndex(ue, channel) = i - 1;
                    minValue(ue, channel) = inputOld(channel);
                    ue += 1;
                    if (ue == C.filterLength) { ue = 0; }
                    if (i == C.filterLength + minIndex(uf, channel))
                    {
                        uf += 1;
                        if (uf == C.filterLength) { uf = 0; }
                    }
                }
                else
                {
                    while (uf != ue)
                    {
                        auto lp = ue - 1;
                        if (lp < 0) { lp = C.filterLength - 1; }
                        if (input(i, channel) >= minValue(lp, channel))
                        {
                            if (i == C.filterLength + minIndex(uf, channel))
                            {
                                uf += 1;
                                if (uf == C.filterLength) { uf = 0; }
                            }
                            break;
                        }
                        ue = lp;
                    }
                }
                if (uf == ue) { output(i, channel) = input(i, channel); }
                else { output(i, channel) = minValue(uf, channel); }
                inputOld(channel) = input(i, channel);
            }
            lowerFrontIndex(channel) = uf;
            lowerEndIndex(channel) = ue;
        }
        // update Index to allow next Process() to continue as if they are successive frames
        minIndex -= static_cast<int>(input.rows());
    }

    void resetInitialValue(const float iOld) 
    { 
        reset(); 
        inputOld.setConstant(iOld); 
    }

    void resetInitialValue(I::Real iOld)
    { 
        if (iOld.size() == inputOld.size())
        {
            reset();
            inputOld = iOld; 
        }
    }

private:

    size_t getDynamicSizeVariables() const final
    {
        auto size = minIndex.getDynamicMemorySize();
        size += minValue.getDynamicMemorySize();
        size += inputOld.getDynamicMemorySize();
        size += lowerFrontIndex.getDynamicMemorySize();
        size += lowerEndIndex.getDynamicMemorySize();
        return size;
    }

    void resetVariables() final 
    {
        minIndex.setZero();
        minValue.setZero();
        inputOld.setZero();
        lowerFrontIndex.setZero();
        lowerEndIndex.setZero();
    }

    ArrayXXi minIndex;
    ArrayXXf minValue;
    ArrayXf inputOld;
    ArrayXi lowerFrontIndex, lowerEndIndex;
};

class FilterMaxLemire : public AlgorithmImplementation<FilterMaxConfiguration, FilterMaxLemire>
{
public:
    FilterMaxLemire(Coefficients c = Coefficients()) :
        AlgorithmImplementation<FilterMaxConfiguration, FilterMaxLemire>{ c },
        streaming{c}
    {
        wHalf = (c.filterLength - 1) / 2;
    }

    StreamingMaxLemire streaming;
    DEFINE_MEMBER_ALGORITHMS(streaming)

    void processOn(Input input, Output output)
    {
        streaming.resetInitialValue(input.row(0).transpose());
        // this output is discarded and is only used to update internal values
        streaming.process(input.topRows(wHalf), output.topRows(wHalf));
        // This is the shifted streaming filter, which creates a symmetric window
        ArrayXXf xSymmetric(input.rows(), input.cols());
        xSymmetric.topRows(input.rows() - wHalf) = input.bottomRows(input.rows() - wHalf);
        xSymmetric.bottomRows(wHalf) = input.bottomRows<1>().replicate(wHalf, 1);
        streaming.process(xSymmetric, output);
    }

    void resetInitialValue(const float iOld) { streaming.resetInitialValue(iOld); }
    void resetInitialValue(I::Real iOld) { streaming.resetInitialValue(iOld); }

private:

    int wHalf;
};

class FilterMinLemire : public AlgorithmImplementation<FilterMinConfiguration, FilterMinLemire>
{
public:
    FilterMinLemire(Coefficients c = Coefficients()) :
        AlgorithmImplementation<FilterMinConfiguration, FilterMinLemire>{ c },
        streaming{c}
    {
        wHalf = (c.filterLength - 1) / 2;
    }

    StreamingMinLemire streaming;
    DEFINE_MEMBER_ALGORITHMS(streaming)

    void processOn(Input input, Output output)
    {
        streaming.resetInitialValue(input.row(0).transpose());
        // this output is discarded and is only used to update internal values
        streaming.process(input.topRows(wHalf), output.topRows(wHalf));
        // This is the shifted streaming filter, which creates a symmetric window
        ArrayXXf xSymmetric(input.rows(), input.cols());
        xSymmetric.topRows(input.rows() - wHalf) = input.bottomRows(input.rows() - wHalf);
        xSymmetric.bottomRows(wHalf) = input.bottomRows<1>().replicate(wHalf, 1);
        streaming.process(xSymmetric, output);
    }

    void resetInitialValue(const float iOld) { streaming.resetInitialValue(iOld); }
    void resetInitialValue(I::Real iOld) { streaming.resetInitialValue(iOld); }

private:

    int wHalf;
};