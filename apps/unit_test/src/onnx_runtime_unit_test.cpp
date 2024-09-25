#include "onnxruntime_cxx_api.h"
#include "onnxruntime_run_options_config_keys.h"
#include "onnxruntime_session_options_config_keys.h"
#include "unit_test.h"
#include "gtest/gtest.h"

// startup environment with verbose logging level
TEST(ONNXRUNTIME, EnvironmentStartup)
{
    Ort::Env env(ORT_LOGGING_LEVEL_VERBOSE, "test");

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

    std::string mPath({"model.onnx"});
    std::basic_string<ORTCHAR_T> modelPath(mPath.begin(), mPath.end()); // ORTCHAR_T is defined in onnxruntime_c_api.h and is wchar_t on Windows, and char_t on Linux
    Ort::Session session(env, modelPath.c_str(), sessionOptions);

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
        total *= static_cast<int>(i);
    return total;
}

TEST(ONNXRUNTIME, RunEmptyONNXModel)
{
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
    std::string mPath({"model.onnx"});
    std::basic_string<ORTCHAR_T> modelPath(mPath.begin(), mPath.end()); // ORTCHAR_T is defined in onnxruntime_c_api.h and is wchar_t on Windows, and char_t on Linux
    Ort::Session session(env, modelPath.c_str(), Ort::SessionOptions());

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
    std::generate(inputValues.begin(), inputValues.end(), [&] { return static_cast<float>(rand() % 255); });

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

// test that the ImageClassifier.onnx model can be loaded and run
TEST(ONNXRUNTIME, RunMLModel)
{
    const int nBands = 257;

    Ort::MemoryInfo mem_info = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

    size_t maxMemory = 0;           // 0 = default
    int arenaExtendStrategy = -1;   // -1 = default
    int initialChunkSizeBytes = -1; // -1 = default
    int maxDeadBytesPerChunk = -1;  // -1 = default
    Ort::ArenaCfg arenaCfg = Ort::ArenaCfg(maxMemory, arenaExtendStrategy, initialChunkSizeBytes, maxDeadBytesPerChunk);

    Ort::ThreadingOptions threadOptions;
    threadOptions.SetGlobalInterOpNumThreads(2);
    Ort::Env env(threadOptions, ORT_LOGGING_LEVEL_WARNING, "global_environment");

    env.CreateAndRegisterAllocator(mem_info, arenaCfg);
    env.DisableTelemetryEvents();

    Ort::SessionOptions sessionOptions;
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    sessionOptions.DisablePerSessionThreads();
    sessionOptions.DisableProfiling();
    sessionOptions.AddConfigEntry(kOrtSessionOptionsConfigUseEnvAllocators, "1");

    std::string mPath({"ImageClassifier.onnx"});
    std::basic_string<ORTCHAR_T> modelPath(mPath.begin(), mPath.end()); // ORTCHAR_T is defined in onnxruntime_c_api.h and is wchar_t on Windows, and char_t on Linux
    Ort::Session session(env, modelPath.c_str(), sessionOptions);

    std::vector<std::string> inputNames = {"magnitude", "phase", "time state", "gru1 state", "gru2 state"};
    std::vector<const char *> inputNamesChar;
    std::vector<std::string> outputNames = {"gain", "time state out", "gru1 state out", "gru2 state out"};
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

    inputNamesChar.resize(inputNames.size());
    std::transform(std::begin(inputNames), std::end(inputNames), std::begin(inputNamesChar), [&](const std::string &str) { return str.c_str(); });
    outputNamesChar.resize(outputNames.size());
    std::transform(std::begin(outputNames), std::end(outputNames), std::begin(outputNamesChar), [&](const std::string &str) { return str.c_str(); });

    magnitudeShape = {1, nBands};
    phaseShape = {1, nBands};
    timeBufferShape = {10, nBands};
    gru1BufferShape = {1, 32, 64};
    gru2BufferShape = {1, 32, 64};
    magnitude = Eigen::ArrayXf::Zero(nBands);
    phase = Eigen::ArrayXf::Zero(nBands);
    timeBuffer = Eigen::ArrayXXf::Zero(nBands, 10);
    gru1Buffer = Eigen::ArrayXXf::Zero(64, 32);
    gru2Buffer = Eigen::ArrayXXf::Zero(64, 32);

    inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, magnitude.data(), nBands, magnitudeShape.data(), magnitudeShape.size()));
    inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, phase.data(), nBands, phaseShape.data(), phaseShape.size()));
    inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, timeBuffer.data(), 10 * nBands, timeBufferShape.data(), timeBufferShape.size()));
    inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, gru1Buffer.data(), 32 * 64, gru1BufferShape.data(), gru1BufferShape.size()));
    inputTensors.emplace_back(Ort::Value::CreateTensor<float>(mem_info, gru2Buffer.data(), 32 * 64, gru2BufferShape.data(), gru2BufferShape.size()));

    magnitude = Eigen::ArrayXf::Random(nBands).abs2();
    phase = Eigen::ArrayXf::Random(nBands);
    std::cout << "processing\n" << std::endl;
    auto outputTensors =
        session.Run(Ort::RunOptions{nullptr}, inputNamesChar.data(), inputTensors.data(), inputNamesChar.size(), outputNamesChar.data(), outputNamesChar.size());
    std::cout << "processing done!\n" << std::endl;

    std::cout << "processing\n" << std::endl;
    outputTensors = session.Run(Ort::RunOptions{nullptr}, inputNamesChar.data(), inputTensors.data(), inputNamesChar.size(), outputNamesChar.data(), outputNamesChar.size());
    std::cout << "processing done!\n" << std::endl;
}