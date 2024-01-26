/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/CSSISpaceWeather.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solvers/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::ctnr::Tuple;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::filesystem::Directory;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Size;

using ostk::math::geometry::d3::objects::Composite;
using ostk::math::geometry::d3::objects::Cuboid;
using ostk::math::geometry::d3::objects::Point;
using ostk::math::object::Matrix3d;
using ostk::math::object::MatrixXd;
using ostk::math::object::Vector3d;
using ostk::math::object::VectorXd;

using ostk::physics::data::Scalar;
using ostk::physics::Environment;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::environment::ephemerides::Analytical;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Time;
using ostk::physics::time::Scale;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using ostk::physics::environment::object::celestial::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using SWManager = ostk::physics::environment::atmospheric::earth::Manager;
using ostk::physics::environment::atmospheric::earth::CSSISpaceWeather;

using ostk::astro::eventcondition::InstantCondition;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::Orbit;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;
using ostk::astro::trajectory::LocalOrbitalFrameDirection;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::Dynamics;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::SatelliteSystemBuilder;
using ostk::astro::dynamics::PositionDerivative;
using ostk::astro::dynamics::CentralBodyGravity;
using ostk::astro::dynamics::ThirdBodyGravity;
using ostk::astro::dynamics::AtmosphericDrag;
using ostk::astro::dynamics::Thruster;
using ostk::astro::dynamics::Tabulated;
using ostk::astro::guidancelaw::ConstantThrust;
using ostk::astro::guidancelaw::QLaw;
using ostk::astro::solvers::FiniteDifferenceSolver;

class OpenSpaceToolkit_Astrodynamics_Propagation_Validation : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        const PropulsionSystem propulsionSystem = {
            Scalar(0.1, PropulsionSystem::thrustSIUnit),
            Scalar(1500.0, PropulsionSystem::specificImpulseSIUnit),
        };

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

    const NumericalSolver defaultRK4_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const NumericalSolver defaultRKD5_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);
    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);

    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();
    Composite satelliteGeometry_ = Composite::Undefined();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();

    const Shared<CoordinatesBroker> dragCoordinatesBrokerSPtr_ = std::make_shared<CoordinatesBroker>(CoordinatesBroker(
        {CartesianPosition::Default(),
         CartesianVelocity::Default(),
         CoordinatesSubset::Mass(),
         CoordinatesSubset::SurfaceArea(),
         CoordinatesSubset::DragCoefficient()}
    ));
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Propagation_Validation, CrossValidation_ForceModel_TwoBody)
{
    // Two body vs STK in GCRF and ITRF
    {
        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/STK_TwoBody_2hr_run.csv"
            )),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referencePositionArray_ITRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArray_ITRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC));

            referencePositionArrayGCRF.add({
                referenceRow[1].accessReal(),
                referenceRow[2].accessReal(),
                referenceRow[3].accessReal(),
            });
            referenceVelocityArrayGCRF.add({
                referenceRow[4].accessReal(),
                referenceRow[5].accessReal(),
                referenceRow[6].accessReal(),
            });

            referencePositionArray_ITRF.add({
                referenceRow[7].accessReal(),
                referenceRow[8].accessReal(),
                referenceRow[9].accessReal(),
            });
            referenceVelocityArray_ITRF.add({
                referenceRow[10].accessReal(),
                referenceRow[11].accessReal(),
                referenceRow[12].accessReal(),
            });
        }

        const Shared<const Frame> itrfSPtr = Frame::ITRF();

        // Current state and instant setup
        const Instant startInstant = instantArray[0];

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Propagate all states
        const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(2e-7, positionErrorGCRF);
            ASSERT_GT(2e-10, velocityErrorGCRF);

            // ITRF Compare
            const Position position_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).getPosition();
            const Velocity velocity_ITRF = (propagatedStateArray[i].inFrame(itrfSPtr)).getVelocity();

            const double positionError_ITRF =
                (position_ITRF.accessCoordinates() - referencePositionArray_ITRF[i]).norm();
            const double velocityError_ITRF =
                (velocity_ITRF.accessCoordinates() - referenceVelocityArray_ITRF[i]).norm();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(2e-2, positionError_ITRF);
            ASSERT_GT(2e-5, velocityError_ITRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;

            // // Results console output
            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "ITRF Position error is: " << positionError_ITRF << "m" << std::endl;
            // std::cout << "ITRF Velocity error is: " << velocityError_ITRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Propagation_Validation, CrossValidation_ForceModel_EGM2008)
{
    // EGM2008 100x100 perturbation only vs STK EGM2008
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_EGM2008_100x100_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Create dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM2008(100, 100));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(1.5e-1, positionErrorGCRF);
            ASSERT_GT(1.5e-4, velocityErrorGCRF);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Propagation_Validation, CrossValidation_ForceModel_WGS84EGM96)
{
    // EGM96 70x70 perturbation only vs STK WGS84EGM96
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84EGM96_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Create dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM96(70, 70));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(1.5e-4, positionErrorGCRF);
            ASSERT_GT(1.5e-7, velocityErrorGCRF);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Propagation_Validation, CrossValidation_ForceModel_EGM84)
{
    // EGM84 70x70 perturbation only vs STK EGM84
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup dynamics
        const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM84(70, 70));
        const Array<Shared<Dynamics>> dynamics = {
            std::make_shared<PositionDerivative>(),
            std::make_shared<CentralBodyGravity>(earth),
        };

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

        // Setup Propagator
        const Propagator propagator = {defaultNumericalSolver_, dynamics};

        // Propagate all states
        const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

        // Validation loop
        for (size_t i = 0; i < instantArray.getSize(); i++)
        {
            // GCRF Compare
            const Position positionGCRF = propagatedStateArray[i].getPosition();
            const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

            const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
            const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

            ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

            ASSERT_GT(7e-5, positionErrorGCRF);
            ASSERT_GT(7e-8, velocityErrorGCRF);

            // Results console output

            // std::cout << "**************************************" << std::endl;
            // std::cout.setf(std::ios::scientific,std::ios::floatfield);
            // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
            // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
            // std::cout.setf(std::ios::fixed,std::ios::floatfield);
            // std::cout << "**************************************" << std::endl;
        }
    }

    // EGM84 180x180, 70x70, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_WGS84_70x70_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

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

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Propagation_Validation, SelfValidation_ForceModel_TabulatedConstantThrust)
{
    VectorXd coordinates(9);
    coordinates << 1204374.4983743676, -6776950.422456586, 0.0, -967.6410027260863, -171.96557073856417,
        7546.1119092033205, 110.0, 500.0, 2.2;

    const State initialState = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0, 0), Scale::UTC),
        coordinates,
        Frame::GCRF(),
        dragCoordinatesBrokerSPtr_,
    };

    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM96, Directory::Undefined(), 20, 20),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
    );
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const PropulsionSystem propulsionSystem = {
        Scalar(1e-1, PropulsionSystem::thrustSIUnit),
        Scalar(3000.0, PropulsionSystem::specificImpulseSIUnit),
    };

    const SatelliteSystem satelliteSystem = SatelliteSystemBuilder::Default()
                                                .withPropulsionSystem(propulsionSystem)
                                                .withDryMass(Mass::Kilograms(100.0))
                                                .build();

    const Shared<ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(ConstantThrust::Intrack());

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
        std::make_shared<Thruster>(satelliteSystem, constantThrustSPtr),
    };

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        1.0,
        1e-12,
        1e-12,
    };

    // Create contribution profile
    Propagator propagator = {numericalSolver, dynamics};
    const State endStatePropagated =
        propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Hours(1.5));

    // TBI: For some reason it's pushing the initial state twice
    const Array<State> tmpStates = propagator.accessNumericalSolver().getObservedStates();
    const Array<State> states(tmpStates.begin() + 1, tmpStates.end());

    MatrixXd contributions(states.getSize(), 4);
    for (Size i = 0; i < states.getSize(); ++i)
    {
        const VectorXd contribution =
            dynamics[3]->computeContribution(states[i].accessInstant(), states[i].accessCoordinates(), Frame::GCRF());
        contributions.row(i) = contribution;
    }
    const Array<Instant> instants = states.map<Instant>(
        [](const auto& state)
        {
            return state.accessInstant();
        }
    );

    const Tabulated tabulated = {
        instants,
        contributions,
        {CartesianVelocity::Default(), CoordinatesSubset::Mass()},
        gcrfSPtr_,
    };

    // Re-propagate with tabulated dynamics
    const Array<Shared<Dynamics>> tabulatedDynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
        std::make_shared<Tabulated>(tabulated),
    };

    const NumericalSolver tabulatedNumericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        3.0,
        1e-12,
        1e-12,
    };

    Propagator tabulatedPropagator = {tabulatedNumericalSolver, tabulatedDynamics};
    const State endStateTabulated =
        propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Hours(1.5));

    for (Size i = 0; i < endStateTabulated.getSize(); ++i)
    {
        EXPECT_NEAR(endStatePropagated.accessCoordinates()[i], endStateTabulated.accessCoordinates()[i], 1e-14);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Propagation_Validation, SelfValidation_PropagationInterval)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Propagation_Validation, SelfValidation_StepperType)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Propagation_Validation, SelfValidation_GravityModel)
{
    // EGM2008 360x360, 100x100, 70x70, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/STK_EGM2008_100x100_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

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
        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
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

    // EGM96 180x180, 90x90, 45x45 perturbation only self comparison
    {
        // Current state and instant setup
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                   "Propagated/GMAT_EGM96_360x360_2hr_run.csv")),
            Table::Format::CSV,
            true
        );

        Array<Instant> instantArray = Array<Instant>::Empty();
        Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
        Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

        for (const auto& referenceRow : referenceData)
        {
            instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

            referencePositionArrayGCRF.add(
                1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
            );
            referenceVelocityArrayGCRF.add(
                1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
            );
        }

        // Setup initial conditions
        const State state = {
            startInstant,
            Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
            Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
        };

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
            ASSERT_GT(9e-7, positionErrorGCRF_360_180);
            ASSERT_GT(9e-10, velocityErrorGCRF_360_180);

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
}

// TBI: Specific class for parameterized tests on thruster dynamics, could inherit from base fixture for common
class OpenSpaceToolkit_Astrodynamics_Propagation_Validation_Thruster
    : public ::testing::TestWithParam<
          Tuple<std::string, Shared<const LocalOrbitalFrameFactory>, Vector3d, Real, Real, Real, Real, Real, Real>>
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        this->satelliteGeometry_ = satelliteGeometry;

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

    const NumericalSolver defaultRK4_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const NumericalSolver defaultRKD5_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);
    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    Composite satelliteGeometry_ = Composite::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();
};

TEST_P(OpenSpaceToolkit_Astrodynamics_Propagation_Validation_Thruster, CrossValidation_ForceModel_Thrust)
{
    // Improvements:
    // percentage diff to add
    // add multiple "segments" propagation cases

    // Setup environment

    // Access the test parameters
    const auto parameters = GetParam();

    const String referenceDataFileName = std::get<0>(parameters);
    Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactory = std::get<1>(parameters);
    const Vector3d localOrbitalFrameThrustVector = std::get<2>(parameters);
    const Real satelliteDryMassReal = std::get<3>(parameters);
    const Real thrustReal = std::get<4>(parameters);
    const Real specificImpulseReal = std::get<5>(parameters);

    // Reference data setup
    const Table referenceData = Table::Load(File::Path(Path::Parse(referenceDataFileName)), Table::Format::CSV, true);

    // Initialize reference data arrays
    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceTotalAccelerationArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceManeuverAccelerationArrayGCRF = Array<Vector3d>::Empty();

    Array<Vector3d> referencePositionArrayLOF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayLOF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceManeuverAccelerationArrayLOF = Array<Vector3d>::Empty();
    Array<double> referenceMassArray = Array<double>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(
            Instant::DateTime(DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC)
        );

        referencePositionArrayGCRF.add(
            Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
        );
        referenceTotalAccelerationArrayGCRF.add(
            Vector3d(referenceRow[13].accessReal(), referenceRow[14].accessReal(), referenceRow[15].accessReal())
        );
        referenceManeuverAccelerationArrayGCRF.add(
            Vector3d(referenceRow[16].accessReal(), referenceRow[17].accessReal(), referenceRow[18].accessReal())
        );

        referencePositionArrayLOF.add(
            Vector3d(referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal())
        );
        referenceVelocityArrayLOF.add(
            Vector3d(referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal())
        );
        referenceManeuverAccelerationArrayLOF.add(
            Vector3d(referenceRow[19].accessReal(), referenceRow[20].accessReal(), referenceRow[21].accessReal())
        );

        referenceMassArray.add(referenceRow[22].accessReal());
    }

    // Local Orbital Frame Direction
    const LocalOrbitalFrameDirection thrustDirection =
        LocalOrbitalFrameDirection(localOrbitalFrameThrustVector, localOrbitalFrameFactory);

    // Coordinates Broker (scenario-independent)
    const Shared<const CoordinatesBroker> coordinatesBrokerSPtr =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker({
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            CoordinatesSubset::Mass(),
        }));

    // Setup initial state
    VectorXd initialCoordinates(7);

    initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0],
        propellantMass_.inKilograms() + satelliteDryMassReal;

    const State initialState = {
        instantArray[0],
        initialCoordinates,
        gcrfSPtr_,
        coordinatesBrokerSPtr,
    };

    // Setup satellite system
    PropulsionSystem propulsionSystem = {
        Scalar(thrustReal, PropulsionSystem::thrustSIUnit),
        Scalar(specificImpulseReal, PropulsionSystem::specificImpulseSIUnit),
    };

    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    SatelliteSystem satelliteSystem = {
        Mass::Kilograms(satelliteDryMassReal),
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.1,
        propulsionSystem,
    };

    // Setup validation tolerances
    const Real positionErrorGCRFTolerance = std::get<6>(parameters);
    const Real velocityErrorGCRFTolerance = std::get<7>(parameters);
    const Real MassErrorTolerance = std::get<8>(parameters);

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Undefined)
    );
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    Shared<ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(thrustDirection);

    Shared<Thruster> thrusterDynamicsSPtr = std::make_shared<Thruster>(satelliteSystem, constantThrustSPtr);
    Shared<CentralBodyGravity> centralBodyGravitySPtr = std::make_shared<CentralBodyGravity>(earthSPtr);

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(), centralBodyGravitySPtr, thrusterDynamicsSPtr
    };

    // Setup Propagator model and orbit
    const Propagator propagator = {defaultRK4_, dynamics};

    // Propagate all states with OSTk
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(initialState, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize() - 1; i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].inFrame(gcrfSPtr_).getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].inFrame(gcrfSPtr_).getVelocity();
        const double mass = propagatedStateArray[i].extractCoordinate(CoordinatesSubset::Mass())[0];

        VectorXd OSTkStateCoordinatesGCRF(7);
        OSTkStateCoordinatesGCRF << positionGCRF.accessCoordinates(), velocityGCRF.accessCoordinates(), mass;

        const VectorXd maneuverContributionGCRF =
            thrusterDynamicsSPtr->computeContribution(instantArray[i], OSTkStateCoordinatesGCRF, gcrfSPtr_);
        const VectorXd centralBodyGravityContributionGCRF =
            centralBodyGravitySPtr->computeContribution(instantArray[i], positionGCRF.accessCoordinates(), gcrfSPtr_);
        const VectorXd totalAccelerationGCRF =
            maneuverContributionGCRF.segment(0, 3) + centralBodyGravityContributionGCRF;

        const Vector3d OrekitCentralBodyGravityContributionGCRF =
            referenceTotalAccelerationArrayGCRF[i] - referenceManeuverAccelerationArrayGCRF[i];

        // LOF Compare
        Shared<const Frame> lofSPtr = localOrbitalFrameFactory->generateFrame(
            instantArray[i], positionGCRF.accessCoordinates(), velocityGCRF.accessCoordinates()
        );
        State lofState = propagatedStateArray[i].inFrame(lofSPtr);

        const VectorXd maneuverContributionLOF =
            thrusterDynamicsSPtr->computeContribution(instantArray[i], OSTkStateCoordinatesGCRF, lofSPtr);

        const Position positionLOF = lofState.getPosition();
        const Velocity velocityLOF = lofState.getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();
        const double maneuverAccelerationContributionErrorGCRF =
            (maneuverContributionGCRF.segment(0, 3) - referenceManeuverAccelerationArrayGCRF[i]).norm();
        const double positionErrorLOF = (positionLOF.accessCoordinates() - referencePositionArrayLOF[i]).norm();
        const double velocityErrorLOF = (velocityLOF.accessCoordinates() - referencePositionArrayLOF[i]).norm();
        const double maneuverAccelerationContributionErrorLOF =
            (maneuverContributionLOF.segment(0, 3) - referenceManeuverAccelerationArrayLOF[i]).norm();
        const double totalAccelerationErrorGCRF =
            (totalAccelerationGCRF - referenceTotalAccelerationArrayGCRF[i]).norm();
        const double centralBodyGravityAccelerationContributionErrorGCRF =
            (OrekitCentralBodyGravityContributionGCRF - centralBodyGravityContributionGCRF).norm();
        const double massError = std::abs(mass - referenceMassArray[i]);

        // Frames verification
        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());
        ASSERT_EQ(*lofSPtr, *positionLOF.accessFrame());
        ASSERT_EQ(*lofSPtr, *velocityLOF.accessFrame());

        // GCRF Errors
        // State
        ASSERT_GT(positionErrorGCRFTolerance, positionErrorGCRF);
        ASSERT_GT(velocityErrorGCRFTolerance, velocityErrorGCRF);
        ASSERT_GT(MassErrorTolerance, massError);

        // Accelerations from dynamics
        ASSERT_GT(1e-9, maneuverAccelerationContributionErrorGCRF);
        ASSERT_GT(5e-8, centralBodyGravityAccelerationContributionErrorGCRF);
        ASSERT_GT(5e-8, totalAccelerationErrorGCRF);

        // LOF Errors
        // State
        ASSERT_GT(1e-15, positionErrorLOF);
        ASSERT_GT(1e-15, velocityErrorLOF);

        // Acceleration from dynamics
        ASSERT_GT(1e-8, maneuverAccelerationContributionErrorLOF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Instant is: " << instantArray[i] << std::endl;
        // // Quaternion quat = gcrfSPtr_->getTransformTo(lofSPtr, instantArray[i]).getOrientation();
        // // std::cout << lofSPtr->getOriginIn(gcrfSPtr_, instantArray[i]) << std::endl;
        // // std::cout << quat << std::endl ;
        // std::cout << "Position OSTk is: " << positionGCRF.accessCoordinates() << "m" << std::endl;
        // std::cout << "Position Orekit is: " << referencePositionArrayGCRF[i] << "m" << std::endl;
        // std::cout << "Velocity OSTk is: " << velocityGCRF.accessCoordinates() << "m/s" << std::endl;
        // std::cout << "Velocity Orekit is: " << referenceVelocityArrayGCRF[i] << "m/s" << std::endl;
        // std::cout << "Position error GCRF is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error GCRF is: " << velocityErrorGCRF << "m/s" << std::endl;
        // std::cout << "Position OSTk LOF is: " << positionLOF.accessCoordinates() << "m" << std::endl;
        // std::cout << "Position Orekit LOF is: " << referencePositionArrayLOF[i] << "m" << std::endl;
        // std::cout << "Velocity OSTk LOF is: " << velocityLOF.accessCoordinates() << "m/s" << std::endl;
        // std::cout << "Velocity Orekit LOF is: " << referenceVelocityArrayLOF[i] << "m/s" << std::endl;
        // std::cout << "Position error LOF is: " << positionErrorLOF << "m" << std::endl;
        // std::cout << "Velocity error LOF is: " << velocityErrorLOF << "m/s" << std::endl;
        // std::cout << "Mass OSTk is: " << mass << "kg" << std::endl;
        // std::cout << "Mass Orekit is: " << referenceMassArray[i] << "kg" << std::endl;
        // std::cout << "Mass error is: " << massError << "kg" << std::endl;
        // std::cout << "Maneuver acceleration error GCRF X is: " << maneuverContributionGCRF[0] -
        // referenceManeuverAccelerationArrayGCRF[i][0] << "m/s^2" << std::endl; std::cout << "Maneuver acceleration
        // error GCRF Y is: " << maneuverContributionGCRF[1] - referenceManeuverAccelerationArrayGCRF[i][1] <<
        // "m/s^2"
        // << std::endl; std::cout << "Maneuver acceleration error GCRF Z is: " << maneuverContributionGCRF[2] -
        // referenceManeuverAccelerationArrayGCRF[i][2] << "m/s^2" << std::endl; std::cout << "Maneuver acceleration
        // error LOF X is: " << maneuverContributionLOF[0] - referenceManeuverAccelerationArrayLOF[i][0] << "m/s^2"
        // <<
        // std::endl; std::cout << "Maneuver acceleration error LOF Y is: " << maneuverContributionLOF[1] -
        // referenceManeuverAccelerationArrayLOF[i][1] << "m/s^2" << std::endl; std::cout << "Maneuver acceleration
        // error LOF Z is: " << maneuverContributionLOF[2] - referenceManeuverAccelerationArrayLOF[i][2] << "m/s^2"
        // <<
        // std::endl; std::cout << "Maneuver acceleration error GCRF is: " <<
        // maneuverAccelerationContributionErrorGCRF
        // << "m/s^2" << std::endl; // Do it in percentage std::cout << "Maneuver acceleration error LOF is: " <<
        // maneuverAccelerationContributionErrorLOF << "m/s^2" << std::endl; std::cout << "Total acceleration
        // (central
        // body + maneuver) error GCRF is: " << totalAccelerationErrorGCRF << "m/s^2" << std::endl; std::cout <<
        // "Central body acceleration contribution error GCRF is: " <<
        // centralBodyGravityAccelerationContributionErrorGCRF << "m/s^2" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
}

// TBI: Agree on a format to version Orekit validation files
INSTANTIATE_TEST_SUITE_P(
    CrossValidation_ForceModel_Thrust,
    OpenSpaceToolkit_Astrodynamics_Propagation_Validation_Thruster,
    ::testing::Values(
        // Test Case 0
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 1: Start date in 2021
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2021-05-13T12-34-13.345_115.0_0.1_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 2: QSW LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_QSW_0.0_1.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            LocalOrbitalFrameFactory::QSW(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({0.0, 1.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 3: TNW LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_TNW_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            LocalOrbitalFrameFactory::TNW(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 4: LVLH LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_LVLH_1.0_0.0_"
            "0.0_30.0.csv",                                 // Scenario validation data file path
            LocalOrbitalFrameFactory::LVLH(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            100.0,                                          // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            7e-4,                                           // Position error GCRF tolerance [m]
            8e-7,                                           // Velocity error GCRF tolerance [m/s]
            1e-9                                            // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 5: VVLH LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VVLH_1.0_0.0_"
            "0.0_30.0.csv",                                 // Scenario validation data file path
            LocalOrbitalFrameFactory::VVLH(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            100.0,                                          // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            7e-4,                                           // Position error GCRF tolerance [m]
            8e-7,                                           // Velocity error GCRF tolerance [m/s]
            1e-9                                            // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 6: Increase spacecraft mass to 1000kg
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_1015.0_0.1_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            1000.0,                                        // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 7: Increase maneuver duration to 4h
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_14400.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            3e-3,                                          // Position error GCRF tolerance [m]
            3e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 8: Increase spacecraft mass to 1000kg and LVLH
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_1015.0_0.1_1500.0_3600.0_LVLH_1.0_0."
            "0_0.0_30.0.csv",                               // Scenario validation data file path
            LocalOrbitalFrameFactory::LVLH(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            1000.0,                                         // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            7e-4,                                           // Position error GCRF tolerance [m]
            8e-7,                                           // Velocity error GCRF tolerance [m/s]
            1e-9                                            // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 9: Increase maneuver duration to 4h and LVLH
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_14400.0_LVLH_1.0_0."
            "0_0.0_30.0.csv",                               // Scenario validation data file path
            LocalOrbitalFrameFactory::LVLH(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            100.0,                                          // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            3e-3,                                           // Position error GCRF tolerance [m]
            3e-6,                                           // Velocity error GCRF tolerance [m/s]
            1e-9                                            // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 10: Increase thrust to 10N
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_10.0_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            10.0,                                          // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 11: Equatorial orbit
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_0.0_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_0.0_0."
            "0_30.0.csv",                                  // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 12: Thrust Vector on +Z and Equatorial
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_0.0_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_0.0_0.0_1."
            "0_30.0.csv",                                  // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({0.0, 0.0, 1.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 13: Higher altitude orbit (~1000km)
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_7500000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            7e-4,                                          // Position error GCRF tolerance [m]
            8e-7,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        )
    )
);

class OpenSpaceToolkit_Astrodynamics_Propagation_Validation_Thruster_Drag_Exponential
    : public ::testing::TestWithParam<Tuple<
          std::string,
          Shared<const LocalOrbitalFrameFactory>,
          Vector3d,
          Real,
          Real,
          Real,
          Real,
          Real,
          Real,
          Real,
          Real>>
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        this->satelliteGeometry_ = satelliteGeometry;

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

    const NumericalSolver defaultRK4_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const NumericalSolver defaultRKD5_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);
    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    Composite satelliteGeometry_ = Composite::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();
};

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Propagation_Validation_Thruster_Drag_Exponential,
    CrossValidation_ForceModel_Thrust_Drag_Exponential
)
{
    // Setup environment

    // Access the test parameters
    const auto parameters = GetParam();

    const String referenceDataFileName = std::get<0>(parameters);
    Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactory = std::get<1>(parameters);
    const Vector3d localOrbitalFrameThrustVector = std::get<2>(parameters);
    const Real satelliteDryMassReal = std::get<3>(parameters);
    const Real thrustReal = std::get<4>(parameters);
    const Real specificImpulseReal = std::get<5>(parameters);
    const Real crossSectionReal = std::get<6>(parameters);
    const Real dragCoefficientReal = std::get<7>(parameters);

    // Reference data setup
    const Table referenceData = Table::Load(File::Path(Path::Parse(referenceDataFileName)), Table::Format::CSV, true);

    // Initialize reference data arrays
    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceCentralBodyGravityArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceDragArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceTotalAccelerationArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceManeuverAccelerationArrayGCRF = Array<Vector3d>::Empty();

    Array<Vector3d> referencePositionArrayLOF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayLOF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceManeuverDragAccelerationArrayLOF = Array<Vector3d>::Empty();
    Array<double> referenceMassArray = Array<double>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(
            Instant::DateTime(DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC)
        );

        referencePositionArrayGCRF.add(
            Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
        );
        referenceTotalAccelerationArrayGCRF.add(
            Vector3d(referenceRow[13].accessReal(), referenceRow[14].accessReal(), referenceRow[15].accessReal())
        );
        referenceManeuverAccelerationArrayGCRF.add(
            Vector3d(referenceRow[16].accessReal(), referenceRow[17].accessReal(), referenceRow[18].accessReal())
        );

        referencePositionArrayLOF.add(
            Vector3d(referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal())
        );
        referenceVelocityArrayLOF.add(
            Vector3d(referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal())
        );
        referenceManeuverDragAccelerationArrayLOF.add(
            Vector3d(referenceRow[19].accessReal(), referenceRow[20].accessReal(), referenceRow[21].accessReal())
        );

        referenceCentralBodyGravityArrayGCRF.add(
            Vector3d(referenceRow[22].accessReal(), referenceRow[23].accessReal(), referenceRow[24].accessReal())
        );

        referenceDragArrayGCRF.add(
            Vector3d(referenceRow[25].accessReal(), referenceRow[26].accessReal(), referenceRow[27].accessReal())
        );

        referenceMassArray.add(referenceRow[28].accessReal());
    }

    // Local Orbital Frame Direction
    const LocalOrbitalFrameDirection thrustDirection =
        LocalOrbitalFrameDirection(localOrbitalFrameThrustVector, localOrbitalFrameFactory);

    // Coordinates Broker (scenario-independent)
    const Shared<const CoordinatesBroker> coordinatesBrokerSPtr =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker({
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            CoordinatesSubset::Mass(),
            CoordinatesSubset::SurfaceArea(),
            CoordinatesSubset::DragCoefficient(),
        }));

    // Setup initial conditions
    VectorXd initialCoordinates(9);

    initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0],
        propellantMass_.inKilograms() + satelliteDryMassReal, crossSectionReal, dragCoefficientReal;

    const State initialState = {
        instantArray[0],
        initialCoordinates,
        gcrfSPtr_,
        coordinatesBrokerSPtr,
    };

    // Setup satellite system
    PropulsionSystem propulsionSystem = {
        Scalar(thrustReal, PropulsionSystem::thrustSIUnit),
        Scalar(specificImpulseReal, PropulsionSystem::specificImpulseSIUnit),
    };

    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    SatelliteSystem satelliteSystem = {
        Mass::Kilograms(satelliteDryMassReal),
        satelliteGeometry,
        Matrix3d::Identity(),
        crossSectionReal,
        dragCoefficientReal,
        propulsionSystem,
    };

    // Setup validation tolerances
    const Real positionErrorGCRFTolerance = std::get<8>(parameters);
    const Real velocityErrorGCRFTolerance = std::get<9>(parameters);
    const Real MassErrorTolerance = std::get<10>(parameters);

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
    );
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const Shared<ConstantThrust> guidanceLawSPtr = std::make_shared<ConstantThrust>(thrustDirection);
    const Shared<Thruster> thrusterDynamicsSPtr = std::make_shared<Thruster>(satelliteSystem, guidanceLawSPtr);
    const Shared<CentralBodyGravity> centralBodyGravitySPtr = std::make_shared<CentralBodyGravity>(earthSPtr);
    const Shared<AtmosphericDrag> atmosphericDragSPtr = std::make_shared<AtmosphericDrag>(earthSPtr);

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(), centralBodyGravitySPtr, atmosphericDragSPtr, thrusterDynamicsSPtr
    };

    // Setup Propagator model and orbit
    const Propagator propagator = {defaultRK4_, dynamics};

    // Propagate all states with OSTk
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(initialState, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize() - 1; i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].inFrame(gcrfSPtr_).getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].inFrame(gcrfSPtr_).getVelocity();
        const double mass = propagatedStateArray[i].extractCoordinate(CoordinatesSubset::Mass())[0];

        VectorXd OSTkStateCoordinatesGCRF(9);
        OSTkStateCoordinatesGCRF << positionGCRF.accessCoordinates(), velocityGCRF.accessCoordinates(), mass,
            crossSectionReal, dragCoefficientReal;

        const VectorXd maneuverContributionGCRF =
            thrusterDynamicsSPtr->computeContribution(instantArray[i], OSTkStateCoordinatesGCRF, gcrfSPtr_);
        const VectorXd centralBodyGravityContributionGCRF =
            centralBodyGravitySPtr->computeContribution(instantArray[i], positionGCRF.accessCoordinates(), gcrfSPtr_);
        const VectorXd dragContributionGCRF = atmosphericDragSPtr->computeContribution(
            instantArray[i], OSTkStateCoordinatesGCRF, gcrfSPtr_
        );  // Remove mass from state for drag, not anymore
        const VectorXd totalAccelerationGCRF =
            maneuverContributionGCRF.segment(0, 3) + centralBodyGravityContributionGCRF + dragContributionGCRF;

        // LOF Compare
        Shared<const Frame> lofSPtr = localOrbitalFrameFactory->generateFrame(
            instantArray[i], positionGCRF.accessCoordinates(), velocityGCRF.accessCoordinates()
        );
        State lofState = propagatedStateArray[i].inFrame(lofSPtr);

        const Position positionLOF = lofState.getPosition();
        const Velocity velocityLOF = lofState.getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();
        const double maneuverAccelerationContributionErrorGCRF =
            (maneuverContributionGCRF.segment(0, 3) - referenceManeuverAccelerationArrayGCRF[i]).norm();
        const double centralBodyGravityAccelerationContributionErrorGCRF =
            (centralBodyGravityContributionGCRF.segment(0, 3) - referenceCentralBodyGravityArrayGCRF[i])
                .norm();  // Name TBC
        const double dragAccelerationContributionErrorGCRF =
            (dragContributionGCRF.segment(0, 3) - referenceDragArrayGCRF[i]).norm();
        const double positionErrorLOF = (positionLOF.accessCoordinates() - referencePositionArrayLOF[i]).norm();
        const double velocityErrorLOF = (velocityLOF.accessCoordinates() - referencePositionArrayLOF[i]).norm();
        const double totalAccelerationErrorGCRF =
            (totalAccelerationGCRF - referenceTotalAccelerationArrayGCRF[i]).norm();
        const double massError = std::abs(mass - referenceMassArray[i]);

        // Frame verification
        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());
        ASSERT_EQ(*lofSPtr, *positionLOF.accessFrame());
        ASSERT_EQ(*lofSPtr, *velocityLOF.accessFrame());

        // GCRF Errors
        // State
        ASSERT_GT(positionErrorGCRFTolerance, positionErrorGCRF);
        ASSERT_GT(velocityErrorGCRFTolerance, velocityErrorGCRF);
        ASSERT_GT(MassErrorTolerance, massError);

        // Accelerations from dynamics
        ASSERT_GT(1e-8, centralBodyGravityAccelerationContributionErrorGCRF);
        ASSERT_GT(1e-9, dragAccelerationContributionErrorGCRF);
        ASSERT_GT(1e-9, maneuverAccelerationContributionErrorGCRF);
        ASSERT_GT(5e-9, totalAccelerationErrorGCRF);

        // // LOF Errors
        // // State
        ASSERT_GT(1e-15, positionErrorLOF);
        ASSERT_GT(1e-15, velocityErrorLOF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Instant is: " << instantArray[i] << std::endl;
        // // Quaternion quat = gcrfSPtr_->getTransformTo(lofSPtr, instantArray[i]).getOrientation();
        // // std::cout << lofSPtr->getOriginIn(gcrfSPtr_, instantArray[i]) << std::endl;
        // // std::cout << quat << std::endl ;
        // std::cout << "Position OSTk is: " << positionGCRF.accessCoordinates() << "m" << std::endl;
        // std::cout << "Position Orekit is: " << referencePositionArrayGCRF[i] << "m" << std::endl;
        // std::cout << "Velocity OSTk is: " << velocityGCRF.accessCoordinates() << "m/s" << std::endl;
        // std::cout << "Velocity Orekit is: " << referenceVelocityArrayGCRF[i] << "m/s" << std::endl;
        // std::cout << "Position error GCRF is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error GCRF is: " << velocityErrorGCRF << "m/s" << std::endl;
        // std::cout << "Position OSTk LOF is: " << positionLOF.accessCoordinates() << "m" << std::endl;
        // std::cout << "Position Orekit LOF is: " << referencePositionArrayLOF[i] << "m" << std::endl;
        // std::cout << "Velocity OSTk LOF is: " << velocityLOF.accessCoordinates() << "m/s" << std::endl;
        // std::cout << "Velocity Orekit LOF is: " << referenceVelocityArrayLOF[i] << "m/s" << std::endl;
        // std::cout << "Position error LOF is: " << positionErrorLOF << "m" << std::endl;
        // std::cout << "Velocity error LOF is: " << velocityErrorLOF << "m/s" << std::endl;
        // std::cout << "Mass OSTk is: " << mass << "kg" << std::endl;
        // std::cout << "Mass Orekit is: " << referenceMassArray[i] << "kg" << std::endl;
        // std::cout << "Mass error is: " << massError << "kg" << std::endl;
        // std::cout << "Maneuver acceleration error GCRF X is: " << maneuverContributionGCRF[0] -
        // referenceManeuverAccelerationArrayGCRF[i][0] << "m/s^2" << std::endl; std::cout << "Maneuver acceleration
        // error GCRF Y is: " << maneuverContributionGCRF[1] - referenceManeuverAccelerationArrayGCRF[i][1] <<
        // "m/s^2"
        // << std::endl; std::cout << "Maneuver acceleration error GCRF Z is: " << maneuverContributionGCRF[2] -
        // referenceManeuverAccelerationArrayGCRF[i][2] << "m/s^2" << std::endl; std::cout << "Maneuver acceleration
        // error GCRF is: " << maneuverAccelerationContributionErrorGCRF << "m/s^2" << std::endl;  // Do it in
        // percentage std::cout << "Central Body Gravity acceleration error GCRF is: " <<
        // centralBodyGravityAccelerationContributionErrorGCRF << "m/s^2" << std::endl;  // Do it in percentage
        // std::cout << "Drag acceleration error GCRF is: " << dragAccelerationContributionErrorGCRF << "m/s^2" <<
        // std::endl;  // Do it in percentage std::cout << "Total acceleration (centralbody + maneuver + drag) error
        // GCRF is: " << totalAccelerationErrorGCRF << "m/s^2" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield); std::cout <<
        // "**************************************"
        // << std::endl;
    }
}

// TBI: Agree on a format to version Orekit validation files
INSTANTIATE_TEST_SUITE_P(
    CrossValidation_ForceModel_Thrust_Drag_Exponential,
    OpenSpaceToolkit_Astrodynamics_Propagation_Validation_Thruster_Drag_Exponential,
    ::testing::Values(
        // Test Case 0
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 1: Start date in 2021
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2021-12-23T11-23-21.235_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 2: QSW LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_QSW_0.0_"
            "1.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            LocalOrbitalFrameFactory::QSW(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({0.0, 1.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 3: Increase spacecraft mass to 1000kg
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_1015.0_0.1_1500.0_3600.0_VNC_1."
            "0_0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",  // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),   // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            1000.0,                                         // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            1.0,                                            // Cross section [m^2]
            2.1,                                            // Drag coefficient
            1e-3,                                           // Position error GCRF tolerance [m]
            1e-6,                                           // Velocity error GCRF tolerance [m/s]
            1e-9                                            // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 4: Increase maneuver duration to 2h
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_7200.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 5: Increase maneuver duration to 4h
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_14400.0_VNC_1."
            "0_0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",  // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),   // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            100.0,                                          // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            1.0,                                            // Cross section [m^2]
            2.1,                                            // Drag coefficient
            3e-3,                                           // Position error GCRF tolerance [m]
            3e-6,                                           // Velocity error GCRF tolerance [m/s]
            1e-9                                            // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 6: Increase thrust to 1N, lowering specific impulse to 150.0
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_1.0_150.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            1.0,                                           // Thrust [N]
            150.0,                                         // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 7: Equatorial orbit
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_0.0_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 8: Increase spacecraft cross section
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_25.0_2.1_TRUE.csv",  // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            25.0,                                          // Cross section [m^2]
            2.1,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 9: Increase spacecraft drag coefficient and cross section
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_4.2_TRUE.csv",   // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            4.2,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        ),
        // Test Case 10: Higher initial altitude (~800 km) and increase cross section
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7300000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient
            1e-3,                                          // Position error GCRF tolerance [m]
            1e-6,                                          // Velocity error GCRF tolerance [m/s]
            1e-9                                           // Mass error tolerance [kg]
            // Acceleration errors
        )
    )
);

class OpenSpaceToolkit_Astrodynamics_Propagation_Validation_NRLMSISE00_Data_Success
    : public OpenSpaceToolkit_Astrodynamics_Propagation_Validation,
      public ::testing::WithParamInterface<Tuple<Instant, Instant, Duration, std::string>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Input_Data_Spans_Valid,
    OpenSpaceToolkit_Astrodynamics_Propagation_Validation_NRLMSISE00_Data_Success,
    testing::Values(std::make_tuple(
        Instant::Now() - Duration::Weeks(52 * 5),
        Instant::Now() - Duration::Weeks(52 * 4.9),
        Duration::Days(7),
        "NRLMSISE Past Data limit pass"
    )
                    // [TBI]: This test case fails sporadically because the actual data files are never updated based on
                    // age, meaning they can get stale.

                    // std::make_tuple(
                    //     Instant::Now() + Duration::Weeks(52 * 0.9),
                    //     Instant::Now() + Duration::Weeks(52 * 1.0),
                    //     Duration::Days(7),
                    //     "IERS Finals2000A Future Data limit pass"
                    // )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Propagation_Validation_NRLMSISE00_Data_Success, EdgeCases_Input_Data_Horizon)
{
    // long-horizon, low granularity propagation to test input data availability

    // currently limited by:
    // -5 years in the past due to CSSI space weather data when computing drag
    // +1 year in the future due to finals2000a prediction span when computing polar motion for frame conversions

    const auto parameters = GetParam();

    const Instant startInstant = std::get<0>(parameters);
    const Instant endInstant = std::get<1>(parameters);
    const Duration step = std::get<2>(parameters);

    const Real mass = satelliteDryMass_.inKilograms();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

    VectorXd initialStateVector(9);
    initialStateVector << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, mass, surfaceArea,
        dragCoefficient;

    // Current state and instant setup
    const State state = {startInstant, initialStateVector, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

    const NumericalSolver RK4 = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        step.inSeconds(),
        1.0e-15,
        1.0e-15,
    };

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM2008),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::NRLMSISE00)
    );

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    // Setup Propagator model and orbit
    const Propagator propagator = {
        RK4,
        dynamics,
    };

    // Propagate all states
    EXPECT_NO_THROW(propagator.calculateStateAt(state, endInstant));
}

class OpenSpaceToolkit_Astrodynamics_Propagation_Validation_NRLMSISE00_Data_Failure
    : public OpenSpaceToolkit_Astrodynamics_Propagation_Validation,
      public ::testing::WithParamInterface<Tuple<Instant, Instant, Duration, std::string>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Input_Data_Spans_Invalid,
    OpenSpaceToolkit_Astrodynamics_Propagation_Validation_NRLMSISE00_Data_Failure,
    testing::Values(
        std::make_tuple(
            Instant::Now() - Duration::Weeks(52 * 6.2),
            Instant::Now() - Duration::Weeks(52 * 6.3),
            Duration::Days(7),
            "NRLMSISE Past Data limit fail"
        ),
        std::make_tuple(
            Instant::Now() + Duration::Weeks(52 * 1.0),
            Instant::Now() + Duration::Weeks(52 * 1.1),
            Duration::Days(7),
            "IERS Finals2000A Future Data limit fail"
        )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Propagation_Validation_NRLMSISE00_Data_Failure, EdgeCases_Input_Data_Horizon)
{
    // long-horizon, low granularity propagation to test input data availability

    // currently limited by:
    // -5 years in the past due to CSSI space weather data when computing drag
    // +1 year in the future due to finals2000a prediction span when computing polar motion for frame conversions

    auto parameters = GetParam();

    Instant startInstant = std::get<0>(parameters);
    Instant endInstant = std::get<1>(parameters);
    Duration step = std::get<2>(parameters);

    const Real mass = satelliteDryMass_.inKilograms();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

    VectorXd initialStateVector(9);
    initialStateVector << 7000000.0, 0.0, 0.0, 0.0, 5335.865450622126, 5335.865450622126, mass, surfaceArea,
        dragCoefficient;

    // Current state and instant setup
    const State state = {startInstant, initialStateVector, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

    const NumericalSolver RK4 = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        step.inSeconds(),
        1.0e-15,
        1.0e-15,
    };

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM2008),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::NRLMSISE00)
    );

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    // Setup Propagator model and orbit
    const Propagator propagator = {RK4, dynamics};

    // Propagate all states
    EXPECT_THROW(propagator.calculateStateAt(state, endInstant), ostk::core::error::RuntimeError);
}

class OpenSpaceToolkit_Astrodynamics_Propagation_Validation_QLaw
    : public OpenSpaceToolkit_Astrodynamics_Propagation_Validation,
      public ::testing::WithParamInterface<Tuple<QLaw::GradientStrategy>>
{
};

INSTANTIATE_TEST_SUITE_P(
    QLaw_GradientStrategy,
    OpenSpaceToolkit_Astrodynamics_Propagation_Validation_QLaw,
    testing::Values(
        std::make_tuple(QLaw::GradientStrategy::FiniteDifference), std::make_tuple(QLaw::GradientStrategy::Analytical)
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Propagation_Validation_QLaw, QLaw_Paper_Case_A)
{
    const auto testParameters = GetParam();
    const QLaw::GradientStrategy gradientStrategy = std::get<0>(testParameters);

    const COE targetCOE = {
        Length::Meters(42000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const QLaw::Parameters parameters = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
            {COE::Element::Eccentricity, {1.0, 1e-3}},
        },
        3,
        4,
        2,
        0.01,
        100,
        1.0,
        Length::Kilometers(6578.0)
    };

    const Derived gravitationalParameter =
        Derived(398600.49 * 1e9, EarthGravitationalModel::EGM2008.gravitationalParameter_.getUnit());

    const Shared<QLaw> qlaw =
        std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

    const Mass mass = Mass::Kilograms(50.0);
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    const PropulsionSystem propulsionSystem = {
        Scalar(1.0, PropulsionSystem::thrustSIUnit),
        Scalar(3100.0, PropulsionSystem::specificImpulseSIUnit),
    };

    const SatelliteSystem satelliteSystem = {
        mass,
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.1,
        propulsionSystem,
    };

    const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
    const Shared<CentralBodyGravity> centralBodyGravity =
        std::make_shared<CentralBodyGravity>(CentralBodyGravity(std::make_shared<Earth>(Earth::Spherical())));
    const Shared<PositionDerivative> positionDerivative = std::make_shared<PositionDerivative>(PositionDerivative());
    const Array<Shared<Dynamics>> dynamics = {
        thruster,
        centralBodyGravity,
        positionDerivative,
    };

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        120.0,
        1e-12,
        1e-12,
    };

    const Propagator propagator = {numericalSolver, dynamics};

    const COE currentCOE = {
        Length::Meters(7000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const COE::CartesianState cartesianState = currentCOE.getCartesianState(gravitationalParameter, Frame::GCRF());

    VectorXd coordinates(7);
    coordinates << cartesianState.first.getCoordinates(), cartesianState.second.getCoordinates(), 300.0;

    const State initialState = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinatesSubset::Mass()}
    };

    const State state =
        propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Days(14.86522));

    const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

    EXPECT_TRUE(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()) < 60000.0);
}

TEST_P(OpenSpaceToolkit_Astrodynamics_Propagation_Validation_QLaw, QLaw_Paper_Case_E)
{
    GTEST_SKIP() << "Skipping test as it does not produce comparable results between Release + Debug.";

    const auto testParameters = GetParam();
    const QLaw::GradientStrategy gradientStrategy = std::get<0>(testParameters);

    const COE targetCOE = {
        Length::Meters(26500.0e3),
        0.7,
        Angle::Degrees(116.0),
        Angle::Degrees(180.0),
        Angle::Degrees(270.0),
        Angle::Degrees(0.0),
    };

    const QLaw::Parameters parameters = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
            {COE::Element::Eccentricity, {1.0, 1e-3}},
            {COE::Element::Inclination, {1.0, 1e-4}},
            {COE::Element::Raan, {1.0, 1e-4}},
            {COE::Element::Aop, {1.0, 1e-4}},
        },
    };

    const Derived gravitationalParameter =
        Derived(398600.49 * 1e9, EarthGravitationalModel::EGM2008.gravitationalParameter_.getUnit());

    const Shared<QLaw> qlaw =
        std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

    const Mass mass = Mass::Kilograms(50.0);
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    const PropulsionSystem propulsionSystem = {
        Scalar(2.0, PropulsionSystem::thrustSIUnit),
        Scalar(2000.0, PropulsionSystem::specificImpulseSIUnit),
    };

    const SatelliteSystem satelliteSystem = {
        mass,
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.1,
        propulsionSystem,
    };

    const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
    const Shared<CentralBodyGravity> centralBodyGravity =
        std::make_shared<CentralBodyGravity>(CentralBodyGravity(std::make_shared<Earth>(Earth::Spherical())));
    const Shared<PositionDerivative> positionDerivative = std::make_shared<PositionDerivative>(PositionDerivative());
    const Array<Shared<Dynamics>> dynamics = {
        thruster,
        centralBodyGravity,
        positionDerivative,
    };

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        5.0,
        1e-12,
        1e-12,
    };

    const Propagator propagator = {numericalSolver, dynamics};

    const COE currentCOE = {
        Length::Meters(24505.9e3),
        0.725,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const COE::CartesianState cartesianState = currentCOE.getCartesianState(gravitationalParameter, Frame::GCRF());

    VectorXd coordinates(7);
    coordinates << cartesianState.first.getCoordinates(), cartesianState.second.getCoordinates(), 2000.0;

    const State initialState = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinatesSubset::Mass()}
    };

    const State state =
        propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Days(72.49716435185185187));

    const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

    EXPECT_TRUE(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()) < 50000.0);

    // TBI: These don't close yes, have to investigate why
    // EXPECT_TRUE(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()) < 1e-2);
    // EXPECT_TRUE(std::abs(endCOE.getInclination().inDegrees() - targetCOE.getInclination().inDegrees()) < 1.0);
    // EXPECT_TRUE(std::abs(endCOE.getRaan().inDegrees() - targetCOE.getRaan().inDegrees()) < 1.0);
    // EXPECT_TRUE(std::abs(endCOE.getAop().inDegrees() - targetCOE.getAop().inDegrees()) < 1.0);
}

TEST_P(OpenSpaceToolkit_Astrodynamics_Propagation_Validation_QLaw, SSO_targeting)
{
    const auto testParameters = GetParam();
    const QLaw::GradientStrategy gradientStrategy = std::get<0>(testParameters);

    const Orbit orbit = Orbit::SunSynchronous(
        Instant::J2000(), Length::Meters(585.0e3), Time(10, 0, 0), std::make_shared<Earth>(Earth::Default())
    );

    const Derived gravitationalParameter =
        Derived(398600.49 * 1e9, EarthGravitationalModel::EGM2008.gravitationalParameter_.getUnit());

    const State currentState = orbit.getStateAt(Instant::J2000());
    const COE currentCOE =
        COE::Cartesian({currentState.getPosition(), currentState.getVelocity()}, gravitationalParameter);

    const Mass mass = Mass::Kilograms(200.0);
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    const PropulsionSystem propulsionSystem = {
        Scalar(1.0, PropulsionSystem::thrustSIUnit),
        Scalar(1000.0, PropulsionSystem::specificImpulseSIUnit),
    };

    const SatelliteSystem satelliteSystem = {
        mass,
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.1,
        propulsionSystem,
    };

    const Shared<CentralBodyGravity> centralBodyGravity =
        std::make_shared<CentralBodyGravity>(CentralBodyGravity(std::make_shared<Earth>(Earth::Spherical())));
    const Shared<PositionDerivative> positionDerivative = std::make_shared<PositionDerivative>(PositionDerivative());

    const NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        3.0,
        1e-12,
        1e-12,
    };

    VectorXd coordinates(7);
    coordinates << currentState.getCoordinates(), 214.0;

    const State initialState = {
        Instant::DateTime(DateTime(2024, 10, 1, 0, 0, 0), Scale::UTC),
        coordinates,
        Frame::GCRF(),
        {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinatesSubset::Mass()}
    };

    // SMA Targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis() + Length::Meters(5000.0),
            currentCOE.getEccentricity(),
            currentCOE.getInclination(),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, {1.0, 50.0}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(15.0));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()), 100.0);
    }

    // Eccentricity targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis(),
            currentCOE.getEccentricity() + 1e-3,
            currentCOE.getInclination(),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::Eccentricity, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(21.0));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()), 1e-4);
    }

    // Inclination targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis(),
            currentCOE.getEccentricity(),
            currentCOE.getInclination() + Angle::Degrees(1e-2),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::Inclination, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Hours(1.5));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getInclination().inRadians() - targetCOE.getInclination().inRadians()), 1e-4);
    }

    // // Right Ascension of the Ascending Node targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis(),
            currentCOE.getEccentricity(),
            currentCOE.getInclination(),
            currentCOE.getRaan() - Angle::Degrees(1e-1),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::Raan, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Hours(1.5));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getRaan().inRadians() - targetCOE.getRaan().inRadians()), 1e-4);
    }

    // Semi-Major Axis + Eccentricity targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis() + Length::Meters(5000.0),
            currentCOE.getEccentricity() + 1e-3,
            currentCOE.getInclination(),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(40.0));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()), 100.0);
        EXPECT_LT(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()), 1e-4);
    }

    // Semi-Major Axis + Eccentricity + Inclination targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis() + Length::Meters(5000.0),
            currentCOE.getEccentricity() + 1e-3,
            currentCOE.getInclination() + Angle::Degrees(1e-1),
            currentCOE.getRaan(),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-3}},
                {COE::Element::Inclination, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(130.0));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()), 100.0);
        EXPECT_LT(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()), 1e-3);
        EXPECT_LT(std::abs(endCOE.getInclination().inRadians() - targetCOE.getInclination().inRadians()), 1e-4);
    }

    // Semi-Major Axis + Eccentricity + Inclination + Right Ascension of Ascending Node targeting
    {
        const COE targetCOE = {
            currentCOE.getSemiMajorAxis() + Length::Meters(5000.0),
            currentCOE.getEccentricity() + 1e-3,
            currentCOE.getInclination() + Angle::Degrees(1e-1),
            currentCOE.getRaan() - Angle::Degrees(1e-1),
            currentCOE.getAop(),
            currentCOE.getTrueAnomaly(),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-3}},
                {COE::Element::Inclination, {1.0, 1e-4}},
                {COE::Element::Raan, {1.0, 1e-4}},
            },
        };

        const Shared<QLaw> qlaw =
            std::make_shared<QLaw>(QLaw(targetCOE, gravitationalParameter, parameters, gradientStrategy));

        const Shared<Thruster> thruster = std::make_shared<Thruster>(Thruster(satelliteSystem, qlaw));
        const Array<Shared<Dynamics>> dynamics = {
            thruster,
            centralBodyGravity,
            positionDerivative,
        };

        const Propagator propagator = {numericalSolver, dynamics};

        const State state =
            propagator.calculateStateAt(initialState, initialState.accessInstant() + Duration::Minutes(145.5));

        const COE endCOE = COE::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter);

        EXPECT_LT(std::abs(endCOE.getSemiMajorAxis().inMeters() - targetCOE.getSemiMajorAxis().inMeters()), 100.0);
        EXPECT_LT(std::abs(endCOE.getEccentricity() - targetCOE.getEccentricity()), 1e-3);
        EXPECT_LT(std::abs(endCOE.getInclination().inRadians() - targetCOE.getInclination().inRadians()), 1e-4);
        EXPECT_LT(std::abs(endCOE.getRaan().inRadians() - targetCOE.getRaan().inRadians()), 1e-4);
    }
}
