////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>

#include <Library/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <Library/Physics/Time/DateTime.hpp>
#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Duration.hpp>
#include <Library/Physics/Time/Instant.hpp>
#include <Library/Physics/Time/Scale.hpp>
#include <Library/Physics/Units/Derived/Angle.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Length.hpp>

#include <Library/Mathematics/Objects/Vector.hpp>

#include <Library/Core/Containers/Table.hpp>
#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Integer.hpp>
#include <Library/Core/Types/Shared.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Trajectory_Orbit, Constructor)
{

    using library::core::types::Shared ;
    using library::core::types::Integer ;
    using library::core::types::Real ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel } ;

        EXPECT_TRUE(orbit.isDefined()) ;

    }

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
{ Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;
        
        const Integer initialRevolutionNumber = 123 ;

        const Orbit orbit = { states, initialRevolutionNumber } ;

        EXPECT_TRUE(orbit.isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit, EqualToOperator)
{

    using library::core::types::Shared ;
    using library::core::types::Integer ;
    using library::core::types::Real ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel } ;

        EXPECT_TRUE(orbit == orbit) ;
        
        EXPECT_FALSE(orbit == Orbit::Undefined()) ;
        EXPECT_FALSE(Orbit::Undefined() == orbit) ;

    }

    {

        EXPECT_FALSE(Orbit::Undefined() == Orbit::Undefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit, NotEqualToOperator)
{

    using library::core::types::Shared ;
    using library::core::types::Integer ;
    using library::core::types::Real ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel } ;

        EXPECT_FALSE(orbit != orbit) ;
        
        EXPECT_TRUE(orbit != Orbit::Undefined()) ;
        EXPECT_TRUE(Orbit::Undefined() != orbit) ;

    }

    {

        EXPECT_TRUE(Orbit::Undefined() != Orbit::Undefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit, IsDefined)
{

    using library::core::types::Shared ;
    using library::core::types::Real ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel } ;

        EXPECT_TRUE(orbit.isDefined()) ;

    }

    {

        EXPECT_FALSE(Orbit::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit, GetRevolutionNumberAt)
{

    using library::core::types::Shared ;
    using library::core::types::Integer ;
    using library::core::types::Real ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::State ;
    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::orbit::Pass ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    {

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel } ;

        // Reference data setup

        const Table referenceData = Table::Load(File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Passes.csv")), Table::Format::CSV, true) ;

        // Pass test

        for (const auto& referenceRow : referenceData)
        {

            const Integer referenceRevolutionNumber = referenceRow[0].accessInteger() ;
            const Instant referencePassStartInstant = Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant referencePassEndInstant = Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC) ;

            const Instant instant = referencePassStartInstant + Duration::Between(referencePassStartInstant, referencePassEndInstant) / 2.0 ;

            EXPECT_EQ(referenceRevolutionNumber, orbit.getRevolutionNumberAt(instant)) ;

        }

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit, GetPassAt)
{

    using library::core::types::Shared ;
    using library::core::types::Integer ;
    using library::core::types::Real ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::State ;
    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::orbit::Pass ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    {

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel } ;

        // Reference data setup

        const Table referenceData = Table::Load(File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Passes.csv")), Table::Format::CSV, true) ;

        // Pass test

        for (const auto& referenceRow : referenceData)
        {

            const Instant referencePassStartInstant = Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant referencePassEndInstant = Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC) ;

            const Instant instant = referencePassStartInstant + Duration::Between(referencePassStartInstant, referencePassEndInstant) / 2.0 ;

            const Pass pass = orbit.getPassAt(instant) ;

            EXPECT_TRUE(pass.isDefined()) ;

            EXPECT_GT(Duration::Microseconds(1.0), Duration::Between(referencePassStartInstant, pass.getInterval().getStart()).getAbsolute()) ;
            EXPECT_GT(Duration::Microseconds(1.0), Duration::Between(referencePassEndInstant, pass.getInterval().getEnd()).getAbsolute()) ;

        }

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit, GetPassWithRevolutionNumber)
{

    using library::core::types::Shared ;
    using library::core::types::Integer ;
    using library::core::types::Real ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::State ;
    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::orbit::Pass ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    {

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel } ;

        // Reference data setup

        const Table referenceData = Table::Load(File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Passes.csv")), Table::Format::CSV, true) ;

        // Pass test

        for (const auto& referenceRow : referenceData)
        {

            const Integer referenceRevolutionNumber = referenceRow[0].accessInteger() ;
            const Instant referencePassStartInstant = Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant referencePassEndInstant = Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC) ;

            const Pass pass = orbit.getPassWithRevolutionNumber(referenceRevolutionNumber) ;

            EXPECT_TRUE(pass.isDefined()) ;
            
            EXPECT_EQ(Pass::Type::Complete, pass.getType()) ;

            EXPECT_GT(Duration::Microseconds(1.0), Duration::Between(referencePassStartInstant, pass.getInterval().getStart()).getAbsolute()) ;
            EXPECT_GT(Duration::Microseconds(1.0), Duration::Between(referencePassEndInstant, pass.getInterval().getEnd()).getAbsolute()) ;

        }

    }

    {

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::J2 } ;

        const Orbit orbit = { keplerianModel } ;

        // Reference data setup

        const Table referenceData = Table::Load(File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_2/Satellite Passes.csv")), Table::Format::CSV, true) ;

        // Pass test

        for (const auto& referenceRow : referenceData)
        {

            const Integer referenceRevolutionNumber = referenceRow[0].accessInteger() ;
            const Instant referencePassStartInstant = Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC) ;
            const Instant referencePassEndInstant = Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC) ;

            const Pass pass = orbit.getPassWithRevolutionNumber(referenceRevolutionNumber) ;

            EXPECT_TRUE(pass.isDefined()) ;
            
            EXPECT_EQ(Pass::Type::Complete, pass.getType()) ;

            EXPECT_GT(Duration::Milliseconds(1.0), Duration::Between(referencePassStartInstant, pass.getInterval().getStart()).getAbsolute()) ;
            EXPECT_GT(Duration::Milliseconds(1.0), Duration::Between(referencePassEndInstant, pass.getInterval().getEnd()).getAbsolute()) ;

        }

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit, Print)
{

    using library::core::types::Shared ;
    using library::core::types::Integer ;
    using library::core::types::Real ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;

    {

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(orbit.print(std::cout, true)) ;
        EXPECT_NO_THROW(orbit.print(std::cout, false)) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory_Orbit, Undefined)
{

    using library::astro::trajectory::Orbit ;

    {

        EXPECT_NO_THROW(Orbit::Undefined()) ;
        EXPECT_FALSE(Orbit::Undefined().isDefined()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////