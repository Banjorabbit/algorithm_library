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

    Ort::SessionOptions sessionOptions;
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
    sessionOptions.DisablePerSessionThreads();
    sessionOptions.DisableProfiling();
    sessionOptions.AddConfigEntry(kOrtSessionOptionsConfigUseEnvAllocators, "1");
    Ort::Session session(env, "model.onnx", sessionOptions);

    EXPECT_TRUE(session != nullptr);
}