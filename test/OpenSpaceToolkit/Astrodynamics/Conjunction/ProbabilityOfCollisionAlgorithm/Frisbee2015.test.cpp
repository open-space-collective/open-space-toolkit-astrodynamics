/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Frisbee2015.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::iau::Theory;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Length;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Frisbee2015;
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

// From the NASA CARA Analysis Tools:
// https://github.com/nasa/CARA_Analysis_Tools/blob/master/DistributedMatlab/Maximum2DPc/UnitTests/FrisbeeMaxPc_UnitTest.m
CloseApproach buildNasaCaraFrisbeeExample()
{
    const Instant instant = Instant::DateTime(DateTime::Parse("2008-06-27T15:34:55.320"), Scale::UTC);
    const Shared<const Frame> j2000SPtr = Frame::J2000(Theory::IAU_2006);

    const State object1State = buildState(
        instant,
        j2000SPtr,
        Vector3d(-1818.269382, 1040.563930, -6772.707308) * 1e3,
        Vector3d(-3.609802798, 6.269245755, 1.933211527) * 1e3
    );
    const State object2State = buildState(
        instant,
        j2000SPtr,
        Vector3d(-1819.008023, 1039.894591, -6772.922705) * 1e3,
        Vector3d(6.316590032, -3.383797284, -2.177018239) * 1e3
    );

    const Shared<const Frame> object1QSWFrameSPtr =
        LocalOrbitalFrameFactory::QSW(j2000SPtr)->generateFrame(object1State);

    MatrixXd object1PositionCovariance(3, 3);
    object1PositionCovariance << 9.406873876577049e03, -1.859415720232862e04, -6.767206137928272e04,
        -1.859415720232862e04, 5.124346486116677e04, 1.825065889128269e05, -6.767206137928272e04, 1.825065889128269e05,
        6.654558817959293e05;

    const CovarianceMatrix object1CovarianceMatrix = {
        instant,
        object1PositionCovariance,
        object1QSWFrameSPtr,
        {CartesianPosition::Default()},
    };

    return {object1State, object2State, object1CovarianceMatrix, CovarianceMatrix::Undefined()};
}

struct ComputeProbabilityOfCollisionParams
{
    String name;
    Frisbee2015 algorithm;
    CloseApproach closeApproach;
    Length combinedHardBodyRadius;
    Real expectedProbabilityOfCollision;
};

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015
    : public ::testing::TestWithParam<ComputeProbabilityOfCollisionParams>
{
   protected:
    const Real relativeTolerance_ = 0.01;
};

INSTANTIATE_TEST_SUITE_P(
    NasaCaraFrisbeeExample,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    ::testing::Values(
        ComputeProbabilityOfCollisionParams {
            "Test12",
            Frisbee2015 {},
            buildNasaCaraFrisbeeExample(),
            Length::Meters(5.0),
            0.000043,
        },
        ComputeProbabilityOfCollisionParams {
            "Test13",
            Frisbee2015 {},
            buildNasaCaraFrisbeeExample(),
            Length::Meters(10.0),
            0.000171,
        }
    ),
    [](const ::testing::TestParamInfo<ComputeProbabilityOfCollisionParams>& aParamInfo) -> std::string
    {
        return aParamInfo.param.name;
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Frisbee2015,
    computeProbabilityOfCollision
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
