/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemeris/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Spherical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AnalyticalThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::ephemeris::Analytical;
using ostk::physics::environment::gravitational::Spherical;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using GravitationalModel = ostk::physics::environment::gravitational::Model;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::AnalyticalThirdBodyGravity;
using ostk::astrodynamics::dynamics::ThirdBodyGravity;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity : public ::testing::Test
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
    const Shared<Moon> sphericalMoonSPtr_ = std::make_shared<Moon>(Moon::Spherical());

    const AnalyticalThirdBodyGravity defaultAnalyticalThirdBodyGravity_ = {sphericalMoonSPtr_};

    VectorXd startStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, Constructor)
{
    {
        EXPECT_NO_THROW(AnalyticalThirdBodyGravity analyticalThirdBodyGravity(sphericalMoonSPtr_));
    }

    {
        EXPECT_NO_THROW(AnalyticalThirdBodyGravity analyticalThirdBodyGravity(sphericalMoonSPtr_, "test"));
    }

    {
        const Shared<Sun> sunSPtr = std::make_shared<Sun>(Sun::Spherical());
        EXPECT_NO_THROW(AnalyticalThirdBodyGravity analyticalThirdBodyGravity(sunSPtr));
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
                    AnalyticalThirdBodyGravity analyticalThirdBodyGravity(std::make_shared<Celestial>(earth));
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
                    AnalyticalThirdBodyGravity analyticalThirdBodyGravity(std::make_shared<Celestial>(Earth::Spherical()
                    ));
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

    {
        // A celestial object providing no analytical model is rejected at construction

        const Derived gravitationalParameter = {324858592000000.0, GravitationalParameterSIUnit};
        const Length equatorialRadius = Length::Meters(6051800.0);

        const Shared<GravitationalModel> gravitationalModelSPtr = std::make_shared<Spherical>(
            GravitationalModel::Parameters {gravitationalParameter, equatorialRadius, 0.0, 0.0, 0.0}
        );

        const Celestial venus = {
            "Venus",
            Celestial::Type::Venus,
            gravitationalParameter,
            equatorialRadius,
            0.0,
            0.0,
            0.0,
            std::make_shared<Analytical>(Frame::GCRF()),
            gravitationalModelSPtr,
            nullptr,
            nullptr,
        };

        EXPECT_THROW(
            AnalyticalThirdBodyGravity analyticalThirdBodyGravity(std::make_shared<Celestial>(venus)),
            ostk::core::error::runtime::ToBeImplemented
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, IsDefined)
{
    {
        EXPECT_TRUE(defaultAnalyticalThirdBodyGravity_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultAnalyticalThirdBodyGravity_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultAnalyticalThirdBodyGravity_.print(std::cout, true));
        EXPECT_NO_THROW(defaultAnalyticalThirdBodyGravity_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, GetName)
{
    {
        EXPECT_TRUE(defaultAnalyticalThirdBodyGravity_.getName() != String::Empty());
    }

    {
        const String name = "test";
        const AnalyticalThirdBodyGravity analyticalThirdBodyGravity(sphericalMoonSPtr_, name);
        EXPECT_TRUE(analyticalThirdBodyGravity.getName() == name);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, GetCelestial)
{
    EXPECT_TRUE(defaultAnalyticalThirdBodyGravity_.getCelestial() == sphericalMoonSPtr_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, GetReadCoordinateSubsets)
{
    const Array<Shared<const CoordinateSubset>> subsets = defaultAnalyticalThirdBodyGravity_.getReadCoordinateSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, GetWriteCoordinateSubsets)
{
    const Array<Shared<const CoordinateSubset>> subsets =
        defaultAnalyticalThirdBodyGravity_.getWriteCoordinateSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianVelocity::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, ComputeContribution)
{
    const VectorXd contribution =
        defaultAnalyticalThirdBodyGravity_.computeContribution(startInstant_, startStateVector_, Frame::GCRF());

    EXPECT_EQ(3, contribution.size());
    EXPECT_NEAR(-4.6239790951505204e-07, contribution[0], 1e-17);
    EXPECT_NEAR(2.9506655205372565e-07, contribution[1], 1e-17);
    EXPECT_NEAR(1.3029879999033161e-07, contribution[2], 1e-17);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_AnalyticalThirdBodyGravity, ComputeContributionVersusEphemeris)
{
    // The analytical third body acceleration agrees with the ephemeris-based one to within the accuracy
    // of the analytical position of the third body

    const Array<Shared<Celestial>> celestialObjects = {
        std::make_shared<Moon>(Moon::Spherical()),
        std::make_shared<Sun>(Sun::Spherical()),
    };

    for (const auto& celestialObjectSPtr : celestialObjects)
    {
        const VectorXd analyticalContribution =
            AnalyticalThirdBodyGravity(celestialObjectSPtr)
                .computeContribution(startInstant_, startStateVector_, Frame::GCRF());
        const VectorXd ephemerisContribution =
            ThirdBodyGravity(celestialObjectSPtr).computeContribution(startInstant_, startStateVector_, Frame::GCRF());

        const Real relativeError =
            (analyticalContribution - ephemerisContribution).norm() / ephemerisContribution.norm();

        EXPECT_LT(relativeError, 0.01) << String::Format(
            "{}: relative error = {}", celestialObjectSPtr->getName(), relativeError.toString()
        );
    }
}
