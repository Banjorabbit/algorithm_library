#pragma once
#include "algorithm_library/delay.h"
#include "framework/framework.h"

// Circular buffer. There is a single channel version and a multi-channel version.
//
// author: Kristian Timm Andersen

// Single channel version of circular buffer that is faster than the multi-channel version
class CircularBufferSingleChannel : public AlgorithmImplementation<DelayConfiguration, CircularBufferSingleChannel>
{
  public:
    CircularBufferSingleChannel(Coefficients c = Coefficients()) : BaseAlgorithm{c}
    {
        C.nChannels = 1; // force number of channels to 1 since this is the single channel version
        buffer.resize(c.delayLength);
        resetVariables();
    }

    inline void push(Input input)
    {
        for (auto sample = 0; sample < input.rows(); sample++)
        {
            increment();
            buffer(index) = input(sample, 0);
        }
    }

    inline void pop(Output output)
    {
        for (auto sample = 0; sample < output.rows(); sample++)
        {
            output(sample, 0) = buffer(index);
            decrement();
        }
    }

    inline float get(const int index) const
    {
        auto newIndex = this->index - index;
        if (newIndex < 0) { newIndex += C.delayLength; }
        return buffer(newIndex);
    }

    inline float get(const float index) const
    {
        auto newIndex = this->index - index;
        if (newIndex < 0.f) { newIndex += C.delayLength; }
        auto intIndex = static_cast<int>(newIndex);
        const auto remainder = newIndex - intIndex;
        float value = buffer(intIndex);
        intIndex++;
        if (intIndex == C.delayLength) { intIndex = 0; }
        value += remainder * (buffer(intIndex) - value);
        return value;
    }

  private:
    // push input values into buffer and write output values from buffer
    inline void processOn(Input input, Output output)
    {
        for (auto sample = 0; sample < input.rows(); sample++)
        {
            increment();
            output(sample, 0) = buffer(index);
            buffer(index) = input(sample, 0);
        }
    }

    inline void increment()
    {
        index++;
        if (index >= C.delayLength) { index = 0; }
    }
    inline void increment(const int increment)
    {
        index += increment;
        while (index >= C.delayLength)
        {
            index -= C.delayLength;
        }
    }
    inline void decrement()
    {
        index--;
        if (index < 0) { index = C.delayLength - 1; }
    }
    inline void decrement(const int decrement)
    {
        index -= decrement;
        while (index < 0)
        {
            index += C.delayLength;
        }
    }

    void resetVariables() final
    {
        buffer.setZero();
        index = C.delayLength - 1; // when pushing values, we first increment index and then write value
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = buffer.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXf buffer;
    int index;

    friend BaseAlgorithm;
};

class CircularBuffer : public AlgorithmImplementation<DelayConfiguration, CircularBuffer>
{
  public:
    CircularBuffer(Coefficients c = Coefficients()) : BaseAlgorithm{c}
    {
        buffer.resize(c.delayLength, c.nChannels);
        reset();
    }

    inline void push(Input input)
    {
        for (auto sample = 0; sample < input.rows(); sample++)
        {
            increment();
            buffer.row(index) = input.row(sample);
        }
    }

    inline void pop(Output output)
    {
        for (auto sample = 0; sample < output.rows(); sample++)
        {
            output.row(sample) = buffer.row(index);
            decrement();
        }
    }

    inline Eigen::ArrayXf get(const int index) const
    {
        auto newIndex = this->index - index;
        if (newIndex < 0) { newIndex += C.delayLength; }
        return buffer.row(newIndex);
    }

    inline Eigen::ArrayXf get(const float index) const
    {
        auto newIndex = this->index - index;
        if (newIndex < 0.f) { newIndex += C.delayLength; }
        auto intIndex = static_cast<int>(newIndex);
        const auto remainder = newIndex - intIndex;
        Eigen::ArrayXf value = buffer.row(intIndex).transpose();
        intIndex++;
        if (intIndex == C.delayLength) { intIndex = 0; }
        value += remainder * (buffer.row(intIndex).transpose() - value);
        return value;
    }

  private:
    // push input values into buffer and write output values from buffer
    inline void processOn(Input input, Output output)
    {
        for (auto sample = 0; sample < input.rows(); sample++)
        {
            increment();
            output.row(sample) = buffer.row(index);
            buffer.row(index) = input.row(sample);
        }
    }

    inline void increment()
    {
        index++;
        if (index >= C.delayLength) { index = 0; }
    }
    inline void increment(const int increment)
    {
        index += increment;
        while (index >= C.delayLength)
        {
            index -= C.delayLength;
        }
    }
    inline void decrement()
    {
        index--;
        if (index < 0) { index = C.delayLength - 1; }
    }
    inline void decrement(const int decrement)
    {
        index -= decrement;
        while (index < 0)
        {
            index += C.delayLength;
        }
    }

    void resetVariables() final
    {
        buffer.setZero();
        index = C.delayLength - 1; // when pushing values, we first increment index and then write value
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = buffer.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXXf buffer;
    int index;

    friend BaseAlgorithm;
};
