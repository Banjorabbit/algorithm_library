#include "benchmark/benchmark.h"
#include "spline/spline_cubic.h"
#include "interpolation/interpolation_cubic.h"
#include "fft/fft_real.h"
#include "solver_toeplitz/solver_toeplitz_system.h"
#include "filterbank/filterbank_wola.h"
#include "spectrogram/spectrogram_filterbank.h"
#include "spectrogram/spectrogram_adaptive.h"
#include "normal3d/normal3d_diff.h"

// Macro for defining timing test using google benchmark framework
#define DEFINE_BENCHMARK_ALGORITHM(algorithm) \
static void algorithm##_process(benchmark::State& state) { \
	algorithm algo; \
	algorithm::Configuration::Test<algorithm> test; \
	for (auto _ : state) \
    { \
		test.processAlgorithm(algo); \
		benchmark::DoNotOptimize(test); \
	} \
} \
BENCHMARK(algorithm##_process);

// insert algorithms to be benchmarked

DEFINE_BENCHMARK_ALGORITHM(SplineCubic)
DEFINE_BENCHMARK_ALGORITHM(InterpolationCubicSample)
DEFINE_BENCHMARK_ALGORITHM(InterpolationCubic)
DEFINE_BENCHMARK_ALGORITHM(InterpolationCubicConstant)
DEFINE_BENCHMARK_ALGORITHM(FFTReal)
DEFINE_BENCHMARK_ALGORITHM(SolverToeplitzSystem)
DEFINE_BENCHMARK_ALGORITHM(FilterbankAnalysisWOLA)
DEFINE_BENCHMARK_ALGORITHM(FilterbankSynthesisWOLA)
DEFINE_BENCHMARK_ALGORITHM(SpectrogramFilterbank)
DEFINE_BENCHMARK_ALGORITHM(SpectrogramAdaptive)
DEFINE_BENCHMARK_ALGORITHM(Normal3dDiff)

// benchmark inverse FFT
static void FFTInverse_process(benchmark::State& state) {
	
		FFTReal algo; 
		FFT::Configuration::Test<FFT> test; 
		for (auto _ : state) 
		{ 
			algo.inverse(test.output, test.input);
			benchmark::DoNotOptimize(test); 
		} 
} 
BENCHMARK(FFTInverse_process);

// main function
BENCHMARK_MAIN();
