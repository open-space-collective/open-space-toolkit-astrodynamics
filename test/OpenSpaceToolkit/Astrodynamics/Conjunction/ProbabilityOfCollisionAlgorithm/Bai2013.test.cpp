/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
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

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::iau::Theory;
using ostk::physics::time::Instant;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::conjunction::CloseApproach;
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

CloseApproach buildCloseApproach(
    const Vector3d& anObject1Position_km,
    const Vector3d& anObject1Velocity_km_per_s,
    const Vector3d& anObject2Position_km,
    const Vector3d& anObject2Velocity_km_per_s,
    const Vector3d& anObject1PositionSigmas_km,
    const Vector3d& anObject2PositionSigmas_km
)
{
    const Instant instant = Instant::J2000();
    const Shared<const Frame> j2000FrameSPtr = Frame::J2000(Theory::IAU_2006);

    const State object1State =
        buildState(instant, j2000FrameSPtr, anObject1Position_km * 1e3, anObject1Velocity_km_per_s * 1e3);
    const State object2State =
        buildState(instant, j2000FrameSPtr, anObject2Position_km * 1e3, anObject2Velocity_km_per_s * 1e3);

    const Shared<const Frame> object1QSWFrameSPtr =
        LocalOrbitalFrameFactory::QSW(j2000FrameSPtr)->generateFrame(object1State);
    const Shared<const Frame> object2QSWFrameSPtr =
        LocalOrbitalFrameFactory::QSW(j2000FrameSPtr)->generateFrame(object2State);

    const CovarianceMatrix object1CovarianceMatrix =
        CovarianceMatrix::FromPositionSigmas(instant, anObject1PositionSigmas_km * 1e3, object1QSWFrameSPtr);
    const CovarianceMatrix object2CovarianceMatrix =
        CovarianceMatrix::FromPositionSigmas(instant, anObject2PositionSigmas_km * 1e3, object2QSWFrameSPtr);

    return {object1State, object2State, object1CovarianceMatrix, object2CovarianceMatrix};
}

// Case A (near-circular conjunction) from Bai, Chen, Tang (2013), Tables 1 and 3.
CloseApproach buildCaseA()
{
    return buildCloseApproach(
        Vector3d(-1457.273246, 1589.568484, 6814.189959),
        Vector3d(-7.001731, -2.439512, -0.926209),
        Vector3d(-1457.532155, 1588.932671, 6814.316188),
        Vector3d(3.578705, -6.172896, 2.200215),
        Vector3d(0.0231207, 0.2061885, 0.0719775),
        Vector3d(0.0363234, 0.4102069, 0.0341134)
    );
}

// Case B (elliptic conjunction) from Bai, Chen, Tang (2013), Tables 1 and 3.
CloseApproach buildCaseB()
{
    return buildCloseApproach(
        Vector3d(-7219.855, -455.411, -3058.455),
        Vector3d(-2.492, -2.291, 6.262),
        Vector3d(-7219.486, -442.611, -3070.810),
        Vector3d(2.644, -7.968, 2.687),
        Vector3d(0.693621, 6.185655, 2.159325),
        Vector3d(1.089702, 12.306207, 1.023402)
    );
}

Bai2013 buildBai2013Explicit()
{
    return {
        Derived(100.0, Derived::Unit::MeterPerSecond()),
        1.0,  // Ensure explicit formulation is used
    };
}

struct ComputeProbabilityOfCollisionParams
{
    String name;
    Bai2013 algorithm;
    CloseApproach closeApproach;
    Length combinedHardBodyRadius;
    Real expectedProbabilityOfCollision;
};

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013
    : public ::testing::TestWithParam<ComputeProbabilityOfCollisionParams>
{
   protected:
    const Real relativeTolerance_ = 0.01;
};

INSTANTIATE_TEST_SUITE_P(
    Cases,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013,
    ::testing::Values(ComputeProbabilityOfCollisionParams {
        "CaseA_Explicit",
        buildBai2013Explicit(),
        buildCaseA(),
        Length::Meters(10.0),
        1.807975e-4,
    }  // Case B is not run for the Explicit case.
       // When run we get a PoC of ~1.71e-8 (one oom below the expected PoC of 1.47e-7).
       // Upon further inspection, it seems that the miss distance components (RSW)
       // are off w.r.t. Table 2 NTW values:
       // - Computed RSW miss distances: 3.7km, -15.1km, 8.6km
       // - Table 2 NTW miss distances: 2.5km, 16.2km, 5.9km
       // It looks like they simply injected the NTW miss distances into the explicit formulation
       // instead of injecting the state vectors and let the algorithm compute the RTN miss distances.
       // In order to verify this, when injecting the NTW miss distances directly, we get a PoC of ~1.54e-7,
       // which is now way closer to the expected value.
    ),
    [](const ::testing::TestParamInfo<ComputeProbabilityOfCollisionParams>& aParamInfo) -> std::string
    {
        return aParamInfo.param.name;
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013, computeProbabilityOfCollision
)
{
    const ComputeProbabilityOfCollisionParams& params = GetParam();

    const Real probabilityOfCollision =
        params.algorithm.computeProbabilityOfCollision(params.closeApproach, params.combinedHardBodyRadius);

    EXPECT_NEAR(
        probabilityOfCollision,
        params.expectedProbabilityOfCollision,
        std::abs(params.expectedProbabilityOfCollision) * relativeTolerance_
    );
}
