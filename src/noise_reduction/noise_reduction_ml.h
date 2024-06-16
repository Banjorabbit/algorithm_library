#pragma once
#include "algorithm_library/noise_reduction.h"
#include "framework/framework.h"
#include "onnxruntime_cxx_api.h"
#include "onnxruntime_run_options_config_keys.h"
#include "onnxruntime_session_options_config_keys.h"
#include <iostream>

class NoiseReductionML : public AlgorithmImplementation<NoiseReductionConfiguration, NoiseReductionML>
{
  public:
    NoiseReductionML(const Coefficients &c = {.nBands = 257, .nChannels = 1, .algorithmType = Coefficients::ML})
        : BaseAlgorithm{c}, mem_info{Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault)},
          inputNames{"magnitude", "phase", "time state", "gru1 state", "gru2 state"}, outputNames{"gain", "time state out", "gru1 state out", "gru2 state out"},
          session{Ort::Env(ORT_LOGGING_LEVEL_WARNING, "global_environment"), "ImageClassifier.onnx", Ort::SessionOptions()}
    {
        // setGlobalSessionOptions();

        inputNamesChar.resize(inputNames.size());
        std::transform(std::begin(inputNames), std::end(inputNames), std::begin(inputNamesChar), [&](const std::string &str) { return str.c_str(); });
        outputNamesChar.resize(outputNames.size());
        std::transform(std::begin(outputNames), std::end(outputNames), std::begin(outputNamesChar), [&](const std::string &str) { return str.c_str(); });

        magnitudeShape = {1, C.nBands};
        phaseShape = {1, C.nBands};
        timeBufferShape = {10, C.nBands};
        gru1BufferShape = {1, 32, 64};
        gru2BufferShape = {1, 32, 64};
        magnitude = Eigen::ArrayXf::Zero(C.nBands);
        phase = Eigen::ArrayXf::Zero(C.nBands);
        timeBuffer = Eigen::ArrayXXf::Zero(C.nBands, 10);
        gru1Buffer = Eigen::ArrayXXf::Zero(64, 32);
        gru2Buffer = Eigen::ArrayXXf::Zero(64, 32);

        inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, magnitude.data(), C.nBands, magnitudeShape.data(), magnitudeShape.size()));
        inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, phase.data(), C.nBands, phaseShape.data(), phaseShape.size()));
        inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, timeBuffer.data(), 10 * C.nBands, timeBufferShape.data(), timeBufferShape.size()));
        inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, gru1Buffer.data(), 32 * 64, gru1BufferShape.data(), gru1BufferShape.size()));
        inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, gru2Buffer.data(), 32 * 64, gru2BufferShape.data(), gru2BufferShape.size()));
    }

  private:
    Ort::MemoryInfo mem_info;            //= Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
    std::vector<std::string> inputNames; // = {"magnitude", "phase", "time state", "gru1 state", "gru2 state"};
    std::vector<const char *> inputNamesChar;
    std::vector<std::string> outputNames; // = {"gain", "time state out", "gru1 state out", "gru2 state out"};
    std::vector<const char *> outputNamesChar;

    std::vector<std::int64_t> magnitudeShape;
    std::vector<std::int64_t> phaseShape;
    std::vector<std::int64_t> timeBufferShape;
    std::vector<std::int64_t> gru1BufferShape;
    std::vector<std::int64_t> gru2BufferShape;
    Eigen::ArrayXf magnitude;
    Eigen::ArrayXf phase;
    Eigen::ArrayXXf timeBuffer;
    Eigen::ArrayXXf gru1Buffer;
    Eigen::ArrayXXf gru2Buffer;
    std::vector<Ort::Value> inputTensors;

    void processAlgorithm(Input xFreq, Output yFreq)
    {

        magnitude = xFreq.abs2();
        phase = xFreq.arg();
        // std::cout << "processing\n" << std::endl;
        // auto outputTensors =
        //     session.Run(Ort::RunOptions{nullptr}, inputNamesChar.data(), inputTensors.data(), inputNamesChar.size(), outputNamesChar.data(), outputNamesChar.size());
        // std::cout << "processing done!\n" << std::endl;
        // yFreq = xFreq;
    }

    friend BaseAlgorithm;

    void setGlobalSessionOptions()
    {
        Ort::ThreadingOptions threadOptions;
        threadOptions.SetGlobalInterOpNumThreads(2);
        Ort::Env env(threadOptions, ORT_LOGGING_LEVEL_WARNING, "global_environment");

        // shared memory allocator inspired from: https://github.com/microsoft/onnxruntime/blob/main/onnxruntime/test/shared_lib/test_inference.cc
        Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

        size_t maxMemory = 0;           // 0 = default
        int arenaExtendStrategy = -1;   // -1 = default
        int initialChunkSizeBytes = -1; // -1 = default
        int maxDeadBytesPerChunk = -1;  // -1 = default
        Ort::ArenaCfg arenaCfg = Ort::ArenaCfg(maxMemory, arenaExtendStrategy, initialChunkSizeBytes, maxDeadBytesPerChunk);

        env.CreateAndRegisterAllocator(memInfo, arenaCfg);
        env.DisableTelemetryEvents();

        Ort::SessionOptions sessionOptions;
        sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        sessionOptions.DisablePerSessionThreads();
        sessionOptions.DisableProfiling();
        sessionOptions.AddConfigEntry(kOrtSessionOptionsConfigUseEnvAllocators, "1");

        // session = std::make_unique<Ort::Session>(env, "model.onnx", sessionOptions);

        // inspired from: https://github.com/microsoft/onnxruntime/issues/11627
        // runOption.AddConfigEntry(kOrtRunOptionsConfigEnableMemoryArenaShrinkage, "cpu:0;gpu:0");

        // session->Run(runOption, nullptr, nullptr);
    }

    // std::unique_ptr<Ort::Session> session;
    Ort::Session session;

    bool isCoefficientsValid() const final
    {
        bool flag = C.algorithmType == C.ML;
        flag &= C.nBands == 257;
        flag &= C.nChannels == 1;
        return flag;
    }

    friend BaseAlgorithm;
};
