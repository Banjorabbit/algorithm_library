#pragma once
#include "algorithm_library/interface/macros_json.h"
#include "algorithm_library/interface/public_algorithm.h"
#include "algorithm_library/interface/input_output.h"

template<typename Talgo, typename Tconfiguration>
struct Implementation : public Algorithm<Tconfiguration>::BaseImplementation
{
    using Base = typename Algorithm<Tconfiguration>::BaseImplementation;

    Implementation() : algo{} {}
    Implementation(const typename Tconfiguration::Coefficients& c) : algo{ c } {}
    Talgo algo;
    void process(typename Algorithm<Tconfiguration>::Input input, typename Algorithm<Tconfiguration>::Output output) override { algo.process(input, output); }
    typename Algorithm<Tconfiguration>::Coefficients getCoefficients() const final { return algo.getCoefficients(); }
    typename Algorithm<Tconfiguration>::Parameters getParameters() const final { return algo.getParameters(); }
    typename Algorithm<Tconfiguration>::Setup getSetup() const final { return algo.getSetup(); }
    void setCoefficients(const typename Algorithm<Tconfiguration>::Coefficients& c) final { algo.setCoefficients(c); }
    void setParameters(const typename Algorithm<Tconfiguration>::Parameters& p) final { algo.setParameters(p); }
    void setSetup(const typename Algorithm<Tconfiguration>::Setup& s) final { algo.setSetup(s); }
    void reset() override { algo.reset(); }
};

template<typename Talgo, typename Tconfiguration>
struct SingleBufferImplementation : public AlgorithmBuffer<Tconfiguration>::BufferBaseImplementation
{
    using Base = typename AlgorithmBuffer<Tconfiguration>::BufferBaseImplementation;

    SingleBufferImplementation() : algo{} {}
    SingleBufferImplementation(const typename Tconfiguration::Coefficients& c) : algo{ c } {}
    Talgo algo;
    void process(typename Algorithm<Tconfiguration>::Input input, typename Algorithm<Tconfiguration>::Output output) override { algo.process(input, output); }
    typename Algorithm<Tconfiguration>::Coefficients getCoefficients() const final { return algo.getCoefficients(); }
    typename Algorithm<Tconfiguration>::Parameters getParameters() const final { return algo.getParameters(); }
    typename Algorithm<Tconfiguration>::Setup getSetup() const final { return algo.getSetup(); }
    void setCoefficients(const typename Algorithm<Tconfiguration>::Coefficients& c) final { algo.setCoefficients(c); }
    void setParameters(const typename Algorithm<Tconfiguration>::Parameters& p) final { algo.setParameters(p); }
    void setSetup(const typename Algorithm<Tconfiguration>::Setup& s) final { algo.setSetup(s); }
    void reset() override { algo.reset(); }
    BufferMode getBufferMode() const final { return algo.getCoefficients().bufferMode; }
    int getBufferSize() const final { return algo.getCoefficients().bufferSize; }
    int getNChannels() const final { return algo.getCoefficients().nChannels; }
    int getDelaySamples() const override { return algo.getDelaySamples(); }
};


template<typename Talgo, typename Tconfiguration>
struct MultiBufferImplementation : public SingleBufferImplementation<Talgo, Tconfiguration>
{
    using Base = SingleBufferImplementation<Talgo, Tconfiguration>;
    
    MultiBufferImplementation() : MultiBufferImplementation(Algorithm<Tconfiguration>::Coefficients()) {}
    MultiBufferImplementation(const typename Tconfiguration::Coefficients& c) : Base{ c } { }

    void process(typename Algorithm<Tconfiguration>::Input input, typename Algorithm<Tconfiguration>::Output output) override 
    { 
        int i = 0;
        const int bufferSize = Base::getBufferSize();
        // Process as many full buffers as possible
        for (; i <= (input.rows() - bufferSize); i += bufferSize)
        {
            Base::algo.process(input.middleRows(i, bufferSize), output.middleRows(i, bufferSize));
        }
        // if we have been given a size that is not an integer multiple of bufferSize, zeropad and process. 
        const int remainingSamples = static_cast<int>(input.rows()) - i;
        if (remainingSamples > 0)
        {
            auto const c = Base::getCoefficients();
            typename I::getType<typename Algorithm<Tconfiguration>::Input>::type bufferIn(c.bufferSize, c.nChannels);
            bufferIn.topRows(remainingSamples) = input.middleRows(i, remainingSamples);
            bufferIn.bottomRows(c.bufferSize - remainingSamples).setZero();
            typename O::getType<typename Algorithm<Tconfiguration>::Output>::type bufferOut = Tconfiguration::initOutput(bufferIn, c);
            Base::algo.process(bufferIn, bufferOut);
            output.bottomRows(remainingSamples) = bufferOut.topRows(remainingSamples);
        }
    }
};

template<typename Talgo, typename Tconfiguration>
struct AsynchronousBufferImplementation : public MultiBufferImplementation<Talgo, Tconfiguration>
{
    using Base = MultiBufferImplementation<Talgo, Tconfiguration>;

    AsynchronousBufferImplementation() : AsynchronousBufferImplementation(Algorithm<Tconfiguration>::Coefficients()) {}
    AsynchronousBufferImplementation(const typename Tconfiguration::Coefficients& c) : Base{ c }
    { 
        index = 0; 
        bufferIn.resize(c.bufferSize, c.nChannels);
        bufferOut = Tconfiguration::initOutput(bufferIn, c);
        bufferIn.setZero();
        bufferOut.setZero();
    }

    void process(typename Algorithm<Tconfiguration>::Input input, typename Algorithm<Tconfiguration>::Output output) final 
    { 
        int const bufferSize = Base::getBufferSize();
        for (auto i = 0; i < input.rows(); i++)
        {
            bufferIn.row(index) = input.row(i);
            output.row(i) = bufferOut.row(index);
            index++;
            if (index == bufferSize)
            {
                Base::algo.process(bufferIn, bufferOut);
                index = 0;
            }
        }
    }

    void reset() final
    {
        index = 0;
        bufferIn.setZero(); 
        bufferOut.setZero(); 
        Base::reset();
    }

    // when processing asynchronously, and extra buffer is added to the delay in the process() method
    int getDelaySamples() const final { return Base::getDelaySamples() + Base::getBufferSize(); }

    int index;
    typename I::getType<typename Algorithm<Tconfiguration>::Input>::type bufferIn;
    typename O::getType<typename Algorithm<Tconfiguration>::Output>::type bufferOut;
};

template<typename Tconfiguration, typename Talgo>
class AlgorithmImplementation
{
public:
    using Configuration = Tconfiguration;
    using Input = const typename Configuration::Input&; // force inputs to be const reference
    using Output = typename Configuration::Output;
    using BaseAlgorithm = AlgorithmImplementation;
    using Coefficients = typename Configuration::Coefficients;
    using Parameters = typename Configuration::Parameters;
    using Setup = TSetup<Configuration>;

    static_assert(std::is_trivially_copyable<Coefficients>::value, "Coefficients data type must be trivially copyable.");
    static_assert(std::is_trivially_copyable<Parameters>::value, "Parameters data type must be trivially copyable.");

    AlgorithmImplementation() = default;
    AlgorithmImplementation(const Coefficients& c) : C(c) {}
    AlgorithmImplementation(const Setup& s) : C(s.coefficients), P(s.parameters) { }
    virtual ~AlgorithmImplementation() = default;
    
    size_t getStaticSize() const { return sizeof(Talgo); }

    size_t getDynamicSize() const
    {
        return getDynamicSizeVariables() + getDynamicSizeAlgorithms();
    }

    // Processing method. This is where the core of the algorithm is calculated.
    // When profiling using MSVC compiler it was found that CRTP is faster than virtual methods. 
    // However, using GCC it was found that virtual methods are as fast as CRTP (maybe because the virtual methods in header files can be inlined?).
    inline void process(Input input, Output output)
    {
        static_cast<Talgo&>(*this).processOn(input, output);
    }

    void reset()
    {
        resetVariables();
        resetAlgorithms();
    }

    static constexpr size_t ALGORITHM_VERSION_MINOR = 1; // version changes in implementation

    Coefficients getCoefficients() const { return C; }
    Parameters getParameters() const { return P; }
    Setup getSetup() const { return Setup{ getCoefficients(), getParameters() }; }

    // use SFINAE to call default constructor if Coefficients is empty
    template<typename T = Coefficients>
    typename std::enable_if<std::is_empty<T>::value>::type setCoefficients(const Coefficients& c) 
    {
        static_cast<Talgo&>(*this) = Talgo();
    }

    template<typename T = Coefficients>
    typename std::enable_if<!std::is_empty<T>::value>::type setCoefficients(const Coefficients& c)
    {
        static_cast<Talgo&>(*this) = Talgo(c);
    } 

    void setParameters(const Parameters& p) 
    { 
        P = p; 
        static_cast<Talgo&>(*this).onParametersChanged();
    }

    void setSetup(const Setup& setup) { setCoefficients(setup.coefficients); setParameters(setup.parameters); }

    // return type is deduced compile-time so can't be virtual
    auto getCoefficientsTree() const { return static_cast<Talgo const&>(*this).getCoefficientsTreeImpl(); }
    auto getParametersTree() const { return static_cast<Talgo const&>(*this).getParametersTreeImpl(); }
    auto getSetupTree() const { return static_cast<Talgo const&>(*this).getSetupTreeImpl(); }

    template<typename Tcoefficients>
    void setCoefficientsTree(const Tcoefficients& c) { static_cast<Talgo&>(*this).setCoefficientsTreeImpl(c); }
    
    template<typename Tparameters>
    void setParametersTree(const Tparameters& p) { static_cast<Talgo&>(*this).setParametersTreeImpl(p); }

    template<typename Tsetup>
    void setSetupTree(const Tsetup& s) { static_cast<Talgo&>(*this).setSetupTreeImpl(s); }

    auto validInput(Input input) const { return Configuration::validInput(input, C); }
    auto initOutput(Input input) const { return Configuration::initOutput(input, C); }

protected:
    // these functions will be overridden if defined in derived Talgo
    virtual size_t getDynamicSizeVariables() const { return 0; }
    virtual size_t getDynamicSizeAlgorithms() const { return 0; }
    virtual void resetVariables() { }
    virtual void resetAlgorithms() { }
    void onParametersChanged() {} // If more advanced functionality is needed, then write your own setters but remember to call the setters from this function.

    // these functions will be hidden if macro DEFINE_STATIC_MEMBER_ALGORITHMS(...) or DEFINE_SIMPLE_MEMBER_ALGORITHMS(...) is declared in derived Talgo
    Coefficients getCoefficientsTreeImpl() const { return getCoefficients(); }
    void setCoefficientsTreeImpl(const Coefficients& c) { setCoefficients(c); }
    Parameters getParametersTreeImpl() const { return getParameters(); }
    void setParametersTreeImpl(const Parameters& p) { setParameters(p); }
    Setup getSetupTreeImpl() const { return getSetup(); }
    void setSetupTreeImpl(const Setup& s) { setSetup(s); }

    Coefficients C;
    Parameters P;
};
