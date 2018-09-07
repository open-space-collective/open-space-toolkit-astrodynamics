////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Access/Generator.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Access/Generator.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>

#include <Library/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <Library/Physics/Coordinate/Spherical/AER.hpp>
#include <Library/Physics/Coordinate/Spherical/LLA.hpp>

#include <Library/Core/FileSystem/File.hpp>
#include <Library/Core/FileSystem/Path.hpp>
#include <Library/Core/Containers/Table.hpp>
#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Containers/Tuple.hpp>
#include <Library/Core/Types/Real.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Access_Generator, Constructor)
{

    using library::physics::coord::spherical::AER ;
    using library::physics::Environment ;

    using library::astro::Access ;
    using library::astro::access::Generator ;

    {

        const Environment environment = Environment::Default() ;

        EXPECT_NO_THROW(Generator generator(environment) ;) ;

    }

    {

        const Environment environment = Environment::Default() ;

        const auto aerFilter = [] (const AER&) -> bool
        {
            return true ;
        } ;

        const auto accessFilter = [] (const Access&) -> bool
        {
            return true ;
        } ;

        EXPECT_NO_THROW(Generator generator(environment, aerFilter, accessFilter) ;) ;
        
    }

}

TEST (Library_Astrodynamics_Access_Generator, IsDefined)
{

    using library::physics::coord::spherical::AER ;
    using library::physics::Environment ;

    using library::astro::Access ;
    using library::astro::access::Generator ;

    {

        const Environment environment = Environment::Default() ;

        const Generator generator = { environment } ;

        EXPECT_TRUE(generator.isDefined()) ;

    }

    {

        const Environment environment = Environment::Default() ;

        const auto aerFilter = [] (const AER&) -> bool
        {
            return true ;
        } ;

        const auto accessFilter = [] (const Access&) -> bool
        {
            return true ;
        } ;

        const Generator generator = { environment, aerFilter, accessFilter } ;

        EXPECT_TRUE(generator.isDefined()) ;
        
    }

    {

        EXPECT_FALSE(Generator::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Access_Generator, ComputeAccesses)
{

    using library::core::types::Real ;
    using library::core::types::String ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Tuple ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::DateTime ;
    using library::physics::time::Interval ;
    using library::physics::coord::spherical::LLA ;
    using library::physics::coord::Position ;
    using library::physics::coord::Frame ;
    using library::physics::Environment ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;
    using library::astro::trajectory::orbit::models::SGP4 ;
    using library::astro::trajectory::orbit::models::sgp4::TLE ;
    using library::astro::Access ;
    using library::astro::access::Generator ;

    {

        // Access computation

        const Environment environment = Environment::Default() ;

        const Generator generator = { environment } ;

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC) ;

        const Interval interval = Interval::Closed(startInstant, endInstant) ;

        auto generateFirstOrbit = [&environment, &startInstant] () -> Orbit
        {

            const Length semiMajorAxis = Length::Kilometers(7000.0) ;
            const Real eccentricity = 0.0 ;
            const Angle inclination = Angle::Degrees(+45.0) ;
            const Angle raan = Angle::Degrees(0.0) ;
            const Angle aop = Angle::Degrees(0.0) ;
            const Angle trueAnomaly = Angle::Degrees(0.0) ;

            const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

            const Instant epoch = startInstant ;
            const Derived gravitationalConstant = Earth::GravitationalConstant ;
            const Length equatorialRadius = Earth::EquatorialRadius ;
            const Real J2 = Earth::J2 ;

            const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

            const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

            return orbit ;

        } ;

        auto generateSecondOrbit = [&environment, &startInstant] () -> Orbit
        {

            const Length semiMajorAxis = Length::Kilometers(7000.0) ;
            const Real eccentricity = 0.0 ;
            const Angle inclination = Angle::Degrees(+45.0) ;
            const Angle raan = Angle::Degrees(180.0) ;
            const Angle aop = Angle::Degrees(0.0) ;
            const Angle trueAnomaly = Angle::Degrees(180.0) ;

            const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

            const Instant epoch = startInstant ;
            const Derived gravitationalConstant = Earth::GravitationalConstant ;
            const Length equatorialRadius = Earth::EquatorialRadius ;
            const Real J2 = Earth::J2 ;

            const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

            const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

            return orbit ;

        } ;

        const Orbit fromOrbit = generateFirstOrbit() ;
        const Orbit toOrbit = generateSecondOrbit() ;

        const Array<Access> accesses = generator.computeAccesses(interval, fromOrbit, toOrbit) ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("../test/Library/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 1.csv")) ;

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

        const Duration toleranceDuration = Duration::Seconds(0.1) ;

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize()) ;

        for (const auto accessTuple : library::core::ctnr::iterators::Zip(referenceData, accesses))
        {

            const auto& referenceRow = std::get<0>(accessTuple) ;
            const Access& access = std::get<1>(accessTuple) ;

            const Instant reference_acquisitionOfSignal = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;
            const Instant reference_timeOfClosestApproach = Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant reference_lossOfSignal = Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC) ;
            const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal()) ;

            EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration)) << String::Format("{} ~ {}", reference_acquisitionOfSignal.toString(), access.getAcquisitionOfSignal().toString()) ;
            EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration)) << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(), access.getTimeOfClosestApproach().toString()) ;
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration)) << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString()) ;
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration)) << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString()) ;

        }

    }

    {

        const Environment environment = Environment::Default() ;

        const Generator generator = { environment } ;

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC) ;

        const Interval interval = Interval::Closed(startInstant, endInstant) ;

        auto generateGroundStationTrajectory = [] () -> Trajectory
        {

            const LLA groundStationLla = { Angle::Degrees(0.0), Angle::Degrees(0.0), Length::Meters(20.0) } ;

            const Position groundStationPosition = Position::Meters(groundStationLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF()) ;

            return Trajectory::Position(groundStationPosition) ;

        } ;

        auto generateSatelliteOrbit = [&environment, &startInstant] () -> Orbit
        {

            const Length semiMajorAxis = Length::Kilometers(7000.0) ;
            const Real eccentricity = 0.0 ;
            const Angle inclination = Angle::Degrees(+45.0) ;
            const Angle raan = Angle::Degrees(0.0) ;
            const Angle aop = Angle::Degrees(0.0) ;
            const Angle trueAnomaly = Angle::Degrees(0.0) ;

            const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

            const Instant epoch = startInstant ;
            const Derived gravitationalConstant = Earth::GravitationalConstant ;
            const Length equatorialRadius = Earth::EquatorialRadius ;
            const Real J2 = Earth::J2 ;

            const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

            const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

            return orbit ;

        } ;

        const Trajectory groundStationTrajectory = generateGroundStationTrajectory() ;
        const Orbit satelliteOrbit = generateSatelliteOrbit() ;

        const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit) ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("../test/Library/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 2.csv")) ;

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

        const Duration toleranceDuration = Duration::Seconds(0.1) ;

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize()) ;

        for (const auto accessTuple : library::core::ctnr::iterators::Zip(referenceData, accesses))
        {

            const auto& referenceRow = std::get<0>(accessTuple) ;
            const Access& access = std::get<1>(accessTuple) ;

            const Instant reference_acquisitionOfSignal = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;
            const Instant reference_timeOfClosestApproach = Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant reference_lossOfSignal = Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC) ;
            const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal()) ;

            EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration)) << String::Format("{} ~ {}", reference_acquisitionOfSignal.toString(), access.getAcquisitionOfSignal().toString()) ;
            EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration)) << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(), access.getTimeOfClosestApproach().toString()) ;
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration)) << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString()) ;
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration)) << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString()) ;

        }

    }

    {

        const Environment environment = Environment::Default() ;

        const Generator generator = { environment } ;

        const Instant startInstant = Instant::DateTime(DateTime(2018, 9, 6, 0, 0, 0), Scale::UTC) ;
        const Instant endInstant = Instant::DateTime(DateTime(2018, 9, 7, 0, 0, 0), Scale::UTC) ;

        const Interval interval = Interval::Closed(startInstant, endInstant) ;

        auto generateGroundStationTrajectory = [] () -> Trajectory
        {

            const LLA groundStationLla = { Angle::Degrees(-45.0), Angle::Degrees(-170.0), Length::Meters(5.0) } ;

            const Position groundStationPosition = Position::Meters(groundStationLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF()) ;

            return Trajectory::Position(groundStationPosition) ;

        } ;

        auto generateSatelliteOrbit = [&environment, &startInstant] () -> Orbit
        {

            const TLE tle =
            {
                "1 39419U 13066D   18248.44969859 -.00000394  00000-0 -31796-4 0  9997",
                "2 39419  97.6313 314.6863 0012643 218.7350 141.2966 14.93878994260975"
            } ;

            const SGP4 orbitalModel = { tle } ;

            const Orbit orbit = { orbitalModel, environment.accessCelestialObjectWithName("Earth") } ;

            return orbit ;

        } ;

        const Trajectory groundStationTrajectory = generateGroundStationTrajectory() ;
        const Orbit satelliteOrbit = generateSatelliteOrbit() ;

        const Array<Access> accesses = generator.computeAccesses(interval, groundStationTrajectory, satelliteOrbit) ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("../test/Library/Astrodynamics/Access/Generator/ComputeAccesses/Scenario 3.csv")) ;

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

        const Duration toleranceDuration = Duration::Seconds(0.1) ;

        // Test

        ASSERT_EQ(referenceData.getRowCount(), accesses.getSize()) ;

        for (const auto accessTuple : library::core::ctnr::iterators::Zip(referenceData, accesses))
        {

            const auto& referenceRow = std::get<0>(accessTuple) ;
            const Access& access = std::get<1>(accessTuple) ;

            const Instant reference_acquisitionOfSignal = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;
            const Instant reference_timeOfClosestApproach = Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant reference_lossOfSignal = Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC) ;
            const Duration reference_duration = Duration::Seconds(referenceRow[3].accessReal()) ;

            EXPECT_TRUE(access.getAcquisitionOfSignal().isNear(reference_acquisitionOfSignal, toleranceDuration)) << String::Format("{} ~ {}", reference_acquisitionOfSignal.toString(), access.getAcquisitionOfSignal().toString()) ;
            EXPECT_TRUE(access.getTimeOfClosestApproach().isNear(reference_timeOfClosestApproach, toleranceDuration)) << String::Format("{} ~ {}", reference_timeOfClosestApproach.toString(), access.getTimeOfClosestApproach().toString()) ;
            EXPECT_TRUE(access.getLossOfSignal().isNear(reference_lossOfSignal, toleranceDuration)) << String::Format("{} ~ {}", reference_lossOfSignal.toString(), access.getLossOfSignal().toString()) ;
            EXPECT_TRUE(access.getDuration().isNear(reference_duration, toleranceDuration)) << String::Format("{} ~ {}", reference_duration.toString(), access.getDuration().toString()) ;

        }

    }

}

TEST (Library_Astrodynamics_Access_Generator, SetStep)
{

    using library::physics::time::Duration ;
    using library::physics::Environment ;

    using library::astro::access::Generator ;

    {

        const Environment environment = Environment::Default() ;

        Generator generator = { environment } ;

        const Duration step = Duration::Seconds(1.0) ;

        EXPECT_NO_THROW(generator.setStep(step)) ;

        EXPECT_ANY_THROW(generator.setStep(Duration::Undefined())) ;

    }

}

TEST (Library_Astrodynamics_Access_Generator, SetTolerance)
{

    using library::physics::time::Duration ;
    using library::physics::Environment ;

    using library::astro::access::Generator ;

    {

        const Environment environment = Environment::Default() ;

        Generator generator = { environment } ;

        const Duration tolerance = Duration::Seconds(1.0) ;

        EXPECT_NO_THROW(generator.setTolerance(tolerance)) ;

        EXPECT_ANY_THROW(generator.setTolerance(Duration::Undefined())) ;

    }

}

TEST (Library_Astrodynamics_Access_Generator, SetAerFilter)
{

    using library::physics::coord::spherical::AER ;
    using library::physics::Environment ;

    using library::astro::access::Generator ;

    {

        const Environment environment = Environment::Default() ;

        Generator generator = { environment } ;

        const auto aerFilter = [] (const AER&) -> bool
        {
            return true ;
        } ;

        EXPECT_NO_THROW(generator.setAerFilter(aerFilter)) ;

        EXPECT_NO_THROW(generator.setAerFilter({})) ;

    }

}

TEST (Library_Astrodynamics_Access_Generator, SetAccessFilter)
{

    using library::physics::coord::spherical::AER ;
    using library::physics::Environment ;

    using library::astro::Access ;
    using library::astro::access::Generator ;

    {

        const Environment environment = Environment::Default() ;

        Generator generator = { environment } ;

        const auto accessFilter = [] (const Access&) -> bool
        {
            return true ;
        } ;

        EXPECT_NO_THROW(generator.setAccessFilter(accessFilter)) ;

        EXPECT_NO_THROW(generator.setAccessFilter({})) ;

    }

}

TEST (Library_Astrodynamics_Access_Generator, Undefined)
{

    using library::astro::access::Generator ;

    {

        EXPECT_NO_THROW(Generator::Undefined()) ;

        EXPECT_FALSE(Generator::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Access_Generator, AerRanges)
{

    using library::math::obj::Interval ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::Environment ;

    using library::astro::access::Generator ;

    {

        // const Environment environment = Environment::Default() ;

        // const Interval<Angle> azimuthRange = Interval<Angle>::Closed(Angle::Degrees(30.0), Angle::Degrees(240.0)) ;
        // const Interval<Angle> elevationRange = Interval<Angle>::Closed(Angle::Degrees(10.0), Angle::Degrees(80.0)) ;
        // const Interval<Length> rangeRange = Interval<Length>::Closed(Length::Meters(300e3), Length::Meters(800e3)) ;

        // EXPECT_NO_THROW(Generator::AerRanges(azimuthRange, elevationRange, rangeRange, environment)) ;

        // const Generator generator = Generator::AerRanges(azimuthRange, elevationRange, rangeRange, environment) ;

        // EXPECT_TRUE(generator.isDefined()) ;

        FAIL() ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////