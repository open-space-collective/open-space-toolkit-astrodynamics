/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::filesystem::Directory;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::Thruster;
using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::flight::system::SatelliteSystemBuilder;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Validation_SelfValidation : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        const PropulsionSystem propulsionSystem = {0.1, 1500.0};

        this->satelliteGeometry_ = satelliteGeometry;
        this->propulsionSystem_ = propulsionSystem;

        this->satelliteSystem_ = {
            this->satelliteDryMass_,
            satelliteGeometry_,
            Matrix3d::Identity(),
            1.0,
            2.1,
            propulsionSystem_,
        };

        this->earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
        this->defaultDynamics_ = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earthSpherical_),
        };

        this->defaultPropagator_ = {defaultNumericalSolver_, defaultDynamics_};
    }

    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);

    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();
    Composite satelliteGeometry_ = Composite::Undefined();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();

    const Shared<CoordinateBroker> dragCoordinateBrokerSPtr_ = std::make_shared<CoordinateBroker>(CoordinateBroker(
        {CartesianPosition::Default(),
         CartesianVelocity::Default(),
         CoordinateSubset::Mass(),
         CoordinateSubset::SurfaceArea(),
         CoordinateSubset::DragCoefficient()}
    ));
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_SelfValidation, ForceModel_TabulatedvsConstantThrustvsManeuver)
{
    VectorXd coordinates(7);
    coordinates << 1204374.4983743676, -6776950.422456586, 0.0, -967.6410027260863, -171.96557073856417,
        7546.1119092033205, 110.0;

    const Array<Shared<const CoordinateSubset>> stateCoordinateSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    };

    const State initialState = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0, 0), Scale::UTC),
        coordinates,
        gcrfSPtr_,
        stateCoordinateSubsets,
    };

    const Array<Instant> instants =
        Interval::Closed(initialState.accessInstant(), initialState.accessInstant() + Duration::Hours(1.5))
            .generateGrid(Duration::Minutes(1.0));

    const PropulsionSystem propulsionSystem = {1e-1, 3000.0};

    const SatelliteSystem satelliteSystem = SatelliteSystemBuilder::Default()
                                                .withPropulsionSystem(propulsionSystem)
                                                .withDryMass(Mass::Kilograms(100.0))
                                                .build();

    const Shared<ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(ConstantThrust::Intrack());

    // Create dynamics with thruster
    Array<Shared<Dynamics>> dynamicsWithThruster = defaultDynamics_;
    dynamicsWithThruster.add(std::make_shared<Thruster>(satelliteSystem, constantThrustSPtr));
    Propagator propagatorWithThruster = {defaultNumericalSolver_, dynamicsWithThruster};

    // Propagate all states with thruster to be able to extract contributions for tabulated dynamics
    const Array<State> statesWithThruster = propagatorWithThruster.calculateStatesAt(initialState, instants);
    MatrixXd contributions(statesWithThruster.getSize(), 4);
    for (Size i = 0; i < statesWithThruster.getSize(); i++)
    {
        const VectorXd contribution = dynamicsWithThruster[dynamicsWithThruster.getSize() - 1]->computeContribution(
            statesWithThruster[i].accessInstant(), statesWithThruster[i].accessCoordinates(), gcrfSPtr_
        );
        contributions.row(i) = contribution;
    }

    // Create tabulated dynamics
    const TabulatedDynamics tabulated = {
        instants,
        contributions,
        {CartesianVelocity::Default(), CoordinateSubset::Mass()},
        gcrfSPtr_,
    };

    // Create dynamics with tabulated and propagator with tabulated
    Array<Shared<Dynamics>> dynamicsWithTabulated = defaultDynamics_;
    dynamicsWithTabulated.add(std::make_shared<TabulatedDynamics>(tabulated));
    Propagator propagatorWithTabulated = {defaultNumericalSolver_, dynamicsWithTabulated};
    const Array<State> statesWithTabulated = propagatorWithTabulated.calculateStatesAt(initialState, instants);

    // Create maneuver from tabulated and propagator with maneuver
    Array<State> maneuverStateArray = Array<State>::Empty();
    maneuverStateArray.reserve(instants.getSize());

    const Array<Shared<const CoordinateSubset>> maneuverCoordinateSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CartesianAcceleration::Default(),
        CoordinateSubset::MassFlowRate(),
    };

    for (Size i = 0; i < instants.getSize(); i++)
    {
        const Vector3d accelerationCoordinates = contributions.row(i).head(3);
        const VectorXd positionVelocityCoordinates =
            statesWithTabulated[i].extractCoordinates({maneuverCoordinateSubsets[0], maneuverCoordinateSubsets[1]});
        const Real massFlowRate = contributions(i, 3);

        VectorXd maneuverCoordinates(10);
        maneuverCoordinates << positionVelocityCoordinates, accelerationCoordinates, massFlowRate;

        maneuverStateArray.add({
            instants[i],
            maneuverCoordinates,
            gcrfSPtr_,
            maneuverCoordinateSubsets,
        });
    }

    const Maneuver maneuver = Maneuver(maneuverStateArray);

    Propagator maneuverPropagator = {defaultNumericalSolver_, defaultDynamics_, {maneuver}};
    const Array<State> statesWithManeuver = maneuverPropagator.calculateStatesAt(initialState, instants);

    // Validate that all return very similar results
    for (Size i = 0; i < instants.getSize(); i++)
    {
        const VectorXd positionWithThruster = statesWithThruster[i].extractCoordinate(CartesianPosition::Default());
        const VectorXd velocityWithThruster = statesWithThruster[i].extractCoordinate(CartesianVelocity::Default());
        const Real massWithThruster = statesWithThruster[i].extractCoordinate(CoordinateSubset::Mass())(0);

        const VectorXd positionWithTabulated = statesWithTabulated[i].extractCoordinate(CartesianPosition::Default());
        const VectorXd velocityWithTabulated = statesWithTabulated[i].extractCoordinate(CartesianVelocity::Default());
        const Real massWithTabulated = statesWithTabulated[i].extractCoordinate(CoordinateSubset::Mass())(0);

        const VectorXd positionWithManeuver = statesWithManeuver[i].extractCoordinate(CartesianPosition::Default());
        const VectorXd velocityWithManeuver = statesWithManeuver[i].extractCoordinate(CartesianVelocity::Default());
        const Real massWithManeuver = statesWithManeuver[i].extractCoordinate(CoordinateSubset::Mass())(0);

        // Assert state errors
        ASSERT_GT(5.e-4, (positionWithThruster - positionWithTabulated).norm());
        ASSERT_GT(5.e-7, (velocityWithThruster - velocityWithTabulated).norm());
        ASSERT_GT(1.e-12, (massWithThruster - massWithTabulated));

        ASSERT_GT(5.e-4, (positionWithThruster - positionWithManeuver).norm());
        ASSERT_GT(5.e-7, (velocityWithThruster - velocityWithManeuver).norm());
        ASSERT_GT(1.e-12, (massWithThruster - massWithManeuver));

        ASSERT_GT(1.e-14, (positionWithTabulated - positionWithManeuver).norm());
        ASSERT_GT(1.e-14, (velocityWithTabulated - velocityWithManeuver).norm());
        ASSERT_GT(1.e-14, (massWithTabulated - massWithManeuver));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_SelfValidation, PropagationInterval)
{
    // Test that calculateStatesAt returns the same answer if states are requested at short or long intervals
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Propagate all states at short interval
        const Array<State> propagatedStateArray_short = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Propagate all states at long interval
        Array<Instant> instantArray_longInterval = Array<Instant>::Empty();
        const size_t indexIncrement = 10;
        for (size_t i = 0; i < instantArray.getSize(); i += indexIncrement)
        {
            instantArray_longInterval.add(instantArray[i]);
        }
        const Array<State> propagatedStateArray_long =
            defaultPropagator_.calculateStatesAt(state, instantArray_longInterval);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Run once every X times
            if (i % indexIncrement == 0)
            {
                // GCRF Compare
                const Position positionShort = propagatedStateArray_short[i].getPosition();
                const Velocity velocityShort = propagatedStateArray_short[i].getVelocity();
                const Position positionLong = propagatedStateArray_long[i / indexIncrement].getPosition();
                const Velocity velocityLong = propagatedStateArray_long[i / indexIncrement].getVelocity();

                const double positionErrorGCRF =
                    (positionShort.accessCoordinates() - positionLong.accessCoordinates()).norm();
                const double velocityErrorGCRF =
                    (velocityShort.accessCoordinates() - velocityLong.accessCoordinates()).norm();

                ASSERT_EQ(*Frame::GCRF(), *positionShort.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *velocityShort.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *positionLong.accessFrame());
                ASSERT_EQ(*Frame::GCRF(), *velocityLong.accessFrame());

                ASSERT_GT(1.0e-7, positionErrorGCRF);
                ASSERT_GT(1.0e-7, velocityErrorGCRF);

                // Results console output

                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
                // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }
    }

    // Test that calculateStateAt returns the almost the same answer if states are requested at short or long intervals
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        testing::internal::CaptureStdout();

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // Propagator output generated
            const State stateShort = defaultPropagator_.calculateStateAt(state, instantArray[i]);

            // Run once every X times
            if (i % 10 == 0)
            {
                const State stateLong = defaultPropagator_.calculateStateAt(state, instantArray[i]);

                // GCRF Compare
                const Position positionShort = stateShort.getPosition();
                const Velocity velocityShort = stateShort.getVelocity();
                const Position positionLong = stateLong.getPosition();
                const Velocity velocityLong = stateLong.getVelocity();

                const double positionErrorGCRF =
                    (positionShort.accessCoordinates() - positionLong.accessCoordinates()).norm();
                const double velocityErrorGCRF =
                    (velocityShort.accessCoordinates() - velocityLong.accessCoordinates()).norm();

                ASSERT_GT(3e-8, positionErrorGCRF);
                ASSERT_GT(3e-11, velocityErrorGCRF);

                // Results console output

                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
                // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }

        EXPECT_TRUE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_SelfValidation, StepperType)
{
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        const size_t propagationSpanHours = 24;
        const size_t propagationIntervalSeconds = 30;

        Array<Instant> instantArray = Array<Instant>::Empty();
        for (size_t i = 0; i < (propagationSpanHours * 3600) / propagationIntervalSeconds;
             i += propagationIntervalSeconds)
        {
            instantArray.add(startInstant + Duration::Seconds(i));
        }

        // Construct default numerical solver
        const NumericalSolver numericalSolver54 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15,
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr_),
        };

        // Setup Propagator
        const Propagator propagator54 = {numericalSolver54, defaultDynamics_};

        // Propagate all states
        const Array<State> propagatedStateArray54 = propagator54.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray78 = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position position54 = propagatedStateArray54[i].getPosition();
            const Velocity velocity54 = propagatedStateArray54[i].getVelocity();
            const Position position78 = propagatedStateArray78[i].getPosition();
            const Velocity velocity78 = propagatedStateArray78[i].getVelocity();

            const double positionErrorGCRF = (position54.accessCoordinates() - position78.accessCoordinates()).norm();
            const double velocityErrorGCRF = (velocity54.accessCoordinates() - velocity78.accessCoordinates()).norm();

            ASSERT_GT(5e-7, positionErrorGCRF);
            ASSERT_GT(5e-10, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_SelfValidation, GravityModel)
{
    // EGM2008 360x360, 100x100, 70x70, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);
        const State state = {
            startInstant,
            Position::Meters({-1514668.94081025, -192084.12149140, 6831711.45843687}, gcrfSPtr_),
            Velocity::MetersPerSecond({-6348.07918761, 3867.58249270, -1297.17610443}, gcrfSPtr_),
        };

        const Interval interval = Interval::Closed(startInstant, startInstant + Duration::Seconds(7200.0));
        const Duration stepDuration = Duration::Seconds(60.0);
        const Array<Instant> instantArray = interval.generateGrid(stepDuration);

        // Create dynamics
        const Shared<Celestial> earth_360 = std::make_shared<Celestial>(Earth::EGM2008(360, 360));
        const Shared<Celestial> earth_100 = std::make_shared<Celestial>(Earth::EGM2008(100, 100));
        const Shared<Celestial> earth_70 = std::make_shared<Celestial>(Earth::EGM2008(70, 70));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM2008(45, 45));

        const Array<Shared<Dynamics>> EGM2008dynamics_360 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_360),
        };
        const Array<Shared<Dynamics>> EGM2008dynamics_100 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_100),
        };
        const Array<Shared<Dynamics>> EGM2008dynamics_70 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_70),
        };
        const Array<Shared<Dynamics>> EGM2008dynamics_45 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_45),
        };

        // Setup Propagator
        const Propagator propagator_360 = {defaultNumericalSolver_, EGM2008dynamics_360};
        const Propagator propagator_100 = {defaultNumericalSolver_, EGM2008dynamics_100};
        const Propagator propagator_70 = {defaultNumericalSolver_, EGM2008dynamics_70};
        const Propagator propagator_45 = {defaultNumericalSolver_, EGM2008dynamics_45};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = propagator_360.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_100 = propagator_100.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_70 = propagator_70.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF_360 = propagatedStateArray_360[i].getPosition();
            const Velocity velocityGCRF_360 = propagatedStateArray_360[i].getVelocity();
            const Position positionGCRF_100 = propagatedStateArray_100[i].getPosition();
            const Velocity velocityGCRF_100 = propagatedStateArray_100[i].getVelocity();
            const Position positionGCRF_70 = propagatedStateArray_70[i].getPosition();
            const Velocity velocityGCRF_70 = propagatedStateArray_70[i].getVelocity();
            const Position positionGCRF_45 = propagatedStateArray_45[i].getPosition();
            const Velocity velocityGCRF_45 = propagatedStateArray_45[i].getVelocity();

            // 360 vs 100
            const double positionErrorGCRF_360_100 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_100.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_100 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_100.accessCoordinates()).norm();
            ASSERT_GT(2e-3, positionErrorGCRF_360_100);
            ASSERT_GT(2e-6, velocityErrorGCRF_360_100);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 100 Position error is: " << positionErrorGCRF_360_100 << "m" << std::endl;
            // std::cout << "360 vs 100 Velocity error is: " << velocityErrorGCRF_360_100 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 70
            const double positionErrorGCRF_360_70 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_70.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_70 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_70.accessCoordinates()).norm();
            ASSERT_GT(5e-2, positionErrorGCRF_360_70);
            ASSERT_GT(5e-5, velocityErrorGCRF_360_70);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 70 Position error is: " << positionErrorGCRF_360_70 << "m" << std::endl;
            // std::cout << "360 vs 70 Velocity error is: " << velocityErrorGCRF_360_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionErrorGCRF_360_45 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_45.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_45 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_45.accessCoordinates()).norm();
            ASSERT_GT(5e-1, positionErrorGCRF_360_45);
            ASSERT_GT(5e-4, velocityErrorGCRF_360_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 45 Position error is: " << positionErrorGCRF_360_45 << "m" << std::endl;
            // std::cout << "360 vs 45 Velocity error is: " << velocityErrorGCRF_360_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM96 360x360, 180x180, 90x90, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);
        const State state = {
            startInstant,
            Position::Meters({-1514668.94081025, -192084.12149140, 6831711.45843687}, gcrfSPtr_),
            Velocity::MetersPerSecond({-6348.07918761, 3867.58249270, -1297.17610443}, gcrfSPtr_),
        };

        const Interval interval = Interval::Closed(startInstant, startInstant + Duration::Seconds(7200.0));
        const Duration stepDuration = Duration::Seconds(60.0);
        const Array<Instant> instantArray = interval.generateGrid(stepDuration);

        // Create dynamics
        const Shared<Celestial> earth_360 = std::make_shared<Celestial>(Earth::EGM96(360, 360));
        const Shared<Celestial> earth_180 = std::make_shared<Celestial>(Earth::EGM96(180, 180));
        const Shared<Celestial> earth_90 = std::make_shared<Celestial>(Earth::EGM96(90, 90));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM96(45, 45));

        const Array<Shared<Dynamics>> EGM96Dynamics_360 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_360),
        };
        const Array<Shared<Dynamics>> EGM96Dynamics_180 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_180),
        };
        const Array<Shared<Dynamics>> EGM96Dynamics_90 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_90),
        };
        const Array<Shared<Dynamics>> EGM96Dynamics_45 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_45),
        };

        // Setup Propagator
        const Propagator propagator_360 = {defaultNumericalSolver_, EGM96Dynamics_360};
        const Propagator propagator_180 = {defaultNumericalSolver_, EGM96Dynamics_180};
        const Propagator propagator_90 = {defaultNumericalSolver_, EGM96Dynamics_90};
        const Propagator propagator_45 = {defaultNumericalSolver_, EGM96Dynamics_45};

        // Propagate all states
        const Array<State> propagatedStateArray_360 = propagator_360.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_180 = propagator_180.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_90 = propagator_90.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF_360 = propagatedStateArray_360[i].getPosition();
            const Velocity velocityGCRF_360 = propagatedStateArray_360[i].getVelocity();
            const Position positionGCRF_180 = propagatedStateArray_180[i].getPosition();
            const Velocity velocityGCRF_180 = propagatedStateArray_180[i].getVelocity();
            const Position positionGCRF_90 = propagatedStateArray_90[i].getPosition();
            const Velocity velocityGCRF_90 = propagatedStateArray_90[i].getVelocity();
            const Position positionGCRF_45 = propagatedStateArray_45[i].getPosition();
            const Velocity velocityGCRF_45 = propagatedStateArray_45[i].getVelocity();

            // 360 vs 180
            const double positionErrorGCRF_360_180 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_180.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_180 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_180.accessCoordinates()).norm();
            ASSERT_GT(1.0e-6, positionErrorGCRF_360_180);
            ASSERT_GT(1.2e-9, velocityErrorGCRF_360_180);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 180 Position error is: " << positionErrorGCRF_360_180 << "m" << std::endl;
            // std::cout << "360 vs 180 Velocity error is: " << velocityErrorGCRF_360_180 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 90
            const double positionErrorGCRF_360_90 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_90.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_90 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_90.accessCoordinates()).norm();
            ASSERT_GT(7e-3, positionErrorGCRF_360_90);
            ASSERT_GT(7e-6, velocityErrorGCRF_360_90);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 90 Position error is: " << positionErrorGCRF_360_90 << "m" << std::endl;
            // std::cout << "360 vs 90 Velocity error is: " << velocityErrorGCRF_360_90 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 360 vs 45
            const double positionErrorGCRF_360_45 =
                (positionGCRF_360.accessCoordinates() - positionGCRF_45.accessCoordinates()).norm();
            const double velocityErrorGCRF_360_45 =
                (velocityGCRF_360.accessCoordinates() - velocityGCRF_45.accessCoordinates()).norm();
            ASSERT_GT(5e-1, positionErrorGCRF_360_45);
            ASSERT_GT(5e-4, velocityErrorGCRF_360_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "360 vs 45 Position error is: " << positionErrorGCRF_360_45 << "m" << std::endl;
            // std::cout << "360 vs 45 Velocity error is: " << velocityErrorGCRF_360_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM84 180x180, 70x70, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);
        const State state = {
            startInstant,
            Position::Meters({-1514668.94081025, -192084.12149140, 6831711.45843687}, gcrfSPtr_),
            Velocity::MetersPerSecond({-6348.07918761, 3867.58249270, -1297.17610443}, gcrfSPtr_),
        };

        const Interval interval = Interval::Closed(startInstant, startInstant + Duration::Seconds(7200.0));
        const Duration stepDuration = Duration::Seconds(60.0);
        const Array<Instant> instantArray = interval.generateGrid(stepDuration);

        // Create dynamics
        const Shared<Celestial> earth_180 = std::make_shared<Celestial>(Earth::EGM84(180, 180));
        const Shared<Celestial> earth_70 = std::make_shared<Celestial>(Earth::EGM84(70, 70));
        const Shared<Celestial> earth_45 = std::make_shared<Celestial>(Earth::EGM84(45, 45));

        const Array<Shared<Dynamics>> EGM84Dynamics_180 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_180),
        };
        const Array<Shared<Dynamics>> EGM84Dynamics_70 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_70),
        };
        const Array<Shared<Dynamics>> EGM84Dynamics_45 = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth_45),
        };

        // Setup Propagator
        const Propagator propagator_180 = {defaultNumericalSolver_, EGM84Dynamics_180};
        const Propagator propagator_70 = {defaultNumericalSolver_, EGM84Dynamics_70};
        const Propagator propagator_45 = {defaultNumericalSolver_, EGM84Dynamics_45};

        // Propagate all states
        const Array<State> propagatedStateArray_180 = propagator_180.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_70 = propagator_70.calculateStatesAt(state, instantArray);
        const Array<State> propagatedStateArray_45 = propagator_45.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF_180 = propagatedStateArray_180[i].getPosition();
            const Velocity velocityGCRF_180 = propagatedStateArray_180[i].getVelocity();
            const Position positionGCRF_70 = propagatedStateArray_70[i].getPosition();
            const Velocity velocityGCRF_70 = propagatedStateArray_70[i].getVelocity();
            const Position positionGCRF_45 = propagatedStateArray_45[i].getPosition();
            const Velocity velocityGCRF_45 = propagatedStateArray_45[i].getVelocity();

            // 180 vs 70
            const double positionErrorGCRF_180_70 =
                (positionGCRF_180.accessCoordinates() - positionGCRF_70.accessCoordinates()).norm();
            const double velocityErrorGCRF_180_70 =
                (velocityGCRF_180.accessCoordinates() - velocityGCRF_70.accessCoordinates()).norm();
            ASSERT_GT(6e-2, positionErrorGCRF_180_70);
            ASSERT_GT(6e-5, velocityErrorGCRF_180_70);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 70 Position error is: " << positionErrorGCRF_180_70 << "m" << std::endl;
            // std::cout << "180 vs 70 Velocity error is: " << velocityErrorGCRF_180_70 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // 180 vs 45
            const double positionErrorGCRF_180_45 =
                (positionGCRF_180.accessCoordinates() - positionGCRF_45.accessCoordinates()).norm();
            const double velocityErrorGCRF_180_45 =
                (velocityGCRF_180.accessCoordinates() - velocityGCRF_45.accessCoordinates()).norm();
            ASSERT_GT(3e-1, positionErrorGCRF_180_45);
            ASSERT_GT(3e-4, velocityErrorGCRF_180_45);

            // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "180 vs 45 Position error is: " << positionErrorGCRF_180_45 << "m" << std::endl;
            // std::cout << "180 vs 45 Velocity error is: " << velocityErrorGCRF_180_45 <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }
}
