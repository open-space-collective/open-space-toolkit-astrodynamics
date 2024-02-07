/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::type::Real;
using ostk::core::type::Index;
using ostk::core::container::Array;

using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::Vector3d;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;

using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::GuidanceLaw;
using ostk::astro::dynamics::Thruster;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

class MockGuidanceLaw : public GuidanceLaw
{
   public:
    MockGuidanceLaw(const String& aName)
        : GuidanceLaw(aName)
    {
    }

    Vector3d calculateThrustAccelerationAt(
        [[maybe_unused]] const Instant& anInstant,
        [[maybe_unused]] const Vector3d& aPositionCoordinates,
        [[maybe_unused]] const Vector3d& aVelocityCoordinates,
        [[maybe_unused]] const Real& aThrustDirection,
        [[maybe_unused]] const Shared<const Frame>& outputFrameSPtr
    ) const override
    {
        return {0.0, 0.0, 0.0};
    }
};

class OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster : public ::testing::Test
{
   protected:
    Shared<const MockGuidanceLaw> defaultGuidanceLaw_ =
        std::make_shared<MockGuidanceLaw>(MockGuidanceLaw("test guidance law"));
    const SatelliteSystem defaultSatelliteSystem_ = SatelliteSystem::Default();
    const String defaultName_ = "TestThruster";

    const Thruster defaultThruster_ = {defaultSatelliteSystem_, defaultGuidanceLaw_, defaultName_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, Constructor)
{
    EXPECT_NO_THROW(Thruster thruster(defaultSatelliteSystem_, defaultGuidanceLaw_, defaultName_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << defaultThruster_ << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(defaultThruster_.print(std::cout, true));
    EXPECT_NO_THROW(defaultThruster_.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, GetSatelliteSystem)
{
    {
        EXPECT_EQ(defaultThruster_.getSatelliteSystem(), defaultSatelliteSystem_);
    }

    {
        const Thruster undefinedThruster = {SatelliteSystem::Undefined(), defaultGuidanceLaw_, defaultName_};
        EXPECT_THROW(undefinedThruster.getSatelliteSystem(), ostk::core::error::runtime::Undefined);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, GetGuidanceLaw)
{
    EXPECT_NO_THROW(defaultThruster_.getGuidanceLaw());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, GetName)
{
    EXPECT_EQ(defaultThruster_.getName(), defaultName_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, GetReadCoordinatesSubsets)
{
    const Array<Shared<const CoordinatesSubset>> expectedSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    };

    const Array<Shared<const CoordinatesSubset>> readSubsets = defaultThruster_.getReadCoordinatesSubsets();

    for (Index i = 0; i < expectedSubsets.getSize(); ++i)
    {
        EXPECT_EQ(*readSubsets[i], *expectedSubsets[i]);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, GetWriteCoordinatesSubsets)
{
    const Array<Shared<const CoordinatesSubset>> expectedSubsets = {
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    };

    const Array<Shared<const CoordinatesSubset>> writeSubsets = defaultThruster_.getWriteCoordinatesSubsets();

    for (Index i = 0; i < expectedSubsets.getSize(); ++i)
    {
        EXPECT_EQ(*writeSubsets[i], *expectedSubsets[i]);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, IsDefined)
{
    EXPECT_TRUE(defaultThruster_.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster, ComputeContribution)
{
    {
        VectorXd coordinates(7);
        coordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 105.0;

        const VectorXd acceleration =
            defaultThruster_.computeContribution(Instant::J2000(), coordinates, Frame::GCRF());

        VectorXd expectedAcceleration(4);
        expectedAcceleration << 0.0, 0.0, 0.0, 0.0;

        EXPECT_TRUE(acceleration.isNear(expectedAcceleration, 1e-12));
    }
}

// Add more tests based on public methods of Thruster class
