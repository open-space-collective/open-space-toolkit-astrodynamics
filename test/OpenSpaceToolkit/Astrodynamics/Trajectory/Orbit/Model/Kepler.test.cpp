/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
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

using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, Constructor)
// {

//     using ostk::core::type::Shared;
//     using ostk::core::type::Real;
//     using ostk::core::container::Array;
//     using ostk::core::container::Table;
//     using ostk::core::filesystem::Path;
//     using ostk::core::filesystem::File;

//     using ostk::mathematics::object::Vector3d;

//     using ostk::physics::unit::Length;
//     using ostk::physics::unit::Angle;
//     using ostk::physics::unit::Derived;
//     using ostk::physics::time::Scale;
//     using ostk::physics::time::Instant;
//     using ostk::physics::time::Duration;
//     using ostk::physics::time::Interval;
//     using ostk::physics::time::DateTime;
//     using ostk::physics::coordinate::Frame;
//     using ostk::physics::coordinate::Position;
//     using ostk::physics::coordinate::Velocity;
//     using ostk::physics::environment::object::celestial::Earth;

//     using ostk::astrodynamics::trajectory::Orbit;
//     using ostk::astrodynamics::trajectory::State;
//     using ostk::astrodynamics::trajectory::orbit::model::Kepler;
//     using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

//     // {

//     //     const Earth earth = Earth::Analytical();

//     //     const Length semiMajorAxis = Length::Kilometers(7000.0);
//     //     const double eccentricity = 0.0;
//     //     const Angle inclination = Angle::Degrees(0.0);
//     //     const Angle raan = Angle::Degrees(0.0);
//     //     const Angle aop = Angle::Degrees(0.0);
//     //     const Angle trueAnomaly = Angle::Degrees(0.0);

//     //     const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly };

//     //     std::cout << coe << std::endl;

//     //     const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC);

//     //     const Kepler keplerianModel = { coe, epoch, earth, Kepler::PerturbationType::None };

//     //     std::cout << keplerianModel.getClassicalOrbitalElements() << std::endl;

//     // }

//     {

//         const Earth earth = Earth::Analytical();

//         const Length semiMajorAxis = Length::Kilometers(7000.0);
//         const double eccentricity = 0.0;
//         const Angle inclination = Angle::Degrees(0.0);
//         const Angle raan = Angle::Degrees(0.0);
//         const Angle aop = Angle::Degrees(0.0);
//         const Angle trueAnomaly = Angle::Degrees(0.0);

//         const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly };

//         std::cout << coe << std::endl;

//         const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC);

//         const Kepler keplerianModel = { coe, epoch, earth, Kepler::PerturbationType::None, true };

//         std::cout << keplerianModel.getClassicalOrbitalElements() << std::endl;

//     }

// }

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, EqualToOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, NotEqualToOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, StreamOperator)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, IsDefined)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, GetClassicalOrbitalElements)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, GetEpoch)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, GetRevolutionNumberAtEpoch)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, GetGravitationalParameter)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, GetEquatorialRadius)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, GetJ2)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, GetJ4)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, GetPerturbationType)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, CalculateStateAt)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, CalculateRevolutionNumberAt)
// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, StringFromPerturbationType)

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, Test_1)
{
    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC);
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const double J2 = Earth::EGM2008.J2_;
        const double J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_1/Satellite Orbit.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
            };
            const Vector3d referenceVelocity_GCRF = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
            };

            const Vector3d referencePosition_ITRF = {
                referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()
            };
            const Vector3d referenceVelocity_ITRF = {
                referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()
            };

            const double referenceRevolutionNumber = referenceRow[13].accessReal();

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.getPosition();
            const Velocity velocity_GCRF = state_GCRF.getVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1e-3, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(1e-6, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.getPosition();
            const Velocity velocity_ITRF = state_ITRF.getVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(1e-1, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(1e-0, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            ASSERT_EQ(static_cast<int>(std::floor(referenceRevolutionNumber)), orbit.getRevolutionNumberAt(instant));

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

            // std::cout << "dx | dv = " << double((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << double((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << double((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << double((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, Test_2)
{
    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::EGM96.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM96.equatorialRadius_;
        const double J2 = Earth::EGM96.J2_;
        const double J4 = Earth::EGM96.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J2
        };

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_2/Satellite Orbit.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
            };
            const Vector3d referenceVelocity_GCRF = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
            };

            const Vector3d referencePosition_ITRF = {
                referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()
            };
            const Vector3d referenceVelocity_ITRF = {
                referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()
            };

            const double referenceRevolutionNumber = referenceRow[13].accessReal();

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.getPosition();
            const Velocity velocity_GCRF = state_GCRF.getVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(1e-3, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.getPosition();
            const Velocity velocity_ITRF = state_ITRF.getVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(1e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(1e-3, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            ASSERT_EQ(static_cast<int>(std::floor(referenceRevolutionNumber)), orbit.getRevolutionNumberAt(instant));

            if (static_cast<int>(std::floor(referenceRevolutionNumber)) != orbit.getRevolutionNumberAt(instant))
            {
                std::cout << "instant = " << instant.toString() << std::endl;

                FAIL();
            }

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

            // std::cout << "dx | dv = " << double((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << double((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << double((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << double((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, Test_3)
{
    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 7.3226415931607501e-03;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::EGM96.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM96.equatorialRadius_;
        const double J2 = Earth::EGM96.J2_;
        const double J4 = Earth::EGM96.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J2
        };

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_3/Satellite Orbit.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
            };
            const Vector3d referenceVelocity_GCRF = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
            };

            const Vector3d referencePosition_ITRF = {
                referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()
            };
            const Vector3d referenceVelocity_ITRF = {
                referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()
            };

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.getPosition();
            const Velocity velocity_GCRF = state_GCRF.getVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(5e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(1e-2, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.getPosition();
            const Velocity velocity_ITRF = state_ITRF.getVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(5e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(1e-2, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

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

            // std::cout << "dx | dv = " << double((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << double((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << double((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << double((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, Test_4)
{
    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::EGM96.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM96.equatorialRadius_;
        const double J2 = Earth::EGM96.J2_;
        const double J4 = Earth::EGM96.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J4
        };

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_4/Satellite Orbit.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
            };
            const Vector3d referenceVelocity_GCRF = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
            };

            const Vector3d referencePosition_ITRF = {
                referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()
            };
            const Vector3d referenceVelocity_ITRF = {
                referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()
            };

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.getPosition();
            const Velocity velocity_GCRF = state_GCRF.getVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(1e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(1e-3, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.getPosition();
            const Velocity velocity_ITRF = state_ITRF.getVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(1e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(1e-3, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

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

            // std::cout << "dx | dv = " << double((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << double((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << double((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << double((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, Test_5)
{
    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const double eccentricity = 7.3226415931607501e-03;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::EGM96.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM96.equatorialRadius_;
        const double J2 = Earth::EGM96.J2_;
        const double J4 = Earth::EGM96.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J4
        };

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_5/Satellite Orbit.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
            };
            const Vector3d referenceVelocity_GCRF = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
            };

            const Vector3d referencePosition_ITRF = {
                referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()
            };
            const Vector3d referenceVelocity_ITRF = {
                referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()
            };

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.getPosition();
            const Velocity velocity_GCRF = state_GCRF.getVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(5e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(5e-3, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.getPosition();
            const Velocity velocity_ITRF = state_ITRF.getVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(5e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(5e-3, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

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

            // std::cout << "dx | dv = " << double((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << double((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << double((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << double((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, Test_6)
{
    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const Length semiMajorAxis = Length::Kilometers(6878.14);
        const double eccentricity = 0.0012;
        const Angle inclination = Angle::Degrees(97.5106);
        const Angle raan = Angle::Degrees(319.2343);
        const Angle aop = Angle::Degrees(187.4562);
        const Angle trueAnomaly = Angle::Degrees(172.6498);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::EGM96.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM96.equatorialRadius_;
        const double J2 = Earth::EGM96.J2_;
        const double J4 = Earth::EGM96.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J4
        };

        // Orbit setup

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_6/Satellite Orbit.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_GCRF = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
            };
            const Vector3d referenceVelocity_GCRF = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
            };

            const Vector3d referencePosition_ITRF = {
                referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()
            };
            const Vector3d referenceVelocity_ITRF = {
                referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()
            };

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.getPosition();
            const Velocity velocity_GCRF = state_GCRF.getVelocity();

            ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            ASSERT_GT(5e-0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            ASSERT_GT(5e-3, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.getPosition();
            const Velocity velocity_ITRF = state_ITRF.getVelocity();

            ASSERT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            ASSERT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            ASSERT_GT(5e-0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            ASSERT_GT(5e-3, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

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

            // std::cout << "dx | dv = " << double((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << double((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12) << " | " << double((velocity_GCRF.accessCoordinates() -
            // referenceVelocity_GCRF).norm()).toString(12) << " - " << double((velocity_ITRF.accessCoordinates() -
            // referenceVelocity_ITRF).norm()).toString(12) << std::endl;
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, CalculateRevolutionNumberAt)
{
    const Shared<const Frame> gcrfSPtr = Frame::GCRF();

    const Environment environment = Environment::Default();

    const Instant defaultEpoch = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);

    // Kepler's revolution number at epoch is hardcoded to 1
    const int defaultRevolutionNumber = 1;

    const Derived gravitationalParameter = Earth::Spherical.gravitationalParameter_;
    const Length equatorialRadius = Earth::Spherical.equatorialRadius_;
    const double J2 = Earth::Spherical.J2_;
    const double J4 = Earth::Spherical.J4_;

    // Test simple positive and negative revolution numbers for non-edge case initial state (far away from the ascending
    // node)
    {
        const Position initialPosition = Position::Meters({0.0, 0.0, 7000000.0}, gcrfSPtr);
        const Velocity initialVelocity =
            Velocity::MetersPerSecond({5335.865450622126, 5335.865450622126, 0.0}, gcrfSPtr);

        const COE coe = COE::Cartesian({initialPosition, initialVelocity}, gravitationalParameter);
        const Duration orbitalPeriod = coe.getOrbitalPeriod(gravitationalParameter);

        // Setup Kepler model and orbit
        const Kepler keplerianModel = {
            coe, defaultEpoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Check revolution numbers for Kepler model
        EXPECT_EQ(defaultRevolutionNumber, keplerianModel.calculateRevolutionNumberAt(defaultEpoch));
        EXPECT_EQ(
            defaultRevolutionNumber + 1, keplerianModel.calculateRevolutionNumberAt(defaultEpoch + orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber - 1, keplerianModel.calculateRevolutionNumberAt(defaultEpoch - orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber + 2, keplerianModel.calculateRevolutionNumberAt(defaultEpoch + 2 * orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber - 2, keplerianModel.calculateRevolutionNumberAt(defaultEpoch - 2 * orbitalPeriod)
        );

        // Check revolution numbers for orbit
        EXPECT_EQ(defaultRevolutionNumber, orbit.getRevolutionNumberAt(defaultEpoch));
        EXPECT_EQ(defaultRevolutionNumber - 1, orbit.getRevolutionNumberAt(defaultEpoch - orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber + 1, orbit.getRevolutionNumberAt(defaultEpoch + orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber + 2, orbit.getRevolutionNumberAt(defaultEpoch + 2 * orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber - 2, orbit.getRevolutionNumberAt(defaultEpoch - 2 * orbitalPeriod));
    }

    // Test simple positive and negative revolution numbers for edge case initial state (at the ascending node)
    {
        const Position initialPosition = Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr);
        const Velocity initialVelocity =
            Velocity::MetersPerSecond({0.0, 5335.865450622126, 5335.865450622126}, gcrfSPtr);

        const COE coe = COE::Cartesian({initialPosition, initialVelocity}, gravitationalParameter);
        const Duration orbitalPeriod = coe.getOrbitalPeriod(gravitationalParameter);

        // Setup Kepler model and orbit
        const Kepler keplerianModel = {
            coe, defaultEpoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Check revolution numbers for Kepler model
        EXPECT_EQ(defaultRevolutionNumber, keplerianModel.calculateRevolutionNumberAt(defaultEpoch));
        EXPECT_EQ(
            defaultRevolutionNumber + 1, keplerianModel.calculateRevolutionNumberAt(defaultEpoch + orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber - 1, keplerianModel.calculateRevolutionNumberAt(defaultEpoch - orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber + 2, keplerianModel.calculateRevolutionNumberAt(defaultEpoch + 2 * orbitalPeriod)
        );
        EXPECT_EQ(
            defaultRevolutionNumber - 2, keplerianModel.calculateRevolutionNumberAt(defaultEpoch - 2 * orbitalPeriod)
        );

        // Check revolution numbers for orbit
        EXPECT_EQ(defaultRevolutionNumber, orbit.getRevolutionNumberAt(defaultEpoch));
        EXPECT_EQ(defaultRevolutionNumber - 1, orbit.getRevolutionNumberAt(defaultEpoch - orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber + 1, orbit.getRevolutionNumberAt(defaultEpoch + orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber + 2, orbit.getRevolutionNumberAt(defaultEpoch + 2 * orbitalPeriod));
        EXPECT_EQ(defaultRevolutionNumber - 2, orbit.getRevolutionNumberAt(defaultEpoch - 2 * orbitalPeriod));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler, Frame)
{
    const Shared<const ostk::physics::environment::object::Celestial> earthSPtr =
        Environment::Default().accessCelestialObjectWithName("Earth");

    const Instant epoch = Instant::DateTime(DateTime::Parse("2026-01-01T00:00:00"), Scale::UTC);

    const COE coe = {
        Length::Kilometers(7000.0),
        0.01,
        Angle::Degrees(97.5),
        Angle::Degrees(40.0),
        Angle::Degrees(10.0),
        Angle::Degrees(20.0),
    };

    const Shared<const Frame> temeSPtr = Frame::TEME();

    // Default frame is GCRF
    {
        const Kepler model = {coe, epoch, *earthSPtr, Kepler::PerturbationType::J2};

        EXPECT_EQ(*model.getFrame(), *Frame::GCRF());
        EXPECT_EQ(model.calculateStateAt(epoch + Duration::Hours(1.0)).getFrame(), Frame::GCRF());

        const Kepler modelExplicit = {coe, epoch, *earthSPtr, Kepler::PerturbationType::J2, false, Frame::GCRF()};

        EXPECT_EQ(model, modelExplicit);
    }

    // Explicit Earth-parameter constructor
    {
        const Kepler model = {
            coe,
            epoch,
            earthSPtr->getGravitationalParameter(),
            earthSPtr->getEquatorialRadius(),
            earthSPtr->getJ2(),
            earthSPtr->getJ4(),
            Kepler::PerturbationType::J2,
            temeSPtr
        };

        EXPECT_EQ(*model.getFrame(), *temeSPtr);
    }

    // States are returned in the model frame, for every perturbation type
    for (const Kepler::PerturbationType& perturbationType :
         {Kepler::PerturbationType::None, Kepler::PerturbationType::J2, Kepler::PerturbationType::J4})
    {
        const Kepler modelGCRF = {coe, epoch, *earthSPtr, perturbationType};
        const Kepler modelTEME = {coe, epoch, *earthSPtr, perturbationType, false, temeSPtr};

        const Instant instant = epoch + Duration::Hours(6.0);

        const State stateGCRF = modelGCRF.calculateStateAt(instant);
        const State stateTEME = modelTEME.calculateStateAt(instant);

        EXPECT_EQ(*stateTEME.getFrame(), *temeSPtr);

        // Same elements, same numbers: only the frame they are interpreted in differs
        EXPECT_TRUE(stateTEME.getPosition().getCoordinates().isNear(stateGCRF.getPosition().getCoordinates(), 1e-6));
        EXPECT_TRUE(stateTEME.getVelocity().getCoordinates().isNear(stateGCRF.getVelocity().getCoordinates(), 1e-9));

        // ... which is a physically different orbit (TEME and GCRF differ by ~0.3 deg of precession since J2000)
        const double separation =
            (stateTEME.inFrame(Frame::GCRF()).getPosition().getCoordinates() - stateGCRF.getPosition().getCoordinates())
                .norm();

        EXPECT_GT(separation, 1.0e3);
    }

    // Body-fixed elements are converted into the model frame
    {
        const Kepler model = {coe, epoch, *earthSPtr, Kepler::PerturbationType::None, true, temeSPtr};

        const Vector3d expectedPositionITRF =
            coe.getCartesianState(earthSPtr->getGravitationalParameter(), earthSPtr->accessFrame())
                .first.getCoordinates();

        const Vector3d positionITRF =
            model.calculateStateAt(epoch).getPosition().inFrame(earthSPtr->accessFrame(), epoch).getCoordinates();

        EXPECT_TRUE(positionITRF.isNear(expectedPositionITRF, 1e-6));
    }

    // Equality accounts for the frame
    {
        const Kepler modelGCRF = {coe, epoch, *earthSPtr, Kepler::PerturbationType::J2};
        const Kepler modelTEME = {coe, epoch, *earthSPtr, Kepler::PerturbationType::J2, false, temeSPtr};

        EXPECT_NE(modelGCRF, modelTEME);
        EXPECT_EQ(modelTEME, modelTEME);
    }

    // Undefined frame
    {
        EXPECT_ANY_THROW(Kepler(coe, epoch, *earthSPtr, Kepler::PerturbationType::J2, false, nullptr));
        EXPECT_ANY_THROW(Kepler(coe, epoch, *earthSPtr, Kepler::PerturbationType::J2, true, nullptr));
    }

    // Frame must be quasi-inertial
    {
        EXPECT_ANY_THROW(Kepler(coe, epoch, *earthSPtr, Kepler::PerturbationType::J2, false, Frame::ITRF()));
        EXPECT_ANY_THROW(Kepler(coe, epoch, *earthSPtr, Kepler::PerturbationType::J2, true, Frame::ITRF()));
        EXPECT_ANY_THROW(Kepler(
            coe,
            epoch,
            earthSPtr->getGravitationalParameter(),
            earthSPtr->getEquatorialRadius(),
            earthSPtr->getJ2(),
            earthSPtr->getJ4(),
            Kepler::PerturbationType::J2,
            Frame::ITRF()
        ));
    }
}
