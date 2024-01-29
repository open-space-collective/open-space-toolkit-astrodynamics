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

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
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

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::data::Scalar;
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
using ostk::physics::units::Mass;

using ostk::astro::Dynamics;
using ostk::astro::dynamics::AtmosphericDrag;
using ostk::astro::dynamics::CentralBodyGravity;
using ostk::astro::dynamics::PositionDerivative;
using ostk::astro::dynamics::Tabulated;
using ostk::astro::dynamics::Thruster;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::SatelliteSystemBuilder;
using ostk::astro::guidancelaw::ConstantThrust;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;
using ostk::astro::trajectory::LocalOrbitalFrameDirection;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation : public ::testing::Test
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation, ForceModel_TwoBody)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation, ForceModel_EGM2008)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation, ForceModel_WGS84EGM96)
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation, ForceModel_EGM84)
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
}

// TBI: Specific class for parameterized tests on thruster dynamics, could inherit from base fixture for common
class OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster
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

    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);
    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    Composite satelliteGeometry_ = Composite::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();
};

TEST_P(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster, ForceModel_Thrust)
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
    ForceModel_Thrust,
    OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster,
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

class OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster_Drag_Exponential
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

    const Mass satelliteDryMass_ = Mass(100.0, Mass::Unit::Kilogram);
    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();
    Composite satelliteGeometry_ = Composite::Undefined();
    Shared<Celestial> earthSpherical_ = nullptr;
    Propagator defaultPropagator_ = Propagator::Undefined();
};

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster_Drag_Exponential,
    ForceModel_Thrust_Drag_Exponential
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
    ForceModel_Thrust_Drag_Exponential,
    OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster_Drag_Exponential,
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
