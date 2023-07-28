/// Apache License 2.0

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Index;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

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
using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::NumericalSolver;

using namespace boost::numeric::odeint;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        startStateVector_.resize(6);
        startStateVector_ << 7000000.0, 0.0, 0.0, 0.0, 0.0, 0.0;
    }

    // Current state and instant setup, choose equinox as instant to make geometry simple
    // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
    const Instant startInstant_ = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

    const Shared<Celestial> sphericalEarthSPtr_ = std::make_shared<Celestial>(Earth::Spherical());

    NumericalSolver::StateVector startStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, Constructor)
{
    {
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_));
    }

    {
        EXPECT_NO_THROW(CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_, "test"));
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
                    EXPECT_EQ(expectedString, e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Undefined
        );
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, GetName)
{
    {
        const CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_);
        EXPECT_TRUE(centralBodyGravity.getName() != String::Empty());
    }

    {
        const String name = "test";
        const CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_, name);
        EXPECT_TRUE(centralBodyGravity.getName() == name);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, GetCelestial)
{
    const CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_);

    EXPECT_TRUE(centralBodyGravity.getCelestial() == sphericalEarthSPtr_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, GetReadCoordinatesSubsets)
{
    const CentralBodyGravity centralBodyGravity = CentralBodyGravity(sphericalEarthSPtr_);

    const Array<Shared<const CoordinatesSubset>> subsets = centralBodyGravity.getReadCoordinatesSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, GetWriteCoordinatesSubsets)
{
    const CentralBodyGravity centralBodyGravity = CentralBodyGravity(sphericalEarthSPtr_);

    const Array<Shared<const CoordinatesSubset>> subsets = centralBodyGravity.getWriteCoordinatesSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianVelocity::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, ComputeContribution)
{
    CentralBodyGravity centralBodyGravity(sphericalEarthSPtr_);

    const VectorXd contribution =
        centralBodyGravity.computeContribution(startInstant_, startStateVector_.segment(0, 3), Frame::GCRF());

    EXPECT_EQ(3, contribution.size());
    EXPECT_GT(1e-15, -8.134702887755102 - contribution[0]);
    EXPECT_GT(1e-15, 0.0 - contribution[1]);
    EXPECT_GT(1e-15, 0.0 - contribution[2]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_CentralBodyGravity, OneStepEarthOnly)
{
    const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::Spherical());

    const Pair<Index, Size> positionInformation = {0, 3};
    const Pair<Index, Size> velocityInformation = {3, 3};

    Dynamics::DynamicsInformation positionDerivativeInformation = {
        std::make_shared<PositionDerivative>(), {velocityInformation}, {positionInformation}, 3};

    Dynamics::DynamicsInformation centralBodyGravityInformation = {
        std::make_shared<CentralBodyGravity>(CentralBodyGravity(earth)),
        {positionInformation},
        {velocityInformation},
        3};

    const Array<Dynamics::DynamicsInformation> dynamicsInformation = {
        positionDerivativeInformation, centralBodyGravityInformation};

    // Perform 1.0s integration step
    runge_kutta4<NumericalSolver::StateVector> stepper;
    stepper.do_step(
        Dynamics::GetDynamicalEquations(dynamicsInformation, startInstant_, Frame::GCRF()),
        startStateVector_,
        (0.0),
        1.0
    );

    // Set reference pull values for the Earth
    NumericalSolver::StateVector Earth_ReferencePull(6);
    Earth_ReferencePull << 6.999995932647768e+06, -2.312964634635743e-17, 0.0, -8.134706038871020e+00,
        -4.625929269271485e-17, 0.0;

    EXPECT_TRUE(((startStateVector_ - Earth_ReferencePull).array() < 1e-15).all());
}
