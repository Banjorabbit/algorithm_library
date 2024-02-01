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

// Define a version of NLOHMANN_JSON_SERIALIZE_ENUM that is friendly so it can be used inside structs
#define DEFINE_TUNABLE_ENUM(ENUM_TYPE, ...)                                     				\
    template<typename BasicJsonType>                                                            \
    friend void to_json(BasicJsonType& j, const ENUM_TYPE& e)                                   \
    {                                                                                           \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");          \
        static const std::pair<ENUM_TYPE, BasicJsonType> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [e](const std::pair<ENUM_TYPE, BasicJsonType>& ej_pair) -> bool  \
        {                                                                                       \
            return ej_pair.first == e;                                                          \
        });                                                                                     \
        j = ((it != std::end(m)) ? it : std::begin(m))->second;                                 \
    }                                                                                           \
    template<typename BasicJsonType>                                                            \
    friend void from_json(const BasicJsonType& j, ENUM_TYPE& e)                                 \
    {                                                                                           \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");          \
        static const std::pair<ENUM_TYPE, BasicJsonType> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [&j](const std::pair<ENUM_TYPE, BasicJsonType>& ej_pair) -> bool \
        {                                                                                       \
            return ej_pair.second == j;                                                         \
        });                                                                                     \
        e = ((it != std::end(m)) ? it : std::begin(m))->first;                                  \
    }
