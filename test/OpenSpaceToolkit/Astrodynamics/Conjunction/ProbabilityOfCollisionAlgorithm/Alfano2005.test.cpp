/// Apache License 2.0

#include <algorithm>
#include <cmath>
#include <optional>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Alfano2005.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Utility.test.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::iau::Theory;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::hardbody::Spherical;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Alfano2005;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::uncertainty::Covariance;

namespace
{

State removeCovariance(const State& aState)
{
    return aState.withCovariance(std::nullopt);
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

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005 : public ::testing::Test
{
   protected:
    const Alfano2005 algorithm_ = {};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005, Constructor)
{
    {
        EXPECT_NO_THROW(Alfano2005());
    }

    {
        EXPECT_NO_THROW(Alfano2005(Derived(10.0, Derived::Unit::MeterPerSecond()), Frame::J2000(Theory::IAU_2006)));
    }

    {
        EXPECT_THROW(
            try {
                Alfano2005(Derived(100.0, Derived::Unit::MeterPerSecond()), nullptr);
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
                Alfano2005(Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::Undefined());
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
                Alfano2005(Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::ITRF());
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("Frame = ITRF is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005, IsMaximum)
{
    EXPECT_FALSE(algorithm_.isMaximum());
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005,
    IdentifyUnsatisfiedAssumptions
)
{
    // Alfano 2009 Test Case 3 has a relative speed of ~16 m/s: below the default threshold (100 m/s), above 10 m/s
    const Alfano2005 algorithm = {Derived(10.0, Derived::Unit::MeterPerSecond())};

    const CloseApproach closeApproach = PoCTestCase::BuildAlfano2009PoCTestCases()[2].closeApproach;
    const State object1State = closeApproach.getObject1State();
    const State object2State = closeApproach.getObject2State();

    // All assumptions satisfied
    {
        EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(closeApproach).isEmpty());
    }

    // No covariance for Object 1
    {
        const Array<String> unsatisfiedAssumptions =
            algorithm.identifyUnsatisfiedAssumptions({removeCovariance(object1State), object2State});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // No covariance for Object 2
    {
        const Array<String> unsatisfiedAssumptions =
            algorithm.identifyUnsatisfiedAssumptions({object1State, removeCovariance(object2State)});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 2"));
    }

    // Covariance without position uncertainty for Object 1
    {
        const State object1StateWithVelocityCovarianceOnly = removeCovariance(object1State)
                                                                 .withCovariance(Covariance(
                                                                     object1State.accessInstant(),
                                                                     MatrixXd::Identity(3, 3),
                                                                     object1State.accessFrame(),
                                                                     {CartesianVelocity::Default()}
                                                                 ));

        const Array<String> unsatisfiedAssumptions =
            algorithm.identifyUnsatisfiedAssumptions({object1StateWithVelocityCovarianceOnly, object2State});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // Relative speed below the (default) threshold
    {
        const Array<String> unsatisfiedAssumptions = algorithm_.identifyUnsatisfiedAssumptions(closeApproach);

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "Relative speed"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "is below the threshold"));
    }

    // All assumptions unsatisfied
    {
        const Array<String> unsatisfiedAssumptions =
            algorithm_.identifyUnsatisfiedAssumptions({removeCovariance(object1State), removeCovariance(object2State)});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 3);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 2"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "is below the threshold"));
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005,
    ComputeProbabilityOfCollision_IdenticalOrbits
)
{
    // Identical orbits (Alfano 2009 Test Case 12, i.e. Object 1 of Alfano 2009 Test Case 11 on both sides): zero miss
    // distance and zero relative velocity, the encounter plane is undefined
    const State objectState = PoCTestCase::BuildAlfano2009PoCTestCases()[10].closeApproach.getObject1State();
    const CloseApproach closeApproach = {objectState, objectState};

    EXPECT_THROW(
        try {
            algorithm_.computeProbabilityOfCollision(
                closeApproach, std::make_shared<Spherical>(2.0), std::make_shared<Spherical>(2.0)
            );
        } catch (const ostk::core::error::RuntimeError& e) {
            EXPECT_EQ("Relative velocity is zero.", e.getMessage());
            throw;
        },
        ostk::core::error::RuntimeError
    );
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005,
    ComputeProbabilityOfCollision_UndefinedHardBody
)
{
    const CloseApproach closeApproach = PoCTestCase::BuildAlfano2009PoCTestCases()[2].closeApproach;
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

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_Parameterized
    : public ::testing::TestWithParam<PoCTestCase>
{
   protected:
    const Alfano2005 algorithm_ = {};
    const double relativeTolerance_ = 0.01;
};

INSTANTIATE_TEST_SUITE_P(
    Cases,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_Parameterized,
    ::testing::ValuesIn(PoCTestCase::BuildAlfano2009PoCTestCases()),
    [](const ::testing::TestParamInfo<PoCTestCase>& aParamInfo) -> std::string
    {
        return ToTestName(aParamInfo.param.name);
    }
);

INSTANTIATE_TEST_SUITE_P(
    Orekit,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_Parameterized,
    ::testing::ValuesIn(PoCTestCase::BuildOrekitRotatedEncounterFrameTestCases("EXPECTED_ALFANO2005_POC")),
    [](const ::testing::TestParamInfo<PoCTestCase>& aParamInfo) -> std::string
    {
        return ToTestName(aParamInfo.param.name);
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_Parameterized,
    ComputeProbabilityOfCollision
)
{
    const PoCTestCase& testCase = GetParam();

    const double probabilityOfCollision = algorithm_.computeProbabilityOfCollision(
        testCase.closeApproach, testCase.hardBody1SPtr, testCase.hardBody2SPtr
    );

    EXPECT_NEAR(
        probabilityOfCollision,
        testCase.expectedProbabilityOfCollision,
        std::abs(testCase.expectedProbabilityOfCollision) * relativeTolerance_
    ) << testCase.getDescription();
}

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_AnalysisParameterized
    : public ::testing::TestWithParam<PoCAnalysisTestCase>
{
   protected:
    const Shared<const Alfano2005> algorithmSPtr_ = std::make_shared<Alfano2005>();
    const double relativeTolerance_ = 0.01;
};

INSTANTIATE_TEST_SUITE_P(
    Cases,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_AnalysisParameterized,
    ::testing::ValuesIn(PoCAnalysisTestCase::BuildNASACARADilutionPoCAnalysisTestCases()),
    [](const ::testing::TestParamInfo<PoCAnalysisTestCase>& aParamInfo) -> std::string
    {
        return ToTestName(aParamInfo.param.name);
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_AnalysisParameterized,
    ComputeProbabilityAnalysis
)
{
    const PoCAnalysisTestCase& testCase = GetParam();

    const ProbabilityOfCollisionAlgorithm::Analysis analysis =
        ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
            algorithmSPtr_, testCase.closeApproach, testCase.hardBody1SPtr, testCase.hardBody2SPtr, true, true
        );

    EXPECT_EQ(analysis.region, testCase.expectedRegion) << testCase.getDescription();
    ASSERT_TRUE(analysis.maximumProbabilityOfCollision.has_value()) << testCase.getDescription();
    EXPECT_NEAR(
        analysis.maximumProbabilityOfCollision.value(),
        testCase.expectedMaximumProbabilityOfCollision,
        std::abs(testCase.expectedMaximumProbabilityOfCollision) * relativeTolerance_
    ) << testCase.getDescription();
}
