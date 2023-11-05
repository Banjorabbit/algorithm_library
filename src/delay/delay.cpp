#include "delay/circular_buffer.h"

using CircularBufferSingleChannelImpl = Implementation<CircularBufferSingleChannel, DelayConfiguration>;
using CircularBufferImpl = Implementation<CircularBuffer, DelayConfiguration>;

template<> 
Algorithm<DelayConfiguration>::~Algorithm() {} 

template<>
Algorithm<DelayConfiguration>::Algorithm(const Coefficients& c) 
{
    if (c.nChannels == 1)
    {
        pimpl = std::make_unique<CircularBufferSingleChannelImpl>(c);
    }
    else
    {
        pimpl = std::make_unique<CircularBufferImpl>(c);
    }
} 

Delay::Delay(const Coefficients& c) : Algorithm<DelayConfiguration>(c) {}
