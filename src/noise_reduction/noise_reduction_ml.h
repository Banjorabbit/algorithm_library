#pragma once
#include "algorithm_library/noise_reduction.h"
#include "framework/framework.h"
#include "onnxruntime_cxx_api.h"

class NoiseReductionML : public AlgorithmImplementation<NoiseReductionConfiguration, NoiseReductionML>
{
  public:
    NoiseReductionML(const Coefficients &c = Coefficients()) : BaseAlgorithm{c}, session{env(), "model.onnx", Ort::SessionOptions{}} {}
    // {
    //     // Load the model
    //     Ort::SessionOptions session_options;
    //     session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    //     session_options.SetIntraOpNumThreads(1);
    //     session_options.SetInterOpNumThreads(1);
    //     session_options.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);
    //     session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    //     session_options.SetOptimizedModelFilePath("model.onnx");
    //     session = Ort::Session(env(), "model.onnx", session_options);
    // }

  private:
    void processAlgorithm(Input xFreq, Output yFreq) { yFreq = xFreq; }

    friend BaseAlgorithm;

    Ort::Env env();
    Ort::Session session;
};