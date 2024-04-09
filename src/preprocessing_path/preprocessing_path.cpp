#include "preprocessing_path/beamformer_path.h"

template<>
Algorithm<PreprocessingPathConfiguration>::Algorithm(const Coefficients& c) 
{ 
    pimpl = std::make_unique<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration>>(c);
} 

template<>
void AlgorithmBuffer<PreprocessingPathConfiguration>::setBufferMode(BufferMode bufferMode)
{
    if (bufferMode == MULTI_BUFFER)
    {
        if (static_cast<BufferBaseImplementation*>(Base::pimpl.get())->getBufferMode() != MULTI_BUFFER)
        {
            auto c = pimpl->getCoefficients();
            pimpl = std::make_unique<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration>>(c);
        }
    }
    else
    {
        auto c = pimpl->getCoefficients();
        pimpl = std::make_unique<AsynchronousBufferImplementation<BeamformerPath, PreprocessingPathConfiguration>>(c);
    }
}

PreprocessingPath::PreprocessingPath(const Coefficients& c) : AlgorithmBuffer<PreprocessingPathConfiguration>(c) {}
