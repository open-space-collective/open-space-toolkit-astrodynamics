/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AttitudeQuaternion.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::type::Shared;
using ostk::core::container::Array;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::State;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, Constructor)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        EXPECT_NO_THROW(State state(instant, coordinates, Frame::GCRF(), brokerSPtr));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Array<Shared<const CoordinateSubset>> subsets = {
            CartesianPosition::Default(), CartesianVelocity::Default()
        };
        EXPECT_NO_THROW(State state(instant, coordinates, Frame::GCRF(), subsets));

        const State state(instant, coordinates, Frame::GCRF(), subsets);

        EXPECT_EQ(state.extractCoordinate(CartesianPosition::Default()), coordinates.segment(0, 3));
        EXPECT_EQ(state.extractCoordinate(CartesianVelocity::Default()), coordinates.segment(3, 3));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(3);
        coordinates << 1.0, 2.0, 3.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        EXPECT_THROW(State state(instant, coordinates, Frame::GCRF(), brokerSPtr), ostk::core::error::runtime::Wrong);
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
        const Instant instant = Instant::Undefined();
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

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

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
        const Quaternion attitude = Quaternion::Unit();
        const VectorXd angularVelocity = Vector3d::X();

        EXPECT_NO_THROW(State state(instant, position, velocity, attitude, angularVelocity, Frame::GCRF()));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::ITRF());
        const Quaternion attitude = Quaternion::Unit();
        const VectorXd angularVelocity = Vector3d::X();

        EXPECT_ANY_THROW(State state(instant, position, velocity, attitude, angularVelocity, Frame::GCRF()));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
        const Quaternion attitude = Quaternion::Unit();
        const VectorXd angularVelocity = Vector3d::X();

        EXPECT_ANY_THROW(State state(instant, position, velocity, attitude, angularVelocity, Frame::ITRF()));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::ITRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
        const Quaternion attitude = Quaternion::Unit();
        const VectorXd angularVelocity = Vector3d::X();

        EXPECT_ANY_THROW(State state(instant, position, velocity, attitude, angularVelocity, Frame::GCRF()));
    }

    {
        const Instant instant = Instant::Undefined();
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
        const Quaternion attitude = Quaternion::Unit();
        const VectorXd angularVelocity = Vector3d::X();

        EXPECT_ANY_THROW(State state(instant, position, velocity, attitude, angularVelocity, Frame::GCRF()));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Undefined();
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
        const Quaternion attitude = Quaternion::Unit();
        const VectorXd angularVelocity = Vector3d::X();

        EXPECT_ANY_THROW(State state(instant, position, velocity, attitude, angularVelocity, Frame::GCRF()));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::Undefined();
        const Quaternion attitude = Quaternion::Unit();
        const VectorXd angularVelocity = Vector3d::X();

        EXPECT_ANY_THROW(State state(instant, position, velocity, attitude, angularVelocity, Frame::GCRF()));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());
        const Quaternion attitude = Quaternion::Unit();
        const VectorXd angularVelocity = Vector3d::Undefined();

        EXPECT_ANY_THROW(State state(instant, position, velocity, attitude, angularVelocity, Frame::GCRF()));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        State state(instant, coordinates, Frame::GCRF(), brokerSPtr);

        EXPECT_NO_THROW(State anotherState(state));

        const State anotherState(state);

        EXPECT_EQ(state, anotherState);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, CopyAssignmentOperator)
{
    // Create a state
    const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
    VectorXd coordinates(6);
    coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
    const Shared<const CoordinateBroker> brokerSPtr =
        std::make_shared<CoordinateBroker>(CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()}
        ));
    State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

    // Copy the state
    State anotherState = State::Undefined();

    // Check that the two states are no longer equal
    EXPECT_NE(aState, anotherState);

    anotherState = aState;

    // Check that the two states are equal
    EXPECT_EQ(aState, anotherState);

    // Modify the original state
    aState = State::Undefined();

    // Check that the two states are no longer equal
    EXPECT_NE(aState, anotherState);
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, EqualToOperator)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};
        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_TRUE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinateBroker> brokerSPtr1 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        VectorXd anotherCoordinates(7);
        anotherCoordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 100.0;
        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()})
        );

        const State anotherState = {instant, anotherCoordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinateBroker> brokerSPtr1 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_TRUE(aState == anotherState);
    }

    {
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, coordinates, Frame::GCRF(), brokerSPtr};

        const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

        const State anotherState = {anotherInstant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
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
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        const State anotherState = {instant, coordinates, Frame::ITRF(), brokerSPtr};

        EXPECT_FALSE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinateBroker> brokerSPtr1 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_TRUE(aState == anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinateBroker> brokerSPtr1 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianVelocity::Default(), CartesianPosition::Default()})
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
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_FALSE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinateBroker> brokerSPtr1 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        VectorXd anotherCoordinates(7);
        anotherCoordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 100.0;
        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()})
        );

        const State anotherState = {instant, anotherCoordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinateBroker> brokerSPtr1 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_FALSE(aState != anotherState);
    }

    {
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        CoordinateBroker broker = CoordinateBroker();
        broker.addSubset(CartesianPosition::Default());
        broker.addSubset(CartesianVelocity::Default());
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<const CoordinateBroker>(broker);

        const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const State aState = {anInstant, coordinates, Frame::GCRF(), brokerSPtr};

        const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

        const State anotherState = {anotherInstant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        CoordinateBroker broker = CoordinateBroker();
        broker.addSubset(CartesianPosition::Default());
        broker.addSubset(CartesianVelocity::Default());
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<const CoordinateBroker>(broker);

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
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        const State anotherState = {instant, coordinates, Frame::ITRF(), brokerSPtr};

        EXPECT_TRUE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinateBroker> brokerSPtr1 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

        EXPECT_FALSE(aState != anotherState);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

        const Shared<const CoordinateBroker> brokerSPtr1 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianVelocity::Default(), CartesianPosition::Default()})
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
        const Shared<const CoordinateBroker> brokerSPtr1 =
            std::make_shared<CoordinateBroker>(CoordinateBroker({CartesianPosition::Default()}));
        const State aState = State(instant, aCoordinates, Frame::GCRF(), brokerSPtr1);

        VectorXd anotherCoordinates(6);
        anotherCoordinates << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;
        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
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
        const Shared<const CoordinateBroker> brokerSPtr1 =
            std::make_shared<CoordinateBroker>(CoordinateBroker({CartesianPosition::Default()}));
        const State aState = State(instant, aCoordinates, Frame::GCRF(), brokerSPtr1);

        VectorXd anotherCoordinates(6);
        anotherCoordinates << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;
        const Shared<const CoordinateBroker> brokerSPtr2 = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
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
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State state = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_EQ(instant, state.accessInstant());
        EXPECT_EQ(coordinates, state.accessCoordinates());
        EXPECT_EQ(Frame::GCRF(), state.accessFrame());
        EXPECT_EQ(brokerSPtr, state.accessCoordinateBroker());
    }

    {
        EXPECT_ANY_THROW(State::Undefined().accessInstant());
        EXPECT_ANY_THROW(State::Undefined().accessCoordinates());
        EXPECT_ANY_THROW(State::Undefined().accessFrame());
        EXPECT_ANY_THROW(State::Undefined().accessCoordinateBroker());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, Getters)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF());
        const Quaternion attitude = Quaternion(-0.003, -0.904, 0.301, 0.304, Quaternion::Format::XYZS);
        const Vector3d angularVelocity = {-1.0, -2.0, -3.0};

        VectorXd coordinates(13);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, -0.003, -0.904, 0.301, 0.304, -1.0, -2.0, -3.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker(
            {CartesianPosition::Default(),
             CartesianVelocity::Default(),
             AttitudeQuaternion::Default(),
             AngularVelocity::Default()}
        ));

        const State state = State(instant, coordinates, Frame::GCRF(), brokerSPtr);

        EXPECT_EQ(13, state.getSize());
        EXPECT_EQ(instant, state.getInstant());
        EXPECT_EQ(position, state.getPosition());
        EXPECT_EQ(velocity, state.getVelocity());
        EXPECT_EQ(attitude, state.getAttitude());
        EXPECT_EQ(angularVelocity, state.getAngularVelocity());
        EXPECT_EQ(coordinates, state.getCoordinates());
        EXPECT_EQ(state.getCoordinateSubsets(), brokerSPtr->getSubsets());
        EXPECT_TRUE(state.hasSubset(CartesianPosition::Default()));
        EXPECT_TRUE(state.hasSubset(CartesianVelocity::Default()));
        EXPECT_TRUE(state.hasSubset(AttitudeQuaternion::Default()));
        EXPECT_TRUE(state.hasSubset(AngularVelocity::Default()));
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
        EXPECT_ANY_THROW(state.getAttitude());
        EXPECT_ANY_THROW(state.getAngularVelocity());
        EXPECT_TRUE(state.hasSubset(CartesianPosition::Default()));
        EXPECT_TRUE(state.hasSubset(CartesianVelocity::Default()));
        EXPECT_EQ(coordinates, state.getCoordinates());
        EXPECT_EQ(Frame::GCRF(), state.getFrame());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF());
        const Quaternion attitude = Quaternion(-0.003, -0.904, 0.301, 0.304, Quaternion::Format::XYZS);
        const Vector3d angularVelocity = {-1.0, -2.0, -3.0};

        VectorXd coordinates(13);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, -0.003, -0.904, 0.301, 0.304, -1.0, -2.0, -3.0;

        const State state = {instant, position, velocity, attitude, angularVelocity, Frame::GCRF()};

        EXPECT_EQ(13, state.getSize());
        EXPECT_EQ(instant, state.getInstant());
        EXPECT_EQ(position, state.getPosition());
        EXPECT_EQ(velocity, state.getVelocity());
        EXPECT_EQ(attitude, state.getAttitude());
        EXPECT_EQ(angularVelocity, state.getAngularVelocity());
        EXPECT_TRUE(state.hasSubset(CartesianPosition::Default()));
        EXPECT_TRUE(state.hasSubset(CartesianVelocity::Default()));
        EXPECT_TRUE(state.hasSubset(AttitudeQuaternion::Default()));
        EXPECT_TRUE(state.hasSubset(AngularVelocity::Default()));
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
        EXPECT_ANY_THROW(State::Undefined().getAttitude());
        EXPECT_ANY_THROW(State::Undefined().getAngularVelocity());
        EXPECT_ANY_THROW(State::Undefined().getCoordinates());
        EXPECT_ANY_THROW(State::Undefined().getCoordinateSubsets());
        EXPECT_ANY_THROW(State::Undefined().hasSubset(CartesianPosition::Default()));
        EXPECT_ANY_THROW(State::Undefined().getFrame());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, IsDefined)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State state = State(instant, coordinates, Frame::GCRF(), brokerSPtr);

        EXPECT_TRUE(state.isDefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );

        const State state = State(instant, coordinates, nullptr, brokerSPtr);

        EXPECT_FALSE(state.isDefined());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
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
        EXPECT_FALSE(State::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, ExtractCoordinate)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(0);
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(CoordinateBroker());
        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_ANY_THROW(aState.extractCoordinate(CartesianPosition::Default()));
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );
        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        EXPECT_EQ(coordinates.segment(0, 3), aState.extractCoordinate(CartesianPosition::Default()));
        EXPECT_EQ(coordinates.segment(3, 3), aState.extractCoordinate(CartesianVelocity::Default()));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, ExtractCoordinates)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        VectorXd coordinates(6);
        coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
        const Shared<const CoordinateBroker> brokerSPtr = std::make_shared<CoordinateBroker>(
            CoordinateBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
        );
        const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

        const Array<Shared<const CoordinateSubset>> positionSubset = {CartesianPosition::Default()};
        EXPECT_EQ(coordinates.segment(0, 3), aState.extractCoordinates(positionSubset));

        const Array<Shared<const CoordinateSubset>> velocitySubset = {CartesianVelocity::Default()};
        EXPECT_EQ(coordinates.segment(3, 3), aState.extractCoordinates(velocitySubset));

        const Array<Shared<const CoordinateSubset>> positionAndVelocitySubset = {
            CartesianPosition::Default(),
            CartesianVelocity::Default(),
        };
        EXPECT_EQ(coordinates, aState.extractCoordinates(positionAndVelocitySubset));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_State, InFrame)
{
    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({7.0e6, 0, 0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({0, 8.0e3, 0}, Frame::GCRF());
        const Quaternion attitude = Quaternion(-0.003, -0.904, 0.301, 0.304, Quaternion::Format::XYZS);
        const Vector3d angularVelocity = {-1.0, -2.0, -3.0};

        const State stateGCRF1 = {instant, position, velocity, attitude, angularVelocity, Frame::GCRF()};

        const State stateGCRF2 = stateGCRF1.inFrame(Frame::GCRF());

        EXPECT_EQ(stateGCRF2, stateGCRF1);
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({7.0e6, 0, 0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({0, 8.0e3, 0}, Frame::GCRF());
        const Quaternion attitude = Quaternion(-0.003, -0.904, 0.301, 0.304, Quaternion::Format::XYZS);
        const Vector3d angularVelocity = {-1.0, -2.0, -3.0};

        const State stateGCRF = {instant, position, velocity, attitude, angularVelocity, Frame::GCRF()};

        const State stateITRF = stateGCRF.inFrame(Frame::ITRF());

        EXPECT_EQ(stateITRF.getInstant(), stateGCRF.getInstant());
        EXPECT_EQ(Frame::ITRF(), stateITRF.getFrame());

        // Reference numbers obtained from Orekit
        EXPECT_TRUE(stateITRF.getPosition().getCoordinates().isNear(
            {-1259967.7426724238, -6885661.583104378, 12076.853293084983}, 1e-1
        ));
        EXPECT_TRUE(stateITRF.getVelocity().getCoordinates().isNear(
            {7367.228766065452, -1348.0875171584087, -0.3170883426267612}, 1e-4
        ));
        EXPECT_TRUE(stateITRF.getAttitude().toNormalized().isNear(
            Quaternion(
                0.6966619559964469,
                0.5764994153890691,
                0.04016298268235078,
                -0.4253698131571204,
                Quaternion::Format::XYZS
            )
                .toNormalized(),
            Angle::Degrees(1e-12)
        ));
        EXPECT_TRUE(stateITRF.getAngularVelocity().isNear({-1.0000398449, -1.9999601581, -2.9999537027}, 1e-7));

        EXPECT_EQ(stateITRF.getPosition().accessFrame(), Frame::ITRF());
        EXPECT_EQ(stateITRF.getVelocity().accessFrame(), Frame::ITRF());
    }

    {
        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Position position = Position::Meters({7.0e6, 0, 0}, Frame::GCRF());
        const Velocity velocity = Velocity::MetersPerSecond({0, 8.0e3, 0}, Frame::GCRF());
        const Quaternion attitude = Quaternion(-0.003, -0.904, 0.301, 0.304, Quaternion::Format::XYZS);
        const Vector3d angularVelocity = {-1.0, -2.0, -3.0};

        const State state = {instant, position, velocity, attitude, angularVelocity, Frame::GCRF()};

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
