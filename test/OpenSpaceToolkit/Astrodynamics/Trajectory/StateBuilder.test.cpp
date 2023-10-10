/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

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

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder : public ::testing::Test
{
   protected:
    const Array<Shared<CoordinatesSubset>> posVelSubsets = {
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
    }

    const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
        CoordinatesBroker(posVelSubsets)
    );
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Constructor)
{
    {
        EXPECT_NO_THROW(StateBuilder builder(Frame::GCRF(), brokerSPtr));  
    }

    {
        EXPECT_NO_THROW(StateBuilder builder(Frame::GCRF(), posVelSubsets););
    }
}

// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, EqualToOperator)
// {
//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};
//         const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

//         EXPECT_TRUE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

//         const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

//         EXPECT_TRUE(aState == anotherState);
//     }

//     {
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

//         const State aState = {anInstant, coordinates, Frame::GCRF(), brokerSPtr};

//         const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

//         const State anotherState = {anotherInstant, coordinates, Frame::GCRF(), brokerSPtr};

//         EXPECT_FALSE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         VectorXd aCoordinates(6);
//         aCoordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const State aState = {instant, aCoordinates, Frame::GCRF(), brokerSPtr};

//         VectorXd anotherCoordinates(6);
//         anotherCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;

//         const State anotherState = {instant, anotherCoordinates, Frame::GCRF(), brokerSPtr};

//         EXPECT_FALSE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

//         const State anotherState = {instant, coordinates, Frame::ITRF(), brokerSPtr};

//         EXPECT_FALSE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

//         const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

//         EXPECT_TRUE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

//         const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianVelocity::Default(), CartesianPosition::Default()})
//         );

//         const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

//         EXPECT_FALSE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const State state = {instant, position, velocity};

//         EXPECT_TRUE(state == state);
//     }

//     {
//         const Position position = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

//         const State aState = {instant, position, velocity};

//         const State anotherState = {instant, position, velocity};

//         EXPECT_TRUE(aState == anotherState);
//     }

//     {
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

//         const State aState = {anInstant, position, velocity};

//         const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

//         const State anotherState = {anotherInstant, position, velocity};

//         EXPECT_FALSE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

//         const Position aPosition = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

//         const State aState = {instant, aPosition, velocity};

//         const Position anotherPosition = Position::Meters({1001.0, 2001.0, 3001.0}, Frame::GCRF());

//         const State anotherState = {instant, anotherPosition, velocity};

//         EXPECT_FALSE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

//         const Position aPosition = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

//         const State aState = {instant, aPosition, velocity};

//         const Position anotherPosition = aPosition.inUnit(Length::Unit::Foot);

//         const State anotherState = {instant, anotherPosition, velocity};

//         EXPECT_TRUE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

//         const Velocity aVelocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

//         const State aState = {instant, position, aVelocity};

//         const Velocity anotherVelocity = Velocity::MetersPerSecond({11, 21, 31}, Frame::GCRF());

//         const State anotherState = {instant, position, anotherVelocity};

//         EXPECT_FALSE(aState == anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const State state = {instant, position, velocity};

//         EXPECT_FALSE(StateBuilder::Undefined() == state);
//         EXPECT_FALSE(state == StateBuilder::Undefined());
//         EXPECT_FALSE(StateBuilder::Undefined() == StateBuilder::Undefined());
//     }
// }

// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, NotEqualToOperator)
// {
//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

//         const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

//         EXPECT_FALSE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

//         const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

//         EXPECT_FALSE(aState != anotherState);
//     }

//     {
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         CoordinatesBroker broker = CoordinatesBroker();
//         broker.addSubset(CartesianPosition::Default());
//         broker.addSubset(CartesianVelocity::Default());
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<const CoordinatesBroker>(broker);

//         const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

//         const State aState = {anInstant, coordinates, Frame::GCRF(), brokerSPtr};

//         const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

//         const State anotherState = {anotherInstant, coordinates, Frame::GCRF(), brokerSPtr};

//         EXPECT_TRUE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         CoordinatesBroker broker = CoordinatesBroker();
//         broker.addSubset(CartesianPosition::Default());
//         broker.addSubset(CartesianVelocity::Default());
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<const CoordinatesBroker>(broker);

//         VectorXd aCoordinates(6);
//         aCoordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const State aState = {instant, aCoordinates, Frame::GCRF(), brokerSPtr};

//         VectorXd anotherCoordinates(6);
//         anotherCoordinates << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0;

//         const State anotherState = {instant, anotherCoordinates, Frame::GCRF(), brokerSPtr};

//         EXPECT_TRUE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr};

//         const State anotherState = {instant, coordinates, Frame::ITRF(), brokerSPtr};

//         EXPECT_TRUE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

//         const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

//         EXPECT_FALSE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const Shared<const CoordinatesBroker> brokerSPtr1 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State aState = {instant, coordinates, Frame::GCRF(), brokerSPtr1};

//         const Shared<const CoordinatesBroker> brokerSPtr2 = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianVelocity::Default(), CartesianPosition::Default()})
//         );

//         const State anotherState = {instant, coordinates, Frame::GCRF(), brokerSPtr2};

//         EXPECT_TRUE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const State state = {instant, position, velocity};

//         EXPECT_FALSE(state != state);
//     }

//     {
//         const Position position = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

//         const State aState = {instant, position, velocity};

//         const State anotherState = {instant, position, velocity};

//         EXPECT_FALSE(aState != anotherState);
//     }

//     {
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const Instant anInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

//         const State aState = {anInstant, position, velocity};

//         const Instant anotherInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC);

//         const State anotherState = {anotherInstant, position, velocity};

//         EXPECT_TRUE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

//         const Position aPosition = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

//         const State aState = {instant, aPosition, velocity};

//         const Position anotherPosition = Position::Meters({1001.0, 2001.0, 3001.0}, Frame::GCRF());

//         const State anotherState = {instant, anotherPosition, velocity};

//         EXPECT_TRUE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Velocity velocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

//         const Position aPosition = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

//         const State aState = {instant, aPosition, velocity};

//         const Position anotherPosition = aPosition.inUnit(Length::Unit::Foot);

//         const State anotherState = {instant, anotherPosition, velocity};

//         EXPECT_FALSE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1000.0, 2000.0, 3000.0}, Frame::GCRF());

//         const Velocity aVelocity = Velocity::MetersPerSecond({10, 20, 30}, Frame::GCRF());

//         const State aState = {instant, position, aVelocity};

//         const Velocity anotherVelocity = Velocity::MetersPerSecond({11, 21, 31}, Frame::GCRF());

//         const State anotherState = {instant, position, anotherVelocity};

//         EXPECT_TRUE(aState != anotherState);
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const State state = {instant, position, velocity};

//         EXPECT_TRUE(StateBuilder::Undefined() != state);
//         EXPECT_TRUE(state != StateBuilder::Undefined());
//         EXPECT_TRUE(StateBuilder::Undefined() != StateBuilder::Undefined());
//     }
// }

// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, StreamOperator)
// {
//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const State state = {instant, position, velocity};

//         testing::internal::CaptureStdout();

//         EXPECT_NO_THROW(std::cout << state << std::endl);

//         EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
//     }

//     {
//         testing::internal::CaptureStdout();

//         EXPECT_NO_THROW(std::cout << StateBuilder::Undefined() << std::endl);

//         EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
//     }
// }

// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Accessors)
// {
//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State state = {instant, coordinates, Frame::GCRF(), brokerSPtr};

//         EXPECT_EQ(instant, state.accessInstant());
//         EXPECT_EQ(coordinates, state.accessCoordinates());
//         EXPECT_EQ(Frame::GCRF(), state.accessFrame());
//         EXPECT_EQ(brokerSPtr, state.accessCoordinatesBroker());
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const State state = {instant, position.inUnit(Length::Unit::Foot), velocity};

//         EXPECT_TRUE(position.accessCoordinates().isApprox(state.getPosition().accessCoordinates(), 1e-16));
//     }

//     {
//         EXPECT_ANY_THROW(StateBuilder::Undefined().accessInstant());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().accessCoordinates());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().accessFrame());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().accessCoordinatesBroker());
//     }
// }

// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Getters)
// {
//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF());
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State state = State(instant, coordinates, Frame::GCRF(), brokerSPtr);

//         EXPECT_EQ(6, state.getSize());
//         EXPECT_EQ(instant, state.getInstant());
//         EXPECT_EQ(position, state.getPosition());
//         EXPECT_EQ(velocity, state.getVelocity());
//         EXPECT_EQ(coordinates, state.getCoordinates());
//         EXPECT_EQ(state.getCoordinatesSubsets(), brokerSPtr->getSubsets());
//         EXPECT_EQ(Frame::GCRF(), state.getFrame());
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF());
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const State state = {instant, position, velocity};

//         EXPECT_EQ(6, state.getSize());
//         EXPECT_EQ(instant, state.getInstant());
//         EXPECT_EQ(position, state.getPosition());
//         EXPECT_EQ(velocity, state.getVelocity());
//         EXPECT_EQ(coordinates, state.getCoordinates());
//         EXPECT_EQ(Frame::GCRF(), state.getFrame());
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF());
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const State state = {instant, position.inUnit(Length::Unit::Foot), velocity};

//         EXPECT_EQ(6, state.getSize());
//         EXPECT_EQ(instant, state.getInstant());
//         EXPECT_TRUE(position.getCoordinates().isApprox(state.getPosition().getCoordinates(), 1e-12));
//         EXPECT_EQ(velocity, state.getVelocity());
//         EXPECT_TRUE(coordinates.isApprox(state.getCoordinates(), 1e-12));
//         EXPECT_EQ(Frame::GCRF(), state.getFrame());
//     }

//     {
//         EXPECT_ANY_THROW(StateBuilder::Undefined().getSize());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().getInstant());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().getPosition());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().getVelocity());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().getCoordinates());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().getCoordinatesSubsets());
//         EXPECT_ANY_THROW(StateBuilder::Undefined().getFrame());
//     }
// }

// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, IsDefined)
// {
//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State state = State(instant, coordinates, Frame::GCRF(), brokerSPtr);

//         EXPECT_TRUE(state.isDefined());
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State state = State(instant, coordinates, nullptr, brokerSPtr);

//         EXPECT_FALSE(state.isDefined());
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
//         const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(
//             CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
//         );

//         const State state = State(instant, coordinates, Frame::Undefined(), brokerSPtr);

//         EXPECT_FALSE(state.isDefined());
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         VectorXd coordinates(6);
//         coordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;

//         const State state = State(instant, coordinates, Frame::GCRF(), nullptr);

//         EXPECT_FALSE(state.isDefined());
//     }

//     {
//         const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const State state = {instant, position, velocity};

//         EXPECT_TRUE(state.isDefined());
//     }

//     {
//         const Instant instant = Instant::Undefined();
//         const Position position = Position::Meters({1.2, 3.4, 5.6}, Frame::GCRF());
//         const Velocity velocity = Velocity::MetersPerSecond({7.8, 9.0, 1.2}, Frame::GCRF());

//         const State state = {instant, position, velocity};

//         EXPECT_FALSE(state.isDefined());
//     }

//     {
//         EXPECT_FALSE(StateBuilder::Undefined().isDefined());
//     }
// }

// TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_StateBuilder, Undefined)
// {
//     {
//         EXPECT_NO_THROW(StateBuilder::Undefined());
//     }
// }
