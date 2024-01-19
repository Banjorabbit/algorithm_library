#pragma once
#include "macros_json.h"
#include "input_output.h"

template<typename Tconfiguration>
struct TSetup
{
	using Coefficients = typename Tconfiguration::Coefficients;
	using Parameters = typename Tconfiguration::Parameters;

	TSetup() = default;
	TSetup(const Coefficients& c, const Parameters& p) { coefficients = c; parameters = p; }
	Coefficients coefficients;
	Parameters parameters;
	DEFINE_TUNABLE_SETUP(coefficients, parameters)
};

template<typename Tconfiguration>
class Algorithm
{
public:
	using Configuration = Tconfiguration;
	using Input = const typename Configuration::Input&; // force inputs to be const reference
	using Output = typename Configuration::Output;
	using Coefficients = typename Configuration::Coefficients;
	using Parameters = typename Configuration::Parameters;
	using Setup = TSetup<Configuration>;

    Algorithm() : Algorithm(Coefficients()) {} // default constructor
	Algorithm(const Coefficients& c); // define this in derived source file.
	
	void process(Input input, Output output) { pimpl->process(input, output); }
	Coefficients getCoefficients() const { return pimpl->getCoefficients(); }
	Parameters getParameters() const { return pimpl->getParameters(); }
	Setup getSetup() const { return pimpl->getSetup(); }
	void reset() { pimpl->reset(); }
	
	void setCoefficients(const Coefficients& c) { pimpl->setCoefficients(c); }
	void setParameters(const Parameters& p) { pimpl->setParameters(p); }
	void setSetup(const Setup& s) { pimpl->setSetup(s); }

	auto validInput(Input input) const { return Configuration::validInput(input, getCoefficients()); }
	auto initOutput(Input input) const { return Configuration::initOutput(input, getCoefficients()); }

	static constexpr size_t ALGORITHM_VERSION_MAJOR = 1; // version changes in ABI

	struct BaseImplementation // Base of implementation. Allows to derive different implementations from this struct.
	{ 
    	BaseImplementation() = default;
    	virtual ~BaseImplementation() = default;
    	virtual void process(Input input, Output output) = 0;
    	virtual Coefficients getCoefficients() const = 0;
    	virtual Parameters getParameters() const = 0;
    	virtual Setup getSetup() const = 0;
    	virtual void setCoefficients(const Coefficients& c) = 0;
    	virtual void setParameters(const Parameters& p) = 0;
    	virtual void setSetup(const Setup& s) = 0;
    	virtual void reset() = 0;
	};
	
protected:
	~Algorithm() = default;;
	
	std::unique_ptr<BaseImplementation> pimpl; // PIMPL. Define in derived source file
};

enum BufferMode {SINGLE_BUFFER, MULTI_BUFFER, ASYNCHRONOUS_BUFFER };


template<typename Tconfiguration>
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

	// check if conditions for Tconfiguration  and input/output are fulfilled in AlgorithmBuffer
	static_assert(std::is_same<int, decltype(Coefficients::bufferSize)>::value); // Coefficients has integer member variable bufferSize
	static_assert(std::is_same<int, decltype(Coefficients::nChannels)>::value); // Coefficients has integer member variable nChannels
	static_assert(std::is_same<BufferMode, decltype(Coefficients::bufferMode)>::value); // Coefficients has BufferMode member variable bufferMode
	static_assert(Eigen::Dynamic == I::getType<Input>::type::RowsAtCompileTime); // input rows at compile time is Dynamic
	static_assert(Eigen::Dynamic == I::getType<Input>::type::ColsAtCompileTime); // input cols at compile time is Dynamic
	static_assert(Eigen::Dynamic == O::getType<Output>::type::RowsAtCompileTime); // output rows at compile time is Dynamic
	static_assert(Eigen::Dynamic == I::getType<Output>::type::ColsAtCompileTime); // output cols at compile time is Dynamic

	AlgorithmBuffer() : AlgorithmBuffer(Coefficients()) {}
	AlgorithmBuffer(const Coefficients& c) : Algorithm<Tconfiguration>(c) {}

	struct BufferBaseImplementation : public Base::BaseImplementation
	{
		BufferBaseImplementation() = default;
		virtual ~BufferBaseImplementation() = default;
		virtual BufferMode getBufferMode() const = 0;
		virtual int getBufferSize() const = 0;
		virtual int getNChannels() const = 0;
		virtual int getDelaySamples() const = 0;
	};

	BufferMode getBufferMode() const { return static_cast<BufferBaseImplementation*>(Base::pimpl.get())->getBufferMode(); }
	int getBufferSize() const { return static_cast<BufferBaseImplementation*>(Base::pimpl.get())->getBufferSize(); }
	int getNChannels() const { return static_cast<BufferBaseImplementation*>(Base::pimpl.get())->getNChannels(); }
	int getDelaySamples() const { return static_cast<BufferBaseImplementation*>(Base::pimpl.get())->getDelaySamples(); }

protected:
	~AlgorithmBuffer() = default;
};