#include "preprocessing_path/beamformer_path.h"

template <>
Algorithm<PreprocessingPathConfiguration>::Algorithm(const Coefficients &c)
{
    pimpl = std::make_unique<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration>>(c);
}

template <>
void AlgorithmBuffer<PreprocessingPathConfiguration>::setBufferMode(BufferMode bufferMode)
{
    if (bufferMode == SYNCHRONOUS_BUFFER)
    {
        if (static_cast<BufferBaseImplementation *>(Base::pimpl.get())->getBufferMode() != SYNCHRONOUS_BUFFER)
        {
            auto cTree = static_cast<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration> *>(pimpl.get())->algo.getCoefficientsTree();
            auto pTree = static_cast<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration> *>(pimpl.get())->algo.getParametersTree();
            pimpl = std::make_unique<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration>>(cTree);
            static_cast<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration> *>(pimpl.get())->algo.setParametersTree(pTree);
        }
    }
    else
    {
        auto cTree = static_cast<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration> *>(pimpl.get())->algo.getCoefficientsTree();
        auto pTree = static_cast<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration> *>(pimpl.get())->algo.getParametersTree();
        pimpl = std::make_unique<AsynchronousBufferImplementation<BeamformerPath, PreprocessingPathConfiguration>>(cTree);
        static_cast<BufferImplementation<BeamformerPath, PreprocessingPathConfiguration> *>(pimpl.get())->algo.setParametersTree(pTree);
    }
}

PreprocessingPath::PreprocessingPath(const Coefficients &c) : AlgorithmBuffer<PreprocessingPathConfiguration>(c) {}
