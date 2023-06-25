#pragma once
#include "interface/macros_json.h"
#include "interface/public_algorithm.h"

template<typename Tconfiguration, typename Talgo>
class IAlgorithm
{
public:
	using Configuration = Tconfiguration;
	using Input = const typename Configuration::Input&; // force inputs to be const reference
	using Output = typename Configuration::Output;
	using BaseAlgorithm = IAlgorithm;
	using Coefficients = typename Configuration::Coefficients;
	using Parameters = typename Configuration::Parameters;
	using Setup = TSetup<Configuration>;

	static_assert(std::is_trivially_copyable<Coefficients>::value, "Coefficients data type must be trivially copyable.");
	static_assert(std::is_trivially_copyable<Parameters>::value, "Parameters data type must be trivially copyable.");


	IAlgorithm() = default;
	IAlgorithm(const Coefficients& c) : C(c) {}
	IAlgorithm(const Parameters& p) : P(p) { }
	IAlgorithm(const Setup& s) : C(s.coefficients), P(s.parameters) { }
	~IAlgorithm() = default;
	
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
		resetMembers();
		resetSubMembers();
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

protected:
	// these functions will be overridden if defined in derived Talgo
	virtual size_t getMembersDynamicSize() const { return 0; }
	virtual size_t getSubMembersDynamicSize() const { return 0; }
	virtual void resetMembers() { }
	virtual void resetSubMembers() { }

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
