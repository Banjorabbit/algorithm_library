#pragma once
#include "macros_json.h"

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

	template<typename T>
	Algorithm(const T& t);
	Algorithm();
	
	// define in pimpl source file
	void process(Input input, Output output);
	Coefficients getCoefficients() const;
	Parameters getParameters() const;
	Setup getSetup() const;
	void reset();
	
	void setCoefficients(const Coefficients& c);
	void setParameters(const Parameters& p);
	void setSetup(const Setup& s);

	auto validateInput(Input input) const { return Configuration::validateInput(input, getCoefficients()); }
	auto initOutput(Input input) const { return Configuration::initOutput(input, getCoefficients()); }

	static constexpr size_t ALGORITHM_VERSION_MAJOR = 1; // version changes in ABI

protected:
	~Algorithm();

	struct Impl;
	std::unique_ptr<Impl> pimpl; // PIMPL. Define in derived source file

	
};
