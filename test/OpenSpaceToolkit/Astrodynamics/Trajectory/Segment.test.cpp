/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/HeterogeneousGuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::eventcondition::COECondition;
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::guidancelaw::HeterogeneousGuidanceLaw;
using ostk::astrodynamics::guidancelaw::QLaw;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::Segment;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class CustomGuidanceLaw : public ostk::astrodynamics::GuidanceLaw
{
   public:
    CustomGuidanceLaw(const Array<Interval>& anIntervalArray)
        : GuidanceLaw("Custom Guidance Law"),
          intervals_(anIntervalArray)
    {
    }

    ostk::mathematics::object::Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        [[maybe_unused]] const ostk::mathematics::object::Vector3d& aPositionCoordinates,
        [[maybe_unused]] const ostk::mathematics::object::Vector3d& aVelocityCoordinates,
        [[maybe_unused]] const Real& aThrustAcceleration,
        [[maybe_unused]] const Shared<const Frame>& outputFrameSPtr
    ) const override
    {
        // Check if anInstant is contained in any of the intervals
        for (const Interval& interval : intervals_)
        {
            if (interval.contains(anInstant))
            {
                return {1.0 * aThrustAcceleration, 0.0, 0.0};
            }
        }
        return {0.0, 0.0, 0.0};
    }

   private:
    Array<Interval> intervals_;
};

class OpenSpaceToolkit_Astrodynamics_Trajectory_Segment : public ::testing::Test
{
    void SetUp() override
    {
        VectorXd initialCoordinates(7);
        initialCoordinates << +7000000.000000, +0000000.000000, -0000000.000000, +00000.00000000, -05719.55029824,
            -04922.36372519, 200.0000000;

        VectorXd intermediateCoordinates(7);
        intermediateCoordinates << +6996339.698520, -0171556.896937, -0147645.427045, -00243.99897054, -05716.57931343,
            -04919.80683398, 199.999796057;

        VectorXd finalCoordinates(7);
        finalCoordinates << +6985362.605194, -0342934.972574, -0295136.956767, -00487.74360713, -05707.62991393,
            -04912.10479499, 199.999592114;

        initialStateWithMass_ = {Instant::J2000(), initialCoordinates, Frame::GCRF(), thrustCoordinateBrokerSPtr_};

        intermediateStateWithMass_ = {
            Instant::J2000() + Duration::Seconds(30.0),
            intermediateCoordinates,
            Frame::GCRF(),
            thrustCoordinateBrokerSPtr_
        };

        finalStateWithMass_ = {
            Instant::J2000() + Duration::Seconds(60.0), finalCoordinates, Frame::GCRF(), thrustCoordinateBrokerSPtr_
        };
    }

   protected:
    const Shared<const Frame> defaultFrameSPtr_ = Frame::GCRF();
    const State defaultState_ = {
        Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, defaultFrameSPtr_),
        Velocity::MetersPerSecond({0.0, 7546.05329, 0.0}, defaultFrameSPtr_),
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
        5.0,
        1.0e-8,  // Reducing tolerances so that the solving doesn't take forever
        1.0e-8,  // Reducing tolerances so that the solving doesn't take forever
    };

    const Shared<InstantCondition> defaultInstantCondition_ = std::make_shared<InstantCondition>(
        InstantCondition::Criterion::AnyCrossing, defaultState_.accessInstant() + Duration::Minutes(15.0)
    );

    const Shared<const LocalOrbitalFrameFactory> defaultLocalOrbitalFrameFactorySPtr_ =
        LocalOrbitalFrameFactory::VNC(defaultFrameSPtr_);

    const Shared<const ConstantThrust> constantThrustSPtr_ = std::make_shared<ConstantThrust>(
        LocalOrbitalFrameDirection({1.0, 0.0, 0.0}, defaultLocalOrbitalFrameFactorySPtr_)
    );

    const SatelliteSystem defaultSatelliteSystem_ = SatelliteSystem::Default();

    const Shared<Thruster> defaultThrusterDynamicsSPtr_ =
        std::make_shared<Thruster>(defaultSatelliteSystem_, constantThrustSPtr_);

    const Segment defaultCoastSegment_ =
        Segment::Coast(defaultName_, defaultInstantCondition_, defaultDynamics_, defaultNumericalSolver_);

    const Shared<CoordinateBroker> thrustCoordinateBrokerSPtr_ = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));

    const COE defaultCurrentCOE_ = {
        Length::Meters(6800.0e3),
        0.01,
        Angle::Degrees(80.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(90.0),
    };

    const COE defaultTargetCOE_ = {
        defaultCurrentCOE_.getSemiMajorAxis(),
        defaultCurrentCOE_.getEccentricity(),
        defaultCurrentCOE_.getInclination() + Angle::Degrees(0.1),
        defaultCurrentCOE_.getRaan(),
        defaultCurrentCOE_.getAop(),
        defaultCurrentCOE_.getTrueAnomaly(),
    };

    const QLaw::Parameters defaultQLawParameters_ = {
        {
            {
                COE::Element::Inclination,
                {
                    1.0,   // Control element weight
                    1e-4,  // Convergence threshold of 0.0001
                },
            },
        },
        3,                           // M value
        4,                           // N value
        2,                           // R value
        0.01,                        // B value
        100,                         // K value
        1.0,                         // Periapsis weight
        Length::Kilometers(6578.0),  // Minimum periapsis
        0.5,                         // Absolute effectivity threshold
    };

    const QLaw defaultQLaw_ = {
        defaultTargetCOE_,
        EarthGravitationalModel::EGM2008.gravitationalParameter_,
        defaultQLawParameters_,
        QLaw::GradientStrategy::Analytical,
    };

    const Shared<Thruster> defaultQLawThrusterDynamicsSPtr_ =
        std::make_shared<Thruster>(defaultSatelliteSystem_, std::make_shared<QLaw>(defaultQLaw_));

    State initialStateWithMass_ = State::Undefined();
    State intermediateStateWithMass_ = State::Undefined();
    State finalStateWithMass_ = State::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_Constructor)
{
    {
        EXPECT_NO_THROW(Segment::Solution(defaultName_, defaultDynamics_, {}, true, Segment::Type::Coast));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_Accessors)
{
    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {defaultState_, defaultState_}, true, Segment::Type::Coast
        );

        EXPECT_NO_THROW(segmentSolution.accessStartInstant());
        EXPECT_NO_THROW(segmentSolution.accessEndInstant());
    }

    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {}, true, Segment::Type::Coast);

        EXPECT_THROW(segmentSolution.accessStartInstant(), ostk::core::error::RuntimeError);
        EXPECT_THROW(segmentSolution.accessEndInstant(), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_Getters)
{
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {}, true, Segment::Type::Coast);

        EXPECT_THROW(segmentSolution.getInitialMass(), ostk::core::error::RuntimeError);
        EXPECT_THROW(segmentSolution.getFinalMass(), ostk::core::error::RuntimeError);
        EXPECT_THROW(segmentSolution.getPropagationDuration(), ostk::core::error::RuntimeError);
        EXPECT_THROW(segmentSolution.getInterval(), ostk::core::error::RuntimeError);
    }

    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Maneuver
        );

        EXPECT_DOUBLE_EQ(200.0, segmentSolution.getInitialMass().inKilograms());
        EXPECT_DOUBLE_EQ(199.999592114, segmentSolution.getFinalMass().inKilograms());
        EXPECT_DOUBLE_EQ(60.0, segmentSolution.getPropagationDuration().inSeconds());
        EXPECT_EQ(
            Interval::Closed(initialStateWithMass_.accessInstant(), finalStateWithMass_.accessInstant()),
            segmentSolution.getInterval()
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_ComputeDeltaV)
{
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {}, true, Segment::Type::Coast);

        EXPECT_DOUBLE_EQ(0.0, segmentSolution.computeDeltaV(1500.0));
    }

    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Maneuver
        );

        // 1500 * 9.80665 * ln(200 / 199.999592114) -> Rocket equation
        EXPECT_DOUBLE_EQ(0.029999994906376375, segmentSolution.computeDeltaV(1500.0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_ComputeDeltaMass)
{
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {}, true, Segment::Type::Coast);

        EXPECT_DOUBLE_EQ(0.0, segmentSolution.computeDeltaMass().inKilograms());
    }

    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Maneuver
        );

        EXPECT_DOUBLE_EQ(200.0 - 199.999592114, segmentSolution.computeDeltaMass().inKilograms());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_GetThrusterDynamics)
{
    // Coast segment solution should throw when trying to get thruster dynamics
    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Coast
        );

        EXPECT_THROW(
            {
                try
                {
                    segmentSolution.getThrusterDynamics();
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Cannot get thruster dynamics from a Coast segment solution.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    // Maneuver segment solution with no thruster dynamics should throw
    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Maneuver
        );

        EXPECT_THROW(
            {
                try
                {
                    segmentSolution.getThrusterDynamics();
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("No Thruster dynamics found in Maneuvering segment solution.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    // Maneuver segment solution with multiple thruster dynamics should throw
    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_,
            defaultDynamics_ + Array<Shared<Dynamics>> {defaultThrusterDynamicsSPtr_, defaultThrusterDynamicsSPtr_},
            {initialStateWithMass_, finalStateWithMass_},
            true,
            Segment::Type::Maneuver
        );

        EXPECT_THROW(
            {
                try
                {
                    segmentSolution.getThrusterDynamics();
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Multiple Thruster dynamics found in Maneuvering segment solution.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    // Maneuver segment solution should return thruster dynamics
    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_,
            defaultDynamics_ + Array<Shared<Dynamics>> {defaultThrusterDynamicsSPtr_},
            {initialStateWithMass_, finalStateWithMass_},
            true,
            Segment::Type::Maneuver
        );

        const Shared<Thruster> thrusterDynamics = segmentSolution.getThrusterDynamics();
        EXPECT_EQ(defaultThrusterDynamicsSPtr_, thrusterDynamics);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_ExtractManeuvers)
{
    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_,
            defaultDynamics_ + Array<Shared<Dynamics>> {defaultThrusterDynamicsSPtr_},
            {initialStateWithMass_, finalStateWithMass_},
            true,
            Segment::Type::Maneuver
        );

        EXPECT_THROW(
            {
                try
                {
                    segmentSolution.extractManeuvers(nullptr);
                }
                catch (const ostk::core::error::runtime::Undefined& e)
                {
                    EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            {
                try
                {
                    segmentSolution.extractManeuvers(Frame::Undefined());
                }
                catch (const ostk::core::error::runtime::Undefined& e)
                {
                    EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {}, true, Segment::Type::Maneuver);

        EXPECT_THROW(
            {
                try
                {
                    segmentSolution.extractManeuvers(defaultFrameSPtr_);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("No states exist within Segment Solution.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Maneuver
        );

        EXPECT_THROW(
            {
                try
                {
                    segmentSolution.extractManeuvers(defaultFrameSPtr_);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("No Thruster dynamics found in Maneuvering segment solution.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_,
            defaultDynamics_ + Array<Shared<Dynamics>> {defaultThrusterDynamicsSPtr_, defaultThrusterDynamicsSPtr_},
            {initialStateWithMass_, finalStateWithMass_},
            true,
            Segment::Type::Maneuver
        );

        EXPECT_THROW(
            {
                try
                {
                    segmentSolution.extractManeuvers(defaultFrameSPtr_);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Multiple Thruster dynamics found in Maneuvering segment solution.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Coast
        );

        EXPECT_EQ(0, segmentSolution.extractManeuvers(defaultFrameSPtr_).getSize());
    }

    // Check that a maneuver can be extracted when a thruster dynamics is found in a maneuvering segment
    {
        const Shared<RealCondition> durationCondition = std::make_shared<RealCondition>(
            RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(15.0))
        );

        // Create maneuvering segment
        Segment maneuveringSegment = Segment::Maneuver(
            "Maneuvering Segment",
            durationCondition,
            defaultThrusterDynamicsSPtr_,
            defaultDynamics_,
            {
                NumericalSolver::LogType::NoLog,
                NumericalSolver::StepperType::RungeKuttaDopri5,
                5.0,
                1.0e-12,
                1.0e-12,
            }
        );

        const Mass initialWetMass = Mass::Kilograms(200.0);
        VectorXd initialCoordinatesWithWetMass(7);
        initialCoordinatesWithWetMass << +7000000.000000, +0000000.000000, -0000000.000000, +00000.00000000,
            -05719.55029824, -04922.36372519, initialWetMass.inKilograms();

        const State initialStateWithWetMass = {
            Instant::J2000(), initialCoordinatesWithWetMass, Frame::GCRF(), thrustCoordinateBrokerSPtr_
        };

        const Segment::Solution maneuveringSegmentSolution = maneuveringSegment.solve(initialStateWithWetMass);

        const Array<Maneuver> maneuvers = maneuveringSegmentSolution.extractManeuvers(defaultFrameSPtr_);
        EXPECT_EQ(1, maneuvers.getSize());
        EXPECT_EQ(maneuveringSegmentSolution.states.getSize(), maneuvers[0].getStates().getSize());

        for (Size i = 0; i < maneuvers[0].getStates().getSize(); i++)
        {
            EXPECT_EQ(
                maneuveringSegmentSolution.states[i].accessInstant(), maneuvers[0].getStates()[i].accessInstant()
            );
        }

        // Check that metrics from the maneuvering segment solution are the same as the ones from the extracted maneuver
        EXPECT_NEAR(
            maneuveringSegmentSolution.computeDeltaV(defaultSatelliteSystem_.getPropulsionSystem().getSpecificImpulse()
            ),
            maneuvers[0].calculateDeltaV(),
            1e-10 * maneuveringSegmentSolution.computeDeltaV(
                        defaultSatelliteSystem_.getPropulsionSystem().getSpecificImpulse()  // Scale to relative error
                    )
        );
        EXPECT_NEAR(
            maneuveringSegmentSolution.computeDeltaMass().inKilograms(),
            maneuvers[0].calculateDeltaMass().inKilograms(),
            1e-10 * maneuveringSegmentSolution.computeDeltaMass().inKilograms()  // Scale to relative error
        );

        // These are not as close as the dV and dM above likely due to the midpoint trapezoidal rule used to calculate
        // thrusts at each profile interval midpoint
        EXPECT_NEAR(
            defaultSatelliteSystem_.getPropulsionSystem().getThrust(),
            maneuvers[0].calculateAverageThrust(initialWetMass),
            5e-6 * defaultSatelliteSystem_.getPropulsionSystem().getThrust()  // Scale to relative error
        );

        EXPECT_NEAR(
            defaultSatelliteSystem_.getPropulsionSystem().getSpecificImpulse(),
            maneuvers[0].calculateAverageSpecificImpulse(initialWetMass),
            5e-6 * defaultSatelliteSystem_.getPropulsionSystem().getSpecificImpulse()  // Scale to relative error
        );
    }

    // Check that multiple (or no) maneuvers can be extracted when more than one maneuvers are carried out per
    // maneuvering segment
    {
        const Shared<RealCondition> durationCondition = std::make_shared<RealCondition>(
            RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(90.0))
        );

        // Create maneuvering segment
        Segment maneuveringSegment = Segment::Maneuver(
            "Maneuvering Segment",
            durationCondition,
            defaultQLawThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_
        );

        // Check that multiple maneuvers can be extracted when more than one maneuvers are carried out per maneuvering
        // segment
        {
            const COE::CartesianState cartesianStatePair = defaultCurrentCOE_.getCartesianState(
                EarthGravitationalModel::EGM2008.gravitationalParameter_, defaultFrameSPtr_
            );
            VectorXd currentCoordinates(7);
            currentCoordinates << cartesianStatePair.first.accessCoordinates(),
                cartesianStatePair.second.accessCoordinates(), 200.0;
            const State currentState = {
                Instant::J2000(),
                currentCoordinates,
                defaultFrameSPtr_,
                thrustCoordinateBrokerSPtr_,
            };

            const Segment::Solution maneuveringSegmentSolution = maneuveringSegment.solve(currentState);

            const Array<Maneuver> maneuvers = maneuveringSegmentSolution.extractManeuvers(defaultFrameSPtr_);

            EXPECT_EQ(2, maneuvers.getSize());

            // Hardcoded values for the expected maneuvers from this QLaw solve. If QLaw is changed, this likely will
            // break
            Size numberOfInstantInFirstManeuver = 31;
            Size startingIndexFirstManeuver = 16;
            Size numberOfInstantInSecondManeuver = 30;
            Size startingIndexSecondManeuver = 71;

            // First maneuver
            EXPECT_EQ(numberOfInstantInFirstManeuver, maneuvers[0].getStates().getSize());

            for (Size i = 0; i < maneuvers[0].getStates().getSize(); i++)
            {
                EXPECT_EQ(
                    maneuveringSegmentSolution.states[startingIndexFirstManeuver + i].getInstant(),
                    maneuvers[0].getStates()[i].accessInstant()
                );
            }

            // Second maneuver
            EXPECT_EQ(numberOfInstantInSecondManeuver, maneuvers[1].getStates().getSize());

            for (Size j = 0; j < maneuvers[1].getStates().getSize(); j++)
            {
                EXPECT_EQ(
                    maneuveringSegmentSolution.states[startingIndexSecondManeuver + j].getInstant(),
                    maneuvers[1].getStates()[j].accessInstant()
                );
            }
        }

        // Check that when no thrusting is performed in a maneuvering segment that no maneuvers are outputted
        {
            // Pretend that we are already at the target COE, so no thrusting will occur
            const COE::CartesianState cartesianStatePair = defaultTargetCOE_.getCartesianState(
                EarthGravitationalModel::EGM2008.gravitationalParameter_, defaultFrameSPtr_
            );
            VectorXd currentCoordinates(7);
            currentCoordinates << cartesianStatePair.first.accessCoordinates(),
                cartesianStatePair.second.accessCoordinates(), 200.0;
            const State currentState = {
                Instant::J2000(),
                currentCoordinates,
                defaultFrameSPtr_,
                thrustCoordinateBrokerSPtr_,
            };

            const Segment::Solution maneuveringSegmentSolution = maneuveringSegment.solve(currentState);

            const Array<Maneuver> maneuvers = maneuveringSegmentSolution.extractManeuvers(defaultFrameSPtr_);

            EXPECT_EQ(0, maneuvers.getSize());
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_CalculateStatesAt)
{
    // Test that the function throws when the segment has no states
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {}, true, Segment::Type::Coast);

        EXPECT_THROW(
            segmentSolution.calculateStatesAt({Instant::J2000()}, defaultNumericalSolver_),
            ostk::core::error::RuntimeError
        );
    }

    // Test that the returns an empty state array if an empty instant array is provided
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        const Array<State> propagatedStates = segmentSolution.calculateStatesAt({}, defaultNumericalSolver_);

        EXPECT_EQ(0, propagatedStates.getSize());
    }

    // Test that the function throws when the instant array is out of order
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        EXPECT_THROW(
            segmentSolution.calculateStatesAt({Instant::Now(), Instant::J2000()}, defaultNumericalSolver_),
            ostk::core::error::runtime::Wrong
        );
    }

    // Test that the function throws when an instant outside the segment is desired
    {
        const Array<Instant> instantArrayOutsideSegment = {
            defaultState_.getInstant(), defaultState_.getInstant() + Duration::Minutes(1.0)
        };
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        EXPECT_THROW(
            segmentSolution.calculateStatesAt(instantArrayOutsideSegment, defaultNumericalSolver_),
            ostk::core::error::RuntimeError
        );
    }

    // Test successfull result for propagation to states within segment including bounds
    {
        const Array<Instant> instantArray = {
            defaultState_.getInstant(),
            defaultState_.getInstant() + Duration::Minutes(1.0),
            defaultState_.getInstant() + Duration::Minutes(2.0)
        };

        const State state1 = {
            defaultState_.getInstant() + Duration::Minutes(2.0),
            defaultState_.getPosition(),
            defaultState_.getVelocity(),
        };

        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_, state1}, true, Segment::Type::Coast);

        const Array<State> propagatedStates = segmentSolution.calculateStatesAt(instantArray, defaultNumericalSolver_);

        for (Size i = 0; i < instantArray.getSize(); i++)
        {
            EXPECT_EQ(instantArray[i], propagatedStates[i].getInstant());
        }
        EXPECT_EQ(defaultState_, propagatedStates[0]);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_GetDynamicsContribution)
{
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        const Shared<const Frame> stateFrame = defaultState_.accessFrame();
        for (const Shared<Dynamics>& dynamics : defaultDynamics_)
        {
            EXPECT_NO_THROW(segmentSolution.getDynamicsContribution(dynamics, stateFrame));
        }
    }

    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        const Shared<const Frame> stateFrame = defaultState_.accessFrame();
        for (const Shared<Dynamics>& dynamics : defaultDynamics_)
        {
            const MatrixXd contributionDefault = segmentSolution.getDynamicsContribution(dynamics, stateFrame);
            const Array<Shared<const CoordinateSubset>> dynamicsWriteCoordinateSubsets =
                dynamics->getWriteCoordinateSubsets();
            const MatrixXd contributionExplicit =
                segmentSolution.getDynamicsContribution(dynamics, stateFrame, dynamicsWriteCoordinateSubsets);
            EXPECT_EQ(contributionDefault, contributionExplicit);
        }
    }

    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        const Shared<const Frame> stateFrame = defaultState_.accessFrame();

        for (const Shared<Dynamics>& dynamics : defaultDynamics_)
        {
            const Array<Shared<const CoordinateSubset>> dynamicsWriteCoordinateSubsets =
                dynamics->getWriteCoordinateSubsets();
            const Shared<const CoordinateSubset> dynamicsWriteCoordinateSubset = dynamicsWriteCoordinateSubsets[0];
            const MatrixXd contribution =
                segmentSolution.getDynamicsContribution(dynamics, stateFrame, {dynamicsWriteCoordinateSubset});
            EXPECT_EQ(contribution.cols(), dynamicsWriteCoordinateSubset->getSize());
            EXPECT_EQ(contribution.rows(), segmentSolution.states.getSize());
        }
    }

    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        const Shared<const Frame> stateFrame = defaultState_.accessFrame();

        // Construct a coordinatesSubset not part of the dynamics for which the contribution is requested
        const Shared<Dynamics> dynamics = defaultDynamics_[0];
        const Shared<const CoordinateSubset> coordinatesSubset = CoordinateSubset::DragCoefficient();

        EXPECT_FALSE(dynamics->getWriteCoordinateSubsets().contains(coordinatesSubset));

        const String expectedString =
            "Provided coordinate subset is not part of the dynamics write coordinate subsets.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    MatrixXd contribution =
                        segmentSolution.getDynamicsContribution(dynamics, stateFrame, {coordinatesSubset});
                }
                catch (const ostk::core::error::runtime::Undefined& e)
                {
                    EXPECT_EQ(expectedString, e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        // Construct a dynamics not part of the segment
        const Earth earth = Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
        );
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
        const Shared<Dynamics> atmosphericDragDynamics = std::make_shared<AtmosphericDrag>(earthSPtr);

        const Shared<const Frame> stateFrame = defaultState_.accessFrame();

        const String expectedString = "Provided dynamics is not part of the segment dynamics.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    MatrixXd contribution =
                        segmentSolution.getDynamicsContribution(atmosphericDragDynamics, stateFrame);
                }
                catch (const ostk::core::error::runtime::Undefined& e)
                {
                    EXPECT_EQ(expectedString, e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_GetDynamicsAccelerationContribution)
{
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        const Shared<const Frame> stateFrame = defaultState_.accessFrame();

        // Check error for PositionDerivative
        const String expectedString =
            "Provided coordinate subset is not part of the dynamics write coordinate subsets.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    MatrixXd accelerationContribution =
                        segmentSolution.getDynamicsAccelerationContribution(defaultDynamics_[0], stateFrame);
                }
                catch (const ostk::core::error::runtime::Undefined& e)
                {
                    EXPECT_EQ(expectedString, e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );

        // Check output for Gravity
        MatrixXd accelerationContribution =
            segmentSolution.getDynamicsAccelerationContribution(defaultDynamics_[1], stateFrame);
        EXPECT_EQ(accelerationContribution.cols(), 3);
        EXPECT_EQ(accelerationContribution.rows(), segmentSolution.states.getSize());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_GetAllDynamicsContributions)
{
    {
        const Segment::Solution segmentSolution =
            Segment::Solution(defaultName_, defaultDynamics_, {defaultState_}, true, Segment::Type::Coast);

        const Shared<const Frame> stateFrame = defaultState_.accessFrame();
        Map<Shared<Dynamics>, MatrixXd> contributions = segmentSolution.getAllDynamicsContributions(stateFrame);

        EXPECT_EQ(defaultDynamics_.getSize(), contributions.size());
        for (const Shared<Dynamics>& dynamics : defaultDynamics_)
        {
            EXPECT_EQ(1, contributions.count(dynamics));  // Check all dynamics are present
            EXPECT_EQ(
                segmentSolution.states.getSize(), contributions.at(dynamics).rows()
            );  // Check the number of rows corresponds to the number of states
            EXPECT_GT(
                contributions.at(dynamics).cols(), dynamics->getWriteCoordinateSubsets().getSize()
            );  // Check the number of columns corresponds to the number of coordinate subsets to which the dynamics
                // writes
        }
    }

    {
        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Maneuver
        );

        const Shared<const Frame> stateFrame = initialStateWithMass_.accessFrame();
        Map<Shared<Dynamics>, MatrixXd> contributions = segmentSolution.getAllDynamicsContributions(stateFrame);

        EXPECT_EQ(defaultDynamics_.getSize(), contributions.size());
        for (const Shared<Dynamics>& dynamics : defaultDynamics_)
        {
            EXPECT_EQ(1, contributions.count(dynamics));  // Check all dynamics are present
            EXPECT_EQ(
                segmentSolution.states.getSize(), contributions.at(dynamics).rows()
            );  // Check the number of rows corresponds to the number of states
            EXPECT_GT(
                contributions.at(dynamics).cols(), dynamics->getWriteCoordinateSubsets().getSize()
            );  // Check the number of columns corresponds to the number of coordinate subsets to which the dynamics
                // writes
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_Print)
{
    {
        testing::internal::CaptureStdout();

        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Maneuver
        );

        EXPECT_NO_THROW(segmentSolution.print(std::cout, true));
        EXPECT_NO_THROW(segmentSolution.print(std::cout, false));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, SegmentSolution_StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        const Segment::Solution segmentSolution = Segment::Solution(
            defaultName_, defaultDynamics_, {initialStateWithMass_, finalStateWithMass_}, true, Segment::Type::Maneuver
        );

        EXPECT_NO_THROW(std::cout << segmentSolution << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, Coast)
{
    {
        EXPECT_NO_THROW(
            Segment::Coast(defaultName_, defaultInstantCondition_, defaultDynamics_, defaultNumericalSolver_)
        );
    }

    {
        EXPECT_THROW(
            Segment::Coast(defaultName_, nullptr, defaultDynamics_, NumericalSolver::Undefined()),
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            Segment::Coast(defaultName_, defaultInstantCondition_, {}, NumericalSolver::Undefined()),
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            Segment::Coast(defaultName_, defaultInstantCondition_, defaultDynamics_, NumericalSolver::Undefined()),
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const Array<Shared<Dynamics>> dynamicsWithThruster =
            defaultDynamics_ + Array<Shared<Dynamics>> {defaultThrusterDynamicsSPtr_};

        EXPECT_THROW(
            {
                try
                {
                    Segment::Coast(
                        defaultName_, defaultInstantCondition_, dynamicsWithThruster, defaultNumericalSolver_
                    );
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_NE(
                        e.getMessage().find("Coast dynamics array cannot contain Thruster dynamics"), std::string::npos
                    );
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, Maneuver)
{
    {
        const Segment segment = Segment::Maneuver(
            defaultName_,
            defaultInstantCondition_,
            defaultThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_
        );
    }

    {
        const Array<Shared<Dynamics>> dynamicsWithThruster =
            defaultDynamics_ + Array<Shared<Dynamics>> {defaultThrusterDynamicsSPtr_};

        EXPECT_THROW(
            {
                try
                {
                    Segment::Maneuver(
                        defaultName_,
                        defaultInstantCondition_,
                        defaultThrusterDynamicsSPtr_,
                        dynamicsWithThruster,
                        defaultNumericalSolver_
                    );
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_NE(
                        e.getMessage().find("Coast dynamics array cannot contain Thruster dynamics"), std::string::npos
                    );
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, ConstantLocalOrbitalFrameDirectionManeuver)
{
    {
        EXPECT_NO_THROW(Segment::ConstantLocalOrbitalFrameDirectionManeuver(
            defaultName_,
            defaultInstantCondition_,
            defaultThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_,
            defaultLocalOrbitalFrameFactorySPtr_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, GetName)
{
    EXPECT_EQ(defaultName_, defaultCoastSegment_.getName());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, GetEventCondition)
{
    EXPECT_TRUE(defaultCoastSegment_.getEventCondition() == defaultInstantCondition_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, GetDynamics)
{
    EXPECT_EQ(defaultDynamics_, defaultCoastSegment_.getDynamics());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, getFreeDynamics)
{
    EXPECT_EQ(defaultDynamics_, defaultCoastSegment_.getFreeDynamics());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, GetNumericalSolver)
{
    EXPECT_EQ(defaultNumericalSolver_, defaultCoastSegment_.getNumericalSolver());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, GetType)
{
    EXPECT_EQ(Segment::Type::Coast, defaultCoastSegment_.getType());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, GetThrusterDynamics)
{
    // Coast segment should throw when trying to get thruster dynamics
    {
        EXPECT_THROW(
            {
                try
                {
                    defaultCoastSegment_.getThrusterDynamics();
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Coast segment does not have thruster dynamics.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    // Maneuver segment should return thruster dynamics
    {
        const Segment maneuverSegment = Segment::Maneuver(
            defaultName_,
            defaultInstantCondition_,
            defaultThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_
        );

        const Shared<Thruster> thrusterDynamics = maneuverSegment.getThrusterDynamics();
        EXPECT_EQ(defaultThrusterDynamicsSPtr_, thrusterDynamics);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, AccessEventCondition)
{
    EXPECT_TRUE(defaultCoastSegment_.accessEventCondition() == defaultInstantCondition_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, AccessNumericalSolver)
{
    EXPECT_EQ(defaultNumericalSolver_, defaultCoastSegment_.accessNumericalSolver());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, ToCoastSegment)
{
    // Coast segment to coast segment
    {
        const Segment newCoastSegment = defaultCoastSegment_.toCoastSegment("New Coast Segment");

        EXPECT_EQ("New Coast Segment", newCoastSegment.getName());
        EXPECT_EQ(Segment::Type::Coast, newCoastSegment.getType());
        EXPECT_EQ(defaultInstantCondition_, newCoastSegment.getEventCondition());
        EXPECT_EQ(defaultDynamics_, newCoastSegment.getDynamics());
        EXPECT_EQ(defaultNumericalSolver_, newCoastSegment.getNumericalSolver());
    }

    // Maneuver segment to coast segment with custom name
    {
        const Segment maneuverSegment = Segment::Maneuver(
            defaultName_,
            defaultInstantCondition_,
            defaultThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_
        );

        const String newName = "New Coast Segment";
        const Segment coastSegment = maneuverSegment.toCoastSegment(newName);

        EXPECT_EQ(newName, coastSegment.getName());
        EXPECT_EQ(Segment::Type::Coast, coastSegment.getType());
        EXPECT_EQ(defaultInstantCondition_, coastSegment.getEventCondition());
        EXPECT_EQ(defaultDynamics_, coastSegment.getDynamics());
        EXPECT_EQ(defaultNumericalSolver_, coastSegment.getNumericalSolver());
        EXPECT_THROW(coastSegment.getThrusterDynamics(), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, ToManeuverSegment)
{
    // Convert maneuver segment to maneuver segment
    {
        const Segment originalManeuverSegment = Segment::Maneuver(
            defaultName_,
            defaultInstantCondition_,
            defaultThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_
        );

        const Shared<Thruster> newThrusterDynamics =
            std::make_shared<Thruster>(defaultSatelliteSystem_, constantThrustSPtr_);

        const Segment newManeuverSegment =
            originalManeuverSegment.toManeuverSegment(newThrusterDynamics, "New Maneuver Segment");

        EXPECT_EQ("New Maneuver Segment", newManeuverSegment.getName());
        EXPECT_EQ(Segment::Type::Maneuver, newManeuverSegment.getType());
        EXPECT_EQ(defaultInstantCondition_, newManeuverSegment.getEventCondition());
        EXPECT_EQ(defaultDynamics_, newManeuverSegment.getDynamics());
        EXPECT_EQ(defaultNumericalSolver_, newManeuverSegment.getNumericalSolver());
        EXPECT_EQ(newThrusterDynamics, newManeuverSegment.getThrusterDynamics());
        EXPECT_NE(defaultThrusterDynamicsSPtr_, newManeuverSegment.getThrusterDynamics());
    }

    // Convert coast segment to maneuver segment
    {
        const Segment maneuverSegment =
            defaultCoastSegment_.toManeuverSegment(defaultThrusterDynamicsSPtr_, "New Maneuver Segment");

        EXPECT_EQ("New Maneuver Segment", maneuverSegment.getName());
        EXPECT_EQ(Segment::Type::Maneuver, maneuverSegment.getType());
        EXPECT_EQ(defaultInstantCondition_, maneuverSegment.getEventCondition());
        EXPECT_EQ(defaultDynamics_, maneuverSegment.getDynamics());
        EXPECT_EQ(defaultNumericalSolver_, maneuverSegment.getNumericalSolver());
        EXPECT_EQ(defaultThrusterDynamicsSPtr_, maneuverSegment.getThrusterDynamics());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultCoastSegment_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, Solve)
{
    {
        const Segment::Solution solution = defaultCoastSegment_.solve(defaultState_);

        EXPECT_LT(
            (solution.states.accessLast().getInstant() - defaultInstantCondition_->getInstant()).inSeconds(), 1e-7
        );
        EXPECT_TRUE(solution.states.getSize() > 0);
    }

    {
        const Shared<RealCondition> eventCondition = std::make_shared<RealCondition>(COECondition::Eccentricity(
            RealCondition::Criterion::AnyCrossing,
            defaultFrameSPtr_,
            Real(0.5),
            EarthGravitationalModel::EGM2008.gravitationalParameter_
        ));

        const Segment segment =
            Segment::Coast("SMA condition", eventCondition, defaultDynamics_, defaultNumericalSolver_);

        const Segment::Solution solution = segment.solve(defaultState_, Duration::Minutes(1.0));

        EXPECT_TRUE(solution.states.getSize() > 0);
        EXPECT_FALSE(solution.conditionIsSatisfied);
    }

    // No maneuvers should be produced
    {
        const NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaDopri5,
            1.0,
            1.0e-12,
            1.0e-12,
        };

        VectorXd initialCoordinates(7);
        initialCoordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;
        const State initialState = {Instant::J2000(), initialCoordinates, Frame::GCRF(), thrustCoordinateBrokerSPtr_};

        const Shared<InstantCondition> eventCondition = std::make_shared<InstantCondition>(
            InstantCondition::Criterion::AnyCrossing, Instant::J2000() + Duration::Minutes(60.0)
        );

        const Duration tolerance = Duration::Milliseconds(1.0);

        const Shared<Thruster> customThrusterDynamics = std::make_shared<Thruster>(
            defaultSatelliteSystem_, std::make_shared<CustomGuidanceLaw>(Array<Interval>::Empty())
        );

        Segment segment = Segment::Maneuver(
            "Maneuvering Segment with Custom Guidance Law",
            eventCondition,
            customThrusterDynamics,
            defaultDynamics_,
            numericalSolver
        );

        {
            const Segment::Solution solution = segment.solve(initialState, Duration::Minutes(80.0));

            ASSERT_FALSE(solution.states.isEmpty());
            EXPECT_TRUE(solution.states.accessFirst().getInstant().isNear(initialState.getInstant(), tolerance));
            EXPECT_TRUE(solution.states.accessLast().getInstant().isNear(
                initialState.getInstant() + Duration::Minutes(60.0), tolerance
            ));
            EXPECT_TRUE(solution.conditionIsSatisfied);
            EXPECT_TRUE(solution.extractManeuvers(defaultFrameSPtr_).isEmpty());
        }

        {
            const Segment::Solution solution = segment.solveToNextManeuver(initialState, Duration::Minutes(80.0));

            ASSERT_FALSE(solution.states.isEmpty());
            EXPECT_TRUE(solution.states.accessFirst().getInstant().isNear(initialState.getInstant(), tolerance));
            EXPECT_TRUE(solution.states.accessLast().getInstant().isNear(
                initialState.getInstant() + Duration::Minutes(60.0), tolerance
            ));
            EXPECT_TRUE(solution.conditionIsSatisfied);
            EXPECT_TRUE(solution.extractManeuvers(defaultFrameSPtr_).isEmpty());
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, Solve_MaximumAllowedAngularOffset)
{
    // Single Maneuver
    {
        Segment maneuveringSegment = Segment::Maneuver(
            "Maneuvering Segment",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(45.0))
            ),
            defaultQLawThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_
        );

        const COE::CartesianState cartesianStatePair = defaultCurrentCOE_.getCartesianState(
            EarthGravitationalModel::EGM2008.gravitationalParameter_, defaultFrameSPtr_
        );
        VectorXd currentCoordinates(7);
        currentCoordinates << cartesianStatePair.first.accessCoordinates(),
            cartesianStatePair.second.accessCoordinates(), 200.0;
        const State currentState = {
            Instant::J2000(),
            currentCoordinates,
            defaultFrameSPtr_,
            thrustCoordinateBrokerSPtr_,
        };

        const Maneuver maneuver = maneuveringSegment.solve(currentState).extractManeuvers(defaultFrameSPtr_)[0];
        const Maneuver::MeanDirectionAndMaximumAngularOffset meanDirectionAndMaximumAngularOffset =
            maneuver.calculateMeanThrustDirectionAndMaximumAngularOffset(defaultLocalOrbitalFrameFactorySPtr_);

        // Maximum allowed angular offset is not provided
        {
            Segment constantLofDirectionManeuveringSegment = Segment::ConstantLocalOrbitalFrameDirectionManeuver(
                "Constant Local Orbital Frame Direction Maneuver Segment",
                std::make_shared<RealCondition>(
                    RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(45.0))
                ),
                defaultQLawThrusterDynamicsSPtr_,
                defaultDynamics_,
                defaultNumericalSolver_,
                defaultLocalOrbitalFrameFactorySPtr_
            );

            const Segment::Solution constantLofDirectionManeuveringSegmentSolution =
                constantLofDirectionManeuveringSegment.solve(currentState);

            EXPECT_TRUE(constantLofDirectionManeuveringSegmentSolution.conditionIsSatisfied);
        }

        // Maximum allowed angular offset is provided but not violated
        {
            Segment constantLofDirectionManeuveringSegment = Segment::ConstantLocalOrbitalFrameDirectionManeuver(
                "Constant Local Orbital Frame Direction Maneuver Segment",
                std::make_shared<RealCondition>(
                    RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(45.0))
                ),
                defaultQLawThrusterDynamicsSPtr_,
                defaultDynamics_,
                defaultNumericalSolver_,
                defaultLocalOrbitalFrameFactorySPtr_,
                Angle::Degrees(1.1 * meanDirectionAndMaximumAngularOffset.second.inDegrees(0.0, 360.0))
            );

            const Segment::Solution constantLofDirectionManeuveringSegmentSolution =
                constantLofDirectionManeuveringSegment.solve(currentState);

            EXPECT_TRUE(constantLofDirectionManeuveringSegmentSolution.conditionIsSatisfied);
        }

        // Maximum allowed angular offset is provided and violated
        {
            Segment constantLofDirectionManeuveringSegment = Segment::ConstantLocalOrbitalFrameDirectionManeuver(
                "Constant Local Orbital Frame Direction Maneuver Segment",
                std::make_shared<RealCondition>(
                    RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(45.0))
                ),
                defaultQLawThrusterDynamicsSPtr_,
                defaultDynamics_,
                defaultNumericalSolver_,
                defaultLocalOrbitalFrameFactorySPtr_,
                Angle::Degrees(0.9 * meanDirectionAndMaximumAngularOffset.second.inDegrees(0.0, 360.0))
            );

            EXPECT_THROW(
                try {
                    constantLofDirectionManeuveringSegment.solve(currentState);
                } catch (const ostk::core::error::RuntimeError& e) {
                    EXPECT_NE(e.getMessage().find("Maximum angular offset"), std::string::npos);
                    EXPECT_NE(e.getMessage().find(" is greater than the maximum allowed ("), std::string::npos);
                    throw;
                },
                ostk::core::error::RuntimeError
            );
        }
    }

    // Multiple maneuvers
    {
        Segment maneuveringSegment = Segment::Maneuver(
            "Maneuvering Segment",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(90.0))
            ),
            defaultQLawThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_
        );

        Segment constantLofDirectionManeuveringSegment = Segment::ConstantLocalOrbitalFrameDirectionManeuver(
            "Constant Local Orbital Frame Direction Maneuver Segment",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(90.0))
            ),
            defaultQLawThrusterDynamicsSPtr_,
            defaultDynamics_,
            defaultNumericalSolver_,
            defaultLocalOrbitalFrameFactorySPtr_
        );

        const COE::CartesianState cartesianStatePair = defaultCurrentCOE_.getCartesianState(
            EarthGravitationalModel::EGM2008.gravitationalParameter_, defaultFrameSPtr_
        );
        VectorXd currentCoordinates(7);
        currentCoordinates << cartesianStatePair.first.accessCoordinates(),
            cartesianStatePair.second.accessCoordinates(), 200.0;
        const State currentState = {
            Instant::J2000(),
            currentCoordinates,
            defaultFrameSPtr_,
            thrustCoordinateBrokerSPtr_,
        };

        const Segment::Solution maneuveringSegmentSolution = maneuveringSegment.solve(currentState);
        const Segment::Solution constantLofDirectionManeuveringSegmentSolution =
            constantLofDirectionManeuveringSegment.solve(currentState);

        const Array<Maneuver> maneuvers = maneuveringSegmentSolution.extractManeuvers(defaultFrameSPtr_);
        const Array<Maneuver> constantLofDirectionManeuvers =
            constantLofDirectionManeuveringSegmentSolution.extractManeuvers(defaultFrameSPtr_);

        EXPECT_TRUE(maneuveringSegmentSolution.accessStartInstant().isNear(
            constantLofDirectionManeuveringSegmentSolution.accessStartInstant(), Duration::Milliseconds(0.0)
        ));
        EXPECT_TRUE(maneuveringSegmentSolution.accessEndInstant().isNear(
            constantLofDirectionManeuveringSegmentSolution.accessEndInstant(), Duration::Milliseconds(1.0)
        ));
        EXPECT_TRUE(constantLofDirectionManeuveringSegmentSolution.conditionIsSatisfied);
        EXPECT_TRUE(maneuveringSegmentSolution.conditionIsSatisfied);
        EXPECT_EQ(2, maneuvers.getSize());
        EXPECT_EQ(2, constantLofDirectionManeuvers.getSize());

        for (Size i = 0; i < maneuvers.getSize(); i++)
        {
            EXPECT_TRUE(maneuvers[i].getInterval().getStart().isNear(
                constantLofDirectionManeuvers[i].getInterval().getStart(), Duration::Seconds(1.5)
            ));
            EXPECT_TRUE(maneuvers[i].getInterval().getEnd().isNear(
                constantLofDirectionManeuvers[i].getInterval().getEnd(), Duration::Seconds(1.5)
            ));
        }

        const Shared<HeterogeneousGuidanceLaw> heterogeneousGuidanceLaw =
            std::make_shared<HeterogeneousGuidanceLaw>(HeterogeneousGuidanceLaw());
        for (Size i = 0; i < maneuvers.getSize(); i++)
        {
            heterogeneousGuidanceLaw->addGuidanceLaw(
                std::make_shared<ConstantThrust>(
                    ConstantThrust::FromManeuver(maneuvers[i], defaultLocalOrbitalFrameFactorySPtr_)
                ),
                maneuvers[i].getInterval()
            );
        }

        Segment expectedEquivalentSegment = Segment::Maneuver(
            "Expected Equivalent Maneuvering Segment",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::AnyCrossing, Duration::Minutes(90.0))
            ),
            std::make_shared<Thruster>(
                defaultQLawThrusterDynamicsSPtr_->getSatelliteSystem(),
                heterogeneousGuidanceLaw,
                defaultQLawThrusterDynamicsSPtr_->getName() + " (With Heterogeneous Guidance Law)"
            ),
            defaultDynamics_,
            defaultNumericalSolver_
        );

        const Segment::Solution expectedEquivalentSegmentSolution = expectedEquivalentSegment.solve(currentState);
        const Array<Maneuver> expectedEquivalentManeuvers =
            expectedEquivalentSegmentSolution.extractManeuvers(defaultFrameSPtr_);
        EXPECT_TRUE(maneuveringSegmentSolution.accessStartInstant().isNear(
            expectedEquivalentSegmentSolution.accessStartInstant(), Duration::Milliseconds(0.0)
        ));
        EXPECT_TRUE(maneuveringSegmentSolution.accessEndInstant().isNear(
            expectedEquivalentSegmentSolution.accessEndInstant(), Duration::Milliseconds(1.0)
        ));
        EXPECT_TRUE(expectedEquivalentSegmentSolution.conditionIsSatisfied);

        EXPECT_EQ(2, expectedEquivalentManeuvers.getSize());
        for (Size i = 0; i < expectedEquivalentManeuvers.getSize(); i++)
        {
            EXPECT_TRUE(expectedEquivalentManeuvers[i].getInterval().getStart().isNear(
                maneuvers[i].getInterval().getStart(), Duration::Seconds(1.5)
            ));
            EXPECT_TRUE(expectedEquivalentManeuvers[i].getInterval().getEnd().isNear(
                maneuvers[i].getInterval().getEnd(), Duration::Seconds(1.5)
            ));
        }

        const State finalState = constantLofDirectionManeuveringSegmentSolution.states.accessLast();
        const State expectedFinalState = expectedEquivalentSegmentSolution.states.accessLast();
        EXPECT_EQ(finalState, expectedFinalState);
    }
}

// Define parameter structure for parametrized tests
struct SolveTestParams
{
    String description;
    Array<Tuple<Duration, Duration>> maneuverIntervals;
    bool solveMultipleManeuvers;
    Duration expectedFinalDuration;
    bool expectedConditionSatisfied;
    Array<Tuple<Duration, Duration>> expectedManeuverIntervals;
};

class OpenSpaceToolkit_Astrodynamics_Trajectory_Segment_Solve_Parameterized
    : public ::testing::TestWithParam<SolveTestParams>
{
   protected:
    const Shared<const Frame> defaultFrameSPtr_ = Frame::GCRF();
    const Shared<Celestial> earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> defaultDynamics_ = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSpherical_),
    };
    const SatelliteSystem defaultSatelliteSystem_ = SatelliteSystem::Default();
    const Shared<CoordinateBroker> thrustCoordinateBrokerSPtr_ = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));
};

INSTANTIATE_TEST_SUITE_P(
    SolveParameterizedTests,
    OpenSpaceToolkit_Astrodynamics_Trajectory_Segment_Solve_Parameterized,
    ::testing::Values(
        // Leading edge maneuver - allow multiple
        SolveTestParams {
            "LeadingEdge_MultipleManeuvers",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-10.0), Duration::Minutes(10.0)}
            },
            true,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(0.0), Duration::Minutes(10.0)}
            }
        },
        // Leading edge maneuver - single only
        SolveTestParams {
            "LeadingEdge_NextOnly",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-10.0), Duration::Minutes(10.0)}
            },
            false,
            Duration::Minutes(10.0),
            false,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(0.0), Duration::Minutes(10.0)}
            }
        },
        // Middle maneuver - allow multiple
        SolveTestParams {
            "Middle_MultipleManeuvers",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(20.0)}
            },
            true,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(20.0)}
            }
        },
        // Middle maneuver - single only
        SolveTestParams {
            "Middle_NextOnly",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(20.0)}
            },
            false,
            Duration::Minutes(20.0),
            false,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(20.0)}
            }
        },
        // Trailing edge maneuver - allow multiple
        SolveTestParams {
            "TrailingEdge_MultipleManeuvers",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(70.0)}
            },
            true,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(60.0)}
            }
        },
        // Trailing edge maneuver - single only
        SolveTestParams {
            "TrailingEdge_NextOnly",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(70.0)}
            },
            false,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(60.0)}
            }
        },
        // Maneuver spans entire interval - allow multiple
        SolveTestParams {
            "SpansEntireInterval_MultipleManeuvers",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-10.0), Duration::Minutes(70.0)}
            },
            true,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(0.0), Duration::Minutes(60.0)}
            }
        },
        // Maneuver spans entire interval - single only
        SolveTestParams {
            "SpansEntireInterval_NextOnly",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-10.0), Duration::Minutes(70.0)}
            },
            false,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(0.0), Duration::Minutes(60.0)}
            }
        },
        // Maneuver ends at condition time - allow multiple
        SolveTestParams {
            "EndsAtCondition_MultipleManeuvers",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(60.0)}
            },
            true,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(60.0)}
            }
        },
        // Maneuver ends at condition time - single only
        SolveTestParams {
            "EndsAtCondition_NextOnly",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(60.0)}
            },
            false,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(60.0)}
            }
        },
        // Leading edge + middle maneuvers - allow multiple
        SolveTestParams {
            "LeadingAndMiddle_MultipleManeuvers",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-10.0), Duration::Minutes(10.0)},
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(50.0)}
            },
            true,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(0.0), Duration::Minutes(10.0)},
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(50.0)}
            }
        },
        // Leading edge + middle maneuvers - single only
        SolveTestParams {
            "LeadingAndMiddle_NextOnly",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-10.0), Duration::Minutes(10.0)},
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(50.0)}
            },
            false,
            Duration::Minutes(10.0),
            false,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(0.0), Duration::Minutes(10.0)}
            }
        },
        // Two middle maneuvers - allow multiple
        SolveTestParams {
            "TwoMiddle_MultipleManeuvers",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(20.0)},
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(50.0)}
            },
            true,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(20.0)},
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(50.0)}
            }
        },
        // Two middle maneuvers - single only
        SolveTestParams {
            "TwoMiddle_NextOnly",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(20.0)},
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(50.0)}
            },
            false,
            Duration::Minutes(20.0),
            false,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(20.0)}
            }
        },
        // Trailing maneuver with one beyond condition - allow multiple
        SolveTestParams {
            "TrailingWithBeyond_MultipleManeuvers",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(70.0)},
                Tuple<Duration, Duration> {Duration::Minutes(80.0), Duration::Minutes(90.0)}
            },
            true,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(60.0)}
            }
        },
        // Trailing maneuver with one beyond condition - single only
        SolveTestParams {
            "TrailingWithBeyond_NextOnly",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(70.0)},
                Tuple<Duration, Duration> {Duration::Minutes(80.0), Duration::Minutes(90.0)}
            },
            false,
            Duration::Minutes(60.0),
            true,
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(60.0)}
            }
        }
    ),
    [](const ::testing::TestParamInfo<SolveTestParams>& paramInfo)
    {
        return paramInfo.param.description;
    }
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment_Solve_Parameterized, ManeuverScenarios)
{
    const auto params = GetParam();

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        1.0,
        1.0e-12,
        1.0e-12,
    };

    VectorXd initialCoordinates(7);
    initialCoordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;
    const Instant referenceInstant = Instant::J2000();
    const State initialState = {referenceInstant, initialCoordinates, Frame::GCRF(), thrustCoordinateBrokerSPtr_};

    const Shared<InstantCondition> eventCondition = std::make_shared<InstantCondition>(
        InstantCondition::Criterion::AnyCrossing, referenceInstant + Duration::Minutes(60.0)
    );

    const Duration tolerance = Duration::Milliseconds(1.0);

    Array<Interval> guidanceLawIntervals = Array<Interval>::Empty();
    for (const auto& durationTuple : params.maneuverIntervals)
    {
        guidanceLawIntervals.add(Interval::Closed(
            referenceInstant + std::get<0>(durationTuple), referenceInstant + std::get<1>(durationTuple)
        ));
    }

    const Shared<Thruster> customThrusterDynamics =
        std::make_shared<Thruster>(defaultSatelliteSystem_, std::make_shared<CustomGuidanceLaw>(guidanceLawIntervals));

    Segment segment = Segment::Maneuver(
        "Maneuvering Segment with Custom Guidance Law",
        eventCondition,
        customThrusterDynamics,
        defaultDynamics_,
        numericalSolver
    );

    const Segment::Solution solution = params.solveMultipleManeuvers
                                         ? segment.solve(initialState, Duration::Minutes(80.0))
                                         : segment.solveToNextManeuver(initialState, Duration::Minutes(80.0));

    Array<Interval> expectedManeuverIntervals = Array<Interval>::Empty();
    for (const auto& durationTuple : params.expectedManeuverIntervals)
    {
        expectedManeuverIntervals.add(Interval::Closed(
            referenceInstant + std::get<0>(durationTuple), referenceInstant + std::get<1>(durationTuple)
        ));
    }

    ASSERT_FALSE(solution.states.isEmpty());
    EXPECT_TRUE(solution.states.accessFirst().getInstant().isNear(initialState.getInstant(), tolerance));
    EXPECT_TRUE(solution.states.accessLast().getInstant().isNear(
        initialState.getInstant() + params.expectedFinalDuration, tolerance
    ));
    EXPECT_EQ(solution.conditionIsSatisfied, params.expectedConditionSatisfied);

    const Array<Maneuver> maneuvers = solution.extractManeuvers(defaultFrameSPtr_);
    EXPECT_EQ(maneuvers.getSize(), expectedManeuverIntervals.getSize());

    for (Size idx = 0; idx < expectedManeuverIntervals.getSize(); ++idx)
    {
        EXPECT_TRUE(expectedManeuverIntervals[idx].getStart().isNear(maneuvers[idx].getInterval().getStart(), tolerance)
        );
        EXPECT_TRUE(expectedManeuverIntervals[idx].getEnd().isNear(maneuvers[idx].getInterval().getEnd(), tolerance));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Segment, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultCoastSegment_.print(std::cout, true));
    EXPECT_NO_THROW(defaultCoastSegment_.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}
