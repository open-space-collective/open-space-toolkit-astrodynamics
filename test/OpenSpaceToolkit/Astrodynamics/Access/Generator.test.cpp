////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Access/Generator.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Global.test.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Tuple.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/AER.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, Constructor)
{
    using ostk::physics::Environment;
    using ostk::physics::coord::spherical::AER;

    using ostk::astro::Access;
    using ostk::astro::access::Generator;
    using ostk::astro::trajectory::State;

    {
        const Environment environment = Environment::Default();

        EXPECT_NO_THROW(Generator generator(environment););
    }

    {
        const Environment environment = Environment::Default();

        const auto aerFilter = [](const AER&) -> bool {
            return true;
        };

        const auto accessFilter = [](const Access&) -> bool {
            return true;
        };

        EXPECT_NO_THROW(Generator generator(environment, aerFilter, accessFilter););
    }

    {
        const Environment environment = Environment::Default();

        const auto aerFilter = [](const AER&) -> bool {
            return true;
        };

        const auto accessFilter = [](const Access&) -> bool {
            return true;
        };

        const auto stateFilter = [](const State&, const State&) -> bool {
            return true;
        };

        EXPECT_NO_THROW(Generator generator(environment, aerFilter, accessFilter, stateFilter););
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, IsDefined)
{
    using ostk::physics::Environment;
    using ostk::physics::coord::spherical::AER;

    using ostk::astro::Access;
    using ostk::astro::access::Generator;

    {
        const Environment environment = Environment::Default();

        const Generator generator = {environment};

        EXPECT_TRUE(generator.isDefined());
    }

    {
        const Environment environment = Environment::Default();

        const auto aerFilter = [](const AER&) -> bool {
            return true;
        };

        const auto accessFilter = [](const Access&) -> bool {
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
    using ostk::physics::Environment;
    using ostk::physics::time::Duration;

    using ostk::astro::access::Generator;

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
    using ostk::physics::Environment;
    using ostk::physics::time::Duration;

    using ostk::astro::access::Generator;

    {
        const Generator generator = {Environment::Default()};

        EXPECT_EQ(Duration::Microseconds(1.0), generator.getTolerance());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getTolerance());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::ctnr::Tuple;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::coord::spherical::LLA;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::Access;
    using ostk::astro::Trajectory;
    using ostk::astro::access::Generator;
    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::SGP4;
    using ostk::astro::trajectory::orbit::models::kepler::COE;
    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        // Access computation

        const Environment environment = Environment::Default();

        const Generator generator = {environment};

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const auto generateFirstOrbit = [&environment, &startInstant]() -> Orbit {
            const Length semiMajorAxis = Length::Kilometers(7000.0);
            const Real eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(+45.0);
            const Angle raan = Angle::Degrees(0.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(0.0);

            const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const Instant epoch = startInstant;
            const Derived gravitationalParameter = Earth::GravitationalParameter;
            const Length equatorialRadius = Earth::EquatorialRadius;
            const Real J2 = Earth::J2;
            const Real J4 = Earth::J4;

            const Kepler keplerianModel = {coe, epoch, gravitationalParameter,        equatorialRadius,
                                           J2,  J4,    Kepler::PerturbationType::None};

            const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const auto generateSecondOrbit = [&environment, &startInstant]() -> Orbit {
            const Length semiMajorAxis = Length::Kilometers(7000.0);
            const Real eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(+45.0);
            const Angle raan = Angle::Degrees(180.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(180.0);

            const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const Instant epoch = startInstant;
            const Derived gravitationalParameter = Earth::GravitationalParameter;
            const Length equatorialRadius = Earth::EquatorialRadius;
            const Real J2 = Earth::J2;
            const Real J4 = Earth::J4;

            const Kepler keplerianModel = {coe, epoch, gravitationalParameter,        equatorialRadius,
                                           J2,  J4,    Kepler::PerturbationType::None};

            const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const Orbit fromOrbit = generateFirstOrbit();
        const Orbit toOrbit = generateSecondOrbit();

        const Array<Access> accesses = generator.computeAccesses(interval, fromOrbit, toOrbit);

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 1.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        const Duration toleranceDuration = Duration::Seconds(0.1);

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

        for (const auto accessTuple : ostk::core::ctnr::iterators::Zip(referenceData, accesses))
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
                << String::Format("{} ~ {}", reference_acquisitionOfSignal.toString(),
                                  access.getAcquisitionOfSignal().toString());
            EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration))
                << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(),
                                  access.getTimeOfClosestApproach().toString());
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
                << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
        }
    }

    {
        const Environment environment = Environment::Default();

        const Generator generator = {environment};

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const auto generateGroundStationTrajectory = []() -> Trajectory {
            const LLA groundStationLla = {Angle::Degrees(0.0), Angle::Degrees(0.0), Length::Meters(20.0)};

            const Position groundStationPosition = Position::Meters(
                groundStationLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF());

            return Trajectory::Position(groundStationPosition);
        };

        const auto generateSatelliteOrbit = [&environment, &startInstant]() -> Orbit {
            const Length semiMajorAxis = Length::Kilometers(7000.0);
            const Real eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(+45.0);
            const Angle raan = Angle::Degrees(0.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(0.0);

            const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const Instant epoch = startInstant;
            const Derived gravitationalParameter = Earth::GravitationalParameter;
            const Length equatorialRadius = Earth::EquatorialRadius;
            const Real J2 = Earth::J2;
            const Real J4 = Earth::J4;

            const Kepler keplerianModel = {coe, epoch, gravitationalParameter,        equatorialRadius,
                                           J2,  J4,    Kepler::PerturbationType::None};

            const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const Trajectory groundStationTrajectory = generateGroundStationTrajectory();
        const Orbit satelliteOrbit = generateSatelliteOrbit();

        const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit);

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 2.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        const Duration toleranceDuration = Duration::Seconds(0.1);

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

        for (const auto accessTuple : ostk::core::ctnr::iterators::Zip(referenceData, accesses))
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
                << String::Format("{} ~ {}", reference_acquisitionOfSignal.toString(),
                                  access.getAcquisitionOfSignal().toString());
            EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration))
                << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(),
                                  access.getTimeOfClosestApproach().toString());
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
                << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
        }
    }

    {
        const Environment environment = Environment::Default();

        const Generator generator = {environment};

        const Instant startInstant = Instant::DateTime(DateTime(2018, 9, 6, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 9, 7, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const auto generateGroundStationTrajectory = []() -> Trajectory {
            const LLA groundStationLla = {Angle::Degrees(-45.0), Angle::Degrees(-170.0), Length::Meters(5.0)};

            const Position groundStationPosition = Position::Meters(
                groundStationLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF());

            return Trajectory::Position(groundStationPosition);
        };

        const auto generateSatelliteOrbit = [&environment, &startInstant]() -> Orbit {
            const TLE tle = {"1 39419U 13066D   18248.44969859 -.00000394  00000-0 -31796-4 0  9997",
                             "2 39419  97.6313 314.6863 0012643 218.7350 141.2966 14.93878994260975"};

            const SGP4 orbitalModel = {tle};

            const Orbit orbit = {orbitalModel, environment.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const Trajectory groundStationTrajectory = generateGroundStationTrajectory();
        const Orbit satelliteOrbit = generateSatelliteOrbit();

        const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit);

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 3.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        const Duration toleranceDuration = Duration::Seconds(0.1);

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

        for (const auto accessTuple : ostk::core::ctnr::iterators::Zip(referenceData, accesses))
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
                << String::Format("{} ~ {}", reference_acquisitionOfSignal.toString(),
                                  access.getAcquisitionOfSignal().toString());
            EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration))
                << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(),
                                  access.getTimeOfClosestApproach().toString());
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
                << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
        }
    }

    {
        const Instant startInstant = Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2020, 1, 1, 0, 10, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);
        const Duration step = Duration::Minutes(1.0);

        const auto generateTrajectory = [&interval, &step](const Position& aStartPosition,
                                                           const Velocity& aVelocity) -> Trajectory {
            Array<State> states = Array<State>::Empty();

            for (const auto& instant : interval.generateGrid(step))
            {
                const State state = {
                    instant,
                    Position::Meters(aStartPosition.getCoordinates() +
                                         (aVelocity.getCoordinates() * (instant - interval.getStart()).inSeconds()),
                                     Frame::GCRF()),
                    aVelocity};

                states.add(state);
            }

            return Trajectory(states);
        };

        const Trajectory firstTrajectory =
            generateTrajectory(Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()),
                               Velocity::MetersPerSecond({1.0, 0.0, 0.0}, Frame::GCRF()));

        const Trajectory secondTrajectory =
            generateTrajectory(Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()),
                               Velocity::MetersPerSecond({0.0, 0.0, 1.0}, Frame::GCRF()));

        {
            const auto stateFilter = [](const State& aFirstState, const State& aSecondState) -> bool {
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
            const auto stateFilter = [](const State& aFirstState, const State& aSecondState) -> bool {
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
                Instant::DateTime(DateTime(2020, 1, 1, 0, 3, 0), Scale::UTC), toleranceDuration));

            EXPECT_TRUE(accesses.at(1).getAcquisitionOfSignal().isNear(
                Instant::DateTime(DateTime(2020, 1, 1, 0, 7, 0), Scale::UTC), toleranceDuration));
            EXPECT_TRUE(accesses.at(1).getLossOfSignal().isNear(endInstant, toleranceDuration));
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetStep)
{
    using ostk::physics::Environment;
    using ostk::physics::time::Duration;

    using ostk::astro::access::Generator;

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
    using ostk::physics::Environment;
    using ostk::physics::time::Duration;

    using ostk::astro::access::Generator;

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
    using ostk::physics::Environment;
    using ostk::physics::coord::spherical::AER;

    using ostk::astro::access::Generator;

    {
        const Environment environment = Environment::Default();

        Generator generator = {environment};

        const auto aerFilter = [](const AER&) -> bool {
            return true;
        };

        EXPECT_NO_THROW(generator.setAerFilter(aerFilter));

        EXPECT_NO_THROW(generator.setAerFilter({}));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetAccessFilter)
{
    using ostk::physics::Environment;
    using ostk::physics::coord::spherical::AER;

    using ostk::astro::Access;
    using ostk::astro::access::Generator;

    {
        const Environment environment = Environment::Default();

        Generator generator = {environment};

        const auto accessFilter = [](const Access&) -> bool {
            return true;
        };

        EXPECT_NO_THROW(generator.setAccessFilter(accessFilter));

        EXPECT_NO_THROW(generator.setAccessFilter({}));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetStateFilter)
{
    using ostk::physics::Environment;
    using ostk::physics::coord::spherical::AER;

    using ostk::astro::Access;
    using ostk::astro::access::Generator;
    using ostk::astro::trajectory::State;

    {
        const Environment environment = Environment::Default();

        Generator generator = {environment};

        const auto stateFilter = [](const State&, const State&) -> bool {
            return true;
        };

        EXPECT_NO_THROW(generator.setStateFilter(stateFilter));

        EXPECT_NO_THROW(generator.setStateFilter({}));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, Undefined)
{
    using ostk::astro::access::Generator;

    {
        EXPECT_NO_THROW(Generator::Undefined());

        EXPECT_FALSE(Generator::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, AerRanges)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::spherical::LLA;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::Access;
    using ostk::astro::Trajectory;
    using ostk::astro::access::Generator;
    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Access computation

        const Environment environment = Environment::Default();

        const ostk::math::obj::Interval<Real> azimuthRange = ostk::math::obj::Interval<Real>::Closed(0.0, 360.0);
        const ostk::math::obj::Interval<Real> elevationRange = ostk::math::obj::Interval<Real>::Closed(60.0, 90.0);
        const ostk::math::obj::Interval<Real> rangeRange = ostk::math::obj::Interval<Real>::Closed(0.0, 10000e3);

        const Generator generator = Generator::AerRanges(azimuthRange, elevationRange, rangeRange, environment);

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 10, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const auto generateGroundStationTrajectory = []() -> Trajectory {
            const LLA groundStationLla = {Angle::Degrees(47.8864), Angle::Degrees(106.906), Length::Meters(10.0)};

            const Position groundStationPosition = Position::Meters(
                groundStationLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF());

            return Trajectory::Position(groundStationPosition);
        };

        const auto generateSatelliteOrbit = [&environment, &startInstant]() -> Orbit {
            const Length semiMajorAxis = Length::Kilometers(6878.14);
            const Real eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(97.5034);
            const Angle raan = Angle::Degrees(0.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(0.0);

            const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const Instant epoch = startInstant;
            const Derived gravitationalParameter = Earth::GravitationalParameter;
            const Length equatorialRadius = Earth::EquatorialRadius;
            const Real J2 = Earth::J2;
            const Real J4 = Earth::J4;

            const Kepler keplerianModel = {coe, epoch, gravitationalParameter,      equatorialRadius,
                                           J2,  J4,    Kepler::PerturbationType::J2};

            const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const Trajectory groundStationTrajectory = generateGroundStationTrajectory();
        const Orbit satelliteOrbit = generateSatelliteOrbit();

        const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit);
        // std::cout << accesses << std::endl ;

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/AerRanges/Scenario 1.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        const Duration toleranceDuration = Duration::Seconds(0.5);

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

        for (const auto accessTuple : ostk::core::ctnr::iterators::Zip(referenceData, accesses))
        {
            const auto& referenceRow = std::get<0>(accessTuple);
            const Access& access = std::get<1>(accessTuple);

            const Instant reference_acquisitionOfSignal =
                Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
            // const Instant reference_timeOfClosestApproach =
            // Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant reference_lossOfSignal =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);
            const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal());

            EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_acquisitionOfSignal.toString(),
                                  access.getAcquisitionOfSignal().toString());
            // EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration))
            // << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(),
            // access.getTimeOfClosestApproach().toString()) ;
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
                << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Access_Generator, AerMask)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::spherical::LLA;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::Access;
    using ostk::astro::Trajectory;
    using ostk::astro::access::Generator;
    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Access computation

        const Environment environment = Environment::Default();

        const ostk::core::ctnr::Map<Real, Real> azimuthElevationMask = {
            {0.0, 30.0}, {90.0, 60.0}, {180.0, 60.0}, {270.0, 30.0}, {359.0, 30.0}};
        const ostk::math::obj::Interval<Real> rangeRange = ostk::math::obj::Interval<Real>::Closed(0.0, 10000e3);

        const Generator generator = Generator::AerMask(azimuthElevationMask, rangeRange, environment);

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 5, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const auto generateGroundStationTrajectory = []() -> Trajectory {
            const LLA groundStationLla = {Angle::Degrees(47.8864), Angle::Degrees(106.906), Length::Meters(10.0)};

            const Position groundStationPosition = Position::Meters(
                groundStationLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF());

            return Trajectory::Position(groundStationPosition);
        };

        const auto generateSatelliteOrbit = [&environment, &startInstant]() -> Orbit {
            const Length semiMajorAxis = Length::Kilometers(6878.14);
            const Real eccentricity = 0.0;
            const Angle inclination = Angle::Degrees(97.5034);
            const Angle raan = Angle::Degrees(0.0);
            const Angle aop = Angle::Degrees(0.0);
            const Angle trueAnomaly = Angle::Degrees(0.0);

            const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

            const Instant epoch = startInstant;
            const Derived gravitationalParameter = Earth::GravitationalParameter;
            const Length equatorialRadius = Earth::EquatorialRadius;
            const Real J2 = Earth::J2;
            const Real J4 = Earth::J4;

            const Kepler keplerianModel = {coe, epoch, gravitationalParameter,      equatorialRadius,
                                           J2,  J4,    Kepler::PerturbationType::J2};

            const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const Trajectory groundStationTrajectory = generateGroundStationTrajectory();
        const Orbit satelliteOrbit = generateSatelliteOrbit();

        const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit);
        // std::cout << accesses << std::endl ;

        // Reference data setup

        const File referenceDataFile =
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Access/Generator/AerMask/Scenario 1.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        const Duration toleranceDuration = Duration::Seconds(0.5);

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize());

        for (const auto accessTuple : ostk::core::ctnr::iterators::Zip(referenceData, accesses))
        {
            const auto& referenceRow = std::get<0>(accessTuple);
            const Access& access = std::get<1>(accessTuple);

            const Instant reference_acquisitionOfSignal =
                Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
            // const Instant reference_timeOfClosestApproach =
            // Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant reference_lossOfSignal =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);
            const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal());

            EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_acquisitionOfSignal.toString(),
                                  access.getAcquisitionOfSignal().toString());
            // EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration))
            // << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(),
            // access.getTimeOfClosestApproach().toString()) ;
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration))
                << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString());
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration))
                << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
