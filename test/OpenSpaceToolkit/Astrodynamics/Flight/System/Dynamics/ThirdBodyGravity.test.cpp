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
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/ThirdBodyGravity.hpp>

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
using ostk::astro::flight::system::dynamics::ThirdBodyGravity;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;

using namespace boost::numeric::odeint;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        startStateVector_.resize(6);

        startStateVector_[0] = 7000000.0;
        startStateVector_[1] = 0.0;
        startStateVector_[2] = 0.0;
        startStateVector_[3] = 0.0;
        startStateVector_[4] = 0.0;
        startStateVector_[5] = 0.0;
    }

    // Current state and instant setup, choose equinox as instant to make geometry simple
    // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
    const Instant startInstant_ = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

    Dynamics::StateVector startStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, Constructor)
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
                    ThirdBodyGravity thirdBodyGravitationalDynamics(std::make_shared<Celestial>(earth));
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
        const String expectedString = "Cannot calculate third body acceleration for the Earth yet.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    ThirdBodyGravity thirdBodyGravitationalDynamics(std::make_shared<Celestial>(Earth::Spherical()));
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(expectedString, e.what());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        const Shared<Celestial> moonSPtr = std::make_shared<Celestial>(Moon::Spherical());
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravitationalDynamics(moonSPtr));
    }

    {
        const Shared<Celestial> sunSPtr = std::make_shared<Celestial>(Sun::Spherical());
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravitationalDynamics(sunSPtr));
    }

    {
        const Shared<Celestial> moonSPtr = std::make_shared<Celestial>(Moon::Spherical());
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravitationalDynamics(moonSPtr));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, IsDefined)
{
    {
        const Shared<Celestial> moonSPtr = std::make_shared<Celestial>(Moon::Spherical());
        const ThirdBodyGravity thirdBodyGravitationalDynamics(moonSPtr);

        EXPECT_TRUE(thirdBodyGravitationalDynamics.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, GetCelestial)
{
    const Shared<Celestial> moonSPtr = std::make_shared<Celestial>(Moon::Spherical());
    const ThirdBodyGravity thirdBodyGravitationalDynamics(moonSPtr);

    EXPECT_TRUE(thirdBodyGravitationalDynamics.getCelestial() == moonSPtr);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, Update)
{
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Moon::Spherical());
    ThirdBodyGravity thirdBodyGravitationalDynamics(earthSPtr);

    Dynamics::StateVector dxdt(6, 0.0);
    thirdBodyGravitationalDynamics.update(startStateVector_, dxdt, startInstant_);

    EXPECT_GT(1e-15, 0.0 - dxdt[0]);
    EXPECT_GT(1e-15, 0.0 - dxdt[1]);
    EXPECT_GT(1e-15, 0.0 - dxdt[2]);
    EXPECT_GT(1e-15, -4.620543790697659e-07 - dxdt[3]);
    EXPECT_GT(1e-15, 2.948717888154649e-07 - dxdt[4]);
    EXPECT_GT(1e-15, 1.301648617451192e-07 - dxdt[5]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, OneStepSunOnly)
{
    Dynamics::StateVector Sun_ReferencePull(6);
    // Setup dynamics
    const Shared<Celestial> sun = std::make_shared<Celestial>(Sun::Spherical());
    const Array<Shared<Dynamics>> dynamics = {std::make_shared<ThirdBodyGravity>(sun)};

    // Perform 1.0s integration step
    runge_kutta4<Dynamics::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(startInstant_, dynamics), startStateVector_, (0.0), 1.0);

    // Set reference pull values for the Earth
    Sun_ReferencePull[0] = 7.000000000000282e+06;
    Sun_ReferencePull[1] = -1.266173652819505e-09;
    Sun_ReferencePull[2] = -5.501324277544413e-10;
    Sun_ReferencePull[3] = 5.618209329643997e-07;
    Sun_ReferencePull[4] = -2.532321435973975e-09;
    Sun_ReferencePull[5] = -1.100253640019350e-09;

    EXPECT_GT(1e-15, startStateVector_[0] - Sun_ReferencePull[0]);
    EXPECT_GT(1e-15, startStateVector_[1] - Sun_ReferencePull[1]);
    EXPECT_GT(1e-15, startStateVector_[2] - Sun_ReferencePull[2]);
    EXPECT_GT(1e-15, startStateVector_[3] - Sun_ReferencePull[3]);
    EXPECT_GT(1e-15, startStateVector_[4] - Sun_ReferencePull[4]);
    EXPECT_GT(1e-15, startStateVector_[5] - Sun_ReferencePull[5]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, OneStepMoonOnly)
{
    Dynamics::StateVector Moon_ReferencePull(6);

    // Setup dynamics
    const Shared<Celestial> moon = std::make_shared<Celestial>(Moon::Spherical());
    const Array<Shared<Dynamics>> dynamics = {std::make_shared<ThirdBodyGravity>(moon)};

    // Perform 1.0s integration step
    runge_kutta4<Dynamics::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(startInstant_, dynamics), startStateVector_, (0.0), 1.0);

    // Set reference pull values for the Earth
    Moon_ReferencePull[0] = 6.999999999999768e+06;
    Moon_ReferencePull[1] = 1.474353635647267e-07;
    Moon_ReferencePull[2] = 6.508220913373722e-08;
    Moon_ReferencePull[3] = -4.620551958115301e-07;
    Moon_ReferencePull[4] = 2.948701962648114e-07;
    Moon_ReferencePull[5] = 1.301641965195380e-07;

    EXPECT_GT(1e-15, startStateVector_[0] - Moon_ReferencePull[0]);
    EXPECT_GT(1e-15, startStateVector_[1] - Moon_ReferencePull[1]);
    EXPECT_GT(1e-15, startStateVector_[2] - Moon_ReferencePull[2]);
    EXPECT_GT(1e-15, startStateVector_[3] - Moon_ReferencePull[3]);
    EXPECT_GT(1e-15, startStateVector_[4] - Moon_ReferencePull[4]);
    EXPECT_GT(1e-15, startStateVector_[5] - Moon_ReferencePull[5]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_ThirdBodyGravity, OneStepSunMoonEarth)
{
    Dynamics::StateVector Earth_Sun_Moon_ReferencePull(6);

    const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::Spherical());
    const Shared<Celestial> sun = std::make_shared<Celestial>(Sun::Spherical());
    const Shared<Celestial> moon = std::make_shared<Celestial>(Moon::Spherical());

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<CentralBodyGravity>(earth),
        std::make_shared<ThirdBodyGravity>(sun),
        std::make_shared<ThirdBodyGravity>(moon)};

    // Perform 1.0s integration step
    runge_kutta4<Dynamics::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(startInstant_, dynamics), startStateVector_, (0.0), 1.0);

    // Set reference pull values for the Earth
    Earth_Sun_Moon_ReferencePull[0] = 6.999995932647768e+06 + 6.999999999999768e+06 + 7.000000000000282e+06;
    Earth_Sun_Moon_ReferencePull[1] = -2.312964634635743e-17 + 1.474353635647267e-07 + -1.266173652819505e-09;
    Earth_Sun_Moon_ReferencePull[2] = 0.000000000000000e+00 + 6.508220913373722e-08 + -5.501324277544413e-10;
    Earth_Sun_Moon_ReferencePull[3] = -8.134706038871020e+00 + -4.620551958115301e-07 + 5.618209329643997e-07;
    Earth_Sun_Moon_ReferencePull[4] = -4.625929269271485e-17 + 2.948701962648114e-07 + -2.532321435973975e-09;
    Earth_Sun_Moon_ReferencePull[5] = 0.000000000000000e+00 + 1.301641965195380e-07 + -1.100253640019350e-09;

    // [0] = 6.999995935640380e+06 ; Earth_Sun_Moon_ReferencePull[1] = 4.700487584518332e-06 ;
    // Earth_Sun_Moon_ReferencePull[2] = 2.137317833766671e-06 ; Earth_Sun_Moon_ReferencePull[3] =
    // -8.128720814005144 ; Earth_Sun_Moon_ReferencePull[4] = 9.401159910098908e-06 ;
    // Earth_Sun_Moon_ReferencePull[5] = 4.274716925865539e-06 ;

    EXPECT_GT(5e-14, startStateVector_[0] - Earth_Sun_Moon_ReferencePull[0]);
    EXPECT_GT(5e-14, startStateVector_[1] - Earth_Sun_Moon_ReferencePull[1]);
    EXPECT_GT(5e-14, startStateVector_[2] - Earth_Sun_Moon_ReferencePull[2]);
    EXPECT_GT(5e-14, startStateVector_[3] - Earth_Sun_Moon_ReferencePull[3]);
    EXPECT_GT(5e-14, startStateVector_[4] - Earth_Sun_Moon_ReferencePull[4]);
    EXPECT_GT(5e-14, startStateVector_[5] - Earth_Sun_Moon_ReferencePull[5]);
}
