#include "benchmark/benchmark.h"
#include "spline/spline_cubic.h"
#include "interpolation/interpolation_cubic.h"
#include "fft/fft_real.h"
#include "solver_toeplitz/solver_toeplitz_system.h"
#include "filterbank/filterbank_wola.h"
#include "spectrogram/spectrogram_filterbank.h"
#include "spectrogram/spectrogram_nonlinear.h"
#include "normal3d/normal3d_diff.h"
#include "min_phase_spectrum/min_phase_spectrum_cepstrum.h"
#include "critical_bands/critical_bands_bark.h"
#include "filter_min_max/filter_min_max_lemire.h"
#include "dc_remover/dc_remover_first_order.h"
#include "delay/circular_buffer.h"
#include "noise_estimation/noise_estimation_activity_detection.h"

// Macro for defining timing test using google benchmark framework
#define DEFINE_BENCHMARK_ALGORITHM(algorithm) \
static void algorithm##_process(benchmark::State& state) { \
	algorithm::Configuration::Test<algorithm> test; \
	for (auto _ : state) \
    { \
		test.processAlgorithm(); \
		benchmark::DoNotOptimize(test); \
	} \
} \
BENCHMARK(algorithm##_process);

// insert algorithms to be benchmarked

DEFINE_BENCHMARK_ALGORITHM(CircularBuffer)
DEFINE_BENCHMARK_ALGORITHM(NoiseEstimationActivityDetection)
DEFINE_BENCHMARK_ALGORITHM(CircularBufferSingleChannel)
DEFINE_BENCHMARK_ALGORITHM(SplineCubic)
DEFINE_BENCHMARK_ALGORITHM(InterpolationCubicSample)
DEFINE_BENCHMARK_ALGORITHM(InterpolationSample)
DEFINE_BENCHMARK_ALGORITHM(InterpolationCubic)
DEFINE_BENCHMARK_ALGORITHM(InterpolationCubicConstant)
DEFINE_BENCHMARK_ALGORITHM(FFTReal)
DEFINE_BENCHMARK_ALGORITHM(SolverToeplitzSystem)
DEFINE_BENCHMARK_ALGORITHM(FilterbankAnalysisWOLA)
DEFINE_BENCHMARK_ALGORITHM(FilterbankSynthesisWOLA)
DEFINE_BENCHMARK_ALGORITHM(SpectrogramFilterbank)
DEFINE_BENCHMARK_ALGORITHM(SpectrogramNonlinear)
DEFINE_BENCHMARK_ALGORITHM(Spectrogram)
DEFINE_BENCHMARK_ALGORITHM(Normal3dDiff)
DEFINE_BENCHMARK_ALGORITHM(MinPhaseSpectrumCepstrum)
DEFINE_BENCHMARK_ALGORITHM(CriticalBandsBarkSum)
DEFINE_BENCHMARK_ALGORITHM(FilterMinMaxLemire)
DEFINE_BENCHMARK_ALGORITHM(FilterMaxLemire)
DEFINE_BENCHMARK_ALGORITHM(FilterMinLemire)
DEFINE_BENCHMARK_ALGORITHM(StreamingMinMaxLemire)
DEFINE_BENCHMARK_ALGORITHM(StreamingMaxLemire)
DEFINE_BENCHMARK_ALGORITHM(StreamingMinLemire)
DEFINE_BENCHMARK_ALGORITHM(DCRemoverFirstOrder)



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
