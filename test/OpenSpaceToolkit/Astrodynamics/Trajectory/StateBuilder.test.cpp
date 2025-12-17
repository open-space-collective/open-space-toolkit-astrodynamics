/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder : public ::testing::Test
{
   protected:
    const Array<Shared<const CoordinateSubset>> posVelSubsets = {
        CartesianPosition::Default(), CartesianVelocity::Default()
    };

    const Array<Shared<const CoordinateSubset>> posVelMassSubsets = {
        CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()
    };

    const Array<Shared<const CoordinateSubset>> massPosSubsets = {
        CoordinateSubset::Mass(), CartesianPosition::Default()
    };

    const Shared<const CoordinateBroker> posVelBrokerSPtr =
        std::make_shared<CoordinateBroker>(CoordinateBroker(posVelSubsets));

    const Shared<const CoordinateBroker> posVelMassBrokerSPtr =
        std::make_shared<CoordinateBroker>(CoordinateBroker(posVelMassSubsets));

    const Shared<const CoordinateBroker> massPosBrokerSPtr =
        std::make_shared<CoordinateBroker>(CoordinateBroker(massPosSubsets));
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Constructor)
{
    {
        EXPECT_NO_THROW(StateBuilder builder(Frame::GCRF(), posVelBrokerSPtr));
    }

    {
        EXPECT_NO_THROW(StateBuilder builder(Frame::GCRF(), posVelSubsets););
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State state = State(instant, coordinates, Frame::GCRF(), posVelBrokerSPtr);

        EXPECT_NO_THROW(StateBuilder builder(state));
    }

    {
        EXPECT_ANY_THROW(StateBuilder builder(State::Undefined()));
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

        const Shared<const CoordinateBroker> posVelBrokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr2};

        EXPECT_TRUE(aStateBuilder == anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Shared<const CoordinateBroker> velPosBrokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianVelocity::Default(), CartesianPosition::Default()})
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

        const Shared<const CoordinateBroker> posVelBrokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const StateBuilder anotherStateBuilder = {Frame::GCRF(), posVelBrokerSPtr2};

        EXPECT_FALSE(aStateBuilder != anotherStateBuilder);
    }

    {
        const StateBuilder aStateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Shared<const CoordinateBroker> velPosBrokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianVelocity::Default(), CartesianPosition::Default()})
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, AdditionOperator)
{
    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);
        const StateBuilder expandedStateBuilder = stateBuilder + CoordinateSubset::Mass();

        EXPECT_FALSE(stateBuilder == expandedStateBuilder);

        const Array<Shared<const CoordinateSubset>> subsets = stateBuilder.accessCoordinateBroker()->accessSubsets();
        EXPECT_EQ(2, subsets.size());
        EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
        EXPECT_EQ(CartesianVelocity::Default(), subsets[1]);

        const Array<Shared<const CoordinateSubset>> expandedSubsets =
            expandedStateBuilder.accessCoordinateBroker()->accessSubsets();
        EXPECT_EQ(3, expandedSubsets.size());
        EXPECT_EQ(CartesianPosition::Default(), expandedSubsets[0]);
        EXPECT_EQ(CartesianVelocity::Default(), expandedSubsets[1]);
        EXPECT_EQ(CoordinateSubset::Mass(), expandedSubsets[2]);
    }

    {
        const StateBuilder stateBuilder = StateBuilder::Undefined();

        EXPECT_ANY_THROW(stateBuilder + CartesianPosition::Default());
    }

    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder + CartesianPosition::Default());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, SubtractionOperator)
{
    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelMassBrokerSPtr);
        const StateBuilder contractedStateBuilder = stateBuilder - CartesianVelocity::Default();

        EXPECT_FALSE(stateBuilder == contractedStateBuilder);

        const Array<Shared<const CoordinateSubset>> subsets = stateBuilder.accessCoordinateBroker()->accessSubsets();
        EXPECT_EQ(3, subsets.size());
        EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
        EXPECT_EQ(CartesianVelocity::Default(), subsets[1]);
        EXPECT_EQ(CoordinateSubset::Mass(), subsets[2]);

        const Array<Shared<const CoordinateSubset>> contractedSubsets =
            contractedStateBuilder.accessCoordinateBroker()->accessSubsets();
        EXPECT_EQ(2, contractedSubsets.size());
        EXPECT_EQ(CartesianPosition::Default(), contractedSubsets[0]);
        EXPECT_EQ(CoordinateSubset::Mass(), contractedSubsets[1]);
    }

    {
        const StateBuilder stateBuilder = StateBuilder::Undefined();

        EXPECT_ANY_THROW(stateBuilder - CartesianPosition::Default());
    }

    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder - CoordinateSubset::Mass());
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Build)
{
    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State state = stateBuilder.build(instant, coordinates);

        EXPECT_EQ(instant, state.accessInstant());
        EXPECT_EQ(coordinates, state.accessCoordinates());
        EXPECT_EQ(Frame::GCRF(), state.accessFrame());
        EXPECT_EQ(posVelBrokerSPtr, state.accessCoordinateBroker());

        EXPECT_EQ(stateBuilder.accessFrame(), state.accessFrame());
        EXPECT_EQ(stateBuilder.accessCoordinateBroker(), state.accessCoordinateBroker());
    }

    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Instant instant1 = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates1(6);
        coordinates1 << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State state1 = stateBuilder.build(instant1, coordinates1);

        const Instant instant2 = Instant::DateTime(DateTime(2019, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates2(6);
        coordinates2 << 2.0, 3.0, 4.0, 5.0, 6.0, 7.0;

        const State state2 = stateBuilder.build(instant2, coordinates2);

        EXPECT_NE(state1.accessInstant(), state2.accessInstant());
        EXPECT_NE(state1.accessCoordinates(), state2.accessCoordinates());

        EXPECT_EQ(state1.accessFrame(), state2.accessFrame());
        EXPECT_EQ(state1.accessCoordinateBroker(), state2.accessCoordinateBroker());
    }

    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(3);
        coordinates << 1.0, 2.0, 3.0;

        EXPECT_ANY_THROW(stateBuilder.build(instant, coordinates));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        EXPECT_ANY_THROW(StateBuilder::Undefined().build(instant, coordinates));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Reduce)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(7);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 100.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), posVelMassBrokerSPtr);

        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), massPosBrokerSPtr);

        const State anotherState = stateBuilder.reduce(aState);

        VectorXd expectedAnotherCoordinates(4);
        expectedAnotherCoordinates << 100.0, 1.0, 2.0, 3.0;

        EXPECT_FALSE(aState == anotherState);
        EXPECT_EQ(aState.accessInstant(), anotherState.accessInstant());
        EXPECT_EQ(aState.accessFrame(), anotherState.accessFrame());
        EXPECT_EQ(coordinates, aState.accessCoordinates());
        EXPECT_EQ(expectedAnotherCoordinates, anotherState.accessCoordinates());
    }

    {
        const State aState = State::Undefined();

        const StateBuilder stateBuilder = StateBuilder(Frame::ITRF(), massPosBrokerSPtr);  // Undefined

        EXPECT_ANY_THROW(stateBuilder.reduce(aState));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(7);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 100.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), posVelMassBrokerSPtr);

        const StateBuilder stateBuilder = StateBuilder::Undefined();  // Undefined

        EXPECT_ANY_THROW(stateBuilder.reduce(aState));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(7);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 100.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), posVelMassBrokerSPtr);

        const StateBuilder stateBuilder = StateBuilder(Frame::ITRF(), massPosBrokerSPtr);  // Different Frame

        const State anotherState = stateBuilder.reduce(aState);

        VectorXd expectedAnotherCoordinates(4);
        expectedAnotherCoordinates << 100.0, 1.0, 2.0, 3.0;

        EXPECT_FALSE(aState == anotherState);
        EXPECT_EQ(aState.accessInstant(), anotherState.accessInstant());
        EXPECT_EQ(aState.accessFrame(), anotherState.accessFrame());  // Uses State's frame, not StateBuilder's
        EXPECT_EQ(Frame::GCRF(), anotherState.accessFrame());
        EXPECT_EQ(coordinates, aState.accessCoordinates());
        EXPECT_EQ(expectedAnotherCoordinates, anotherState.accessCoordinates());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), posVelBrokerSPtr);  // Missing Mass

        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), massPosBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder.reduce(aState));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Expand)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(4);
        coordinates << 100.0, 1.0, 2.0, 3.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), massPosBrokerSPtr);

        VectorXd defaultCoordinates(6);
        defaultCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;
        const State defaultState = State(instant, defaultCoordinates, Frame::GCRF(), posVelBrokerSPtr);

        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelMassBrokerSPtr);

        const State anotherState = stateBuilder.expand(aState, defaultState);

        VectorXd expectedAnotherCoordinates(7);
        expectedAnotherCoordinates << 1.0, 2.0, 3.0, -4.0, -5.0, -6.0, 100.0;

        EXPECT_FALSE(aState == anotherState);
        EXPECT_FALSE(defaultState == anotherState);
        EXPECT_EQ(aState.accessInstant(), anotherState.accessInstant());
        EXPECT_EQ(aState.accessFrame(), anotherState.accessFrame());
        EXPECT_EQ(coordinates, aState.accessCoordinates());
        EXPECT_EQ(defaultCoordinates, defaultState.accessCoordinates());
        EXPECT_EQ(expectedAnotherCoordinates, anotherState.accessCoordinates());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const State aState = State::Undefined();  // Undefined

        VectorXd defaultCoordinates(6);
        defaultCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;
        const State defaultState = State(instant, defaultCoordinates, Frame::GCRF(), posVelBrokerSPtr);

        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelMassBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder.expand(aState, defaultState));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(4);
        coordinates << 100.0, 1.0, 2.0, 3.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), massPosBrokerSPtr);

        const State defaultState = State::Undefined();  // Undefined

        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelMassBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder.expand(aState, defaultState));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(4);
        coordinates << 100.0, 1.0, 2.0, 3.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), massPosBrokerSPtr);

        VectorXd defaultCoordinates(6);
        defaultCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;
        const State defaultState = State(instant, defaultCoordinates, Frame::GCRF(), posVelBrokerSPtr);

        const StateBuilder stateBuilder = StateBuilder::Undefined();  // Undefined

        EXPECT_ANY_THROW(stateBuilder.expand(aState, defaultState));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(4);
        coordinates << 100.0, 1.0, 2.0, 3.0;
        const State aState = State(instant, coordinates, Frame::ITRF(), massPosBrokerSPtr);  // Different Frame

        VectorXd defaultCoordinates(6);
        defaultCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;
        const State defaultState = State(instant, defaultCoordinates, Frame::GCRF(), posVelBrokerSPtr);

        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelMassBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder.expand(aState, defaultState));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(4);
        coordinates << 100.0, 1.0, 2.0, 3.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), massPosBrokerSPtr);

        VectorXd defaultCoordinates(6);
        defaultCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;
        const State defaultState =
            State(instant, defaultCoordinates, Frame::ITRF(), posVelBrokerSPtr);  // Different Frame

        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelMassBrokerSPtr);

        EXPECT_ANY_THROW(stateBuilder.expand(aState, defaultState));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(4);
        coordinates << 100.0, 1.0, 2.0, 3.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), massPosBrokerSPtr);

        VectorXd defaultCoordinates(6);
        defaultCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;
        const State defaultState = State(instant, defaultCoordinates, Frame::GCRF(), posVelBrokerSPtr);

        const StateBuilder stateBuilder = StateBuilder(Frame::ITRF(), posVelMassBrokerSPtr);  // Different Frame

        EXPECT_ANY_THROW(stateBuilder.expand(aState, defaultState));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State aState = State(instant, coordinates, Frame::GCRF(), posVelBrokerSPtr);

        VectorXd defaultCoordinates(4);
        defaultCoordinates << 100.0, -1.0, -2.0, -3.0;
        const State defaultState = State(instant, defaultCoordinates, Frame::GCRF(), massPosBrokerSPtr);

        const StateBuilder stateBuilder =
            StateBuilder(Frame::GCRF(), massPosBrokerSPtr);  // Not an expansion (missing velocity)

        EXPECT_ANY_THROW(stateBuilder.expand(aState, defaultState));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Accessors)
{
    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelBrokerSPtr};

        EXPECT_EQ(Frame::GCRF(), stateBuilder.accessFrame());
        EXPECT_EQ(posVelBrokerSPtr, stateBuilder.accessCoordinateBroker());
    }

    {
        const StateBuilder stateBuilder = {Frame::GCRF(), posVelSubsets};

        EXPECT_EQ(Frame::GCRF(), stateBuilder.accessFrame());
        EXPECT_EQ(posVelSubsets, stateBuilder.accessCoordinateBroker()->getSubsets());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const State state = State(instant, coordinates, Frame::GCRF(), posVelBrokerSPtr);

        const StateBuilder stateBuilder = {state};
        EXPECT_EQ(Frame::GCRF(), stateBuilder.accessFrame());
        EXPECT_EQ(posVelBrokerSPtr, stateBuilder.accessCoordinateBroker());
    }

    {
        EXPECT_ANY_THROW(StateBuilder::Undefined().accessFrame());
        EXPECT_ANY_THROW(StateBuilder::Undefined().accessCoordinateBroker());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Getters)
{
    {
        const StateBuilder stateBuilder = StateBuilder(Frame::GCRF(), posVelBrokerSPtr);

        EXPECT_EQ(stateBuilder.getCoordinateSubsets(), posVelBrokerSPtr->getSubsets());
        EXPECT_EQ(stateBuilder.getSize(), posVelBrokerSPtr->getNumberOfCoordinates());
        EXPECT_EQ(Frame::GCRF(), stateBuilder.getFrame());
    }

    {
        EXPECT_ANY_THROW(StateBuilder::Undefined().getCoordinateSubsets());
        EXPECT_ANY_THROW(StateBuilder::Undefined().getSize());
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
