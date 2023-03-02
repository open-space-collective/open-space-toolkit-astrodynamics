////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Global.test.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Constructor)
// {

//     using ostk::core::types::Shared ;
//     using ostk::core::types::Real ;
//     using ostk::core::ctnr::Array ;
//     using ostk::core::ctnr::Table ;
//     using ostk::core::fs::Path ;
//     using ostk::core::fs::File ;

//     using ostk::math::obj::Vector3d ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Scale ;
//     using ostk::physics::time::Instant ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::time::Interval ;
//     using ostk::physics::time::DateTime ;
//     using ostk::physics::coord::Frame ;
//     using ostk::physics::coord::Position ;
//     using ostk::physics::coord::Velocity ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::Orbit ;
//     using ostk::astro::trajectory::State ;
//     using ostk::astro::trajectory::orbit::models::Kepler ;
//     using ostk::astro::trajectory::orbit::models::kepler::COE ;

//     // {

//     //     const Earth earth = Earth::Analytical() ;

//     //     const Length semiMajorAxis = Length::Kilometers(7000.0) ;
//     //     const Real eccentricity = 0.0 ;
//     //     const Angle inclination = Angle::Degrees(0.0) ;
//     //     const Angle raan = Angle::Degrees(0.0) ;
//     //     const Angle aop = Angle::Degrees(0.0) ;
//     //     const Angle trueAnomaly = Angle::Degrees(0.0) ;

//     //     const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//     //     std::cout << coe << std::endl ;

//     //     const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;

//     //     const Kepler keplerianModel = { coe, epoch, earth, Kepler::PerturbationType::None } ;

//     //     std::cout << keplerianModel.getClassicalOrbitalElements() << std::endl ;

//     // }

//     {

//         const Earth earth = Earth::Analytical() ;

//         const Length semiMajorAxis = Length::Kilometers(7000.0) ;
//         const Real eccentricity = 0.0 ;
//         const Angle inclination = Angle::Degrees(0.0) ;
//         const Angle raan = Angle::Degrees(0.0) ;
//         const Angle aop = Angle::Degrees(0.0) ;
//         const Angle trueAnomaly = Angle::Degrees(0.0) ;

//         const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//         std::cout << coe << std::endl ;

//         const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC) ;

//         const Kepler keplerianModel = { coe, epoch, earth, Kepler::PerturbationType::None, true } ;

//         std::cout << keplerianModel.getClassicalOrbitalElements() << std::endl ;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, EqualToOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, NotEqualToOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, StreamOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, IsDefined)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetClassicalOrbitalElements)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetEpoch)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetRevolutionNumberAtEpoch)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetGravitationalParameter)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetEquatorialRadius)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetJ2)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetJ4)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, GetPerturbationType)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, CalculateStateAt)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, CalculateRevolutionNumberAt)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, StringFromPerturbationType)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Test_1)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;

    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC);
        const Derived gravitationalParameter = Earth::Models::EGM2008::GravitationalParameter;
        const Length equatorialRadius = Earth::Models::EGM2008::EquatorialRadius;
        const Real J2 = Earth::Models::EGM2008::J2;
        const Real J4 = Earth::Models::EGM2008::J4;

        const Kepler keplerianModel = {coe, epoch, gravitationalParameter,        equatorialRadius,
                                       J2,  J4,    Kepler::PerturbationType::None};

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Orbit.csv")),
            Table::Format::CSV, true);

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {referenceRow[1].accessReal(), referenceRow[2].accessReal(),
                                                     referenceRow[3].accessReal()};
            const Vector3d referenceVelocity_GCRF = {referenceRow[4].accessReal(), referenceRow[5].accessReal(),
                                                     referenceRow[6].accessReal()};

            const Vector3d referencePosition_ITRF = {referenceRow[7].accessReal(), referenceRow[8].accessReal(),
                                                     referenceRow[9].accessReal()};
            const Vector3d referenceVelocity_ITRF = {referenceRow[10].accessReal(), referenceRow[11].accessReal(),
                                                     referenceRow[12].accessReal()};

            const Real referenceRevolutionNumber = referenceRow[13].accessReal();

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.accessPosition();
            const Velocity velocity_GCRF = state_GCRF.accessVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1e-3, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(1e-6, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.accessPosition();
            const Velocity velocity_ITRF = state_ITRF.accessVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(1e-1, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(1e-0, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            ASSERT_EQ(referenceRevolutionNumber.floor(), orbit.getRevolutionNumberAt(instant));

            // std::cout << "x @ GCRF = " << referencePosition_GCRF.toString(10) << " / " <<
            // position_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "x @ ITRF = " <<
            // referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dx = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()
            // << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl ;

            // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " <<
            // velocity_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "v @ ITRF = " <<
            // referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()
            // << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl ;

            // std::cout << "dx | dv = " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << Real((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl ;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Test_2)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;

    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::Models::EGM96::GravitationalParameter;
        const Length equatorialRadius = Earth::Models::EGM96::EquatorialRadius;
        const Real J2 = Earth::Models::EGM96::J2;
        const Real J4 = Earth::Models::EGM96::J4;

        const Kepler keplerianModel = {coe, epoch, gravitationalParameter,      equatorialRadius,
                                       J2,  J4,    Kepler::PerturbationType::J2};

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_2/Satellite Orbit.csv")),
            Table::Format::CSV, true);

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {referenceRow[1].accessReal(), referenceRow[2].accessReal(),
                                                     referenceRow[3].accessReal()};
            const Vector3d referenceVelocity_GCRF = {referenceRow[4].accessReal(), referenceRow[5].accessReal(),
                                                     referenceRow[6].accessReal()};

            const Vector3d referencePosition_ITRF = {referenceRow[7].accessReal(), referenceRow[8].accessReal(),
                                                     referenceRow[9].accessReal()};
            const Vector3d referenceVelocity_ITRF = {referenceRow[10].accessReal(), referenceRow[11].accessReal(),
                                                     referenceRow[12].accessReal()};

            const Real referenceRevolutionNumber = referenceRow[13].accessReal();

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.accessPosition();
            const Velocity velocity_GCRF = state_GCRF.accessVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(1e-3, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.accessPosition();
            const Velocity velocity_ITRF = state_ITRF.accessVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(1e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(1e-3, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            ASSERT_EQ(referenceRevolutionNumber.floor(), orbit.getRevolutionNumberAt(instant));

            if (referenceRevolutionNumber.floor() != orbit.getRevolutionNumberAt(instant))
            {
                std::cout << "instant = " << instant.toString() << std::endl;

                FAIL();
            }

            // std::cout << "x @ GCRF = " << referencePosition_GCRF.toString(10) << " / " <<
            // position_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "x @ ITRF = " <<
            // referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dx = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()
            // << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl ;

            // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " <<
            // velocity_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "v @ ITRF = " <<
            // referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()
            // << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl ;

            // std::cout << "dx | dv = " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << Real((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl ;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Test_3)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;

    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 7.3226415931607501e-03;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::Models::EGM96::GravitationalParameter;
        const Length equatorialRadius = Earth::Models::EGM96::EquatorialRadius;
        const Real J2 = Earth::Models::EGM96::J2;
        const Real J4 = Earth::Models::EGM96::J4;

        const Kepler keplerianModel = {coe, epoch, gravitationalParameter,      equatorialRadius,
                                       J2,  J4,    Kepler::PerturbationType::J2};

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_3/Satellite Orbit.csv")),
            Table::Format::CSV, true);

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {referenceRow[1].accessReal(), referenceRow[2].accessReal(),
                                                     referenceRow[3].accessReal()};
            const Vector3d referenceVelocity_GCRF = {referenceRow[4].accessReal(), referenceRow[5].accessReal(),
                                                     referenceRow[6].accessReal()};

            const Vector3d referencePosition_ITRF = {referenceRow[7].accessReal(), referenceRow[8].accessReal(),
                                                     referenceRow[9].accessReal()};
            const Vector3d referenceVelocity_ITRF = {referenceRow[10].accessReal(), referenceRow[11].accessReal(),
                                                     referenceRow[12].accessReal()};

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.accessPosition();
            const Velocity velocity_GCRF = state_GCRF.accessVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(5e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(1e-2, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.accessPosition();
            const Velocity velocity_ITRF = state_ITRF.accessVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(5e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(1e-2, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            // std::cout << "x @ GCRF = " << referencePosition_GCRF.toString(10) << " / " <<
            // position_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "x @ ITRF = " <<
            // referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dx = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()
            // << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl ;

            // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " <<
            // velocity_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "v @ ITRF = " <<
            // referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()
            // << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl ;

            // std::cout << "dx | dv = " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << Real((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl ;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Test_4)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;

    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::Models::EGM96::GravitationalParameter;
        const Length equatorialRadius = Earth::Models::EGM96::EquatorialRadius;
        const Real J2 = Earth::Models::EGM96::J2;
        const Real J4 = Earth::Models::EGM96::J4;

        const Kepler keplerianModel = {coe, epoch, gravitationalParameter,      equatorialRadius,
                                       J2,  J4,    Kepler::PerturbationType::J4};

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_4/Satellite Orbit.csv")),
            Table::Format::CSV, true);

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {referenceRow[1].accessReal(), referenceRow[2].accessReal(),
                                                     referenceRow[3].accessReal()};
            const Vector3d referenceVelocity_GCRF = {referenceRow[4].accessReal(), referenceRow[5].accessReal(),
                                                     referenceRow[6].accessReal()};

            const Vector3d referencePosition_ITRF = {referenceRow[7].accessReal(), referenceRow[8].accessReal(),
                                                     referenceRow[9].accessReal()};
            const Vector3d referenceVelocity_ITRF = {referenceRow[10].accessReal(), referenceRow[11].accessReal(),
                                                     referenceRow[12].accessReal()};

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.accessPosition();
            const Velocity velocity_GCRF = state_GCRF.accessVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(1e-3, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.accessPosition();
            const Velocity velocity_ITRF = state_ITRF.accessVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(1e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(1e-3, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            // std::cout << "x @ GCRF = " << referencePosition_GCRF.toString(10) << " / " <<
            // position_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "x @ ITRF = " <<
            // referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dx = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()
            // << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl ;

            // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " <<
            // velocity_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "v @ ITRF = " <<
            // referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()
            // << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl ;

            // std::cout << "dx | dv = " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << Real((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl ;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Test_5)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;

    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 7.3226415931607501e-03;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::Models::EGM96::GravitationalParameter;
        const Length equatorialRadius = Earth::Models::EGM96::EquatorialRadius;
        const Real J2 = Earth::Models::EGM96::J2;
        const Real J4 = Earth::Models::EGM96::J4;

        const Kepler keplerianModel = {coe, epoch, gravitationalParameter,      equatorialRadius,
                                       J2,  J4,    Kepler::PerturbationType::J4};

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_5/Satellite Orbit.csv")),
            Table::Format::CSV, true);

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {referenceRow[1].accessReal(), referenceRow[2].accessReal(),
                                                     referenceRow[3].accessReal()};
            const Vector3d referenceVelocity_GCRF = {referenceRow[4].accessReal(), referenceRow[5].accessReal(),
                                                     referenceRow[6].accessReal()};

            const Vector3d referencePosition_ITRF = {referenceRow[7].accessReal(), referenceRow[8].accessReal(),
                                                     referenceRow[9].accessReal()};
            const Vector3d referenceVelocity_ITRF = {referenceRow[10].accessReal(), referenceRow[11].accessReal(),
                                                     referenceRow[12].accessReal()};

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.accessPosition();
            const Velocity velocity_GCRF = state_GCRF.accessVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(5e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(5e-3, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.accessPosition();
            const Velocity velocity_ITRF = state_ITRF.accessVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(5e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(5e-3, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            // std::cout << "x @ GCRF = " << referencePosition_GCRF.toString(10) << " / " <<
            // position_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "x @ ITRF = " <<
            // referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dx = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()
            // << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl ;

            // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " <<
            // velocity_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "v @ ITRF = " <<
            // referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()
            // << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl ;

            // std::cout << "dx | dv = " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << Real((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl ;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler, Test_6)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;

    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(6878.14);
        const Real eccentricity = 0.0012;
        const Angle inclination = Angle::Degrees(97.5106);
        const Angle raan = Angle::Degrees(319.2343);
        const Angle aop = Angle::Degrees(187.4562);
        const Angle trueAnomaly = Angle::Degrees(172.6498);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::Models::EGM96::GravitationalParameter;
        const Length equatorialRadius = Earth::Models::EGM96::EquatorialRadius;
        const Real J2 = Earth::Models::EGM96::J2;
        const Real J4 = Earth::Models::EGM96::J4;

        const Kepler keplerianModel = {coe, epoch, gravitationalParameter,      equatorialRadius,
                                       J2,  J4,    Kepler::PerturbationType::J4};

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_6/Satellite Orbit.csv")),
            Table::Format::CSV, true);

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {referenceRow[1].accessReal(), referenceRow[2].accessReal(),
                                                     referenceRow[3].accessReal()};
            const Vector3d referenceVelocity_GCRF = {referenceRow[4].accessReal(), referenceRow[5].accessReal(),
                                                     referenceRow[6].accessReal()};

            const Vector3d referencePosition_ITRF = {referenceRow[7].accessReal(), referenceRow[8].accessReal(),
                                                     referenceRow[9].accessReal()};
            const Vector3d referenceVelocity_ITRF = {referenceRow[10].accessReal(), referenceRow[11].accessReal(),
                                                     referenceRow[12].accessReal()};

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.accessPosition();
            const Velocity velocity_GCRF = state_GCRF.accessVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(5e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(5e-3, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.accessPosition();
            const Velocity velocity_ITRF = state_ITRF.accessVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(5e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(5e-3, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            // std::cout << "x @ GCRF = " << referencePosition_GCRF.toString(10) << " / " <<
            // position_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "x @ ITRF = " <<
            // referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dx = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()
            // << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl ;

            // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " <<
            // velocity_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "v @ ITRF = " <<
            // referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()
            // << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl ;

            // std::cout << "dx | dv = " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << Real((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl ;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
