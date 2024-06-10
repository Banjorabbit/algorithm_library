#pragma once
#include "algorithm_library/noise_reduction.h"
#include "framework/framework.h"
#include "onnxruntime_cxx_api.h"
#include "onnxruntime_run_options_config_keys.h"
#include "onnxruntime_session_options_config_keys.h"

class NoiseReductionML : public AlgorithmImplementation<NoiseReductionConfiguration, NoiseReductionML>
{
  public:
    NoiseReductionML(const Coefficients &c = Coefficients()) : BaseAlgorithm{c} { setGlobalSessionOptions(); }
    // {
    //     // Load the model
    //     Ort::SessionOptions session_options;
    // session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    //     session_options.SetIntraOpNumThreads(1);
    //     session_options.SetInterOpNumThreads(1);
    //     session_options.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);
    //     session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    //     session_options.SetOptimizedModelFilePath("model.onnx");
    // session = Ort::Session(env(), "model.onnx", session_options);
    // }

  private:
    void processAlgorithm(Input xFreq, Output yFreq) { yFreq = xFreq; }

    friend BaseAlgorithm;

    void setGlobalSessionOptions()
    {
        // inspired from: https://github.com/microsoft/onnxruntime/issues/14018
        std::unique_ptr<Ort::Env> env;
        const OrtApi *g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
        std::unique_ptr<OrtStatus, decltype(OrtApi::ReleaseStatus)> st_ptr(nullptr, g_ort->ReleaseStatus);
        OrtThreadingOptions *tp_options;

        st_ptr.reset(g_ort->CreateThreadingOptions(&tp_options));
        st_ptr.reset(g_ort->SetGlobalIntraOpNumThreads(tp_options, 2));
        env.reset(new Ort::Env(tp_options, ORT_LOGGING_LEVEL_FATAL, "Default"));
        g_ort->ReleaseThreadingOptions(tp_options);

        Ort::SessionOptions sessionOptions;
        sessionOptions.DisablePerSessionThreads();
        sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        // sessionOptions.DisableCpuMemArena();
        // sessionOptions.DisableMemPattern();
        sessionOptions.DisableProfiling();
        sessionOptions.AddConfigEntry(
            kOrtSessionOptionsConfigUseEnvAllocators,
            "1"); // inspired from: https://github.com/microsoft/onnxruntime/blob/821baa5b839b04581ab9b7a8b8cd44bb5b002c5a/onnxruntime/test/shared_lib/test_inference.cc#L2035
        session = new Ort::Session(*env.get(), "model.onnx", sessionOptions);

        // inspired from: https://github.com/microsoft/onnxruntime/issues/11627
        runOption.AddConfigEntry(kOrtRunOptionsConfigEnableMemoryArenaShrinkage, "cpu:0;gpu:0");

        // session->Run(runOption, nullptr, nullptr);
    }
    Ort::Session *session;
    Ort::RunOptions runOption;
};