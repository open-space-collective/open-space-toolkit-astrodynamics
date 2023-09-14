/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/TrajectorySegment.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::Real;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;

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
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using ostk::physics::data::Scalar;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::trajectory::TrajectorySegment;
using ostk::astro::trajectory::Sequence;
using ostk::astro::trajectory::LocalOrbitalFrameDirection;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::flight::system::dynamics::AtmosphericDrag;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::flight::system::dynamics::thruster::ConstantThrust;
using ostk::astro::eventcondition::COECondition;
using ostk::astro::eventcondition::AngularCondition;
using ostk::astro::eventcondition::RealCondition;
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

    const Size defaultRepetitionCount_ = 2;
    const Duration defaultMaximumPropagationDuration_ = Duration::Days(7.0);
    Sequence defaultSequence_ = {
        defaultRepetitionCount_,
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
        EXPECT_NO_THROW(Sequence sequence(defaultRepetitionCount_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(defaultRepetitionCount_, defaultNumericalSolver_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(defaultRepetitionCount_, defaultNumericalSolver_, defaultDynamics_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(
            defaultRepetitionCount_, defaultNumericalSolver_, defaultDynamics_, defaultMaximumPropagationDuration_
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

    EXPECT_TRUE(solution.segmentSolutions.getSize() == 1 * defaultRepetitionCount_);

    for (const TrajectorySegment::Solution& segmentSolution : solution.segmentSolutions)
    {
        EXPECT_TRUE(segmentSolution.states.getSize() > 0);
        const Real targetAngle = defaultCondition_->getEvaluator()(segmentSolution.states.accessLast());
        EXPECT_NEAR(targetAngle, defaultCondition_->getTargetAngle().inRadians(0.0, Real::TwoPi()), 1e-6);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_2)
{
    // dynamics
    const Shared<Earth> earthSPtr = std::make_shared<Earth>(Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
    ));

    SatelliteSystem satelliteSystem = SatelliteSystem::Undefined();
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    const Scalar thrust = Scalar(1.0, PropulsionSystem::thrustSIUnit);
    const Scalar specificImpulse = Scalar(1500.0, PropulsionSystem::specificImpulseSIUnit);
    const PropulsionSystem propulsionSystem = PropulsionSystem(thrust, specificImpulse);

    satelliteSystem = {
        Mass::Kilograms(100.0),
        satelliteGeometry,
        Matrix3d::Identity(),
        500.0,
        2.1,
        propulsionSystem,
    };

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr, satelliteSystem),
    };

    const LocalOrbitalFrameDirection lofDirection = {
        {1.0, 0.0, 0.0},
        LocalOrbitalFrameFactory::VNC(Frame::GCRF()),
    };

    Sequence sequence = {
        defaultRepetitionCount_,
        defaultNumericalSolver_,
        dynamics,
        defaultMaximumPropagationDuration_,
    };

    sequence.addCoastSegment(std::make_shared<RealCondition>(COECondition::SemiMajorAxis(
        RealCondition::Criterion::AnyCrossing,
        Frame::GCRF(),
        Length::Kilometers(6999.5),
        EarthGravitationalModel::EGM2008.gravitationalParameter_
    )));
    sequence.addManeuverSegment(
        std::make_shared<RealCondition>(COECondition::SemiMajorAxis(
            RealCondition::Criterion::AnyCrossing,
            Frame::GCRF(),
            Length::Kilometers(7000.0),
            EarthGravitationalModel::EGM2008.gravitationalParameter_
        )),
        std::make_shared<ConstantThrust>(satelliteSystem, lofDirection)
    );

    const Shared<const CoordinatesBroker> coordinatesBrokerSPtr_ =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker({
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            CoordinatesSubset::Mass(),
        }));

    VectorXd coordinates(7);
    coordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;
    const State state = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        coordinatesBrokerSPtr_,
    };

    Sequence::Solution solution = sequence.solve(state);

    EXPECT_TRUE(solution.segmentSolutions.getSize() == 2 * defaultRepetitionCount_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultSequence_.print(std::cout, true));
    EXPECT_NO_THROW(defaultSequence_.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}
