/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

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
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
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
using ostk::core::types::Real;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;

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
using ostk::astro::flight::system::dynamics::AtmosphericDrag;
using ostk::astro::flight::system::dynamics::CentralBodyGravity;
using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));
        
        const Real mass = 100;
        const Real area = 1;
        const Real cd = 2.2;

        startStateVector_.resize(9);
        startStateVector_ << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, mass, area, cd;

        earthSPtr_ = std::make_shared<Celestial>(earth_);
    }

    // Current state and instant setup, choose equinox as instant to make geometry simple
    // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
    const Instant startInstant_ = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

    const Earth earth_ = {
        EarthGravitationalModel::Spherical.gravitationalParameter_,
        EarthGravitationalModel::Spherical.equatorialRadius_,
        EarthGravitationalModel::Spherical.flattening_,
        EarthGravitationalModel::Spherical.J2_,
        EarthGravitationalModel::Spherical.J4_,
        std::make_shared<Analytical>(Frame::ITRF()),
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Undefined),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential),
    };

    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();

    VectorXd startStateVector_;

    Shared<Celestial> earthSPtr_ = nullptr;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, Constructor)
{
    {
        EXPECT_NO_THROW(AtmosphericDrag atmosphericDrag(earthSPtr_, satelliteSystem_));
    }

    {
        EXPECT_NO_THROW(AtmosphericDrag atmosphericDrag(earthSPtr_, satelliteSystem_, "test"));
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

        const String expectedString = "{Atmospheric Model} is undefined.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    AtmosphericDrag atmosphericDrag(std::make_shared<Celestial>(earth), satelliteSystem_);
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, IsDefined)
{
    AtmosphericDrag atmosphericDynamics(earthSPtr_, satelliteSystem_);
    EXPECT_TRUE(atmosphericDynamics.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, StreamOperator)
{
    {
        AtmosphericDrag atmosphericDynamics(earthSPtr_, satelliteSystem_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << atmosphericDynamics << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, Print)
{
    {
        AtmosphericDrag atmosphericDynamics(earthSPtr_, satelliteSystem_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(atmosphericDynamics.print(std::cout, true));
        EXPECT_NO_THROW(atmosphericDynamics.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, GetName)
{
    {
        AtmosphericDrag atmosphericDynamics(earthSPtr_, satelliteSystem_);
        EXPECT_TRUE(atmosphericDynamics.getName() != String::Empty());
    }

    {
        const String name = "test";
        AtmosphericDrag atmosphericDynamics(earthSPtr_, satelliteSystem_, name);
        EXPECT_TRUE(atmosphericDynamics.getName() == name);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, GetCelestial)
{
    AtmosphericDrag atmosphericDynamics(earthSPtr_, satelliteSystem_);
    EXPECT_TRUE(atmosphericDynamics.getCelestial() == earthSPtr_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, GetSatelliteSystem)
{
    AtmosphericDrag atmosphericDynamics(earthSPtr_, satelliteSystem_);
    EXPECT_TRUE(atmosphericDynamics.getSatelliteSystem() == satelliteSystem_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, GetReadCoordinatesSubsets)
{
    AtmosphericDrag atmosphericDrag(earthSPtr_, satelliteSystem_);

    const Array<Shared<const CoordinatesSubset>> subsets = atmosphericDrag.getReadCoordinatesSubsets();

    EXPECT_EQ(5, subsets.size());
    EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
    EXPECT_EQ(CartesianVelocity::Default(), subsets[1]);
    EXPECT_EQ(CoordinatesSubset::Mass(), subsets[2]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, GetWriteCoordinatesSubsets)
{
    AtmosphericDrag atmosphericDrag(earthSPtr_, satelliteSystem_);

    const Array<Shared<const CoordinatesSubset>> subsets = atmosphericDrag.getWriteCoordinatesSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianVelocity::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_AtmosphericDrag, ComputeContribution)
{
    AtmosphericDrag atmosphericDrag(earthSPtr_, satelliteSystem_);

    const VectorXd contribution = atmosphericDrag.computeContribution(startInstant_, startStateVector_, Frame::GCRF());

    EXPECT_EQ(3, contribution.size());
    EXPECT_GT(1e-15, 0.0 - contribution[0]);
    EXPECT_GT(5e-11, -0.0000278707803890 - contribution[1]);
    EXPECT_GT(5e-11, -0.0000000000197640 - contribution[2]);
}
