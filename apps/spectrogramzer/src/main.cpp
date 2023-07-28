#include <cxxopts.hpp>
#include "AudioFile.h"
#include <iostream>
#include <algorithm_library/spectrogram.h>
#include <matplot/matplot.h>

using namespace Eigen;
using namespace matplot;

// set constants
enum Languages {EN, DE, ES, FR};
constexpr int VERSION = 1;
int main(int argc, char** argv)
{
    const std::unordered_map<std::string, Languages> languages{
      {"en", EN},
      {"de", DE},
      {"es", ES},
      {"fr", FR},
  };

    cxxopts::Options options(*argv, "A program for creating spectrograms of audio files.");
    
    std::string inputName;
    std::string outputName;
    float hopSizeMilliseconds;
    float spectrumSizeMilliseconds;

    options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("i,input", "Name of input file", cxxopts::value(inputName)->default_value("input.wav"))
    ("o,output", "Name of output file", cxxopts::value(outputName)->default_value("output"))
    ("t,hop_size", "Size of each time hop between spectrums in milliseconds", cxxopts::value(hopSizeMilliseconds)->default_value("10.f"))
    ("s,spectrum_size", "Size of each frame used for calculating the spectrum in milliseconds", cxxopts::value(spectrumSizeMilliseconds)->default_value("40.f"));
  
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

  auto c = Spectrogram::Coefficients();
  c.bufferSize = static_cast<int>(hopSizeMilliseconds / 1000.f * audioFileInput.getSampleRate());
  float fftSize = spectrumSizeMilliseconds / 1000.f * audioFileInput.getSampleRate();
  c.fftSize = static_cast<int>(std::pow(2, std::roundf(std::log2f(fftSize))));
  Spectrogram spectrogram(c);
  
  int nFrames = audioFileInput.getNumSamplesPerChannel() / c.bufferSize;
  //std::vector<std::vector<float>> spec(nFrames, std::vector<float>(c.fftSize/2 + 1));
  ArrayXXf spec(c.fftSize/2+1, nFrames);

  for (auto nFrame = 0; nFrame < nFrames; nFrame++)
  {
    ArrayXf frameIn = Map<ArrayXf>(&audioFileInput.samples[0][nFrame * c.bufferSize], c.bufferSize);
    //ArrayXf frameOut = Map<ArrayXf>(&(spec[nFrame][0]), c.fftSize/2+1);
    spectrogram.process(frameIn, spec.col(nFrame));
    //Map<ArrayXf>(&(spec[nFrame][0]), c.fftSize/2+1) = 20.f * Map<ArrayXf>(&(spec[nFrame][0]), c.fftSize/2+1).log10();
    //frameOut = 20 * frameOut.log10();
  }
  
  std::vector<std::vector<float>> specVec(c.fftSize/2 + 1, std::vector<float>(nFrames));
  for (auto i = 0; i < (c.fftSize/2+1); i++)
  {
    for (auto j = 0; j < nFrames; j++)
    {
      specVec[i][j] = 20.f * std::log10(spec(i,j));
    }
  }
  
  imagesc(specVec);
  gca()->color_box_range(-80, 20);
  colorbar();
  save(outputName, "jpeg"); 

  std::cout << "DONE!\n" << std::endl;

  return 0;
}