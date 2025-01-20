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
using ostk::core::type::Shared;
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
using ostk::physics::environment::object::Celestial;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::Access;
using ostk::astrodynamics::access::AccessTarget;
using ostk::astrodynamics::access::Generator;
using ostk::astrodynamics::access::VisibilityCriterion;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Access_Generator : public ::testing::Test
{
   protected:
    Environment defaultEnvironment_ = Environment::Default();
    const Shared<const Celestial> defaultEarthSPtr_ = defaultEnvironment_.accessCelestialObjectWithName("Earth");
    Duration defaultStep_ = Duration::Minutes(1.0);
    Duration defaultTolerance_ = Duration::Microseconds(1.0);
    Generator defaultGenerator_ = {this->defaultEnvironment_, this->defaultStep_, this->defaultTolerance_};
};

class OpenSpaceToolkit_Astrodynamics_Access_AccessTarget : public ::testing::Test
{
   protected:
    Environment defaultEnvironment_ = Environment::Default();
    const Shared<const Celestial> defaultEarthSPtr_ = defaultEnvironment_.accessCelestialObjectWithName("Earth");
    const ostk::mathematics::object::Interval<Real> defaultAzimuthInterval_ =
        ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0);
    const ostk::mathematics::object::Interval<Real> defaultElevationInterval_ =
        ostk::mathematics::object::Interval<Real>::Closed(0.0, 90.0);
    const ostk::mathematics::object::Interval<Real> defaultRangeInterval_ =
        ostk::mathematics::object::Interval<Real>::Closed(0.0, 1.0e10);
    const LLA defaultLLA_ = LLA::Vector({0.0, 0.0, 0.0});

    const VisibilityCriterion defaultVisibilityCriterion_ =
        VisibilityCriterion::FromAERInterval(defaultAzimuthInterval_, defaultElevationInterval_, defaultRangeInterval_);

    const AccessTarget defaultAccessTarget_ =
        AccessTarget::FromLLA(defaultVisibilityCriterion_, defaultLLA_, defaultEarthSPtr_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, Constructor)
{
    // Constructor

    {
        // FromLLA

        {
            EXPECT_NO_THROW(AccessTarget::FromLLA(defaultVisibilityCriterion_, defaultLLA_, defaultEarthSPtr_));

            EXPECT_THROW(
                AccessTarget::FromLLA(defaultVisibilityCriterion_, LLA::Undefined(), defaultEarthSPtr_),
                ostk::core::error::runtime::Undefined
            );
        }

        // FromPosition

        {
            {
                const Position position = Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF());
                EXPECT_NO_THROW(AccessTarget::FromPosition(defaultVisibilityCriterion_, position));
            }

            {
                EXPECT_THROW(
                    AccessTarget::FromPosition(defaultVisibilityCriterion_, Position::Undefined()),
                    ostk::core::error::runtime::Undefined
                );
            }
        }

        // FromTrajectory

        {
            {
                const Trajectory trajectory = Trajectory::Position(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()));
                EXPECT_NO_THROW(AccessTarget::FromTrajectory(defaultVisibilityCriterion_, trajectory));
            }

            {
                EXPECT_THROW(
                    AccessTarget::FromTrajectory(defaultVisibilityCriterion_, Trajectory::Undefined()),
                    ostk::core::error::runtime::Undefined
                );
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, Constructor)
{
    {
        EXPECT_NO_THROW(Generator generator(defaultEnvironment_););
    }

    {
        const auto accessFilter = [](const Access&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(Generator generator(defaultEnvironment_, defaultStep_, defaultTolerance_, accessFilter););
    }

    {
        const auto accessFilter = [](const Access&) -> bool
        {
            return true;
        };

        const auto stateFilter = [](const State&, const State&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(
            Generator generator(defaultEnvironment_, defaultStep_, defaultTolerance_, accessFilter, stateFilter);
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, AccessType)
{
    EXPECT_EQ(defaultAccessTarget_.accessType(), AccessTarget::Type::Fixed);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, AccessVisibilityCriterion)
{
    EXPECT_EQ(defaultAccessTarget_.accessVisibilityCriterion(), defaultVisibilityCriterion_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, AccessTrajectory)
{
    EXPECT_NO_THROW(defaultAccessTarget_.accessTrajectory());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, GetPosition)
{
    {
        EXPECT_VECTORS_ALMOST_EQUAL(
            defaultAccessTarget_.getPosition().accessCoordinates(),
            Position::Meters(
                defaultLLA_.toCartesian(Earth::EGM2008.equatorialRadius_, Earth::EGM2008.flattening_), Frame::ITRF()
            )
                .accessCoordinates(),
            1e-13
        );
    }

    {
        const AccessTarget nonFixedAccessTarget = AccessTarget::FromTrajectory(
            defaultVisibilityCriterion_, Trajectory::Position(defaultAccessTarget_.getPosition())
        );
        EXPECT_THROW(nonFixedAccessTarget.getPosition(), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, GetLLA)
{
    EXPECT_VECTORS_ALMOST_EQUAL(
        defaultAccessTarget_.getLLA(defaultEarthSPtr_).toVector(), defaultLLA_.toVector(), 1e-15
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, ComputeR_SEZ_ECEF)
{
    Matrix3d r_SEZ_ECEF;
    r_SEZ_ECEF.row(0) = Vector3d {0.0, 0.0, -1.0};
    r_SEZ_ECEF.row(1) = Vector3d {0.0, 1.0, 0.0};
    r_SEZ_ECEF.row(2) = Vector3d {1.0, 0.0, 0.0};

    EXPECT_MATRICES_ALMOST_EQUAL(defaultAccessTarget_.computeR_SEZ_ECEF(defaultEarthSPtr_), r_SEZ_ECEF, 1e-15);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, FromLLA)
{
    const AccessTarget accessTarget =
        AccessTarget::FromLLA(defaultVisibilityCriterion_, defaultLLA_, defaultEarthSPtr_);

    EXPECT_EQ(accessTarget.accessType(), AccessTarget::Type::Fixed);
    EXPECT_EQ(accessTarget.accessVisibilityCriterion(), defaultVisibilityCriterion_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, FromPosition)
{
    const Position position = Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF());
    const AccessTarget accessTarget = AccessTarget::FromPosition(defaultVisibilityCriterion_, position);

    EXPECT_EQ(accessTarget.accessType(), AccessTarget::Type::Fixed);
    EXPECT_EQ(accessTarget.accessVisibilityCriterion(), defaultVisibilityCriterion_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_AccessTarget, FromTrajectory)
{
    const Trajectory trajectory = Trajectory::Position(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()));
    const AccessTarget accessTarget = AccessTarget::FromTrajectory(defaultVisibilityCriterion_, trajectory);

    EXPECT_EQ(accessTarget.accessType(), AccessTarget::Type::Trajectory);
    EXPECT_EQ(accessTarget.accessVisibilityCriterion(), defaultVisibilityCriterion_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, IsDefined)
{
    {
        EXPECT_TRUE(defaultGenerator_.isDefined());
    }

    {
        const auto stateFilter = [](const State& aFirstState, const State& aSecondState) -> bool
        {
            (void)aFirstState;
            (void)aSecondState;
            return true;
        };

        const auto accessFilter = [](const Access&) -> bool
        {
            return true;
        };

        const Generator generator = {defaultEnvironment_, defaultStep_, defaultTolerance_, accessFilter, stateFilter};

        EXPECT_TRUE(generator.isDefined());
    }

    {
        EXPECT_FALSE(Generator::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetStep)
{
    {
        EXPECT_EQ(defaultStep_, defaultGenerator_.getStep());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getStep());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetTolerance)
{
    {
        EXPECT_EQ(defaultTolerance_, defaultGenerator_.getTolerance());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getTolerance());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetAccessFilter)
{
    {
        EXPECT_EQ(nullptr, defaultGenerator_.getAccessFilter());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getAccessFilter());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetStateFilter)
{
    {
        EXPECT_EQ(nullptr, defaultGenerator_.getStateFilter());
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getStateFilter());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, GetConditionFunction)
{
    const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

    const auto generateFirstOrbit = [this, &epoch]() -> Orbit
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

        const Orbit orbit = {keplerianModel, defaultEnvironment_.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const auto generateSecondOrbit = [this, &epoch]() -> Orbit
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

        const Orbit orbit = {keplerianModel, defaultEnvironment_.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const Orbit fromOrbit = generateFirstOrbit();
    const Orbit toOrbit = generateSecondOrbit();

    const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromAERInterval(
        ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0),
        ostk::mathematics::object::Interval<Real>::Closed(-90.0, 90.0),
        ostk::mathematics::object::Interval<Real>::Closed(0.0, 1.0e10)
    );
    const AccessTarget accessTarget = AccessTarget::FromTrajectory(visibilityCriterion, fromOrbit);

    {
        const auto conditionFunction = defaultGenerator_.getConditionFunction(accessTarget, toOrbit);

        EXPECT_NE(nullptr, conditionFunction);
        EXPECT_TRUE(conditionFunction(epoch));
    }

    {
        EXPECT_ANY_THROW(Generator::Undefined().getConditionFunction(accessTarget, toOrbit));
    }

    {
        EXPECT_ANY_THROW(defaultGenerator_.getConditionFunction(accessTarget, Trajectory::Undefined()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses_1)
{
    const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
    const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);

    const Interval interval = Interval::Closed(startInstant, endInstant);

    const auto generateFirstOrbit = [this, &startInstant]() -> Orbit
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

        const Orbit orbit = {keplerianModel, defaultEnvironment_.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const auto generateSecondOrbit = [this, &startInstant]() -> Orbit
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

        const Orbit orbit = {keplerianModel, defaultEnvironment_.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const Orbit fromOrbit = generateFirstOrbit();
    const Orbit toOrbit = generateSecondOrbit();

    const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

    const AccessTarget accessTarget = AccessTarget::FromTrajectory(visibilityCriterion, fromOrbit);

    const Array<Access> accesses = defaultGenerator_.computeAccesses(interval, accessTarget, toOrbit);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses_2)
{
    const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
    const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 3, 0, 0, 0), Scale::UTC);

    const Interval interval = Interval::Closed(startInstant, endInstant);

    const LLA groundStationLla = {Angle::Degrees(0.0), Angle::Degrees(0.0), Length::Meters(20.0)};

    const auto generateSatelliteOrbit = [this, &startInstant]() -> Orbit
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

        const Orbit orbit = {keplerianModel, defaultEnvironment_.accessCelestialObjectWithName("Earth")};

        return orbit;
    };

    const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

    const AccessTarget groundStationTarget =
        AccessTarget::FromLLA(visibilityCriterion, groundStationLla, defaultEarthSPtr_);

    const Orbit satelliteOrbit = generateSatelliteOrbit();

    const Array<Access> accesses = defaultGenerator_.computeAccesses(interval, groundStationTarget, satelliteOrbit);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses_3)
{
    const Instant startInstant = Instant::DateTime(DateTime(2018, 9, 6, 0, 0, 0), Scale::UTC);
    const Instant endInstant = Instant::DateTime(DateTime(2018, 9, 7, 0, 0, 0), Scale::UTC);

    const Interval interval = Interval::Closed(startInstant, endInstant);

    const LLA groundStationLla = {Angle::Degrees(-45.0), Angle::Degrees(-170.0), Length::Meters(5.0)};

    const auto generateSatelliteOrbit = [this, &startInstant]() -> Orbit
    {
        const TLE tle = {
            "1 39419U 13066D   18248.44969859 -.00000394  00000-0 -31796-4 0  9997",
            "2 39419  97.6313 314.6863 0012643 218.7350 141.2966 14.93878994260975"
        };

        const SGP4 orbitalModel = {tle};

        const Orbit orbit = {orbitalModel, defaultEarthSPtr_};

        return orbit;
    };

    const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

    const AccessTarget groundStationTarget =
        AccessTarget::FromLLA(visibilityCriterion, groundStationLla, defaultEarthSPtr_);
    const Orbit satelliteOrbit = generateSatelliteOrbit();

    const Array<Access> accesses = defaultGenerator_.computeAccesses(interval, groundStationTarget, satelliteOrbit);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses_4)
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

    const VisibilityCriterion visibilityCriterion = VisibilityCriterion::FromLineOfSight(defaultEnvironment_);

    const AccessTarget accessTarget = AccessTarget::FromTrajectory(visibilityCriterion, firstTrajectory);

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

        const Generator generator = {defaultEnvironment_, defaultStep_, defaultTolerance_, {}, stateFilter};

        const Array<Access> accesses = generator.computeAccesses(interval, accessTarget, secondTrajectory);

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

        const Generator generator = {defaultEnvironment_, defaultStep_, defaultTolerance_, {}, stateFilter};

        const Array<Access> accesses = generator.computeAccesses(interval, accessTarget, secondTrajectory);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, ComputeAccesses)
{
    {
        const Instant startInstant = Instant::J2000();
        const Instant endInstant = startInstant + Duration::Days(1.0);
        const Interval interval = Interval::Closed(startInstant, endInstant);

        const TLE tle = {
            "1 39419U 13066D   18248.44969859 -.00000394  00000-0 -31796-4 0  9997",
            "2 39419  97.6313 314.6863 0012643 218.7350 141.2966 14.93878994260975"
        };

        const SGP4 sgp4 = SGP4(tle);
        const Orbit orbit = Orbit(sgp4, defaultEarthSPtr_);

        const ostk::mathematics::object::Interval<Real> azimuthInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0);
        const ostk::mathematics::object::Interval<Real> elevationInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 90.0);
        const ostk::mathematics::object::Interval<Real> rangeInterval =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 1.0e10);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval);

        const AccessTarget accessTarget = AccessTarget::FromLLA(
            visibilityCriterion,
            LLA(Angle::Degrees(53.406), Angle::Degrees(-6.225), Length::Meters(50.5)),
            defaultEarthSPtr_
        );

        const AccessTarget trajectoryTarget = AccessTarget::FromTrajectory(visibilityCriterion, orbit);

        // array of targets

        {
            EXPECT_THROW(
                defaultGenerator_.computeAccesses(Interval::Undefined(), {accessTarget}, orbit),
                ostk::core::error::runtime::Undefined
            );

            EXPECT_THROW(defaultGenerator_.computeAccesses(interval, {}, orbit), ostk::core::error::runtime::Undefined);

            EXPECT_THROW(
                defaultGenerator_.computeAccesses(interval, {accessTarget}, Trajectory::Undefined()),
                ostk::core::error::runtime::Undefined
            );

            EXPECT_THROW(
                defaultGenerator_.computeAccesses(interval, {trajectoryTarget}, orbit, true),
                ostk::core::error::RuntimeError
            );

            EXPECT_THROW(
                defaultGenerator_.computeAccesses(interval, {trajectoryTarget, accessTarget}, orbit, true),
                ostk::core::error::RuntimeError
            );

            {
                const auto stateFilter = [](const State&, const State&) -> bool
                {
                    return true;
                };

                defaultGenerator_.setStateFilter(stateFilter);

                EXPECT_THROW(
                    defaultGenerator_.computeAccesses(interval, {accessTarget, accessTarget}, orbit),
                    ostk::core::error::RuntimeError
                );

                defaultGenerator_.setStateFilter(nullptr);
            }
        }

        // single target

        {
            EXPECT_THROW(
                defaultGenerator_.computeAccesses(Interval::Undefined(), accessTarget, orbit),
                ostk::core::error::runtime::Undefined
            );

            EXPECT_THROW(
                defaultGenerator_.computeAccesses(interval, accessTarget, Trajectory::Undefined()),
                ostk::core::error::runtime::Undefined
            );

            EXPECT_THROW(
                defaultGenerator_.computeAccesses(interval, trajectoryTarget, orbit, true),
                ostk::core::error::RuntimeError
            );
        }
    }

    {
        const TLE tle = {
            "1 60504U 24149AN  24293.10070306  .00000000  00000-0  58313-3 0    08",
            "2 60504  97.4383   7.6998 0003154 274.9510 182.9597 15.19652001  9607",
        };
        const SGP4 sgp4 = SGP4(tle);
        const Orbit aToTrajectory = Orbit(sgp4, defaultEnvironment_.accessCelestialObjectWithName("Earth"));

        const Instant startInstant = Instant::Parse("2024-10-19 02:25:00.744.384", Scale::UTC);
        const Instant endInstant = startInstant + Duration::Hours(6.0);
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

        // AER Interval visibility criterion, multiple targets

        {
            const ostk::mathematics::object::Interval<Real> azimuthInterval =
                ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0);
            const ostk::mathematics::object::Interval<Real> elevationInterval =
                ostk::mathematics::object::Interval<Real>::Closed(0.0, 90.0);
            const ostk::mathematics::object::Interval<Real> rangeInterval =
                ostk::mathematics::object::Interval<Real>::Closed(0.0, 1.0e10);

            const VisibilityCriterion visibilityCriterion =
                VisibilityCriterion::FromAERInterval(azimuthInterval, elevationInterval, rangeInterval);

            Array<AccessTarget> accessTargets = LLAs.map<AccessTarget>(
                [&visibilityCriterion, this](const LLA& lla) -> AccessTarget
                {
                    return AccessTarget::FromLLA(visibilityCriterion, lla, defaultEarthSPtr_);
                }
            );

            const Array<Array<Access>> accessesPerTarget =
                defaultGenerator_.computeAccesses(interval, accessTargets, aToTrajectory);

            ASSERT_EQ(accessesPerTarget.getSize(), accessTargets.getSize());

            for (Index i = 0; i < accessesPerTarget.getSize(); ++i)
            {
                const Array<Access> accesses = accessesPerTarget.at(i);
                const AccessTarget groundTarget = accessTargets.at(i);

                const Array<Access> expectedAccesses =
                    defaultGenerator_.computeAccesses(interval, groundTarget, aToTrajectory);

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

        // Elevation Interval visibility criterion, multiple targets

        {
            const ostk::mathematics::object::Interval<Real> elevationInterval =
                ostk::mathematics::object::Interval<Real>::Closed(0.0, 90.0);

            const VisibilityCriterion visibilityCriterion =
                VisibilityCriterion::FromElevationInterval(elevationInterval);

            Array<AccessTarget> accessTargets = LLAs.map<AccessTarget>(
                [&visibilityCriterion, this](const LLA& lla) -> AccessTarget
                {
                    return AccessTarget::FromLLA(visibilityCriterion, lla, defaultEarthSPtr_);
                }
            );

            const Array<Array<Access>> accessesPerTarget =
                defaultGenerator_.computeAccesses(interval, accessTargets, aToTrajectory);

            ASSERT_EQ(accessesPerTarget.getSize(), accessTargets.getSize());

            for (Index i = 0; i < accessesPerTarget.getSize(); ++i)
            {
                const Array<Access> accesses = accessesPerTarget.at(i);
                const AccessTarget groundTarget = accessTargets.at(i);

                const Array<Access> expectedAccesses =
                    defaultGenerator_.computeAccesses(interval, groundTarget, aToTrajectory);

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
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetStep)
{
    {
        const Duration step = Duration::Seconds(1.0);

        EXPECT_NO_THROW(defaultGenerator_.setStep(step));

        EXPECT_ANY_THROW(defaultGenerator_.setStep(Duration::Undefined()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetTolerance)
{
    {
        const Duration tolerance = Duration::Seconds(1.0);

        EXPECT_NO_THROW(defaultGenerator_.setTolerance(tolerance));

        EXPECT_ANY_THROW(defaultGenerator_.setTolerance(Duration::Undefined()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetAccessFilter)
{
    {
        const auto accessFilter = [](const Access&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(defaultGenerator_.setAccessFilter(accessFilter));

        EXPECT_NO_THROW(defaultGenerator_.setAccessFilter({}));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, SetStateFilter)
{
    {
        const auto stateFilter = [](const State&, const State&) -> bool
        {
            return true;
        };

        EXPECT_NO_THROW(defaultGenerator_.setStateFilter(stateFilter));

        EXPECT_NO_THROW(defaultGenerator_.setStateFilter({}));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, Undefined)
{
    {
        EXPECT_NO_THROW(Generator::Undefined());

        EXPECT_FALSE(Generator::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, AerRanges_1)
{
    {
        // Access computation

        const ostk::mathematics::object::Interval<Real> azimuthRange =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 360.0);
        const ostk::mathematics::object::Interval<Real> elevationRange =
            ostk::mathematics::object::Interval<Real>::Closed(60.0, 90.0);
        const ostk::mathematics::object::Interval<Real> rangeRange =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 10000e3);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERInterval(azimuthRange, elevationRange, rangeRange);

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 10, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const LLA groundStationLla = {Angle::Degrees(47.8864), Angle::Degrees(106.906), Length::Meters(10.0)};

        const auto generateSatelliteOrbit = [this, &startInstant]() -> Orbit
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

            const Orbit orbit = {keplerianModel, defaultEnvironment_.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const AccessTarget groundStationTarget =
            AccessTarget::FromLLA(visibilityCriterion, groundStationLla, defaultEarthSPtr_);
        const Orbit satelliteOrbit = generateSatelliteOrbit();

        const Array<Access> accesses = defaultGenerator_.computeAccesses(interval, groundStationTarget, satelliteOrbit);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Access_Generator, AerMask_1)
{
    {
        // Access computation

        const ostk::core::container::Map<Real, Real> azimuthElevationMask = {
            {0.0, 30.0}, {90.0, 60.0}, {180.0, 60.0}, {270.0, 30.0}, {359.0, 30.0}
        };
        const ostk::mathematics::object::Interval<Real> rangeRange =
            ostk::mathematics::object::Interval<Real>::Closed(0.0, 10000e3);

        const VisibilityCriterion visibilityCriterion =
            VisibilityCriterion::FromAERMask(azimuthElevationMask, rangeRange);

        const Instant startInstant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Instant endInstant = Instant::DateTime(DateTime(2018, 1, 5, 0, 0, 0), Scale::UTC);

        const Interval interval = Interval::Closed(startInstant, endInstant);

        const LLA groundStationLla = {Angle::Degrees(47.8864), Angle::Degrees(106.906), Length::Meters(10.0)};

        const auto generateSatelliteOrbit = [this, &startInstant]() -> Orbit
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

            const Orbit orbit = {keplerianModel, defaultEnvironment_.accessCelestialObjectWithName("Earth")};

            return orbit;
        };

        const AccessTarget groundStationTarget =
            AccessTarget::FromLLA(visibilityCriterion, groundStationLla, defaultEarthSPtr_);

        const Orbit satelliteOrbit = generateSatelliteOrbit();

        const Array<Access> accesses = defaultGenerator_.computeAccesses(interval, groundStationTarget, satelliteOrbit);

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
