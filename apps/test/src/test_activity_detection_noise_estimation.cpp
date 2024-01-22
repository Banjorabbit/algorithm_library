#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "activity_detection_frequency_domain/activity_detection_noise_estimation.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(ActivityDetectionNoiseEstimation, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<ActivityDetectionNoiseEstimation>());
}

TEST(ActivityDetectionNoiseEstimation, ActivityDetect)
		{
			ActivityDetectionNoiseEstimation vad;
			auto c = vad.getCoefficients();
			c.nBands = 256;
			vad.setCoefficients(c);

			// run for 3 seconds with random noise
			int nEnd = int(3.0f * c.filterbankRate);
			bool activity = false;
            Eigen::ArrayXXf noise(c.nBands, c.nChannels);
			for (auto n = 0; n < nEnd; n++)
			{
				ArrayXXcf noise = 1e-2f * ArrayXXcf::Random(c.nBands, c.nChannels);
				vad.process(noise, activity);
			}

			// Add activity at certain times and test activity detection
			for (auto n = 0; n < 51; n++)
			{
				ArrayXXcf noise = 1e-2f * ArrayXXcf::Random(c.nBands, c.nChannels);
				switch (n)
				{
				case 0: noise.col(0) = ArrayXcf::Ones(c.nBands); break;
				case 10:noise.col(1) = ArrayXcf::Ones(c.nBands); break;
				case 20:noise.col(0).segment(20, 45) = ArrayXcf::Constant(45, 2.f); break;
				case 30:noise.col(1).segment(70, 45) = ArrayXcf::Constant(45, 2.f); break;
				case 40:noise.col(0).segment(133, 45) = ArrayXcf::Constant(45, 2.f); break;
				case 50:noise.col(1).segment(201, 45) = ArrayXcf::Constant(45, 2.f); break;
				default: break;
				}
				vad.process(noise, activity);

				if (n % 10 == 0)
				{
					EXPECT_TRUE(activity);
				}
				else {
					EXPECT_FALSE(activity);
				}
			}
		}