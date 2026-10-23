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

using ostk::mathematics::object::MatrixXd;
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
            closeApproach.computeRelativeSpeed().in(Derived::Unit::MeterPerSecond()),
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
        EXPECT_EQ(closeApproach.computeMissDistance(), swappedCloseApproach.computeMissDistance());
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, ComputeRelativeState)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();
        const Shared<const Frame> itrfFrame = Frame::ITRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State =
            buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3)).inFrame(itrfFrame);

        const CloseApproach closeApproach(object1State, object2State);

        const State relativeState = closeApproach.computeRelativeState();

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
            try { closeApproach.computeRelativeState(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, ComputeMissDistance)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Length missDistance = closeApproach.computeMissDistance();

        // Expected miss distance: sqrt((7.0e6)^2 + (7.0e6)^2) = 9899494.936611665 m
        const double expectedMissDistance = std::sqrt(7.0e6 * 7.0e6 + 7.0e6 * 7.0e6);

        EXPECT_NEAR(missDistance.inMeters(), expectedMissDistance, 1e-6);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.computeMissDistance(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, ComputeRelativeSpeed)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        const Derived relativeSpeed = closeApproach.computeRelativeSpeed();

        // Expected relative speed: sqrt(0^2 + (-8e3)^2 + (8e3)^2) = 11313.708498984761 m/s
        const double expectedRelativeSpeed = std::sqrt(8.0e3 * 8.0e3 + 8.0e3 * 8.0e3);

        EXPECT_EQ(Derived::Unit::MeterPerSecond(), relativeSpeed.getUnit());
        EXPECT_NEAR(relativeSpeed.in(Derived::Unit::MeterPerSecond()), expectedRelativeSpeed, 1e-6);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.computeRelativeSpeed(); } catch (const ostk::core::error::runtime::Undefined& e) {
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

        const LocalOrbitalFrameFactory qswFactory = LocalOrbitalFrameFactory::QSW(gcrfFrame);
        const Shared<const Frame> qswFrameSPtr = qswFactory.generateFrame(object1State);

        const Tuple<Length, Length, Length> missDistanceComponents =
            closeApproach.computeMissDistanceComponentsInFrame(qswFrameSPtr);
        const Tuple<Length, Length, Length> expectedMissDistanceComponents =
            closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(qswFactory);

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

        const LocalOrbitalFrameFactory qswFactory = LocalOrbitalFrameFactory::QSW(gcrfFrame);

        const Tuple<Length, Length, Length> missDistanceComponents =
            closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(qswFactory);

        EXPECT_NEAR(std::get<0>(missDistanceComponents).inMeters(), -7.0e6, 1e-3);
        EXPECT_NEAR(std::get<1>(missDistanceComponents).inMeters(), 7.0e6, 1e-3);
        EXPECT_NEAR(std::get<2>(missDistanceComponents).inMeters(), 0.0, 1e-3);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        const LocalOrbitalFrameFactory qswFactory = LocalOrbitalFrameFactory::QSW(Frame::GCRF());

        EXPECT_THROW(
            try {
                closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(qswFactory);
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

        const LocalOrbitalFrameFactory undefinedFactory = LocalOrbitalFrameFactory::Undefined();

        EXPECT_THROW(
            try {
                closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(undefinedFactory);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{LocalOrbitalFrameFactory} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, ComputeEncounterFrame)
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

        const Shared<const Frame> encounterFrame = closeApproach.computeEncounterFrame();

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

        const Shared<const Frame> defaultEncounterFrame = closeApproach.computeEncounterFrame();
        const Shared<const Frame> explicitGcrfEncounterFrame = closeApproach.computeEncounterFrame(gcrfFrame);

        EXPECT_EQ(defaultEncounterFrame, explicitGcrfEncounterFrame);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_THROW(
            try {
                closeApproach.computeEncounterFrame(nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                closeApproach.computeEncounterFrame(Frame::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                closeApproach.computeEncounterFrame(Frame::ITRF());
            } catch (const ostk::core::error::runtime::Wrong& e) {
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

        EXPECT_THROW(closeApproach.computeEncounterFrame(), ostk::core::error::RuntimeError);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State =
            buildState(instant, gcrfFrame, Vector3d(8.0e6, 0.0, 0.0), Vector3d(1.0e3, 8.0e3, 0.0));

        const CloseApproach closeApproach(object1State, object2State);

        EXPECT_THROW(closeApproach.computeEncounterFrame(), ostk::core::error::RuntimeError);
    }

    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try { closeApproach.computeEncounterFrame(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, ComputeRotatedEncounterFrameParameters)
{
    // Head-on encounter: the relative velocity is along the GCRF -Y axis, hence the encounter plane is the GCRF X-Z
    // plane, and Object 2 is 60 m along X and 80 m along Z from Object 1 (a 100 m miss distance). The GCRF Y position
    // standard deviations (along the relative velocity) do not contribute.
    const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
    const Shared<const Frame> gcrfFrame = Frame::GCRF();

    const auto buildCloseApproach = [&](const Vector3d& anObject1PositionSigmas,
                                        const Vector3d& anObject2PositionSigmas) -> CloseApproach
    {
        return {
            buildStateWithCovariance(
                instant,
                gcrfFrame,
                Vector3d(7.0e6, 0.0, 0.0),
                Vector3d(0.0, 7.5e3, 0.0),
                Covariance::FromPositionVelocitySigmas(
                    instant, anObject1PositionSigmas, Vector3d(0.01, 0.01, 0.01), gcrfFrame
                )
            ),
            buildStateWithCovariance(
                instant,
                gcrfFrame,
                Vector3d(7.0e6 + 60.0, 0.0, 80.0),
                Vector3d(0.0, -7.5e3, 0.0),
                Covariance::FromPositionVelocitySigmas(
                    instant, anObject2PositionSigmas, Vector3d(0.01, 0.01, 0.01), gcrfFrame
                )
            ),
        };
    };

    // Combined standard deviations: 10 m along X (6 m and 8 m), 5 m along Z (3 m and 4 m). The major principal axis is
    // along X, hence the 60 m miss distance component is along the major principal axis.
    {
        const CloseApproach closeApproach = buildCloseApproach({6.0, 50.0, 3.0}, {8.0, 50.0, 4.0});
        const Shared<const Frame> encounterFrameSPtr = closeApproach.computeEncounterFrame();

        const CloseApproach::RotatedEncounterFrameParameters parameters =
            closeApproach.computeRotatedEncounterFrameParameters(encounterFrameSPtr);

        EXPECT_NEAR(parameters.xM.inMeters(), 60.0, 1e-6);
        EXPECT_NEAR(parameters.yM.inMeters(), 80.0, 1e-6);
        EXPECT_NEAR(parameters.sigmaX.inMeters(), 10.0, 1e-6);
        EXPECT_NEAR(parameters.sigmaY.inMeters(), 5.0, 1e-6);

        // The rotated encounter frame is a child of the encounter frame, centered on Object 1
        const Shared<const Frame> rotatedEncounterFrameSPtr = parameters.rotatedEncounterFrameSPtr;

        ASSERT_TRUE(rotatedEncounterFrameSPtr != nullptr);
        EXPECT_TRUE(rotatedEncounterFrameSPtr->isDefined());
        EXPECT_EQ(encounterFrameSPtr, rotatedEncounterFrameSPtr->accessParent());

        // In the rotated encounter frame, the miss distance lies in the x-y plane, with the 60 m component along x
        const Tuple<Length, Length, Length> missDistanceComponents =
            closeApproach.computeMissDistanceComponentsInFrame(rotatedEncounterFrameSPtr);

        EXPECT_NEAR(std::abs(std::get<0>(missDistanceComponents).inMeters()), 60.0, 1e-6);
        EXPECT_NEAR(std::abs(std::get<1>(missDistanceComponents).inMeters()), 80.0, 1e-6);
        EXPECT_NEAR(std::get<2>(missDistanceComponents).inMeters(), 0.0, 1e-6);

        // In the rotated encounter frame, the combined position covariance is diagonal in the x-y plane
        const Covariance combinedCovariance = Covariance(
            instant,
            closeApproach.getObject1State().getCovariance()->reduce({CartesianPosition::Default()}).getCoordinates() +
                closeApproach.getObject2State()
                    .getCovariance()
                    ->reduce({CartesianPosition::Default()})
                    .getCoordinates(),
            gcrfFrame,
            {CartesianPosition::Default()}
        );
        const MatrixXd combinedCovarianceInRotatedEncounterFrame =
            combinedCovariance.rotate(rotatedEncounterFrameSPtr).getCoordinates();

        EXPECT_NEAR(combinedCovarianceInRotatedEncounterFrame(0, 0), 100.0, 1e-6);
        EXPECT_NEAR(combinedCovarianceInRotatedEncounterFrame(1, 1), 25.0, 1e-6);
        EXPECT_NEAR(combinedCovarianceInRotatedEncounterFrame(0, 1), 0.0, 1e-6);

        // The same rotated encounter frame is returned for the same close approach
        EXPECT_EQ(
            rotatedEncounterFrameSPtr,
            closeApproach.computeRotatedEncounterFrameParameters(encounterFrameSPtr).rotatedEncounterFrameSPtr
        );
    }

    // Combined standard deviations: 5 m along X, 10 m along Z. The major principal axis is now along Z, hence the 80 m
    // miss distance component is along the major principal axis.
    {
        const CloseApproach closeApproach = buildCloseApproach({3.0, 50.0, 6.0}, {4.0, 50.0, 8.0});

        const CloseApproach::RotatedEncounterFrameParameters parameters =
            closeApproach.computeRotatedEncounterFrameParameters(closeApproach.computeEncounterFrame());

        EXPECT_NEAR(parameters.xM.inMeters(), 80.0, 1e-6);
        EXPECT_NEAR(parameters.yM.inMeters(), 60.0, 1e-6);
        EXPECT_NEAR(parameters.sigmaX.inMeters(), 10.0, 1e-6);
        EXPECT_NEAR(parameters.sigmaY.inMeters(), 5.0, 1e-6);
    }

    // Combined standard deviations: 10 m along X, 5 m along Z, with the x-axis along the minor principal axis (along
    // Z), hence the 80 m miss distance component is along the x-axis
    {
        const CloseApproach closeApproach = buildCloseApproach({6.0, 50.0, 3.0}, {8.0, 50.0, 4.0});
        const Shared<const Frame> encounterFrameSPtr = closeApproach.computeEncounterFrame();

        const CloseApproach::RotatedEncounterFrameParameters parameters =
            closeApproach.computeRotatedEncounterFrameParameters(encounterFrameSPtr, false);

        EXPECT_NEAR(parameters.xM.inMeters(), 80.0, 1e-6);
        EXPECT_NEAR(parameters.yM.inMeters(), 60.0, 1e-6);
        EXPECT_NEAR(parameters.sigmaX.inMeters(), 5.0, 1e-6);
        EXPECT_NEAR(parameters.sigmaY.inMeters(), 10.0, 1e-6);

        // In the rotated encounter frame, the miss distance lies in the x-y plane, with the 80 m component along x
        const Shared<const Frame> rotatedEncounterFrameSPtr = parameters.rotatedEncounterFrameSPtr;

        ASSERT_TRUE(rotatedEncounterFrameSPtr != nullptr);
        EXPECT_EQ(encounterFrameSPtr, rotatedEncounterFrameSPtr->accessParent());

        const Tuple<Length, Length, Length> missDistanceComponents =
            closeApproach.computeMissDistanceComponentsInFrame(rotatedEncounterFrameSPtr);

        EXPECT_NEAR(std::abs(std::get<0>(missDistanceComponents).inMeters()), 80.0, 1e-6);
        EXPECT_NEAR(std::abs(std::get<1>(missDistanceComponents).inMeters()), 60.0, 1e-6);
        EXPECT_NEAR(std::get<2>(missDistanceComponents).inMeters(), 0.0, 1e-6);

        // The rotated encounter frame differs from the one with the x-axis along the major principal axis
        EXPECT_NE(
            rotatedEncounterFrameSPtr,
            closeApproach.computeRotatedEncounterFrameParameters(encounterFrameSPtr, true).rotatedEncounterFrameSPtr
        );
    }

    // Combined standard deviations: 5 m along X, 10 m along Z, with the x-axis along the minor principal axis (along
    // X), hence the 60 m miss distance component is along the x-axis
    {
        const CloseApproach closeApproach = buildCloseApproach({3.0, 50.0, 6.0}, {4.0, 50.0, 8.0});

        const CloseApproach::RotatedEncounterFrameParameters parameters =
            closeApproach.computeRotatedEncounterFrameParameters(closeApproach.computeEncounterFrame(), false);

        EXPECT_NEAR(parameters.xM.inMeters(), 60.0, 1e-6);
        EXPECT_NEAR(parameters.yM.inMeters(), 80.0, 1e-6);
        EXPECT_NEAR(parameters.sigmaX.inMeters(), 5.0, 1e-6);
        EXPECT_NEAR(parameters.sigmaY.inMeters(), 10.0, 1e-6);
    }

    // Undefined encounter frame
    {
        const CloseApproach closeApproach = buildCloseApproach({6.0, 50.0, 3.0}, {8.0, 50.0, 4.0});

        EXPECT_THROW(
            try {
                closeApproach.computeRotatedEncounterFrameParameters(nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Encounter frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                closeApproach.computeRotatedEncounterFrameParameters(Frame::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Encounter frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    // Missing position covariances
    {
        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 7.5e3, 0.0));
        const State object2State =
            buildState(instant, gcrfFrame, Vector3d(7.0e6 + 60.0, 0.0, 80.0), Vector3d(0.0, -7.5e3, 0.0));

        const Covariance covariance = Covariance::FromPositionVelocitySigmas(
            instant, Vector3d(1.0, 1.0, 1.0), Vector3d(0.01, 0.01, 0.01), gcrfFrame
        );

        {
            const CloseApproach closeApproach = {object1State, object2State.withCovariance(covariance)};

            EXPECT_THROW(
                try {
                    closeApproach.computeRotatedEncounterFrameParameters(closeApproach.computeEncounterFrame());
                } catch (const ostk::core::error::runtime::Undefined& e) {
                    EXPECT_EQ("{Object 1 position covariance} is undefined.", e.getMessage());
                    throw;
                },
                ostk::core::error::runtime::Undefined
            );
        }

        {
            const CloseApproach closeApproach = {object1State.withCovariance(covariance), object2State};

            EXPECT_THROW(
                try {
                    closeApproach.computeRotatedEncounterFrameParameters(closeApproach.computeEncounterFrame());
                } catch (const ostk::core::error::runtime::Undefined& e) {
                    EXPECT_EQ("{Object 2 position covariance} is undefined.", e.getMessage());
                    throw;
                },
                ostk::core::error::runtime::Undefined
            );
        }
    }

    // Undefined close approach
    {
        const CloseApproach closeApproach = CloseApproach::Undefined();

        EXPECT_THROW(
            try {
                closeApproach.computeRotatedEncounterFrameParameters(gcrfFrame);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{CloseApproach} is undefined.", e.getMessage());
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
