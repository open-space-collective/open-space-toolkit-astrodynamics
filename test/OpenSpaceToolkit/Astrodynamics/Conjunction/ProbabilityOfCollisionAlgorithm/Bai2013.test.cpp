/// Apache License 2.0

#include <algorithm>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Bai2013.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::iau::Theory;
using ostk::physics::time::Instant;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::conjunction::hardbody::Spherical;
using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Bai2013;
using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

namespace
{

State buildState(
    const Instant& anInstant,
    const Shared<const Frame>& aFrameSPtr,
    const Vector3d& aPosition_m,
    const Vector3d& aVelocity_m_per_s
)
{
    const StateBuilder stateBuilder = {
        aFrameSPtr,
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };

    VectorXd coordinates(6);
    coordinates << aPosition_m, aVelocity_m_per_s;

    return stateBuilder.build(anInstant, coordinates);
}

// Local orbital frame in which the position sigmas of Table 3 are expressed
enum class CovarianceFrame
{
    RSW,  ///< Radial, in-track, cross-track (used by the explicit expression)
    NTW   ///< Normal, tangential, cross-track (used by the modified expression)
};

CloseApproach buildCloseApproach(
    const Vector3d& anObject1Position_km,
    const Vector3d& anObject1Velocity_km_per_s,
    const Vector3d& anObject2Position_km,
    const Vector3d& anObject2Velocity_km_per_s,
    const Vector3d& anObject1PositionSigmas_km,
    const Vector3d& anObject2PositionSigmas_km,
    const CovarianceFrame& aCovarianceFrame
)
{
    const Instant instant = Instant::J2000();
    const Shared<const Frame> j2000FrameSPtr = Frame::J2000(Theory::IAU_2006);

    State object1State =
        buildState(instant, j2000FrameSPtr, anObject1Position_km * 1e3, anObject1Velocity_km_per_s * 1e3);
    State object2State =
        buildState(instant, j2000FrameSPtr, anObject2Position_km * 1e3, anObject2Velocity_km_per_s * 1e3);

    // RSW frames are QSW frames. NTW sigmas are expressed in TNW frames, hence reordered (T, N, W)
    const Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactorySPtr =
        (aCovarianceFrame == CovarianceFrame::RSW) ? LocalOrbitalFrameFactory::QSW(j2000FrameSPtr)
                                                   : LocalOrbitalFrameFactory::TNW(j2000FrameSPtr);

    const auto toFrameSigmas = [&aCovarianceFrame](const Vector3d& aPositionSigmas_km) -> Vector3d
    {
        const Vector3d positionSigmas_m = aPositionSigmas_km * 1e3;

        if (aCovarianceFrame == CovarianceFrame::RSW)
        {
            return positionSigmas_m;
        }

        return {positionSigmas_m(1), positionSigmas_m(0), positionSigmas_m(2)};
    };

    object1State.setCovarianceMatrix(CovarianceMatrix::FromPositionSigmas(
        instant, toFrameSigmas(anObject1PositionSigmas_km), localOrbitalFrameFactorySPtr->generateFrame(object1State)
    ));
    object2State.setCovarianceMatrix(CovarianceMatrix::FromPositionSigmas(
        instant, toFrameSigmas(anObject2PositionSigmas_km), localOrbitalFrameFactorySPtr->generateFrame(object2State)
    ));

    return {object1State, object2State};
}

// Test cases from:
//
/// Bai, Xian-Zong & Chen, Lei & Tang, Guo-Jin. (2013). Explicit expression of collision probability in terms of RSW and
/// NTW components of relative position. Advances in Space Research. 52. 1078-1096. 10.1016/j.asr.2013.05.034.

// Case A (near-circular conjunction) from Bai, Chen, Tang (2013), Tables 1 and 3.
CloseApproach buildCaseA(const CovarianceFrame& aCovarianceFrame)
{
    return buildCloseApproach(
        Vector3d(-1457.273246, 1589.568484, 6814.189959),
        Vector3d(-7.001731, -2.439512, -0.926209),
        Vector3d(-1457.532155, 1588.932671, 6814.316188),
        Vector3d(3.578705, -6.172896, 2.200215),
        Vector3d(0.0231207, 0.2061885, 0.0719775),
        Vector3d(0.0363234, 0.4102069, 0.0341134),
        aCovarianceFrame
    );
}

// Case B (elliptic conjunction) from Bai, Chen, Tang (2013), Tables 1 and 3.
CloseApproach buildCaseB(const CovarianceFrame& aCovarianceFrame)
{
    return buildCloseApproach(
        Vector3d(-7219.855, -455.411, -3058.455),
        Vector3d(-2.492, -2.291, 6.262),
        Vector3d(-7219.486, -442.611, -3070.810),
        Vector3d(2.644, -7.968, 2.687),
        Vector3d(0.693621, 6.185655, 2.159325),
        Vector3d(1.089702, 12.306207, 1.023402),
        aCovarianceFrame
    );
}

Bai2013 buildBai2013Explicit()
{
    return {
        Derived(100.0, Derived::Unit::MeterPerSecond()),
        1.0,  // Ensure explicit formulation is used
    };
}

Bai2013 buildBai2013Modified()
{
    return {
        Derived(100.0, Derived::Unit::MeterPerSecond()),
        0.0,  // Ensure modified formulation is used
    };
}

State removeCovarianceMatrix(const State& aState)
{
    return buildState(
        aState.accessInstant(),
        aState.accessFrame(),
        aState.getPosition().getCoordinates(),
        aState.getVelocity().getCoordinates()
    );
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

struct ComputeProbabilityOfCollisionParams
{
    String name;
    Bai2013 algorithm;
    CloseApproach closeApproach;
    Shared<const HardBody> hardBody1SPtr;
    Shared<const HardBody> hardBody2SPtr;
    Real expectedProbabilityOfCollision;
};

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013 : public ::testing::Test
{
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013, Constructor)
{
    {
        EXPECT_NO_THROW(Bai2013());
    }

    {
        EXPECT_NO_THROW(Bai2013(
            Derived(10.0, Derived::Unit::MeterPerSecond()),
            0.01,
            Frame::J2000(Theory::IAU_2006),
            Derived(3.986004418e14, Derived::Unit::MeterCubedPerSecondSquared())
        ));
    }

    {
        EXPECT_THROW(
            try {
                Bai2013(Derived(100.0, Derived::Unit::MeterPerSecond()), 0.001, nullptr);
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
                Bai2013(Derived(100.0, Derived::Unit::MeterPerSecond()), 0.001, Frame::Undefined());
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
                Bai2013(Derived(100.0, Derived::Unit::MeterPerSecond()), 0.001, Frame::ITRF());
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("Frame = ITRF is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013, IdentifyUnsatisfiedAssumptions
)
{
    const Bai2013 algorithm = buildBai2013Explicit();

    const CloseApproach closeApproach = buildCaseA(CovarianceFrame::RSW);
    const State object1State = closeApproach.getObject1State();
    const State object2State = closeApproach.getObject2State();

    // All assumptions satisfied
    {
        EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(closeApproach).isEmpty());
    }

    // No covariance matrix for Object 1
    {
        const Array<String> unsatisfiedAssumptions =
            algorithm.identifyUnsatisfiedAssumptions({removeCovarianceMatrix(object1State), object2State});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // No covariance matrix for Object 2
    {
        const Array<String> unsatisfiedAssumptions =
            algorithm.identifyUnsatisfiedAssumptions({object1State, removeCovarianceMatrix(object2State)});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 2"));
    }

    // Covariance matrix without position uncertainty for Object 1
    {
        State object1StateWithVelocityCovarianceOnly = removeCovarianceMatrix(object1State);
        object1StateWithVelocityCovarianceOnly.setCovarianceMatrix(CovarianceMatrix(
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

    // Relative velocity below the threshold
    {
        const Bai2013 highRelativeVelocityThresholdAlgorithm = {
            Derived(20000.0, Derived::Unit::MeterPerSecond()),
            1.0,
        };

        const Array<String> unsatisfiedAssumptions =
            highRelativeVelocityThresholdAlgorithm.identifyUnsatisfiedAssumptions(closeApproach);

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "Relative velocity"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "is below the threshold"));
    }

    // All assumptions unsatisfied
    {
        const Bai2013 highRelativeVelocityThresholdAlgorithm = {
            Derived(20000.0, Derived::Unit::MeterPerSecond()),
            1.0,
        };

        const Array<String> unsatisfiedAssumptions =
            highRelativeVelocityThresholdAlgorithm.identifyUnsatisfiedAssumptions(
                {removeCovarianceMatrix(object1State), removeCovarianceMatrix(object2State)}
            );

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 3);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 2"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "is below the threshold"));
    }
}

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013_Parameterized
    : public ::testing::TestWithParam<ComputeProbabilityOfCollisionParams>
{
   protected:
    const Real relativeTolerance_ = 0.01;
};

INSTANTIATE_TEST_SUITE_P(
    Cases,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013_Parameterized,
    ::testing::Values(
        ComputeProbabilityOfCollisionParams {
            "CaseA_Explicit",
            buildBai2013Explicit(),
            buildCaseA(CovarianceFrame::RSW),
            std::make_shared<Spherical>(5.0),
            std::make_shared<Spherical>(5.0),
            1.807975e-4,
        },  // Case B is not run for the Explicit case.
            // When run we get a PoC of ~1.71e-8 (one oom below the expected PoC of 1.47e-7).
            // Upon further inspection, it seems that the miss distance components (RSW)
            // are off w.r.t. Table 2 NTW values:
            // - Computed RSW miss distances: 3.7km, -15.1km, 8.6km
            // - Table 2 NTW miss distances: 2.5km, 16.2km, 5.9km
            // It looks like they simply injected the NTW miss distances into the explicit formulation
            // instead of injecting the state vectors and let the algorithm compute the RTN miss distances.
            // In order to verify this, when injecting the NTW miss distances directly, we get a PoC of ~1.54e-7,
            // which is now way closer to the expected value.
        ComputeProbabilityOfCollisionParams {
            "CaseA_Modified",
            buildBai2013Modified(),
            buildCaseA(CovarianceFrame::NTW),
            std::make_shared<Spherical>(5.0),
            std::make_shared<Spherical>(5.0),
            1.804311e-4,
        },
        ComputeProbabilityOfCollisionParams {
            "CaseB_Modified",
            buildBai2013Modified(),
            buildCaseB(CovarianceFrame::NTW),
            std::make_shared<Spherical>(5.0),
            std::make_shared<Spherical>(5.0),
            8.363766e-7,
        }
    ),
    [](const ::testing::TestParamInfo<ComputeProbabilityOfCollisionParams>& aParamInfo) -> std::string
    {
        return aParamInfo.param.name;
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013_Parameterized,
    computeProbabilityOfCollision
)
{
    const ComputeProbabilityOfCollisionParams& params = GetParam();

    const Real probabilityOfCollision = params.algorithm.computeProbabilityOfCollision(
        params.closeApproach, params.hardBody1SPtr, params.hardBody2SPtr
    );

    EXPECT_NEAR(
        probabilityOfCollision,
        params.expectedProbabilityOfCollision,
        std::abs(params.expectedProbabilityOfCollision) * relativeTolerance_
    );
}
