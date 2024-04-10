/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>
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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Utility.test.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::container::Map;
using ostk::core::container::Pair;
using ostk::core::container::Table;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Index;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::Environment;
using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::time::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::orbit::Pass;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Constructor)
{
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

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
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Integer initialRevolutionNumber = 123;

        const Orbit orbit = {states, initialRevolutionNumber, environment.accessCelestialObjectWithName("Earth")};

        EXPECT_TRUE(orbit.isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, EqualToOperator)
{
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        EXPECT_TRUE(orbit.isDefined());
    }

    {
        EXPECT_FALSE(Orbit::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, GetRevolutionNumberAt)
{
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_1/Satellite Passes.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto &referenceRow : referenceData)
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_1/Satellite Passes.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto &referenceRow : referenceData)
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
                Duration::Between(referencePassStartInstant, pass.accessInstantAtAscendingNode()).getAbsolute()
            );
            EXPECT_GT(
                Duration::Microseconds(1.0),
                Duration::Between(referencePassEndInstant, pass.accessInstantAtPassBreak()).getAbsolute()
            );
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, ComputePasses)
{
    // undefined revolution number
    {
        EXPECT_THROW(Orbit::ComputePasses({}, Integer::Undefined()), ostk::core::error::runtime::Undefined);
    }

    // states < 2
    {
        const Array<State> states = {
            {
                Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
                Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({1.0, 0.0, 0.0}, Frame::GCRF()),
            },
        };
        EXPECT_THROW(Orbit::ComputePasses(states, 1).isEmpty(), ostk::core::error::RuntimeError);
    }

    // states out of order
    {
        const Array<State> states = {
            {
                Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC),
                Position::Meters({0.0, 0.0, 0.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({1.0, 0.0, 0.0}, Frame::GCRF()),
            },
            {
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
                Position::Meters({1.0, 0.0, 0.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({1.0, 0.0, 0.0}, Frame::GCRF()),
            }
        };
        EXPECT_THROW(Orbit::ComputePasses(states, 1), ostk::core::error::RuntimeError);
    }

    // partial revolution
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Pass test
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime::Parse("2018-01-01 02:30:00"), Scale::UTC);
        const Array<Instant> instants =
            Interval::Closed(startInstant, endInstant).generateGrid(Duration::Seconds(20.0));
        const Array<State> states = orbit.getStatesAt(instants);

        const Array<Pair<Index, Pass>> passMap = Orbit::ComputePasses(states, 1);

        for (const auto &row : passMap)
        {
            const Pass &pass = row.second;

            EXPECT_TRUE(pass.isDefined());
        }

        EXPECT_EQ(passMap.accessLast().second.getType(), Pass::Type::Partial);
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_1/"
                                   "Satellite Passes.csv")),
            Table::Format::CSV,
            true
        );

        // Pass test
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime::Parse("2018-01-01 23:00:00"), Scale::UTC);
        const Array<Instant> instants =
            Interval::Closed(startInstant, endInstant).generateGrid(Duration::Seconds(20.0));
        const Array<State> states = orbit.getStatesAt(instants);

        const Array<Pair<Index, Pass>> passMap = Orbit::ComputePasses(states, 1);

        EXPECT_EQ(referenceData.getRowCount(), passMap.size() - 1);  // We're generating 1 pass over the reference data

        Index i = 0;
        Index stateIndex = 0;
        for (const auto &row : passMap)
        {
            // Ignore the lass pass, as it is not complete
            if (i > referenceData.getRowCount() - 1)
            {
                break;
            }

            // test computed Pass
            const Pass &pass = row.second;

            const auto &referenceRow = referenceData[i];

            const Instant referencePassStartInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[1].accessString()), Scale::UTC);
            const Instant referencePassEndInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[2].accessString()), Scale::UTC);

            const Instant referencePassAscendingNodeInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[3].accessString()), Scale::UTC);
            const Instant referencePassDescendingNodeInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[5].accessString()), Scale::UTC);

            const Instant referencePassNorthPointInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[4].accessString()), Scale::UTC);
            const Instant referencePassSouthPointInstant =
                Instant::DateTime(DateTime::Parse(referenceRow[6].accessString()), Scale::UTC);

            EXPECT_TRUE(pass.isDefined());

            EXPECT_EQ(pass.getRevolutionNumber(), referenceRow[0].accessInteger());
            EXPECT_LT(std::fabs((referencePassStartInstant - pass.accessInstantAtAscendingNode()).inSeconds()), 1e-6);
            EXPECT_LT(std::fabs((referencePassEndInstant - pass.accessInstantAtPassBreak()).inSeconds()), 1e-6);
            EXPECT_LT(
                std::fabs((referencePassAscendingNodeInstant - pass.accessInstantAtAscendingNode()).inSeconds()), 1e-6
            );
            EXPECT_LT(
                std::fabs((referencePassDescendingNodeInstant - pass.accessInstantAtDescendingNode()).inSeconds()), 1e-6
            );
            EXPECT_LT(std::fabs((referencePassNorthPointInstant - pass.accessInstantAtNorthPoint()).inSeconds()), 3.0);
            EXPECT_LT(std::fabs((referencePassSouthPointInstant - pass.accessInstantAtSouthPoint()).inSeconds()), 3.0);

            // test state index
            const Index &endStateIndex = row.first;

            EXPECT_TRUE(referencePassStartInstant <= states[stateIndex].accessInstant());
            EXPECT_TRUE(referencePassEndInstant >= states[endStateIndex - 1].accessInstant());

            stateIndex = endStateIndex;

            ++i;
        }
    }

    {
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2020-07-18 20:38:11.102.455"), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime::Parse("2020-07-18 22:26:11.102.455"), Scale::UTC);

        Array<Instant> instants = Interval::Closed(startInstant, endInstant).generateGrid(Duration::Seconds(60.0));
        if (instants.accessLast() != endInstant)
        {
            instants.add(endInstant);
        }

        const TLE tle =
            TLE("1 43890U 18111Q   20195.55622117 +.00000241 +00000-0 +28512-4 0  9991",
                "2 43890 097.6899 099.9703 0003551 181.1072 179.0140 14.92932318084236");
        const SGP4 sgp4 = SGP4(tle);

        Array<State> states;
        for (const auto &instant : instants)
        {
            states.add(sgp4.calculateStateAt(instant));
        }

        EXPECT_NO_THROW(Orbit::ComputePasses(states, 8502));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, GetPassWithRevolutionNumber)
{
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_1/"
                                   "Satellite Passes.csv")),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto &referenceRow : referenceData)
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
                Duration::Between(referencePassStartInstant, pass.accessInstantAtAscendingNode()).getAbsolute()
            );
            EXPECT_GT(
                Duration::Microseconds(1.0),
                Duration::Between(referencePassEndInstant, pass.accessInstantAtPassBreak()).getAbsolute()
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J2
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_2/"
                                   "Satellite Passes.csv")),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto &referenceRow : referenceData)
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
                Duration::Between(referencePassStartInstant, pass.accessInstantAtAscendingNode()).getAbsolute()
            );
            EXPECT_GT(
                Duration::Milliseconds(1.0),
                Duration::Between(referencePassEndInstant, pass.accessInstantAtPassBreak()).getAbsolute()
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::J4
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/Test_4/"
                                   "Satellite Passes.csv")),
            Table::Format::CSV,
            true
        );

        // Pass test

        for (const auto &referenceRow : referenceData)
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
                Duration::Between(referencePassStartInstant, pass.accessInstantAtAscendingNode()).getAbsolute()
            );
            EXPECT_GT(
                Duration::Milliseconds(2.0),
                Duration::Between(referencePassEndInstant, pass.accessInstantAtPassBreak()).getAbsolute()
            );
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, GetOrbitalFrame)
{
    using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;
    // Environment setup

    const Environment environment = Environment::Default();

    // NED

    {
        // Orbit setup

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Shared<const Earth> earthSPtr =
            std::dynamic_pointer_cast<const Earth>(environment.accessObjectWithName("Earth"));

        const Orbit orbit = {keplerianModel, earthSPtr};

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/GetOrbitalFrame/NED_ITRF 1.csv")
        );

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        const Shared<const Frame> nedOrbitalFrameSPtr = orbit.getOrbitalFrame(Orbit::FrameType::NED);

        for (const auto &referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d x_NED_ITRF_ref = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
            };
            const Vector3d v_NED_ITRF_in_ITRF_ref = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
            };

            const Quaternion q_NED_ITRF_ref = Quaternion::XYZS(
                                                  referenceRow[7].accessReal(),
                                                  referenceRow[8].accessReal(),
                                                  referenceRow[9].accessReal(),
                                                  referenceRow[10].accessReal()
            )
                                                  .normalize();
            const Vector3d w_NED_ITRF_in_NED_ref = {
                referenceRow[11].accessReal(), referenceRow[12].accessReal(), referenceRow[13].accessReal()
            };

            const Quaternion q_ITRF_NED_ref = q_NED_ITRF_ref.toConjugate();
            const Vector3d w_ITRF_NED_in_ITRF_ref = -(q_ITRF_NED_ref * w_NED_ITRF_in_NED_ref);

            const Vector3d x_NED_ITRF = nedOrbitalFrameSPtr->getOriginIn(Frame::ITRF(), instant).getCoordinates();
            const Vector3d v_NED_ITRF_in_ITRF =
                nedOrbitalFrameSPtr->getVelocityIn(Frame::ITRF(), instant).getCoordinates();

            const Quaternion q_ITRF_NED = nedOrbitalFrameSPtr->getTransformTo(Frame::ITRF(), instant).getOrientation();
            const Vector3d w_ITRF_NED_in_ITRF =
                nedOrbitalFrameSPtr->getTransformTo(Frame::ITRF(), instant).getAngularVelocity();

            EXPECT_TRUE(x_NED_ITRF.isNear(x_NED_ITRF_ref, 1e-1)) << String::Format(
                "@ {}: {} - {} = {} [m]",
                instant.toString(),
                x_NED_ITRF_ref.toString(),
                x_NED_ITRF.toString(),
                (x_NED_ITRF - x_NED_ITRF_ref).norm()
            );
            EXPECT_TRUE(v_NED_ITRF_in_ITRF.isNear(v_NED_ITRF_in_ITRF_ref, 1e-4)) << String::Format(
                "@ {}: {} - {} = {} [m/s]",
                instant.toString(),
                v_NED_ITRF_in_ITRF_ref.toString(),
                v_NED_ITRF_in_ITRF.toString(),
                (v_NED_ITRF_in_ITRF - v_NED_ITRF_in_ITRF_ref).norm()
            );

            EXPECT_TRUE(q_ITRF_NED.isNear(q_ITRF_NED_ref, Angle::Arcseconds(1.0))) << String::Format(
                "@ {}: {} / {} = {} [asec]",
                instant.toString(),
                q_ITRF_NED_ref.toString(),
                q_ITRF_NED.toString(),
                q_ITRF_NED.angularDifferenceWith(q_ITRF_NED_ref).inArcseconds().toString()
            );
            EXPECT_TRUE(w_ITRF_NED_in_ITRF.isNear(w_ITRF_NED_in_ITRF_ref, 1e-7)) << String::Format(
                "@ {}: {} - {} = {} [rad/s]",
                instant.toString(),
                w_ITRF_NED_in_ITRF_ref.toString(),
                w_ITRF_NED_in_ITRF.toString(),
                (w_ITRF_NED_in_ITRF - w_ITRF_NED_in_ITRF_ref).norm()
            );
        }
    }

    {
        const Length semiMajorAxis = Length::Meters(6878137.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(97.401458306592275);
        const Angle raan = Angle::Degrees(279.82375866413543);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {
            semiMajorAxis,
            eccentricity,
            inclination,
            raan,
            aop,
            trueAnomaly,
        };

        const Instant epoch = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);

        const Kepler keplerianModel = {
            coe,
            epoch,
            EarthGravitationalModel::EGM2008.gravitationalParameter_,
            EarthGravitationalModel::EGM2008.equatorialRadius_,
            EarthGravitationalModel::EGM2008.J2_,
            EarthGravitationalModel::EGM2008.J4_,
            Kepler::PerturbationType::None,
        };

        const Shared<const Earth> earthSPtr =
            std::dynamic_pointer_cast<const Earth>(environment.accessObjectWithName("Earth"));

        const Orbit orbit = {
            keplerianModel,
            earthSPtr,
        };

        const Array<Tuple<Orbit::FrameType, Angle, Angle>> testCases = {
            {Orbit::FrameType::LVLH, Angle::Arcseconds(1.0), Angle::Arcseconds(1.0)},
            {Orbit::FrameType::VVLH, Angle::Arcseconds(1.0), Angle::Arcseconds(1.0)},
            {Orbit::FrameType::LVLHGD, Angle::Arcseconds(1.0), Angle::Arcseconds(1.0)},
            {Orbit::FrameType::LVLHGDGT, Angle::Arcseconds(250.0), Angle::Arcseconds(15.0)},
            {Orbit::FrameType::TNW, Angle::Arcseconds(1.0), Angle::Arcseconds(1.0)},
            {Orbit::FrameType::VNC, Angle::Arcseconds(1.0), Angle::Arcseconds(1.0)},
        };

        for (const auto &testCase : testCases)
        {
            const Orbit::FrameType frameType = std::get<0>(testCase);
            const Angle angularTolerance = std::get<1>(testCase);
            const Angle angularVelocityTolerance = std::get<2>(testCase);

            const File referenceDataFile = File::Path(Path::Parse(String::Format(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/GetOrbitalFrame/{}_GCRF.csv",
                Orbit::StringFromFrameType(frameType)
            )));

            const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

            const Shared<const Frame> orbitalFrameSPtr = orbit.getOrbitalFrame(frameType);

            for (const auto &referenceRow : referenceData)
            {
                const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

                const Quaternion q_Target_GCRF_ref = Quaternion::XYZS(
                                                         referenceRow[1].accessReal(),
                                                         referenceRow[2].accessReal(),
                                                         referenceRow[3].accessReal(),
                                                         referenceRow[4].accessReal()
                )
                                                         .normalize();
                const Vector3d w_Target_GCRF_in_Target_ref = {
                    referenceRow[5].accessReal(),
                    referenceRow[6].accessReal(),
                    referenceRow[7].accessReal(),
                };

                const Quaternion q_GCRF_Target_ref = q_Target_GCRF_ref.toConjugate();
                const Vector3d w_GCRF_Target_in_GCRF_ref = -(q_GCRF_Target_ref * w_Target_GCRF_in_Target_ref);

                const Quaternion q_GCRF_Target =
                    orbitalFrameSPtr->getTransformTo(Frame::GCRF(), instant).getOrientation();
                const Vector3d w_GCRF_Target_in_GCRF =
                    orbitalFrameSPtr->getTransformTo(Frame::GCRF(), instant).getAngularVelocity();

                EXPECT_TRUE(q_GCRF_Target.isNear(q_GCRF_Target_ref, angularTolerance)) << String::Format(
                    "[{}] @ {}: {} / {} = {} [asec]",
                    Orbit::StringFromFrameType(frameType),
                    instant.toString(),
                    q_GCRF_Target_ref.toString(),
                    q_GCRF_Target.toString(),
                    q_GCRF_Target.angularDifferenceWith(q_GCRF_Target_ref).inArcseconds().toString()
                );

                EXPECT_TRUE(
                    w_GCRF_Target_in_GCRF.isNear(w_GCRF_Target_in_GCRF_ref, angularVelocityTolerance.inRadians())
                )
                    << String::Format(
                           "[{}] @ {}: {} - {} = {} [rad/s]",
                           Orbit::StringFromFrameType(frameType),
                           instant.toString(),
                           w_GCRF_Target_in_GCRF_ref.toString(),
                           w_GCRF_Target_in_GCRF.toString(),
                           (w_GCRF_Target_in_GCRF - w_GCRF_Target_in_GCRF_ref).norm()
                       );
            }
        }
    }

    {
        const Array<Orbit::FrameType> orbitFrameTypes = {
            Orbit::FrameType::NED,
            Orbit::FrameType::LVLH,
            Orbit::FrameType::VVLH,
            Orbit::FrameType::LVLHGD,
            Orbit::FrameType::LVLHGDGT,
            Orbit::FrameType::TNW,
            Orbit::FrameType::VNC,
        };

        for (const auto &frameType : orbitFrameTypes)
        {
            EXPECT_ANY_THROW(Orbit::Undefined().getOrbitalFrame(frameType));
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Print)
{
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
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(orbit.print(std::cout, true));
        EXPECT_NO_THROW(orbit.print(std::cout, false));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Undefined)
{
    {
        EXPECT_NO_THROW(Orbit::Undefined());
        EXPECT_FALSE(Orbit::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, Circular)
{
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
             1e-4}
        };

        for (const auto &scenario : scenarios)
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
             1e-3}
        };

        for (const auto &scenario : scenarios)
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
             1e-4}
        };

        for (const auto &scenario : scenarios)
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

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, GeoSynchronous)
{
    // Test longitude alignement for a certain longitude
    {
        // Environment setup
        const Environment environment = Environment::Default();

        // Set up epoch
        const Instant epoch = Instant::J2000();

        // Set up inclination and longitude
        const Angle inclination = Angle::Degrees(0.01);
        const Angle longitude = Angle::Degrees(0.0);

        // Orbit setup
        const Orbit orbit =
            Orbit::GeoSynchronous(epoch, inclination, longitude, environment.accessCelestialObjectWithName("Earth"));

        // Test
        const State state = orbit.getStateAt(epoch);
        const VectorXd ascendingNodeVector = state.getCoordinates();
        VectorXd comparisonVector(6);
        comparisonVector << 7462233.829725, -41498548.350819, 0.000000, 3026.125690, 544.155360, 0.536630;

        const double positionError = (comparisonVector - ascendingNodeVector).norm();

        EXPECT_GT(1e-6, positionError);
    }

    // Test longitude alignement for a certain longitude
    {
        // Environment setup
        const Environment environment = Environment::Default();

        // Set up epoch
        const Instant epoch = Instant::J2000();

        // Set up inclination and longitude
        const Angle inclination = Angle::Degrees(0.01);
        const Angle longitude = Angle::Degrees(180.0);

        // Orbit setup
        const Orbit orbit =
            Orbit::GeoSynchronous(epoch, inclination, longitude, environment.accessCelestialObjectWithName("Earth"));

        // Test
        const State state = orbit.getStateAt(epoch);
        const VectorXd ascendingNodeVector = state.getCoordinates();
        VectorXd comparisonVector(6);
        comparisonVector << -7462233.829725, 41498548.350819, 0.000000, -3026.125690, -544.155360, 0.536630;

        const double positionError = (comparisonVector - ascendingNodeVector).norm();

        EXPECT_GT(1e-6, positionError);
    }

    // Test longitude alignement for a certain longitude
    {
        // Environment setup
        const Environment environment = Environment::Default();

        // Set up epoch
        const Instant epoch = Instant::J2000();

        // Set up inclination and longitude
        const Angle inclination = Angle::Degrees(0.01);
        const Angle longitude = Angle::Degrees(90.0);

        // Orbit setup
        const Orbit orbit =
            Orbit::GeoSynchronous(epoch, inclination, longitude, environment.accessCelestialObjectWithName("Earth"));

        // Test
        const State state = orbit.getStateAt(epoch);
        const VectorXd ascendingNodeVector = state.getCoordinates();
        VectorXd comparisonVector(6);
        comparisonVector << 41498548.355815, 7462233.801941, 0.000000, -544.155357, 3026.125690, 0.536630;

        const double positionError = (comparisonVector - ascendingNodeVector).norm();

        EXPECT_GT(1e-6, positionError);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit, SunSynchronous)
{
    {
        {
            const Orbit orbit = Orbit::SunSynchronous(
                Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
                Length::Kilometers(500.0),
                Time::Parse("12:00:00"),
                Environment::Default().accessCelestialObjectWithName("Earth")
            );

            EXPECT_TRUE(orbit.isDefined());
        }

        {
            const Angle angle = Angle::Degrees(50.0);
            const Orbit orbit = Orbit::SunSynchronous(
                Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
                Length::Kilometers(500.0),
                Time::Parse("12:00:00"),
                Environment::Default().accessCelestialObjectWithName("Earth"),
                angle
            );

            EXPECT_TRUE(orbit.isDefined());
            EXPECT_EQ(orbit.accessModel().as<Kepler>().getClassicalOrbitalElements().getTrueAnomaly(), angle);
        }
    }

    {
        {
            EXPECT_THROW(
                Orbit::SunSynchronous(
                    Instant::Undefined(),
                    Length::Kilometers(500.0),
                    Time::Parse("12:00:00"),
                    Environment::Default().accessCelestialObjectWithName("Earth"),
                    Angle::Degrees(50.0)
                ),
                ostk::core::error::runtime::Undefined
            );
        }

        {
            EXPECT_THROW(
                Orbit::SunSynchronous(
                    Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
                    Length::Undefined(),
                    Time::Parse("12:00:00"),
                    Environment::Default().accessCelestialObjectWithName("Earth"),
                    Angle::Degrees(50.0)
                ),
                ostk::core::error::runtime::Undefined
            );
        }

        {
            EXPECT_THROW(
                Orbit::SunSynchronous(
                    Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
                    Length::Kilometers(500.0),
                    Time::Undefined(),
                    Environment::Default().accessCelestialObjectWithName("Earth"),
                    Angle::Degrees(50.0)
                ),
                ostk::core::error::runtime::Undefined
            );
        }

        {
            {
                EXPECT_THROW(
                    Orbit::SunSynchronous(
                        Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
                        Length::Kilometers(500.0),
                        Time::Parse("12:00:00"),
                        nullptr,
                        Angle::Degrees(50.0)
                    ),
                    ostk::core::error::runtime::Undefined
                );
            }

            {
                EXPECT_THROW(
                    Orbit::SunSynchronous(
                        Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
                        Length::Kilometers(500.0),
                        Time::Parse("12:00:00"),
                        std::make_shared<Celestial>(Celestial::Undefined()),
                        Angle::Degrees(50.0)
                    ),
                    ostk::core::error::runtime::Undefined
                );
            }
        }

        {
            EXPECT_THROW(
                Orbit::SunSynchronous(
                    Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
                    Length::Kilometers(500.0),
                    Time::Parse("12:00:00"),
                    Environment::Default().accessCelestialObjectWithName("Earth"),
                    Angle::Undefined()
                ),
                ostk::core::error::runtime::Undefined
            );
        }
    }
    // {
    //     struct Scenario
    //     {
    //         String identifier;
    //         Instant epoch;
    //         Length altitude;
    //         Time localTimeAtDescendingNode;
    //         File referenceDataFile;
    //         Real positionTolerance_GCRF_m;
    //         Real velocityTolerance_GCRF_mps;
    //         Real positionTolerance_ITRF_m;
    //         Real velocityTolerance_ITRF_mps;
    //     };

    //     const Array<Scenario> scenarios = {{
    //         "Scenario 1",
    //         Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00"), Scale::UTC),
    //         Length::Kilometers(500.0),
    //         Time::Parse("12:00:00"),
    //         File::Path(
    //             Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/SunSynchronous/Scenario 1.csv")
    //         ),
    //         100.0,
    //         1.0,
    //         100.0,
    //         1.0
    //         // 1e-3,
    //         // 1e-6,
    //         // 1e-1,
    //         // 1e-4
    //     }};

    //     for (const auto& scenario : scenarios)
    //     {
    //         // Environment setup

    //         const Environment environment = Environment::Default();

    //         // Orbit setup

    //         const Orbit orbit = Orbit::SunSynchronous(
    //             scenario.epoch,
    //             scenario.altitude,
    //             scenario.localTimeAtDescendingNode,
    //             environment.accessCelestialObjectWithName("Earth")
    //         );

    //         // Test

    //         testOrbit(
    //             scenario.identifier,
    //             scenario.referenceDataFile,
    //             orbit,
    //             scenario.positionTolerance_GCRF_m,
    //             scenario.velocityTolerance_GCRF_mps,
    //             scenario.positionTolerance_ITRF_m,
    //             scenario.velocityTolerance_ITRF_mps
    //         );
    //     }
    // }
}
