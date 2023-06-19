/// Apache License 2.0

#include <boost/numeric/odeint.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::env::ephem::Analytical;
using ostk::physics::coord::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::units::Length;
using ostk::physics::units::Derived;
using ostk::physics::units::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;

using namespace boost::numeric::odeint;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        startStateVector.resize(6);

        startStateVector[0] = 7000000.0;
        startStateVector[1] = 0.0;
        startStateVector[2] = 0.0;
        startStateVector[3] = 0.0;
        startStateVector[4] = 0.0;
        startStateVector[5] = 0.0;
    }

    // Current state and instant setup, choose equinox as instant to make geometry simple
    // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
    const Instant startInstant = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

    Dynamics::StateVector startStateVector;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, Constructor)
{
    {
        const Earth earth = {
            {398600441500000.0, GravitationalParameterSIUnit},
            Length::Meters(6378137.0),
            0.0,
            0.0,
            0.0,
            std::make_shared<Analytical>(Frame::ITRF()),
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Undefined),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Undefined),
        };

        const String expectedString = "{Gravitational Model} is undefined.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    CentralBodyGravity centralBodyGravitationalDynamics(std::make_shared<Celestial>(earth));
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

    {
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Earth::Spherical());
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravitationalDynamics(earthSPtr));
    }

    {
        const Shared<Celestial> earthSPtrWGS84 = std::make_shared<Celestial>(Earth::WGS84());
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravitationalDynamics(earthSPtrWGS84));
    }

    {
        const Shared<Celestial> sun = std::make_shared<Celestial>(Sun::Spherical());
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravitationalDynamics(sun));
    }

    {
        const Shared<Celestial> moon = std::make_shared<Celestial>(Moon::Spherical());
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravitationalDynamics(moon));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, IsDefined)
{
    {
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Earth::Spherical());
        const CentralBodyGravity centralBodyGravitationalDynamics(earthSPtr);

        EXPECT_TRUE(centralBodyGravitationalDynamics.isDefined());
    }

    {
        const Earth earth = {
            {398600441500000.0, GravitationalParameterSIUnit},
            Length::Meters(6378137.0),
            0.0,
            0.0,
            0.0,
            std::make_shared<Analytical>(Frame::ITRF()),
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Undefined),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Dipole),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential),
        };

        CentralBodyGravity centralBodyGravitationalDynamics(std::make_shared<Celestial>(earth));

        EXPECT_FALSE(centralBodyGravitationalDynamics.isDefined());
    }

    {
        const Earth earth = {
            {398600441500000.0, GravitationalParameterSIUnit},
            Length::Meters(6378137.0),
            0.0,
            0.0,
            0.0,
            nullptr,
            std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::EGM84),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Dipole),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential),
        };

        CentralBodyGravity centralBodyGravitationalDynamics(std::make_shared<Celestial>(earth));

        EXPECT_FALSE(centralBodyGravitationalDynamics.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, GetCelestial)
{
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Earth::Spherical());
    const CentralBodyGravity centralBodyGravitationalDynamics(earthSPtr);

    EXPECT_TRUE(centralBodyGravitationalDynamics.getCelestial() == earthSPtr);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, Update)
{
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Earth::Spherical());
    CentralBodyGravity centralBodyGravitationalDynamics(earthSPtr);

    Dynamics::StateVector dxdt(6, 0.0);
    centralBodyGravitationalDynamics.update(startStateVector, dxdt, startInstant);
    EXPECT_GT(1e-15, 0.0 - dxdt[0]);
    EXPECT_GT(1e-15, 0.0 - dxdt[1]);
    EXPECT_GT(1e-15, 0.0 - dxdt[2]);

    std::cout << "CENTRAL BODY GRAVITY" << std::endl;
    std::cout << dxdt[3] << std::endl;
    std::cout << dxdt[4] << std::endl;
    std::cout << dxdt[5] << std::endl;
    //     EXPECT_GT(1e-15, 0.0 - dxdt[3]);
    //     EXPECT_GT(5e-11, -0.0000278707803890 - dxdt[4]);
    //     EXPECT_GT(5e-11, -0.0000000000197640 - dxdt[5]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, OneStepEarthOnly)
{
    Dynamics::StateVector Earth_ReferencePull(6);

    // Setup dynamics
    const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> dynamics = {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth))};

    // Perform 1.0s integration step
    runge_kutta4<Dynamics::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(startInstant_, dynamics), startStateVector_, (0.0), 1.0);

    // Set reference pull values for the Earth
    Earth_ReferencePull[0] = 6.999995932647768e+06;
    Earth_ReferencePull[1] = -2.312964634635743e-17;
    Earth_ReferencePull[2] = 0.000000000000000e+00;
    Earth_ReferencePull[3] = -8.134706038871020e+00;
    Earth_ReferencePull[4] = -4.625929269271485e-17;
    Earth_ReferencePull[5] = 0.000000000000000e+00;

    EXPECT_GT(1e-15, startStateVector_[0] - Earth_ReferencePull[0]);
    EXPECT_GT(1e-15, startStateVector_[1] - Earth_ReferencePull[1]);
    EXPECT_GT(1e-15, startStateVector_[2] - Earth_ReferencePull[2]);
    EXPECT_GT(1e-15, startStateVector_[3] - Earth_ReferencePull[3]);
    EXPECT_GT(1e-15, startStateVector_[4] - Earth_ReferencePull[4]);
    EXPECT_GT(1e-15, startStateVector_[5] - Earth_ReferencePull[5]);
}
