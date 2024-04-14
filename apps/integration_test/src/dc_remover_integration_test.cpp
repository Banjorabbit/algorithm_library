#include "integration_test.h"
#include "algorithm_library/dc_remover.h"

// derived test class for DCRemover
class TestDCRemover : public TestFixture<DCRemover>
{
public:
	TestDCRemover() : TestFixture("createDCRemoverInstance") {}
};

TEST_F(TestDCRemover, printNChannels) {

	auto c = algoPtr->getCoefficients();
	nlohmann::json j = c;
	fmt::print("nChannels: {}\n", j.dump(4));
}

