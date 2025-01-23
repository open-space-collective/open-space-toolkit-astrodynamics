/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
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
using ostk::core::container::Table;
using ostk::core::container::Tuple;
using ostk::core::filesystem::Directory;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
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
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

// TBM: All of these validation tests currently do not fit within the OSTk validation framework, but the intention is to
// move them there in the future.
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
    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation, ForceModel_TwoBody)
{
    // Two body vs STK in GCRF and ITRF
    {
        // Reference data setup
        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/STK_TwoBody_2hr_run.csv"
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
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/"
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
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/"
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
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/"
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

class OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster : public ::testing::TestWithParam<Tuple<
                                                                               std::string,
                                                                               bool,
                                                                               Shared<const LocalOrbitalFrameFactory>,
                                                                               Vector3d,
                                                                               Real,
                                                                               Real,
                                                                               Real,
                                                                               Real,
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
        this->satelliteGeometry_ = Composite(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        // Dynamics without atmosphere
        this->earthSpherical_ = std::make_shared<Celestial>(Earth::Spherical());
        this->centralBodyGravitySPtr_ = std::make_shared<CentralBodyGravity>(earthSpherical_);
        this->defaultDynamicsWithoutAtmosphere_ = {
            std::make_shared<PositionDerivative>(),
            centralBodyGravitySPtr_,
        };

        // Dynamics with atmosphere
        this->earthSphericalWithAtmosphere_ = std::make_shared<Celestial>(Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
        ));
        this->atmosphericDragSPtr_ = std::make_shared<AtmosphericDrag>(earthSphericalWithAtmosphere_);
        this->defaultDynamicsWithAtmosphere_ = {
            std::make_shared<PositionDerivative>(),
            centralBodyGravitySPtr_,
            atmosphericDragSPtr_,
        };
    }

    const NumericalSolver defaultNumericalSolver_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        5.0,
        1.0e-12,
        1.0e-12,
    };

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    const Mass propellantMass_ = Mass(15.0, Mass::Unit::Kilogram);
    Composite satelliteGeometry_ = Composite::Undefined();

    Array<Shared<Dynamics>> defaultDynamics_ = Array<Shared<Dynamics>>::Empty();

    Array<Shared<Dynamics>> defaultDynamicsWithoutAtmosphere_ = Array<Shared<Dynamics>>::Empty();
    Shared<Celestial> earthSpherical_ = nullptr;
    Shared<CentralBodyGravity> centralBodyGravitySPtr_ = nullptr;

    Array<Shared<Dynamics>> defaultDynamicsWithAtmosphere_ = Array<Shared<Dynamics>>::Empty();
    Shared<Celestial> earthSphericalWithAtmosphere_ = nullptr;
    Shared<AtmosphericDrag> atmosphericDragSPtr_ = nullptr;

    Shared<const CoordinateBroker> coordinatesBrokerSPtr_ = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));
};

TEST_P(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster, ForceModel_Thrust)
{
    // Access the test parameters
    const auto parameters = GetParam();

    const String param_referenceDataFileName = std::get<0>(parameters);
    const bool param_withAtmosphere = std::get<1>(parameters);
    Shared<const LocalOrbitalFrameFactory> param_localOrbitalFrameFactory = std::get<2>(parameters);
    const Vector3d param_localOrbitalFrameThrustVector = std::get<3>(parameters);
    const Real param_satelliteDryMassReal = std::get<4>(parameters);
    const Real param_thrustReal = std::get<5>(parameters);
    const Real param_specificImpulseReal = std::get<6>(parameters);
    const Real param_crossSectionReal = std::get<7>(parameters);
    const Real param_dragCoeffReal = std::get<8>(parameters);
    const Real param_positionErrorGCRFTolerance = std::get<9>(parameters);
    const Real param_velocityErrorGCRFTolerance = std::get<10>(parameters);
    const Real param_accelerationErrorGCRFTolerance = std::get<11>(parameters);
    const Real param_massErrorTolerance = std::get<12>(parameters);
    const Real param_positionErrorLOFTolerance = std::get<13>(parameters);
    const Real param_velocityErrorLOFTolerance = std::get<14>(parameters);
    const Real param_accelerationErrorLOFTolerance = std::get<15>(parameters);

    // Initialize reference data arrays
    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceManeuverAccelerationArrayGCRF = Array<Vector3d>::Empty();

    Array<Vector3d> referencePositionArrayLOF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayLOF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceManeuverAccelerationArrayLOF = Array<Vector3d>::Empty();
    Array<double> referenceMassArray = Array<double>::Empty();

    for (const auto& referenceRow :
         Table::Load(File::Path(Path::Parse(param_referenceDataFileName)), Table::Format::CSV, true))
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
        referenceManeuverAccelerationArrayGCRF.add(
            Vector3d(referenceRow[16].accessReal(), referenceRow[17].accessReal(), referenceRow[18].accessReal())
        );

        referencePositionArrayLOF.add(
            Vector3d(referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal())
        );
        referenceVelocityArrayLOF.add(
            Vector3d(referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal())
        );

        if (!param_withAtmosphere)  // Different indices for the mass in the with and without atmosphere csv files
        {
            referenceManeuverAccelerationArrayLOF.add(
                Vector3d(referenceRow[19].accessReal(), referenceRow[20].accessReal(), referenceRow[21].accessReal())
            );
            referenceMassArray.add(referenceRow[22].accessReal());
        }
        else
        {
            referenceManeuverAccelerationArrayLOF.add(Vector3d::Zero());
            referenceMassArray.add(referenceRow[28].accessReal());
        }
    }

    // Setup initial state
    VectorXd initialCoordinates(7);
    initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0],
        propellantMass_.inKilograms() + param_satelliteDryMassReal;

    // If cross validation with atmospheric drag is being included
    if (param_withAtmosphere)
    {
        initialCoordinates.resize(9);
        initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0],
            propellantMass_.inKilograms() + param_satelliteDryMassReal, param_crossSectionReal, param_dragCoeffReal;

        coordinatesBrokerSPtr_ = std::make_shared<CoordinateBroker>(CoordinateBroker({
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
            CoordinateSubset::Mass(),
            CoordinateSubset::SurfaceArea(),
            CoordinateSubset::DragCoefficient(),
        }));

        defaultDynamics_ = defaultDynamicsWithAtmosphere_;
    }
    else
    {
        defaultDynamics_ = defaultDynamicsWithoutAtmosphere_;
    }

    const State initialState = {
        instantArray[0],
        initialCoordinates,
        gcrfSPtr_,
        coordinatesBrokerSPtr_,
    };

    // Setup satellite system
    PropulsionSystem propulsionSystem = {param_thrustReal, param_specificImpulseReal};
    SatelliteSystem satelliteSystem = {
        Mass::Kilograms(param_satelliteDryMassReal),
        satelliteGeometry_,
        Matrix3d::Identity(),
        param_crossSectionReal,
        param_dragCoeffReal,
        propulsionSystem,
    };

    // Create dynamics with thruster
    const LocalOrbitalFrameDirection thrustDirection =
        LocalOrbitalFrameDirection(param_localOrbitalFrameThrustVector, param_localOrbitalFrameFactory);
    Shared<ConstantThrust> constantThrustSPtr = std::make_shared<ConstantThrust>(thrustDirection);

    Array<Shared<Dynamics>> dynamicsWithThruster = defaultDynamics_;
    Shared<Thruster> thrusterDynamicsSPtr = std::make_shared<Thruster>(satelliteSystem, constantThrustSPtr);
    dynamicsWithThruster.add(thrusterDynamicsSPtr);
    Propagator propagatorWithThruster = {defaultNumericalSolver_, dynamicsWithThruster};

    // Propagate all states with thruster to be able to extract contributions for tabulated dynamics
    const Array<State> propagatedStateArray_Thruster =
        propagatorWithThruster.calculateStatesAt(initialState, instantArray);
    MatrixXd thrusterContributionsGCRF(propagatedStateArray_Thruster.getSize(), 4);
    for (Size i = 0; i < propagatedStateArray_Thruster.getSize(); i++)
    {
        const VectorXd thrusterContributionGCRF =
            dynamicsWithThruster[dynamicsWithThruster.getSize() - 1]->computeContribution(
                propagatedStateArray_Thruster[i].accessInstant(),
                propagatedStateArray_Thruster[i].accessCoordinates(),
                gcrfSPtr_
            );
        thrusterContributionsGCRF.row(i) = thrusterContributionGCRF;
    }

    // Create tabulated dynamics
    const TabulatedDynamics tabulated = {
        instantArray,
        thrusterContributionsGCRF,
        {CartesianVelocity::Default(), CoordinateSubset::Mass()},
        gcrfSPtr_,
    };

    // Create dynamics with tabulated and propagator with tabulated
    Array<Shared<Dynamics>> dynamicsWithTabulated = defaultDynamics_;
    Shared<TabulatedDynamics> tabulatedSPtr = std::make_shared<TabulatedDynamics>(tabulated);
    dynamicsWithTabulated.add(tabulatedSPtr);
    Propagator propagatorWithTabulated = {defaultNumericalSolver_, dynamicsWithTabulated};
    const Array<State> propagatedStateArray_Tabulated =
        propagatorWithTabulated.calculateStatesAt(initialState, instantArray);

    // Create maneuver from tabulated and propagator with maneuver
    Array<State> maneuverStateArray = Array<State>::Empty();
    maneuverStateArray.reserve(instantArray.getSize());

    for (Size i = 0; i < instantArray.getSize(); i++)
    {
        const Vector3d accelerationCoordinates = thrusterContributionsGCRF.row(i).head(3);
        const VectorXd positionVelocityCoordinates = propagatedStateArray_Tabulated[i].extractCoordinates(
            {Maneuver::RequiredCoordinateSubsets[0], Maneuver::RequiredCoordinateSubsets[1]}
        );
        const Real massFlowRate = thrusterContributionsGCRF(i, 3);

        VectorXd coordinates(10);
        coordinates << positionVelocityCoordinates, accelerationCoordinates, massFlowRate;

        maneuverStateArray.add({
            instantArray[i],
            coordinates,
            gcrfSPtr_,
            Maneuver::RequiredCoordinateSubsets,
        });
    }

    const Maneuver maneuver = Maneuver(maneuverStateArray);
    Propagator maneuverPropagator = {defaultNumericalSolver_, defaultDynamics_, {maneuver}};
    const Array<State> propagatedStateArray_Maneuver = maneuverPropagator.calculateStatesAt(initialState, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize() - 1; i++)
    {
        // Get GCRF Position
        const Position positionGCRF_Thruster = propagatedStateArray_Thruster[i].inFrame(gcrfSPtr_).getPosition();
        const Position positionGCRF_Tabulated = propagatedStateArray_Tabulated[i].inFrame(gcrfSPtr_).getPosition();
        const Position positionGCRF_Maneuver = propagatedStateArray_Maneuver[i].inFrame(gcrfSPtr_).getPosition();

        // Get GCRF Position Error
        const double positionErrorGCRF_Thruster =
            (positionGCRF_Thruster.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double positionErrorGCRF_Tabulated =
            (positionGCRF_Tabulated.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double positionErrorGCRF_Maneuver =
            (positionGCRF_Maneuver.accessCoordinates() - referencePositionArrayGCRF[i]).norm();

        // Get GCRF Velocity
        const Velocity velocityGCRF_Thruster = propagatedStateArray_Thruster[i].inFrame(gcrfSPtr_).getVelocity();
        const Velocity velocityGCRF_Tabulated = propagatedStateArray_Tabulated[i].inFrame(gcrfSPtr_).getVelocity();
        const Velocity velocityGCRF_Maneuver = propagatedStateArray_Maneuver[i].inFrame(gcrfSPtr_).getVelocity();

        // Get GCRF Velocity Error
        const double velocityErrorGCRF_Thruster =
            (velocityGCRF_Thruster.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();
        const double velocityErrorGCRF_Tabulated =
            (velocityGCRF_Tabulated.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();
        const double velocityErrorGCRF_Maneuver =
            (velocityGCRF_Maneuver.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        // Get Mass
        const double mass_Thruster = propagatedStateArray_Thruster[i].extractCoordinate(CoordinateSubset::Mass())[0];
        const double mass_Tabulated = propagatedStateArray_Tabulated[i].extractCoordinate(CoordinateSubset::Mass())[0];
        const double mass_Maneuver = propagatedStateArray_Maneuver[i].extractCoordinate(CoordinateSubset::Mass())[0];

        // Get Mass Error
        const double massError_Thruster = std::abs(mass_Thruster - referenceMassArray[i]);
        const double massError_Tabulated = std::abs(mass_Tabulated - referenceMassArray[i]);
        const double massError_Maneuver = std::abs(mass_Maneuver - referenceMassArray[i]);

        // Put position, velocity, and mass together into a state
        VectorXd OSTkStateCoordinatesGCRF_Thruster(7);
        OSTkStateCoordinatesGCRF_Thruster << positionGCRF_Thruster.accessCoordinates(),
            velocityGCRF_Thruster.accessCoordinates(), mass_Thruster;
        VectorXd OSTkStateCoordinatesGCRF_Tabulated(7);
        OSTkStateCoordinatesGCRF_Tabulated << positionGCRF_Tabulated.accessCoordinates(),
            velocityGCRF_Tabulated.accessCoordinates(), mass_Tabulated;
        VectorXd OSTkStateCoordinatesGCRF_Maneuver(7);
        OSTkStateCoordinatesGCRF_Maneuver << positionGCRF_Maneuver.accessCoordinates(),
            velocityGCRF_Maneuver.accessCoordinates(), mass_Maneuver;

        // Get GCRF Acceleration
        const VectorXd maneuverContributionGCRF_Thruster =
            thrusterDynamicsSPtr->computeContribution(instantArray[i], OSTkStateCoordinatesGCRF_Thruster, gcrfSPtr_);
        const VectorXd maneuverContributionGCRF_Tabulated =
            tabulatedSPtr->computeContribution(instantArray[i], OSTkStateCoordinatesGCRF_Tabulated, gcrfSPtr_);
        const VectorXd maneuverAccelerationGCRF_Maneuver = maneuver.toTabulatedDynamics(gcrfSPtr_)->computeContribution(
            instantArray[i], OSTkStateCoordinatesGCRF_Maneuver, gcrfSPtr_
        );

        // Get GCRF Acceleration Error
        const double maneuverAccelerationErrorGCRF_Thruster =
            (maneuverContributionGCRF_Thruster.segment(0, 3) - referenceManeuverAccelerationArrayGCRF[i]).norm();
        const double maneuverAccelerationErrorGCRF_Tabulated =
            (maneuverContributionGCRF_Tabulated.segment(0, 3) - referenceManeuverAccelerationArrayGCRF[i]).norm();
        const double maneuverAccelerationErrorGCRF_Maneuver =
            (maneuverAccelerationGCRF_Maneuver.segment(0, 3) - referenceManeuverAccelerationArrayGCRF[i]).norm();

        // Get LOF
        Shared<const Frame> lofSPtr = param_localOrbitalFrameFactory->generateFrame(
            State(instantArray[i], positionGCRF_Thruster, velocityGCRF_Thruster)
        );

        // Get LOF State
        State lofState_Thruster = propagatedStateArray_Thruster[i].inFrame(lofSPtr);
        State lofState_Tabulated = propagatedStateArray_Tabulated[i].inFrame(lofSPtr);
        State lofState_Maneuver = propagatedStateArray_Maneuver[i].inFrame(lofSPtr);

        // Get LOF Position Error
        const double positionErrorLOF_Thruster =
            (lofState_Thruster.getPosition().accessCoordinates() - referencePositionArrayLOF[i]).norm();
        const double positionErrorLOF_Tabulated =
            (lofState_Tabulated.getPosition().accessCoordinates() - referencePositionArrayLOF[i]).norm();
        const double positionErrorLOF_Maneuver =
            (lofState_Maneuver.getPosition().accessCoordinates() - referencePositionArrayLOF[i]).norm();

        // Get LOF Velocity Error
        const double velocityErrorLOF_Thruster =
            (lofState_Thruster.getVelocity().accessCoordinates() - referencePositionArrayLOF[i]).norm();
        const double velocityErrorLOF_Tabulated =
            (lofState_Tabulated.getVelocity().accessCoordinates() - referencePositionArrayLOF[i]).norm();
        const double velocityErrorLOF_Maneuver =
            (lofState_Maneuver.getVelocity().accessCoordinates() - referencePositionArrayLOF[i]).norm();

        // Get LOF Acceleration
        const VectorXd maneuverContributionLOF_Thruster =
            thrusterDynamicsSPtr->computeContribution(instantArray[i], OSTkStateCoordinatesGCRF_Thruster, lofSPtr);
        // TBM: implement TabulatedDynamics frame conversion for computeContributions
        // const VectorXd maneuverContributionLOF_Tabulated =
        //     tabulatedSPtr->computeContribution(instantArray[i], OSTkStateCoordinatesGCRF_Tabulated, lofSPtr);
        // const VectorXd maneuverContributionLOF_Maneuver =
        // maneuver.toTabulatedDynamics(gcrfSPtr_)->computeContribution(
        //     instantArray[i], OSTkStateCoordinatesGCRF_Maneuver, lofSPtr
        // );

        // Get LOF Acceleration Error
        const double maneuverAccelerationErrorLOF_Thruster =
            (maneuverContributionLOF_Thruster.segment(0, 3) - referenceManeuverAccelerationArrayLOF[i]).norm();
        // TBM: implement TabulatedDynamics frame conversion for computeContributions
        // const double maneuverAccelerationErrorLOF_Tabulated =
        //     (maneuverContributionLOF_Tabulated.segment(0, 3) - referenceManeuverAccelerationArrayLOF[i]).norm();
        // const double maneuverAccelerationErrorLOF_Maneuver =
        //     (maneuverContributionLOF_Maneuver.segment(0, 3) - referenceManeuverAccelerationArrayLOF[i]).norm();

        // Frames verification with Thruster Dynamics
        ASSERT_EQ(*Frame::GCRF(), *positionGCRF_Thruster.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF_Thruster.accessFrame());
        ASSERT_EQ(*lofSPtr, *(lofState_Thruster.getPosition()).accessFrame());
        ASSERT_EQ(*lofSPtr, *(lofState_Thruster.getVelocity()).accessFrame());

        // Frames verification with Tabulated Dynamics
        ASSERT_EQ(*Frame::GCRF(), *positionGCRF_Tabulated.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF_Tabulated.accessFrame());
        ASSERT_EQ(*lofSPtr, *(lofState_Tabulated.getPosition()).accessFrame());
        ASSERT_EQ(*lofSPtr, *(lofState_Tabulated.getVelocity()).accessFrame());

        // Frames verification with Maneuver
        ASSERT_EQ(*Frame::GCRF(), *positionGCRF_Maneuver.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF_Maneuver.accessFrame());
        ASSERT_EQ(*lofSPtr, *(lofState_Maneuver.getPosition()).accessFrame());
        ASSERT_EQ(*lofSPtr, *(lofState_Maneuver.getVelocity()).accessFrame());

        // GCRF Errors with Thruster Dynamics
        ASSERT_GT(param_positionErrorGCRFTolerance, positionErrorGCRF_Thruster);
        ASSERT_GT(param_velocityErrorGCRFTolerance, velocityErrorGCRF_Thruster);
        ASSERT_GT(param_accelerationErrorGCRFTolerance, maneuverAccelerationErrorGCRF_Thruster);
        ASSERT_GT(param_massErrorTolerance, massError_Thruster);

        // GCRF Errors with Tabulated Dynamics
        ASSERT_GT(param_positionErrorGCRFTolerance, positionErrorGCRF_Tabulated);
        ASSERT_GT(param_velocityErrorGCRFTolerance, velocityErrorGCRF_Tabulated);
        ASSERT_GT(param_accelerationErrorGCRFTolerance, maneuverAccelerationErrorGCRF_Tabulated);
        ASSERT_GT(param_massErrorTolerance, massError_Tabulated);

        // GCRF Errors with Maneuver
        ASSERT_GT(param_positionErrorGCRFTolerance, positionErrorGCRF_Maneuver);
        ASSERT_GT(param_velocityErrorGCRFTolerance, velocityErrorGCRF_Maneuver);
        ASSERT_GT(param_accelerationErrorGCRFTolerance, maneuverAccelerationErrorGCRF_Maneuver);
        ASSERT_GT(param_massErrorTolerance, massError_Maneuver);

        // LOF Errors with Thruster Dynamics
        ASSERT_GT(param_positionErrorLOFTolerance, positionErrorLOF_Thruster);
        ASSERT_GT(param_velocityErrorLOFTolerance, velocityErrorLOF_Thruster);
        if (!param_withAtmosphere)
        {
            ASSERT_GT(param_accelerationErrorLOFTolerance, maneuverAccelerationErrorLOF_Thruster);
        }

        // LOF Errors with Tabulated Dynamics
        ASSERT_GT(param_positionErrorLOFTolerance, positionErrorLOF_Tabulated);
        ASSERT_GT(param_velocityErrorLOFTolerance, velocityErrorLOF_Tabulated);
        // TBM: implement TabulatedDynamics frame conversion for computeContributions
        // if (!param_withAtmosphere)
        // {
        //     ASSERT_GT(param_accelerationErrorLOFTolerance, maneuverAccelerationErrorLOF_Tabulated);
        // }

        // LOF Errors with Maneuver
        ASSERT_GT(param_positionErrorLOFTolerance, positionErrorLOF_Maneuver);
        ASSERT_GT(param_velocityErrorLOFTolerance, velocityErrorLOF_Maneuver);
        // TBM: implement TabulatedDynamics frame conversion for computeContributions
        // if (!param_withAtmosphere)
        // {
        //     ASSERT_GT(param_accelerationErrorLOFTolerance, maneuverAccelerationErrorLOF_Maneuver);
        // }

        // Results console output
        // if (i >= instantArray.getSize() - 2)
        // {
        //     std::cout << "**************************************" << std::endl;
        //     std::cout.setf(std::ios::scientific, std::ios::floatfield);
        //     std::cout << "Instant: " << instantArray[i] << std::endl;
        //     std::cout << "Position Error GCRF for Thruster: " << positionErrorGCRF_Thruster << "m" << std::endl;
        //     std::cout << "Position Error GCRF for Tabulated: " << positionErrorGCRF_Tabulated << "m" << std::endl;
        //     std::cout << "Position Error GCRF for Maneuver: " << positionErrorGCRF_Maneuver << "m" << std::endl;

        //     std::cout << "Velocity Error GRCRF for Thruster: " << velocityErrorGCRF_Thruster << "m/s" << std::endl;
        //     std::cout << "Velocity Error GCRF for Tabulated: " << velocityErrorGCRF_Tabulated << "m/s" << std::endl;
        //     std::cout << "Velocity Error GCRF for Maneuver: " << velocityErrorGCRF_Maneuver << "m/s" << std::endl;

        //     std::cout << "Acceleration Error GCRF for Thruster: " << maneuverAccelerationErrorGCRF_Thruster <<
        //     "m/s^2"
        //               << std::endl;
        //     std::cout << "Acceleration Error GCRF for Tabulated: " << maneuverAccelerationErrorGCRF_Tabulated <<
        //     "m/s^2"
        //               << std::endl;
        //     std::cout << "Acceleration Error GCRF for Maneuver: " << maneuverAccelerationErrorGCRF_Maneuver <<
        //     "m/s^2"
        //               << std::endl;

        //     std::cout << "Mass Error for Thruster: " << massError_Thruster << "kg" << std::endl;
        //     std::cout << "Mass Error for Tabulated: " << massError_Tabulated << "kg" << std::endl;
        //     std::cout << "Mass Error for Maneuver: " << massError_Maneuver << "kg" << std::endl;

        //     std::cout << "Position Error LOF for Thruster: " << positionErrorLOF_Thruster << "m" << std::endl;
        //     std::cout << "Position Error LOF for Tabulated: " << positionErrorLOF_Tabulated << "m" << std::endl;
        //     std::cout << "Position Error LOF for Maneuver: " << positionErrorLOF_Maneuver << "m" << std::endl;

        //     std::cout << "Velocity Error LOF for Thruster: " << velocityErrorLOF_Thruster << "m/s" << std::endl;
        //     std::cout << "Velocity Error LOF for Tabulated: " << velocityErrorLOF_Tabulated << "m/s" << std::endl;
        //     std::cout << "Velocity Error LOF for Maneuver: " << velocityErrorLOF_Maneuver << "m/s" << std::endl;

        //     if (!param_withAtmosphere)
        //     {
        //         std::cout << "Acceleration Error LOF for Thruster: " << maneuverAccelerationErrorLOF_Thruster <<
        //         "m/s^2"
        //                   << std::endl;
        //         // std::cout << "Acceleration Error LOF for Tabulated: " << maneuverAccelerationErrorLOF_Tabulated <<
        //         // "m/s^2"
        //         //           << std::endl;
        //         // std::cout << "Acceleration Error LOF for Maneuver: " << maneuverAccelerationErrorLOF_Maneuver <<
        //         // "m/s^2"
        //         //           << std::endl;
        //     }
        //     std::cout.setf(std::ios::fixed, std::ios::floatfield);
        //     std::cout << "**************************************" << std::endl;
        // }
    }
}

// TBI: Agree on a format to version Orekit validation files
INSTANTIATE_TEST_SUITE_P(
    ForceModel_Thrust,
    OpenSpaceToolkit_Astrodynamics_Validation_CrossValidation_Thruster,
    ::testing::Values(
        //
        //
        // Without atmosphere
        //
        //
        // Test Case 0
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 1: Start date in 2021
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2021-05-13T12-34-13.345_115.0_0.1_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 2: QSW LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_QSW_0.0_1.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::QSW(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({0.0, 1.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 3: TNW LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_TNW_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::TNW(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 4: LVLH LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_LVLH_1.0_0.0_"
            "0.0_30.0.csv",                                 // Scenario validation data file path
            false,                                          // With atmosphere
            LocalOrbitalFrameFactory::LVLH(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            100.0,                                          // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            0.0,                                            // Cross section [m^2]
            0.0,                                            // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 5: VVLH LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VVLH_1.0_0.0_"
            "0.0_30.0.csv",                                 // Scenario validation data file path
            false,                                          // With atmosphere
            LocalOrbitalFrameFactory::VVLH(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            100.0,                                          // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            0.0,                                            // Cross section [m^2]
            0.0,                                            // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 6: Increase spacecraft mass to 1000kg
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_1015.0_0.1_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            1000.0,                                        // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 7: Increase maneuver duration to 4h
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_14400.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-4,  // Position error GCRF tolerance [m]
            8e-7,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-5,  // Position error LOF tolerance [m]
            8e-8,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 8: Increase spacecraft mass to 1000kg and LVLH
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_1015.0_0.1_1500.0_3600.0_LVLH_1.0_0."
            "0_0.0_30.0.csv",                               // Scenario validation data file path
            false,                                          // With atmosphere
            LocalOrbitalFrameFactory::LVLH(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            1000.0,                                         // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            0.0,                                            // Cross section [m^2]
            0.0,                                            // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 9: Increase maneuver duration to 4h and LVLH
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_14400.0_LVLH_1.0_0."
            "0_0.0_30.0.csv",                               // Scenario validation data file path
            false,                                          // With atmosphere
            LocalOrbitalFrameFactory::LVLH(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            100.0,                                          // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            0.0,                                            // Cross section [m^2]
            0.0,                                            // Drag coefficient [-]
            // Assertion tolerances
            8e-4,  // Position error GCRF tolerance [m]
            8e-7,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-5,  // Position error LOF tolerance [m]
            8e-8,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 10: Increase thrust to 10N
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_10.0_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            10.0,                                          // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-4,  // Position error GCRF tolerance [m]
            8e-7,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-4,  // Position error LOF tolerance [m]
            8e-7,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 11: Equatorial orbit
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_0.0_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_0.0_0."
            "0_30.0.csv",                                  // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 12: Thrust Vector on +Z and Equatorial
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7000000.0_0.0_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_0.0_0.0_1."
            "0_30.0.csv",                                  // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({0.0, 0.0, 1.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        // Test Case 13: Higher altitude orbit (~1000km)
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_7500000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_0.0_"
            "0.0_30.0.csv",                                // Scenario validation data file path
            false,                                         // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            0.0,                                           // Cross section [m^2]
            0.0,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            5e-9   // Acceleration error LOF tolerance [m/s^2]
        ),
        //
        //
        // With Atmosphere
        //
        //
        // Test Case 14
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 15: Start date in 2021
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2021-12-23T11-23-21.235_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 16: QSW LOF
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_QSW_0.0_"
            "1.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::QSW(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({0.0, 1.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 17: Increase spacecraft mass to 1000kg
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_1015.0_0.1_1500.0_3600.0_VNC_1."
            "0_0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",  // Scenario validation data file path
            true,                                           // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),   // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            1000.0,                                         // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            1.0,                                            // Cross section [m^2]
            2.1,                                            // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 18: Increase maneuver duration to 2h
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_7200.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-4,  // Position error GCRF tolerance [m]
            8e-7,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-5,  // Position error LOF tolerance [m]
            8e-8,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 19: Increase maneuver duration to 4h
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_14400.0_VNC_1."
            "0_0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",  // Scenario validation data file path
            true,                                           // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),   // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                      // Thrust direction in Local Orbital Frame
            100.0,                                          // Satellite dry mass [kg]
            0.1,                                            // Thrust [N]
            1500.0,                                         // Specific impulse [s]
            1.0,                                            // Cross section [m^2]
            2.1,                                            // Drag coefficient [-]
            // Assertion tolerances
            8e-4,  // Position error GCRF tolerance [m]
            8e-7,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-5,  // Position error LOF tolerance [m]
            8e-8,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 20: Increase thrust to 1N, lowering specific impulse to 150.0
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_1.0_150.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            1.0,                                           // Thrust [N]
            150.0,                                         // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-4,  // Position error GCRF tolerance [m]
            8e-7,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-5,  // Position error LOF tolerance [m]
            8e-8,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 21: Equatorial orbit
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_0.0_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 22: Increase spacecraft cross section
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_25.0_2.1_TRUE.csv",  // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            25.0,                                          // Cross section [m^2]
            2.1,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-4,  // Position error GCRF tolerance [m]
            8e-7,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-5,  // Position error LOF tolerance [m]
            8e-8,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 23: Increase spacecraft drag coefficient and cross section
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7000000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_4.2_TRUE.csv",   // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            4.2,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-4,  // Position error GCRF tolerance [m]
            8e-7,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-5,  // Position error LOF tolerance [m]
            8e-8,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        ),
        // Test Case 24: Higher initial altitude (~800 km) and increase cross section
        std::make_tuple(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated/"
            "Orekit_ConstantThrustThruster_Drag_7300000.0_98.1_2023-01-01T00-00-00.000_115.0_0.1_1500.0_3600.0_VNC_1.0_"
            "0.0_0.0_30.0_EXPONENTIAL_1.0_2.1_TRUE.csv",   // Scenario validation data file path
            true,                                          // With atmosphere
            LocalOrbitalFrameFactory::VNC(Frame::GCRF()),  // Local Orbital Frame Factory to express thrust direction
            Vector3d({1.0, 0.0, 0.0}),                     // Thrust direction in Local Orbital Frame
            100.0,                                         // Satellite dry mass [kg]
            0.1,                                           // Thrust [N]
            1500.0,                                        // Specific impulse [s]
            1.0,                                           // Cross section [m^2]
            2.1,                                           // Drag coefficient [-]
            // Assertion tolerances
            8e-5,  // Position error GCRF tolerance [m]
            8e-8,  // Velocity error GCRF tolerance [m/s]
            1e-9,  // Acceleration error GCRF tolerance [m/s^2]
            1e-9,  // Mass error tolerance [kg]
            8e-6,  // Position error LOF tolerance [m]
            8e-9,  // Velocity error LOF tolerance [m/s]
            0.0    // Acceleration error LOF tolerance [m/s^2] Not tested because quantity is not recorded in csv
        )
    )
);
