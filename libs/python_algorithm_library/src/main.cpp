#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <algorithm_library/spectrogram.h>

#include <tuple>
#include <type_traits>
#include <cassert>

template <class T, class... TArgs> decltype(void(T{std::declval<TArgs>()...}), std::true_type{}) test_is_braces_constructible(int);
template <class, class...> std::false_type test_is_braces_constructible(...);
template <class T, class... TArgs> using is_braces_constructible = decltype(test_is_braces_constructible<T, TArgs...>(0));

struct any_type {
  template<class T>
  constexpr operator T(); // non explicit
};

template<class T>
auto to_tuple(T&& object) noexcept {
    using type = std::decay_t<T>;
    if constexpr(is_braces_constructible<type, any_type, any_type, any_type, any_type>{}) {
      auto&& [p1, p2, p3, p4] = object;
      return std::make_tuple(p1, p2, p3, p4);
    } else if constexpr(is_braces_constructible<type, any_type, any_type, any_type>{}) {
      auto&& [p1, p2, p3] = object;
      return std::make_tuple(p1, p2, p3);
    } else if constexpr(is_braces_constructible<type, any_type, any_type>{}) {
      auto&& [p1, p2] = object;
      return std::make_tuple(p1, p2);
    } else if constexpr(is_braces_constructible<type, any_type>{}) {
      auto&& [p1] = object;
      return std::make_tuple(p1);
    } else {
        return std::make_tuple();
    }
}
// int main() {
//     {
//       struct s {
//         int p1;
//         double p2;
//       };

//       auto t = to_tuple(s{1, 2.0});
//       static_assert(std::is_same<std::tuple<int, double>, decltype(t)>{});
//       assert(1 == std::get<0>(t));
//       assert(2.0 == std::get<1>(t));
//     }
    
//     {
//       struct s {
//         struct nested { } p1;
//         int p2;
//         int p3;
//         s* p4;
//       };

//       auto t = to_tuple(s{s::nested{}, 42, 87, nullptr});
//       static_assert(std::is_same<std::tuple<s::nested, int, int, s*>, decltype(t)>{});
//       assert(42 == std::get<1>(t));
//       assert(87 == std::get<2>(t));
//       assert(nullptr == std::get<3>(t));
//     }
// }

namespace nb = nanobind;
using namespace nb::literals;

int add(int i, int j=1) {
    return i + j;
}

struct Dog
{
    std::string name;

    std::string bark() const 
    {
        return name + ": woof!";
    }
};

NB_MODULE(PythonAlgorithmLibrary, m) {
    m.def("add", &add, nb::arg("a"), nb::arg("b") = 1, "This functions adds two numbers and increments if only one is provided");
    m.attr("the_answer") = 42;
    m.doc() = "A simple example python extension";

    nb::class_<Dog>(m,"Dog")
        .def(nb::init<>())
        .def(nb::init<const std::string &>())
        .def("bark", &Dog::bark)
        .def_rw("name", &Dog::name)
        .def("__repr__", [](const Dog& p) { return "<PythonAlgorithmLibrary.Dog named '" + p.name + "'>";});
    
    nb::class_<SpectrogramConfiguration::Coefficients>(m, "Coefficients")
        .def(nb::init<>())
        .def_rw("fftSize", &SpectrogramConfiguration::Coefficients::fftSize)
        .def_rw("bufferSize", &SpectrogramConfiguration::Coefficients::bufferSize)
        .def("__repr__", [](const SpectrogramConfiguration::Coefficients& c) 
        { 
            nlohmann::json j = c;
            return "<PythonAlgorithmLibrary.SpectrogramConfiguration.Coefficients: \n" + j.dump(4) + ">";
        });

    nb::class_<Spectrogram>(m,"Spectrogram")
        .def(nb::init<>())
        .def(nb::init<const SpectrogramConfiguration::Coefficients&>())
        .def("__repr__", [](const Spectrogram& algo) 
        { 
            nlohmann::json coef = algo.getCoefficients();
            nlohmann::json param = algo.getParameters();
            return "<PythonAlgorithmLibrary.Spectrogram\nCoefficients: \n" + coef.dump(4) + "\nParameters: \n" + param.dump(4) + "\n>";
        })
        .def("getCoefficients", [](const Spectrogram& algo)  { return static_cast<nlohmann::json>(algo.getCoefficients()).dump(); })
        .def("setCoefficients", [](Spectrogram& algo, const std::string &s) { algo.setCoefficients(nlohmann::json::parse(s)); })
        .def("getParameters", [](const Spectrogram& algo)  { return static_cast<nlohmann::json>(algo.getParameters()).dump(); })
        .def("setParameters", [](Spectrogram& algo, const std::string &s) { algo.setParameters(nlohmann::json::parse(s)); })
        .def("process", [](Spectrogram& algo, SpectrogramConfiguration::Input input), SpectrogramConfiguration::Output output) 
        { 
            algo.process(input, output); 
            return output; 
        });
}

