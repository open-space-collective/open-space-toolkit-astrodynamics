/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Index;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
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
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::AngularCondition;
using ostk::astrodynamics::eventcondition::COECondition;
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::eventcondition::LogicalCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::Segment;
using ostk::astrodynamics::trajectory::Sequence;
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

class OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence : public ::testing::Test
{
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
        NumericalSolver::LogType::NoLog,
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

    const Segment coastSegment_ = Segment::Coast("Coast", defaultCondition_, defaultDynamics_, defaultNumericalSolver_);

    const Array<Segment> defaultSegments_ = {coastSegment_};

    const Segment::Solution defaultSegmentSolution_ = {
        "A Segment",
        defaultDynamics_,
        {defaultState_},
        true,
        Segment::Type::Coast,
    };

    const Size defaultVerbosityLevel_ = 1;

    const Size defaultRepetitionCount_ = 2;
    const Duration defaultMaximumPropagationDuration_ = Duration::Days(7.0);
    Sequence defaultSequence_ = {
        defaultSegments_,
        defaultNumericalSolver_,
        defaultDynamics_,
        defaultMaximumPropagationDuration_,
        defaultVerbosityLevel_,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, SequenceSolution)
{
    // Test throw with empty sequence solving
    {
        Sequence::Solution sequenceSolution = {{}, true};
        EXPECT_THROW(sequenceSolution.getStates(), ostk::core::error::RuntimeError);
    }

    {
        const auto generateState = [this](const Instant& anInstant) -> State
        {
            VectorXd coordinates(7);
            coordinates << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 100.0;
            return {
                anInstant,
                coordinates,
                Frame::GCRF(),
                {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()},
            };
        };

        const State state1 = generateState(defaultState_.getInstant());
        const State state2 = generateState(defaultState_.getInstant() + Duration::Minutes(1.0));
        const State state3 = generateState(defaultState_.getInstant() + Duration::Minutes(2.0));

        const Segment::Solution segmentSolution1 = {
            "A Segment",
            defaultDynamics_,
            {state1, state2},
            true,
            Segment::Type::Coast,
        };

        const Segment::Solution segmentSolution2 = {
            "A Segment",
            defaultDynamics_,
            {state2, state3},
            true,
            Segment::Type::Coast,
        };

        const Sequence::Solution sequenceSolution = {{segmentSolution1, segmentSolution2}, true};

        EXPECT_EQ(sequenceSolution.accessStartInstant(), state1.getInstant());
        EXPECT_EQ(sequenceSolution.accessEndInstant(), state3.getInstant());
        EXPECT_EQ(sequenceSolution.getInterval(), Interval::Closed(state1.getInstant(), state3.getInstant()));
        EXPECT_EQ(
            sequenceSolution.getInitialMass().inKilograms(), state1.extractCoordinate(CoordinateSubset::Mass())[0]
        );
        EXPECT_EQ(sequenceSolution.getFinalMass().inKilograms(), state3.extractCoordinate(CoordinateSubset::Mass())[0]);
        EXPECT_EQ(sequenceSolution.getPropagationDuration(), state3.getInstant() - state1.getInstant());
        EXPECT_EQ(sequenceSolution.computeDeltaV(1.0), 0.0);
        EXPECT_EQ(sequenceSolution.computeDeltaMass().inKilograms(), 0.0);

        const Array<State> states = sequenceSolution.getStates();

        EXPECT_EQ(states[0], state1);
        EXPECT_EQ(states[1], state2);
        EXPECT_EQ(states[2], state3);
        EXPECT_EQ(states.getSize(), 3);

        const Array<State> propagatedStates = sequenceSolution.calculateStatesAt(
            {state1.getInstant(), state2.getInstant(), state3.getInstant()}, defaultNumericalSolver_
        );
        EXPECT_EQ(propagatedStates[0].getInstant(), state1.getInstant());
        EXPECT_EQ(propagatedStates[1].getInstant(), state2.getInstant());
        EXPECT_EQ(propagatedStates[2].getInstant(), state3.getInstant());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, SequenceSolution_CalculateStatesAt)
{
    // Test throw with empty sequence solving
    {
        Sequence::Solution sequenceSolution = {{}, true};
        EXPECT_THROW(
            sequenceSolution.calculateStatesAt({Instant::J2000()}, defaultNumericalSolver_),
            ostk::core::error::RuntimeError
        );
    }

    const State state1 = defaultState_;
    const State state2 = {
        defaultState_.getInstant() + Duration::Minutes(1.0),
        defaultState_.getPosition(),
        defaultState_.getVelocity(),
    };
    const State state3 = {
        defaultState_.getInstant() + Duration::Minutes(2.0),
        defaultState_.getPosition(),
        defaultState_.getVelocity(),
    };

    const Segment::Solution segmentSolution1 = {
        "A Segment",
        defaultDynamics_,
        {state1, state2},
        true,
        Segment::Type::Coast,
    };

    const Segment::Solution segmentSolution2 = {
        "A Segment",
        defaultDynamics_,
        {state2, state3},
        true,
        Segment::Type::Coast,
    };

    // Test regular states success
    {
        const Sequence::Solution sequenceSolution = {{segmentSolution1, segmentSolution2}, true};

        const Array<State> propagatedStates = sequenceSolution.calculateStatesAt(
            {
                state1.getInstant(),
                state2.getInstant(),
                state3.getInstant(),
            },
            defaultNumericalSolver_
        );

        EXPECT_EQ(propagatedStates[0].getInstant(), state1.getInstant());
        EXPECT_EQ(propagatedStates[1].getInstant(), state2.getInstant());
        EXPECT_EQ(propagatedStates[2].getInstant(), state3.getInstant());
        EXPECT_EQ(propagatedStates.getSize(), 3);
    }

    // Test regular states in between success
    {
        const Sequence::Solution sequenceSolution = {{segmentSolution1, segmentSolution2}, true};

        const Array<State> propagatedStatesInBetween = sequenceSolution.calculateStatesAt(
            {
                state1.getInstant() + Duration::Minutes(0.5),
                state2.getInstant() + Duration::Minutes(0.5),
            },
            defaultNumericalSolver_
        );

        EXPECT_EQ(propagatedStatesInBetween[0].getInstant(), state1.getInstant() + Duration::Minutes(0.5));
        EXPECT_EQ(propagatedStatesInBetween[1].getInstant(), state2.getInstant() + Duration::Minutes(0.5));
        EXPECT_EQ(propagatedStatesInBetween.getSize(), 2);
    }

    // Test regular states out of range failure
    {
        const Sequence::Solution sequenceSolution = {{segmentSolution1, segmentSolution2}, true};

        const Array<State> propagatedStatesOutsideSequence = sequenceSolution.calculateStatesAt(
            {
                state1.getInstant() - Duration::Minutes(0.5),
                state3.getInstant() + Duration::Minutes(0.5),
            },
            defaultNumericalSolver_
        );

        EXPECT_EQ(propagatedStatesOutsideSequence.getSize(), 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, SequenceSolution_ExtractManeuvers)
{
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> dynamicsSPtr = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
    };

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaDopri5, 1.0, 1.0e-12, 1.0e-12
    };

    const SatelliteSystem satelliteSystem = SatelliteSystem::Default();

    const Shared<const CoordinateBroker> coordinatesBrokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));

    VectorXd coordinates(7);
    coordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;
    const State initialState = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        coordinatesBrokerSPtr,
    };

    const Duration maximumPropagationDuration = Duration::Minutes(200.0);
    const Shared<const Frame> frameSPtr = Frame::GCRF();

    // With no maneuvers
    {
        Segment segment1 = Segment::Maneuver(
            "Segment 1",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(15.0))
            ),
            std::make_shared<Thruster>(satelliteSystem, std::make_shared<CustomGuidanceLaw>(Array<Interval> {})),
            dynamicsSPtr,
            numericalSolver
        );

        Segment segment2 = Segment::Maneuver(
            "Segment 2",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(15.0))
            ),
            std::make_shared<Thruster>(satelliteSystem, std::make_shared<CustomGuidanceLaw>(Array<Interval> {})),
            dynamicsSPtr,
            numericalSolver
        );

        Sequence sequence = {
            {
                segment1,
                segment2,
            },
            numericalSolver,
            dynamicsSPtr,
            maximumPropagationDuration,
        };

        const Sequence::Solution solution = sequence.solve(initialState, 2);

        const Array<Maneuver> maneuvers = solution.extractManeuvers(frameSPtr);
        EXPECT_EQ(maneuvers.getSize(), 0);
    }

    // With Maneuvers
    {
        Segment segment1 = Segment::Maneuver(
            "Segment 1",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(15.0))
            ),
            std::make_shared<Thruster>(
                satelliteSystem,
                std::make_shared<CustomGuidanceLaw>(Array<Interval> {
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(0.0), Instant::J2000() + Duration::Minutes(10.0)
                    ),
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(31.0), Instant::J2000() + Duration::Minutes(37.0)
                    ),
                })
            ),
            dynamicsSPtr,
            numericalSolver
        );

        Segment segment2 = Segment::Maneuver(
            "Segment 2",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(15.0))
            ),
            std::make_shared<Thruster>(
                satelliteSystem,
                std::make_shared<CustomGuidanceLaw>(Array<Interval> {
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(50.0), Instant::J2000() + Duration::Minutes(55.0)
                    ),
                })
            ),
            dynamicsSPtr,
            numericalSolver
        );

        Sequence sequence = {
            {
                segment1,
                segment2,
            },
            numericalSolver,
            dynamicsSPtr,
            maximumPropagationDuration,
        };

        const Sequence::Solution solution = sequence.solve(initialState, 2);

        const Array<Maneuver> extractedManeuvers = solution.extractManeuvers(frameSPtr);
        EXPECT_EQ(extractedManeuvers.getSize(), 3);

        const Array<Segment::Solution> segmentSolutions = solution.segmentSolutions;
        EXPECT_EQ(segmentSolutions.getSize(), 4);

        Array<Maneuver> expectedManeuvers = Array<Maneuver>::Empty();

        const Array<Maneuver> segmentSolution1Maneuvers = segmentSolutions[0].extractManeuvers(frameSPtr);
        EXPECT_EQ(segmentSolution1Maneuvers.getSize(), 1);
        expectedManeuvers.add(segmentSolution1Maneuvers[0]);
        const Array<Maneuver> segmentSolution2Maneuvers = segmentSolutions[1].extractManeuvers(frameSPtr);
        EXPECT_EQ(segmentSolution2Maneuvers.getSize(), 0);
        const Array<Maneuver> segmentSolution3Maneuvers = segmentSolutions[2].extractManeuvers(frameSPtr);
        EXPECT_EQ(segmentSolution3Maneuvers.getSize(), 1);
        expectedManeuvers.add(segmentSolution3Maneuvers[0]);
        const Array<Maneuver> segmentSolution4Maneuvers = segmentSolutions[3].extractManeuvers(frameSPtr);
        EXPECT_EQ(segmentSolution4Maneuvers.getSize(), 1);
        expectedManeuvers.add(segmentSolution4Maneuvers[0]);

        for (Size idx = 0; idx < extractedManeuvers.getSize(); ++idx)
        {
            EXPECT_EQ(extractedManeuvers[idx], expectedManeuvers[idx]);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, SequenceSolution_Print)
{
    {
        testing::internal::CaptureStdout();

        Sequence::Solution sequenceSolution = {{defaultSegmentSolution_}, true};

        EXPECT_NO_THROW(sequenceSolution.print(std::cout, true));
        EXPECT_NO_THROW(sequenceSolution.print(std::cout, false));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, SequenceSolution_StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        Sequence::Solution sequenceSolution = {{defaultSegmentSolution_}, true};

        EXPECT_NO_THROW(std::cout << sequenceSolution << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Constructor)
{
    {
        EXPECT_NO_THROW(Sequence sequence);
    }

    {
        EXPECT_NO_THROW(Sequence sequence(defaultSegments_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(defaultSegments_, defaultNumericalSolver_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(defaultSegments_, defaultNumericalSolver_, defaultDynamics_));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(
            defaultSegments_, defaultNumericalSolver_, defaultDynamics_, defaultMaximumPropagationDuration_
        ));
    }

    {
        EXPECT_NO_THROW(Sequence sequence(
            defaultSegments_, defaultNumericalSolver_, defaultDynamics_, defaultMaximumPropagationDuration_
        ));
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Sequence sequence(
                        defaultSegments_, defaultNumericalSolver_, defaultDynamics_, Duration::Undefined()
                    );
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_NE(e.getMessage().find("Maximum propagation duration must be defined."), std::string::npos);
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Sequence sequence(
                        defaultSegments_, defaultNumericalSolver_, defaultDynamics_, Duration::Seconds(-1.0)
                    );
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_NE(
                        e.getMessage().find("Maximum propagation duration must be strictly positive."),
                        std::string::npos
                    );
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );

        EXPECT_THROW(
            {
                try
                {
                    Sequence sequence(defaultSegments_, defaultNumericalSolver_, defaultDynamics_, Duration::Zero());
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_NE(
                        e.getMessage().find("Maximum propagation duration must be strictly positive."),
                        std::string::npos
                    );
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        {
            for (Size verbosity = 0; verbosity <= 5; ++verbosity)
            {
                EXPECT_NO_THROW(Sequence sequence(
                    defaultSegments_,
                    defaultNumericalSolver_,
                    defaultDynamics_,
                    defaultMaximumPropagationDuration_,
                    verbosity
                ));
            }
        }

        {
            EXPECT_THROW(
                Sequence sequence(
                    defaultSegments_, defaultNumericalSolver_, defaultDynamics_, defaultMaximumPropagationDuration_, 6
                ),
                ostk::core::error::runtime::Wrong
            );
        }
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
    EXPECT_EQ(defaultSegments_.getSize(), defaultSequence_.getSegments().getSize());
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, AddSegment)
{
    {
        EXPECT_NO_THROW(defaultSequence_.addSegment(coastSegment_));
    }

    {
        EXPECT_NO_THROW(defaultSequence_.addSegments({coastSegment_, coastSegment_}));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, AddCoastSegment)
{
    {
        const Size segmentsCount = defaultSequence_.getSegments().getSize();

        defaultSequence_.addCoastSegment(std::make_shared<RealCondition>(COECondition::SemiMajorAxis(
            RealCondition::Criterion::AnyCrossing,
            Frame::GCRF(),
            Length::Kilometers(6999.5),
            EarthGravitationalModel::EGM2008.gravitationalParameter_
        )));

        EXPECT_TRUE(defaultSequence_.getSegments().getSize() == segmentsCount + 1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, AddManeuverSegment)
{
    {
        const Size segmentsCount = defaultSequence_.getSegments().getSize();

        const Shared<const ConstantThrust> constantThrustSPtr =
            std::make_shared<ConstantThrust>(ConstantThrust::Intrack());

        const Shared<Thruster> thrustDynamicsSPtr =
            std::make_shared<Thruster>(SatelliteSystem::Default(), constantThrustSPtr);

        defaultSequence_.addManeuverSegment(
            std::make_shared<RealCondition>(COECondition::SemiMajorAxis(
                RealCondition::Criterion::AnyCrossing,
                Frame::GCRF(),
                Length::Kilometers(7000.0),
                EarthGravitationalModel::EGM2008.gravitationalParameter_
            )),
            thrustDynamicsSPtr
        );

        EXPECT_TRUE(defaultSequence_.getSegments().getSize() == segmentsCount + 1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve)
{
    // default solve
    {
        EXPECT_THROW(defaultSequence_.solve(defaultState_, 0), ostk::core::error::runtime::Wrong);
    }

    {
        const Sequence::Solution solution = defaultSequence_.solve(defaultState_, defaultRepetitionCount_);

        EXPECT_TRUE(
            solution.segmentSolutions.getSize() == defaultSequence_.getSegments().getSize() * defaultRepetitionCount_
        );

        Size statesSize = 1;
        for (const Segment::Solution& segmentSolution : solution.segmentSolutions)
        {
            EXPECT_TRUE(segmentSolution.states.getSize() > 0);

            const Real targetAngle = defaultCondition_->getEvaluator()(segmentSolution.states.accessLast());
            EXPECT_NEAR(targetAngle, defaultCondition_->getTargetAngle().inRadians(0.0, Real::TwoPi()), 1e-6);

            statesSize += segmentSolution.states.getSize() - 1;
        }

        EXPECT_EQ(solution.getStates().getSize(), statesSize);
    }

    // segment termination due to maximum propagation duration
    {
        Sequence sequence = {
            defaultSegments_,
            defaultNumericalSolver_,
            defaultDynamics_,
            Duration::Seconds(1.0),
        };

        const Sequence::Solution solution = sequence.solve(defaultState_, defaultRepetitionCount_);

        EXPECT_FALSE(solution.executionIsComplete);
        EXPECT_EQ(solution.segmentSolutions.getSize(), 1);
        EXPECT_FALSE(solution.segmentSolutions[0].conditionIsSatisfied);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, SolveToCondition)
{
    // sequence completion due to event condition
    {
        const Sequence sequence = {
            defaultSegments_,
            defaultNumericalSolver_,
            defaultDynamics_,
            defaultMaximumPropagationDuration_,
        };

        const InstantCondition eventCondition = InstantCondition(
            InstantCondition::Criterion::StrictlyPositive, defaultState_.accessInstant() + Duration::Seconds(1.0)
        );

        const Sequence::Solution solution = sequence.solveToCondition(defaultState_, eventCondition);

        EXPECT_TRUE(solution.executionIsComplete);
        EXPECT_EQ(solution.segmentSolutions.getSize(), 1);
    }

    // sequence failure, segment termination due to maximum propagation duration
    {
        const Sequence sequence = {
            defaultSegments_,
            defaultNumericalSolver_,
            defaultDynamics_,
            Duration::Seconds(1.0),
        };

        const InstantCondition eventCondition = InstantCondition(
            InstantCondition::Criterion::StrictlyPositive, defaultState_.accessInstant() + Duration::Days(1.0)
        );

        const Sequence::Solution solution = sequence.solveToCondition(defaultState_, eventCondition);

        EXPECT_FALSE(solution.executionIsComplete);
        EXPECT_EQ(solution.segmentSolutions.getSize(), 1);
        EXPECT_FALSE(solution.segmentSolutions[0].conditionIsSatisfied);
    }

    // sequence failure, event condition not met
    {
        const Sequence sequence = {
            defaultSegments_,
            defaultNumericalSolver_,
            defaultDynamics_,
            defaultMaximumPropagationDuration_,
        };

        const InstantCondition eventCondition = InstantCondition(
            InstantCondition::Criterion::StrictlyPositive, defaultState_.accessInstant() + Duration::Days(1.0)
        );

        const Sequence::Solution solution =
            sequence.solveToCondition(defaultState_, eventCondition, Duration::Minutes(1.0));

        EXPECT_FALSE(solution.executionIsComplete);
    }

    // sequence completion due to duration condition (Regression test)
    {
        const Segment segment = Segment::Coast(
            "Coast",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(10.0))
            ),
            defaultDynamics_,
            defaultNumericalSolver_
        );

        const Sequence sequence = {
            {segment},
            defaultNumericalSolver_,
            defaultDynamics_,
            defaultMaximumPropagationDuration_,
        };

        const RealCondition eventCondition =
            RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(1.0));

        const Sequence::Solution solution = sequence.solveToCondition(defaultState_, eventCondition);

        EXPECT_TRUE(solution.executionIsComplete);
        EXPECT_EQ(solution.segmentSolutions.getSize(), 6);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, SolveToCondition_SegmentIsCutShortDueToSequenceCondition)
{
    // Sequence condition depending only on start and end states
    {
        const Segment segment = Segment::Coast(
            "Coast",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(10.0))
            ),
            defaultDynamics_,
            defaultNumericalSolver_
        );

        const Sequence sequence = {
            {segment},
            defaultNumericalSolver_,
            defaultDynamics_,
            defaultMaximumPropagationDuration_,
        };

        const RealCondition sequenceCondition =
            RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(15.0));

        const Sequence::Solution solution = sequence.solveToCondition(defaultState_, sequenceCondition);

        EXPECT_EQ(solution.segmentSolutions.getSize(), 2);
        EXPECT_TRUE(solution.segmentSolutions[0].conditionIsSatisfied);
        EXPECT_FALSE(solution.segmentSolutions[1].conditionIsSatisfied);  // This currently fails as the last segment is not cut short
        EXPECT_TRUE(solution.executionIsComplete);  // This is "ok" just because the sequence condition just depends on start and end states
        EXPECT_NEAR((solution.accessEndInstant() - solution.accessStartInstant()).inSeconds(), 15.0, 1e-6);  // This currently fails as the last segment is not cut short
    }

    // Sequence condition skipped during segment solve
    {
        const Segment segment = Segment::Coast(
            "Coast",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(10.0))
            ),
            defaultDynamics_,
            defaultNumericalSolver_
        );

        const Sequence sequence = {
            {segment},
            defaultNumericalSolver_,
            defaultDynamics_,
            defaultMaximumPropagationDuration_,
        };

        const LogicalCondition sequenceCondition =
            LogicalCondition(
                "Sequence Condition",
                LogicalCondition::Type::And,
                {
                    std::make_shared<RealCondition>(
                        RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(13.0))
                    ),
                    std::make_shared<RealCondition>(
                        RealCondition::DurationCondition(RealCondition::Criterion::StrictlyNegative, Duration::Seconds(17.0))
                    )
                }
            );

        const Sequence::Solution solution = sequence.solveToCondition(
            defaultState_, sequenceCondition, Duration::Minutes(1.0)  // Currently here a maximum propagation duration to shorten the test
        );

        EXPECT_EQ(solution.segmentSolutions.getSize(), 2);
        EXPECT_TRUE(solution.segmentSolutions[0].conditionIsSatisfied);
        EXPECT_FALSE(solution.segmentSolutions[1].conditionIsSatisfied);  // This currently fails as the last segment is not cut short
        EXPECT_TRUE(solution.executionIsComplete);  // This currently fails as the sequence condition is evaluated using only start and end states
        EXPECT_NEAR((solution.accessEndInstant() - solution.accessStartInstant()).inSeconds(), 13.0, 1e-6);  // This currently fails as the last segment is not cut short
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

    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    const PropulsionSystem propulsionSystem = PropulsionSystem(1.0, 1500.0);

    const SatelliteSystem satelliteSystem = {
        Mass::Kilograms(100.0),
        satelliteGeometry,
        Matrix3d::Identity(),
        500.0,
        2.1,
        propulsionSystem,
    };

    const Real mass = satelliteSystem.getMass().inKilograms();
    const Real surfaceArea = satelliteSystem.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem.getDragCoefficient();

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    Sequence sequence = {
        Array<Segment>::Empty(),
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
        std::make_shared<Thruster>(satelliteSystem, std::make_shared<ConstantThrust>(ConstantThrust::Intrack()))
    );

    const Shared<const CoordinateBroker> coordinatesBrokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
        CoordinateSubset::SurfaceArea(),
        CoordinateSubset::DragCoefficient(),
    }));

    VectorXd coordinates(9);
    coordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, mass + 100.0, surfaceArea, dragCoefficient;
    const State state = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        coordinatesBrokerSPtr,
    };

    const Sequence::Solution solution = sequence.solve(state, defaultRepetitionCount_);

    EXPECT_TRUE(solution.segmentSolutions.getSize() == 2 * defaultRepetitionCount_);

    EXPECT_EQ(solution.accessStartInstant(), Instant::J2000());
    EXPECT_EQ(solution.accessEndInstant(), solution.segmentSolutions.accessLast().states.accessLast().accessInstant());

    const Array<State> solutionStates = solution.getStates();

    for (Index i = 1; i < solutionStates.getSize(); ++i)
    {
        EXPECT_TRUE(solutionStates[i].accessInstant() > solutionStates[i - 1].accessInstant());
    }
    EXPECT_DOUBLE_EQ(solution.getInitialMass().inKilograms(), mass + 100.0);
    EXPECT_DOUBLE_EQ(
        solution.getFinalMass().inKilograms(),
        solution.segmentSolutions.accessLast().states.accessLast().accessCoordinates()[6]
    );
    EXPECT_EQ(solution.getPropagationDuration(), solution.accessEndInstant() - solution.accessStartInstant());

    EXPECT_DOUBLE_EQ(
        solution.computeDeltaMass().inKilograms(),
        solution.getInitialMass().inKilograms() - solution.getFinalMass().inKilograms()
    );
    EXPECT_NEAR(
        solution.computeDeltaV(3000.0),
        3000.0 * EarthGravitationalModel::gravityConstant *
            std::log(solution.getInitialMass().inKilograms() / solution.getFinalMass().inKilograms()),
        1e-3
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_3)
{
    const Size reptitionCount = 3;

    {
        const Shared<AngularCondition> relativeTrueAnomalyCondition =
            std::make_shared<AngularCondition>(COECondition::TrueAnomaly(
                AngularCondition::Criterion::AnyCrossing,
                Frame::GCRF(),
                EventCondition::Target(Angle::Degrees(5.0), EventCondition::Target::Type::Relative),
                EarthGravitationalModel::EGM2008.gravitationalParameter_
            ));

        const Array<Segment> segments = {
            Segment::Coast(
                "Relative True Anomaly", relativeTrueAnomalyCondition, defaultDynamics_, defaultNumericalSolver_
            ),
        };

        Sequence sequence = {
            segments,
            defaultNumericalSolver_,
            defaultDynamics_,
            Duration::Days(1.0),
        };

        const Sequence::Solution solution = sequence.solve(defaultState_, reptitionCount);

        EXPECT_TRUE(solution.executionIsComplete);
        EXPECT_EQ(solution.segmentSolutions.getSize(), reptitionCount);

        COE initialCOE = COE::Cartesian(
            {defaultState_.getPosition(), defaultState_.getVelocity()},
            EarthGravitationalModel::EGM2008.gravitationalParameter_
        );

        for (const Segment::Solution& segmentSolution : solution.segmentSolutions)
        {
            EXPECT_TRUE(segmentSolution.conditionIsSatisfied);
            const COE coe = COE::Cartesian(
                {segmentSolution.states.accessLast().getPosition(), segmentSolution.states.accessLast().getVelocity()},
                EarthGravitationalModel::EGM2008.gravitationalParameter_
            );
            EXPECT_NEAR(coe.getTrueAnomaly().inDegrees() - initialCOE.getTrueAnomaly().inDegrees(), 5.0, 1e-5);
            initialCOE = coe;
        }
    }

    {
        const Shared<RealCondition> durationCondition = std::make_shared<RealCondition>(
            RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Seconds(30.0))
        );

        const Array<Segment> segments = {
            Segment::Coast("Duration", durationCondition, defaultDynamics_, defaultNumericalSolver_),
        };

        Sequence sequence = {
            segments,
            defaultNumericalSolver_,
            defaultDynamics_,
            Duration::Days(1.0),
        };

        const Sequence::Solution solution = sequence.solve(defaultState_, reptitionCount);

        EXPECT_TRUE(solution.executionIsComplete);
        EXPECT_EQ(solution.segmentSolutions.getSize(), reptitionCount);

        Size iter = 1;
        for (const Segment::Solution& segmentSolution : solution.segmentSolutions)
        {
            EXPECT_TRUE(segmentSolution.conditionIsSatisfied);
            EXPECT_NEAR(
                (segmentSolution.accessEndInstant() - defaultState_.accessInstant()).inSeconds(), 30.0 * iter, 1e-6
            );

            ++iter;
        }
    }
}

// Define parameter structure for parametrized Sequence maneuvering constraints tests
struct ManeuveringConstraintsTestParams
{
    String description;
    Array<Tuple<Duration, Duration>> maneuverIntervals;
    Segment::ManeuverConstraints maneuverConstraints;
    Array<Tuple<Duration, Duration, bool>>
        expectedManeuverIntervals;  // bool: true if we should use a "loose" tolerance
};

class OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence_ManeuveringConstraints_Parameterized
    : public ::testing::TestWithParam<ManeuveringConstraintsTestParams>
{
   protected:
    const Shared<Celestial> earthSPtr_ = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> dynamicsSPtr_ = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr_),
    };
    const NumericalSolver numericalSolver_ = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaDopri5, 1.0, 1.0e-12, 1.0e-12
    };
    const SatelliteSystem satelliteSystem_ = SatelliteSystem::Default();
    const Shared<const CoordinateBroker> coordinatesBrokerSPtr_ = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));
};

INSTANTIATE_TEST_SUITE_P(
    ManeuveringConstraintsTests,
    OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence_ManeuveringConstraints_Parameterized,
    ::testing::Values(
        // No Maneuvers
        ManeuveringConstraintsTestParams {
            "NoManeuvers",
            Array<Tuple<Duration, Duration>>::Empty(),
            Segment::ManeuverConstraints(
                Duration::Seconds(30.0),
                Duration::Undefined(),
                Duration::Seconds(30.0),
                Segment::MaximumManeuverDurationViolationStrategy::TruncateEnd
            ),
            Array<Tuple<Duration, Duration, bool>>::Empty()
        },
        // With Minimum Maneuver Duration Constraint
        ManeuveringConstraintsTestParams {
            "MinimumManeuverDuration",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-5.0), Duration::Minutes(7.0)},  // Too short, skipped
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(21.0)},
                Tuple<Duration, Duration> {Duration::Minutes(25.0), Duration::Minutes(30.0)},  // Too short, skipped
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(70.0)},
                Tuple<Duration, Duration> {Duration::Minutes(85.0), Duration::Minutes(110.0)}
            },
            Segment::ManeuverConstraints(
                Duration::Minutes(10.0),
                Duration::Undefined(),
                Duration::Seconds(30.0),
                Segment::MaximumManeuverDurationViolationStrategy::TruncateEnd
            ),
            Array<Tuple<Duration, Duration, bool>> {
                Tuple<Duration, Duration, bool> {Duration::Minutes(10.0), Duration::Minutes(21.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(50.0), Duration::Minutes(70.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(85.0), Duration::Minutes(100.0), false}
            }
        },
        // With Minimum Maneuver Separation Constraint
        ManeuveringConstraintsTestParams {
            "MinimumManeuverSeparation",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-5.0), Duration::Minutes(7.0)},
                Tuple<Duration, Duration> {Duration::Minutes(10.0), Duration::Minutes(12.0)},  // Too soon, skipped
                Tuple<Duration, Duration> {Duration::Minutes(25.0), Duration::Minutes(30.0)},
                Tuple<Duration, Duration> {Duration::Minutes(50.0), Duration::Minutes(70.0)},
                Tuple<Duration, Duration> {Duration::Minutes(75.0), Duration::Minutes(110.0)}  // Too soon, delayed
            },
            Segment::ManeuverConstraints(
                Duration::Seconds(30.0),
                Duration::Undefined(),
                Duration::Minutes(10.0),
                Segment::MaximumManeuverDurationViolationStrategy::TruncateEnd
            ),
            Array<Tuple<Duration, Duration, bool>> {
                Tuple<Duration, Duration, bool> {Duration::Minutes(0.0), Duration::Minutes(7.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(25.0), Duration::Minutes(30.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(50.0), Duration::Minutes(70.0), false},
                Tuple<Duration, Duration, bool> {
                    Duration::Minutes(80.0), Duration::Minutes(100.0), true
                }  // Lose tolerance as the maneuver start will be delayed to satisfy the minimum separation, and it
                   // might start slightly later (due to the numerical step)
            }
        },
        // With Maximum Maneuver Duration Constraint (Skip Strategy)
        ManeuveringConstraintsTestParams {
            "MaximumManeuverDurationSkip",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {Duration::Minutes(-5.0), Duration::Minutes(14.0)},  // Too long, skipped
                Tuple<Duration, Duration> {Duration::Minutes(20.0), Duration::Minutes(25.0)},
                Tuple<Duration, Duration> {Duration::Minutes(30.0), Duration::Minutes(50.0)},  // Too long, skipped
                Tuple<Duration, Duration> {Duration::Minutes(60.0), Duration::Minutes(110.0)}  // Too long, skipped
            },
            Segment::ManeuverConstraints(
                Duration::Seconds(30.0),
                Duration::Minutes(10.0),
                Duration::Seconds(30.0),
                Segment::MaximumManeuverDurationViolationStrategy::Skip
            ),
            Array<Tuple<Duration, Duration, bool>> {
                Tuple<Duration, Duration, bool> {Duration::Minutes(20.0), Duration::Minutes(25.0), false},
            }
        },
        // With Maximum Maneuver Duration Constraint (Slice Strategy)
        ManeuveringConstraintsTestParams {
            "MaximumManeuverDurationTruncateEnd",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {
                    Duration::Minutes(-5.0), Duration::Minutes(14.0)
                },  // Too long, sliced [0, 10] (skiping [13, 14] as it would be too short)
                Tuple<Duration, Duration> {Duration::Minutes(20.0), Duration::Minutes(25.0)},
                Tuple<Duration, Duration> {
                    Duration::Minutes(30.0), Duration::Minutes(50.0)
                },  // Too long, sliced to [30, 40] and [43, 50]
                Tuple<Duration, Duration> {
                    Duration::Minutes(60.0), Duration::Minutes(110.0)
                }  // Too long, sliced to [60, 70], [73, 83], [86, 96] (skiping [99, 100] as it would be too short)
            },
            Segment::ManeuverConstraints(
                Duration::Minutes(4.0),
                Duration::Minutes(10.0),
                Duration::Minutes(3.0),
                Segment::MaximumManeuverDurationViolationStrategy::TruncateEnd
            ),
            Array<Tuple<Duration, Duration, bool>> {
                Tuple<Duration, Duration, bool> {Duration::Minutes(0.0), Duration::Minutes(10.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(20.0), Duration::Minutes(25.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(30.0), Duration::Minutes(40.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(43.0), Duration::Minutes(50.0), true},
                Tuple<Duration, Duration, bool> {Duration::Minutes(60.0), Duration::Minutes(70.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(73.0), Duration::Minutes(83.0), true},
                Tuple<Duration, Duration, bool> {Duration::Minutes(86.0), Duration::Minutes(96.0), true}
            }
        },
        // With Maximum Maneuver Duration Constraint (Center Strategy)
        ManeuveringConstraintsTestParams {
            "MaximumManeuverDurationCenter",
            Array<Tuple<Duration, Duration>> {
                Tuple<Duration, Duration> {
                    Duration::Minutes(-5.0), Duration::Minutes(14.0)
                },  // Too long, centered around 7.0
                Tuple<Duration, Duration> {Duration::Minutes(20.0), Duration::Minutes(25.0)},
                Tuple<Duration, Duration> {
                    Duration::Minutes(30.0), Duration::Minutes(50.0)
                },  // Too long, centered around 40.0
                Tuple<Duration, Duration> {
                    Duration::Minutes(60.0), Duration::Minutes(110.0)
                }  // Too long, centered around 80.0
            },
            Segment::ManeuverConstraints(
                Duration::Seconds(30.0),
                Duration::Minutes(10.0),
                Duration::Minutes(3.0),
                Segment::MaximumManeuverDurationViolationStrategy::Center
            ),
            Array<Tuple<Duration, Duration, bool>> {
                Tuple<Duration, Duration, bool> {Duration::Minutes(2.0), Duration::Minutes(12.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(20.0), Duration::Minutes(25.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(35.0), Duration::Minutes(45.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(48.0), Duration::Minutes(50.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(75.0), Duration::Minutes(85.0), false},
                Tuple<Duration, Duration, bool> {Duration::Minutes(89.0), Duration::Minutes(99.0), false},
            }
        }
    ),
    [](const ::testing::TestParamInfo<ManeuveringConstraintsTestParams>& paramInfo)
    {
        return paramInfo.param.description;
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence_ManeuveringConstraints_Parameterized, ManeuverConstraintScenarios
)
{
    const auto params = GetParam();

    VectorXd coordinates(7);
    coordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;
    const Instant referenceInstant = Instant::DateTime(DateTime(2021, 1, 1, 12, 0, 0), Scale::UTC);
    const State initialState = {
        referenceInstant,
        coordinates,
        Frame::GCRF(),
        coordinatesBrokerSPtr_,
    };

    const Shared<RealCondition> maneuverSegmentConditionSPtr = std::make_shared<RealCondition>(
        RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(100.0))
    );

    // When solving to condition, set a slightly shorter duration than the expected segment termination, so that
    // we can avoid trailing edge artifacts when testing
    const RealCondition sequenceCondition =
        RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(99.0));

    const Duration maximumPropagationDuration = Duration::Minutes(200.0);
    const Duration tolerance = Duration::Milliseconds(10.0);

    Array<Interval> guidanceLawIntervals = Array<Interval>::Empty();
    for (const auto& durationTuple : params.maneuverIntervals)
    {
        guidanceLawIntervals.add(Interval::Closed(
            referenceInstant + std::get<0>(durationTuple), referenceInstant + std::get<1>(durationTuple)
        ));
    }

    const Shared<Thruster> customThrusterDynamicsSPtr =
        std::make_shared<Thruster>(satelliteSystem_, std::make_shared<CustomGuidanceLaw>(guidanceLawIntervals));

    Segment maneuverSegment = Segment::Maneuver(
        "Maneuvering Segment",
        maneuverSegmentConditionSPtr,
        customThrusterDynamicsSPtr,
        dynamicsSPtr_,
        numericalSolver_,
        params.maneuverConstraints
    );

    Sequence sequence = {
        {maneuverSegment},
        numericalSolver_,
        dynamicsSPtr_,
        maximumPropagationDuration,
    };

    // Solve sequence using both methods
    const Sequence::Solution solutionUsingRepetitionCount = sequence.solve(initialState, 1);
    const Sequence::Solution solutionUsingCondition =
        sequence.solveToCondition(initialState, sequenceCondition, maximumPropagationDuration);

    EXPECT_TRUE(solutionUsingRepetitionCount.executionIsComplete);
    EXPECT_TRUE(solutionUsingRepetitionCount.getInterval().getStart().isNear(referenceInstant, tolerance));
    EXPECT_TRUE(solutionUsingRepetitionCount.getInterval().getEnd().isNear(
        referenceInstant + Duration::Minutes(100.0), tolerance
    ));

    // Extract maneuvers from both solutions
    const Array<Maneuver> maneuversUsingRepetitionCount = solutionUsingRepetitionCount.extractManeuvers(Frame::GCRF());
    const Array<Maneuver> maneuversUsingCondition = solutionUsingCondition.extractManeuvers(Frame::GCRF());

    EXPECT_EQ(maneuversUsingRepetitionCount.getSize(), params.expectedManeuverIntervals.getSize());
    EXPECT_EQ(maneuversUsingCondition.getSize(), params.expectedManeuverIntervals.getSize());

    for (Size idx = 0; idx < params.expectedManeuverIntervals.getSize(); ++idx)
    {
        const Instant expectedStart = referenceInstant + std::get<0>(params.expectedManeuverIntervals[idx]);
        const Instant expectedEnd = referenceInstant + std::get<1>(params.expectedManeuverIntervals[idx]);
        const Duration toleranceToUse =
            std::get<2>(params.expectedManeuverIntervals[idx]) ? Duration::Seconds(20.0) : tolerance;

        EXPECT_TRUE(maneuversUsingRepetitionCount[idx].getInterval().getStart().isNear(expectedStart, toleranceToUse));
        EXPECT_TRUE(maneuversUsingRepetitionCount[idx].getInterval().getEnd().isNear(expectedEnd, toleranceToUse));

        EXPECT_TRUE(maneuversUsingCondition[idx].getInterval().getStart().isNear(expectedStart, toleranceToUse));
        EXPECT_TRUE(maneuversUsingCondition[idx].getInterval().getEnd().isNear(expectedEnd, toleranceToUse));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Solve_WithMaximumManeuverDurationConstraintFail)
{
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> dynamicsSPtr = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
    };

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaDopri5, 1.0, 1.0e-12, 1.0e-12
    };

    const SatelliteSystem satelliteSystem = SatelliteSystem::Default();

    const Shared<const CoordinateBroker> coordinatesBrokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));

    VectorXd coordinates(7);
    coordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;
    const State initialState = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        coordinatesBrokerSPtr,
    };

    const Duration maximumPropagationDuration = Duration::Minutes(200.0);
    const Shared<const Frame> frameSPtr = Frame::GCRF();

    Segment::ManeuverConstraints maneuverConstraints;
    maneuverConstraints.maximumDuration = Duration::Minutes(10.0);

    Segment segment = Segment::Maneuver(
        "Segment",
        std::make_shared<RealCondition>(
            RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(15.0))
        ),
        std::make_shared<Thruster>(
            satelliteSystem,
            std::make_shared<CustomGuidanceLaw>(Array<Interval> {
                Interval::Closed(Instant::J2000() + Duration::Minutes(0.0), Instant::J2000() + Duration::Minutes(20.0)),
            })
        ),
        dynamicsSPtr,
        numericalSolver,
        maneuverConstraints
    );

    Sequence sequence = {
        {segment},
        numericalSolver,
        dynamicsSPtr,
        maximumPropagationDuration,
    };

    EXPECT_THROW(
        {
            try
            {
                sequence.solve(initialState, 1);
            }
            catch (const ostk::core::error::RuntimeError& e)
            {
                EXPECT_NE(
                    e.getMessage().find("Maneuver duration exceeds maximum maneuver duration constraint"),
                    std::string::npos
                );
                throw;
            }
        },
        ostk::core::error::RuntimeError
    );
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence,
    Solve_WithMinimumManeuverSeparationConstraintConcatenatingSegments
)
{
    const SatelliteSystem satelliteSystem = SatelliteSystem::Default();

    const Shared<const CoordinateBroker> coordinatesBrokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));

    VectorXd coordinates(7);
    coordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;
    const State initialState = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        coordinatesBrokerSPtr,
    };

    const Duration maximumPropagationDuration = Duration::Minutes(200.0);
    const Duration tolerance = Duration::Milliseconds(10.0);

    // With Minimum Maneuver Separation Constraint between Segments
    {
        Segment::ManeuverConstraints maneuverConstraints;
        maneuverConstraints.minimumSeparation = Duration::Minutes(10.0);

        const Segment maneuverSegment1 = Segment::Maneuver(
            "Maneuvering Segment 1",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(15.0))
            ),
            std::make_shared<Thruster>(
                satelliteSystem,
                std::make_shared<CustomGuidanceLaw>(Array<Interval> {
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(-5.0), Instant::J2000() + Duration::Minutes(12.0)
                    ),
                })
            ),
            defaultDynamics_,
            defaultNumericalSolver_,
            maneuverConstraints
        );
        const Segment maneuverSegment2 = Segment::Maneuver(
            "Maneuvering Segment 2",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(15.0))
            ),
            std::make_shared<Thruster>(
                satelliteSystem,
                std::make_shared<CustomGuidanceLaw>(Array<Interval> {
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(16.0),  // Too soon, completely skipped
                        Instant::J2000() + Duration::Minutes(20.0)
                    ),
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(23.0), Instant::J2000() + Duration::Minutes(28.0)
                    ),
                })
            ),
            defaultDynamics_,
            defaultNumericalSolver_,
            maneuverConstraints
        );

        const Sequence sequence = {
            {
                maneuverSegment1,
                maneuverSegment2,
            },
            defaultNumericalSolver_,
            defaultDynamics_,
            maximumPropagationDuration,
        };

        const Sequence::Solution solutionUsingRepetitionCount = sequence.solve(initialState, 1);
        const Sequence::Solution solutionUsingCondition = sequence.solveToCondition(
            initialState,
            RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(29.0))
        );

        EXPECT_TRUE(solutionUsingRepetitionCount.executionIsComplete);
        EXPECT_TRUE(solutionUsingRepetitionCount.getInterval().getStart().isNear(Instant::J2000(), tolerance));
        EXPECT_TRUE(solutionUsingRepetitionCount.getInterval().getEnd().isNear(
            Instant::J2000() + Duration::Minutes(30.0), tolerance
        ));

        EXPECT_TRUE(solutionUsingCondition.executionIsComplete);
        EXPECT_TRUE(solutionUsingCondition.getInterval().getStart().isNear(Instant::J2000(), tolerance));
        EXPECT_TRUE(
            solutionUsingCondition.getInterval().getEnd().isNear(Instant::J2000() + Duration::Minutes(30.0), tolerance)
        );

        const Array<Maneuver> maneuversUsingRepetitionCount =
            solutionUsingRepetitionCount.extractManeuvers(Frame::GCRF());
        const Array<Maneuver> maneuversUsingCondition = solutionUsingCondition.extractManeuvers(Frame::GCRF());

        EXPECT_EQ(maneuversUsingRepetitionCount.getSize(), 2);
        EXPECT_EQ(maneuversUsingCondition.getSize(), 2);

        EXPECT_TRUE(maneuversUsingRepetitionCount[0].getInterval().getStart().isNear(
            Instant::J2000() + Duration::Minutes(0.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingRepetitionCount[0].getInterval().getEnd().isNear(
            Instant::J2000() + Duration::Minutes(12.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingRepetitionCount[1].getInterval().getStart().isNear(
            Instant::J2000() + Duration::Minutes(23.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingRepetitionCount[1].getInterval().getEnd().isNear(
            Instant::J2000() + Duration::Minutes(28.0), tolerance
        ));

        EXPECT_TRUE(maneuversUsingCondition[0].getInterval().getStart().isNear(
            Instant::J2000() + Duration::Minutes(0.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingCondition[0].getInterval().getEnd().isNear(
            Instant::J2000() + Duration::Minutes(12.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingCondition[1].getInterval().getStart().isNear(
            Instant::J2000() + Duration::Minutes(23.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingCondition[1].getInterval().getEnd().isNear(
            Instant::J2000() + Duration::Minutes(28.0), tolerance
        ));
    }

    // With Minimum Maneuver Separation Constraint between repetitions
    {
        Segment::ManeuverConstraints maneuverConstraints;
        maneuverConstraints.minimumSeparation = Duration::Minutes(10.0);

        const Segment maneuverSegment = Segment::Maneuver(
            "Maneuvering Segment 1",
            std::make_shared<RealCondition>(
                RealCondition::DurationCondition(RealCondition::Criterion::StrictlyPositive, Duration::Minutes(15.0))
            ),
            std::make_shared<Thruster>(
                satelliteSystem,
                std::make_shared<CustomGuidanceLaw>(Array<Interval> {
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(-5.0), Instant::J2000() + Duration::Minutes(12.0)
                    ),
                    // First repetition will end here (at 15.0 minutes)
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(16.0),  // Too soon, completely skipped
                        Instant::J2000() + Duration::Minutes(20.0)
                    ),
                    Interval::Closed(
                        Instant::J2000() + Duration::Minutes(23.0), Instant::J2000() + Duration::Minutes(28.0)
                    ),
                })
            ),
            defaultDynamics_,
            defaultNumericalSolver_,
            maneuverConstraints
        );
        const Sequence sequence = {
            {maneuverSegment},
            defaultNumericalSolver_,
            defaultDynamics_,
            maximumPropagationDuration,
        };
        const Sequence::Solution solutionUsingRepetitionCount = sequence.solve(initialState, 2);
        EXPECT_TRUE(solutionUsingRepetitionCount.executionIsComplete);
        EXPECT_TRUE(solutionUsingRepetitionCount.getInterval().getStart().isNear(Instant::J2000(), tolerance));
        EXPECT_TRUE(solutionUsingRepetitionCount.getInterval().getEnd().isNear(
            Instant::J2000() + Duration::Minutes(30.0), tolerance
        ));
        Array<Maneuver> maneuversUsingRepetitionCount = solutionUsingRepetitionCount.extractManeuvers(Frame::GCRF());
        EXPECT_EQ(maneuversUsingRepetitionCount.getSize(), 2);
        EXPECT_TRUE(maneuversUsingRepetitionCount[0].getInterval().getStart().isNear(
            Instant::J2000() + Duration::Minutes(0.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingRepetitionCount[0].getInterval().getEnd().isNear(
            Instant::J2000() + Duration::Minutes(12.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingRepetitionCount[1].getInterval().getStart().isNear(
            Instant::J2000() + Duration::Minutes(23.0), tolerance
        ));
        EXPECT_TRUE(maneuversUsingRepetitionCount[1].getInterval().getEnd().isNear(
            Instant::J2000() + Duration::Minutes(28.0), tolerance
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultSequence_.print(std::cout, true));
        EXPECT_NO_THROW(defaultSequence_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        const PropulsionSystem propulsionSystem = PropulsionSystem(1.0, 1500.0);

        const SatelliteSystem satelliteSystem = {
            Mass::Kilograms(100.0),
            satelliteGeometry,
            Matrix3d::Identity(),
            500.0,
            2.1,
            propulsionSystem,
        };

        Sequence sequence = {
            Array<Segment>::Empty(),
            defaultNumericalSolver_,
            defaultDynamics_,
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
            std::make_shared<Thruster>(satelliteSystem, std::make_shared<ConstantThrust>(ConstantThrust::Intrack()))
        );

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(sequence.print(std::cout, true));
        EXPECT_NO_THROW(sequence.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}
