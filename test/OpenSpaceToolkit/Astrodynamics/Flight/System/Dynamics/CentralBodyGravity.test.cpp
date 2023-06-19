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

    const Shared<Celestial> sphericalEarthSPtr_ = std::make_shared<Celestial>(Earth::Spherical());

    Dynamics::StateVector startStateVector_;
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
                    CentralBodyGravity centralBodyGravity(std::make_shared<Celestial>(earth));
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
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_));
    }

    {
        const Shared<Celestial> earthSPtrWGS84 = std::make_shared<Celestial>(Earth::WGS84());
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravity(earthSPtrWGS84));
    }

    {
        const Shared<Celestial> sun = std::make_shared<Celestial>(Sun::Spherical());
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravity(sun));
    }

    {
        const Shared<Celestial> moon = std::make_shared<Celestial>(Moon::Spherical());
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravity(moon));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, IsDefined)
{
    {
        const CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_);

        EXPECT_TRUE(centralBodyGravity.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, StreamOperator)
{
    {
        const CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << centralBodyGravity << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, Print)
{
    {
        const CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(centralBodyGravity.print(std::cout, true));
        EXPECT_NO_THROW(centralBodyGravity.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, GetCelestial)
{
    const CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_);

    EXPECT_TRUE(centralBodyGravity.getCelestial() == sphericalEarthSPtr_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, Update)
{
    CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_);

    Dynamics::StateVector dxdt(6, 0.0);
    centralBodyGravity.update(startStateVector_, dxdt, startInstant_);
    EXPECT_GT(1e-15, 0.0 - dxdt[0]);
    EXPECT_GT(1e-15, 0.0 - dxdt[1]);
    EXPECT_GT(1e-15, 0.0 - dxdt[2]);
    EXPECT_GT(1e-15, -8.134702887755102 - dxdt[3]);
    EXPECT_GT(1e-15, 0.0 - dxdt[4]);
    EXPECT_GT(1e-15, 0.0 - dxdt[5]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, OneStepEarthOnly)
{
    Dynamics::StateVector Earth_ReferencePull(6);

    // Setup dynamics
    const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::Spherical());
    const Array<Shared<Dynamics>> dynamics = {std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth))};

    // Perform 1.0s integration step
    runge_kutta4<Dynamics::StateVector> stepper;
    stepper.do_step(Dynamics::GetDynamicalEquations(dynamics, startInstant_), startStateVector_, (0.0), 1.0);

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
