/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/TrajectorySegment.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::Real;

using ostk::physics::env::obj::Celestial;
using ostk::physics::time::Instant;
using ostk::physics::time::Duration;
using ostk::physics::time::DateTime;
using ostk::physics::time::Scale;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::units::Angle;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::TrajectorySegment;
using ostk::astro::trajectory::Sequence;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::eventcondition::COECondition;
using ostk::astro::eventcondition::AngularCondition;
using ostk::astro::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence : public ::testing::Test
{
    void SetUp() override
    {
        defaultSequence_.addSegment(coastSegment_);
    }

   protected:
    const State defaultState_ = {
        Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({0.0, 7546.05329, 0.0}, Frame::GCRF()),
    };

    const Duration propagationDuration_ = Duration::Minutes(15.0);

    const Shared<Celestial> earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> defaultDynamics_ = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSpherical_),
    };

    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::LogAdaptive,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        1e-3,
        1.0e-12,
        1.0e-12,
    };

    const Shared<AngularCondition> defaultCondition_ = std::make_shared<AngularCondition>(COECondition::TrueAnomaly(
        AngularCondition::Criterion::AnyCrossing,
        Frame::GCRF(),
        Angle::Degrees(0.0),
        EarthGravitationalModel::EGM2008.gravitationalParameter_
    ));

    const TrajectorySegment coastSegment_ =
        TrajectorySegment::Coast("Coast", defaultCondition_, defaultDynamics_, defaultNumericalSolver_);

    const Array<TrajectorySegment> segments_ = {coastSegment_};

    const Size repetitionCount_ = 2;
    const Duration defaultMaximumPropagationDuration_ = Duration::Days(7.0);
    Sequence defaultSequence_ = {
        repetitionCount_,
        defaultNumericalSolver_,
        defaultDynamics_,
        defaultMaximumPropagationDuration_,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Constructor)
{
    {
        EXPECT_NO_THROW(Sequence sequence());
    }

    {
        EXPECT_NO_THROW(Sequence sequence(repetitionCount_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(repetitionCount_, defaultNumericalSolver_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(repetitionCount_, defaultNumericalSolver_, defaultDynamics_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(
            repetitionCount_, defaultNumericalSolver_, defaultDynamics_, defaultMaximumPropagationDuration_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultSequence_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, GetSegments)
{
    EXPECT_EQ(segments_.getSize(), defaultSequence_.getSegments().getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, GetNumericalSolver)
{
    EXPECT_EQ(defaultNumericalSolver_, defaultSequence_.getNumericalSolver());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, GetDynamics)
{
    EXPECT_EQ(defaultDynamics_, defaultSequence_.getDynamics());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, GetMaximumPropagationDuration)
{
    EXPECT_EQ(defaultMaximumPropagationDuration_, defaultSequence_.getMaximumPropagationDuration());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve)
{
    Sequence::Solution solution = defaultSequence_.solve(defaultState_);

    EXPECT_TRUE(solution.segmentSolutions.getSize() == 1 * repetitionCount_);

    for (const TrajectorySegment::Solution& segmentSolution : solution.segmentSolutions)
    {
        EXPECT_TRUE(segmentSolution.states.getSize() > 0);
        const Real targetAngle = defaultCondition_->getEvaluator()(segmentSolution.states.accessLast());
        EXPECT_NEAR(targetAngle, defaultCondition_->getTargetAngle().inRadians(0.0, Real::TwoPi()), 1e-6);
    }
}

// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_2)
// {
//     // dynamics
//     const Shared<Earth> earthSPtr = std::make_shared<Earth>(Earth::FromModels(
//         std::make_shared<EarthGravitationalModel>(EarthGravitationalModel.Type.Spherical),
//         std::make_shared<EarthMagneticModel>(EarthMagneticModel.Type.Undefined),
//         std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel.Type.Exponential)
//     ));

//     SatelliteSystem satelliteSystem = {};
//     const Composite satelliteGeometry(Cuboid(
//         {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
//         {1.0, 2.0, 3.0}
//     ));

//     satelliteSystem_ = {
//         Mass::Kilograms(100.0),
//         satelliteGeometry,
//         Matrix3d::Identity(),
//         500.0,
//         2.1,
//     };

//     const Array<Shared<Dynamics>> dynamics = {
//         std::make_shared<PositionDerivative>(),
//         std::make_shared<CentralBodyGravity>(earthSPtr),
//         std::make_shared<AtmosphericDrag>(earthSPtr, satelliteSystem),
//     };

//     // segments
//     const Array<TrajectorySegment> segments = {
//         TrajectorySegment::Coast(std::make_shared<RealCondition>(COECondition::SemiMajorAxis(
//             RealCondition::Criterion::AnyCrossing,
//             Frame::GCRF(),
//             Length.kilometers(6995.0),
//             EarthGravitationalModel::EGM2008.gravitationalParameter_
//         ))),
//         // do burn
//     };
// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultSequence_.print(std::cout, true));
    EXPECT_NO_THROW(defaultSequence_.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}
