/// Apache License 2.0

#include <cmath>
#include <numeric>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::Propagated;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated : public ::testing::Test
{
   protected:
    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();
    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15
    };

    const Instant defaultInstant_ = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);

    const int defaultRevolutionNumber_ = 5;

    Position defaultPosition_ = Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_);
    Velocity defaultVelocity_ = Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_);
    State defaultState_ = {defaultInstant_, defaultPosition_, defaultVelocity_};

    Shared<Celestial> earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
    Array<Shared<Dynamics>> defaultDynamics_ = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSpherical_),
    };

    Propagator propagator_ = Propagator(defaultNumericalSolver_, defaultDynamics_);

    // Maneuver setup: the maneuver spans [J2000, J2000 + 60 s], the state with mass is 60 s before it starts
    const Instant maneuverEpoch_ = Instant::J2000();

    State stateWithMassGenerator(const Instant& anInstant) const
    {
        VectorXd coordinates(7);
        coordinates << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, 100.0;

        return {
            anInstant,
            coordinates,
            gcrfSPtr_,
            {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()}
        };
    }

    State maneuverStateGenerator(const Instant& anInstant, const Vector3d& anAcceleration, double aMassFlowRate) const
    {
        VectorXd coordinates(10);
        coordinates << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, anAcceleration(0), anAcceleration(1), anAcceleration(2),
            aMassFlowRate;

        return {anInstant, coordinates, gcrfSPtr_, Maneuver::RequiredCoordinateSubsets};
    }

    Maneuver maneuverGenerator(const Instant& aStartInstant, const Duration& aDuration) const
    {
        return {
            {
                maneuverStateGenerator(aStartInstant, {1e-3, 0.0, 0.0}, -1.1e-5),
                maneuverStateGenerator(aStartInstant + aDuration, {0.0, 1e-3, 0.0}, -0.9e-5),
            },
            Interpolator::Type::Linear
        };
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, Constructor)
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
        EXPECT_NO_THROW(Propagated(propagator_, defaultState_, defaultRevolutionNumber_));
        EXPECT_NO_THROW(Propagated(propagator_, stateArray, defaultRevolutionNumber_));
    }

    // Constructor with an empty cached state array
    {
        EXPECT_THROW(Propagated(propagator_, Array<State>::Empty()), ostk::core::error::RuntimeError);
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
                    EXPECT_EQ(e.getMessage(), String("Cached state array is empty."));
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, EqualToOperator)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, NotEqualToOperator)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, StreamOperator)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << propagatedModel << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, Print)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(propagatedModel.print(std::cout, true));
        EXPECT_NO_THROW(propagatedModel.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, getEpoch)
{
    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        EXPECT_EQ(propagatedModel.getEpoch(), defaultInstant_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, getRevolutionNumberAtEpoch)
{
    {
        const Propagated propagatedModel = {
            propagator_,
            defaultState_,
            defaultRevolutionNumber_,
        };

        EXPECT_EQ(propagatedModel.getRevolutionNumberAtEpoch(), defaultRevolutionNumber_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, CalculateStateAt)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, CalculateStatesAt)
{
    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/"
                               "Propagated/CalculateStatesAt_StateValidation.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArray = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArray = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].getString()), Scale::UTC));

        referencePositionArray.add({
            referenceRow[1].getReal(),
            referenceRow[2].getReal(),
            referenceRow[3].getReal(),
        });
        referenceVelocityArray.add({
            referenceRow[4].getReal(),
            referenceRow[5].getReal(),
            referenceRow[6].getReal(),
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

            const double positionError = (position.getCoordinates() - referencePositionArray[i]).norm();
            const double velocityError = (velocity.getCoordinates() - referenceVelocityArray[i]).norm();

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, CalculateRevolutionNumberAt)
{
    // Test propagator without central body gravity
    {
        const Propagator propagatorWithoutCentralBodyGravity = {
            defaultNumericalSolver_, {std::make_shared<PositionDerivative>()}
        };

        const Propagated propagatedModel = {
            propagatorWithoutCentralBodyGravity,
            defaultState_,
            defaultRevolutionNumber_,
        };

        EXPECT_THROW(
            propagatedModel.calculateRevolutionNumberAt(defaultInstant_ + Duration::Hours(1.0)),
            ostk::core::error::runtime::Undefined
        );
    }

    // Test simple positive and negative revolution numbers for non-edge case initial state (far away from the ascending
    // node)
    {
        const State initialStateAtPole = {
            defaultInstant_,
            Position::Meters({0.0, 0.0, 7000000.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({5335.865450622126, 5335.865450622126, 0.0}, gcrfSPtr_)
        };

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {
            propagator_,
            initialStateAtPole,
            defaultRevolutionNumber_,
        };

        const COE coe = COE::Cartesian(
            {initialStateAtPole.getPosition(), initialStateAtPole.getVelocity()},
            EarthGravitationalModel::Spherical.gravitationalParameter_
        );  // (Spherical earth has the most modern value which is the correct one)
        const Duration orbitalPeriod = coe.getOrbitalPeriod(EarthGravitationalModel::Spherical.gravitationalParameter_);

        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Check revolution numbers for propagated model
        EXPECT_EQ(defaultRevolutionNumber_, propagatedModel.calculateRevolutionNumberAt(defaultInstant_));
        EXPECT_EQ(
            defaultRevolutionNumber_ + 1, propagatedModel.calculateRevolutionNumberAt(defaultInstant_ + orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber_ - 1, propagatedModel.calculateRevolutionNumberAt(defaultInstant_ - orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber_ + 2,
            propagatedModel.calculateRevolutionNumberAt(defaultInstant_ + 2 * orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber_ - 2,
            propagatedModel.calculateRevolutionNumberAt(defaultInstant_ - 2 * orbitalPeriod)
        );

        // Check revolution numbers for orbit
        EXPECT_EQ(defaultRevolutionNumber_, orbit.getRevolutionNumberAt(defaultInstant_));
        EXPECT_EQ(defaultRevolutionNumber_ - 1, orbit.getRevolutionNumberAt(defaultInstant_ - orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber_ + 1, orbit.getRevolutionNumberAt(defaultInstant_ + orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber_ + 2, orbit.getRevolutionNumberAt(defaultInstant_ + 2 * orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber_ - 2, orbit.getRevolutionNumberAt(defaultInstant_ - 2 * orbitalPeriod));
    }

    // Test simple positive and negative revolution numbers for edge case initial state (at the ascending node)
    {
        const State initialStateAtAscendingNode = {
            defaultInstant_,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_)
        };

        // Setup Propagated model and orbit
        const Propagated propagatedModel = {
            propagator_,
            initialStateAtAscendingNode,
            defaultRevolutionNumber_,
        };

        const COE coe = COE::Cartesian(
            {initialStateAtAscendingNode.getPosition(), initialStateAtAscendingNode.getVelocity()},
            EarthGravitationalModel::Spherical.gravitationalParameter_
        );  // (Spherical earth has the most modern value which is the correct one)
        const Duration orbitalPeriod = coe.getOrbitalPeriod(EarthGravitationalModel::Spherical.gravitationalParameter_);

        const Orbit orbit = {propagatedModel, earthSpherical_};

        // Check revolution numbers for propagated model
        EXPECT_EQ(defaultRevolutionNumber_, propagatedModel.calculateRevolutionNumberAt(defaultInstant_));
        EXPECT_EQ(
            defaultRevolutionNumber_ + 1, propagatedModel.calculateRevolutionNumberAt(defaultInstant_ + orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber_ - 1, propagatedModel.calculateRevolutionNumberAt(defaultInstant_ - orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber_ + 2,
            propagatedModel.calculateRevolutionNumberAt(defaultInstant_ + 2 * orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber_ - 2,
            propagatedModel.calculateRevolutionNumberAt(defaultInstant_ - 2 * orbitalPeriod)
        );

        // Check revolution numbers for orbit
        EXPECT_EQ(defaultRevolutionNumber_, orbit.getRevolutionNumberAt(defaultInstant_));
        EXPECT_EQ(defaultRevolutionNumber_ - 1, orbit.getRevolutionNumberAt(defaultInstant_ - orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber_ + 1, orbit.getRevolutionNumberAt(defaultInstant_ + orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber_ + 2, orbit.getRevolutionNumberAt(defaultInstant_ + 2 * orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber_ - 2, orbit.getRevolutionNumberAt(defaultInstant_ - 2 * orbitalPeriod));
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
        const int initialRevolutionNumber = 0;
        const Propagated propagatedModel_twobody = {propagator_, state, initialRevolutionNumber};
        const Propagated propagatedModel_fullgrav = {
            {defaultNumericalSolver_, dynamics}, state, initialRevolutionNumber
        };

        // Calculate gravitational parameter
        const Derived gravitationalParameter = EarthGravitationalModel::Spherical.gravitationalParameter_;
        const Derived::Unit GravitationalParameterSIUnit =
            Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);
        const double gravitationalParameter_SI = gravitationalParameter.in(GravitationalParameterSIUnit);

        // Get position and velocity coordinates from starting state
        Position currentPosition = state.getPosition();
        Velocity currentVelocity = state.getVelocity();
        Vector3d currentPositionCoordinates = currentPosition.inUnit(Position::Unit::Meter).getCoordinates();
        Vector3d currentVelocityCoordinates = currentVelocity.inUnit(Velocity::Unit::MeterPerSecond).getCoordinates();

        // Calculate orbital period
        const double semiMajorAxis =
            -gravitationalParameter_SI * currentPositionCoordinates.norm() /
            (currentPositionCoordinates.norm() * std::pow(currentVelocityCoordinates.norm(), 2) -
             2.0 * gravitationalParameter_SI);
        const Duration currentOrbitalPeriod =
            Duration::Seconds(2.0 * M_PI * std::sqrt(std::pow(semiMajorAxis, 3) / gravitationalParameter_SI));

        // Calculate revolution number at instant
        const int propagationRevolutions = 5;
        const Instant endInstant = startInstant - currentOrbitalPeriod * propagationRevolutions;

        const int revolutionNumber_twobody = propagatedModel_twobody.calculateRevolutionNumberAt(endInstant);
        const int revolutionNumber_fullgrav = propagatedModel_fullgrav.calculateRevolutionNumberAt(endInstant);

        EXPECT_EQ(initialRevolutionNumber - propagationRevolutions, revolutionNumber_twobody);
        EXPECT_EQ(initialRevolutionNumber - propagationRevolutions, revolutionNumber_fullgrav);
        EXPECT_EQ(revolutionNumber_twobody, revolutionNumber_fullgrav);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, GetCachedStateArray)
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

        const Array<State> cachedStateArray = propagatedModel.getCachedStateArray();

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

        const Array<State> cachedStateArray = propagatedModel.getCachedStateArray();

        // Check array chronological order
        EXPECT_LT(cachedStateArray[0].getInstant(), cachedStateArray[1].getInstant());
        EXPECT_LT(cachedStateArray[1].getInstant(), cachedStateArray[2].getInstant());
        EXPECT_EQ(cachedStateArray.getSize(), 3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, SetCachedStateArray)
{
    // Empty cached state array is rejected
    {
        Propagated propagatedModel = {propagator_, defaultState_};

        EXPECT_THROW(propagatedModel.setCachedStateArray(Array<State>::Empty()), ostk::core::error::RuntimeError);
    }

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

        // Access cachedStateArray and verify
        const Array<State> cachedStateArray = propagatedModel.getCachedStateArray();

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

        // Access cachedStateArray and verify
        const Array<State> cachedStateArray = propagatedModel.getCachedStateArray();

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
                    EXPECT_EQ(e.getMessage(), String("Cached state array is empty."));
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, PropagationAtExactInstants)
{
    {
        const Array<State> states = {
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

        const Propagated propagatedModel(propagator_, states);

        const State estimatedState = propagatedModel.calculateStateAt(states[1].getInstant());

        ASSERT_EQ(estimatedState, states[1]);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, ToTabulated)
{
    using ostk::astrodynamics::trajectory::orbit::model::Tabulated;

    {
        const Propagated propagatedModel = {propagator_, defaultState_};

        const Tabulated tabulatedModel = propagatedModel.toTabulated(
            {defaultInstant_,
             defaultInstant_ + Duration::Minutes(1.0),
             defaultInstant_ + Duration::Minutes(2.0),
             defaultInstant_ + Duration::Minutes(3.0)}
        );

        ASSERT_EQ(
            tabulatedModel.getInterval(), Interval::Closed(defaultInstant_, defaultInstant_ + Duration::Minutes(3.0))
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Propagated, Maneuvers)
{
    const State initialState = stateWithMassGenerator(maneuverEpoch_ - Duration::Seconds(60.0));
    const Maneuver maneuver = maneuverGenerator(maneuverEpoch_, Duration::Seconds(60.0));

    // The model stores the maneuvers, and they are part of its identity
    {
        const Propagated propagatedWithoutManeuvers = {propagator_, initialState};
        const Propagated propagatedWithManeuvers = {propagator_, initialState, defaultRevolutionNumber_, {maneuver}};

        EXPECT_TRUE(propagatedWithoutManeuvers.getManeuvers().isEmpty());
        ASSERT_EQ(1, propagatedWithManeuvers.getManeuvers().getSize());
        EXPECT_EQ(maneuver, propagatedWithManeuvers.getManeuvers()[0]);

        const Propagated sameManeuversPropagated = {propagator_, initialState, 1, {maneuver}};
        const Propagated otherManeuversPropagated = {
            propagator_,
            initialState,
            1,
            {maneuverGenerator(maneuverEpoch_ + Duration::Seconds(10.0), Duration::Seconds(60.0))}
        };
        EXPECT_TRUE(sameManeuversPropagated == Propagated(propagator_, initialState, 1, {maneuver}));
        EXPECT_FALSE(sameManeuversPropagated == otherManeuversPropagated);
        EXPECT_FALSE(sameManeuversPropagated == Propagated(propagator_, initialState));
        EXPECT_TRUE(sameManeuversPropagated != Propagated(propagator_, initialState));

        // Same through the base model interface
        const Orbit orbit = {propagatedWithManeuvers, earthSpherical_};
        EXPECT_EQ(1, orbit.getModel().as<Propagated>().getManeuvers().getSize());
    }

    // A single cached state: the model gives what the propagator gives with the same maneuvers
    {
        const Propagated propagated = {propagator_, initialState, 1, {maneuver}};

        const Array<Instant> instants = {
            maneuverEpoch_ - Duration::Seconds(90.0),
            maneuverEpoch_ - Duration::Seconds(60.0),
            maneuverEpoch_ - Duration::Seconds(10.0),
            maneuverEpoch_ + Duration::Seconds(30.0),
            maneuverEpoch_ + Duration::Seconds(60.0),
            maneuverEpoch_ + Duration::Seconds(200.0),
        };

        const Array<State> propagatedStates = propagated.calculateStatesAt(instants);
        const Array<State> expectedStates = propagator_.calculateStatesAt(initialState, instants, {maneuver});
        const Array<State> coastingStates = propagator_.calculateStatesAt(initialState, instants);

        ASSERT_EQ(instants.getSize(), propagatedStates.getSize());
        for (Size i = 0; i < instants.getSize(); ++i)
        {
            EXPECT_EQ(expectedStates[i], propagatedStates[i]) << "Index " << i;

            // A single instant takes a different integration path than the batch: equal up to integration error
            const State singleState = propagated.calculateStateAt(instants[i]);
            EXPECT_LT(
                (singleState.extractCoordinate(CartesianPosition::Default()) -
                 expectedStates[i].extractCoordinate(CartesianPosition::Default()))
                    .norm(),
                1e-4
            ) << "Index "
              << i;
            EXPECT_NEAR(
                singleState.extractCoordinate(CoordinateSubset::Mass())(0),
                expectedStates[i].extractCoordinate(CoordinateSubset::Mass())(0),
                1e-9
            ) << "Index "
              << i;
        }

        // The maneuver is effective: it burns propellant and changes the orbit
        const double initialMass = initialState.extractCoordinate(CoordinateSubset::Mass())(0);
        EXPECT_NEAR(initialMass, propagatedStates[2].extractCoordinate(CoordinateSubset::Mass())(0), 1e-12);
        EXPECT_LT(propagatedStates.getLast().extractCoordinate(CoordinateSubset::Mass())(0), initialMass - 1e-4);
        EXPECT_GT(
            (propagatedStates.getLast().extractCoordinate(CartesianPosition::Default()) -
             coastingStates.getLast().extractCoordinate(CartesianPosition::Default()))
                .norm(),
            1.0
        );
    }

    // Cached states around the maneuver: states inside it are blended from forward and backward propagation, both
    // of which go through the maneuver
    {
        const State endState =
            propagator_.calculateStateAt(initialState, maneuverEpoch_ + Duration::Seconds(120.0), {maneuver});

        const Propagated propagated = {propagator_, {initialState, endState}, 1, {maneuver}};

        const Instant instant = maneuverEpoch_ + Duration::Seconds(30.0);

        const State propagatedState = propagated.calculateStateAt(instant);
        const State expectedState = propagator_.calculateStateAt(initialState, instant, {maneuver});

        EXPECT_LT(
            (propagatedState.extractCoordinate(CartesianPosition::Default()) -
             expectedState.extractCoordinate(CartesianPosition::Default()))
                .norm(),
            1e-6
        );
        EXPECT_LT(
            (propagatedState.extractCoordinate(CartesianVelocity::Default()) -
             expectedState.extractCoordinate(CartesianVelocity::Default()))
                .norm(),
            1e-9
        );
        EXPECT_NEAR(
            expectedState.extractCoordinate(CoordinateSubset::Mass())(0),
            propagatedState.extractCoordinate(CoordinateSubset::Mass())(0),
            1e-9
        );

        // The maneuver is not applied to the model without it
        const Propagated coastingPropagated = {propagator_, {initialState, endState}};
        EXPECT_GT(
            (coastingPropagated.calculateStateAt(instant).extractCoordinate(CartesianPosition::Default()) -
             expectedState.extractCoordinate(CartesianPosition::Default()))
                .norm(),
            1e-6
        );
    }

    // Revolution numbers are counted along the maneuvered trajectory
    {
        const Propagated propagated = {propagator_, initialState, defaultRevolutionNumber_, {maneuver}};

        EXPECT_NO_THROW(propagated.calculateRevolutionNumberAt(maneuverEpoch_ + Duration::Hours(4.0)));
        EXPECT_EQ(
            Propagated(propagator_, initialState, defaultRevolutionNumber_)
                .calculateRevolutionNumberAt(maneuverEpoch_ + Duration::Hours(4.0)),
            propagated.calculateRevolutionNumberAt(maneuverEpoch_ + Duration::Hours(4.0))
        );
        EXPECT_EQ(defaultRevolutionNumber_, propagated.calculateRevolutionNumberAt(initialState.getInstant()));
    }

    // Validation: overlapping or duplicated maneuvers, and states without mass
    {
        const Maneuver overlappingManeuver =
            maneuverGenerator(maneuverEpoch_ + Duration::Seconds(30.0), Duration::Seconds(60.0));
        const Maneuver touchingManeuver =
            maneuverGenerator(maneuverEpoch_ + Duration::Seconds(60.0), Duration::Seconds(30.0));

        for (const Array<Maneuver>& maneuvers : Array<Array<Maneuver>> {
                 {maneuver, maneuver},
                 {maneuver, overlappingManeuver},
                 {overlappingManeuver, maneuver},
                 {maneuver, touchingManeuver},
             })
        {
            EXPECT_THROW(Propagated(propagator_, initialState, 1, maneuvers), ostk::core::error::RuntimeError);
            EXPECT_THROW(
                Propagated(propagator_, Array<State> {initialState}, 1, maneuvers), ostk::core::error::RuntimeError
            );
        }

        // Non overlapping maneuvers are accepted, in any order
        const Maneuver laterManeuver =
            maneuverGenerator(maneuverEpoch_ + Duration::Seconds(61.0), Duration::Seconds(30.0));
        EXPECT_NO_THROW(Propagated(propagator_, initialState, 1, {maneuver, laterManeuver}));
        EXPECT_NO_THROW(Propagated(propagator_, initialState, 1, {laterManeuver, maneuver}));

        // Without maneuvers, the mass is not required
        EXPECT_NO_THROW(Propagated(propagator_, defaultState_));

        // With maneuvers, it is
        EXPECT_THROW(
            {
                try
                {
                    Propagated(propagator_, defaultState_, 1, {maneuver});
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(e.getMessage(), String("Cached states must contain the mass when there are maneuvers."));
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );

        // The cached state array of a model with maneuvers cannot be replaced by states without mass
        Propagated propagated = {propagator_, initialState, 1, {maneuver}};
        EXPECT_THROW(propagated.setCachedStateArray({defaultState_}), ostk::core::error::RuntimeError);
        ASSERT_EQ(1, propagated.getCachedStateArray().getSize());
        EXPECT_EQ(initialState, propagated.getCachedStateArray()[0]);
        EXPECT_NO_THROW(propagated.setCachedStateArray({stateWithMassGenerator(maneuverEpoch_ - Duration::Seconds(30.0))
        }));
    }
}
