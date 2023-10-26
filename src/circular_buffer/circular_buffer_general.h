#pragma once
#include "framework/framework.h"

// Circular buffer 
//
// author: Kristian Timm Andersen

struct CircularBufferSingleChannelConfiguration : public Configuration<I::Real, O::Real>
{
    struct Coefficients
    {
        int bufferLength = 100;
        DEFINE_TUNABLE_COEFFICIENTS(bufferLength)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input.size() > 0);
    }
    
	static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXf(input.size()); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;

        Test(const Coefficients& c = {}) : algo(c)
        {
            input = Eigen::ArrayXf(c.bufferLength);
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputFinite() const { return output.allFinite(); }
    };
};


class CircularBufferSingleChannel : public IAlgorithm<CircularBufferSingleChannelConfiguration, CircularBufferSingleChannel>
{
public:
    CircularBufferSingleChannel(Coefficients c =  Coefficients()) :
		IAlgorithm<CircularBufferSingleChannelConfiguration, CircularBufferSingleChannel>{ c }
	{
        buffer.resize(c.bufferLength);
        reset();
     }

    // push input values into buffer and write output values from buffer
    inline void processOn(Input input, Output output)
    {
        for (auto sample = 0; sample < input.rows(); sample++)
        {
            increment();
            output(sample) = buffer(index);
            buffer(index) = input(sample);
        }
    }

    inline void push(Input input)
    {
        for (auto sample = 0; sample < input.rows(); sample++)
        {
            increment();
            buffer(index) = input(sample);
        }
    }

    inline void pop(Output output)
    {
        for (auto sample = 0; sample < output.rows(); sample++)
        {
            output(sample) = buffer(index);
            decrement();
        }
    }

    inline float get(const int index) const
    {
        auto newIndex = this->index - index;
        if (newIndex < 0) { newIndex += C.bufferLength; }
        return buffer(newIndex);
    }

    inline float get(const float index) const
    {
        auto newIndex = this->index - index;
        if (newIndex < 0.f) { newIndex += C.bufferLength; }
        auto intIndex = static_cast<int>(newIndex);
        const auto remainder = newIndex - intIndex;
        float value = buffer(intIndex);
        intIndex++;
        if (intIndex == C.bufferLength) { intIndex = 0; }
        value += remainder * (buffer(intIndex) - value);
        return value;
    }

private:

    inline void increment() { index++; if (index >= C.bufferLength) { index = 0; } }
	inline void increment(const int increment) { index += increment; while (index >= C.bufferLength) { index -= C.bufferLength; } }
	inline void decrement() { index--; if (index < 0) { index = C.bufferLength - 1; } }
	inline void decrement(const int decrement) { index -= decrement; while (index < 0) { index += C.bufferLength; } }

    void resetMembers() final
    {
        buffer.setZero();
        index = C.bufferLength - 1; // when pushing values, we first increment index and then write value
        
    }

    size_t getMembersDynamicSize() const final
	{
		size_t size = buffer.getDynamicMemorySize();
		return size;
	}

    Eigen::ArrayXf buffer;
    int index;
};



struct CircularBufferConfiguration : public Configuration<I::Real2D, O::Real2D>
{
    struct Coefficients
    {
        int nChannels = 2;
        int bufferLength = 100;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, bufferLength)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input.rows() > 0) && (input.cols() == c.nChannels);
    }
    
	static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(input.rows(), c.nChannels); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;

        Test(const Coefficients& c = {}) : algo(c)
        {
            input = Eigen::ArrayXXf(c.bufferLength, c.nChannels);
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputFinite() const { return output.allFinite(); }
    };
};


class CircularBufferGeneral : public IAlgorithm<CircularBufferConfiguration, CircularBufferGeneral>
{
public:
    CircularBufferGeneral(Coefficients c =  Coefficients()) :
		IAlgorithm<CircularBufferConfiguration, CircularBufferGeneral>{ c }
	{
        buffer.resize(c.bufferLength, c.nChannels);
        reset();
     }

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
        if (newIndex < 0) { newIndex += C.bufferLength; }
        return buffer.row(newIndex);
    }

    inline Eigen::ArrayXf get(const float index) const
    {
        auto newIndex = this->index - index;
        if (newIndex < 0.f) { newIndex += C.bufferLength; }
        auto intIndex = static_cast<int>(newIndex);
        const auto remainder = newIndex - intIndex;
        Eigen::ArrayXf value = buffer.row(intIndex).transpose();
        intIndex++;
        if (intIndex == C.bufferLength) { intIndex = 0; }
        value += remainder * (buffer.row(intIndex).transpose() - value);
        return value;
    }

private:

    inline void increment() { index++; if (index >= C.bufferLength) { index = 0; } }
	inline void increment(const int increment) { index += increment; while (index >= C.bufferLength) { index -= C.bufferLength; } }
	inline void decrement() { index--; if (index < 0) { index = C.bufferLength - 1; } }
	inline void decrement(const int decrement) { index -= decrement; while (index < 0) { index += C.bufferLength; } }

    void resetMembers() final
    {
        buffer.setZero();
        index = C.bufferLength - 1; // when pushing values, we first increment index and then write value
        
    }

    size_t getMembersDynamicSize() const final
	{
		size_t size = buffer.getDynamicMemorySize();
		return size;
	}

    Eigen::ArrayXXf buffer;
    int index;
};
