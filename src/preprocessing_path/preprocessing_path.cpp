#include "preprocessing_path/beamformer_path.h"

using BeamformerPathSingleBufferImpl = SingleBufferImplementation<BeamformerPath, PreprocessingPathConfiguration>;
using BeamformerPathMultiBufferImpl = MultiBufferImplementation<BeamformerPath, PreprocessingPathConfiguration>;
using BeamformerPathAsynchronousBufferImpl = AsynchronousBufferImplementation<BeamformerPath, PreprocessingPathConfiguration>;

template<>
Algorithm<PreprocessingPathConfiguration>::Algorithm(const Coefficients& c) 
{ 
    switch (c.bufferMode)
    {
        case SINGLE_BUFFER:
            pimpl = std::make_unique<BeamformerPathSingleBufferImpl>(c); 
            break;
        case MULTI_BUFFER:
            pimpl = std::make_unique<BeamformerPathMultiBufferImpl>(c); 
            break;
        case ASYNCHRONOUS_BUFFER:
            pimpl = std::make_unique<BeamformerPathAsynchronousBufferImpl>(c); 
            break;
    }
} 

PreprocessingPath::PreprocessingPath(const Coefficients& c) : AlgorithmBuffer<PreprocessingPathConfiguration>(c) {}
