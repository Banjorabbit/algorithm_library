#include "onnxruntime_cxx_api.h"
#include "onnxruntime_run_options_config_keys.h"
#include "onnxruntime_session_options_config_keys.h"
#include "unit_test.h"
#include "gtest/gtest.h"

TEST(ONNXRUNTIME, EnvironmentStartup)
{
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

    EXPECT_TRUE(env != nullptr);
    env.DisableTelemetryEvents();
    fmt::print("static size of environment: {} bytes\n", sizeof(env));
}

// test that the session can be created with global threads and shared allocator
TEST(ONNXRUNTIME, SessionWithGlobalThreadOptions)
{
    Ort::ThreadingOptions threadOptions;
    threadOptions.SetGlobalInterOpNumThreads(2);
    Ort::Env env(threadOptions, ORT_LOGGING_LEVEL_WARNING, "test");

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
    Ort::Session session(env, "model.onnx", sessionOptions);

    EXPECT_TRUE(session != nullptr);
}

// pretty prints a shape dimension vector
std::string printShape(const std::vector<std::int64_t> &v)
{
    std::stringstream ss("");
    for (std::size_t i = 0; i < v.size() - 1; i++)
        ss << v[i] << "x";
    ss << v[v.size() - 1];
    return ss.str();
}

int calculateProduct(const std::vector<std::int64_t> &v)
{
    int total = 1;
    for (auto &i : v)
        total *= i;
    return total;
}

TEST(ONNXRUNTIME, RunONNXModel)
{
    Ort::ThreadingOptions threadOptions;
    threadOptions.SetGlobalInterOpNumThreads(2);
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
    Ort::Session session(env, "model.onnx", Ort::SessionOptions());

    Ort::AllocatorWithDefaultOptions allocator;
    auto metaData = session.GetModelMetadata();
    std::cout << "Producer name: " << metaData.GetProducerNameAllocated(allocator).get() << std::endl;

    // input/output names has to be cast to std:string before passing const char* const* to Run()
    const int nInput = 1; // number of inputs
    auto inputName = static_cast<std::string>(session.GetInputNameAllocated(0, allocator).get());
    const char *inputNamePtr = inputName.c_str();

    const int nOutput = 1; // number of outputs
    auto outputName = static_cast<std::string>(session.GetOutputNameAllocated(0, allocator).get());
    const char *outputNamePtr = outputName.c_str();

    std::vector<std::int64_t> inputShapes = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape(); // vector of size 3: 1x1x1
    int inputValueSize = calculateProduct(inputShapes);                                                         // 1x1x1 = 1 (total number of elements in tensor)

    std::vector<std::int64_t> outputShapes = session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape(); // vector of size 3: 1x1x1

    // create random input values
    std::vector<float> inputValues(inputValueSize);
    std::generate(inputValues.begin(), inputValues.end(), [&] { return rand() % 255; });

    // create input tensor and copy values into it
    Ort::MemoryInfo mem_info = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
    Ort::Value tensor = Ort::Value::CreateTensor<float>(mem_info, inputValues.data(), inputValueSize, inputShapes.data(), inputShapes.size());

    // print input/output node names and shapes
    std::cout << "Input node name/shape (" << session.GetInputCount() << "):" << std::endl;
    std::cout << "\t" << inputName << " : " << printShape(inputShapes) << std::endl;
    std::cout << "Output node name/shape (" << session.GetOutputCount() << "):" << std::endl;
    std::cout << "\t" << outputName << " : " << printShape(outputShapes) << std::endl;
    std::cout << "Input tensor shape: " << printShape(tensor.GetTensorTypeAndShapeInfo().GetShape()) << std::endl;

    std::cout << "Running inference..." << std::endl;
    auto output_tensors = session.Run(Ort::RunOptions{nullptr}, &inputNamePtr, &tensor, nInput, &outputNamePtr, nOutput);
    std::cout << "Inference done." << std::endl;
}