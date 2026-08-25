/// Apache License 2.0

#include <thread>
#include <vector>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/SGP4FullPrecision.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Time;

using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::SGP4FullPrecision;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision : public ::testing::Test
{
   protected:
    static Derived MeanMotion(const Real& aValue_revPerDay)
    {
        return {aValue_revPerDay, Derived::Unit::AngularVelocity(Angle::Unit::Revolution, Time::Unit::Day)};
    }

    const TLE tle_ = {
        "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    };

    const SGP4FullPrecision sgp4FullPrecision_ = SGP4FullPrecision::FromTLE(tle_);

    /// A geostationary element set: its period is well past the 225 min above which the
    /// propagator switches to its deep-space path. Built directly rather than through a TLE,
    /// which is the point of the class.
    const SGP4FullPrecision deepSpaceSGP4FullPrecision_ = {
        tle_.getEpoch(),
        Angle::Degrees(0.0503),
        Angle::Degrees(84.1275),
        1.7e-4,
        Angle::Degrees(304.2210),
        Angle::Degrees(101.4390),
        MeanMotion(1.00272923),
        1.0e-5,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, Constructor)
{
    EXPECT_NO_THROW(SGP4FullPrecision(
        tle_.getEpoch(),
        tle_.getInclination(),
        tle_.getRaan(),
        tle_.getEccentricity(),
        tle_.getAop(),
        tle_.getMeanAnomaly(),
        tle_.getMeanMotion(),
        tle_.getBStarDragTerm()
    ));

    // The revolution number and the output frame are optional, and default to 1 and TEME.
    {
        const SGP4FullPrecision defaulted = {
            tle_.getEpoch(),
            tle_.getInclination(),
            tle_.getRaan(),
            tle_.getEccentricity(),
            tle_.getAop(),
            tle_.getMeanAnomaly(),
            tle_.getMeanMotion(),
            tle_.getBStarDragTerm(),
        };

        EXPECT_EQ(defaulted.getRevolutionNumberAtEpoch(), 1);
        EXPECT_EQ(*defaulted.getOutputFrame(), *Frame::TEME());
    }

    EXPECT_NO_THROW(SGP4FullPrecision(
        tle_.getEpoch(),
        tle_.getInclination(),
        tle_.getRaan(),
        tle_.getEccentricity(),
        tle_.getAop(),
        tle_.getMeanAnomaly(),
        tle_.getMeanMotion(),
        tle_.getBStarDragTerm(),
        12345,
        Frame::GCRF()
    ));

    // The propagator is built here, so an element set it cannot propagate is rejected at
    // construction rather than at the first call for a state.
    EXPECT_THROW(
        SGP4FullPrecision(
            tle_.getEpoch(),
            tle_.getInclination(),
            tle_.getRaan(),
            1.5,  // eccentricity outside [0, 1)
            tle_.getAop(),
            tle_.getMeanAnomaly(),
            tle_.getMeanMotion(),
            tle_.getBStarDragTerm()
        ),
        ostk::core::error::RuntimeError
    );

    // An undefined element set carries no propagator, so it constructs without complaint.
    EXPECT_NO_THROW(SGP4FullPrecision::Undefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, Clone)
{
    const SGP4FullPrecision* clonePtr = sgp4FullPrecision_.clone();

    EXPECT_NE(clonePtr, nullptr);
    EXPECT_TRUE(*clonePtr == sgp4FullPrecision_);

    // The clone carries a propagator of its own, and propagates to the same place.
    EXPECT_EQ(
        clonePtr->calculateStateAt(tle_.getEpoch() + Duration::Minutes(45.0)).getPosition().getCoordinates(),
        sgp4FullPrecision_.calculateStateAt(tle_.getEpoch() + Duration::Minutes(45.0)).getPosition().getCoordinates()
    );

    delete clonePtr;
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, EqualToOperator)
{
    EXPECT_TRUE(sgp4FullPrecision_ == SGP4FullPrecision::FromTLE(tle_));

    EXPECT_FALSE(sgp4FullPrecision_ != SGP4FullPrecision::FromTLE(tle_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, IsDefined)
{
    EXPECT_TRUE(sgp4FullPrecision_.isDefined());
    EXPECT_TRUE(deepSpaceSGP4FullPrecision_.isDefined());
    EXPECT_FALSE(SGP4FullPrecision::Undefined().isDefined());

    EXPECT_FALSE(SGP4FullPrecision(
                     tle_.getEpoch(),
                     tle_.getInclination(),
                     tle_.getRaan(),
                     tle_.getEccentricity(),
                     tle_.getAop(),
                     tle_.getMeanAnomaly(),
                     tle_.getMeanMotion(),
                     tle_.getBStarDragTerm(),
                     Integer::Undefined()
    )
                     .isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, Getters)
{
    EXPECT_EQ(sgp4FullPrecision_.getEpoch(), tle_.getEpoch());
    EXPECT_EQ(sgp4FullPrecision_.getInclination(), tle_.getInclination());
    EXPECT_EQ(sgp4FullPrecision_.getRaan(), tle_.getRaan());
    EXPECT_EQ(sgp4FullPrecision_.getEccentricity(), tle_.getEccentricity());
    EXPECT_EQ(sgp4FullPrecision_.getAop(), tle_.getAop());
    EXPECT_EQ(sgp4FullPrecision_.getMeanAnomaly(), tle_.getMeanAnomaly());
    EXPECT_EQ(sgp4FullPrecision_.getMeanMotion(), tle_.getMeanMotion());
    EXPECT_EQ(sgp4FullPrecision_.getBStarDragTerm(), tle_.getBStarDragTerm());
    EXPECT_EQ(sgp4FullPrecision_.getRevolutionNumberAtEpoch(), tle_.getRevolutionNumberAtEpoch());
    EXPECT_EQ(*sgp4FullPrecision_.getOutputFrame(), *Frame::TEME());

    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getEpoch());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getInclination());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getRaan());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getEccentricity());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getAop());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getMeanAnomaly());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getMeanMotion());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getBStarDragTerm());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getRevolutionNumberAtEpoch());
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().getOutputFrame());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << sgp4FullPrecision_ << std::endl);
    EXPECT_NO_THROW(std::cout << SGP4FullPrecision::Undefined() << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(sgp4FullPrecision_.print(std::cout, true));
    EXPECT_NO_THROW(sgp4FullPrecision_.print(std::cout, false));
    EXPECT_NO_THROW(SGP4FullPrecision::Undefined().print(std::cout, true));

    const String output = testing::internal::GetCapturedStdout();

    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("Revolution Number"), std::string::npos);
}

/// Fed the values a TLE carries, this must agree with the TLE path -- and since orbit::model::
/// SGP4 decodes its TLE into an SGP4FullPrecision and propagates that, the two are the same
/// computation and the difference is exactly zero. The epsilon below is slack, not tolerance.
/// This guards the structure: reintroduce a second propagator behind the TLE path and it fails.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, MatchesSGP4FromTLE)
{
    const SGP4 sgp4 = {tle_, Frame::TEME()};

    for (const Real minutesFromEpoch : {-1440.0, -60.0, 0.0, 1.0, 60.0, 720.0, 1440.0})
    {
        const Instant instant = tle_.getEpoch() + Duration::Minutes(minutesFromEpoch);

        const State fromSGP4FullPrecision = sgp4FullPrecision_.calculateStateAt(instant);
        const State fromTLE = sgp4.calculateStateAt(instant);

        EXPECT_LT(
            (fromSGP4FullPrecision.getPosition().getCoordinates() - fromTLE.getPosition().getCoordinates()).norm(),
            1e-12
        );
        EXPECT_LT(
            (fromSGP4FullPrecision.getVelocity().getCoordinates() - fromTLE.getVelocity().getCoordinates()).norm(),
            1e-12
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, CalculateStateAt)
{
    const State state = sgp4FullPrecision_.calculateStateAt(tle_.getEpoch());

    EXPECT_TRUE(state.isDefined());
    EXPECT_EQ(state.getInstant(), tle_.getEpoch());
    EXPECT_EQ(*state.accessFrame(), *Frame::TEME());

    const State stateGCRF = SGP4FullPrecision::FromTLE(tle_, Frame::GCRF()).calculateStateAt(tle_.getEpoch());

    EXPECT_EQ(*stateGCRF.accessFrame(), *Frame::GCRF());
    EXPECT_GT((stateGCRF.getPosition().getCoordinates() - state.getPosition().getCoordinates()).norm(), 1.0);

    EXPECT_ANY_THROW(sgp4FullPrecision_.calculateStateAt(Instant::Undefined()));
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().calculateStateAt(tle_.getEpoch()));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, CalculateStatesAt)
{
    const Array<Instant> instants = {
        tle_.getEpoch(),
        tle_.getEpoch() + Duration::Minutes(10.0),
        tle_.getEpoch() + Duration::Minutes(20.0),
    };

    const Array<State> states = sgp4FullPrecision_.calculateStatesAt(instants);

    ASSERT_EQ(states.getSize(), instants.getSize());

    for (Size i = 0; i < instants.getSize(); ++i)
    {
        EXPECT_EQ(states[i].getInstant(), instants[i]);
        EXPECT_EQ(*states[i].accessFrame(), *Frame::TEME());
        EXPECT_LT(
            (states[i].getPosition().getCoordinates() -
             sgp4FullPrecision_.calculateStateAt(instants[i]).getPosition().getCoordinates())
                .norm(),
            1e-12
        );
    }

    for (const State& stateGCRF : SGP4FullPrecision::FromTLE(tle_, Frame::GCRF()).calculateStatesAt(instants))
    {
        EXPECT_EQ(*stateGCRF.accessFrame(), *Frame::GCRF());
    }

    EXPECT_EQ(sgp4FullPrecision_.calculateStatesAt({}).getSize(), 0);

    EXPECT_ANY_THROW(sgp4FullPrecision_.calculateStatesAt({Instant::Undefined()}));
    EXPECT_ANY_THROW(SGP4FullPrecision::Undefined().calculateStatesAt(instants));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, Undefined)
{
    const SGP4FullPrecision sgp4FullPrecision = SGP4FullPrecision::Undefined();

    EXPECT_FALSE(sgp4FullPrecision.isDefined());
    EXPECT_ANY_THROW(sgp4FullPrecision.calculateStateAt(tle_.getEpoch()));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, FromTLE)
{
    EXPECT_EQ(sgp4FullPrecision_.getEpoch(), tle_.getEpoch());
    EXPECT_EQ(sgp4FullPrecision_.getRevolutionNumberAtEpoch(), tle_.getRevolutionNumberAtEpoch());
    EXPECT_EQ(*sgp4FullPrecision_.getOutputFrame(), *Frame::TEME());

    EXPECT_EQ(*SGP4FullPrecision::FromTLE(tle_, Frame::GCRF()).getOutputFrame(), *Frame::GCRF());

    EXPECT_ANY_THROW(SGP4FullPrecision::FromTLE(TLE::Undefined()));
}

/// The reason this class exists. A step in eccentricity below the TLE's 1e-7 quantization must
/// still move the propagated position; through a TLE it does not move it at all.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, RespondsBelowTheTLEQuantum)
{
    const Real nearCircularEccentricity = 2.4e-5;
    const Real step = 1e-3 * nearCircularEccentricity;  // ~2.4e-8, a quarter of the 1e-7 quantization

    const auto elementsWith = [this](const Real& anEccentricity) -> SGP4FullPrecision
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

/// Vallado's propagator keeps its working state in the element record it is handed, and the
/// deep-space path carries an integrator state across calls, so one SGP4FullPrecision propagates
/// one record in place under a lock rather than handing every call a copy of it. Concurrent
/// propagation of a single model therefore has to come out both race-free and independent of the
/// order the calls happen to land in; this pins that down.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_SGP4FullPrecision, CalculateStateAtIsThreadSafe)
{
    for (const SGP4FullPrecision& sgp4FullPrecision :
         Array<SGP4FullPrecision>({sgp4FullPrecision_, deepSpaceSGP4FullPrecision_}))
    {
        Array<Instant> instants = Array<Instant>::Empty();

        for (Size i = 0; i < 32; ++i)
        {
            // Deliberately out of order and far apart, so a carried-over integrator state
            // would show up as a difference rather than as a coincidence.
            instants.add(sgp4FullPrecision.getEpoch() + Duration::Minutes(((i * 17) % 32) * 53.0));
        }

        const Array<State> expectedStates = sgp4FullPrecision.calculateStatesAt(instants);

        std::vector<Vector3d> positions(instants.getSize(), Vector3d::Zero());
        std::vector<std::thread> threads;
        threads.reserve(instants.getSize());

        for (Size i = 0; i < instants.getSize(); ++i)
        {
            threads.emplace_back(
                [&sgp4FullPrecision, &instants, &positions, i]()
                {
                    positions[i] = sgp4FullPrecision.calculateStateAt(instants[i]).getPosition().getCoordinates();
                }
            );
        }

        for (std::thread& thread : threads)
        {
            thread.join();
        }

        for (Size i = 0; i < instants.getSize(); ++i)
        {
            EXPECT_EQ(expectedStates[i].getPosition().getCoordinates(), positions[i]) << "instant " << i;
        }
    }
}
