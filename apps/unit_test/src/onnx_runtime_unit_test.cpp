#include "onnxruntime_cxx_api.h"
#include "unit_test.h"
#include "gtest/gtest.h"

TEST(ONNXRUNTIME, EnvironmentStartup)
{
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

    EXPECT_TRUE(env != nullptr);
    env.DisableTelemetryEvents();
    fmt::print("static size of environment: {} bytes\n", sizeof(env));
}
