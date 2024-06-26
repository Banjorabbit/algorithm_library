#pragma once
#include "input_output.h"
#include "macros_json.h"

template <typename Tconfiguration>
struct TSetup
{
    using Coefficients = typename Tconfiguration::Coefficients;
    using Parameters = typename Tconfiguration::Parameters;

    TSetup() = default;
    TSetup(const Coefficients &c, const Parameters &p)
    {
        coefficients = c;
        parameters = p;
    }
    Coefficients coefficients;
    Parameters parameters;
    DEFINE_TUNABLE_SETUP(coefficients, parameters)
};

template <typename Tconfiguration>
class Algorithm
{
  public:
    using Configuration = Tconfiguration;
    using Input = const typename Configuration::Input &; // force inputs to be const reference
    using Output = typename Configuration::Output;
    using Coefficients = typename Configuration::Coefficients;
    using Parameters = typename Configuration::Parameters;
    using Setup = TSetup<Configuration>;

    Algorithm() : Algorithm(Coefficients()) {} // default constructor
    Algorithm(const Coefficients &c);          // define this in derived source file.

    void process(Input input, Output output) { pimpl->process(input, output); }
    Coefficients getCoefficients() const { return pimpl->getCoefficients(); }
    Parameters getParameters() const { return pimpl->getParameters(); }
    Setup getSetup() const { return pimpl->getSetup(); }
    void reset() { pimpl->reset(); }

    void setCoefficients(const Coefficients &c) { pimpl->setCoefficients(c); }
    void setParameters(const Parameters &p) { pimpl->setParameters(p); }
    void setSetup(const Setup &s) { pimpl->setSetup(s); }

    nlohmann::json getDebugJson() const { return pimpl->getDebugJson(); }
    void setDebugJson(const nlohmann::json &s) { pimpl->setDebugJson(s); }

    auto validInput(Input input) const { return Configuration::validInput(input, getCoefficients()); }
    auto validOutput(Output output) const { return Configuration::validOutput(output, getCoefficients()); }
    auto initInput() const { return Configuration::initInput(getCoefficients()); }
    auto initOutput(Input input) const { return Configuration::initOutput(input, getCoefficients()); }

    bool isConfigurationValid() const { return pimpl->isConfigurationValid(); } // if this returns false, then behaviour of algorithm is undefined

    static constexpr size_t ALGORITHM_VERSION_MAJOR = 1; // version changes in ABI

    struct BaseImplementation // Base of implementation. Allows to derive different implementations from this struct.
    {
        BaseImplementation() = default;
        virtual ~BaseImplementation() = default;
        virtual void process(Input input, Output output) = 0;
        virtual Coefficients getCoefficients() const = 0;
        virtual Parameters getParameters() const = 0;
        virtual Setup getSetup() const = 0;
        virtual void setCoefficients(const Coefficients &c) = 0;
        virtual void setParameters(const Parameters &p) = 0;
        virtual void setSetup(const Setup &s) = 0;
        virtual void reset() = 0;
        virtual nlohmann::json getDebugJson() const = 0;
        virtual void setDebugJson(const nlohmann::json &s) = 0;
        virtual bool isConfigurationValid() const = 0;
    };

  protected:
    ~Algorithm() = default;

    std::unique_ptr<BaseImplementation> pimpl; // PIMPL. Define in derived source file
};

enum BufferMode { SYNCHRONOUS_BUFFER, ASYNCHRONOUS_BUFFER };

// AlgorithmBuffer<Tconfiguration> is a class that derives from Algorithm<Tconfiguration>
// It is for algorithms that have a dynamic size input/output array and allows to change
// the mode that the algorithm is using to process the input using processAnySize:
//
//	SYNCHRONOUS_BUFFER - zeropad the input to be a multiple of Configuration.bufferSize and process the entire input by successive calls to process
//	ASYNCHRONOUS_BUFFER - Create an internal buffer and fill it with input values in a for-loop. Every time the buffer is full, call process and output the result. This
// results in an additional delay equal to bufferSize

template <typename Tconfiguration>
class AlgorithmBuffer : public Algorithm<Tconfiguration>
{
  public:
    using Base = Algorithm<Tconfiguration>;
    using Configuration = typename Base::Configuration;
    using Input = typename Base::Input;
    using Output = typename Base::Output;
    using Coefficients = typename Base::Coefficients;
    using Parameters = typename Base::Parameters;
    using Setup = typename Base::Setup;

    // check if conditions for AlgorithmBuffer<Tconfiguration> are fulfilled
    static_assert(std::is_same<int, decltype(Coefficients::bufferSize)>::value,
                  "Coefficients must have integer variable bufferSize");                                                 // Coefficients has integer member variable bufferSize
    static_assert(Eigen::Dynamic == I::getType<Input>::type::RowsAtCompileTime, "Input number of rows must be dynamic"); // input rows size is Dynamic
    static_assert(Eigen::Dynamic == O::getType<Output>::type::RowsAtCompileTime, "Output number of rows must be dynamic"); // output rows size is Dynamic

    AlgorithmBuffer() : AlgorithmBuffer(Coefficients()) {}
    AlgorithmBuffer(const Coefficients &c) : Algorithm<Tconfiguration>(c) {}

    struct BufferBaseImplementation : public Base::BaseImplementation
    {
        BufferBaseImplementation() = default;
        virtual ~BufferBaseImplementation() = default;
        virtual BufferMode getBufferMode() const = 0;
        virtual int getBufferSize() const = 0;
        virtual int getDelaySamples() const = 0;
        virtual void processAnySize(Input input, Output output) = 0;
    };

    BufferMode getBufferMode() const { return static_cast<BufferBaseImplementation *>(Base::pimpl.get())->getBufferMode(); }
    int getBufferSize() const { return static_cast<BufferBaseImplementation *>(Base::pimpl.get())->getBufferSize(); }
    int getDelaySamples() const { return static_cast<BufferBaseImplementation *>(Base::pimpl.get())->getDelaySamples(); }
    void processAnySize(Input input, Output output) { static_cast<BufferBaseImplementation *>(Base::pimpl.get())->processAnySize(input, output); }
    void setBufferMode(BufferMode bufferMode); // define in source file

  protected:
    ~AlgorithmBuffer() = default;
};