/// Apache License 2.0

#include <sstream>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationVector.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::container::Table;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationVector;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4 : public ::testing::Test
{
   protected:
    const TLE tle_ = {
        "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537",
    };

    const SGP4 sgp4_ = SGP4(tle_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, Constructor)
{
    // Single TLE in array
    {
        const Array<TLE> tleArray = {tle_};

        const SGP4 sgp4(tleArray);
        EXPECT_TRUE(sgp4.isDefined());
        EXPECT_EQ(Frame::TEME(), sgp4.getOutputFrame());
        EXPECT_EQ(1u, sgp4.getTles().getSize());
    }

    // Multiple TLEs in array
    {
        const TLE tle1 = tle_;
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Array<TLE> tleArray = {tle2, tle1};  // Intentionally reversed order

        const SGP4 sgp4(tleArray);
        EXPECT_TRUE(sgp4.isDefined());
        EXPECT_EQ(2u, sgp4.getTles().getSize());
        // Should be sorted by epoch: tle1 first
        EXPECT_EQ(tle1.getEpoch(), sgp4.getTles()[0].getEpoch());
        EXPECT_EQ(tle2.getEpoch(), sgp4.getTles()[1].getEpoch());
    }

    // With output frame
    {
        const Array<TLE> tleArray = {tle_};

        const SGP4 sgp4(tleArray, Frame::TEME());
        EXPECT_TRUE(sgp4.isDefined());
        EXPECT_EQ(Frame::TEME(), sgp4.getOutputFrame());
    }

    // Empty array throws
    {
        const Array<TLE> emptyTleArray = Array<TLE>::Empty();
        EXPECT_THROW(SGP4 sgp4(emptyTleArray), ostk::core::error::RuntimeError);
    }

    // Validity intervals are auto-generated
    {
        const TLE tle1 = tle_;
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Array<TLE> tleArray = {tle1, tle2};
        const SGP4 sgp4(tleArray);

        const Array<Interval> intervals = sgp4.getValidityIntervals();
        EXPECT_EQ(2u, intervals.getSize());

        // Midpoint should be 12 hours after tle1 epoch
        const Instant midpoint = tle1.getEpoch() + Duration::Hours(12.0);
        EXPECT_TRUE(intervals[0].contains(tle1.getEpoch()));
        EXPECT_TRUE(intervals[1].contains(tle2.getEpoch()));

        // The boundary is at the midpoint
        EXPECT_FALSE(intervals[0].contains(midpoint));
        EXPECT_TRUE(intervals[1].contains(midpoint));
    }

    // Basic construction with explicit intervals
    {
        const TLE tle1 = tle_;
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Instant boundary = tle1.getEpoch() + Duration::Hours(18.0);
        const Interval interval1 = Interval::Closed(tle1.getEpoch() - Duration::Days(1.0), boundary);
        const Interval interval2 = Interval::Closed(boundary, tle2.getEpoch() + Duration::Days(1.0));

        const Array<Pair<TLE, Interval>> tleIntervalArray = {
            {tle1, interval1},
            {tle2, interval2},
        };

        const SGP4 sgp4(tleIntervalArray);
        EXPECT_TRUE(sgp4.isDefined());
        EXPECT_EQ(2u, sgp4.getTles().getSize());
        EXPECT_EQ(2u, sgp4.getValidityIntervals().getSize());
    }

    // Custom intervals: 18 hours boundary instead of midpoint
    {
        const TLE tle1 = tle_;
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Instant boundary = tle1.getEpoch() + Duration::Hours(18.0);
        const Interval interval1 = Interval::HalfOpenRight(tle1.getEpoch() - Duration::Days(100.0), boundary);
        const Interval interval2 = Interval::Closed(boundary, tle2.getEpoch() + Duration::Days(100.0));

        const Array<Pair<TLE, Interval>> tleIntervalArray = {
            {tle1, interval1},
            {tle2, interval2},
        };

        const SGP4 sgp4(tleIntervalArray, Frame::GCRF());

        // 15 hours after tle1 should still use tle1 (within 18-hour boundary)
        const Instant instantInTle1 = tle1.getEpoch() + Duration::Hours(15.0);
        const SGP4 sgp4Single1(tle1);
        const State stateMulti = sgp4.calculateStateAt(instantInTle1);
        const State stateSingle = sgp4Single1.calculateStateAt(instantInTle1);

        EXPECT_LT(
            (stateMulti.getPosition().accessCoordinates() - stateSingle.getPosition().accessCoordinates()).norm(), 1e-6
        );

        // 19 hours after tle1 should use tle2 (past 18-hour boundary)
        const Instant instantInTle2 = tle1.getEpoch() + Duration::Hours(19.0);
        const SGP4 sgp4Single2(tle2);
        const State stateMulti2 = sgp4.calculateStateAt(instantInTle2);
        const State stateSingle2 = sgp4Single2.calculateStateAt(instantInTle2);

        EXPECT_LT(
            (stateMulti2.getPosition().accessCoordinates() - stateSingle2.getPosition().accessCoordinates()).norm(),
            1e-6
        );
    }

    // Empty array throws
    {
        const Array<Pair<TLE, Interval>> emptyArray = Array<Pair<TLE, Interval>>::Empty();
        EXPECT_THROW(SGP4 sgp4(emptyArray), ostk::core::error::RuntimeError);
    }

    // Sorted by epoch regardless of input order
    {
        const TLE tle1 = tle_;
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Interval interval1 =
            Interval::Closed(tle1.getEpoch() - Duration::Days(1.0), tle1.getEpoch() + Duration::Days(0.5));
        const Interval interval2 =
            Interval::Closed(tle2.getEpoch() - Duration::Days(0.5), tle2.getEpoch() + Duration::Days(1.0));

        // Input in reversed order
        const Array<Pair<TLE, Interval>> tleIntervalArray = {
            {tle2, interval2},
            {tle1, interval1},
        };

        const SGP4 sgp4(tleIntervalArray);
        EXPECT_EQ(tle1.getEpoch(), sgp4.getTles()[0].getEpoch());
        EXPECT_EQ(tle2.getEpoch(), sgp4.getTles()[1].getEpoch());
    }

    // TLE interval does not contain the TLE epoch
    {
        const Interval interval =
            Interval::Closed(tle_.getEpoch() - Duration::Days(1.0), tle_.getEpoch() - Duration::Days(0.5));
        const Array<Pair<TLE, Interval>> tleIntervalArray = {{tle_, interval}};

        EXPECT_THROW(SGP4 sgp4(tleIntervalArray), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, CopyConstructor)

{
    const SGP4 sgp4Copy(sgp4_);

    EXPECT_TRUE(sgp4Copy.isDefined());
    EXPECT_EQ(sgp4_, sgp4Copy);

    const State state = sgp4_.calculateStateAt(tle_.getEpoch());
    const State stateCopy = sgp4Copy.calculateStateAt(tle_.getEpoch());

    EXPECT_LT((state.getPosition().accessCoordinates() - stateCopy.getPosition().accessCoordinates()).norm(), 1e-6);
    EXPECT_LT((state.getVelocity().accessCoordinates() - stateCopy.getVelocity().accessCoordinates()).norm(), 1e-9);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, CopyAssignmentOperator)
{
    SGP4 sgp4Copy(tle_, Frame::TEME());

    EXPECT_NE(sgp4_, sgp4Copy);

    sgp4Copy = sgp4_;

    EXPECT_EQ(sgp4_, sgp4Copy);

    const State state = sgp4_.calculateStateAt(tle_.getEpoch());
    const State stateCopy = sgp4Copy.calculateStateAt(tle_.getEpoch());

    EXPECT_LT((state.getPosition().accessCoordinates() - stateCopy.getPosition().accessCoordinates()).norm(), 1e-6);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, Clone)
{
    const SGP4* sgp4ClonePtr = sgp4_.clone();

    EXPECT_TRUE(sgp4ClonePtr->isDefined());
    EXPECT_EQ(sgp4_, *sgp4ClonePtr);

    const State state = sgp4_.calculateStateAt(tle_.getEpoch());
    const State stateClone = sgp4ClonePtr->calculateStateAt(tle_.getEpoch());

    EXPECT_LT((state.getPosition().accessCoordinates() - stateClone.getPosition().accessCoordinates()).norm(), 1e-6);

    delete sgp4ClonePtr;
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, EqualityOperator)
{
    // Same TLE, same frame
    {
        const SGP4 sgp4A(tle_);
        const SGP4 sgp4B(tle_);

        EXPECT_TRUE(sgp4A == sgp4B);
        EXPECT_FALSE(sgp4A != sgp4B);
    }

    // Same TLE, different frame
    {
        const SGP4 sgp4A(tle_);
        const SGP4 sgp4B(tle_, Frame::TEME());

        EXPECT_FALSE(sgp4A == sgp4B);
        EXPECT_TRUE(sgp4A != sgp4B);
    }

    // Different TLE
    {
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const SGP4 sgp4A(tle_);
        const SGP4 sgp4B(tle2);

        EXPECT_FALSE(sgp4A == sgp4B);
        EXPECT_TRUE(sgp4A != sgp4B);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, StreamOperator)
{
    std::ostringstream stream;
    stream << sgp4_;

    EXPECT_FALSE(stream.str().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, IsDefined)
{
    EXPECT_TRUE(sgp4_.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, GetTle)
{
    // Single TLE
    {
        EXPECT_EQ(tle_, sgp4_.getTle());
    }

    // Multiple TLEs should throw
    {
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Array<TLE> tleArray = {tle_, tle2};
        const SGP4 sgp4(tleArray);

        EXPECT_THROW(sgp4.getTle(), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, GetEpoch)
{
    // Single TLE
    {
        EXPECT_EQ(tle_.getEpoch(), sgp4_.getEpoch());
    }

    // Multiple TLEs should throw
    {
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Array<TLE> tleArray = {tle_, tle2};
        const SGP4 sgp4(tleArray);

        EXPECT_THROW(sgp4.getEpoch(), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, GetRevolutionNumberAtEpoch)
{
    // Single TLE
    {
        EXPECT_EQ(tle_.getRevolutionNumberAtEpoch(), sgp4_.getRevolutionNumberAtEpoch());
    }

    // Multiple TLEs should throw
    {
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Array<TLE> tleArray = {tle_, tle2};
        const SGP4 sgp4(tleArray);

        EXPECT_THROW(sgp4.getRevolutionNumberAtEpoch(), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, Print)
{
    // Single TLE with decorator
    {
        std::ostringstream stream;
        sgp4_.print(stream, true);

        EXPECT_FALSE(stream.str().empty());
        EXPECT_NE(std::string::npos, stream.str().find("SGP4"));
    }

    // Single TLE without decorator
    {
        std::ostringstream stream;
        sgp4_.print(stream, false);

        EXPECT_FALSE(stream.str().empty());
    }

    // Multiple TLEs
    {
        TLE tle2 = tle_;
        tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

        const Array<TLE> tleArray = {tle_, tle2};
        const SGP4 sgp4(tleArray);

        std::ostringstream stream;
        sgp4.print(stream, true);

        EXPECT_FALSE(stream.str().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, OutputFrame)
{
    // Default constructor outputs in GCRF
    {
        EXPECT_EQ(Frame::GCRF(), sgp4_.getOutputFrame());

        const State state = sgp4_.calculateStateAt(tle_.getEpoch());
        EXPECT_EQ(*Frame::GCRF(), *state.accessFrame());
    }

    // Constructor with TEME output frame
    {
        const SGP4 sgp4(tle_, Frame::TEME());
        EXPECT_EQ(Frame::TEME(), sgp4.getOutputFrame());

        const State state = sgp4.calculateStateAt(tle_.getEpoch());
        EXPECT_EQ(*Frame::TEME(), *state.accessFrame());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, CalculateStateAt_MultiTLE)
{
    const TLE tle1 = tle_;
    TLE tle2 = tle_;
    tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

    const Array<TLE> tleArray = {tle1, tle2};
    const SGP4 sgp4Multi(tleArray, Frame::GCRF());

    // State at tle1 epoch should match single-TLE SGP4 with tle1
    {
        const SGP4 sgp4Single(tle1);
        const State stateMulti = sgp4Multi.calculateStateAt(tle1.getEpoch());
        const State stateSingle = sgp4Single.calculateStateAt(tle1.getEpoch());

        EXPECT_LT(
            (stateMulti.getPosition().accessCoordinates() - stateSingle.getPosition().accessCoordinates()).norm(), 1e-6
        );
        EXPECT_LT(
            (stateMulti.getVelocity().accessCoordinates() - stateSingle.getVelocity().accessCoordinates()).norm(), 1e-9
        );
    }

    // State at tle2 epoch should match single-TLE SGP4 with tle2
    {
        const SGP4 sgp4Single(tle2);
        const State stateMulti = sgp4Multi.calculateStateAt(tle2.getEpoch());
        const State stateSingle = sgp4Single.calculateStateAt(tle2.getEpoch());

        EXPECT_LT(
            (stateMulti.getPosition().accessCoordinates() - stateSingle.getPosition().accessCoordinates()).norm(), 1e-6
        );
        EXPECT_LT(
            (stateMulti.getVelocity().accessCoordinates() - stateSingle.getVelocity().accessCoordinates()).norm(), 1e-9
        );
    }

    // State closer to tle1 should use tle1
    {
        const Instant instantNearTle1 =
            tle1.getEpoch() + Duration::Hours(6.0);  // 6 hours after tle1, 18 hours before tle2
        const SGP4 sgp4Single(tle1);
        const State stateMulti = sgp4Multi.calculateStateAt(instantNearTle1);
        const State stateSingle = sgp4Single.calculateStateAt(instantNearTle1);

        EXPECT_LT(
            (stateMulti.getPosition().accessCoordinates() - stateSingle.getPosition().accessCoordinates()).norm(), 1e-6
        );
    }

    // State closer to tle2 should use tle2
    {
        const Instant instantNearTle2 =
            tle2.getEpoch() - Duration::Hours(6.0);  // 18 hours after tle1, 6 hours before tle2
        const SGP4 sgp4Single(tle2);
        const State stateMulti = sgp4Multi.calculateStateAt(instantNearTle2);
        const State stateSingle = sgp4Single.calculateStateAt(instantNearTle2);

        EXPECT_LT(
            (stateMulti.getPosition().accessCoordinates() - stateSingle.getPosition().accessCoordinates()).norm(), 1e-6
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, CalculateStatesAt_MultiTLE)
{
    const TLE tle1 = tle_;
    TLE tle2 = tle_;
    tle2.setEpoch(tle2.getEpoch() + Duration::Days(1.0));

    const Array<TLE> tleArray = {tle1, tle2};
    const SGP4 sgp4(tleArray, Frame::GCRF());

    const Array<Instant> instants = {
        tle1.getEpoch(),
        tle1.getEpoch() + Duration::Hours(6.0),
        tle2.getEpoch() - Duration::Hours(6.0),
        tle2.getEpoch(),
    };

    const Array<State> states = sgp4.calculateStatesAt(instants);

    EXPECT_EQ(4u, states.getSize());

    // Verify each state matches individual calculateStateAt
    for (Size i = 0; i < instants.getSize(); ++i)
    {
        const State individualState = sgp4.calculateStateAt(instants[i]);
        EXPECT_LT(
            (states[i].getPosition().accessCoordinates() - individualState.getPosition().accessCoordinates()).norm(),
            1e-6
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, CalculateStatesAt_SingleTLE)
{
    const Array<Instant> instants = {
        tle_.getEpoch(),
        tle_.getEpoch() + Duration::Hours(1.0),
        tle_.getEpoch() + Duration::Hours(2.0),
    };

    const Array<State> states = sgp4_.calculateStatesAt(instants);

    EXPECT_EQ(3u, states.getSize());

    for (Size i = 0; i < instants.getSize(); ++i)
    {
        const State individualState = sgp4_.calculateStateAt(instants[i]);
        EXPECT_LT(
            (states[i].getPosition().accessCoordinates() - individualState.getPosition().accessCoordinates()).norm(),
            1e-6
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_SGP4, Test_1)
{
    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const TLE tle = TLE::Load(File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/Test_1/Satellite.tle")
        ));

        const SGP4 sgp4Model = {tle};

        // Orbit setup

        const Orbit orbit = {sgp4Model, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/Test_1/Satellite Orbit.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_TEME = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
            };
            const Vector3d referenceVelocity_TEME = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
            };

            const Vector3d referencePosition_GCRF = {
                referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()
            };
            const Vector3d referenceVelocity_GCRF = {
                referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()
            };

            const Vector3d referencePosition_ITRF = {
                referenceRow[13].accessReal(), referenceRow[14].accessReal(), referenceRow[15].accessReal()
            };
            const Vector3d referenceVelocity_ITRF = {
                referenceRow[16].accessReal(), referenceRow[17].accessReal(), referenceRow[18].accessReal()
            };

            // const Real referenceRevolutionNumber = referenceRow[19].accessReal();

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.getPosition();
            const Velocity velocity_GCRF = state_GCRF.getVelocity();

            EXPECT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            EXPECT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            EXPECT_LT((position_GCRF.accessCoordinates() - referencePosition_GCRF).norm(), 10.0);
            EXPECT_LT((velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm(), 1e-2);

            const Shared<const Frame> teme = Frame::TEME();

            const State state_TEME = state_GCRF.inFrame(teme);

            const Position position_TEME = state_TEME.getPosition();
            const Velocity velocity_TEME = state_TEME.getVelocity();

            // STK

            // const Quaternion ref_q_TEME_GCRF = Quaternion::XYZS(0.000013730697, 0.000889461232, -0.002075913109,
            // 0.999997449624).normalize();

            // OUT

            // const Quaternion q_TEME_GCRF = Frame::GCRF()->getTransformTo(Frame::TEME(), instant).getOrientation();

            // std::cout << "[REF] q_TEME_GCRF = " << ref_q_TEME_GCRF.toString(12) << std::endl;

            // std::cout << "[REF] x_GCRF: " << referencePosition_GCRF.toString(12) << std::endl;
            // std::cout << "[REF] x_TEME: " << referencePosition_TEME.toString(12) << std::endl;

            // std::cout << "q_TEME_GCRF = " << q_TEME_GCRF.toString(12) << std::endl;

            // std::cout << "x_GCRF: " << position_GCRF.accessCoordinates().toString(12) << std::endl;
            // std::cout << "x_TEME: " << position_TEME.accessCoordinates().toString(12) << std::endl;

            // std::cout << "dq = " << RotationVector::Quaternion((q_TEME_GCRF /
            // ref_q_TEME_GCRF).normalize().rectify()).getAngle().inArcseconds().toString(12) << " [asec]" << std::endl
            //; std::cout << "dx_GCRF = " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " [m]" << std::endl; std::cout << "dx_TEME = " <<
            // Real((position_TEME.accessCoordinates() - referencePosition_TEME).norm()).toString(12) << " [m]" <<
            // std::endl;

            // std::cout << "x_GCRF A: " << (ref_q_TEME_GCRF.toConjugate() *
            // position_TEME.accessCoordinates()).toString(12) << " [m]" << std::endl; std::cout << "x_GCRF B: " <<
            // (q_TEME_GCRF.toConjugate() * position_TEME.accessCoordinates()).toString(12) << " [m]" << std::endl;

            // std::cout << "position_GCRF 1 = " << std::endl << position_GCRF << std::endl;
            // std::cout << "position_GCRF 2 = " << std::endl <<
            // state_GCRF.inFrame(teme).inFrame(Frame::GCRF()).getPosition() << std::endl;

            // std::cout << "position_TEME 1 = " << std::endl << position_TEME << std::endl;
            // std::cout << "position_TEME 2 = " << std::endl <<
            // state_TEME.inFrame(Frame::GCRF()).inFrame(teme).getPosition() << std::endl;

            EXPECT_EQ(*Frame::TEME(), *position_TEME.accessFrame());
            EXPECT_EQ(*Frame::TEME(), *velocity_TEME.accessFrame());

            EXPECT_LT((position_TEME.accessCoordinates() - referencePosition_TEME).norm(), 10.0);
            EXPECT_LT((velocity_TEME.accessCoordinates() - referenceVelocity_TEME).norm(), 1e-2);

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.getPosition();
            const Velocity velocity_ITRF = state_ITRF.getVelocity();

            EXPECT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            EXPECT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            EXPECT_LT((position_ITRF.accessCoordinates() - referencePosition_ITRF).norm(), 10.0);
            EXPECT_LT((velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm(), 1e-2);

            // EXPECT_EQ(referenceRevolutionNumber.floor(), orbit.getRevolutionNumberAt(instant));

            // std::cout << "x @ GCRF = " << referencePosition_GCRF.toString(10) << " / " <<
            // position_GCRF.accessCoordinates().toString(10) << std::endl; std::cout << "x @ ITRF = " <<
            // referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) <<
            // std::endl; std::cout << "dx = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()
            // << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl;

            // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " <<
            // velocity_GCRF.accessCoordinates().toString(10) << std::endl; std::cout << "v @ ITRF = " <<
            // referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) <<
            // std::endl; std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()
            // << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl;

            // std::cout   << "dx = " << Real((position_TEME.accessCoordinates() -
            // referencePosition_TEME).norm()).toString(12) << " - " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12)
            //    << " | " << "dv = " << Real((velocity_TEME.accessCoordinates() -
            //    referenceVelocity_TEME).norm()).toString(12) << " - " << Real((velocity_GCRF.accessCoordinates() -
            //    referenceVelocity_GCRF).norm()).toString(12) << std::endl;

            // std::cout   << "dx = " << Real((position_TEME.accessCoordinates() -
            // referencePosition_TEME).norm()).toString(12) << " - " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12)
            //    << " | " << "dv = " << Real((velocity_TEME.accessCoordinates() -
            //    referenceVelocity_TEME).norm()).toString(12) << " - " << Real((velocity_GCRF.accessCoordinates() -
            //    referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() -
            //    referenceVelocity_ITRF).norm()).toString(12) << std::endl;

            // FAIL();
        }
    }
}
