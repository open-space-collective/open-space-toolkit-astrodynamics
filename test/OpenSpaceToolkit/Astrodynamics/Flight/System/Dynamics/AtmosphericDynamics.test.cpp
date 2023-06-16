/// Apache License 2.0

#include <boost/numeric/odeint.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDynamics.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::env::ephem::Analytical;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::units::Mass;
using ostk::physics::units::Length;
using ostk::physics::units::Derived;
using ostk::physics::units::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::dynamics::AtmosphericDynamics;

using namespace boost::numeric::odeint;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDynamics : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        satelliteSystem_ =
            SatelliteSystem(Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 500.0, 2.1);

        startStateVector_.resize(6);
        startStateVector_[0] = 7000000.0;
        startStateVector_[1] = 0.0;
        startStateVector_[2] = 0.0;
        startStateVector_[3] = 0.0;
        startStateVector_[4] = 7546.05329;
        startStateVector_[5] = 0.0;

        earthSPtr_ = std::make_shared<Celestial>(earth_);
    }

    // Current state and instant setup, choose equinox as instant to make geometry simple
    // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
    const Instant startInstant_ = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

    const Earth earth_ = {
        Earth::Models::Spherical::GravitationalParameter,
        Earth::Models::Spherical::EquatorialRadius,
        Earth::Models::Spherical::Flattening,
        Earth::Models::Spherical::J2,
        Earth::Models::Spherical::J4,
        std::make_shared<Analytical>(Frame::ITRF()),
        EarthGravitationalModel::Type::Undefined,
        EarthMagneticModel::Type::Undefined,
        EarthAtmosphericModel::Type::Exponential,
    };

    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();

    Dynamics::StateVector startStateVector_;

    Shared<Celestial> earthSPtr_ = nullptr;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDynamics, Constructor)
{
    EXPECT_NO_THROW(AtmosphericDynamics atmosphericDynamics(earthSPtr_, satelliteSystem_));

    const Earth earth = {
        {398600441500000.0, GravitationalParameterSIUnit},
        Length::Meters(6378137.0),
        0.0,
        0.0,
        0.0,
        std::make_shared<Analytical>(Frame::ITRF()),
        EarthGravitationalModel::Type::Undefined,
        EarthMagneticModel::Type::Undefined,
        EarthAtmosphericModel::Type::Undefined,
    };

    const String expectedString = "{Atmospheric Model} is undefined.";

    // Test the throw and the message that is thrown
    EXPECT_THROW(
        {
            try
            {
                AtmosphericDynamics atmosphericDynamics(std::make_shared<Celestial>(earth), satelliteSystem_);
            }
            catch (const ostk::core::error::runtime::Undefined& e)
            {
                EXPECT_EQ(expectedString, e.what());
                throw;
            }
        },
        ostk::core::error::runtime::Undefined
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDynamics, IsDefined)
{
    AtmosphericDynamics atmosphericDynamics(earthSPtr_, satelliteSystem_);
    EXPECT_TRUE(atmosphericDynamics.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDynamics, Update)
{
    Dynamics::StateVector dxdt(6, 0.0);
    AtmosphericDynamics atmosphericDynamics(earthSPtr_, satelliteSystem_);
    atmosphericDynamics.update(startStateVector_, dxdt, startInstant_);
    EXPECT_GT(1e-15, 0.0 - dxdt[0]);
    EXPECT_GT(1e-15, 0.0 - dxdt[1]);
    EXPECT_GT(1e-15, 0.0 - dxdt[2]);
    EXPECT_GT(1e-15, 0.0 - dxdt[3]);
    EXPECT_GT(5e-11, -0.0000278707803890 - dxdt[4]);
    EXPECT_GT(5e-11, -0.0000000000197640 - dxdt[5]);
}

// Test data gathered from Orekit
TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDynamics, OneStepAtmosphereOnly)
{
    // Setup dynamics
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<AtmosphericDynamics>(AtmosphericDynamics(earthSPtr_, satelliteSystem_))};

    // Perform 1.0s integration step
    runge_kutta4<Dynamics::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(startInstant_, dynamics), startStateVector_, (0.0), 1.0);

    // Set reference pull values for the Earth
    Dynamics::StateVector Earth_ReferencePull = {
        7000000.0000000000000000, 0.0, 0.0, 0.0, 7546.0532621292200000, -00000.0000000000197640,};

    EXPECT_GT(5e-11, startStateVector_[0] - Earth_ReferencePull[0]);
    EXPECT_GT(5e-11, startStateVector_[1] - Earth_ReferencePull[1]);
    EXPECT_GT(5e-11, startStateVector_[2] - Earth_ReferencePull[2]);
    EXPECT_GT(5e-11, startStateVector_[3] - Earth_ReferencePull[3]);
    EXPECT_GT(5e-11, startStateVector_[4] - Earth_ReferencePull[4]);
    EXPECT_GT(5e-11, startStateVector_[5] - Earth_ReferencePull[5]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDynamics, OneStepAtmosphereGravity)
{
    // Setup dynamics
    const Earth earth = {
        Earth::Models::Spherical::GravitationalParameter,
        Earth::Models::Spherical::EquatorialRadius,
        Earth::Models::Spherical::Flattening,
        Earth::Models::Spherical::J2,
        Earth::Models::Spherical::J4,
        std::make_shared<Analytical>(Frame::ITRF()),
        EarthGravitationalModel::Type::Spherical,
        EarthMagneticModel::Type::Undefined,
        EarthAtmosphericModel::Type::Exponential,
    };

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<AtmosphericDynamics>(AtmosphericDynamics(earthSPtr, satelliteSystem_))};

    // Setup initial conditions
    const Instant startInstant = Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC);

    Dynamics::StateVector startStateVector = {
        6878137.0,
        0.0,
        0.0,
        0.0,
        7612.608170359118000,
        0.0,
    };

    // Perform 1.0s integration step
    runge_kutta4<Dynamics::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(startInstant, dynamics), startStateVector, (0.0), 1.0);

    // Set reference pull values for the Earth
    Dynamics::StateVector Earth_ReferencePull = {
        6878132.787246078000000,
        7612.606615971900000,
        -0.000000000000330,
        -8.425506982847088,
        7612.603507382901000,
        -0.000000000000649,
    };

    EXPECT_GT(1e-12, startStateVector_[0] - Earth_ReferencePull[0]);
    EXPECT_GT(1e-12, startStateVector_[1] - Earth_ReferencePull[1]);
    EXPECT_GT(1e-12, startStateVector_[2] - Earth_ReferencePull[2]);
    EXPECT_GT(1e-12, startStateVector_[3] - Earth_ReferencePull[3]);
    EXPECT_GT(1e-12, startStateVector_[4] - Earth_ReferencePull[4]);
    EXPECT_GT(1e-12, startStateVector_[5] - Earth_ReferencePull[5]);
}
