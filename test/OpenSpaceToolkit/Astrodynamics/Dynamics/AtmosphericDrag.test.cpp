/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemeris/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Index;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::ephemeris::Analytical;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;
using ostk::physics::unit::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag : public ::testing::Test
{
   protected:
    void SetUp() override
    {
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

    VectorXd startStateVector_;

    Shared<Celestial> earthSPtr_ = nullptr;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, Constructor)
{
    {
        EXPECT_NO_THROW(AtmosphericDrag atmosphericDrag(earthSPtr_));
    }

    {
        EXPECT_NO_THROW(AtmosphericDrag atmosphericDrag(earthSPtr_, "test"));
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
                    AtmosphericDrag atmosphericDrag(std::make_shared<Celestial>(earth));
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, IsDefined)
{
    AtmosphericDrag atmosphericDynamics(earthSPtr_);
    EXPECT_TRUE(atmosphericDynamics.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, StreamOperator)
{
    {
        AtmosphericDrag atmosphericDynamics(earthSPtr_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << atmosphericDynamics << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, Print)
{
    {
        AtmosphericDrag atmosphericDynamics(earthSPtr_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(atmosphericDynamics.print(std::cout, true));
        EXPECT_NO_THROW(atmosphericDynamics.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, GetName)
{
    {
        AtmosphericDrag atmosphericDynamics(earthSPtr_);
        EXPECT_TRUE(atmosphericDynamics.getName() != String::Empty());
    }

    {
        const String name = "test";
        AtmosphericDrag atmosphericDynamics(earthSPtr_, name);
        EXPECT_TRUE(atmosphericDynamics.getName() == name);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, GetCelestial)
{
    AtmosphericDrag atmosphericDynamics(earthSPtr_);
    EXPECT_TRUE(atmosphericDynamics.getCelestial() == earthSPtr_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, GetReadCoordinateSubsets)
{
    AtmosphericDrag atmosphericDrag(earthSPtr_);

    const Array<Shared<const CoordinateSubset>> subsets = atmosphericDrag.getReadCoordinateSubsets();

    EXPECT_EQ(5, subsets.size());
    EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
    EXPECT_EQ(CartesianVelocity::Default(), subsets[1]);
    EXPECT_EQ(CoordinateSubset::Mass(), subsets[2]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, GetWriteCoordinateSubsets)
{
    AtmosphericDrag atmosphericDrag(earthSPtr_);

    const Array<Shared<const CoordinateSubset>> subsets = atmosphericDrag.getWriteCoordinateSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianVelocity::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AtmosphericDrag, ComputeContribution)
{
    AtmosphericDrag atmosphericDrag(earthSPtr_);

    const VectorXd contribution = atmosphericDrag.computeContribution(startInstant_, startStateVector_);

    EXPECT_EQ(3, contribution.size());
    EXPECT_GT(1e-15, 0.0 - contribution[0]);
    EXPECT_GT(5e-11, -0.0000278707803890 - contribution[1]);
    EXPECT_GT(5e-11, -0.0000000000197640 - contribution[2]);
}
