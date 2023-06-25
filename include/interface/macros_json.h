#pragma once
#include "nlohmann/json.hpp"
// author: Kristian Timm Andersen

#define DEFINE_NO_TUNABLE(Type) \
	friend void to_json(nlohmann::json& j, const Type& p) { j = nlohmann::json{}; } \
	friend void from_json(const nlohmann::json& j, Type& p) {}

#define DEFINE_NO_TUNABLE_PARAMETERS DEFINE_NO_TUNABLE(Parameters)
#define DEFINE_NO_TUNABLE_COEFFICIENTS DEFINE_NO_TUNABLE(Coefficients)

#define DEFINE_TUNABLE( ...) NLOHMANN_DEFINE_TYPE_INTRUSIVE( __VA_ARGS__)
#define DEFINE_TUNABLE_PARAMETERS( ...) DEFINE_TUNABLE(Parameters, __VA_ARGS__)
#define DEFINE_TUNABLE_COEFFICIENTS( ...) DEFINE_TUNABLE(Coefficients, __VA_ARGS__)
#define DEFINE_TUNABLE_SETUP( ...) DEFINE_TUNABLE(TSetup, __VA_ARGS__)
