/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/TrajectorySegment.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::physics::env::obj::Celestial;
using ostk::physics::time::Instant;
using ostk::physics::time::Duration;
using ostk::physics::time::DateTime;
using ostk::physics::time::Scale;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::dynamics::thruster::ConstantThrust;
using ostk::astro::trajectory::TrajectorySegment;
using ostk::astro::trajectory::LocalOrbitalFrameDirection;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::eventcondition::InstantCondition;
using ostk::astro::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment : public ::testing::Test
{
   protected:
    const State defaultState_ = {
        Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({0.0, 7546.05329, 0.0}, Frame::GCRF()),
    };

    const String defaultName_ = "A Segment";

    const Shared<Celestial> earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> defaultDynamics_ = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSpherical_),
    };
    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        1e-2,
        1.0e-15,
        1.0e-15,
        {100u, 1e-8}
    };
    const Shared<InstantCondition> defaultInstantCondition_ = std::make_shared<InstantCondition>(
        InstantCondition::Criterion::AnyCrossing, defaultState_.accessInstant() + Duration::Minutes(15.0)
    );
    const Shared<ConstantThrust> defaultConstantThrust_ = std::make_shared<ConstantThrust>(
        SatelliteSystem::Default(), LocalOrbitalFrameDirection({1.0, 0.0, 0.0}, LocalOrbitalFrameFactory::VNC(Frame::GCRF()))
    );
    const TrajectorySegment defaultCoastSegment_ =
        TrajectorySegment::Coast(defaultName_, defaultInstantCondition_, defaultDynamics_, defaultNumericalSolver_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, Coast)
{
    {
        EXPECT_NO_THROW(
            TrajectorySegment::Coast(defaultName_, defaultInstantCondition_, defaultDynamics_, defaultNumericalSolver_)
        );
    }

    {
        EXPECT_THROW(
            TrajectorySegment::Coast(defaultName_, nullptr, defaultDynamics_, NumericalSolver::Undefined()),
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            TrajectorySegment::Coast(defaultName_, defaultInstantCondition_, {}, NumericalSolver::Undefined()),
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            TrajectorySegment::Coast(
                defaultName_, defaultInstantCondition_, defaultDynamics_, NumericalSolver::Undefined()
            ),
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, Maneuver)
{
    {
        EXPECT_NO_THROW(TrajectorySegment::Maneuver(
            defaultName_, defaultInstantCondition_, defaultConstantThrust_, defaultDynamics_, defaultNumericalSolver_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, GetName)
{
    EXPECT_EQ(defaultName_, defaultCoastSegment_.getName());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, GetEventCondition)
{
    EXPECT_TRUE(defaultCoastSegment_.getEventCondition() == defaultInstantCondition_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, GetDynamics)
{
    EXPECT_EQ(defaultDynamics_, defaultCoastSegment_.getDynamics());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, GetNumericalSolver)
{
    EXPECT_EQ(defaultNumericalSolver_, defaultCoastSegment_.getNumericalSolver());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, GetType)
{
    EXPECT_EQ(TrajectorySegment::Type::Coast, defaultCoastSegment_.getType());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, AccessEventCondition)
{
    EXPECT_TRUE(defaultCoastSegment_.accessEventCondition() == defaultInstantCondition_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, AccessDynamics)
{
    EXPECT_EQ(defaultDynamics_, defaultCoastSegment_.accessDynamics());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, AccessNumericalSolver)
{
    EXPECT_EQ(defaultNumericalSolver_, defaultCoastSegment_.accessNumericalSolver());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultCoastSegment_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, Solve)
{
    {
        TrajectorySegment::Solution solution = defaultCoastSegment_.solve(defaultState_);

        EXPECT_LT(
            (solution.states.accessLast().getInstant() - defaultInstantCondition_->getInstant()).inSeconds(), 1e-7
        );
        EXPECT_TRUE(solution.states.getSize() > 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_TrajectorySegment, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultCoastSegment_.print(std::cout, true));
    EXPECT_NO_THROW(defaultCoastSegment_.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}
