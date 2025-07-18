/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Utility/Eclipse.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Eclipse/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::Environment;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::utilities::Eclipse;
using ostk::physics::environment::utilities::EclipsePhase;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::eclipse::Generator;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

// Helper function to build trajectory from orbital elements
Trajectory buildTrajectory(
    const Environment& anEnvironment,
    const Instant& anEpoch = Instant::J2000(),
    const Length& aSemiMajorAxis = Length::Meters(6700e3)
)
{
    const COE coe = {
        aSemiMajorAxis,
        0.0,                  // eccentricity
        Angle::Degrees(0.0),  // inclination
        Angle::Degrees(0.0),  // raan
        Angle::Degrees(0.0),  // aop
        Angle::Degrees(0.0)   // true anomaly
    };

    const Shared<const Celestial> earth = anEnvironment.accessCelestialObjectWithName("Earth");

    const Kepler kepler = {coe, anEpoch, *earth, Kepler::PerturbationType::None, false};

    return Orbit(kepler, earth);
}

// Helper function to parse instant from string
Instant parseInstant(const String& aTimeString)
{
    return Instant::Parse(aTimeString, Scale::UTC);
}

// Helper function to create position from LLA
Position positionFromLLA(const LLA& aLLA, const Shared<const Celestial>& anEarthSPtr)
{
    const Vector3d cartesian = aLLA.toCartesian(anEarthSPtr->getEquatorialRadius(), anEarthSPtr->getFlattening());
    return Position::Meters(cartesian, Frame::ITRF());
}

// Helper function to assert eclipses match expected values
void assertEclipses(
    const Array<Eclipse>& actualEclipses, const Array<Array<EclipsePhase>>& expectedEclipses, const Duration& tolerance
)
{
    EXPECT_EQ(actualEclipses.getSize(), expectedEclipses.getSize());

    for (size_t eclipseIndex = 0; eclipseIndex < actualEclipses.getSize(); ++eclipseIndex)
    {
        const Eclipse& actualEclipse = actualEclipses[eclipseIndex];
        const Array<EclipsePhase>& expectedPhases = expectedEclipses[eclipseIndex];
        const Array<EclipsePhase>& actualPhases = actualEclipse.getPhases();

        EXPECT_EQ(actualPhases.getSize(), expectedPhases.getSize())
            << "Eclipse " << eclipseIndex << " has wrong number of phases";

        for (size_t phaseIndex = 0; phaseIndex < actualPhases.getSize(); ++phaseIndex)
        {
            const EclipsePhase& actualPhase = actualPhases[phaseIndex];
            const EclipsePhase& expectedPhase = expectedPhases[phaseIndex];

            EXPECT_EQ(actualPhase.getRegion(), expectedPhase.getRegion())
                << "Eclipse " << eclipseIndex << ", Phase " << phaseIndex << " has wrong region";

            const Duration startTimeDifference =
                actualPhase.getInterval().getStart() - expectedPhase.getInterval().getStart();
            EXPECT_TRUE(startTimeDifference.isNear(Duration::Zero(), tolerance))
                << "Eclipse " << eclipseIndex << ", Phase " << phaseIndex
                << " start time differs by more than tolerance";

            const Duration endTimeDifference =
                actualPhase.getInterval().getEnd() - expectedPhase.getInterval().getEnd();
            EXPECT_TRUE(endTimeDifference.isNear(Duration::Zero(), tolerance))
                << "Eclipse " << eclipseIndex << ", Phase " << phaseIndex << " end time differs by more than tolerance";

            EXPECT_EQ(actualPhase.isComplete(), expectedPhase.isComplete())
                << "Eclipse " << eclipseIndex << ", Phase " << phaseIndex << " has wrong completeness flag";
        }
    }
}

class OpenSpaceToolkit_Astrodynamics_Eclipse_Generator : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        environment_ = Environment::Default();
        searchStepSize_ = Duration::Seconds(60.0);
        searchTolerance_ = Duration::Milliseconds(1.0);
        trajectory_ = buildTrajectory(environment_);
    }

    Environment environment_ = Environment::Undefined();
    Duration searchStepSize_ = Duration::Undefined();
    Duration searchTolerance_ = Duration::Undefined();
    Trajectory trajectory_ = Trajectory::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Eclipse_Generator, ConstructorAndProperties)
{
    // Test constructor with all parameters
    {
        const Generator generator(environment_, searchStepSize_, searchTolerance_);

        EXPECT_TRUE(generator.isDefined());
        EXPECT_TRUE(generator.getEnvironment().isDefined());
        EXPECT_EQ(generator.getSearchStepSize(), searchStepSize_);
        EXPECT_EQ(generator.getSearchTolerance(), searchTolerance_);
    }

    // Test constructor with default parameters
    {
        const Generator generator;

        EXPECT_TRUE(generator.isDefined());
        EXPECT_TRUE(generator.getSearchStepSize().isDefined());
        EXPECT_TRUE(generator.getSearchTolerance().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Eclipse_Generator, Generate_Eclipses_Trajectory_With_No_Eclipses)
{
    const Generator generator(environment_, searchStepSize_, searchTolerance_);

    const Trajectory geo =
        buildTrajectory(environment_, parseInstant("2000-06-19T00:00:00.000Z"), Length::Meters(42164e3));
    const Interval analysisInterval =
        Interval::Closed(parseInstant("2000-06-19T00:00:00.000Z"), parseInstant("2000-06-23T00:00:00.000Z"));

    const Array<Eclipse> eclipses = generator.generate(geo, analysisInterval);

    // Data from Orekit
    const Array<Array<EclipsePhase>> expectedEclipses = {};

    // Assert eclipses match expected values
    assertEclipses(eclipses, expectedEclipses, Duration::Milliseconds(250.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Eclipse_Generator, Generate_Eclipses_LEO_Trajectory)
{
    const Generator generator(environment_, searchStepSize_, searchTolerance_);

    const Trajectory leo = buildTrajectory(environment_, Instant::J2000(), Length::Meters(6700e3));
    const Interval analysisInterval =
        Interval::Closed(parseInstant("2000-01-01T14:00:00.000Z"), parseInstant("2000-01-01T17:00:00.000Z"));

    const Array<Eclipse> eclipses = generator.generate(leo, analysisInterval);

    // Data from Orekit
    const Array<Array<EclipsePhase>> expectedEclipses = {
        // Eclipse 1
        {EclipsePhase(
             EclipsePhase::Region::Umbra,
             Interval::Closed(
                 parseInstant("2000-01-01T14:00:00.000000000Z"), parseInstant("2000-01-01T14:13:16.055522687Z")
             ),
             false
         ),
         EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-01-01T14:13:16.055522687Z"), parseInstant("2000-01-01T14:13:25.067876961Z")
             ),
             true
         )},
        // Eclipse 2
        {EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-01-01T15:08:35.144864310Z"), parseInstant("2000-01-01T15:08:44.156951218Z")
             ),
             true
         ),
         EclipsePhase(
             EclipsePhase::Region::Umbra,
             Interval::Closed(
                 parseInstant("2000-01-01T15:08:44.156951218Z"), parseInstant("2000-01-01T15:44:14.994980968Z")
             ),
             true
         ),
         EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-01-01T15:44:14.994980968Z"), parseInstant("2000-01-01T15:44:24.006957218Z")
             ),
             true
         )},
        // Eclipse 3
        {EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-01-01T16:39:34.061018956Z"), parseInstant("2000-01-01T16:39:43.072726210Z")
             ),
             true
         ),
         EclipsePhase(
             EclipsePhase::Region::Umbra,
             Interval::Closed(
                 parseInstant("2000-01-01T16:39:43.072726210Z"), parseInstant("2000-01-01T17:00:00.000000000Z")
             ),
             false
         )}
    };

    // Assert eclipses match expected values
    assertEclipses(eclipses, expectedEclipses, Duration::Milliseconds(250.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Eclipse_Generator, Generate_Eclipses_GEO_Trajectory)
{
    const Generator generator(environment_, searchStepSize_, searchTolerance_);

    const Trajectory geo =
        buildTrajectory(environment_, parseInstant("2000-03-18T00:00:00.000Z"), Length::Meters(42164e3));
    const Interval analysisInterval =
        Interval::Closed(parseInstant("2000-03-18T11:16:00.000Z"), parseInstant("2000-03-20T12:25:00.000"));

    const Array<Eclipse> eclipses = generator.generate(geo, analysisInterval);

    // Data from Orekit
    const Array<Array<EclipsePhase>> expectedEclipses = {
        // Eclipse 1
        {EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-03-18T11:16:00.000000000Z"), parseInstant("2000-03-18T11:17:49.170283546Z")
             ),
             false  // Search starts in penumbra.
         ),
         EclipsePhase(
             EclipsePhase::Region::Umbra,
             Interval::Closed(
                 parseInstant("2000-03-18T11:17:49.170283546Z"), parseInstant("2000-03-18T12:25:02.640634319Z")
             ),
             true
         ),
         EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-03-18T12:25:02.640634319Z"), parseInstant("2000-03-18T12:27:11.505337272Z")
             ),
             true
         )},
        // Eclipse 2
        {EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-03-19T11:15:16.494817350Z"), parseInstant("2000-03-19T11:17:24.967283715Z")
             ),
             true
         ),
         EclipsePhase(
             EclipsePhase::Region::Umbra,
             Interval::Closed(
                 parseInstant("2000-03-19T11:17:24.967283715Z"), parseInstant("2000-03-19T12:24:50.171841532Z")
             ),
             true
         ),
         EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-03-19T12:24:50.171841532Z"), parseInstant("2000-03-19T12:26:58.642335399Z")
             ),
             true
         )},
        // Eclipse 3
        {EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-03-20T11:14:56.723634367Z"), parseInstant("2000-03-20T11:17:05.074445450Z")
             ),
             true
         ),
         EclipsePhase(
             EclipsePhase::Region::Umbra,
             Interval::Closed(
                 parseInstant("2000-03-20T11:17:05.074445450Z"), parseInstant("2000-03-20T12:24:33.109398732Z")
             ),
             true
         ),
         EclipsePhase(
             EclipsePhase::Region::Penumbra,
             Interval::Closed(
                 parseInstant("2000-03-20T12:24:33.109398732Z"), parseInstant("2000-03-20T12:25:00.000000000Z")
             ),
             false  // Search ends in penumbra.
         )}
    };

    // Assert eclipses match expected values
    assertEclipses(eclipses, expectedEclipses, Duration::Milliseconds(250.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Eclipse_Generator, Generate_Eclipses_Trajectory_Starts_And_Ends_In_Eclipse)
{
    const Generator generator(environment_, searchStepSize_, searchTolerance_);

    const Trajectory geo =
        buildTrajectory(environment_, parseInstant("2000-03-18T00:00:00.000Z"), Length::Meters(42164e3));
    const Interval analysisInterval =
        Interval::Closed(parseInstant("2000-03-18T11:16:00.000Z"), parseInstant("2000-03-18T11:17:00.000"));

    const Array<Eclipse> eclipses = generator.generate(geo, analysisInterval);

    // Data from Orekit
    const Array<Array<EclipsePhase>> expectedEclipses = {
        // Eclipse 1
        {EclipsePhase(
            EclipsePhase::Region::Penumbra,
            Interval::Closed(
                parseInstant("2000-03-18T11:16:00.000000000Z"), parseInstant("2000-03-18T11:17:00.000000000Z")
            ),
            false  // Search starts in penumbra.
        )}
    };

    // Assert eclipses match expected values
    assertEclipses(eclipses, expectedEclipses, Duration::Milliseconds(250.0));
}
