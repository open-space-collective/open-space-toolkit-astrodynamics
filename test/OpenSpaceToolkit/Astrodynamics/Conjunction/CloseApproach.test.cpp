/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Length;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, EqualToOperator)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach1(object1State, object2State);
        const CloseApproach closeApproach2(object1State, object2State);

        EXPECT_TRUE(closeApproach1 == closeApproach2);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));
        const State object3State = buildState(instant, gcrfFrame, Vector3d(0.0, 8.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach1(object1State, object2State);
        const CloseApproach closeApproach2(object1State, object3State);

        EXPECT_FALSE(closeApproach1 == closeApproach2);
    }

    {
        const CloseApproach closeApproach1 = CloseApproach::Undefined();
        const CloseApproach closeApproach2 = CloseApproach::Undefined();

        EXPECT_FALSE(closeApproach1 == closeApproach2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach, NotEqualToOperator)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach1(object1State, object2State);
        const CloseApproach closeApproach2(object1State, object2State);

        EXPECT_FALSE(closeApproach1 != closeApproach2);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrame = Frame::GCRF();

        const State object1State = buildState(instant, gcrfFrame, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 8.0e3, 0.0));
        const State object2State = buildState(instant, gcrfFrame, Vector3d(0.0, 7.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));
        const State object3State = buildState(instant, gcrfFrame, Vector3d(0.0, 8.0e6, 0.0), Vector3d(0.0, 0.0, 8.0e3));

        const CloseApproach closeApproach1(object1State, object2State);
        const CloseApproach closeApproach2(object1State, object3State);

        EXPECT_TRUE(closeApproach1 != closeApproach2);
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
