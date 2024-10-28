/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/AER.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::container::Tuple;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Index;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::AER;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::Access;
using ostk::astrodynamics::access::Generator;
using ostk::astrodynamics::access::GroundTargetConfiguration;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, Constructor)
{
    {
        const Environment environment = Environment::Default();

        EXPECT_NO_THROW(Generator generator(environment););
    }

    {
        const Environment environment = Environment::Default();

        const auto aerFilter = [](const AER&) -> bool
        {
            return true;
        };

        const auto accessFilter = [](const Access&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(Generator generator(environment, aerFilter, accessFilter););
    }

    {
        const Environment environment = Environment::Default();

        const auto aerFilter = [](const AER&) -> bool
        {
            return true;
        };

        const auto accessFilter = [](const Access&) -> bool
        {
            return true;
        };

        const auto stateFilter = [](const State&, const State&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(Generator generator(environment, aerFilter, accessFilter, stateFilter););
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, IsDefined)
{
    {
        const Environment environment = Environment::Default();

        const Generator generator = {environment};

        EXPECT_TRUE(generator.isDefined());
    }

    {
        const Environment environment = Environment::Default();

        const auto aerFilter = [](const AER&) -> bool
        {
            return true;
        };

        const auto accessFilter = [](const Access&) -> bool
        {
            return true;
        };

        const Generator generator = {environment, aerFilter, accessFilter};

        EXPECT_TRUE(generator.isDefined());
    }

    {
        EXPECT_FALSE(Generator::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetStep)
{
    {
        const Generator generator = {Environment::Default()};

        EXPECT_EQ(Duration::Minutes(1.0), generator.getStep());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getStep());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetTolerance)
{
    {
        const Generator generator = {Environment::Default()};

        EXPECT_EQ(Duration::Microseconds(1.0), generator.getTolerance());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getTolerance());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetAerFilter)
{
    {
        const Generator generator = {Environment::Default()};

        EXPECT_EQ(nullptr, generator.getAerFilter());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getAerFilter());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetAccessFilter)
{
    {
        const Generator generator = {Environment::Default()};

        EXPECT_EQ(nullptr, generator.getAccessFilter());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getAccessFilter());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetStateFilter)
{
    {
        const Generator generator = {Environment::Default()};

        EXPECT_EQ(nullptr, generator.getStateFilter());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getStateFilter());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetConditionFunction)
{
    const Environment environment = Environment::Default();

    const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

    const auto generateFirstOrbit = [&environment, &epoch]() -> Orbit
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(+45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const auto generateSecondOrbit = [&environment, &epoch]() -> Orbit
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(+45.0);
        const Angle raan = Angle::Degrees(180.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(180.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const Orbit fromOrbit = generateFirstOrbit();
    const Orbit toOrbit = generateSecondOrbit();

    {
        const Generator generator = {environment};

        const auto conditionFunction = generator.getConditionFunction(fromOrbit, toOrbit);

        EXPECT_NE(nullptr, conditionFunction);
        EXPECT_TRUE(conditionFunction(epoch));
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getConditionFunction(fromOrbit, toOrbit));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses_1)
{
    const Environment environment = Environment::Default();

    const Generator generator = {environment};

    const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
    const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);

    const Interval interval = Interval::Closed(startInstant, endInstant);

    const auto generateFirstOrbit = [&environment, &startInstant]() -> Orbit
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(+45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = startInstant;
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const auto generateSecondOrbit = [&environment, &startInstant]() -> Orbit
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(+45.0);
        const Angle raan = Angle::Degrees(180.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(180.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = startInstant;
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const Orbit fromOrbit = generateFirstOrbit();
    const Orbit toOrbit = generateSecondOrbit();

    const Array<Access> accesses = generator.computeAccesses(interval, fromOrbit, toOrbit);

    // Reference data setup

    const File referenceDataFile = File::Path(
        Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 1.csv")
    );

    const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

    const Duration toleranceDuration = Duration::Seconds(0.1);

    // Test

    ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

    for (const auto accessTuple : ostk::core::container::iterator::Zip(referenceData, accesses))
    {
        const auto& referenceRow = std::get<0>(accessTuple);
        const Access& access = std::get<1>(accessTuple);

        const Instant reference_acquisitionOfSignal =
            Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
        const Instant reference_timeOfClosestApproach =
            Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
        const Instant reference_lossOfSignal =
            Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);
        const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal());

        ASSERT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration))
            << String::Format(
                   "{} ~ {}", reference_acquisitionOfSignal.toString(), access.getAcquisitionOfSignal().toString()
               );
        ASSERT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration))
            << String::Format(
                   "{} ~ {}", reference_timeOfClosestApproach.toString(), access.getTimeOfClosestApproach().toString()
               );
        ASSERT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
            << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
        ASSERT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
            << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses_2)
{
    const Environment environment = Environment::Default();

    const Generator generator = {environment};

    const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
    const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC);

    const Interval interval = Interval::Closed(startInstant, endInstant);

    const auto generateGroundStationTrajectory = []() -> Trajectory
    {
        const LLA groundStationLla = {Angle::Degrees(0.0), Angle::Degrees(0.0), Length::Meters(20.0)};

        const Position groundStationPosition = Position::Meters(
            groundStationLla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
        );

        return Trajectory::Position(groundStationPosition);
    };

    const auto generateSatelliteOrbit = [&environment, &startInstant]() -> Orbit
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(+45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = startInstant;
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const Trajectory groundStationTrajectory = generateGroundStationTrajectory();
    const Orbit satelliteOrbit = generateSatelliteOrbit();

    const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit);

    // Reference data setup

    const File referenceDataFile = File::Path(
        Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 2.csv")
    );

    const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

    const Duration toleranceDuration = Duration::Seconds(0.1);

    // Test

    ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

    for (const auto accessTuple : ostk::core::container::iterator::Zip(referenceData, accesses))
    {
        const auto& referenceRow = std::get<0>(accessTuple);
        const Access& access = std::get<1>(accessTuple);

        const Instant reference_acquisitionOfSignal =
            Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
        const Instant reference_timeOfClosestApproach =
            Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
        const Instant reference_lossOfSignal =
            Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);
        const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal());

        EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration))
            << String::Format(
                   "{} ~ {}", reference_acquisitionOfSignal.toString(), access.getAcquisitionOfSignal().toString()
               );
        EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration))
            << String::Format(
                   "{} ~ {}", reference_timeOfClosestApproach.toString(), access.getTimeOfClosestApproach().toString()
               );
        EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
            << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
        EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
            << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses_3)
{
    const Environment environment = Environment::Default();

    const Generator generator = {environment};

    const Instant startInstant = Instant::DateTime(DateTime(2018, 9, 6, 0, 0, 0), Scale::UTC);
    const Instant endInstant = Instant::DateTime(DateTime(2018, 9, 7, 0, 0, 0), Scale::UTC);

    const Interval interval = Interval::Closed(startInstant, endInstant);

    const auto generateGroundStationTrajectory = []() -> Trajectory
    {
        const LLA groundStationLla = {Angle::Degrees(-45.0), Angle::Degrees(-170.0), Length::Meters(5.0)};

        const Position groundStationPosition = Position::Meters(
            groundStationLla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
        );

        return Trajectory::Position(groundStationPosition);
    };

    const auto generateSatelliteOrbit = [&environment, &startInstant]() -> Orbit
    {
        const TLE tle = {
            "1 39419U 13066D   18248.44969859 -.00000394  00000-0 -31796-4 0  9997",
            "2 39419  97.6313 314.6863 0012643 218.7350 141.2966 14.93878994260975"
        };

        const SGP4 orbitalModel = {tle};

        const Orbit orbit = {orbitalModel, environment.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const Trajectory groundStationTrajectory = generateGroundStationTrajectory();
    const Orbit satelliteOrbit = generateSatelliteOrbit();

    const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit);

    // Reference data setup

    const File referenceDataFile = File::Path(
        Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 3.csv")
    );

    const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

    const Duration toleranceDuration = Duration::Seconds(0.1);

    // Test

    ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

    for (const auto accessTuple : ostk::core::container::iterator::Zip(referenceData, accesses))
    {
        const auto& referenceRow = std::get<0>(accessTuple);
        const Access& access = std::get<1>(accessTuple);

        const Instant reference_acquisitionOfSignal =
            Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
        const Instant reference_timeOfClosestApproach =
            Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
        const Instant reference_lossOfSignal =
            Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);
        const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal());

        EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration))
            << String::Format(
                   "{} ~ {}", reference_acquisitionOfSignal.toString(), access.getAcquisitionOfSignal().toString()
               );
        EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration))
            << String::Format(
                   "{} ~ {}", reference_timeOfClosestApproach.toString(), access.getTimeOfClosestApproach().toString()
               );
        EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
            << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
        EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
            << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses_4)
{
    const Instant startInstant = Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC);
    const Instant endInstant = Instant::DateTime(DateTime(2020, 1, 1, 0, 10, 0), Scale::UTC);

    const Interval interval = Interval::Closed(startInstant, endInstant);
    const Duration step = Duration::Minutes(1.0);

    const auto generateTrajectory = [&interval,
                                     &step](const Position& aStartPosition, const Velocity& aVelocity) -> Trajectory
    {
        Array<State> states = Array<State>::Empty();

        for (const auto& instant : interval.generateGrid(step))
        {
            const State state = {
                instant,
                Position::Meters(
                    aStartPosition.getCoordinates() +
                        (aVelocity.getCoordinates() * (instant - interval.getStart()).inSeconds()),
                    Frame::GCRF()
                ),
                aVelocity
            };

            states.add(state);
        }

        return Trajectory(states);
    };

    const Trajectory firstTrajectory = generateTrajectory(
        Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()), Velocity::MetersPerSecond({1.0, 0.0, 0.0}, Frame::GCRF())
    );

    const Trajectory secondTrajectory = generateTrajectory(
        Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()), Velocity::MetersPerSecond({0.0, 0.0, 1.0}, Frame::GCRF())
    );

    {
        const auto stateFilter = [](const State& aFirstState, const State& aSecondState) -> bool
        {
            (void)aFirstState;
            (void)aSecondState;
            return true;
        };

        const Generator generator = {Environment::Default(), {}, {}, stateFilter};

        const Array<Access> accesses = generator.computeAccesses(interval, firstTrajectory, secondTrajectory);

        EXPECT_EQ(1, accesses.getSize());

        const Duration toleranceDuration = Duration::Microseconds(1.0);

        EXPECT_TRUE(accesses.at(0).getAcquisitionOfSignal().isNear(startInstant, toleranceDuration));
        EXPECT_TRUE(accesses.at(0).getLossOfSignal().isNear(endInstant, toleranceDuration));
    }

    {
        const auto stateFilter = [](const State& aFirstState, const State& aSecondState) -> bool
        {
            (void)aSecondState;
            return (aFirstState.getInstant() - Instant::DateTime(DateTime(2020, 1, 1, 0, 5, 0), Scale::UTC))
                       .getAbsolute() >= Duration::Minutes(2.0);
        };

        const Generator generator = {Environment::Default(), {}, {}, stateFilter};

        const Array<Access> accesses = generator.computeAccesses(interval, firstTrajectory, secondTrajectory);

        EXPECT_EQ(2, accesses.getSize());

        const Duration toleranceDuration = Duration::Microseconds(1.0);

        EXPECT_TRUE(accesses.at(0).getAcquisitionOfSignal().isNear(startInstant, toleranceDuration));
        EXPECT_TRUE(accesses.at(0).getLossOfSignal().isNear(
            Instant::DateTime(DateTime(2020, 1, 1, 0, 3, 0), Scale::UTC), toleranceDuration
        ));

        EXPECT_TRUE(accesses.at(1).getAcquisitionOfSignal().isNear(
            Instant::DateTime(DateTime(2020, 1, 1, 0, 7, 0), Scale::UTC), toleranceDuration
        ));
        EXPECT_TRUE(accesses.at(1).getLossOfSignal().isNear(endInstant, toleranceDuration));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, GroundTargetConfiguration)
{
    const ostk::mathematics::object::Interval<Real> azimuthInterval =
        ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0);
    const ostk::mathematics::object::Interval<Real> elevationInterval =
        ostk::mathematics::object::Interval<Real>::Closed(0.0, 90.0);
    const ostk::mathematics::object::Interval<Real> rangeInterval =
        ostk::mathematics::object::Interval<Real>::Closed(0.0, 1.0e10);
    const LLA lla = LLA::Vector({0.0, 0.0, 0.0});

    // Constructor
    {
        {
            EXPECT_NO_THROW(GroundTargetConfiguration(azimuthInterval, elevationInterval, rangeInterval, lla));
        }

        {
            {
                const Position position = Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF());
                EXPECT_NO_THROW(GroundTargetConfiguration(azimuthInterval, elevationInterval, rangeInterval, position));
            }

            {
                const Position position = Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF());
                EXPECT_THROW(
                    GroundTargetConfiguration(azimuthInterval, elevationInterval, rangeInterval, position),
                    ostk::core::error::RuntimeError
                );
            }
        }
    }

    // Getters
    {
        const GroundTargetConfiguration groundTargetConfiguration =
            GroundTargetConfiguration(azimuthInterval, elevationInterval, rangeInterval, lla);
        EXPECT_EQ(groundTargetConfiguration.getAzimuthInterval(), azimuthInterval);
        EXPECT_EQ(groundTargetConfiguration.getElevationInterval(), elevationInterval);
        EXPECT_EQ(groundTargetConfiguration.getRangeInterval(), rangeInterval);
        EXPECT_EQ(groundTargetConfiguration.getLLA(), lla);
        EXPECT_EQ(
            groundTargetConfiguration.getPosition(),
            Position::Meters(
                lla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
            )
        );
        EXPECT_NO_THROW(groundTargetConfiguration.getTrajectory());

        Matrix3d r_SEZ_ECEF;
        r_SEZ_ECEF.row(0) = Vector3d {0.0, 0.0, 1.0};
        r_SEZ_ECEF.row(1) = Vector3d {0.0, 1.0, 0.0};
        r_SEZ_ECEF.row(2) = Vector3d {-1.0, 0.0, 0.0};

        EXPECT_EQ(groundTargetConfiguration.getR_SEZ_ECEF(), r_SEZ_ECEF);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccessesWithGroundTargets)
{
    {
        const Environment environment = Environment::Default();

        const TLE tle = {
            "1 60504U 24149AN  24293.10070306  .00000000  00000-0  58313-3 0    08",
            "2 60504  97.4383   7.6998 0003154 274.9510 182.9597 15.19652001  9607",
        };
        const SGP4 sgp4 = SGP4(tle);
        const Orbit aToTrajectory = Orbit(sgp4, environment.accessCelestialObjectWithName("Earth"));

        const Instant startInstant = Instant::Parse("2024-10-19 02:25:00.744.384", Scale::UTC);
        const Instant endInstant = startInstant + Duration::Days(1.0);
        const Interval interval = Interval::Closed(startInstant, endInstant);

        const Array<LLA> LLAs = {
            LLA(Angle::Degrees(53.406), Angle::Degrees(-6.225), Length::Meters(50.5)),
            LLA(Angle::Degrees(13.51), Angle::Degrees(144.82), Length::Meters(46)),
            LLA(Angle::Degrees(42.77), Angle::Degrees(141.62), Length::Meters(100)),
            LLA(Angle::Degrees(47.2393), Angle::Degrees(-119.88515), Length::Meters(392.5)),
            LLA(Angle::Degrees(78.22702), Angle::Degrees(15.38624), Length::Meters(493)),
            LLA(Angle::Degrees(60.674), Angle::Degrees(17.142), Length::Meters(100)),
            LLA(Angle::Degrees(37.7716), Angle::Degrees(-122.4135), Length::Meters(100)),
            LLA(Angle::Degrees(69.09), Angle::Degrees(17.6986), Length::Meters(12)),
            LLA(Angle::Degrees(78.23164), Angle::Degrees(15.37725), Length::Meters(483)),
            LLA(Angle::Degrees(-72.0021), Angle::Degrees(2.5251), Length::Meters(1401)),
            LLA(Angle::Degrees(-25.89), Angle::Degrees(27.71), Length::Meters(1562.66)),
            LLA(Angle::Degrees(-46.53), Angle::Degrees(168.38), Length::Meters(13.65)),
            LLA(Angle::Degrees(71.275), Angle::Degrees(-156.806), Length::Meters(24)),
            LLA(Angle::Degrees(-52.9351), Angle::Degrees(-70.8713), Length::Meters(23))
        };

        const ostk::mathematics::object::Interval<Real> azimuthInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0);
        const ostk::mathematics::object::Interval<Real> elevationInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 90.0);
        const ostk::mathematics::object::Interval<Real> rangeInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 1.0e10);

        Array<GroundTargetConfiguration> groundTargets = LLAs.map<GroundTargetConfiguration>(
            [&azimuthInterval, &elevationInterval, &rangeInterval](const LLA& lla) -> GroundTargetConfiguration
            {
                return GroundTargetConfiguration(azimuthInterval, elevationInterval, rangeInterval, lla);
            }
        );

        const Generator generator =
            Generator::AerRanges(azimuthInterval, elevationInterval, rangeInterval, environment);

        const Array<Array<Access>> accessesPerTarget =
            generator.computeAccessesWithGroundTargets(interval, groundTargets, aToTrajectory);

        ASSERT_EQ(accessesPerTarget.getSize(), groundTargets.getSize());

        for (Index i = 0; i < accessesPerTarget.getSize(); ++i)
        {
            const Array<Access> accesses = accessesPerTarget.at(i);
            const GroundTargetConfiguration groundTarget = groundTargets.at(i);

            const Array<Access> expectedAccesses =
                generator.computeAccesses(interval, groundTarget.getTrajectory(), aToTrajectory);

            ASSERT_EQ(accesses.getSize(), expectedAccesses.getSize());

            for (Index j = 0; j < accesses.getSize(); ++j)
            {
                const Access& access = accesses.at(j);
                const Access& expectedAccess = expectedAccesses.at(j);

                EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(
                    expectedAccess.getAcquisitionOfSignal(), Duration::Microseconds(1.0)
                )) << access.getAcquisitionOfSignal().toString()
                   << " ~ " << expectedAccess.getAcquisitionOfSignal().toString();
                EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(
                    expectedAccess.getTimeOfClosestApproach(), Duration::Microseconds(1.0)
                )) << access.getTimeOfClosestApproach().toString()
                   << " ~ " << expectedAccess.getTimeOfClosestApproach().toString();
                EXPECT_TRUE(
                    access.getLossOfSignal().isNear(expectedAccess.getLossOfSignal(), Duration::Microseconds(1.0))
                ) << access.getLossOfSignal().toString()
                  << " ~ " << expectedAccess.getLossOfSignal().toString();
                EXPECT_TRUE(access.getDuration().isNear(expectedAccess.getDuration(), Duration::Microseconds(1.0)))
                    << access.getDuration().toString() << " ~ " << expectedAccess.getDuration().toString();
            }
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SpeedTest)
{
    {
        using ostk::astrodynamics::trajectory::orbit::model::Tabulated;
        using ostk::mathematics::curvefitting::Interpolator;
        const Environment environment = Environment::Default();

        const TLE tle = {
            "1 60504U 24149AN  24293.10070306  .00000000  00000-0  58313-3 0    08",
            "2 60504  97.4383   7.6998 0003154 274.9510 182.9597 15.19652001  9607",
        };
        const SGP4 sgp4 = SGP4(tle);
        const Orbit aToTrajectory = Orbit(sgp4, environment.accessCelestialObjectWithName("Earth"));

        const Instant startInstant = Instant::Parse("2024-10-19 02:25:00.744.384", Scale::UTC);
        const Instant endInstant = startInstant + Duration::Days(14.0);
        const Interval interval = Interval::Closed(startInstant, endInstant);

        const Array<Instant> instants = interval.generateGrid(Duration::Seconds(20.0));
        const Array<State> states = aToTrajectory.getStatesAt(instants);
        const Orbit tabulatedOrbit = Orbit(
            Tabulated(states, 1, Interpolator::Type::CubicSpline), environment.accessCelestialObjectWithName("Earth")
        );

        const Array<LLA> LLAs = {
            LLA(Angle::Degrees(53.406), Angle::Degrees(-6.225), Length::Meters(50.5)),
            LLA(Angle::Degrees(13.51), Angle::Degrees(144.82), Length::Meters(46)),
            LLA(Angle::Degrees(42.77), Angle::Degrees(141.62), Length::Meters(100)),
            LLA(Angle::Degrees(47.2393), Angle::Degrees(-119.88515), Length::Meters(392.5)),
            LLA(Angle::Degrees(78.22702), Angle::Degrees(15.38624), Length::Meters(493)),
            LLA(Angle::Degrees(60.674), Angle::Degrees(17.142), Length::Meters(100)),
            LLA(Angle::Degrees(37.7716), Angle::Degrees(-122.4135), Length::Meters(100)),
            LLA(Angle::Degrees(69.09), Angle::Degrees(17.6986), Length::Meters(12)),
            LLA(Angle::Degrees(78.23164), Angle::Degrees(15.37725), Length::Meters(483)),
            LLA(Angle::Degrees(-72.0021), Angle::Degrees(2.5251), Length::Meters(1401)),
            LLA(Angle::Degrees(-25.89), Angle::Degrees(27.71), Length::Meters(1562.66)),
            LLA(Angle::Degrees(-46.53), Angle::Degrees(168.38), Length::Meters(13.65)),
            LLA(Angle::Degrees(71.275), Angle::Degrees(-156.806), Length::Meters(24)),
            LLA(Angle::Degrees(-52.9351), Angle::Degrees(-70.8713), Length::Meters(23)),
        };

        const ostk::mathematics::object::Interval<Real> azimuthInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0);
        const ostk::mathematics::object::Interval<Real> elevationInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 90.0);
        const ostk::mathematics::object::Interval<Real> rangeInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 1.0e10);

        Array<GroundTargetConfiguration> groundTargets = LLAs.map<GroundTargetConfiguration>(
            [&azimuthInterval, &elevationInterval, &rangeInterval](const LLA& lla) -> GroundTargetConfiguration
            {
                return GroundTargetConfiguration(azimuthInterval, elevationInterval, rangeInterval, lla);
            }
        );

        std::cout << "Speed test: ComputeAccessesWithGroundTargets" << std::endl;
        // start timer
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        const Generator generator =
            Generator::AerRanges(azimuthInterval, elevationInterval, rangeInterval, environment);

        const Array<Array<Access>> accessesPerTarget =
            generator.computeAccessesWithGroundTargets(interval, groundTargets, tabulatedOrbit);

        // end timer
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << " ms" << std::endl;

        ASSERT_EQ(accessesPerTarget.getSize(), groundTargets.getSize());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetStep)
{
    {
        const Environment environment = Environment::Default();

        Generator generator = {environment};

        const Duration step = Duration::Seconds(1.0);

        EXPECT_NO_THROW(generator.setStep(step));

        EXPECT_ANY_THROW(generator.setStep(Duration::Undefined()));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetTolerance)
{
    {
        const Environment environment = Environment::Default();

        Generator generator = {environment};

        const Duration tolerance = Duration::Seconds(1.0);

        EXPECT_NO_THROW(generator.setTolerance(tolerance));

        EXPECT_ANY_THROW(generator.setTolerance(Duration::Undefined()));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetAerFilter)
{
    {
        const Environment environment = Environment::Default();

        Generator generator = {environment};

        const auto aerFilter = [](const AER&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(generator.setAerFilter(aerFilter));

        EXPECT_NO_THROW(generator.setAerFilter({}));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetAccessFilter)
{
    {
        const Environment environment = Environment::Default();

        Generator generator = {environment};

        const auto accessFilter = [](const Access&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(generator.setAccessFilter(accessFilter));

        EXPECT_NO_THROW(generator.setAccessFilter({}));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetStateFilter)
{
    {
        const Environment environment = Environment::Default();

        Generator generator = {environment};

        const auto stateFilter = [](const State&, const State&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(generator.setStateFilter(stateFilter));

        EXPECT_NO_THROW(generator.setStateFilter({}));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, Undefined)
{
    {
        EXPECT_NO_THROW(Generator::Undefined());

        EXPECT_FALSE(Generator::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, AerRanges)
{
    {
        // Access computation

        const Environment environment = Environment::Default();

        const ostk::mathematics::object::Interval<Real> azimuthRange =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0);
        const ostk::mathematics::object::Interval<Real> elevationRange =
            ostk::mathematics::object::Interval<Real>::Closed(60.0, 90.0);
        const ostk::mathematics::object::Interval<Real> rangeRange =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 10000e3);

        const Generator generator = Generator::AerRanges(azimuthRange, elevationRange, rangeRange, environment);

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 10, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const auto generateGroundStationTrajectory = []() -> Trajectory
        {
            const LLA groundStationLla = {Angle::Degrees(47.8864), Angle::Degrees(106.906), Length::Meters(10.0)};

            const Position groundStationPosition = Position::Meters(
                groundStationLla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_),
                Frame::ITRF()
            );

            return Trajectory::Position(groundStationPosition);
        };

        const auto generateSatelliteOrbit = [&environment, &startInstant]() -> Orbit
        {
            const Length semiMajorAxis = Length::Kilometers(6878.14);
            const Real eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(97.5034);
            const Angle raan = Angle::Degrees(0.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(0.0);

            const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const Instant epoch = startInstant;
            const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
            const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
            const Real J2 = Earth::EGM2008.J2_;
            const Real J4 = Earth::EGM2008.J4_;

            const Kepler keplerianModel = {
                coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J2
            };

            const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const Trajectory groundStationTrajectory = generateGroundStationTrajectory();
        const Orbit satelliteOrbit = generateSatelliteOrbit();

        const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit);
        // std::cout << accesses << std::endl;

        // Reference data setup

        const File referenceDataFile =
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/AerRanges/Scenario 1.csv")
            );

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        const Duration toleranceDuration = Duration::Seconds(0.5);

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

        for (const auto accessTuple : ostk::core::container::iterator::Zip(referenceData, accesses))
        {
            const auto& referenceRow = std::get<0>(accessTuple);
            const Access& access = std::get<1>(accessTuple);

            const Instant reference_acquisitionOfSignal =
                Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
            // const Instant reference_timeOfClosestApproach =
            // Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
            const Instant reference_lossOfSignal =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);
            const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal());

            EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration))
                << String::Format(
                       "{} ~ {}", reference_acquisitionOfSignal.toString(), access.getAcquisitionOfSignal().toString()
                   );
            // EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach,
            // toleranceDuration))
            // << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(),
            // access.getTimeOfClosestApproach().toString());
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
                << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, AerMask)
{
    {
        // Access computation

        const Environment environment = Environment::Default();

        const ostk::core::container::Map<Real, Real> azimuthElevationMask = {
            {0.0, 30.0}, {90.0, 60.0}, {180.0, 60.0}, {270.0, 30.0}, {359.0, 30.0}
        };
        const ostk::mathematics::object::Interval<Real> rangeRange =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 10000e3);

        const Generator generator = Generator::AerMask(azimuthElevationMask, rangeRange, environment);

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 5, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const auto generateGroundStationTrajectory = []() -> Trajectory
        {
            const LLA groundStationLla = {Angle::Degrees(47.8864), Angle::Degrees(106.906), Length::Meters(10.0)};

            const Position groundStationPosition = Position::Meters(
                groundStationLla.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_),
                Frame::ITRF()
            );

            return Trajectory::Position(groundStationPosition);
        };

        const auto generateSatelliteOrbit = [&environment, &startInstant]() -> Orbit
        {
            const Length semiMajorAxis = Length::Kilometers(6878.14);
            const Real eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(97.5034);
            const Angle raan = Angle::Degrees(0.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(0.0);

            const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const Instant epoch = startInstant;
            const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
            const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
            const Real J2 = Earth::EGM2008.J2_;
            const Real J4 = Earth::EGM2008.J4_;

            const Kepler keplerianModel = {
                coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J2
            };

            const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const Trajectory groundStationTrajectory = generateGroundStationTrajectory();
        const Orbit satelliteOrbit = generateSatelliteOrbit();

        const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit);
        // std::cout << accesses << std::endl;

        // Reference data setup

        const File referenceDataFile =
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/AerMask/Scenario 1.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        const Duration toleranceDuration = Duration::Seconds(0.5);

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

        for (const auto accessTuple : ostk::core::container::iterator::Zip(referenceData, accesses))
        {
            const auto& referenceRow = std::get<0>(accessTuple);
            const Access& access = std::get<1>(accessTuple);

            const Instant reference_acquisitionOfSignal =
                Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
            // const Instant reference_timeOfClosestApproach =
            // Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
            const Instant reference_lossOfSignal =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);
            const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal());

            EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration))
                << String::Format(
                       "{} ~ {}", reference_acquisitionOfSignal.toString(), access.getAcquisitionOfSignal().toString()
                   );
            // EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach,
            // toleranceDuration))
            // << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(),
            // access.getTimeOfClosestApproach().toString());
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
                << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
        }
    }
}
