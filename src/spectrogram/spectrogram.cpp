#include "spectrogram/spectrogram_filterbank.h"
#include "spectrogram/spectrogram_nonlinear.h"

// resolve type of spectrogram algorithm in SpectrogramConfiguration
using SpectrogramType = std::conditional_t<SpectrogramConfiguration::spectrogramType == SpectrogramConfiguration::FILTERBANK, SpectrogramFilterbank, SpectrogramNonlinear>;

template<> 
struct Algorithm<SpectrogramConfiguration>::Impl 
{ 
    virtual void process(Input input, Output output) = 0;
    virtual Coefficients getCoefficients() const = 0;
    virtual Parameters getParameters() const = 0;
    virtual Setup getSetup() const = 0;
    virtual void setCoefficients(const Coefficients& c) = 0;
    virtual void setParameters(const Parameters& p) = 0;
    virtual void setSetup(const Setup& s) = 0;
    virtual void reset() = 0;
    virtual void setWindow(I::Real window) = 0;
};

struct ImplFilterbank : public Algorithm<SpectrogramConfiguration>::Impl
{
    ImplFilterbank() : algo{} {}
    ImplFilterbank(const SpectrogramConfiguration::Coefficients& c) : algo{ c } {}
    virtual ~ImplFilterbank() {}
    SpectrogramFilterbank algo;
    void process(Algorithm<SpectrogramConfiguration>::Input input, Algorithm<SpectrogramConfiguration>::Output output) override { algo.process(input, output); }
    Algorithm<SpectrogramConfiguration>::Coefficients getCoefficients() const override { return algo.getCoefficients(); }
    Algorithm<SpectrogramConfiguration>::Parameters getParameters() const override { return algo.getParameters(); }
    Algorithm<SpectrogramConfiguration>::Setup getSetup() const override { return algo.getSetup(); }
    void setCoefficients(const Algorithm<SpectrogramConfiguration>::Coefficients& c) override { algo.setCoefficients(c); }
    void setParameters(const Algorithm<SpectrogramConfiguration>::Parameters& p) override { algo.setParameters(p); }
    void setSetup(const Algorithm<SpectrogramConfiguration>::Setup& s) override { algo.setSetup(s); }
    void reset() override { algo.reset(); }
    void setWindow(I::Real window) override { algo.setWindow(window); }
};

struct ImplNonlinear : public Algorithm<SpectrogramConfiguration>::Impl
{
    SpectrogramNonlinear algo;
    void process(Algorithm<SpectrogramConfiguration>::Input input, Algorithm<SpectrogramConfiguration>::Output output) override { algo.process(input, output); }
    Algorithm<SpectrogramConfiguration>::Coefficients getCoefficients() const override { return algo.getCoefficients(); }
    Algorithm<SpectrogramConfiguration>::Parameters getParameters() const override { return algo.getParameters(); }
    Algorithm<SpectrogramConfiguration>::Setup getSetup() const override { return algo.getSetup(); }
    void setCoefficients(const Algorithm<SpectrogramConfiguration>::Coefficients& c) override { algo.setCoefficients(c); }
    void setParameters(const Algorithm<SpectrogramConfiguration>::Parameters& p) override { algo.setParameters(p); }
    void setSetup(const Algorithm<SpectrogramConfiguration>::Setup& s) override { algo.setSetup(s); }
    void reset() override { algo.reset(); }
    void setWindow(I::Real window) override { algo.setWindow(window); }
};

template<> 
void Algorithm<SpectrogramConfiguration>::process(Input input, Output output) { pimpl->process(input, output); } 

template<> 
Algorithm<SpectrogramConfiguration>::Algorithm() : pimpl{ std::make_unique<ImplFilterbank>() } {} 

template<> 
Algorithm<SpectrogramConfiguration>::~Algorithm() {} 

template<> template<typename T> 
Algorithm<SpectrogramConfiguration>::Algorithm(const T& t) : pimpl{ std::make_unique<ImplFilterbank>(t) } {} 

template<> 
Algorithm<SpectrogramConfiguration>::Coefficients Algorithm<SpectrogramConfiguration>::getCoefficients() const { return pimpl->getCoefficients(); } 

template<> 
Algorithm<SpectrogramConfiguration>::Parameters Algorithm<SpectrogramConfiguration>::getParameters() const { return pimpl->getParameters(); } 

template<> 
Algorithm<SpectrogramConfiguration>::Setup Algorithm<SpectrogramConfiguration>::getSetup() const { return pimpl->getSetup(); } 

template<> 
void Algorithm<SpectrogramConfiguration>::setCoefficients(const Coefficients& c) { pimpl->setCoefficients(c); } 

template<> 
void Algorithm<SpectrogramConfiguration>::setParameters(const Parameters& p) { pimpl->setParameters(p); } 

template<> 
void Algorithm<SpectrogramConfiguration>::setSetup(const Setup& s) { pimpl->setSetup(s); } 

template<> 
void Algorithm<SpectrogramConfiguration>::reset() { pimpl->reset(); }

Spectrogram::Spectrogram(const Coefficients& c) : Algorithm<SpectrogramConfiguration>(c) {}

void Spectrogram::setWindow(I::Real window)
{
    pimpl->setWindow(window);
}

int Spectrogram::getNFrames(int inputSize, int bufferSize) 
{
    return SpectrogramType::getNFrames(inputSize, bufferSize);
}

int Spectrogram::getValidFFTSize(int fftSize)
{
    return FFTReal::getValidFFTSize(fftSize);
}