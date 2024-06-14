#include "algorithm_library/dc_remover.h"
#include "integration_test.h"

// derived test class for DCRemover
class TestDCRemover : public TestFixture<DCRemover>
{
  public:
    TestDCRemover() : TestFixture("createDCRemoverInstance") {}
};

TEST_F(TestDCRemover, printCoefficients)
{

    auto c = algoPtr->getCoefficients();
    nlohmann::json j = c;
    fmt::print("Coefficients: {}\n", j.dump(4));
}

TEST_F(TestDCRemover, printIsConfigurationValid)
{
    fmt::print("isConfigurationValid: {}\n", algoPtr->isConfigurationValid());
    EXPECT_TRUE(algoPtr->isConfigurationValid());
}