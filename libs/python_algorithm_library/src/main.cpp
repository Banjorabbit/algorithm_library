#include <pybind11/pybind11.h>
#include <pybind11/eigen/matrix.h>

#include <algorithm_library/critical_bands.h>
#include <algorithm_library/fft.h>
#include <algorithm_library/filter_min_max.h>
#include <algorithm_library/filterbank.h>
#include <algorithm_library/interpolation.h>
#include <algorithm_library/min_phase_spectrum.h>
#include <algorithm_library/normal3d.h>
#include <algorithm_library/solver_toeplitz.h>
#include <algorithm_library/spectrogram.h>
#include <algorithm_library/spline.h>
#include <algorithm_library/dc_remover.h>
#include <algorithm_library/noise_estimation.h>
#include <algorithm_library/activity_detection_frequency_domain.h>

#include <pybind11_json/pybind11_json.hpp>
#include <pfr.hpp>

// Python wrapper library. The library replicates the C++ interface found in /include/algorithm_library/interface using templates and the pybind11 library
//
// author: Kristian Timm Andersen

namespace py = pybind11;

// remove Eigen::Ref<> from type
template<class T> struct removeEigenRef{ typedef T type; };
template<class T> struct removeEigenRef<Eigen::Ref<T>> { typedef T type; };
template< class T >
using removeEigenRef_t = typename removeEigenRef<T>::type;

// remove any of the following qualifiers: const Eigen::Ref<const T>&
template<typename T>
using eigenRemoveQualifiers = std::remove_const_t<removeEigenRef_t<std::remove_const_t<std::remove_reference_t<T>>>>;

// ---------------------------------------------------------------------------------
// template function for returning number of elements in an Input/Output struct
template<bool b, typename T>
struct numberOfElementsImpl
{
    static constexpr int n = pfr::tuple_size_v<T>;
};

template<typename T>
struct numberOfElementsImpl<false, T>
{
    static constexpr int n = 1;
};

// T is Input/Output struct type
template<typename T>
constexpr int numberOfElements()
{
  using Tp = std::remove_reference_t<T>;
  return numberOfElementsImpl<std::is_aggregate_v<Tp>,Tp>::n;
}

// -----------------------------------------------------------------------------------
// template struct for getting the type of a field in an Input/Output struct
template<int n, bool b, typename T>
struct getTypeFieldImpl
{
  using type = eigenRemoveQualifiers<pfr::tuple_element_t<n, T>>;
};

template<int n, typename T>
struct getTypeFieldImpl<n, false, T>
{
  using type = eigenRemoveQualifiers<T>;
};

// T is Input/Output struct type and n is index number in struct to get
template<typename T, int n=0>
using getTypeField = typename getTypeFieldImpl<n, std::is_aggregate_v<std::remove_reference_t<T>>,T>::type;

// ------------------------------------------------------------------------------------
// copy data from Python args to a C++ tuple, where template parameter T is the Input/Output type
template <typename T, size_t... Idx>
constexpr auto make_tuple_from_python_impl(py::args&& args, std::index_sequence<Idx...>)
{
  return std::make_tuple(py::cast<getTypeField<T, Idx>>(args[Idx])...);
}

template <typename T>
constexpr auto make_tuple_from_python(py::args&& args)
{
  return make_tuple_from_python_impl<T>(std::forward<py::args>(args), std::make_index_sequence<numberOfElements<T>()>{});
}
// ------------------------------------------------------------------------------------
// return true if input is valid
template <typename Talgo, class... Args, size_t... Idx>
inline auto validInputImpl(Talgo& algo, std::tuple<Args...>& t, std::index_sequence<Idx...>)
{
  return algo.validInput({std::get<Idx>(t)...});
}

template<typename Talgo,class... Args>
inline auto validInput(Talgo& algo, std::tuple<Args...>& t)
{
  return validInputImpl(algo, t, std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>{});
}

// ------------------------------------------------------------------------------------
// initialize output
template <typename Talgo, class... Args, size_t... Idx>
inline auto initOutputImpl(Talgo& algo, std::tuple<Args...>& t, std::index_sequence<Idx...>)
{
  return algo.initOutput({std::get<Idx>(t)...});
}

template<typename Talgo,class... Args>
inline auto initOutput(Talgo& algo, std::tuple<Args...>& t)
{
  return initOutputImpl(algo, t, std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>{});
}

// ------------------------------------------------------------------------------------
// process(input, output)
template <typename Talgo, class... ArgsIn, size_t... Idx, typename Toutput>
inline auto processImpl(Talgo& algo, std::tuple<ArgsIn...>& input, std::index_sequence<Idx...>, Toutput output)
{
  algo.process({std::get<Idx>(input)...}, output);
  return output;
}

template <typename Talgo, class... ArgsIn, size_t... IdxIn, class... ArgsOut, size_t... IdxOut>
inline auto processImpl(Talgo& algo, std::tuple<ArgsIn...>& input, std::index_sequence<IdxIn...>, std::tuple<ArgsOut...>& output, std::index_sequence<IdxOut...>)
{
  algo.process({std::get<IdxIn>(input)...}, {std::get<IdxOut>(output)...});
  return output;
}

template<typename Talgo,class... ArgsIn, typename Toutput>
inline auto process(Talgo& algo, std::tuple<ArgsIn...>& input, Toutput& output)
{
  return processImpl(algo, input, std::make_index_sequence<std::tuple_size_v<std::tuple<ArgsIn...>>>{}, output);
}

template<typename Talgo,class... ArgsIn, class... ArgsOut>
inline auto process(Talgo& algo, std::tuple<ArgsIn...>& input, std::tuple<ArgsOut...>& output)
{
  return processImpl(algo, input, std::make_index_sequence<std::tuple_size_v<std::tuple<ArgsIn...>>>{}, output, std::make_index_sequence<std::tuple_size_v<std::tuple<ArgsOut...>>>{});
}

// ------------------------------------------------------------------------------------
// Macro to define interface
#define DEFINE_PYTHON_INTERFACE(AlgorithmName) \
py::class_<AlgorithmName>(m,#AlgorithmName) \
  .def(py::init<>()) \
  .def(py::init<const nlohmann::json&>()) \
  .def("__repr__", [](const AlgorithmName& algo) \
  { \
      nlohmann::json setup = algo.getSetup(); \
      return std::string("PythonAlgorithmLibrary.") + #AlgorithmName + "\n" + setup.dump(4); \
  }) \
  .def("getCoefficients", [](const AlgorithmName& algo)  { return static_cast<nlohmann::json>(algo.getCoefficients()); }) \
  .def("setCoefficients", [](AlgorithmName& algo, const nlohmann::json& c) { algo.setCoefficients(c); }) \
  .def("getParameters", [](const AlgorithmName& algo)  { return static_cast<nlohmann::json>(algo.getParameters()); }) \
  .def("setParameters", [](AlgorithmName& algo, const nlohmann::json& c) { algo.setParameters(c); }) \
  .def("getSetup", [](const AlgorithmName& algo)  { return static_cast<nlohmann::json>(algo.getSetup()); }) \
  .def("setSetup", [](AlgorithmName& algo, const nlohmann::json& s) { algo.setSetup(s); }) \
  .def("validInput", [](const AlgorithmName& algo, py::args args) \
  { \
    auto input = make_tuple_from_python<AlgorithmName::Input>(std::move(args)); \
    return validInput(algo, input); \
  }) \
  .def("process", [](AlgorithmName& algo, py::args args) \
  { \
      auto input = make_tuple_from_python<AlgorithmName::Input>(std::move(args)); \
      auto output = initOutput(algo, input); \
      return process(algo, input, output); \
  })

// ------------------------------------------------------------------------------------

PYBIND11_MODULE(PythonAlgorithmLibrary, m) 
{
  DEFINE_PYTHON_INTERFACE(CriticalBandsMax);
  DEFINE_PYTHON_INTERFACE(CriticalBandsMean);
  DEFINE_PYTHON_INTERFACE(CriticalBandsSum);
  DEFINE_PYTHON_INTERFACE(FFT);
  DEFINE_PYTHON_INTERFACE(FilterMinMax);
  DEFINE_PYTHON_INTERFACE(FilterMin);
  DEFINE_PYTHON_INTERFACE(FilterMax);
  DEFINE_PYTHON_INTERFACE(StreamingMinMax);
  DEFINE_PYTHON_INTERFACE(StreamingMin);
  DEFINE_PYTHON_INTERFACE(StreamingMax);
  DEFINE_PYTHON_INTERFACE(FilterbankAnalysis)
    .def("setStandardFilterbank", [](FilterbankAnalysis& algo, int bufferSize) { algo.setStandardFilterbank(bufferSize); })
    .def("setLowDelayFilterbank", [](FilterbankAnalysis& algo, int bufferSize) { algo.setLowDelayFilterbank(bufferSize); })
    .def("setHighQualityFilterbank", [](FilterbankAnalysis& algo, int bufferSize) { algo.setHighQualityFilterbank(bufferSize); });
  DEFINE_PYTHON_INTERFACE(FilterbankSynthesis)
    .def("setStandardFilterbank", [](FilterbankSynthesis& algo, int bufferSize) { algo.setStandardFilterbank(bufferSize); })
    .def("setLowDelayFilterbank", [](FilterbankSynthesis& algo, int bufferSize) { algo.setLowDelayFilterbank(bufferSize); })
    .def("setHighQualityFilterbank", [](FilterbankSynthesis& algo, int bufferSize) { algo.setHighQualityFilterbank(bufferSize); });
  DEFINE_PYTHON_INTERFACE(Interpolation);
  DEFINE_PYTHON_INTERFACE(InterpolationConstant);
  DEFINE_PYTHON_INTERFACE(InterpolationSample);
  DEFINE_PYTHON_INTERFACE(MinPhaseSpectrum);
  DEFINE_PYTHON_INTERFACE(Normal3d);
  DEFINE_PYTHON_INTERFACE(SolverToeplitz);
  DEFINE_PYTHON_INTERFACE(Spectrogram);
  DEFINE_PYTHON_INTERFACE(Spline);
  DEFINE_PYTHON_INTERFACE(DCRemover);
  DEFINE_PYTHON_INTERFACE(NoiseEstimation);
  DEFINE_PYTHON_INTERFACE(ActivityDetectionFrequencyDomain); 
}

