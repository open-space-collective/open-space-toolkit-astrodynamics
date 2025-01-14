/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Data/Provider/OffNadir.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using ostk::astrodynamics::data::provider::ComputeOffNadirAngles;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_OffNadirAngles_Parameterized
    : public ::testing::TestWithParam<std::tuple<Instant, Vector3d, Vector3d, double, double, double>>
{
};

TEST_P(OpenSpaceToolkit_Astrodynamics_OffNadirAngles_Parameterized, ComputeOffNadirAngles)
{
    const auto& [instant, position, velocity, expectedCrossTrackAngle, expectedAlongTrackAngle, expectedTotalOffNadirAngle] =
        GetParam();

    const Position targetPosition = Position::Meters(
        LLA(Angle::Degrees(40.0), Angle::Degrees(-70.0), Length::Meters(500.0)).toCartesian(), Frame::ITRF()
    );

    const State state =
        State(instant, Position::Meters(position, Frame::GCRF()), Velocity::MetersPerSecond(velocity, Frame::GCRF()));

    const auto [alongTrackAngle, crossTrackAngle, totalOffNadirAngle] = ComputeOffNadirAngles(state, targetPosition);

    EXPECT_NEAR(crossTrackAngle.inDegrees(), expectedCrossTrackAngle, 1e-3);
    EXPECT_NEAR(alongTrackAngle.inDegrees(), expectedAlongTrackAngle, 1e-3);
    EXPECT_NEAR(totalOffNadirAngle.inDegrees(), expectedTotalOffNadirAngle, 1e-3);
}

INSTANTIATE_TEST_SUITE_P(
    OffNadirAngleTests,
    OpenSpaceToolkit_Astrodynamics_OffNadirAngles_Parameterized,
    ::testing::Values(
        std::make_tuple(
            Instant::DateTime(DateTime(2020, 1, 1, 6, 27, 0, 0), Scale::UTC),
            Vector3d {-3472471.208373032510281, 4819515.086990242823958, 3467417.861184711568058},
            Vector3d {3139.592335423410077, -2415.047014679488257, 6500.946934074809178},
            22.337,
            60.044,
            60.716
        ),
        std::make_tuple(
            Instant::DateTime(DateTime(2020, 1, 1, 6, 28, 30, 0), Scale::UTC),
            Vector3d {-3173655.051141597330570, 4578916.883074138313532, 4033633.893281719181687},
            Vector3d {3505.823518748969036, -2932.814986776730166, 6087.659719805535133},
            27.762,
            37.526,
            42.957
        ),
        std::make_tuple(
            Instant::DateTime(DateTime(2020, 1, 1, 6, 31, 0, 0), Scale::UTC),
            Vector3d {-2607469.475604668259621, 4078707.143469315022230, 4886104.743286957032979},
            Vector3d {4037.577517717802493, -3728.889236312796129, 5267.367084905771662},
            30.964,
            -49.953,
            53.113
        )
    )
);

class OpenSpaceToolkit_Astrodynamics_OffNadirAngles : public ::testing::Test
{
   protected:
    const Environment environment_ = Environment::Default(true);
    const Position targetPosition_ = Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF());
};

TEST_F(OpenSpaceToolkit_Astrodynamics_OffNadirAngles, ComputeOffNadirAngles_Undefined)
{
    {
        EXPECT_ANY_THROW(ComputeOffNadirAngles(State::Undefined(), targetPosition_));
    }

    {
        const State state = State(
            Instant::J2000(),
            Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()),
            Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::GCRF())
        );
        EXPECT_ANY_THROW(ComputeOffNadirAngles(state, Position::Undefined()));
    }
}
