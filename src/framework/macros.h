#pragma once
#include "nlohmann/json.hpp"
// author: Kristian Timm Andersen

#define EVAL(...) __VA_ARGS__

#define GET_NUMBER_OF_ARGUMENTS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NUMBER, ...) NUMBER

#define CONCATENATE_METHOD_10(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_9)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_9(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_8)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_8(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_7)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_7(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_6)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_6(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_5)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_5(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_4)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_4(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_3)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_3(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_2)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_2(term, member, ...) member##term, EVAL(EVAL(CONCATENATE_METHOD_1)(term, __VA_ARGS__))
#define CONCATENATE_METHOD_1(term, member) member##term

#define SELECT_CONCATENATE(term, ...)                                                                                                                                         \
    EVAL(GET_NUMBER_OF_ARGUMENTS(__VA_ARGS__, CONCATENATE_METHOD_10, CONCATENATE_METHOD_9, CONCATENATE_METHOD_8, CONCATENATE_METHOD_7, CONCATENATE_METHOD_6,                  \
                                 CONCATENATE_METHOD_5, CONCATENATE_METHOD_4, CONCATENATE_METHOD_3, CONCATENATE_METHOD_2, CONCATENATE_METHOD_1)(term, __VA_ARGS__))

#define DEFINE_MEMBER_10(method, type, member, ...)                                                                                                                           \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_9)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_9(method, type, member, ...)                                                                                                                            \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_8)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_8(method, type, member, ...)                                                                                                                            \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_7)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_7(method, type, member, ...)                                                                                                                            \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_6)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_6(method, type, member, ...)                                                                                                                            \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_5)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_5(method, type, member, ...)                                                                                                                            \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_4)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_4(method, type, member, ...)                                                                                                                            \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_3)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_3(method, type, member, ...)                                                                                                                            \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_2)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_2(method, type, member, ...)                                                                                                                            \
    decltype(member.method()) member##type;                                                                                                                                   \
    EVAL(EVAL(DEFINE_MEMBER_1)(method, type, __VA_ARGS__))
#define DEFINE_MEMBER_1(method, type, member) decltype(member.method()) member##type;

#define SET_MEMBER_METHOD_10(method, type, member, ...)                                                                                                                       \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_9)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_9(method, type, member, ...)                                                                                                                        \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_8)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_8(method, type, member, ...)                                                                                                                        \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_7)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_7(method, type, member, ...)                                                                                                                        \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_6)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_6(method, type, member, ...)                                                                                                                        \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_5)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_5(method, type, member, ...)                                                                                                                        \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_4)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_4(method, type, member, ...)                                                                                                                        \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_3)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_3(method, type, member, ...)                                                                                                                        \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_2)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_2(method, type, member, ...)                                                                                                                        \
    member.method(members.member##type);                                                                                                                                      \
    EVAL(EVAL(SET_MEMBER_METHOD_1)(method, type, __VA_ARGS__))
#define SET_MEMBER_METHOD_1(method, type, member) member.method(members.member##type);

#define APPLY_MEMBER_METHOD_10(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_9)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_9(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_8)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_8(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_7)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_7(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_6)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_6(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_5)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_5(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_4)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_4(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_3)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_3(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_2)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_2(method, member, ...) member.method EVAL(EVAL(APPLY_MEMBER_METHOD_1)(method, __VA_ARGS__))
#define APPLY_MEMBER_METHOD_1(method, member) member.method

#define SELECT_DEFINE_MEMBER(method, type, ...)                                                                                                                               \
    EVAL(GET_NUMBER_OF_ARGUMENTS(__VA_ARGS__, DEFINE_MEMBER_10, DEFINE_MEMBER_9, DEFINE_MEMBER_8, DEFINE_MEMBER_7, DEFINE_MEMBER_6, DEFINE_MEMBER_5, DEFINE_MEMBER_4,         \
                                 DEFINE_MEMBER_3, DEFINE_MEMBER_2, DEFINE_MEMBER_1)(method, type, __VA_ARGS__))
#define SELECT_SET_MEMBER_METHOD(method, type, ...)                                                                                                                           \
    EVAL(GET_NUMBER_OF_ARGUMENTS(__VA_ARGS__, SET_MEMBER_METHOD_10, SET_MEMBER_METHOD_9, SET_MEMBER_METHOD_8, SET_MEMBER_METHOD_7, SET_MEMBER_METHOD_6, SET_MEMBER_METHOD_5,  \
                                 SET_MEMBER_METHOD_4, SET_MEMBER_METHOD_3, SET_MEMBER_METHOD_2, SET_MEMBER_METHOD_1)(method, type, __VA_ARGS__))
#define SELECT_APPLY_MEMBER_METHOD(method, ...)                                                                                                                               \
    EVAL(GET_NUMBER_OF_ARGUMENTS(__VA_ARGS__, APPLY_MEMBER_METHOD_10, APPLY_MEMBER_METHOD_9, APPLY_MEMBER_METHOD_8, APPLY_MEMBER_METHOD_7, APPLY_MEMBER_METHOD_6,             \
                                 APPLY_MEMBER_METHOD_5, APPLY_MEMBER_METHOD_4, APPLY_MEMBER_METHOD_3, APPLY_MEMBER_METHOD_2, APPLY_MEMBER_METHOD_1)(method, __VA_ARGS__))

// Example of DEFINE_MEMBER_SET_GET_FUNCTIONS(Coefficients, coefficients, Algorithm1, Algorithm2):
//
// struct MemberCoefficients {
//     Coefficients coefficients;
//     decltype(Algorithm1.getCoefficientsTree()) Algorithm1Coefficients; decltype(Algorithm2.getCoefficientsTree()) Algorithm2Coefficients;
//	   DEFINE_TUNABLE(MemberCoefficients, Algorithm1Coefficients, Algorithm2Coefficients)
// };
// template<typename...Ts>
// MemberCoefficients getCoefficientsMember(const Ts&... ts) const
// {
//     return MemberCoefficients{getCoefficients(), ts.getCoefficientsTree()... };
// }
// MemberCoefficients getCoefficientsTreeImpl() const { return getCoefficientsMember(Algorithm1, Algorithm2); }
// void setCoefficientsTreeImpl(const MemberCoefficients& members) {
//     setCoefficients(members.coefficients);
//     Algorithm1.setCoefficientsTree(members.Algorithm1Coefficients); Algorithm2.setCoefficientsTree(members.Algorithm2Coefficients);
// }
#define DEFINE_MEMBER_SET_GET_FUNCTIONS(type, name, ...)                                                                                                                      \
    struct Member##type                                                                                                                                                       \
    {                                                                                                                                                                         \
        type name;                                                                                                                                                            \
        SELECT_DEFINE_MEMBER(get##type##Tree, type, __VA_ARGS__)                                                                                                              \
        DEFINE_TUNABLE(Member##type, EVAL(name), SELECT_CONCATENATE(type, __VA_ARGS__))                                                                                       \
    };                                                                                                                                                                        \
    template <typename... Ts>                                                                                                                                                 \
    Member##type get##type##Member(const Ts &...ts) const                                                                                                                     \
    {                                                                                                                                                                         \
        return EVAL(Member##type){get##type(), ts.get##type##Tree()...};                                                                                                      \
    }                                                                                                                                                                         \
    Member##type get##type##TreeImpl() const { return get##type##Member(__VA_ARGS__); }                                                                                       \
    void set##type##TreeImpl(const Member##type &members)                                                                                                                     \
    {                                                                                                                                                                         \
        set##type(members.name);                                                                                                                                              \
        SELECT_SET_MEMBER_METHOD(set##type##Tree, type, __VA_ARGS__)                                                                                                          \
    }

// Example of DEFINE_MEMBER_SETUP_SET_GET_FUNCTIONS(Algorithm1, Algorithm2):
//
// struct MemberSetup {
//     Coefficients coefficients;
//	   Parameters parameters;
//     decltype(Algorithm1.getSetupTree()) Algorithm1; decltype(Algorithm2.getSetupTree()) Algorithm2;
// };
// template<typename...Ts>
// MemberSetup getSetupMember(const Ts&... ts) const
// {
//     return MemberSetup{getCoefficients(), getParameters(), ts.getSetupTree()... };
// }
// MemberSetup getSetupTreeImpl() const { return getSetupMember(Algorithm1, Algorithm2); }
// void setSetupTreeImpl(const MemberSetup& members) {
//     setCoefficients(members.coefficients);
//     setParameters(members.parameters);
//     Algorithm1.setSetupTree(members.Algorithm1); Algorithm2.setSetupTree(members.Algorithm2);
// }
#define DEFINE_MEMBER_SETUP_SET_GET_FUNCTIONS(...)                                                                                                                            \
    struct MemberSetup                                                                                                                                                        \
    {                                                                                                                                                                         \
        Coefficients coefficients;                                                                                                                                            \
        Parameters parameters;                                                                                                                                                \
        SELECT_DEFINE_MEMBER(getSetupTree, Setup, __VA_ARGS__)                                                                                                                \
        DEFINE_TUNABLE(MemberSetup, coefficients, parameters, SELECT_CONCATENATE(Setup, __VA_ARGS__))                                                                         \
    };                                                                                                                                                                        \
    template <typename... Ts>                                                                                                                                                 \
    MemberSetup getSetupMember(const Ts &...ts) const                                                                                                                         \
    {                                                                                                                                                                         \
        return MemberSetup{getCoefficients(), getParameters(), ts.getSetupTree()...};                                                                                         \
    }                                                                                                                                                                         \
    MemberSetup getSetupTreeImpl() const { return getSetupMember(__VA_ARGS__); }                                                                                              \
    void setSetupTreeImpl(const MemberSetup &members)                                                                                                                         \
    {                                                                                                                                                                         \
        setCoefficients(members.coefficients);                                                                                                                                \
        setParameters(members.parameters);                                                                                                                                    \
        SELECT_SET_MEMBER_METHOD(setSetupTree, Setup, __VA_ARGS__)                                                                                                            \
    }

#define DEFINE_MEMBER_ALGORITHMS(...)                                                                                                                                         \
    DEFINE_MEMBER_SET_GET_FUNCTIONS(Parameters, parameters, __VA_ARGS__)                                                                                                      \
    DEFINE_MEMBER_SET_GET_FUNCTIONS(Coefficients, coefficients, __VA_ARGS__)                                                                                                  \
    DEFINE_MEMBER_SETUP_SET_GET_FUNCTIONS(__VA_ARGS__)                                                                                                                        \
    size_t getDynamicSizeAlgorithms() const final { return SELECT_APPLY_MEMBER_METHOD(EVAL(getDynamicSize() +), __VA_ARGS__) 0; }                                             \
    void resetAlgorithms() final { SELECT_APPLY_MEMBER_METHOD(EVAL(reset();), __VA_ARGS__) }                                                                                  \
    bool isAlgorithmsValid() const final { return SELECT_APPLY_MEMBER_METHOD(EVAL(isConfigurationValid() &&), __VA_ARGS__) true; }

#define DEFINE_ALGORITHM_CONSTRUCTOR(PublicAlgorithm, InternalAlgorithm, ConfigurationName)                                                                                   \
    using InternalAlgorithm##SingleBufferImpl = Implementation<InternalAlgorithm, ConfigurationName>;                                                                         \
    template <>                                                                                                                                                               \
    void Algorithm<ConfigurationName>::setImplementation(const Coefficients &c)                                                                                               \
    {                                                                                                                                                                         \
        pimpl = std::make_unique<InternalAlgorithm##SingleBufferImpl>(c);                                                                                                     \
    }                                                                                                                                                                         \
    PublicAlgorithm::PublicAlgorithm(const Coefficients &c) : Algorithm<ConfigurationName>(c) {}                                                                              \
    extern "C" PublicAlgorithm *create##PublicAlgorithm##Instance() { return new PublicAlgorithm; }

#define DEFINE_ALGORITHM_BUFFER_CONSTRUCTOR(PublicAlgorithm, InternalAlgorithm, ConfigurationName)                                                                            \
    template <>                                                                                                                                                               \
    void Algorithm<ConfigurationName>::setImplementation(const Coefficients &c)                                                                                               \
    {                                                                                                                                                                         \
        pimpl = std::make_unique<BufferImplementation<InternalAlgorithm, ConfigurationName>>(c);                                                                              \
    }                                                                                                                                                                         \
    PublicAlgorithm::PublicAlgorithm(const Coefficients &c) : AlgorithmBuffer<ConfigurationName>(c) {}                                                                        \
    extern "C" PublicAlgorithm *create##PublicAlgorithm##Instance() { return new PublicAlgorithm; }                                                                           \
    template <>                                                                                                                                                               \
    void AlgorithmBuffer<ConfigurationName>::setBufferMode(BufferMode bufferMode)                                                                                             \
    {                                                                                                                                                                         \
        if (bufferMode == SYNCHRONOUS_BUFFER)                                                                                                                                 \
        {                                                                                                                                                                     \
            if (static_cast<BufferBaseImplementation *>(Base::pimpl.get())->getBufferMode() != SYNCHRONOUS_BUFFER)                                                            \
            {                                                                                                                                                                 \
                auto cTree = static_cast<AsynchronousBufferImplementation<InternalAlgorithm, ConfigurationName> *>(pimpl.get())->algo.getCoefficientsTree();                  \
                auto pTree = static_cast<AsynchronousBufferImplementation<InternalAlgorithm, ConfigurationName> *>(pimpl.get())->algo.getParametersTree();                    \
                pimpl = std::make_unique<BufferImplementation<InternalAlgorithm, ConfigurationName>>(cTree);                                                                  \
                static_cast<BufferImplementation<InternalAlgorithm, ConfigurationName> *>(pimpl.get())->algo.setParametersTree(pTree);                                        \
            }                                                                                                                                                                 \
        }                                                                                                                                                                     \
        else                                                                                                                                                                  \
        {                                                                                                                                                                     \
            auto cTree = static_cast<BufferImplementation<InternalAlgorithm, ConfigurationName> *>(pimpl.get())->algo.getCoefficientsTree();                                  \
            auto pTree = static_cast<BufferImplementation<InternalAlgorithm, ConfigurationName> *>(pimpl.get())->algo.getParametersTree();                                    \
            pimpl = std::make_unique<AsynchronousBufferImplementation<InternalAlgorithm, ConfigurationName>>(cTree);                                                          \
            static_cast<AsynchronousBufferImplementation<InternalAlgorithm, ConfigurationName> *>(pimpl.get())->algo.setParametersTree(pTree);                                \
        }                                                                                                                                                                     \
    }
