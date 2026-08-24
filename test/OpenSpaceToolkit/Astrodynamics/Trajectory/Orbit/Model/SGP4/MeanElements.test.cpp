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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/MeanElements.hpp>
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
using ostk::astrodynamics::trajectory::orbit::model::sgp4::MeanElements;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements : public ::testing::Test
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

    const MeanElements meanElements_ = MeanElements::FromTLE(tle_);

    /// A geostationary element set: its period is well past the 225 min above which the
    /// propagator switches to its deep-space path. Built directly rather than through a TLE,
    /// which is the point of the class.
    const MeanElements deepSpaceMeanElements_ = {
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

    // The revolution number and the output frame are optional, and default to 1 and TEME.
    {
        const MeanElements defaulted = {
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

    EXPECT_NO_THROW(MeanElements(
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

    // The propagator is built here, so an element set it cannot fly is rejected at
    // construction rather than at the first call for a state.
    EXPECT_THROW(
        MeanElements(
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
    EXPECT_NO_THROW(MeanElements::Undefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, Clone)
{
    const MeanElements* clonePtr = meanElements_.clone();

    EXPECT_NE(clonePtr, nullptr);
    EXPECT_TRUE(*clonePtr == meanElements_);

    // The clone shares the propagator, and flies to the same place.
    EXPECT_EQ(
        clonePtr->calculateStateAt(tle_.getEpoch() + Duration::Minutes(45.0)).getPosition().getCoordinates(),
        meanElements_.calculateStateAt(tle_.getEpoch() + Duration::Minutes(45.0)).getPosition().getCoordinates()
    );

    delete clonePtr;
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, EqualToOperator)
{
    EXPECT_TRUE(meanElements_ == MeanElements::FromTLE(tle_));
    EXPECT_FALSE(meanElements_ != MeanElements::FromTLE(tle_));

    const auto elementsWith = [this](
                                  const Real& anEccentricity,
                                  const Integer& aRevolutionNumberAtEpoch,
                                  const Shared<const Frame>& anOutputFrameSPtr
                              ) -> MeanElements
    {
        return {
            meanElements_.getEpoch(),
            meanElements_.getInclination(),
            meanElements_.getRaan(),
            anEccentricity,
            meanElements_.getAop(),
            meanElements_.getMeanAnomaly(),
            meanElements_.getMeanMotion(),
            meanElements_.getBStarDragTerm(),
            aRevolutionNumberAtEpoch,
            anOutputFrameSPtr,
        };
    };

    const Real eccentricity = meanElements_.getEccentricity();
    const Integer revolutionNumberAtEpoch = meanElements_.getRevolutionNumberAtEpoch();

    EXPECT_TRUE(meanElements_ == elementsWith(eccentricity, revolutionNumberAtEpoch, Frame::TEME()));

    // Any one of the elements differing is enough, down to a step the TLE format could not hold.
    EXPECT_TRUE(meanElements_ != elementsWith(eccentricity + 1e-9, revolutionNumberAtEpoch, Frame::TEME()));

    // ... and so is the revolution number, or the output frame.
    EXPECT_TRUE(meanElements_ != elementsWith(eccentricity, revolutionNumberAtEpoch + 1, Frame::TEME()));
    EXPECT_TRUE(meanElements_ != elementsWith(eccentricity, revolutionNumberAtEpoch, Frame::GCRF()));

    EXPECT_FALSE(MeanElements::Undefined() == MeanElements::Undefined());
    EXPECT_TRUE(MeanElements::Undefined() != MeanElements::Undefined());
    EXPECT_TRUE(meanElements_ != MeanElements::Undefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, IsDefined)
{
    EXPECT_TRUE(meanElements_.isDefined());
    EXPECT_TRUE(deepSpaceMeanElements_.isDefined());
    EXPECT_FALSE(MeanElements::Undefined().isDefined());

    // An undefined revolution number is enough to make the set undefined.
    EXPECT_FALSE(MeanElements(
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, Getters)
{
    EXPECT_EQ(meanElements_.getEpoch(), tle_.getEpoch());
    EXPECT_EQ(meanElements_.getInclination(), tle_.getInclination());
    EXPECT_EQ(meanElements_.getRaan(), tle_.getRaan());
    EXPECT_EQ(meanElements_.getEccentricity(), tle_.getEccentricity());
    EXPECT_EQ(meanElements_.getAop(), tle_.getAop());
    EXPECT_EQ(meanElements_.getMeanAnomaly(), tle_.getMeanAnomaly());
    EXPECT_EQ(meanElements_.getMeanMotion(), tle_.getMeanMotion());
    EXPECT_EQ(meanElements_.getBStarDragTerm(), tle_.getBStarDragTerm());
    EXPECT_EQ(meanElements_.getRevolutionNumberAtEpoch(), tle_.getRevolutionNumberAtEpoch());
    EXPECT_EQ(*meanElements_.getOutputFrame(), *Frame::TEME());

    EXPECT_ANY_THROW(MeanElements::Undefined().getEpoch());
    EXPECT_ANY_THROW(MeanElements::Undefined().getInclination());
    EXPECT_ANY_THROW(MeanElements::Undefined().getRaan());
    EXPECT_ANY_THROW(MeanElements::Undefined().getEccentricity());
    EXPECT_ANY_THROW(MeanElements::Undefined().getAop());
    EXPECT_ANY_THROW(MeanElements::Undefined().getMeanAnomaly());
    EXPECT_ANY_THROW(MeanElements::Undefined().getMeanMotion());
    EXPECT_ANY_THROW(MeanElements::Undefined().getBStarDragTerm());
    EXPECT_ANY_THROW(MeanElements::Undefined().getRevolutionNumberAtEpoch());
    EXPECT_ANY_THROW(MeanElements::Undefined().getOutputFrame());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << meanElements_ << std::endl);
    EXPECT_NO_THROW(std::cout << MeanElements::Undefined() << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(meanElements_.print(std::cout, true));
    EXPECT_NO_THROW(meanElements_.print(std::cout, false));
    EXPECT_NO_THROW(MeanElements::Undefined().print(std::cout, true));

    const String output = testing::internal::GetCapturedStdout();

    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("Revolution Number"), std::string::npos);
}

/// Fed the values a TLE carries, this must agree with the TLE path -- and since orbit::model::
/// SGP4 decodes its TLE into a MeanElements and propagates that, the two are the same
/// computation and the difference is exactly zero. The epsilon below is slack, not tolerance.
/// This guards the structure: reintroduce a second propagator behind the TLE path and it fails.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, MatchesSGP4FromTLE)
{
    const SGP4 sgp4 = {tle_, Frame::TEME()};

    for (const Real minutesFromEpoch : {-1440.0, -60.0, 0.0, 1.0, 60.0, 720.0, 1440.0})
    {
        const Instant instant = tle_.getEpoch() + Duration::Minutes(minutesFromEpoch);

        const State fromMeanElements = meanElements_.calculateStateAt(instant);
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
    const State state = meanElements_.calculateStateAt(tle_.getEpoch());

    EXPECT_TRUE(state.isDefined());
    EXPECT_EQ(state.getInstant(), tle_.getEpoch());
    EXPECT_EQ(*state.accessFrame(), *Frame::TEME());

    // The output frame is the one the element set was built with, not one passed per call.
    const State stateGCRF = MeanElements::FromTLE(tle_, Frame::GCRF()).calculateStateAt(tle_.getEpoch());

    EXPECT_EQ(*stateGCRF.accessFrame(), *Frame::GCRF());
    EXPECT_GT(
        (stateGCRF.getPosition().getCoordinates() - state.getPosition().getCoordinates()).norm(), 1.0
    );  // the two frames really do differ

    // The deep-space path is reached through the same call.
    EXPECT_TRUE(deepSpaceMeanElements_.calculateStateAt(tle_.getEpoch() + Duration::Hours(12.0)).isDefined());

    EXPECT_ANY_THROW(meanElements_.calculateStateAt(Instant::Undefined()));
    EXPECT_ANY_THROW(MeanElements::Undefined().calculateStateAt(tle_.getEpoch()));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, CalculateStatesAt)
{
    const Array<Instant> instants = {
        tle_.getEpoch(),
        tle_.getEpoch() + Duration::Minutes(10.0),
        tle_.getEpoch() + Duration::Minutes(20.0),
    };

    const Array<State> states = meanElements_.calculateStatesAt(instants);

    ASSERT_EQ(states.getSize(), instants.getSize());

    for (Size i = 0; i < instants.getSize(); ++i)
    {
        EXPECT_EQ(states[i].getInstant(), instants[i]);
        EXPECT_EQ(*states[i].accessFrame(), *Frame::TEME());
        EXPECT_LT(
            (states[i].getPosition().getCoordinates() -
             meanElements_.calculateStateAt(instants[i]).getPosition().getCoordinates())
                .norm(),
            1e-12
        );
    }

    // Here too the output frame comes from the element set.
    for (const State& stateGCRF : MeanElements::FromTLE(tle_, Frame::GCRF()).calculateStatesAt(instants))
    {
        EXPECT_EQ(*stateGCRF.accessFrame(), *Frame::GCRF());
    }

    EXPECT_EQ(meanElements_.calculateStatesAt({}).getSize(), 0);

    EXPECT_ANY_THROW(meanElements_.calculateStatesAt({Instant::Undefined()}));
    EXPECT_ANY_THROW(MeanElements::Undefined().calculateStatesAt(instants));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, Undefined)
{
    const MeanElements meanElements = MeanElements::Undefined();

    EXPECT_FALSE(meanElements.isDefined());
    EXPECT_ANY_THROW(meanElements.calculateStateAt(tle_.getEpoch()));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, FromTLE)
{
    EXPECT_EQ(meanElements_.getEpoch(), tle_.getEpoch());
    EXPECT_EQ(meanElements_.getRevolutionNumberAtEpoch(), tle_.getRevolutionNumberAtEpoch());
    EXPECT_EQ(*meanElements_.getOutputFrame(), *Frame::TEME());

    EXPECT_EQ(*MeanElements::FromTLE(tle_, Frame::GCRF()).getOutputFrame(), *Frame::GCRF());

    EXPECT_ANY_THROW(MeanElements::FromTLE(TLE::Undefined()));
}

/// The class is an orbit::Model, so it can be handed anywhere one is expected -- an Orbit
/// above all -- without a TLE in between.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, UsableAsAnOrbitModel)
{
    const ostk::astrodynamics::trajectory::orbit::Model& model = meanElements_;

    EXPECT_TRUE(model.isDefined());
    EXPECT_EQ(model.getEpoch(), tle_.getEpoch());
    EXPECT_EQ(model.getRevolutionNumberAtEpoch(), tle_.getRevolutionNumberAtEpoch());
    EXPECT_TRUE(model.calculateStateAt(tle_.getEpoch()).isDefined());

    EXPECT_TRUE(model.is<MeanElements>());
    EXPECT_TRUE(model.as<MeanElements>() == meanElements_);

    // The epoch short-circuits; away from it the revolution number is counted off passes.
    EXPECT_EQ(model.calculateRevolutionNumberAt(tle_.getEpoch()), tle_.getRevolutionNumberAtEpoch());

    const Orbit orbit = {meanElements_, std::make_shared<Earth>(Earth::Spherical())};

    EXPECT_TRUE(orbit.isDefined());
    EXPECT_TRUE(orbit.getStateAt(tle_.getEpoch()).isDefined());
}

/// The reason this class exists. A step in eccentricity below the TLE's 1e-7 quantization must
/// still move the propagated position; through a TLE it does not move it at all.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, RespondsBelowTheTLEQuantum)
{
    const Real nearCircularEccentricity = 2.4e-5;
    const Real step = 1e-3 * nearCircularEccentricity;  // ~2.4e-8, a quarter of the 1e-7 quantization

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

/// Vallado's propagator keeps its working state in the element record it is handed, and the
/// deep-space path carries an integrator state across calls, so a shared record would make
/// concurrent propagation of one MeanElements -- or of copies, which share the record -- both
/// racy and order-dependent. Each call takes its own copy of the record; this pins that down.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4_MeanElements, CalculateStateAtIsThreadSafe)
{
    for (const MeanElements& meanElements : Array<MeanElements>({meanElements_, deepSpaceMeanElements_}))
    {
        Array<Instant> instants = Array<Instant>::Empty();

        for (Size i = 0; i < 32; ++i)
        {
            // Deliberately out of order and far apart, so a carried-over integrator state
            // would show up as a difference rather than as a coincidence.
            instants.add(meanElements.getEpoch() + Duration::Minutes(((i * 17) % 32) * 53.0));
        }

        const Array<State> expectedStates = meanElements.calculateStatesAt(instants);

        std::vector<Vector3d> positions(instants.getSize(), Vector3d::Zero());
        std::vector<std::thread> threads;
        threads.reserve(instants.getSize());

        for (Size i = 0; i < instants.getSize(); ++i)
        {
            threads.emplace_back(
                [&meanElements, &instants, &positions, i]()
                {
                    positions[i] = meanElements.calculateStateAt(instants[i]).getPosition().getCoordinates();
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
