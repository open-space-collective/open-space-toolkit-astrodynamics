/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Axes.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Axes;
using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;
using ostk::astrodynamics::uncertainty::Covariance;

class OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach : public ::testing::Test
{
   protected:
    // Helper function to build a state
    State buildState(
        const Instant& anInstant,
        const Shared<const Frame>& aFrame,
        const Vector3d& aPosition,
        const Vector3d& aVelocity
    )
    {
        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
        };

        const StateBuilder stateBuilder = StateBuilder(aFrame, coordinateSubsets);

        VectorXd coordinates(6);
        coordinates << aPosition, aVelocity;

        return stateBuilder.build(anInstant, coordinates);
    }

    // Helper function to build a state with a covariance attached
    State buildStateWithCovariance(
        const Instant& anInstant,
        const Shared<const Frame>& aFrame,
        const Vector3d& aPosition,
        const Vector3d& aVelocity,
        const Covariance& aCovariance
    )
    {
        return buildState(anInstant, aFrame, aPosition, aVelocity).withCovariance(aCovariance);
    }

    // Helper function to check that two covariances are equal
    void expectCovarianceEq(const Covariance& anExpectedCovariance, const Covariance& aCovariance)
    {
        EXPECT_EQ(anExpectedCovariance.getInstant(), aCovariance.getInstant());
        EXPECT_EQ(*anExpectedCovariance.getFrame(), *aCovariance.getFrame());
        EXPECT_EQ(anExpectedCovariance.getCoordinateSubsets(), aCovariance.getCoordinateSubsets());
        EXPECT_TRUE(aCovariance.getCoordinates().isNear(anExpectedCovariance.getCoordinates(), 1e-15));
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, Constructor)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        EXPECT_NO_THROW(CloseApproach closeApproach(object1State, object2State););
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const Covariance object1Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(1.0, 1.0, 1.0), Vector3d(0.01, 0.01, 0.01), gcrfFrame
        );
        const Covariance object2Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(2.0, 2.0, 2.0), Vector3d(0.02, 0.02, 0.02), gcrfFrame
        );

        const State object1State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0), object1Covariance
        );
        const State object2State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3), object2Covariance
        );

        EXPECT_NO_THROW(CloseApproach closeApproach(object1State, object2State););

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_TRUE(closeApproach.isDefined());
        expectCovarianceEq(object1Covariance, closeApproach.getObject1State().getCovariance().value());
        expectCovarianceEq(object2Covariance, closeApproach.getObject2State().getCovariance().value());
        EXPECT_NEAR(
            closeApproach.getRelativeSpeed().in(Derived::Unit::MeterPerSecond()),
            std::sqrt(8.0e3 * 8.0e3 + 8.0e3 * 8.0e3),
            1e-6
        );
    }

    {
        const Instant instant1 = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant instant2 = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 1), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State =
            buildState(instant1, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State =
            buildState(instant2, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        EXPECT_THROW(
            try {
                CloseApproach closeApproach(object1State, object2State);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Inconsistent state instants.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, StreamOperator)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << closeApproach << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, IsDefined)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_TRUE(closeApproach.isDefined());
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_FALSE(closeApproach.isDefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = State::Undefined();

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_FALSE(closeApproach.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, GetObject1State)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_EQ(object1State, closeApproach.getObject1State());
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.getObject1State(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, GetObject2State)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_EQ(object2State, closeApproach.getObject2State());
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.getObject2State(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, Scale)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const Covariance object1Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(1.0, 1.0, 1.0), Vector3d(0.01, 0.01, 0.01), gcrfFrame
        );
        const Covariance object2Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(2.0, 2.0, 2.0), Vector3d(0.02, 0.02, 0.02), gcrfFrame
        );

        const State object1State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0), object1Covariance
        );
        const State object2State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3), object2Covariance
        );

        const CloseApproach closeApproach(object1State, object2State);

        const CloseApproach scaledCloseApproach = closeApproach.scale(2.0, 4.0);

        EXPECT_EQ(object1State.getInstant(), scaledCloseApproach.getObject1State().getInstant());
        EXPECT_EQ(object1State.getCoordinates(), scaledCloseApproach.getObject1State().getCoordinates());
        EXPECT_EQ(object2State.getInstant(), scaledCloseApproach.getObject2State().getInstant());
        EXPECT_EQ(object2State.getCoordinates(), scaledCloseApproach.getObject2State().getCoordinates());
        expectCovarianceEq(object1Covariance.scale(2.0), scaledCloseApproach.getObject1State().getCovariance().value());
        expectCovarianceEq(object2Covariance.scale(4.0), scaledCloseApproach.getObject2State().getCovariance().value());
        expectCovarianceEq(object1Covariance, closeApproach.getObject1State().getCovariance().value());
        expectCovarianceEq(object2Covariance, closeApproach.getObject2State().getCovariance().value());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const Covariance object1Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(1.0, 1.0, 1.0), Vector3d(0.01, 0.01, 0.01), gcrfFrame
        );
        const Covariance object2Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(2.0, 2.0, 2.0), Vector3d(0.02, 0.02, 0.02), gcrfFrame
        );

        const State object1State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0), object1Covariance
        );
        const State object2State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3), object2Covariance
        );

        const CloseApproach closeApproach(object1State, object2State);

        const CloseApproach scaledObject1Only = closeApproach.scale(2.0, std::nullopt);
        const CloseApproach scaledObject2Only = closeApproach.scale(std::nullopt, 4.0);
        const CloseApproach unscaledCloseApproach = closeApproach.scale();

        expectCovarianceEq(object1Covariance.scale(2.0), scaledObject1Only.getObject1State().getCovariance().value());
        expectCovarianceEq(object2Covariance, scaledObject1Only.getObject2State().getCovariance().value());
        expectCovarianceEq(object1Covariance, scaledObject2Only.getObject1State().getCovariance().value());
        expectCovarianceEq(object2Covariance.scale(4.0), scaledObject2Only.getObject2State().getCovariance().value());
        expectCovarianceEq(object1Covariance, unscaledCloseApproach.getObject1State().getCovariance().value());
        expectCovarianceEq(object2Covariance, unscaledCloseApproach.getObject2State().getCovariance().value());
        EXPECT_EQ(closeApproach.getObject1State(), unscaledCloseApproach.getObject1State());
        EXPECT_EQ(closeApproach.getObject2State(), unscaledCloseApproach.getObject2State());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const Covariance object1Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(1.0, 1.0, 1.0), Vector3d(0.01, 0.01, 0.01), gcrfFrame
        );

        const State object1State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0), object1Covariance
        );
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        // Scaling only the object with a covariance attached works
        const CloseApproach scaledObject1Only = closeApproach.scale(2.0);

        expectCovarianceEq(object1Covariance.scale(2.0), scaledObject1Only.getObject1State().getCovariance().value());
        EXPECT_FALSE(scaledObject1Only.getObject2State().hasCovariance());

        // Scaling an object without a covariance attached leaves its state unchanged
        const CloseApproach scaledBoth = closeApproach.scale(2.0, 4.0);

        expectCovarianceEq(object1Covariance.scale(2.0), scaledBoth.getObject1State().getCovariance().value());
        EXPECT_FALSE(scaledBoth.getObject2State().hasCovariance());
        EXPECT_EQ(object2State, scaledBoth.getObject2State());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const CloseApproach scaledCloseApproach = closeApproach.scale();

        EXPECT_EQ(closeApproach.getObject1State(), scaledCloseApproach.getObject1State());
        EXPECT_EQ(closeApproach.getObject2State(), scaledCloseApproach.getObject2State());
        EXPECT_FALSE(scaledCloseApproach.getObject1State().hasCovariance());
        EXPECT_FALSE(scaledCloseApproach.getObject2State().hasCovariance());
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.scale(2.0, 4.0); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, Swap)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const Covariance object1Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(1.0, 1.0, 1.0), Vector3d(0.01, 0.01, 0.01), gcrfFrame
        );
        const Covariance object2Covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(2.0, 2.0, 2.0), Vector3d(0.02, 0.02, 0.02), gcrfFrame
        );

        const State object1State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0), object1Covariance
        );
        const State object2State = buildStateWithCovariance(
            instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3), object2Covariance
        );

        const CloseApproach closeApproach(object1State, object2State);

        const CloseApproach swappedCloseApproach = closeApproach.swap();

        EXPECT_TRUE(swappedCloseApproach.isDefined());
        EXPECT_EQ(object2State, swappedCloseApproach.getObject1State());
        EXPECT_EQ(object1State, swappedCloseApproach.getObject2State());
        expectCovarianceEq(object2Covariance, swappedCloseApproach.getObject1State().getCovariance().value());
        expectCovarianceEq(object1Covariance, swappedCloseApproach.getObject2State().getCovariance().value());
        EXPECT_EQ(closeApproach.getInstant(), swappedCloseApproach.getInstant());
        EXPECT_EQ(closeApproach.getMissDistance(), swappedCloseApproach.getMissDistance());
        EXPECT_EQ(object1State, closeApproach.getObject1State());
        EXPECT_EQ(object2State, closeApproach.getObject2State());
        EXPECT_EQ(closeApproach.getObject1State(), swappedCloseApproach.swap().getObject1State());
        EXPECT_EQ(closeApproach.getObject2State(), swappedCloseApproach.swap().getObject2State());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const CloseApproach swappedCloseApproach = closeApproach.swap();

        EXPECT_EQ(object2State, swappedCloseApproach.getObject1State());
        EXPECT_EQ(object1State, swappedCloseApproach.getObject2State());
        EXPECT_FALSE(swappedCloseApproach.getObject1State().hasCovariance());
        EXPECT_FALSE(swappedCloseApproach.getObject2State().hasCovariance());
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.swap(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, GetInstant)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_EQ(instant, closeApproach.getInstant());
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.getInstant(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, GetMissDistance)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Length missDistance = closeApproach.getMissDistance();

        // Expected miss distance: sqrt((7.0e6)^2 + (7.0e6)^2) = 9899494.936611665 m
        const double expectedMissDistance = std::sqrt(7.0e6 * 7.0e6 + 7.0e6 * 7.0e6);

        EXPECT_NEAR(missDistance.inMeters(), expectedMissDistance, 1e-6);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.getMissDistance(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, GetRelativeState)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();
        const Shared<const Frame> itrfFrame = Frame::ITRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State =
            buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3)).inFrame(itrfFrame);

        const CloseApproach closeApproach(object1State, object2State);

        const State relativeState = closeApproach.getRelativeState();

        EXPECT_EQ(instant, relativeState.getInstant());
        EXPECT_EQ(gcrfFrame, relativeState.accessFrame());

        const Array<Shared<const CoordinateSubset>> coordinateSubsets = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
        };

        const VectorXd coordinates = relativeState.extractCoordinates(coordinateSubsets);

        EXPECT_EQ(6, coordinates.size());

        // Expected relative position: [0, 7e6, 0] - [7e6, 0, 0] = [-7e6, 7e6, 0]
        EXPECT_NEAR(coordinates[0], -7.0e6, 1e-3);
        EXPECT_NEAR(coordinates[1], 7.0e6, 1e-3);
        EXPECT_NEAR(coordinates[2], 0.0, 1e-3);

        // Expected relative velocity: [0, 0, 8e3] - [0, 8e3, 0] = [0, -8e3, 8e3]
        EXPECT_NEAR(coordinates[3], 0.0, 1e-3);
        EXPECT_NEAR(coordinates[4], -8.0e3, 1e-3);
        EXPECT_NEAR(coordinates[5], 8.0e3, 1e-3);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.getRelativeState(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, GetRelativeSpeed)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Derived relativeSpeed = closeApproach.getRelativeSpeed();

        // Expected relative speed: sqrt(0^2 + (-8e3)^2 + (8e3)^2) = 11313.708498984761 m/s
        const double expectedRelativeSpeed = std::sqrt(8.0e3 * 8.0e3 + 8.0e3 * 8.0e3);

        EXPECT_EQ(Derived::Unit::MeterPerSecond(), relativeSpeed.getUnit());
        EXPECT_NEAR(relativeSpeed.in(Derived::Unit::MeterPerSecond()), expectedRelativeSpeed, 1e-6);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.getRelativeSpeed(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, GetEncounterFrame)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const Vector3d object1Position = {7.0e6, 0.0, 0.0};
        const Vector3d object1Velocity = {0.0, 8.0e3, 0.0};
        const Vector3d object2Position = {0.0, 7.0e6, 0.0};
        const Vector3d object2Velocity = {0.0, 0.0, 8.0e3};

        const State object1State = buildState(instant, gcrfFrame, object1Position, object1Velocity);
        const State object2State = buildState(instant, gcrfFrame, object2Position, object2Velocity);

        const CloseApproach closeApproach(object1State, object2State);

        const Shared<const Frame> encounterFrame = closeApproach.getEncounterFrame();

        EXPECT_TRUE(encounterFrame != nullptr);
        EXPECT_TRUE(encounterFrame->isDefined());
        EXPECT_EQ(gcrfFrame, encounterFrame->accessParent());

        const Vector3d relativePosition = object2Position - object1Position;
        const Vector3d relativeVelocity = object2Velocity - object1Velocity;
        const Vector3d expectedZAxis = relativeVelocity.normalized();
        const Vector3d expectedYAxis = expectedZAxis.cross(relativePosition).normalized();
        const Vector3d expectedXAxis = expectedYAxis.cross(expectedZAxis);

        const Axes axes = encounterFrame->getAxesIn(gcrfFrame, instant);

        EXPECT_NEAR((axes.x() - expectedXAxis).norm(), 0.0, 1e-12);
        EXPECT_NEAR((axes.y() - expectedYAxis).norm(), 0.0, 1e-12);
        EXPECT_NEAR((axes.z() - expectedZAxis).norm(), 0.0, 1e-12);
        EXPECT_NEAR(axes.x().cross(axes.y()).dot(axes.z()), 1.0, 1e-12);

        const State object1StateInEncounterFrame = object1State.inFrame(encounterFrame);
        const Vector3d object1PositionInEncounterFrame =
            object1StateInEncounterFrame.getPosition().inMeters().getCoordinates();

        EXPECT_NEAR(object1PositionInEncounterFrame.norm(), 0.0, 1e-6);

        const Shared<const Frame> defaultEncounterFrame = closeApproach.getEncounterFrame();
        const Shared<const Frame> explicitGcrfEncounterFrame = closeApproach.getEncounterFrame(gcrfFrame);

        EXPECT_EQ(defaultEncounterFrame, explicitGcrfEncounterFrame);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_THROW(
            try { closeApproach.getEncounterFrame(nullptr); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                closeApproach.getEncounterFrame(Frame::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try { closeApproach.getEncounterFrame(Frame::ITRF()); } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("Frame = ITRF is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 8.0e3, 0.0));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_THROW(closeApproach.getEncounterFrame(), ostk::core::error::RuntimeError);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State =
            buildState(instant, gcrfFrame, Vector3d(8.0e6, 0.0, 0.0), Vector3d(1.0e3, 8.0e3, 0.0));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_THROW(closeApproach.getEncounterFrame(), ostk::core::error::RuntimeError);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.getEncounterFrame(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, ComputeMissDistanceComponentsInFrame)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Tuple<Length, Length, Length> missDistanceComponents =
            closeApproach.computeMissDistanceComponentsInFrame(gcrfFrame);

        EXPECT_NEAR(std::get<0>(missDistanceComponents).inMeters(), -7.0e6, 1e-3);
        EXPECT_NEAR(std::get<1>(missDistanceComponents).inMeters(), 7.0e6, 1e-3);
        EXPECT_NEAR(std::get<2>(missDistanceComponents).inMeters(), 0.0, 1e-3);
    }

    // Rotating frame (ITRF): the relative position computed in GCRF, rotated into ITRF
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Tuple<Length, Length, Length> missDistanceComponents =
            closeApproach.computeMissDistanceComponentsInFrame(Frame::ITRF());

        // GCRF and ITRF share the same origin, hence the relative position is only rotated
        const Vector3d relativePositionCoordinatesInGCRF = object2State.getPosition().inMeters().getCoordinates() -
                                                           object1State.getPosition().inMeters().getCoordinates();

        const Vector3d expectedMissDistanceComponents = Position::Meters(relativePositionCoordinatesInGCRF, gcrfFrame)
                                                            .inFrame(Frame::ITRF(), instant)
                                                            .inMeters()
                                                            .getCoordinates();

        EXPECT_NEAR(std::get<0>(missDistanceComponents).inMeters(), expectedMissDistanceComponents(0), 1e-3);
        EXPECT_NEAR(std::get<1>(missDistanceComponents).inMeters(), expectedMissDistanceComponents(1), 1e-3);
        EXPECT_NEAR(std::get<2>(missDistanceComponents).inMeters(), expectedMissDistanceComponents(2), 1e-3);
    }

    // Frame not centered on the Earth (local orbital frame, centered on Object 1): matches the local orbital frame
    // components computed from the same factory
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Shared<const LocalOrbitalFrameFactory> qswFactorySPtr = LocalOrbitalFrameFactory::QSW(gcrfFrame);
        const Shared<const Frame> qswFrameSPtr = qswFactorySPtr->generateFrame(object1State);

        const Tuple<Length, Length, Length> missDistanceComponents =
            closeApproach.computeMissDistanceComponentsInFrame(qswFrameSPtr);
        const Tuple<Length, Length, Length> expectedMissDistanceComponents =
            closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(qswFactorySPtr);

        EXPECT_NEAR(
            std::get<0>(missDistanceComponents).inMeters(), std::get<0>(expectedMissDistanceComponents).inMeters(), 1e-3
        );
        EXPECT_NEAR(
            std::get<1>(missDistanceComponents).inMeters(), std::get<1>(expectedMissDistanceComponents).inMeters(), 1e-3
        );
        EXPECT_NEAR(
            std::get<2>(missDistanceComponents).inMeters(), std::get<2>(expectedMissDistanceComponents).inMeters(), 1e-3
        );
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try {
                closeApproach.computeMissDistanceComponentsInFrame(Frame::GCRF());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_THROW(
            try {
                closeApproach.computeMissDistanceComponentsInFrame(nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, ComputeMissDistanceComponentsInLocalOrbitalFrame)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Shared<const LocalOrbitalFrameFactory> qswFactorySPtr = LocalOrbitalFrameFactory::QSW(gcrfFrame);

        const Tuple<Length, Length, Length> missDistanceComponents =
            closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(qswFactorySPtr);

        EXPECT_NEAR(std::get<0>(missDistanceComponents).inMeters(), -7.0e6, 1e-3);
        EXPECT_NEAR(std::get<1>(missDistanceComponents).inMeters(), 7.0e6, 1e-3);
        EXPECT_NEAR(std::get<2>(missDistanceComponents).inMeters(), 0.0, 1e-3);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        const Shared<const LocalOrbitalFrameFactory> qswFactorySPtr = LocalOrbitalFrameFactory::QSW(Frame::GCRF());

        EXPECT_THROW(
            try {
                closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(qswFactorySPtr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Shared<const LocalOrbitalFrameFactory> undefinedFactorySPtr = LocalOrbitalFrameFactory::Undefined();

        EXPECT_THROW(
            try {
                closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(undefinedFactorySPtr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{LocalOrbitalFrameFactory} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_THROW(
            try {
                closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{LocalOrbitalFrameFactory} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, Print)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(closeApproach.print(std::cout, true));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(closeApproach.print(std::cout, false));

        const std::string output = testing::internal::GetCapturedStdout();

        EXPECT_FALSE(output.empty());
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(closeApproach.print(std::cout, true));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, Undefined)
{
    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_FALSE(closeApproach.isDefined());
    }
}
