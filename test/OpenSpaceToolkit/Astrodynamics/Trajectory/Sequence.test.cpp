/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/DurationCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::String;
using ostk::core::types::Shared;
using ostk::core::types::Size;

using ostk::physics::env::obj::Celestial;
using ostk::physics::time::Instant;
using ostk::physics::time::Duration;
using ostk::physics::time::DateTime;
using ostk::physics::time::Scale;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::trajectory::Segment;
using ostk::astro::trajectory::Sequence;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::eventcondition::DurationCondition;
using ostk::astro::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence : public ::testing::Test
{
   protected:
    const State defaultState_ = {
        Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({0.0, 7546.05329, 0.0}, Frame::GCRF()),
    };

    const Duration propagationDuration_ = Duration::Minutes(15.0);

    const Segment coastSegment_ = Segment::Coast(
        std::make_shared<DurationCondition>(DurationCondition::Criteria::AnyCrossing, propagationDuration_)
    );
    const Array<Segment> segments_ = {coastSegment_};
    const Size repetitionCount_ = 2;
    const Shared<Celestial> earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> defaultDynamics_ = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSpherical_),
    };
    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        1e-3,
        1.0e-15,
        1.0e-15,
    };
    const Duration defaultMaximumPropagationDuration_ = Duration::Days(7.0);
    const Sequence sequence_ = {
        segments_,
        repetitionCount_,
        defaultNumericalSolver_,
        defaultDynamics_,
        defaultMaximumPropagationDuration_,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Constructor)
{
    {
        EXPECT_NO_THROW(Sequence sequence(segments_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(segments_, repetitionCount_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(segments_, repetitionCount_, defaultNumericalSolver_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(segments_, repetitionCount_, defaultNumericalSolver_, defaultDynamics_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(
            segments_, repetitionCount_, defaultNumericalSolver_, defaultDynamics_, defaultMaximumPropagationDuration_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << sequence_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, GetSegments)
{
    EXPECT_EQ(segments_.getSize(), sequence_.getSegments().getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, GetNumericalSolver)
{
    EXPECT_EQ(defaultNumericalSolver_, sequence_.getNumericalSolver());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, GetDynamics)
{
    EXPECT_EQ(defaultDynamics_, sequence_.getDynamics());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, GetMaximumPropagationDuration)
{
    EXPECT_EQ(defaultMaximumPropagationDuration_, sequence_.getMaximumPropagationDuration());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve)
{
    Sequence::Solution solution = sequence_.solve(defaultState_);

    EXPECT_TRUE(solution.segmentSolutions.getSize() == 1 * repetitionCount_);

    Segment::Solution segmentSolution = solution.segmentSolutions[0];

    EXPECT_LT(
        (segmentSolution.states.accessLast().getInstant() -
         (segmentSolution.states.accessFirst().getInstant() + propagationDuration_ * repetitionCount_))
            .inSeconds(),
        1e-6
    );
    EXPECT_TRUE(segmentSolution.states.getSize() > 0);
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
//     const Array<Segment> segments = {
//         Segment::Coast(
//             std::make_shared<DurationCondition>(DurationCondition::Criteria::AnyCrossing, Duration::Minutes(15.0))
//         ),
//         Segment::Coast(
//             std::make_shared<COECondition>(COECondition::SemiMajorAxis(6371)))
//         ),
//         Segment::Coast(
//             std::make_shared<DurationCondition>(DurationCondition::Criteria::AnyCrossing, Duration::Minutes(15.0))
//         ),
//     };
// }

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(sequence_.print(std::cout, true));
    EXPECT_NO_THROW(sequence_.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}
