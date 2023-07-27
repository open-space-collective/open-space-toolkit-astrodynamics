/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::units::Length;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::trajectory::State;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, Constructor)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        EXPECT_NO_THROW(State state(instant, coordinates, Frame::GCRF(), brokerSPtr));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        EXPECT_NO_THROW(State state(instant, position, velocity););
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::ITRF());

        EXPECT_ANY_THROW(State state(instant, position, velocity););
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::ITRF());

        EXPECT_ANY_THROW(State state(instant, position, velocity););
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Undefined();
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::ITRF());

        EXPECT_ANY_THROW(State state(instant, position, velocity););
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::Undefined();

        EXPECT_ANY_THROW(State state(instant, position, velocity););
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, EqualToOperator)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};
        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_TRUE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_TRUE(aState == anotherState);
    }

    {
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, coordinates, Frame::GCRF(), brokerSPtr};

        const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

        const State anotherState = {anotherInstant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        VectorXd aCoordinates(6);
        aCoordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State aState = {instant, aCoordinates, Frame::GCRF(), brokerSPtr};

        VectorXd anotherCoordinates(6);
        anotherCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;

        const State anotherState = {instant, anotherCoordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        const State anotherState = {instant, coordinates, Frame::ITRF(), brokerSPtr};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_TRUE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianVelocity::ThreeDimensional(), CartesianPosition::ThreeDimensional()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position, velocity};

        EXPECT_TRUE(state == state);
    }

    {
        const Position position = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {instant, position, velocity};

        const State anotherState = {instant, position, velocity};

        EXPECT_TRUE(aState == anotherState);
    }

    {
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, position, velocity};

        const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

        const State anotherState = {anotherInstant, position, velocity};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

        const Position aPosition = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

        const State aState = {instant, aPosition, velocity};

        const Position anotherPosition = Position::Meters({1001.0, 2001.0, 3001.0}, Frame::GCRF());

        const State anotherState = {instant, anotherPosition, velocity};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

        const Position aPosition = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

        const State aState = {instant, aPosition, velocity};

        const Position anotherPosition = aPosition.inUnit(Length::Unit::Foot);

        const State anotherState = {instant, anotherPosition, velocity};

        EXPECT_TRUE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

        const Velocity aVelocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

        const State aState = {instant, position, aVelocity};

        const Velocity anotherVelocity = Velocity::MetersPerSecond({11, 21, 31}, Frame::GCRF());

        const State anotherState = {instant, position, anotherVelocity};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position, velocity};

        EXPECT_FALSE(State::Undefined() == state);
        EXPECT_FALSE(state == State::Undefined());
        EXPECT_FALSE(State::Undefined() == State::Undefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, NotEqualToOperator)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_FALSE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_FALSE(aState != anotherState);
    }

    {
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        CoordinatesBroker broker = CoordinatesBroker();
        broker.addSubset(CartesianPosition::ThreeDimensional());
        broker.addSubset(CartesianVelocity::ThreeDimensional());
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<const CoordinatesBroker>(broker);

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, coordinates, Frame::GCRF(), brokerSPtr};

        const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

        const State anotherState = {anotherInstant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        CoordinatesBroker broker = CoordinatesBroker();
        broker.addSubset(CartesianPosition::ThreeDimensional());
        broker.addSubset(CartesianVelocity::ThreeDimensional());
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<const CoordinatesBroker>(broker);

        VectorXd aCoordinates(6);
        aCoordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State aState = {instant, aCoordinates, Frame::GCRF(), brokerSPtr};

        VectorXd anotherCoordinates(6);
        anotherCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;

        const State anotherState = {instant, anotherCoordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        const State anotherState = {instant, coordinates, Frame::ITRF(), brokerSPtr};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_FALSE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianVelocity::ThreeDimensional(), CartesianPosition::ThreeDimensional()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position, velocity};

        EXPECT_FALSE(state != state);
    }

    {
        const Position position = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {instant, position, velocity};

        const State anotherState = {instant, position, velocity};

        EXPECT_FALSE(aState != anotherState);
    }

    {
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, position, velocity};

        const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

        const State anotherState = {anotherInstant, position, velocity};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

        const Position aPosition = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

        const State aState = {instant, aPosition, velocity};

        const Position anotherPosition = Position::Meters({1001.0, 2001.0, 3001.0}, Frame::GCRF());

        const State anotherState = {instant, anotherPosition, velocity};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

        const Position aPosition = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

        const State aState = {instant, aPosition, velocity};

        const Position anotherPosition = aPosition.inUnit(Length::Unit::Foot);

        const State anotherState = {instant, anotherPosition, velocity};

        EXPECT_FALSE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

        const Velocity aVelocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

        const State aState = {instant, position, aVelocity};

        const Velocity anotherVelocity = Velocity::MetersPerSecond({11, 21, 31}, Frame::GCRF());

        const State anotherState = {instant, position, anotherVelocity};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position, velocity};

        EXPECT_TRUE(State::Undefined() != state);
        EXPECT_TRUE(state != State::Undefined());
        EXPECT_TRUE(State::Undefined() != State::Undefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, AdditionOperator)
{
    {
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, position, velocity};

        EXPECT_ANY_THROW(aState + State::Undefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const Position aPosition = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State aState = {instant, aPosition, velocity};

        const Position anotherPosition = aPosition.inUnit(Length::Unit::Foot);

        const State anotherState = {instant, anotherPosition, velocity};

        EXPECT_NO_THROW(aState + anotherState);
    }

    {
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, position, velocity};

        const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

        const State anotherState = {anotherInstant, position, velocity};

        EXPECT_ANY_THROW(aState + anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const Position aPosition = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity aVelocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State aState = {instant, aPosition, aVelocity};

        const Position anotherPosition = Position::Meters({1.2, 3.4, 5.6}, Frame::ITRF());
        const Velocity anotherVelocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::ITRF());

        const State anotherState = {instant, anotherPosition, anotherVelocity};

        EXPECT_ANY_THROW(aState + anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position aPosition = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity aVelocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State aState = {instant, aPosition, aVelocity};

        const Position anotherPosition = Position::Meters({0.2, 0.4, 0.6}, Frame::GCRF());
        const Velocity anotherVelocity = Velocity::MetersPerSecond({7.0, 8.0, 1.0}, Frame::GCRF());

        const State anotherState = {instant, anotherPosition, anotherVelocity};

        const Position positionExpected = Position::Meters({1.4, 3.8, 6.2}, Frame::GCRF());
        const Velocity velocityExpected = Velocity::MetersPerSecond({14.8, 17.0, 2.2}, Frame::GCRF());

        const State stateExpected = {instant, positionExpected, velocityExpected};

        const State stateCalculated = aState + anotherState;

        EXPECT_TRUE(stateExpected.getCoordinates().isNear(stateCalculated.getCoordinates(), 1e-12));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        VectorXd aCoordinates(3);
        aCoordinates << 0.0, 0.0, 0.0;
        const Shared<const CoordinatesBroker> brokerSPtr1 =
            std::make_shared<CoordinatesBroker>(CoordinatesBroker({CartesianPosition::ThreeDimensional()}));
        const State aState = State(instant, aCoordinates, Frame::GCRF(), brokerSPtr1);

        VectorXd anotherCoordinates(6);
        anotherCoordinates << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;
        const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );
        const State anotherState = State(instant, anotherCoordinates, Frame::GCRF(), brokerSPtr2);

        EXPECT_ANY_THROW(aState + anotherState);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, SubtractionOperator)
{
    {
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, position, velocity};

        EXPECT_ANY_THROW(aState - State::Undefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const Position aPosition = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity aVelocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State aState = {instant, aPosition, aVelocity};

        const Position anotherPosition = Position::Meters({1.2, 3.4, 5.6}, Frame::ITRF());
        const Velocity anotherVelocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::ITRF());

        const State anotherState = {instant, anotherPosition, anotherVelocity};

        EXPECT_ANY_THROW(aState - anotherState);
    }

    {
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, position, velocity};

        const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

        const State anotherState = {anotherInstant, position, velocity};

        EXPECT_ANY_THROW(aState - anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const Position aPosition = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());

        const State aState = {instant, aPosition, velocity};

        const Position anotherPosition = Position({1.2, 3.4, 5.6}, Length::Unit::Foot, Frame::GCRF());

        const State anotherState = {instant, anotherPosition, velocity};

        EXPECT_NO_THROW(aState - anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position aPosition = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity aVelocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State aState = {instant, aPosition, aVelocity};

        const Position anotherPosition = Position::Meters({0.2, 0.4, 0.6}, Frame::GCRF());
        const Velocity anotherVelocity = Velocity::MetersPerSecond({7.0, 8.0, 1.0}, Frame::GCRF());

        const State anotherState = {instant, anotherPosition, anotherVelocity};

        const Position positionExpected = Position::Meters({1.0, 3.0, 5.0}, Frame::GCRF());
        const Velocity velocityExpected = Velocity::MetersPerSecond({0.8, 1.0, 0.2}, Frame::GCRF());

        const State stateExpected = {instant, positionExpected, velocityExpected};

        const State stateCalculated = aState - anotherState;

        EXPECT_TRUE(stateExpected.getCoordinates().isNear(stateCalculated.getCoordinates(), 1e-12));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        VectorXd aCoordinates(3);
        aCoordinates << 0.0, 0.0, 0.0;
        const Shared<const CoordinatesBroker> brokerSPtr1 =
            std::make_shared<CoordinatesBroker>(CoordinatesBroker({CartesianPosition::ThreeDimensional()}));
        const State aState = State(instant, aCoordinates, Frame::GCRF(), brokerSPtr1);

        VectorXd anotherCoordinates(6);
        anotherCoordinates << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;
        const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );
        const State anotherState = State(instant, anotherCoordinates, Frame::GCRF(), brokerSPtr2);

        EXPECT_ANY_THROW(aState - anotherState);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, StreamOperator)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position, velocity};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << state << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << State::Undefined() << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, Accessors)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State state = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_EQ(instant, state.accessInstant());
        EXPECT_EQ(coordinates, state.accessCoordinates());
        EXPECT_EQ(Frame::GCRF(), state.accessFrame());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position.inUnit(Length::Unit::Foot), velocity};

        EXPECT_TRUE(position.accessCoordinates().isApprox(state.getPosition().accessCoordinates(), 1e-16));
    }

    {
        EXPECT_ANY_THROW(State::Undefined().accessInstant());
        EXPECT_ANY_THROW(State::Undefined().accessCoordinates());
        EXPECT_ANY_THROW(State::Undefined().accessFrame());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, Getters)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF());
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State state = State(instant, coordinates, Frame::GCRF(), brokerSPtr);

        EXPECT_EQ(6, state.getSize());
        EXPECT_EQ(instant, state.getInstant());
        EXPECT_EQ(position, state.getPosition());
        EXPECT_EQ(velocity, state.getVelocity());
        EXPECT_EQ(coordinates, state.getCoordinates());
        EXPECT_EQ(Frame::GCRF(), state.getFrame());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF());
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State state = {instant, position, velocity};

        EXPECT_EQ(6, state.getSize());
        EXPECT_EQ(instant, state.getInstant());
        EXPECT_EQ(position, state.getPosition());
        EXPECT_EQ(velocity, state.getVelocity());
        EXPECT_EQ(coordinates, state.getCoordinates());
        EXPECT_EQ(Frame::GCRF(), state.getFrame());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF());
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State state = {instant, position.inUnit(Length::Unit::Foot), velocity};

        EXPECT_EQ(6, state.getSize());
        EXPECT_EQ(instant, state.getInstant());
        EXPECT_TRUE(position.getCoordinates().isApprox(state.getPosition().getCoordinates(), 1e-12));
        EXPECT_EQ(velocity, state.getVelocity());
        EXPECT_TRUE(coordinates.isApprox(state.getCoordinates(), 1e-12));
        EXPECT_EQ(Frame::GCRF(), state.getFrame());
    }

    {
        EXPECT_ANY_THROW(State::Undefined().getSize());
        EXPECT_ANY_THROW(State::Undefined().getInstant());
        EXPECT_ANY_THROW(State::Undefined().getPosition());
        EXPECT_ANY_THROW(State::Undefined().getVelocity());
        EXPECT_ANY_THROW(State::Undefined().getCoordinates());
        EXPECT_ANY_THROW(State::Undefined().getFrame());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, IsDefined)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State state = State(instant, coordinates, Frame::GCRF(), brokerSPtr);

        EXPECT_TRUE(state.isDefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State state = State(instant, coordinates, nullptr, brokerSPtr);

        EXPECT_FALSE(state.isDefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
            CoordinatesBroker({CartesianPosition::ThreeDimensional(), CartesianVelocity::ThreeDimensional()})
        );

        const State state = State(instant, coordinates, Frame::Undefined(), brokerSPtr);

        EXPECT_FALSE(state.isDefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const State state = State(instant, coordinates, Frame::GCRF(), nullptr);

        EXPECT_FALSE(state.isDefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position, velocity};

        EXPECT_TRUE(state.isDefined());
    }

    {
        const Instant instant = Instant::Undefined();
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position, velocity};

        EXPECT_FALSE(state.isDefined());
    }

    {
        EXPECT_FALSE(State::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, InFrame)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state_GCRF1 = {instant, position, velocity};

        const State state_GCRF2 = state_GCRF1.inFrame(Frame::GCRF());

        EXPECT_EQ(state_GCRF2, state_GCRF1);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state_GCRF1 = {instant, position, velocity};

        const State state_GCRF2 = state_GCRF1.inFrame(Frame::ITRF());

        EXPECT_EQ(state_GCRF2.getInstant(), state_GCRF1.getInstant());
        EXPECT_EQ(Frame::ITRF(), state_GCRF2.getFrame());

        EXPECT_TRUE(
            state_GCRF2.getPosition().getCoordinates().isNear({3.130432245445, -1.782920894026, 5.601927082917}, 1e-12)
        );
        EXPECT_TRUE(
            state_GCRF2.getVelocity().getCoordinates().isNear({7.449331963058, -9.290756194490, 1.213098202596}, 1e-12)
        );

        EXPECT_EQ(state_GCRF2.getPosition().accessFrame(), Frame::ITRF());
        EXPECT_EQ(state_GCRF2.getVelocity().accessFrame(), Frame::ITRF());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

        const State state = {instant, position, velocity};

        EXPECT_ANY_THROW(State::Undefined().inFrame(Frame::GCRF()));
        EXPECT_ANY_THROW(state.inFrame(Frame::Undefined()));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, Undefined)
{
    {
        EXPECT_NO_THROW(State::Undefined());
    }
}
