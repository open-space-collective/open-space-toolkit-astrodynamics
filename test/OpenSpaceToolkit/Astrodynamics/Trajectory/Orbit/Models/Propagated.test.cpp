/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::fs::File;
using ostk::core::fs::Path;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Size;

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::env::Object;
using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::Dynamics;
using ostk::astro::dynamics::PositionDerivative;
using ostk::astro::dynamics::CentralBodyGravity;
using ostk::astro::trajectory::Orbit;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::orbit::models::Propagated;
using ostk::astro::trajectory::state::NumericalSolver;

/* UNIT TESTS */
class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        this->earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
        this->defaultDynamics_ = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSpherical_),
        };

        this->defaultPosition_ = Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_);
        this->defaultVelocity_ = Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_);

        this->defaultState_ = {this->defaultInstant_, this->defaultPosition_, this->defaultVelocity_};

        this->propagator_ = Propagator(defaultNumericalSolver_, defaultDynamics_);
    }

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();
    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15
    };

    const Instant defaultInstant_ = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);

    Position defaultPosition_ = Position::Undefined();
    Velocity defaultVelocity_ = Velocity::Undefined();
    State defaultState_ = State::Undefined();

    Shared<Celestial> earthSpherical_ = nullptr;
    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();

    Propagator propagator_ = Propagator::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Constructor)
{
    // State array setup
    Array<State> stateArray = Array<State>::Empty();
    stateArray.add(defaultState_);
    const Instant instant_1 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
    const State state_1 = {
        instant_1,
        Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr_),
    };
    stateArray.add(state_1);

    // Constructor without putting into orbit object
    {
        // Setup Propagated model
        EXPECT_NO_THROW(Propagated(propagator_, defaultState_));
        EXPECT_NO_THROW(Propagated(propagator_, stateArray));
    }

    // Constructor with putting into orbit object
    {
        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, defaultState_};
        const Propagated propagatedModel_0 = {propagator_, stateArray};

        // Setup Orbit model
        EXPECT_NO_THROW(Orbit(propagatedModel, earthSpherical_));
        EXPECT_NO_THROW(Orbit(propagatedModel_0, earthSpherical_));
    }

    // Constructor with empty cachedStateArray
    {
        EXPECT_THROW(
            {
                try
                {
                    const Propagated propagatedModel = Propagated(propagator_, Array<State>::Empty());
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(e.getMessage(), String("{Propagated} is undefined."));
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, EqualToOperator)
{
    {
        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, defaultState_};
        const Propagated propagatedModel_x = {propagatedModel};
        EXPECT_TRUE(propagatedModel == propagatedModel_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15
        };
        const Propagated propagatedModel_1 = {{numericalSolver_1, defaultDynamics_}, defaultState_};
        EXPECT_FALSE(propagatedModel == propagatedModel_1);

        // State array setup, state Array construction with array of length one and same start
        Array<State> stateArray = Array<State>::Empty();
        stateArray.add(defaultState_);
        const Propagated propagatedModel_2 = {propagator_, stateArray};
        EXPECT_TRUE(propagatedModel == propagatedModel_2);

        // State array setup, state Array construction with array of length two and a different second state
        const Instant instant_0 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
        const State state_0 = {
            instant_0,
            Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr_),
        };
        stateArray.add(state_0);
        const Propagated propagatedModel_3 = {propagator_, stateArray};
        EXPECT_FALSE(propagatedModel == propagatedModel_3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, NotEqualToOperator)
{
    {
        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, defaultState_};
        const Propagated propagatedModel_x = {propagatedModel};
        EXPECT_FALSE(propagatedModel != propagatedModel_x);

        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15
        };
        const Propagated propagatedModel_1 = {{numericalSolver_1, defaultDynamics_}, defaultState_};
        EXPECT_TRUE(propagatedModel != propagatedModel_1);

        // State array setup, state Array construction with array of length one and same start
        Array<State> stateArray = Array<State>::Empty();
        stateArray.add(defaultState_);
        const Propagated propagatedModel_2 = {propagator_, stateArray};
        EXPECT_FALSE(propagatedModel != propagatedModel_2);

        // State array setup, state Array construction with array of length two and a different second state
        const Instant instant_0 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
        const State state_0 = {
            instant_0,
            Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr_),
        };
        stateArray.add(state_0);
        const Propagated propagatedModel_3 = {propagator_, stateArray};
        EXPECT_TRUE(propagatedModel != propagatedModel_3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, IsDefined)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        EXPECT_TRUE(propagatedModel.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, StreamOperator)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << propagatedModel << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, Print)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(propagatedModel.print(std::cout, true));
        EXPECT_NO_THROW(propagatedModel.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getEpoch)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        EXPECT_EQ(propagatedModel.getEpoch(), defaultInstant_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, getRevolutionNumberAtEpoch)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        EXPECT_EQ(propagatedModel.getRevolutionNumberAtEpoch(), 1);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateStateAt)
{
    // Setup Propagated model and orbit
    const Propagated propagatedModel = {propagator_, defaultState_};

    const Orbit orbit = {propagatedModel, earthSpherical_};

    // Setup instants
    const Array<Instant> instantArray = {
        Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC),
        defaultInstant_,
        Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC),
        Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC),
    };

    // Test CalculateStateAt in propagatedModel and in orbit
    {
        for (const Instant& instant : instantArray)
        {
            EXPECT_EQ(orbit.getStateAt(instant), propagatedModel.calculateStateAt(instant));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateStatesAt)
{
    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                               "Propagated/CalculateStatesAt_StateValidation.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC));

        referencePositionArray.add({
            referenceRow[1].accessReal(),
            referenceRow[2].accessReal(),
            referenceRow[3].accessReal(),
        });
        referenceVelocityArray.add({
            referenceRow[4].accessReal(),
            referenceRow[5].accessReal(),
            referenceRow[6].accessReal(),
        });
    }

    const auto getState = [&instantArray, &referencePositionArray, &referenceVelocityArray, this](const Size& index
                          ) -> State
    {
        const Instant startInstant = instantArray[index];
        return {
            startInstant,
            Position::Meters({referencePositionArray[index]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArray[index]}, gcrfSPtr_),
        };
    };

    const auto validatePropagatedStates = [&instantArray, &referencePositionArray, &referenceVelocityArray, this](
                                              const Array<State>& cachedStateArray
                                          ) -> void
    {
        // Setup Propagated model and perform propagation
        const Propagated propagatedModel = {propagator_, cachedStateArray};

        const Array<State> propagatedStateArray = propagatedModel.calculateStatesAt(instantArray);

        // Validation loop
        for (Size i = 0; i < propagatedStateArray.getSize(); i++)
        {
            const Position position = propagatedStateArray[i].getPosition();
            const Velocity velocity = propagatedStateArray[i].getVelocity();

            const double positionError = (position.accessCoordinates() - referencePositionArray[i]).norm();
            const double velocityError = (velocity.accessCoordinates() - referenceVelocityArray[i]).norm();

            ASSERT_GT(2e-7, positionError);
            ASSERT_GT(2e-10, velocityError);
            ASSERT_EQ(propagatedStateArray[i].getInstant(), instantArray[i]);

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionError << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityError <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    };

    // Test cachedStateArray with one cachedState at the beginning of reference states
    validatePropagatedStates({
        getState(0),
    });

    // Test cachedStateArray with one cachedState at the middle of reference states
    validatePropagatedStates({
        getState(30),
    });

    // Test cachedStateArray with one cachedState at the end of reference states
    validatePropagatedStates({
        getState(60),
    });

    // Test cachedStateArray with two cachedStates, one at the beginning and one at the end of reference states
    validatePropagatedStates({
        getState(0),
        getState(60),
    });

    // Test cachedStateArray with half of reference states already in cachedStateArray
    {
        // Choose states to put into cachedStateArray
        Array<State> cachedStateArray = Array<State>::Empty();
        for (Size i = 0; i < instantArray.getSize(); i++)
        {
            if (i % 2 == 0)
            {
                cachedStateArray.add(getState(i));
            }
        }

        validatePropagatedStates(cachedStateArray);
    }

    // Test cachedStateArray with all reference states already in cachedStateArray (no propagation should be needed)
    {
        // Choose states to put into cachedStateArray
        Array<State> cachedStateArray = Array<State>::Empty();
        for (Size i = 0; i < instantArray.getSize(); i++)
        {
            cachedStateArray.add(getState(i));
        }

        validatePropagatedStates(cachedStateArray);
    }

    // Test cachedStateArray with two duplicate States
    {
        const Array<State> cachedStateArray = {
            getState(0),
            getState(0),
        };
        validatePropagatedStates(cachedStateArray);
    }

    // Test cachedStateArray with two cachedStates, unsorted

    {
        const Array<State> cachedStateArray = {
            getState(60),
            getState(0),
        };
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[0].getInstant());

        validatePropagatedStates(cachedStateArray);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, CalculateRevolutionNumberAt)
{
    // Satellite system setup

    // Test basic positive and negative revolution numbers
    {
        // Setup Propagated model and orbit
        const Propagated propagatedModel = {propagator_, defaultState_};

        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Calculate revolution number at instant
        const Instant instant = defaultInstant_;
        const Instant instant_before1 = Instant::DateTime(DateTime(2018, 1, 1, 22, 0, 0), Scale::UTC);
        const Instant instant_after1 = Instant::DateTime(DateTime(2018, 1, 2, 1, 0, 0), Scale::UTC);
        const Instant instant_after2 = Instant::DateTime(DateTime(2018, 1, 2, 2, 0, 0), Scale::UTC);

        // Check revolution numbers for propagated model
        EXPECT_EQ(1, propagatedModel.calculateRevolutionNumberAt(instant));
        EXPECT_EQ(-2, propagatedModel.calculateRevolutionNumberAt(instant_before1));
        EXPECT_EQ(1, propagatedModel.calculateRevolutionNumberAt(instant_after1));
        EXPECT_EQ(2, propagatedModel.calculateRevolutionNumberAt(instant_after2));

        // Check revolution numbers for orbit
        EXPECT_EQ(1, orbit.getRevolutionNumberAt(instant));
        EXPECT_EQ(-2, orbit.getRevolutionNumberAt(instant_before1));
        EXPECT_EQ(1, orbit.getRevolutionNumberAt(instant_after1));
        EXPECT_EQ(2, orbit.getRevolutionNumberAt(instant_after2));
    }

    // Test accuracy of calculateRevolutionNumber at by checking that it returns results accurate to the force model
    // it is using
    {
        // Setup dynamics
        const Shared<Celestial> earthEGM2008_70 = std::make_shared<Celestial>(Earth::EGM2008(70, 70));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthEGM2008_70),
        };

        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime(2021, 1, 2, 0, 0, 0), Scale::UTC);

        const State state = {startInstant, defaultPosition_, defaultVelocity_};

        // Setup Propagated model and orbit
        const Propagated propagatedModel_twobody = {propagator_, state};
        const Propagated propagatedModel_fullgrav = {{defaultNumericalSolver_, dynamics}, state};

        // Calculate gravitational parameter
        const Derived gravitationalParameter = EarthGravitationalModel::Spherical.gravitationalParameter_;
        const Derived::Unit GravitationalParameterSIUnit =
            Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);
        const Real gravitationalParameter_SI = gravitationalParameter.in(GravitationalParameterSIUnit);

        // Get position and velocity coordinates from starting state
        Position currentPosition = state.getPosition();
        Velocity currentVelocity = state.getVelocity();
        Vector3d currentPositionCoordinates = currentPosition.inUnit(Position::Unit::Meter).accessCoordinates();
        Vector3d currentVelocityCoordinates =
            currentVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

        // Calculate orbital period
        const double semiMajorAxis =
            -gravitationalParameter_SI * currentPositionCoordinates.norm() /
            (currentPositionCoordinates.norm() * std::pow(currentVelocityCoordinates.norm(), 2) -
             2.0 * gravitationalParameter_SI);
        const Duration currentOrbitalPeriod =
            Duration::Seconds(Real::TwoPi() * std::sqrt(std::pow(semiMajorAxis, 3) / gravitationalParameter_SI));

        // Calculate revolution number at instant
        const int propagationRevolutions = 5;
        const Instant endInstant = startInstant - currentOrbitalPeriod * propagationRevolutions;

        const Integer revolutionNumber_twobody = propagatedModel_twobody.calculateRevolutionNumberAt(endInstant);
        const Integer revolutionNumber_fullgrav = propagatedModel_fullgrav.calculateRevolutionNumberAt(endInstant);

        // Check that revolution numbers for two body are exact, for full grav are 1 higher due to losses (which are
        // gains in negative time), and that they are not equal
        EXPECT_EQ(-propagationRevolutions - 1, revolutionNumber_twobody);
        EXPECT_EQ(-propagationRevolutions, revolutionNumber_fullgrav);

        EXPECT_FALSE(revolutionNumber_twobody == revolutionNumber_fullgrav);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, AccessCachedStateArray)
{
    // Test cachedStateArray sorting during construction and accessing (not during propagation)
    {
        // Test valid and chronological state array input

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_
        };
        stateArray.add(state_1);
        const State state_2 = {defaultInstant_, defaultPosition_, defaultVelocity_};
        stateArray.add(state_2);
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_
        };
        stateArray.add(state_3);

        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, stateArray};

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }

    {
        // Test sorting of non chronological but valid state array

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_
        };
        stateArray.add(state_3);
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_
        };
        stateArray.add(state_1);
        const State state_2 = {defaultInstant_, defaultPosition_, defaultVelocity_};
        stateArray.add(state_2);

        // Setup Propagated model
        const Propagated propagatedModel = {propagator_, stateArray};

        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, SetCachedStateArray)
{
    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime(2021, 1, 2, 0, 0, 0), Scale::UTC);
    const State state = {startInstant, defaultPosition_, defaultVelocity_};

    // Propagated model setup
    Propagated propagatedModel = {propagator_, state};

    // Test cachedStateArray sorting during construction and accessing
    {
        // Test valid and chronological state array input

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_
        };
        stateArray.add(state_1);
        const State state_2 = {defaultInstant_, defaultPosition_, defaultVelocity_};
        stateArray.add(state_2);
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_
        };
        stateArray.add(state_3);

        // Manually set cachedStateArray
        propagatedModel.setCachedStateArray(stateArray);

        // Check propagatedModel is properly defined
        EXPECT_TRUE(propagatedModel.isDefined());

        // Access cachedStateArray and verify
        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }

    {
        // Test sorting of non chronological + duplicate but valid state array

        // Current state and instant setup
        Array<State> stateArray = Array<State>::Empty();
        const State state_3 = {
            Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_
        };
        stateArray.add(state_3);
        const State state_1 = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), defaultPosition_, defaultVelocity_
        };
        stateArray.add(state_1);
        const State state_2 = {defaultInstant_, defaultPosition_, defaultVelocity_};
        stateArray.add(state_2);
        stateArray.add(state_2);

        // Manually set cachedStateArray
        propagatedModel.setCachedStateArray(stateArray);

        // Check propagatedModel is properly defined
        EXPECT_TRUE(propagatedModel.isDefined());

        // Access cachedStateArray and verify
        const Array<State> cachedStateArray = propagatedModel.accessCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }

    {
        // Test setting empty cachedStateArray

        EXPECT_THROW(
            {
                try
                {
                    propagatedModel.setCachedStateArray(Array<State>::Empty());
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(e.getMessage(), String("{Propagated} is undefined."));
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated, PropagationAtExactInstants)
{
    {
        Array<State> states = {
            {
                Instant::DateTime(DateTime::Parse("2023-01-30T18:30:00.184", DateTime::Format::ISO8601), Scale::UTC),
                Position::Meters({-5981016.371280898340, 2114677.607544674072, 2683244.080375305377}, gcrfSPtr_),
                Velocity::MetersPerSecond({3142.190382984703, 50.122115740761, 6934.877387038766}, gcrfSPtr_),
            },
            {
                Instant::DateTime(DateTime::Parse("2023-01-30T18:35:00.184", DateTime::Format::ISO8601), Scale::UTC),
                Position::Meters({-4730169.446157290600, 2014399.438757266616, 4579373.510992174037}, gcrfSPtr_),
                Velocity::MetersPerSecond({5120.144014790862, -712.422950530929, 5589.998515385998}, gcrfSPtr_),
            },
            {
                Instant::DateTime(DateTime::Parse("2023-01-30T18:40:00.184", DateTime::Format::ISO8601), Scale::UTC),
                Position::Meters({-2965004.866673604585, 1695085.379882899811, 5976267.045515080914}, gcrfSPtr_),
                Velocity::MetersPerSecond({6539.557709872774, -1396.675776577052, 3637.641862957897}, gcrfSPtr_),
            }
        };

        Propagated propagatedModel(propagator_, states);

        State estimatedState = propagatedModel.calculateStateAt(states[1].getInstant());

        ASSERT_EQ(estimatedState, states[1]);
    }
}
