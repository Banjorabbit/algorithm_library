#pragma once
#include "algorithm_library/interface/macros_json.h"
#include "algorithm_library/interface/public_algorithm.h"

template<typename Talgo, typename Tconfiguration>
struct Implementation : public Algorithm<Tconfiguration>::BaseImplementation
{
    Implementation() : algo{} {}
    Implementation(const typename Tconfiguration::Coefficients& c) : algo{ c } {}
    Talgo algo;
    void process(typename Algorithm<Tconfiguration>::Input input, typename Algorithm<Tconfiguration>::Output output) override { algo.process(input, output); }
    typename Algorithm<Tconfiguration>::Coefficients getCoefficients() const override { return algo.getCoefficients(); }
    typename Algorithm<Tconfiguration>::Parameters getParameters() const override { return algo.getParameters(); }
    typename Algorithm<Tconfiguration>::Setup getSetup() const override { return algo.getSetup(); }
    void setCoefficients(const typename Algorithm<Tconfiguration>::Coefficients& c) override { algo.setCoefficients(c); }
    void setParameters(const typename Algorithm<Tconfiguration>::Parameters& p) override { algo.setParameters(p); }
    void setSetup(const typename Algorithm<Tconfiguration>::Setup& s) override { algo.setSetup(s); }
    void reset() override { algo.reset(); }
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
	AlgorithmImplementation(const Parameters& p) : P(p) { }
	AlgorithmImplementation(const Setup& s) : C(s.coefficients), P(s.parameters) { }
	~AlgorithmImplementation() = default;
	
	size_t getStaticSize() const { return sizeof(Talgo); }

	size_t getDynamicSize() const
	{
		return getMembersDynamicSize() + getSubMembersDynamicSize();
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
		resetWithoutMemberAlgorithms();
		resetMemberAlgorithms();
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

	void onParametersChanged() {} // hide in derived algorithm if desired

	auto validInput(Input input) const { return Configuration::validInput(input, C); }
	auto initOutput(Input input) const { return Configuration::initOutput(input, C); }

protected:
	// these functions will be overridden if defined in derived Talgo
	virtual size_t getMembersDynamicSize() const { return 0; }
	virtual size_t getSubMembersDynamicSize() const { return 0; }
	virtual void resetWithoutMemberAlgorithms() { }
	virtual void resetMemberAlgorithms() { }

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
