/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
#include <utility>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Alfano2005.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Frisbee2015.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Utility.test.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::iau::Theory;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::hardbody::Spherical;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Alfano2005;
using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Frisbee2015;
using ostk::astrodynamics::trajectory::State;

namespace
{

State removeCovariance(const State& aState)
{
    return aState.withCovariance(std::nullopt);
}

// Equivalent to NASA CARA passing a zero covariance for Object 1
CloseApproach removeObject1Covariance(const CloseApproach& aCloseApproach)
{
    return {removeCovariance(aCloseApproach.getObject1State()), aCloseApproach.getObject2State()};
}

// Equivalent to NASA CARA passing a zero covariance for Object 2
CloseApproach removeObject2Covariance(const CloseApproach& aCloseApproach)
{
    return {aCloseApproach.getObject1State(), removeCovariance(aCloseApproach.getObject2State())};
}

bool containsMessage(const Array<String>& aMessageArray, const String& aMessageCore)
{
    return std::any_of(
        aMessageArray.begin(),
        aMessageArray.end(),
        [&aMessageCore](const String& aMessage) -> bool
        {
            return aMessage.find(aMessageCore) != std::string::npos;
        }
    );
}

const PoCTestCase& getTestCase(const String& aName)
{
    static const Array<PoCTestCase> testCases = PoCTestCase::BuildNASACARAFrisbeeMaxPcTestCases();

    const auto testCaseIt = std::find_if(
        testCases.begin(),
        testCases.end(),
        [&aName](const PoCTestCase& aTestCase) -> bool
        {
            return aTestCase.name == aName;
        }
    );

    if (testCaseIt == testCases.end())
    {
        throw ostk::core::error::runtime::Wrong("Test case", aName);
    }

    return *testCaseIt;
}

// Manufactured case corresponding to Frisbee's example (FrisbeeMaxPcTestCase_Test01.cdm), with only the Object 1
// covariance known
CloseApproach buildFrisbeeExample()
{
    return getTestCase("NASA CARA Frisbee Max Pc Test Case Frisbee Example | 5.0m HBR | No Object 2 Covariance")
        .closeApproach;
}

// Single covariance Case 1-1 (SingleCovTestCase1-1.cdm) with both covariances, each test case having one of them
CloseApproach buildSingleCovCase1_1()
{
    return {
        getTestCase("NASA CARA Frisbee Max Pc Test Case 1-1 | 20.0m HBR | No Object 2 Covariance")
            .closeApproach.getObject1State(),
        getTestCase("NASA CARA Frisbee Max Pc Test Case 1-1 | 20.0m HBR | No Object 1 Covariance")
            .closeApproach.getObject2State(),
    };
}

// Ratio of the combined hard-body radius to the smaller standard deviation of the critical combined covariance, as
// stated in the test case comment (e.g. "HBR / sigma_min = 2.81")
double getHBRToSigmaMinRatio(const PoCTestCase& aTestCase)
{
    const String prefix = "HBR / sigma_min = ";
    const std::size_t position = aTestCase.comment.find(prefix);

    if (position == std::string::npos)
    {
        throw ostk::core::error::runtime::Undefined("HBR / sigma_min");
    }

    return std::stod(aTestCase.comment.substr(position + prefix.size()));
}

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015 : public ::testing::Test
{
   protected:
    const Derived relativeSpeedThreshold_ = Derived(100.0, Derived::Unit::MeterPerSecond());
    const Frisbee2015 algorithm_ = {true, true, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015, Constructor)
{
    {
        EXPECT_NO_THROW(Frisbee2015());
    }

    {
        EXPECT_NO_THROW(Frisbee2015(true, true, relativeSpeedThreshold_, Frame::GCRF(), std::make_shared<Alfano2005>())
        );
    }

    {
        EXPECT_NO_THROW(Frisbee2015(true, true, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt));
    }

    {
        EXPECT_NO_THROW(Frisbee2015(
            true, true, Derived(10.0, Derived::Unit::MeterPerSecond()), Frame::J2000(Theory::IAU_2006), std::nullopt
        ));
    }

    {
        for (const bool considerObject2Covariance : {true, false})
        {
            for (const bool considerWorstCaseWhenBothCovariancesArePresent : {true, false})
            {
                EXPECT_NO_THROW(Frisbee2015(
                    considerObject2Covariance,
                    considerWorstCaseWhenBothCovariancesArePresent,
                    Derived(10.0, Derived::Unit::MeterPerSecond()),
                    Frame::J2000(Theory::IAU_2006),
                    std::nullopt
                ));
            }
        }
    }

    {
        EXPECT_THROW(
            try {
                Frisbee2015(true, true, relativeSpeedThreshold_, nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Frisbee2015(true, true, relativeSpeedThreshold_, Frame::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Frisbee2015(true, true, relativeSpeedThreshold_, Frame::ITRF());
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("Frame = ITRF is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015, GetName)
{
    {
        EXPECT_EQ("Frisbee 2015 (Closed-form)", algorithm_.getName());
    }

    {
        const Frisbee2015 algorithm = {};

        EXPECT_EQ("Frisbee 2015 (Alfano 2005)", algorithm.getName());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015, IsMaximum)
{
    {
        EXPECT_TRUE(algorithm_.isMaximum());
    }

    {
        const Frisbee2015 algorithm = {};

        EXPECT_TRUE(algorithm.isMaximum());
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    IdentifyUnsatisfiedAssumptions
)
{
    const Frisbee2015 algorithmWithoutObject2Covariance = {false};
    const Frisbee2015 algorithmWithoutWorstCase = {true, false};

    // Single covariance Case 1-1 is a high relative speed encounter (~7 km/s), with both covariances
    const CloseApproach closeApproach = buildSingleCovCase1_1();

    // All assumptions satisfied
    {
        for (const Frisbee2015& algorithm : {algorithm_, algorithmWithoutWorstCase})
        {
            EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(closeApproach).isEmpty());
            EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(removeObject1Covariance(closeApproach)).isEmpty());
            EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(removeObject2Covariance(closeApproach)).isEmpty());
        }

        EXPECT_TRUE(algorithmWithoutObject2Covariance.identifyUnsatisfiedAssumptions(closeApproach).isEmpty());
        EXPECT_TRUE(algorithmWithoutObject2Covariance
                        .identifyUnsatisfiedAssumptions(removeObject2Covariance(closeApproach))
                        .isEmpty());
    }

    // No covariance for Object 1, without considering Object 2 covariance
    {
        const Array<String> unsatisfiedAssumptions =
            algorithmWithoutObject2Covariance.identifyUnsatisfiedAssumptions(removeObject1Covariance(closeApproach));

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // No covariance for either object
    {
        for (const Frisbee2015& algorithm : {algorithm_, algorithmWithoutWorstCase})
        {
            const Array<String> unsatisfiedAssumptions =
                algorithm.identifyUnsatisfiedAssumptions(removeObject1Covariance(removeObject2Covariance(closeApproach))
                );

            EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
            EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No object has position uncertainty"));
        }

        const Array<String> unsatisfiedAssumptions = algorithmWithoutObject2Covariance.identifyUnsatisfiedAssumptions(
            removeObject1Covariance(removeObject2Covariance(closeApproach))
        );

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // Relative speed below the threshold
    {
        const Frisbee2015 algorithm = {true, true, Derived(20000.0, Derived::Unit::MeterPerSecond())};

        const Array<String> unsatisfiedAssumptions = algorithm.identifyUnsatisfiedAssumptions(closeApproach);

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "Relative speed"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "is below the threshold"));
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    ComputeProbabilityOfCollision_UndefinedHardBody
)
{
    const CloseApproach closeApproach = buildFrisbeeExample();
    const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(2.0);

    {
        EXPECT_THROW(
            try {
                algorithm_.computeProbabilityOfCollision(closeApproach, nullptr, hardBodySPtr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Hard body} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                algorithm_.computeProbabilityOfCollision(closeApproach, hardBodySPtr, nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Hard body} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    ComputeProbabilityOfCollision_Behaviour
)
{
    // Single covariance Case 1-1 with Object 1 and Object 2 swapped, so that the maximum probability of collision using
    // the Object 2 covariance (Pc2) is the worst case
    const CloseApproach closeApproach = buildSingleCovCase1_1().swap();
    const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(20.0 / 2.0);

    // Maximum probabilities of collision using a single covariance: Pc1 with the Object 1 covariance only, Pc2 with the
    // Object 2 covariance only (computed by swapping the objects back, so that it is the Object 1 covariance)
    const double probabilityOfCollision1 =
        algorithm_.computeProbabilityOfCollision(removeObject2Covariance(closeApproach), hardBodySPtr, hardBodySPtr);
    const double probabilityOfCollision2 = algorithm_.computeProbabilityOfCollision(
        removeObject2Covariance(closeApproach.swap()), hardBodySPtr, hardBodySPtr
    );

    ASSERT_GT(probabilityOfCollision2, probabilityOfCollision1);

    // Both covariances, but only the Object 1 covariance is used
    {
        const Frisbee2015 algorithm = {true, false, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};

        EXPECT_EQ(
            algorithm.computeProbabilityOfCollision(closeApproach, hardBodySPtr, hardBodySPtr), probabilityOfCollision1
        );
    }

    // Both covariances, and the worst case is used
    {
        const Frisbee2015 algorithm = {true, true, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};

        EXPECT_EQ(
            algorithm.computeProbabilityOfCollision(closeApproach, hardBodySPtr, hardBodySPtr), probabilityOfCollision2
        );
    }

    // Only the Object 2 covariance, which is allowed to be used
    {
        const Frisbee2015 algorithm = {true, false, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};

        EXPECT_EQ(
            algorithm.computeProbabilityOfCollision(removeObject1Covariance(closeApproach), hardBodySPtr, hardBodySPtr),
            probabilityOfCollision2
        );
    }

    // Only the Object 2 covariance, which is allowed to be used, and the worst case is used
    {
        const Frisbee2015 algorithm = {true, true, relativeSpeedThreshold_, Frame::GCRF(), std::nullopt};

        EXPECT_EQ(
            algorithm.computeProbabilityOfCollision(removeObject1Covariance(closeApproach), hardBodySPtr, hardBodySPtr),
            probabilityOfCollision2
        );
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    ComputeProbabilityOfCollision_FrisbeePaperExample
)
{
    // Example of the basic method from Frisbee (2015), Equations 11 to 15 and Table 1. The NASA CARA Frisbee example
    // CDM reproduces the geometry of the paper, with only the Object 1 covariance known: the collision plane miss
    // position and Object 1 covariance are, up to an in-plane rotation (which leaves the probability of collision
    // unchanged) and the rounding of the CDM [m, m^2]:
    //
    //     r_rel = [1000, 200], C_AC = diag(722500, 2500)
    const CloseApproach closeApproach = buildFrisbeeExample();

    // Equation 15b: Pc = R_HB^2 * e^(-1/2) / (20000 * sqrt(314))
    const double probabilityOfCollisionPerSquareMeter = std::exp(-0.5) / (20000.0 * std::sqrt(314.0));

    // Table 1: combined hard-body radius [m] and probability of collision (reported to 1e-6)
    const Array<std::pair<double, double>> table = {
        {5.0, 0.000043},
        {10.0, 0.000171},
        {20.0, 0.000685},
        {25.0, 0.001070},
    };

    for (const auto& [combinedHardBodyRadius, tableProbabilityOfCollision] : table)
    {
        const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(combinedHardBodyRadius / 2.0);

        const double probabilityOfCollision =
            algorithm_.computeProbabilityOfCollision(closeApproach, hardBodySPtr, hardBodySPtr);

        const double expectedProbabilityOfCollision =
            combinedHardBodyRadius * combinedHardBodyRadius * probabilityOfCollisionPerSquareMeter;

        // The relative tolerance accounts for the rounding of the CDM states (~2e-6 on the probability of collision)
        EXPECT_NEAR(probabilityOfCollision, expectedProbabilityOfCollision, expectedProbabilityOfCollision * 1.0e-5)
            << "Combined hard-body radius: " << combinedHardBodyRadius;
        EXPECT_NEAR(probabilityOfCollision, tableProbabilityOfCollision, 5.0e-7)
            << "Combined hard-body radius: " << combinedHardBodyRadius;
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015, ComputeProbabilityAnalysis
)
{
    // The probability of collision is already an upper bound: the analysis is not applicable
    const CloseApproach closeApproach = buildFrisbeeExample();
    const Shared<const Spherical> hardBodySPtr = std::make_shared<Spherical>(2.0);

    {
        EXPECT_THROW(
            try {
                ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                    std::make_shared<Frisbee2015>(algorithm_), closeApproach, hardBodySPtr, hardBodySPtr
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    "The probability of collision is already an upper bound: the analysis is not applicable.",
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                std::make_shared<Frisbee2015>(), closeApproach, hardBodySPtr, hardBodySPtr
            ),
            ostk::core::error::RuntimeError
        );
    }
}

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015_Parameterized
    : public ::testing::TestWithParam<PoCTestCase>
{
   protected:
    const double nominalTolerance_ = 0.01;
    const double ignoreClosedFormForHBRToSigmaMinRatioGreaterThan_ = 0.3;
    const double useFallbackAlgorithmForHBRToSigmaMinRatioGreaterThan_ = 1.3;
    const double fallbackAlgorithmicTolerance_ = 0.15;
};

INSTANTIATE_TEST_SUITE_P(
    Cases,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015_Parameterized,
    ::testing::ValuesIn(PoCTestCase::BuildNASACARAFrisbeeMaxPcTestCases()),
    [](const ::testing::TestParamInfo<PoCTestCase>& aParamInfo) -> std::string
    {
        return ToTestName(aParamInfo.param.name);
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015_Parameterized,
    ComputeProbabilityOfCollision
)
{
    const PoCTestCase& testCase = GetParam();

    const double hbrToSigmaMinRatio = getHBRToSigmaMinRatio(testCase);

    // Closed form (Frisbee 2015, Equation 5)
    {
        const Frisbee2015 algorithm = {
            true, true, Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::GCRF(), std::nullopt
        };

        // The closed form degrades as the smaller collision plane standard deviation nears or becomes smaller
        // than the combined hard-body radius (Frisbee, 2015), hence no probability of collision is asserted
        if (hbrToSigmaMinRatio <= ignoreClosedFormForHBRToSigmaMinRatioGreaterThan_)
        {
            EXPECT_NEAR(
                algorithm.computeProbabilityOfCollision(
                    testCase.closeApproach, testCase.hardBody1SPtr, testCase.hardBody2SPtr
                ),
                testCase.expectedProbabilityOfCollision,
                testCase.expectedProbabilityOfCollision * nominalTolerance_
            ) << "Closed form: "
              << testCase.getDescription();
        }
    }

    // Alfano 2005 numerical integration of the critical degenerate covariance
    {
        const Frisbee2015 algorithm = {
            true, true, Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::GCRF(), std::make_shared<Alfano2005>()
        };

        // The NASA CARA expected value uses an equal-area square hard body (squareEquArea),
        // which differs increasingly from the circle that we use here as HBR-to-sigma_min increases.
        // So we use a higher tolerance for these cases.
        const double tolerance = (hbrToSigmaMinRatio > useFallbackAlgorithmForHBRToSigmaMinRatioGreaterThan_)
                                   ? fallbackAlgorithmicTolerance_
                                   : nominalTolerance_;

        EXPECT_NEAR(
            algorithm.computeProbabilityOfCollision(
                testCase.closeApproach, testCase.hardBody1SPtr, testCase.hardBody2SPtr
            ),
            testCase.expectedProbabilityOfCollision,
            testCase.expectedProbabilityOfCollision * tolerance
        ) << "Alfano 2005: "
          << testCase.getDescription();
    }
}
