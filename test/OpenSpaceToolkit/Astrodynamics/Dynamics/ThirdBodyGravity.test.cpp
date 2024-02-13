/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Ephemeris/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Index;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::environment::ephemeris::Analytical;
using ostk::physics::coordinate::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Length;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::ThirdBodyGravity;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity : public ::testing::Test
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
    const Shared<Celestial> sphericalMoonSPtr_ = std::make_shared<Celestial>(Moon::Spherical());

    const ThirdBodyGravity defaultThirdBodyGravity_ = {sphericalMoonSPtr_};

    VectorXd startStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, Constructor)
{
    {
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_));
    }

    {
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_, "test"));
    }

    {
        const Shared<Celestial> sunSPtr = std::make_shared<Celestial>(Sun::Spherical());
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravity(sunSPtr));
    }

    {
        const Shared<Celestial> moonSPtr = std::make_shared<Celestial>(Moon::Spherical());
        EXPECT_NO_THROW(ThirdBodyGravity thirdBodyGravity(moonSPtr));
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
                    ThirdBodyGravity thirdBodyGravity(std::make_shared<Celestial>(earth));
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

    {
        const String expectedString = "Cannot calculate third body acceleration for the Earth yet.";

        // Test the throw and the message that is thrown
        EXPECT_THROW(
            {
                try
                {
                    ThirdBodyGravity thirdBodyGravity(std::make_shared<Celestial>(Earth::Spherical()));
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(expectedString, e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, IsDefined)
{
    {
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_);

        EXPECT_TRUE(thirdBodyGravity.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, StreamOperator)
{
    {
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << thirdBodyGravity << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, Print)
{
    {
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(thirdBodyGravity.print(std::cout, true));
        EXPECT_NO_THROW(thirdBodyGravity.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, GetName)
{
    {
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_);
        EXPECT_TRUE(thirdBodyGravity.getName() != String::Empty());
    }

    {
        const String name = "test";
        const ThirdBodyGravity thirdBodyGravity(sphericalMoonSPtr_, name);
        EXPECT_TRUE(thirdBodyGravity.getName() == name);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, GetCelestial)
{
    EXPECT_TRUE(defaultThirdBodyGravity_.getCelestial() == sphericalMoonSPtr_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, GetReadCoordinateSubsets)
{
    const Array<Shared<const CoordinateSubset>> subsets = defaultThirdBodyGravity_.getReadCoordinateSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, GetWriteCoordinateSubsets)
{
    const Array<Shared<const CoordinateSubset>> subsets = defaultThirdBodyGravity_.getWriteCoordinateSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianVelocity::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, ComputeContribution)
{
    const VectorXd contribution =
        defaultThirdBodyGravity_.computeContribution(startInstant_, startStateVector_, Frame::GCRF());

    EXPECT_EQ(3, contribution.size());
    EXPECT_GT(1e-15, -4.620543790697659e-07 - contribution[0]);
    EXPECT_GT(1e-15, 2.948717888154649e-07 - contribution[1]);
    EXPECT_GT(1e-15, 1.301648617451192e-07 - contribution[2]);
}
