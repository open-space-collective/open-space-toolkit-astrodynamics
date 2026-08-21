/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/MeanElements.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Time;

using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::MeanElements;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements : public ::testing::Test
{
   protected:
    const TLE tle_ = {
        "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, Constructor)
{
    EXPECT_NO_THROW(MeanElements(
        tle_.getEpoch(),
        tle_.getInclination(),
        tle_.getRaan(),
        tle_.getEccentricity(),
        tle_.getAop(),
        tle_.getMeanAnomaly(),
        tle_.getMeanMotion(),
        tle_.getBStarDragTerm()
    ));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, EqualToOperator)
{
    const MeanElements meanElements = MeanElements::FromTLE(tle_);

    EXPECT_TRUE(meanElements == MeanElements::FromTLE(tle_));
    EXPECT_FALSE(meanElements != MeanElements::FromTLE(tle_));

    const MeanElements other = {
        meanElements.getEpoch(),
        meanElements.getInclination(),
        meanElements.getRaan(),
        meanElements.getEccentricity() + 1e-9,
        meanElements.getAop(),
        meanElements.getMeanAnomaly(),
        meanElements.getMeanMotion(),
        meanElements.getBStarDragTerm(),
    };

    EXPECT_TRUE(meanElements != other);

    EXPECT_FALSE(MeanElements::Undefined() == MeanElements::Undefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, IsDefined)
{
    EXPECT_TRUE(MeanElements::FromTLE(tle_).isDefined());
    EXPECT_FALSE(MeanElements::Undefined().isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, Getters)
{
    const MeanElements meanElements = MeanElements::FromTLE(tle_);

    EXPECT_EQ(meanElements.getEpoch(), tle_.getEpoch());
    EXPECT_EQ(meanElements.getInclination(), tle_.getInclination());
    EXPECT_EQ(meanElements.getRaan(), tle_.getRaan());
    EXPECT_EQ(meanElements.getEccentricity(), tle_.getEccentricity());
    EXPECT_EQ(meanElements.getAop(), tle_.getAop());
    EXPECT_EQ(meanElements.getMeanAnomaly(), tle_.getMeanAnomaly());
    EXPECT_EQ(meanElements.getMeanMotion(), tle_.getMeanMotion());
    EXPECT_EQ(meanElements.getBStarDragTerm(), tle_.getBStarDragTerm());

    EXPECT_ANY_THROW(MeanElements::Undefined().getEpoch());
    EXPECT_ANY_THROW(MeanElements::Undefined().getInclination());
    EXPECT_ANY_THROW(MeanElements::Undefined().getRaan());
    EXPECT_ANY_THROW(MeanElements::Undefined().getEccentricity());
    EXPECT_ANY_THROW(MeanElements::Undefined().getAop());
    EXPECT_ANY_THROW(MeanElements::Undefined().getMeanAnomaly());
    EXPECT_ANY_THROW(MeanElements::Undefined().getMeanMotion());
    EXPECT_ANY_THROW(MeanElements::Undefined().getBStarDragTerm());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << MeanElements::FromTLE(tle_) << std::endl);
    EXPECT_NO_THROW(std::cout << MeanElements::Undefined() << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(MeanElements::FromTLE(tle_).print(std::cout, true));
    EXPECT_NO_THROW(MeanElements::FromTLE(tle_).print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

/// Fed the values a TLE carries, this must agree with the TLE path -- and since orbit::model::
/// SGP4 decodes its TLE into a MeanElements and propagates that, the two are the same
/// computation and the difference is exactly zero. The epsilon below is slack, not tolerance.
/// This guards the structure: reintroduce a second propagator behind the TLE path and it fails.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, MatchesSGP4FromTLE)
{
    const MeanElements meanElements = MeanElements::FromTLE(tle_);
    const SGP4 sgp4 = {tle_, Frame::TEME()};

    for (const Real minutesFromEpoch : {-1440.0, -60.0, 0.0, 1.0, 60.0, 720.0, 1440.0})
    {
        const Instant instant = tle_.getEpoch() + Duration::Minutes(minutesFromEpoch);

        const State fromMeanElements = meanElements.calculateStateAt(instant, Frame::TEME());
        const State fromTLE = sgp4.calculateStateAt(instant);

        EXPECT_LT(
            (fromMeanElements.getPosition().getCoordinates() - fromTLE.getPosition().getCoordinates()).norm(), 1e-12
        );
        EXPECT_LT(
            (fromMeanElements.getVelocity().getCoordinates() - fromTLE.getVelocity().getCoordinates()).norm(), 1e-12
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, CalculateStateAt)
{
    const MeanElements meanElements = MeanElements::FromTLE(tle_);

    const State state = meanElements.calculateStateAt(tle_.getEpoch());

    EXPECT_TRUE(state.isDefined());
    EXPECT_EQ(state.getInstant(), tle_.getEpoch());
    EXPECT_EQ(*state.accessFrame(), *Frame::TEME());

    // an output frame other than TEME must still work
    const State stateGCRF = meanElements.calculateStateAt(tle_.getEpoch(), Frame::GCRF());
    EXPECT_EQ(*stateGCRF.accessFrame(), *Frame::GCRF());

    EXPECT_ANY_THROW(meanElements.calculateStateAt(Instant::Undefined()));
    EXPECT_ANY_THROW(meanElements.calculateStateAt(tle_.getEpoch(), nullptr));
    EXPECT_ANY_THROW(MeanElements::Undefined().calculateStateAt(tle_.getEpoch()));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, CalculateStatesAt)
{
    const MeanElements meanElements = MeanElements::FromTLE(tle_);

    const Array<Instant> instants = {
        tle_.getEpoch(),
        tle_.getEpoch() + Duration::Minutes(10.0),
        tle_.getEpoch() + Duration::Minutes(20.0),
    };

    const Array<State> states = meanElements.calculateStatesAt(instants, Frame::TEME());

    ASSERT_EQ(states.getSize(), instants.getSize());

    for (Size i = 0; i < instants.getSize(); ++i)
    {
        EXPECT_EQ(states[i].getInstant(), instants[i]);
        EXPECT_LT(
            (states[i].getPosition().getCoordinates() -
             meanElements.calculateStateAt(instants[i]).getPosition().getCoordinates())
                .norm(),
            1e-12
        );
    }

    EXPECT_EQ(meanElements.calculateStatesAt({}, Frame::TEME()).getSize(), 0);

    EXPECT_ANY_THROW(meanElements.calculateStatesAt({Instant::Undefined()}));
    EXPECT_ANY_THROW(meanElements.calculateStatesAt(instants, nullptr));
    EXPECT_ANY_THROW(MeanElements::Undefined().calculateStatesAt(instants));
}

/// The reason this class exists. A step in eccentricity below the TLE's 1e-7 quantum must
/// still move the propagated position; through a TLE it does not move it at all.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, RespondsBelowTheTLEQuantum)
{
    const Real nearCircularEccentricity = 2.4e-5;
    const Real step = 1e-3 * nearCircularEccentricity;  // ~2.4e-8, a quarter of the 1e-7 quantum

    const auto elementsWith = [this](const Real& anEccentricity) -> MeanElements
    {
        return {
            tle_.getEpoch(),
            tle_.getInclination(),
            tle_.getRaan(),
            anEccentricity,
            tle_.getAop(),
            tle_.getMeanAnomaly(),
            tle_.getMeanMotion(),
            0.0,
        };
    };

    const Instant instant = tle_.getEpoch() + Duration::Minutes(30.0);

    const Vector3d forward =
        elementsWith(nearCircularEccentricity + step).calculateStateAt(instant).getPosition().getCoordinates();
    const Vector3d backward =
        elementsWith(nearCircularEccentricity - step).calculateStateAt(instant).getPosition().getCoordinates();

    // The same probe taken through the TLE text format writes "0000240" on both sides and
    // therefore produces exactly zero response; here it must not.
    EXPECT_GT((forward - backward).norm(), 1e-3);
}
