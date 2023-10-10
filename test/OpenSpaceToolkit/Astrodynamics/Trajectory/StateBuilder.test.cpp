/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::ctnr::Array;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::units::Length;

using ostk::astro::trajectory::StateBuilder;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;


class OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder : public ::testing::Test
{
   protected:
    const Array<Shared<const CoordinatesSubset>> posVelSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default()
    };

    const Array<Shared<const CoordinatesSubset>> posVelMassSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    };

    const Shared<const CoordinatesBroker> posVelBrokerSPtr = std::make_shared<CoordinatesBroker>(
        CoordinatesBroker(posVelSubsets)
    );

    const Shared<const CoordinatesBroker> posVelMassBrokerSPtr = std::make_shared<CoordinatesBroker>(
        CoordinatesBroker(posVelMassSubsets)
    );
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Constructor)
{
    {
        EXPECT_NO_THROW(StateBuilder builder(Frame::GCRF(), posVelBrokerSPtr));  
    }

    {
        EXPECT_NO_THROW(StateBuilder builder(Frame::GCRF(), posVelSubsets););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, EqualToOperator)
{
    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_TRUE(aStateBuilder == anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelSubsets};
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_TRUE(aStateBuilder == anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const StateBuilder anotherStateBuilder = {Frame::ITRF(), posVelBrokerSPtr};

        EXPECT_FALSE(aStateBuilder == anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelMassBrokerSPtr};

        EXPECT_FALSE(aStateBuilder == anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Shared<const CoordinatesBroker> posVelBrokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr2};

        EXPECT_TRUE(aStateBuilder == anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Shared<const CoordinatesBroker> velPosBrokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianVelocity::Default(), CartesianPosition::Default()})
        );

        const StateBuilder anotherStateBuilder = {Frame::GCRF(), velPosBrokerSPtr};

        EXPECT_TRUE(aStateBuilder == anotherStateBuilder);
    }

    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_TRUE(stateBuilder == stateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_TRUE(aStateBuilder == anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_FALSE(StateBuilder::Undefined() == aStateBuilder);
        EXPECT_FALSE(aStateBuilder == StateBuilder::Undefined());
        EXPECT_FALSE(StateBuilder::Undefined() == StateBuilder::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, NotEqualToOperator)
{
    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_FALSE(aStateBuilder != anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelSubsets};
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_FALSE(aStateBuilder != anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const StateBuilder anotherStateBuilder = {Frame::ITRF(), posVelBrokerSPtr};

        EXPECT_TRUE(aStateBuilder != anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelMassBrokerSPtr};

        EXPECT_TRUE(aStateBuilder != anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Shared<const CoordinatesBroker> posVelBrokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr2};

        EXPECT_FALSE(aStateBuilder != anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Shared<const CoordinatesBroker> velPosBrokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianVelocity::Default(), CartesianPosition::Default()})
        );

        const StateBuilder anotherStateBuilder = {Frame::GCRF(), velPosBrokerSPtr};

        EXPECT_FALSE(aStateBuilder != anotherStateBuilder);
    }

    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_FALSE(stateBuilder != stateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_FALSE(aStateBuilder != anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_TRUE(StateBuilder::Undefined() != aStateBuilder);
        EXPECT_TRUE(aStateBuilder != StateBuilder::Undefined());
        EXPECT_TRUE(StateBuilder::Undefined() != StateBuilder::Undefined());
    }
}


TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, StreamOperator)
{
    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << aStateBuilder << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << StateBuilder::Undefined() << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Accessors)
{
    {
        const  StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_EQ(Frame::GCRF(), stateBuilder.accessFrame());
        EXPECT_EQ(posVelBrokerSPtr, stateBuilder.accessCoordinatesBroker());
    }

    {
        const  StateBuilder stateBuilder = {Frame::GCRF(), posVelSubsets};

        EXPECT_EQ(Frame::GCRF(), stateBuilder.accessFrame());
        EXPECT_EQ(posVelSubsets, stateBuilder.accessCoordinatesBroker()->getSubsets());
    }

    {
        EXPECT_ANY_THROW(StateBuilder::Undefined().accessFrame());
        EXPECT_ANY_THROW(StateBuilder::Undefined().accessCoordinatesBroker());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Getters)
{
    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);

        EXPECT_EQ(stateBuilder.getCoordinatesSubsets(), posVelBrokerSPtr->getSubsets());
        EXPECT_EQ(Frame::GCRF(), stateBuilder.getFrame());
    }

    {
        EXPECT_ANY_THROW(StateBuilder::Undefined().getCoordinatesSubsets());
        EXPECT_ANY_THROW(StateBuilder::Undefined().getFrame());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, IsDefined)
{
    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);

        EXPECT_TRUE(stateBuilder.isDefined());
    }

    {
        const StateBuilder stateBuilder = StateBuilder(nullptr, posVelBrokerSPtr);

        EXPECT_FALSE(stateBuilder.isDefined());
    }

    {
        const StateBuilder stateBuilder = StateBuilder(Frame::Undefined(), posVelBrokerSPtr);

        EXPECT_FALSE(stateBuilder.isDefined());
    }

    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), nullptr);

        EXPECT_FALSE(stateBuilder.isDefined());
    }

    {
        EXPECT_FALSE(StateBuilder::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Undefined)
{
    {
        EXPECT_NO_THROW(StateBuilder::Undefined());
    }
}
