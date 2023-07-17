#pragma once
#include "input_output.h"
#include "macros_json.h"

template<typename Tinput, typename Toutput, typename TinputData = I::Real>
struct Configuration
{
	using Input = Tinput;
	using Output = Toutput;
	using InputData = TinputData;

	struct Coefficients
	{
		DEFINE_NO_TUNABLE_COEFFICIENTS
	};

	struct Parameters
	{
		DEFINE_NO_TUNABLE_PARAMETERS
	};

	// isDefined can be used to test compile-time if a definition exists in the configuration
	template <class T, class Enable = void>
	struct isDefined : std::false_type
	{};

	template <class T>
	struct isDefined<T, std::enable_if_t<sizeof(T)>> : std::true_type
	{};

	static constexpr double PI = 3.14159265358979323846;
	static constexpr double BUTTERWORTH_Q = 0.707106781186548;
};