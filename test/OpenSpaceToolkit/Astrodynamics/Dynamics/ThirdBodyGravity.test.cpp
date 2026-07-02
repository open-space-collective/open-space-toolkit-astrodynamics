/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemeris.hpp>
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
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::environment::Ephemeris;
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
using ostk::physics::unit::Time;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::ThirdBodyGravity;
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

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity, ComputeContributionMatchesGenericPathForPointMassBodies
)
{
    // The point-mass fast path (position + direct formula) must reproduce, to very high precision, what the
    // generic path (two `getGravitationalFieldAt` calls, each followed by an `inFrame` round-trip) computes.
    // This is exactly the computation this class performed prior to the point-mass optimization, so agreement
    // here confirms the optimization is numerically transparent for the real-world point-mass third bodies
    // used in practice (Sun/Moon, with either a SPICE-backed or an analytical ephemeris).
    const Array<Shared<const Celestial>> pointMassCelestialSPtrs = {
        std::make_shared<Celestial>(Sun::Default()),
        std::make_shared<Celestial>(Moon::Default()),
        std::make_shared<Celestial>(Sun::Analytical()),
        std::make_shared<Celestial>(Moon::Analytical()),
    };

    const Vector3d positionCoordinates = Vector3d(startStateVector_[0], startStateVector_[1], startStateVector_[2]);

    for (const Shared<const Celestial>& celestialSPtr : pointMassCelestialSPtrs)
    {
        const ThirdBodyGravity thirdBodyGravity(celestialSPtr);

        const VectorXd contribution =
            thirdBodyGravity.computeContribution(startInstant_, startStateVector_, Frame::GCRF());
        const Vector3d contributionVector = {contribution[0], contribution[1], contribution[2]};

        Vector3d expectedGravitationalAccelerationSI =
            -celestialSPtr->getGravitationalFieldAt(Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()), startInstant_)
                 .inFrame(Frame::GCRF(), startInstant_)
                 .getValue();
        expectedGravitationalAccelerationSI +=
            celestialSPtr->getGravitationalFieldAt(Position::Meters(positionCoordinates, Frame::GCRF()), startInstant_)
                .inFrame(Frame::GCRF(), startInstant_)
                .getValue();

        // Both terms of the third-body formula are ~1e4 times larger than their (near-cancelling) difference
        // for a body as distant as the Sun, so the two computation paths -- which round differently in their
        // last few bits since one goes through frame rotations and the other is direct vector arithmetic --
        // can disagree at ~1e-12 relative once that ~1e4 cancellation amplification is accounted for (observed
        // ~1.8e-12 for the Sun, ~2.4e-14 for the Moon). This is a floating-point property of the formula itself
        // (present in the original code too), not a correctness regression, so a slightly relaxed tolerance is
        // used here; it remains many orders of magnitude tighter than anything that could matter physically.
        EXPECT_TRUE(contributionVector.isApprox(expectedGravitationalAccelerationSI, 1e-9)) << celestialSPtr->getName();
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity,
    ComputeContributionUsesGenericPathForNonPointMassGravitationalModel
)
{
    // Build a synthetic third body that reuses the real (non-degenerate) Moon ephemeris but is attached to a
    // non-spherical (WGS84, oblate) gravitational model under a non-"Earth" name. No point-mass shortcut is
    // valid for this configuration, so `computeContribution` must fall back to the original two-call
    // `getGravitationalFieldAt` + `inFrame` path. This proves that path is real/reachable (not dead code) and
    // that it is not silently short-circuited by the point-mass formula.
    const Shared<Ephemeris> ephemerisSPtr(Moon::Default().accessEphemeris()->clone());

    const Shared<Celestial> oblateThirdBodySPtr = std::make_shared<Celestial>(
        "TestOblateThirdBody",
        Celestial::Type::Undefined,
        Derived(4902800000000.0, GravitationalParameterSIUnit),
        Length::Meters(6378137.0),
        0.0,
        0.0,
        0.0,
        ephemerisSPtr,
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::WGS84),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Undefined)
    );

    const ThirdBodyGravity thirdBodyGravity(oblateThirdBodySPtr);

    const VectorXd contribution = thirdBodyGravity.computeContribution(startInstant_, startStateVector_, Frame::GCRF());
    const Vector3d contributionVector = {contribution[0], contribution[1], contribution[2]};

    const Vector3d positionCoordinates = Vector3d(startStateVector_[0], startStateVector_[1], startStateVector_[2]);

    // Reference: manual replica of the generic (pre-optimization) two-call formula.
    Vector3d expectedGravitationalAccelerationSI =
        -oblateThirdBodySPtr->getGravitationalFieldAt(Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()), startInstant_)
             .inFrame(Frame::GCRF(), startInstant_)
             .getValue();
    expectedGravitationalAccelerationSI +=
        oblateThirdBodySPtr
            ->getGravitationalFieldAt(Position::Meters(positionCoordinates, Frame::GCRF()), startInstant_)
            .inFrame(Frame::GCRF(), startInstant_)
            .getValue();

    EXPECT_TRUE(contributionVector.isApprox(expectedGravitationalAccelerationSI, 1e-12));

    // Sanity check: the oblateness (J2) term is not negligible at lunar range relative to floating-point
    // noise, so if the fast (point-mass) path had incorrectly been used instead of the fallback, the two
    // results would differ well above the tolerance used above.
    const Vector3d thirdBodyPositionCoordinates =
        oblateThirdBodySPtr->getPositionIn(Frame::GCRF(), startInstant_).getCoordinates();
    const Vector3d relativePositionCoordinates = thirdBodyPositionCoordinates - positionCoordinates;
    const Real mu = oblateThirdBodySPtr->getGravitationalParameter().in(GravitationalParameterSIUnit);
    const Vector3d pointMassGravitationalAccelerationSI =
        mu * (relativePositionCoordinates / std::pow(relativePositionCoordinates.norm(), 3) -
              thirdBodyPositionCoordinates / std::pow(thirdBodyPositionCoordinates.norm(), 3));

    EXPECT_FALSE(contributionVector.isApprox(pointMassGravitationalAccelerationSI, 1e-6));
}
