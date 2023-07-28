#include <cxxopts.hpp>
#include "AudioFile.h"
#include <iostream>
#include <algorithm_library/spectrogram.h>
#include <matplot/matplot.h>

using namespace Eigen;
using namespace matplot;

// set constants
constexpr int N_FRAMES_MAX = 4*560;
constexpr int N_BINS_MAX = 4*420;
constexpr int VERSION = 1;

int main(int argc, char** argv)
{
  cxxopts::Options options(*argv, "A program for creating spectrograms of audio files.");
    
  std::string inputName;
  std::string outputName;
  float hopSizeMilliseconds;
  float spectrumSizeMilliseconds;

  options.add_options()
  ("h,help", "Show help")
  ("v,version", "Print the current version number")
  ("i,input", "Name of input file", cxxopts::value(inputName)->default_value("input.wav"))
  ("o,output", "Name of output file", cxxopts::value(outputName)->default_value("output.png"))
  ("t,hop_size", "Size of each time hop between spectrums in milliseconds", cxxopts::value(hopSizeMilliseconds)->default_value("10.f"))
  ("s,spectrum_size", "Size of each frame used for calculating the spectrum in milliseconds", cxxopts::value(spectrumSizeMilliseconds)->default_value("80.f"));
  
  options.allow_unrecognised_options();
    
  auto result = options.parse(argc, argv);

  if (result["help"].as<bool>()) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result["version"].as<bool>()) {
    std::cout << "version " << VERSION << std::endl;
    return 0;
  }

  AudioFile<float> audioFileInput;
  audioFileInput.load(inputName);
  std::cout << "Input file summary:\n";
  audioFileInput.printSummary();
  std::cout << "\n";

  std::cout << "Processing summary:\n";
  auto c = Spectrogram::Coefficients();
  c.bufferSize = static_cast<int>(hopSizeMilliseconds / 1000.f * audioFileInput.getSampleRate());
  std::cout << "Buffer size: " << c.bufferSize << "\n";
  float fftSize = spectrumSizeMilliseconds / 1000.f * audioFileInput.getSampleRate();
  c.fftSize = static_cast<int>(std::pow(2, std::roundf(std::log2f(fftSize))));
  std::cout << "FFT size: " << c.fftSize << "\n";
  Spectrogram spectrogram(c);
  
  int nFrames = audioFileInput.getNumSamplesPerChannel() / c.bufferSize;
  std::cout << "Number of frames: " << nFrames << "\n";
  int nBins = c.fftSize/2+1;
  ArrayXXf spec(nBins, nFrames);

  for (auto nFrame = 0; nFrame < nFrames; nFrame++)
  {
    ArrayXf frameIn = Map<ArrayXf>(&audioFileInput.samples[0][nFrame * c.bufferSize], c.bufferSize);
    spectrogram.process(frameIn, spec.col(nFrame));
  }
  
  int nFramesDecimate = std::max(nFrames / N_FRAMES_MAX, 1);
  int nBinsDecimate = std::max(nBins / N_BINS_MAX, 1);
  int nFramesPlot = nFrames / nFramesDecimate;
  int nBinsPlot = nBins / nBinsDecimate;

  std::vector<std::vector<float>> specVec(nBinsPlot, std::vector<float>(nFramesPlot));
  float maxValue = -100;
  for (auto i = 0; i < nBinsPlot; i++)
  {
    for (auto j = 0; j < nFramesPlot; j++)
    {
      
      specVec[i][j] = 20.f * std::log10(spec.block(nBinsDecimate * i, nFramesDecimate * j, nBinsDecimate, nFramesDecimate).maxCoeff());
      maxValue = std::max(maxValue, specVec[i][j]);
    }
  }
  gca()->color_box_range(maxValue-90, maxValue - 10);
  gcf()->height(nBinsPlot);
  gcf()->width(nFramesPlot);
  gca()->xlabel("Frame number");
  gca()->ylabel("Frequency bin");

  imagesc(specVec);
  colorbar();
  save(outputName); 

  std::cout << "DONE!\n" << std::endl;

  return 0;
}