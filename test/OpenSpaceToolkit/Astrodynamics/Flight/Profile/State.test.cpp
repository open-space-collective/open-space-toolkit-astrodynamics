////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationVector.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, Constructor)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        EXPECT_NO_THROW(State state(instant, position, velocity, attitude, angularVelocity, referenceFrame) ;) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, EqualToOperator)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_TRUE(state == state) ;

    }

    {

        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const Instant instant_1 = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state_1 = { instant_1, position, velocity, attitude, angularVelocity, referenceFrame } ;

        const Instant instant_2 = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC) ;

        const State state_2 = { instant_2, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_FALSE(state_1 == state_2) ;

    }

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_FALSE(State::Undefined() == state) ;
        EXPECT_FALSE(state == State::Undefined()) ;
        EXPECT_FALSE(State::Undefined() == State::Undefined()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, NotEqualToOperator)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    {

        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const Instant instant_1 = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;

        const State state_1 = { instant_1, position, velocity, attitude, angularVelocity, referenceFrame } ;

        const Instant instant_2 = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC) ;

        const State state_2 = { instant_2, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_TRUE(state_1 != state_2) ;

    }

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_FALSE(state != state) ;

    }

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_TRUE(State::Undefined() != state) ;
        EXPECT_TRUE(state != State::Undefined()) ;
        EXPECT_TRUE(State::Undefined() != State::Undefined()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, StreamOperator)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << state << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

    {

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << State::Undefined() << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, IsDefined)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_TRUE(state.isDefined()) ;

    }

    {

        const Instant instant = Instant::Undefined() ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_FALSE(state.isDefined()) ;

    }

    {

        EXPECT_FALSE(State::Undefined().isDefined()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, Accessors)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_EQ(instant, state.accessInstant()) ;
        EXPECT_EQ(position, state.accessPosition()) ;
        EXPECT_EQ(velocity, state.accessVelocity()) ;
        EXPECT_EQ(attitude, state.accessAttitude()) ;
        EXPECT_EQ(angularVelocity, state.accessAngularVelocity()) ;

    }

    {

        EXPECT_ANY_THROW(State::Undefined().accessInstant()) ;
        EXPECT_ANY_THROW(State::Undefined().accessPosition()) ;
        EXPECT_ANY_THROW(State::Undefined().accessVelocity()) ;
        EXPECT_ANY_THROW(State::Undefined().accessAttitude()) ;
        EXPECT_ANY_THROW(State::Undefined().accessAngularVelocity()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, Getters)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_EQ(instant, state.getInstant()) ;
        EXPECT_EQ(position, state.getPosition()) ;
        EXPECT_EQ(velocity, state.getVelocity()) ;
        EXPECT_EQ(attitude, state.getAttitude()) ;
        EXPECT_EQ(angularVelocity, state.getAngularVelocity()) ;
        EXPECT_EQ(referenceFrame, state.getFrame()) ;

    }

    {

        EXPECT_ANY_THROW(State::Undefined().getInstant()) ;
        EXPECT_ANY_THROW(State::Undefined().getPosition()) ;
        EXPECT_ANY_THROW(State::Undefined().getVelocity()) ;
        EXPECT_ANY_THROW(State::Undefined().getAttitude()) ;
        EXPECT_ANY_THROW(State::Undefined().getAngularVelocity()) ;
        EXPECT_ANY_THROW(State::Undefined().getFrame()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, InFrame)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;
    using ostk::math::geom::d3::trf::rot::RotationVector ;

    using ostk::physics::units::Angle ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::Frame ;

    using ostk::astro::flight::profile::State ;

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state_GCRF_1 = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        const State state_GCRF_2 = state_GCRF_1.inFrame(Frame::GCRF()) ;

        EXPECT_EQ(state_GCRF_2, state_GCRF_1) ;

    }

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::RotationVector(RotationVector({ 1.0, 0.0, 0.0 }, Angle::Degrees(90.0))) ; // x_B = x_GCRF, y_B = z_GCRF, z_B = -y_GCRF
        const Vector3d angularVelocity = { 0.0, 0.0, 0.0 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state_GCRF_1 = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        const State state_GCRF_2 = state_GCRF_1.inFrame(Frame::ITRF()) ;

        EXPECT_EQ(state_GCRF_2.getInstant(), state_GCRF_1.getInstant()) ;

        EXPECT_TRUE(state_GCRF_2.getPosition().getCoordinates().isNear({ 3.130432245445, -1.782920894026, 5.601927082917 }, 1e-12)) ;
        EXPECT_TRUE(state_GCRF_2.getVelocity().getCoordinates().isNear({ 7.449331963058, -9.290756194490, 1.213098202596 }, 1e-12)) ;
        EXPECT_TRUE(state_GCRF_2.getAttitude().isNear(Quaternion::XYZS(0.452292767431, -0.543517630607, -0.542758030465, 0.453247788869).normalize(), Angle::Arcseconds(1.0))) ;
        EXPECT_TRUE(state_GCRF_2.getAngularVelocity().isNear({ 0.0, -0.0000729, 0.0 }, 1e-5)) ;

        EXPECT_EQ(state_GCRF_2.getFrame(), Frame::ITRF()) ;

    }

    {

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Position position = Position::Meters({ 1.2, 3.4, 5.6 }, Frame::GCRF()) ;
        const Velocity velocity = Velocity::MetersPerSecond({ 7.8, 9.0, 1.2 }, Frame::GCRF()) ;
        const Quaternion attitude = Quaternion::XYZS(0.0, 0.0, 0.0, 1.0) ;
        const Vector3d angularVelocity = { 3.4, 5.6, 7.8 } ;
        const Shared<const Frame> referenceFrame = Frame::GCRF() ;

        const State state = { instant, position, velocity, attitude, angularVelocity, referenceFrame } ;

        EXPECT_ANY_THROW(State::Undefined().inFrame(referenceFrame)) ;
        EXPECT_ANY_THROW(state.inFrame(Frame::Undefined())) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile_State, Undefined)
{

    using ostk::astro::flight::profile::State ;

    {

        EXPECT_NO_THROW(State::Undefined()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
