/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::ctnr::Array;
using ostk::core::ctnr::Map;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::StateBuilder;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder : public ::testing::Test
{
   protected:
    const Array<Shared<const CoordinatesSubset>> posVelSubsets = {
        CartesianPosition::Default(), CartesianVelocity::Default()
    };

    const Array<Shared<const CoordinatesSubset>> posVelMassSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    };

    const Array<Shared<const CoordinatesSubset>> massPosSubsets = {
        CoordinatesSubset::Mass(),
        CartesianPosition::Default(),
    };

    const Shared<const CoordinatesBroker> posVelBrokerSPtr =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker(posVelSubsets));

    const Shared<const CoordinatesBroker> posVelMassBrokerSPtr =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker(posVelMassSubsets));

    const Shared<const CoordinatesBroker> massPosBrokerSPtr =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker(massPosSubsets));
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

        EXPECT_FALSE(aStateBuilder == anotherStateBuilder);
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

        EXPECT_TRUE(aStateBuilder != anotherStateBuilder);
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, BuildFromCoordinates)
{
    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State state = stateBuilder.buildState(instant, coordinates);

        EXPECT_EQ(instant, state.accessInstant());
        EXPECT_EQ(coordinates, state.accessCoordinates());
        EXPECT_EQ(Frame::GCRF(), state.accessFrame());
        EXPECT_EQ(posVelBrokerSPtr, state.accessCoordinatesBroker());

        EXPECT_EQ(stateBuilder.accessFrame(), state.accessFrame());
        EXPECT_EQ(stateBuilder.accessCoordinatesBroker(), state.accessCoordinatesBroker());
    }

    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Instant instant1 = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates1(6);
        coordinates1 << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State state1 = stateBuilder.buildState(instant1, coordinates1);

        const Instant instant2 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates2(6);
        coordinates2 << 2.0, 3.0, 4.0, 5.0, 6.0, 7.0;

        const State state2 = stateBuilder.buildState(instant2, coordinates2);

        EXPECT_NE(state1.accessInstant(), state2.accessInstant());
        EXPECT_NE(state1.accessCoordinates(), state2.accessCoordinates());

        EXPECT_EQ(state1.accessFrame(), state2.accessFrame());
        EXPECT_EQ(state1.accessCoordinatesBroker(), state2.accessCoordinatesBroker());
    }

    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(3);
        coordinates << 1.0, 2.0, 3.0;

        EXPECT_ANY_THROW(stateBuilder.buildState(instant, coordinates));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        EXPECT_ANY_THROW(StateBuilder::Undefined().buildState(instant, coordinates));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, BuildFromState)
{
    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State aState = aStateBuilder.buildState(instant, coordinates);

        VectorXd massCoordinates(1);
        massCoordinates << 100.0;
        const Map<const Shared<const CoordinatesSubset>, const VectorXd> additionalCoordinatesMap = {
            {CoordinatesSubset::Mass(), massCoordinates}
        };
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), massPosBrokerSPtr};

        const State anotherState = anotherStateBuilder.build(aState, additionalCoordinatesMap);

        VectorXd expectedAnotherCoordinates(4);
        expectedAnotherCoordinates << 100.0, 1.0, 2.0, 3.0;

        EXPECT_FALSE(aState == anotherState);
        EXPECT_EQ(aState.accessFrame(), anotherState.accessFrame());
        EXPECT_EQ(coordinates, aState.accessCoordinates());
        EXPECT_EQ(expectedAnotherCoordinates, anotherState.accessCoordinates());
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State aState = aStateBuilder.buildState(instant, coordinates);

        VectorXd massCoordinates(1);
        massCoordinates << 100.0;
        const Map<const Shared<const CoordinatesSubset>, const VectorXd> additionalCoordinatesMap = {
            {CoordinatesSubset::Mass(), massCoordinates}
        };
        const StateBuilder anotherStateBuilder = StateBuilder::Undefined();  // Undefined should cause an error

        EXPECT_ANY_THROW(anotherStateBuilder.build(aState, additionalCoordinatesMap));
    }

    {
        const State aState = State::Undefined();  // Undefined should cause the error

        VectorXd massCoordinates(1);
        massCoordinates << 100.0;
        const Map<const Shared<const CoordinatesSubset>, const VectorXd> additionalCoordinatesMap = {
            {CoordinatesSubset::Mass(), massCoordinates}
        };
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), massPosBrokerSPtr};

        EXPECT_ANY_THROW(anotherStateBuilder.build(aState, additionalCoordinatesMap));
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State aState = aStateBuilder.buildState(instant, coordinates);

        VectorXd massCoordinates(1);
        massCoordinates << 100.0;
        const Map<const Shared<const CoordinatesSubset>, const VectorXd> additionalCoordinatesMap = {
            {CoordinatesSubset::Mass(), massCoordinates}
        };
        const StateBuilder anotherStateBuilder = {
            Frame::ITRF(), massPosBrokerSPtr
        };  // Different Frame should cause an error

        EXPECT_ANY_THROW(anotherStateBuilder.build(aState, additionalCoordinatesMap));
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State aState = aStateBuilder.buildState(instant, coordinates);

        const Map<const Shared<const CoordinatesSubset>, const VectorXd> additionalCoordinatesMap = {
        };  // Missing mass should cause an error
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), massPosBrokerSPtr};

        EXPECT_ANY_THROW(anotherStateBuilder.build(aState, additionalCoordinatesMap));
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State aState = aStateBuilder.buildState(instant, coordinates);

        VectorXd massCoordinates(1);
        massCoordinates << 100.0;
        VectorXd posCoordinates(3);
        posCoordinates << -1.0, -2.0, -3.0;
        const Map<const Shared<const CoordinatesSubset>, const VectorXd> additionalCoordinatesMap = {
            {CoordinatesSubset::Mass(), massCoordinates},
            {CartesianPosition::Default(), posCoordinates}  // Duplicate Cartesian position should cause an error
        };
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), massPosBrokerSPtr};

        EXPECT_ANY_THROW(anotherStateBuilder.build(aState, additionalCoordinatesMap));
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State aState = aStateBuilder.buildState(instant, coordinates);

        VectorXd massCoordinates(2);  // Mass coordinates size should cause an error
        massCoordinates << 100.0, 200.0;
        const Map<const Shared<const CoordinatesSubset>, const VectorXd> additionalCoordinatesMap = {
            {CoordinatesSubset::Mass(), massCoordinates}
        };
        const StateBuilder anotherStateBuilder = {Frame::GCRF(), massPosBrokerSPtr};

        EXPECT_ANY_THROW(anotherStateBuilder.build(aState, additionalCoordinatesMap));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Accessors)
{
    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_EQ(Frame::GCRF(), stateBuilder.accessFrame());
        EXPECT_EQ(posVelBrokerSPtr, stateBuilder.accessCoordinatesBroker());
    }

    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelSubsets};

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Expand)
{
    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);
        const StateBuilder expandedStateBuilder = stateBuilder.expand(CoordinatesSubset::Mass());

        EXPECT_FALSE(stateBuilder == expandedStateBuilder);

        const Array<Shared<const CoordinatesSubset>> subsets = stateBuilder.accessCoordinatesBroker()->accessSubsets();
        EXPECT_EQ(2, subsets.size());
        EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
        EXPECT_EQ(CartesianVelocity::Default(), subsets[1]);

        const Array<Shared<const CoordinatesSubset>> expandedSubsets =
            expandedStateBuilder.accessCoordinatesBroker()->accessSubsets();
        EXPECT_EQ(3, expandedSubsets.size());
        EXPECT_EQ(CartesianPosition::Default(), expandedSubsets[0]);
        EXPECT_EQ(CartesianVelocity::Default(), expandedSubsets[1]);
        EXPECT_EQ(CoordinatesSubset::Mass(), expandedSubsets[2]);
    }

    {
        const StateBuilder stateBuilder = StateBuilder::Undefined();

        EXPECT_ANY_THROW(stateBuilder.expand(CartesianPosition::Default()));
    }

    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder.expand(CartesianPosition::Default()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, contract)
{
    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelMassBrokerSPtr);
        const StateBuilder contractedStateBuilder = stateBuilder.contract(CartesianVelocity::Default());

        EXPECT_FALSE(stateBuilder == contractedStateBuilder);

        const Array<Shared<const CoordinatesSubset>> subsets = stateBuilder.accessCoordinatesBroker()->accessSubsets();
        EXPECT_EQ(3, subsets.size());
        EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
        EXPECT_EQ(CartesianVelocity::Default(), subsets[1]);
        EXPECT_EQ(CoordinatesSubset::Mass(), subsets[2]);

        const Array<Shared<const CoordinatesSubset>> contractedSubsets =
            contractedStateBuilder.accessCoordinatesBroker()->accessSubsets();
        EXPECT_EQ(2, contractedSubsets.size());
        EXPECT_EQ(CartesianPosition::Default(), contractedSubsets[0]);
        EXPECT_EQ(CoordinatesSubset::Mass(), contractedSubsets[1]);
    }

    {
        const StateBuilder stateBuilder = StateBuilder::Undefined();

        EXPECT_ANY_THROW(stateBuilder.contract(CartesianPosition::Default()));
    }

    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder.contract(CoordinatesSubset::Mass()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Undefined)
{
    {
        EXPECT_NO_THROW(StateBuilder::Undefined());
    }
}
