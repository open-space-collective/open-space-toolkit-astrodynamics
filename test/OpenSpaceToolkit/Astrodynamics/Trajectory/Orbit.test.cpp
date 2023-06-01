/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
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

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Utilities.test.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Constructor)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Integer;
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
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        EXPECT_TRUE(orbit.isDefined());
    }

    {
        const Environment environment = Environment::Default();

        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}};

        const Integer initialRevolutionNumber = 123;

        const Orbit orbit = {states, initialRevolutionNumber, environment.accessCelestialObjectWithName("Earth")};

        EXPECT_TRUE(orbit.isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, EqualToOperator)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Integer;
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
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        EXPECT_TRUE(orbit == orbit);

        EXPECT_FALSE(orbit == Orbit::Undefined());
        EXPECT_FALSE(Orbit::Undefined() == orbit);
    }

    {
        EXPECT_FALSE(Orbit::Undefined() == Orbit::Undefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, NotEqualToOperator)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Integer;
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
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        EXPECT_FALSE(orbit != orbit);

        EXPECT_TRUE(orbit != Orbit::Undefined());
        EXPECT_TRUE(Orbit::Undefined() != orbit);
    }

    {
        EXPECT_TRUE(Orbit::Undefined() != Orbit::Undefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, IsDefined)
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
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        EXPECT_TRUE(orbit.isDefined());
    }

    {
        EXPECT_FALSE(Orbit::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, GetRevolutionNumberAt)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Integer;
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
    using ostk::astro::trajectory::orbit::Pass;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Passes.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto& referenceRow : referenceData)
        {
            const Integer referenceRevolutionNumber = referenceRow[0].accessInteger();
            const Instant referencePassStartInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
            const Instant referencePassEndInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);

            const Instant instant =
                referencePassStartInstant + Duration::Between(referencePassStartInstant, referencePassEndInstant) / 2.0;

            EXPECT_EQ(referenceRevolutionNumber, orbit.getRevolutionNumberAt(instant));
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, GetPassAt)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Integer;
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
    using ostk::astro::trajectory::orbit::Pass;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Passes.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto& referenceRow : referenceData)
        {
            const Instant referencePassStartInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
            const Instant referencePassEndInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);

            const Instant instant =
                referencePassStartInstant + Duration::Between(referencePassStartInstant, referencePassEndInstant) / 2.0;

            const Pass pass = orbit.getPassAt(instant);

            EXPECT_TRUE(pass.isDefined());

            EXPECT_GT(
                Duration::Microseconds(1.0),
                Duration::Between(referencePassStartInstant, pass.getInterval().getStart()).getAbsolute()
            );
            EXPECT_GT(
                Duration::Microseconds(1.0),
                Duration::Between(referencePassEndInstant, pass.getInterval().getEnd()).getAbsolute()
            );
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, GetPassWithRevolutionNumber)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Integer;
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
    using ostk::astro::trajectory::orbit::Pass;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_1/Satellite Passes.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto& referenceRow : referenceData)
        {
            const Integer referenceRevolutionNumber = referenceRow[0].accessInteger();
            const Instant referencePassStartInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
            const Instant referencePassEndInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);

            const Pass pass = orbit.getPassWithRevolutionNumber(referenceRevolutionNumber);

            EXPECT_TRUE(pass.isDefined());

            EXPECT_EQ(Pass::Type::Complete, pass.getType());

            EXPECT_GT(
                Duration::Microseconds(1.0),
                Duration::Between(referencePassStartInstant, pass.getInterval().getStart()).getAbsolute()
            );
            EXPECT_GT(
                Duration::Microseconds(1.0),
                Duration::Between(referencePassEndInstant, pass.getInterval().getEnd()).getAbsolute()
            );
        }
    }

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J2};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_2/Satellite Passes.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto& referenceRow : referenceData)
        {
            const Integer referenceRevolutionNumber = referenceRow[0].accessInteger();
            const Instant referencePassStartInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
            const Instant referencePassEndInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);

            const Pass pass = orbit.getPassWithRevolutionNumber(referenceRevolutionNumber);

            EXPECT_TRUE(pass.isDefined());

            EXPECT_EQ(Pass::Type::Complete, pass.getType());

            EXPECT_GT(
                Duration::Milliseconds(1.0),
                Duration::Between(referencePassStartInstant, pass.getInterval().getStart()).getAbsolute()
            );
            EXPECT_GT(
                Duration::Milliseconds(1.0),
                Duration::Between(referencePassEndInstant, pass.getInterval().getEnd()).getAbsolute()
            );
        }
    }

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J4};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/Test_4/Satellite Passes.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto& referenceRow : referenceData)
        {
            const Integer referenceRevolutionNumber = referenceRow[0].accessInteger();
            const Instant referencePassStartInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
            const Instant referencePassEndInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);

            const Pass pass = orbit.getPassWithRevolutionNumber(referenceRevolutionNumber);

            EXPECT_TRUE(pass.isDefined());

            EXPECT_EQ(Pass::Type::Complete, pass.getType());

            EXPECT_GT(
                Duration::Milliseconds(2.0),
                Duration::Between(referencePassStartInstant, pass.getInterval().getStart()).getAbsolute()
            );
            EXPECT_GT(
                Duration::Milliseconds(2.0),
                Duration::Between(referencePassEndInstant, pass.getInterval().getEnd()).getAbsolute()
            );
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, GetOrbitalFrame)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;
    using ostk::core::types::String;

    using ostk::math::geom::d3::trf::rot::Quaternion;
    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Transform;
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
    using ostk::astro::trajectory::orbit::Pass;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

    // NED

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/GetOrbitalFrame/NED_ITRF 1.csv")
        );

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Shared<const Earth> earthSPtr =
            std::dynamic_pointer_cast<const Earth>(environment.accessObjectWithName("Earth"));

        const Orbit orbit = {keplerianModel, earthSPtr};

        const Shared<const Frame> nedOrbitalFrameSPtr = orbit.getOrbitalFrame(Orbit::FrameType::NED);

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d x_NED_ITRF_ref = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()};
            const Vector3d v_NED_ITRF_in_ITRF_ref = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()};

            const Quaternion q_NED_ITRF_ref = Quaternion::XYZS(
                                                  referenceRow[7].accessReal(),
                                                  referenceRow[8].accessReal(),
                                                  referenceRow[9].accessReal(),
                                                  referenceRow[10].accessReal()
            )
                                                  .normalize();
            const Vector3d w_NED_ITRF_in_NED_ref = {
                referenceRow[11].accessReal(), referenceRow[12].accessReal(), referenceRow[13].accessReal()};

            const Quaternion q_ITRF_NED_ref = q_NED_ITRF_ref.toConjugate();
            const Vector3d w_ITRF_NED_in_ITRF_ref = -(q_ITRF_NED_ref * w_NED_ITRF_in_NED_ref);

            const Vector3d x_NED_ITRF = nedOrbitalFrameSPtr->getOriginIn(Frame::ITRF(), instant).getCoordinates();
            const Vector3d v_NED_ITRF_in_ITRF =
                nedOrbitalFrameSPtr->getVelocityIn(Frame::ITRF(), instant).getCoordinates();

            const Quaternion q_ITRF_NED = nedOrbitalFrameSPtr->getTransformTo(Frame::ITRF(), instant).getOrientation();
            const Vector3d w_ITRF_NED_in_ITRF =
                nedOrbitalFrameSPtr->getTransformTo(Frame::ITRF(), instant).getAngularVelocity();

            ASSERT_TRUE(x_NED_ITRF.isNear(x_NED_ITRF_ref, 1e-1)) << String::Format(
                "@ {}: {} - {} = {} [m]",
                instant.toString(),
                x_NED_ITRF_ref.toString(),
                x_NED_ITRF.toString(),
                (x_NED_ITRF - x_NED_ITRF_ref).norm()
            );
            ASSERT_TRUE(v_NED_ITRF_in_ITRF.isNear(v_NED_ITRF_in_ITRF_ref, 1e-4)) << String::Format(
                "@ {}: {} - {} = {} [m/s]",
                instant.toString(),
                v_NED_ITRF_in_ITRF_ref.toString(),
                v_NED_ITRF_in_ITRF.toString(),
                (v_NED_ITRF_in_ITRF - v_NED_ITRF_in_ITRF_ref).norm()
            );

            ASSERT_TRUE(q_ITRF_NED.isNear(q_ITRF_NED_ref, Angle::Arcseconds(1.0))) << String::Format(
                "@ {}: {} / {} = {} [asec]",
                instant.toString(),
                q_ITRF_NED_ref.toString(),
                q_ITRF_NED.toString(),
                q_ITRF_NED.angularDifferenceWith(q_ITRF_NED_ref).inArcseconds().toString()
            );
            ASSERT_TRUE(w_ITRF_NED_in_ITRF.isNear(w_ITRF_NED_in_ITRF_ref, 1e-8)) << String::Format(
                "@ {}: {} - {} = {} [rad/s]",
                instant.toString(),
                w_ITRF_NED_in_ITRF_ref.toString(),
                w_ITRF_NED_in_ITRF.toString(),
                (w_ITRF_NED_in_ITRF - w_ITRF_NED_in_ITRF_ref).norm()
            );
        }
    }

    // LVLH

    // {

    //     // Environment setup

    //     const Environment environment = Environment::Default() ;

    //     // Orbit setup

    //     const Length semiMajorAxis = Length::Kilometers(7000.0) ;
    //     const Real eccentricity = 0.0 ;
    //     const Angle inclination = Angle::Degrees(0.0) ;
    //     const Angle raan = Angle::Degrees(0.0) ;
    //     const Angle aop = Angle::Degrees(0.0) ;
    //     const Angle trueAnomaly = Angle::Degrees(0.0) ;

    //     const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

    //     const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
    //     const Derived gravitationalParameter = Earth::GravitationalParameter ;
    //     const Length equatorialRadius = Earth::EquatorialRadius ;
    //     const Real J2 = Earth::J2 ;
    //     const Real J4 = Earth::J4 ;

    //     const Kepler keplerianModel = { coe, epoch, gravitationalParameter, equatorialRadius, J2, J4,
    //     Kepler::PerturbationType::None } ;

    //     const Shared<const Earth> earthSPtr = std::dynamic_pointer_cast<const
    //     Earth>(environment.accessObjectWithName("Earth")) ;

    //     const Orbit orbit = { keplerianModel, earthSPtr } ;

    //     const Shared<const Frame> lvlhOrbitalFrameSPtr = orbit.getOrbitalFrame(Orbit::FrameType::LVLH) ;

    //     const Position origin_ITRF = lvlhOrbitalFrameSPtr->getOriginIn(Frame::ITRF(), epoch) ;

    //     std::cout << "Origin:" << std::endl << origin_ITRF << std::endl ;
    //     // std::cout << "Origin:" << std::endl << lvlhOrbitalFrameSPtr->getOriginIn(Frame::GCRF(), epoch) <<
    //     std::endl ;

    //     // std::cout << "Axes:" << std::endl << lvlhOrbitalFrameSPtr->getAxesIn(Frame::ITRF(), epoch) << std::endl ;
    //     // std::cout << "Axes:" << std::endl << lvlhOrbitalFrameSPtr->getAxesIn(Frame::GCRF(), epoch) << std::endl ;

    //     // for (const auto& referenceRow : referenceData)
    //     // {

    //     //     const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC)
    //     ;

    //     //     const Vector3d x_NED_ITRF_ref = { referenceRow[1].accessReal(), referenceRow[2].accessReal(),
    //     referenceRow[3].accessReal() } ;
    //     //     const Vector3d v_NED_ITRF_in_ITRF_ref = { referenceRow[4].accessReal(), referenceRow[5].accessReal(),
    //     referenceRow[6].accessReal() } ;

    //     //     const Quaternion q_NED_ITRF_ref = Quaternion::XYZS(referenceRow[7].accessReal(),
    //     referenceRow[8].accessReal(), referenceRow[9].accessReal(), referenceRow[10].accessReal()).normalize() ;
    //     //     const Vector3d w_NED_ITRF_in_NED_ref = { referenceRow[11].accessReal(), referenceRow[12].accessReal(),
    //     referenceRow[13].accessReal() } ;

    //     //     const Quaternion q_ITRF_NED_ref = q_NED_ITRF_ref.toConjugate() ;
    //     //     const Vector3d w_ITRF_NED_in_ITRF_ref = - (q_ITRF_NED_ref * w_NED_ITRF_in_NED_ref) ;

    //     //     const Vector3d x_NED_ITRF = lvlhOrbitalFrameSPtr->getOriginIn(Frame::ITRF(), instant).getCoordinates()
    //     ;
    //     //     const Vector3d v_NED_ITRF_in_ITRF = lvlhOrbitalFrameSPtr->getVelocityIn(Frame::ITRF(),
    //     instant).getCoordinates() ;

    //     //     const Quaternion q_ITRF_NED = lvlhOrbitalFrameSPtr->getTransformTo(Frame::ITRF(),
    //     instant).getOrientation() ;
    //     //     const Vector3d w_ITRF_NED_in_ITRF = lvlhOrbitalFrameSPtr->getTransformTo(Frame::ITRF(),
    //     instant).getAngularVelocity() ;

    //     //     EXPECT_TRUE(x_NED_ITRF.isNear(x_NED_ITRF_ref, 1e-1)) << String::Format("{} - {} = {} [m]",
    //     x_NED_ITRF_ref.toString(), x_NED_ITRF.toString(), (x_NED_ITRF - x_NED_ITRF_ref).norm()) ;
    //     //     EXPECT_TRUE(v_NED_ITRF_in_ITRF.isNear(v_NED_ITRF_in_ITRF_ref, 1e-4)) << String::Format("{} - {} = {}
    //     [m/s]", v_NED_ITRF_in_ITRF_ref.toString(), v_NED_ITRF_in_ITRF.toString(), (v_NED_ITRF_in_ITRF -
    //     v_NED_ITRF_in_ITRF_ref).norm()) ;

    //     //     EXPECT_TRUE(q_ITRF_NED.isNear(q_ITRF_NED_ref, Angle::Arcseconds(1.0))) << String::Format("{} / {} = {}
    //     [asec]", q_ITRF_NED_ref.toString(), q_ITRF_NED.toString(),
    //     q_ITRF_NED.angularDifferenceWith(q_ITRF_NED_ref).inArcseconds().toString()) ;
    //     //     // EXPECT_TRUE(w_ITRF_NED_in_ITRF.isNear(w_ITRF_NED_in_ITRF_ref, 1e-12)) << String::Format("{} - {} =
    //     {} [rad/s]", w_ITRF_NED_in_ITRF_ref.toString(), w_ITRF_NED_in_ITRF.toString(), (w_ITRF_NED_in_ITRF -
    //     w_ITRF_NED_in_ITRF_ref).norm()) ;

    //     // }

    // }

    // VVLH

    {

        // [TBI]

    }

    {
        EXPECT_ANY_THROW(Orbit::Undefined().getOrbitalFrame(Orbit::FrameType::Undefined));
        EXPECT_ANY_THROW(Orbit::Undefined().getOrbitalFrame(Orbit::FrameType::NED));
        EXPECT_ANY_THROW(Orbit::Undefined().getOrbitalFrame(Orbit::FrameType::LVLH));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Print)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Integer;
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
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::GravitationalParameter;
        const Length equatorialRadius = Earth::EquatorialRadius;
        const Real J2 = Earth::J2;
        const Real J4 = Earth::J4;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(orbit.print(std::cout, true));
        EXPECT_NO_THROW(orbit.print(std::cout, false));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Undefined)
{
    using ostk::astro::trajectory::Orbit;

    {
        EXPECT_NO_THROW(Orbit::Undefined());
        EXPECT_FALSE(Orbit::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Circular)
{
    using ostk::core::ctnr::Array;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;

    using ostk::physics::Environment;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;

    {
        struct Scenario
        {
            String identifier;
            Instant epoch;
            Length altitude;
            Angle inclination;
            File referenceDataFile;
            Real positionTolerance_GCRF_m;
            Real velocityTolerance_GCRF_mps;
            Real positionTolerance_ITRF_m;
            Real velocityTolerance_ITRF_mps;
        };

        const Array<Scenario> scenarios = {
            {"Scenario 1",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(500.0),
             Angle::Degrees(0.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 1.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 2",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(500.0),
             Angle::Degrees(45.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 2.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 3",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(500.0),
             Angle::Degrees(90.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 3.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 4",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(500.0),
             Angle::Degrees(135.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 4.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 5",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(500.0),
             Angle::Degrees(180.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 5.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 6",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(5000.0),
             Angle::Degrees(0.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 6.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 7",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(5000.0),
             Angle::Degrees(45.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 7.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 8",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(5000.0),
             Angle::Degrees(90.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 8.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 9",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(5000.0),
             Angle::Degrees(135.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 9.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 10",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(5000.0),
             Angle::Degrees(180.0),
             File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Circular/Scenario 10.csv"
             )),
             1e-3,
             1e-6,
             1e-1,
             1e-4}};

        for (const auto& scenario : scenarios)
        {
            // Environment setup

            const Environment environment = Environment::Default();

            // Orbit setup

            const Orbit orbit = Orbit::Circular(
                scenario.epoch,
                scenario.altitude,
                scenario.inclination,
                environment.accessCelestialObjectWithName("Earth")
            );

            // Test

            testOrbit(
                scenario.identifier,
                scenario.referenceDataFile,
                orbit,
                scenario.positionTolerance_GCRF_m,
                scenario.velocityTolerance_GCRF_mps,
                scenario.positionTolerance_ITRF_m,
                scenario.velocityTolerance_ITRF_mps
            );
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Equatorial)
{
    using ostk::core::ctnr::Array;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;

    using ostk::physics::Environment;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;

    {
        struct Scenario
        {
            String identifier;
            Instant epoch;
            Length apoapsisAltitude;
            Length periapsisAltitude;
            File referenceDataFile;
            Real positionTolerance_GCRF_m;
            Real velocityTolerance_GCRF_mps;
            Real positionTolerance_ITRF_m;
            Real velocityTolerance_ITRF_mps;
        };

        const Array<Scenario> scenarios = {
            {"Scenario 1",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(500.0),
             Length::Kilometers(400.0),
             File::Path(
                 Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Equatorial/Scenario 1.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 2",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(500.0),
             Length::Kilometers(500.0),
             File::Path(
                 Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Equatorial/Scenario 2.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 3",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(600.0),
             Length::Kilometers(500.0),
             File::Path(
                 Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Equatorial/Scenario 3.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 4",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(5000.0),
             Length::Kilometers(4000.0),
             File::Path(
                 Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Equatorial/Scenario 4.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 5",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(5000.0),
             Length::Kilometers(5000.0),
             File::Path(
                 Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Equatorial/Scenario 5.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 6",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(6000.0),
             Length::Kilometers(5000.0),
             File::Path(
                 Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Equatorial/Scenario 6.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 7",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(5000.0),
             Length::Kilometers(500.0),
             File::Path(
                 Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Equatorial/Scenario 7.csv")
             ),
             1e-3,
             1e-6,
             1e-1,
             1e-4},
            {"Scenario 8",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(50000.0),
             Length::Kilometers(500.0),
             File::Path(
                 Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Equatorial/Scenario 8.csv")
             ),
             1e-3,
             1e-6,
             1e-0,
             1e-3}};

        for (const auto& scenario : scenarios)
        {
            // Environment setup

            const Environment environment = Environment::Default();

            // Orbit setup

            const Orbit orbit = Orbit::Equatorial(
                scenario.epoch,
                scenario.apoapsisAltitude,
                scenario.periapsisAltitude,
                environment.accessCelestialObjectWithName("Earth")
            );

            // Test

            testOrbit(
                scenario.identifier,
                scenario.referenceDataFile,
                orbit,
                scenario.positionTolerance_GCRF_m,
                scenario.velocityTolerance_GCRF_mps,
                scenario.positionTolerance_ITRF_m,
                scenario.velocityTolerance_ITRF_mps
            );
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, CircularEquatorial)
{
    using ostk::core::ctnr::Array;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;

    using ostk::physics::Environment;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;

    {
        struct Scenario
        {
            String identifier;
            Instant epoch;
            Length altitude;
            File referenceDataFile;
            Real positionTolerance_GCRF_m;
            Real velocityTolerance_GCRF_mps;
            Real positionTolerance_ITRF_m;
            Real velocityTolerance_ITRF_mps;
        };

        const Array<Scenario> scenarios = {
            {"Scenario 1",
             Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
             Length::Kilometers(500.0),
             File::Path(Path::Parse(
                 "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/CircularEquatorial/Scenario 1.csv"
             )),
             1e-3,
             1e-6,
             1e-1,
             1e-4}};

        for (const auto& scenario : scenarios)
        {
            // Environment setup

            const Environment environment = Environment::Default();

            // Orbit setup

            const Orbit orbit = Orbit::CircularEquatorial(
                scenario.epoch, scenario.altitude, environment.accessCelestialObjectWithName("Earth")
            );

            // Test

            testOrbit(
                scenario.identifier,
                scenario.referenceDataFile,
                orbit,
                scenario.positionTolerance_GCRF_m,
                scenario.velocityTolerance_GCRF_mps,
                scenario.positionTolerance_ITRF_m,
                scenario.velocityTolerance_ITRF_mps
            );
        }
    }
}

// TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, SunSynchronous)
// {

//     using ostk::core::ctnr::Array ;
//     using ostk::core::fs::Path ;
//     using ostk::core::fs::File ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::time::Scale ;
//     using ostk::physics::time::Instant ;
//     using ostk::physics::time::DateTime ;
//     using ostk::physics::time::Time ;
//     using ostk::physics::Environment ;

//     using ostk::astro::trajectory::Orbit ;

//     {

//         struct Scenario
//         {

//             String identifier ;
//             Instant epoch ;
//             Length altitude ;
//             Time localTimeAtDescendingNode ;
//             File referenceDataFile ;
//             Real positionTolerance_GCRF_m ;
//             Real velocityTolerance_GCRF_mps ;
//             Real positionTolerance_ITRF_m ;
//             Real velocityTolerance_ITRF_mps ;

//         } ;

//         const Array<Scenario> scenarios =
//         {
//             {
//                 "Scenario 1",
//                 Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
//                 Length::Kilometers(500.0),
//                 Time::Parse("12:00:00"),
//                 File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/SunSynchronous/Scenario
//                 1.csv")), 100.0, 1.0, 100.0, 1.0
//                 // 1e-3,
//                 // 1e-6,
//                 // 1e-1,
//                 // 1e-4
//             }
//         } ;

//         for (const auto& scenario : scenarios)
//         {

//             // Environment setup

//             const Environment environment = Environment::Default() ;

//             // Orbit setup

//             const Orbit orbit = Orbit::SunSynchronous(scenario.epoch, scenario.altitude,
//             scenario.localTimeAtDescendingNode, environment.accessCelestialObjectWithName("Earth")) ;

//             // Test

//             testOrbit(scenario.identifier,
//                       scenario.referenceDataFile,
//                       orbit,
//                       scenario.positionTolerance_GCRF_m,
//                       scenario.velocityTolerance_GCRF_mps,
//                       scenario.positionTolerance_ITRF_m,
//                       scenario.velocityTolerance_ITRF_mps) ;

//         }

//     }

// }
