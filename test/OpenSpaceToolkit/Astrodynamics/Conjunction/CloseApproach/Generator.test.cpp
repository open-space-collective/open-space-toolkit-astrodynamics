/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::filesystem::Directory;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::object::Celestial;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;

using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using Earth = ostk::physics::environment::object::celestial::Earth;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::closeapproach::Generator;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

using EarthGravitationalModelType = EarthGravitationalModel::Type;
using EarthMagneticModelType = EarthMagneticModel::Type;
using EarthAtmosphericModelType = EarthAtmosphericModel::Type;
using KeplerPerturbationType = Kepler::PerturbationType;

// CDM test data structure: TCA timestamp, obj1 pos, obj1 vel, obj2 pos, obj2 vel, expected miss distance, R, S, W
using CDMTestData = std::tuple<String, Vector3d, Vector3d, Vector3d, Vector3d, double, double, double, double>;

class OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator : public ::testing::Test
{
   protected:
    const Shared<const Celestial> earthSPtr_ = std::make_shared<Earth>(Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModelType::EGM96, Directory::Undefined(), 20, 20),
        std::make_shared<EarthMagneticModel>(EarthMagneticModelType::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModelType::Undefined)
    ));

    const Instant epoch_ = Instant::J2000();

    const Trajectory referenceTrajectory_ =
        Orbit::Circular(epoch_, Length::Meters(7.0e6), Angle::Degrees(0.0), earthSPtr_);

    // Helper function to build Classical Orbital Elements (COE) for a circular orbit
    COE buildCOE_(const double& aSemiMajorAxis, const double& anInclination, const double& aTrueAnomaly)
    {
        return COE(
            Length::Meters(aSemiMajorAxis),
            0.0,  // eccentricity
            Angle::Degrees(anInclination),
            Angle::Degrees(0.0),  // RAAN
            Angle::Degrees(0.0),  // AOP
            Angle::Degrees(aTrueAnomaly)
        );
    }

    // Helper function to build a Keplerian trajectory
    Trajectory buildTrajectory_(
        const Instant& anEpoch,
        const COE& aCOE,
        const Shared<const Celestial>& aCelestialSPtr,
        const KeplerPerturbationType& aPerturbationType
    )
    {
        const Kepler kepler = Kepler(aCOE, anEpoch, *aCelestialSPtr, aPerturbationType, false);

        return Orbit(kepler, aCelestialSPtr);
    }

    // Helper function to build COE from position and velocity coordinates
    COE buildCOEFromState_(
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Shared<const Frame>& aFrame,
        const Shared<const Celestial>& aCelestialSPtr
    )
    {
        const Position position = Position::Meters(aPositionCoordinates, aFrame);
        const Velocity velocity = Velocity::MetersPerSecond(aVelocityCoordinates, aFrame);

        return COE::Cartesian({position, velocity}, aCelestialSPtr->getGravitationalParameter());
    }
};

class OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator_CDM
    : public OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator,
      public ::testing::WithParamInterface<CDMTestData>
{
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, Constructor)
{
    {
        const Duration step = Duration::Minutes(30.0);
        const Duration tolerance = Duration::Seconds(1.0e-6);

        EXPECT_NO_THROW(Generator generator(referenceTrajectory_, step, tolerance););
    }

    {
        EXPECT_NO_THROW(Generator generator(referenceTrajectory_););
    }

    {
        EXPECT_THROW(
            try {
                Generator generator(referenceTrajectory_, Duration::Undefined(), Duration::Seconds(1.0e-6));
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Step} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                Generator generator(referenceTrajectory_, Duration::Seconds(-60.0), Duration::Seconds(1.0e-6));
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("{Step} is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }

    {
        EXPECT_THROW(
            try {
                Generator generator(referenceTrajectory_, Duration::Minutes(30.0), Duration::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Tolerance} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                Generator generator(referenceTrajectory_, Duration::Minutes(30.0), Duration::Seconds(-1.0e-6));
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("{Tolerance} is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, IsDefined)
{
    {
        const Generator generator(referenceTrajectory_);

        EXPECT_TRUE(generator.isDefined());
    }

    {
        const Generator generator = Generator::Undefined();

        EXPECT_FALSE(generator.isDefined());
    }

    {
        const Generator generator(Trajectory::Undefined(), Duration::Minutes(30.0), Duration::Seconds(1.0e-6));

        EXPECT_FALSE(generator.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, GetReferenceTrajectory)
{
    {
        const Generator generator(referenceTrajectory_);

        EXPECT_EQ(referenceTrajectory_, generator.getReferenceTrajectory());
    }

    {
        const Generator generator = Generator::Undefined();

        EXPECT_THROW(
            try { generator.getReferenceTrajectory(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Generator} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, GetStep)
{
    {
        const Duration step = Duration::Minutes(30.0);
        const Generator generator(referenceTrajectory_, step);

        EXPECT_EQ(step, generator.getStep());
    }

    {
        const Generator generator = Generator::Undefined();

        EXPECT_THROW(
            try { generator.getStep(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Generator} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, GetTolerance)
{
    {
        const Duration tolerance = Duration::Seconds(1.0e-6);
        const Generator generator(referenceTrajectory_, Duration::Minutes(30.0), tolerance);

        EXPECT_EQ(tolerance, generator.getTolerance());
    }

    {
        const Generator generator = Generator::Undefined();

        EXPECT_THROW(
            try { generator.getTolerance(); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Generator} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, SetStep)
{
    {
        Generator generator(referenceTrajectory_);

        const Duration newStep = Duration::Seconds(30.0);

        EXPECT_NO_THROW(generator.setStep(newStep));
        EXPECT_EQ(newStep, generator.getStep());
    }

    {
        Generator generator(referenceTrajectory_);

        EXPECT_THROW(
            try { generator.setStep(Duration::Undefined()); } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Step} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        Generator generator(referenceTrajectory_);

        EXPECT_THROW(
            try { generator.setStep(Duration::Seconds(-60.0)); } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("{Step} is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, SetTolerance)
{
    {
        Generator generator(referenceTrajectory_);

        const Duration newTolerance = Duration::Microseconds(100.0);

        EXPECT_NO_THROW(generator.setTolerance(newTolerance));
        EXPECT_EQ(newTolerance, generator.getTolerance());
    }

    {
        Generator generator(referenceTrajectory_);

        EXPECT_THROW(
            try {
                generator.setTolerance(Duration::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Tolerance} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        Generator generator(referenceTrajectory_);

        EXPECT_THROW(
            try {
                generator.setTolerance(Duration::Seconds(-1.0e-6));
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("{Tolerance} is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator,
    ComputeCloseApproaches_MultipleApproachesFromTwoKeplerianTrajectories
)
{
    const COE referenceCOE = buildCOE_(7.0e6, 0.0, 0.0);

    const Trajectory referenceTrajectory =
        buildTrajectory_(epoch_, referenceCOE, earthSPtr_, KeplerPerturbationType::None);

    const Trajectory otherObjectTrajectory = buildTrajectory_(
        epoch_, buildCOE_(7.0e6, 90.0, 0.0), earthSPtr_, KeplerPerturbationType::None
    );  // Should yield two close approaches each period

    const Duration orbitalPeriod = referenceCOE.getOrbitalPeriod(earthSPtr_->getGravitationalParameter());

    const Generator generator(referenceTrajectory, Duration::Seconds(60.0), Duration::Seconds(1.0e-6));

    const Array<CloseApproach> closeApproaches = generator.computeCloseApproaches(
        otherObjectTrajectory,
        Interval::Closed(epoch_ + Duration::Minutes(10.0), epoch_ + Duration::Minutes(10.0) + 2.0 * orbitalPeriod)
    );

    EXPECT_EQ(4, closeApproaches.getSize());

    const Duration tolerance = Duration::Milliseconds(1.0);

    EXPECT_TRUE(closeApproaches[0].getInstant().isNear(epoch_ + 0.5 * orbitalPeriod, tolerance));
    EXPECT_TRUE(closeApproaches[1].getInstant().isNear(epoch_ + 1.0 * orbitalPeriod, tolerance));
    EXPECT_TRUE(closeApproaches[2].getInstant().isNear(epoch_ + 1.5 * orbitalPeriod, tolerance));
    EXPECT_TRUE(closeApproaches[3].getInstant().isNear(epoch_ + 2.0 * orbitalPeriod, tolerance));
}

// CDM test cases - values taken from real conjunction data messages
INSTANTIATE_TEST_SUITE_P(
    ComputeCloseApproaches_AgainstCDM,
    OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator_CDM,
    ::testing::Values(
        // CDM ID 1042070864
        std::make_tuple(
            "2025-06-09T02:32:01.359000Z",
            Vector3d(-2800.791217e3, 3626.538025e3, -5199.329126e3),
            Vector3d(-1.91561086e3, 5.549281524e3, 4.908889201e3),
            Vector3d(-2800.572435e3, 3626.430643e3, -5199.212261e3),
            Vector3d(-3.746238368e3, -5.878740757e3, -2.08390493e3),
            270.0,
            -232.3,
            -63.1,
            122.9
        ),
        // CDM ID 1044010294
        std::make_tuple(
            "2025-06-09T02:24:28.928000Z",
            Vector3d(-1066.860281e3, 541.42819e3, 6741.350852e3),
            Vector3d(-1.967361972e3, 7.382425792e3, -0.894244647e3),
            Vector3d(-1066.792625e3, 541.454976e3, 6741.07945e3),
            Vector3d(6.915454264e3, 3.273075584e3, 0.824986236e3),
            280.0,
            -275.7,
            40.0,
            -37.0
        ),
        // CDM ID 1043297263
        std::make_tuple(
            "2025-06-09T00:34:57.390000Z",
            Vector3d(-1512.800296e3, -1569.942756e3, 6485.806749e3),
            Vector3d(-6.950717393e3, -2.467422969e3, -2.215154774e3),
            Vector3d(-1513.559046e3, -1569.489523e3, 6486.30632e3),
            Vector3d(1.663289829e3, 7.202191134e3, 2.10240377e3),
            1015.0,
            537.3,
            381.2,
            -772.5
        )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator_CDM, ComputeCloseApproaches_AgainstCDM)
{
    const auto& [tcaString, object1Position, object1Velocity, object2Position, object2Velocity, expectedMissDistance, expectedR, expectedS, expectedW] =
        GetParam();

    const Instant tca = Instant::DateTime(DateTime::Parse(tcaString, DateTime::Format::ISO8601), Scale::UTC);

    const Trajectory referenceTrajectory = buildTrajectory_(
        tca,
        buildCOEFromState_(object1Position, object1Velocity, Frame::ITRF(), earthSPtr_),
        earthSPtr_,
        KeplerPerturbationType::J4
    );

    const Trajectory otherObjectTrajectory = buildTrajectory_(
        tca,
        buildCOEFromState_(object2Position, object2Velocity, Frame::ITRF(), earthSPtr_),
        earthSPtr_,
        KeplerPerturbationType::J4
    );

    const Generator generator(referenceTrajectory, Duration::Seconds(60.0), Duration::Seconds(1.0e-3));

    const Array<CloseApproach> closeApproaches = generator.computeCloseApproaches(
        otherObjectTrajectory, Interval::Closed(tca - Duration::Seconds(10.0), tca + Duration::Seconds(10.0))
    );

    ASSERT_EQ(1, closeApproaches.getSize());

    const CloseApproach& closeApproach = closeApproaches[0];

    EXPECT_TRUE(closeApproach.getInstant().isNear(tca, Duration::Seconds(1.0e-3)));
    EXPECT_NEAR(closeApproach.getMissDistance().inMeters(), expectedMissDistance, expectedMissDistance * 0.01);

    const Shared<const LocalOrbitalFrameFactory> qswFactorySPtr = LocalOrbitalFrameFactory::QSW(Frame::GCRF());
    const auto missDistanceComponents = closeApproach.computeMissDistanceComponentsInLocalOrbitalFrame(qswFactorySPtr);

    // Test it doesn't find a close approach before the TCA
    const Array<CloseApproach> closeApproachesBeforeTCA = generator.computeCloseApproaches(
        otherObjectTrajectory, Interval::Closed(tca - Duration::Seconds(20.0), tca - Duration::Seconds(10.0))
    );

    EXPECT_EQ(0, closeApproachesBeforeTCA.getSize());

    // Test it doesn't find a close approach after the TCA
    const Array<CloseApproach> closeApproachesAfterTCA = generator.computeCloseApproaches(
        otherObjectTrajectory, Interval::Closed(tca + Duration::Seconds(10.0), tca + Duration::Seconds(20.0))
    );

    EXPECT_EQ(0, closeApproachesAfterTCA.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, ComputeCloseApproaches_CoLocatedStates)
{
    const Generator generator = Generator(referenceTrajectory_);

    EXPECT_THROW(
        try {
            generator.computeCloseApproaches(
                referenceTrajectory_, Interval::Closed(epoch_, epoch_ + Duration::Seconds(10.0))
            );
        } catch (const ostk::core::error::RuntimeError& e) {
            EXPECT_EQ("Cannot compute relative distance derivative: states are co-located.", e.getMessage());
            throw;
        },
        ostk::core::error::RuntimeError
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, ComputeCloseApproaches_Undefined)
{
    {
        const Generator generator = Generator::Undefined();

        EXPECT_THROW(
            try {
                generator.computeCloseApproaches(
                    referenceTrajectory_, Interval::Closed(epoch_, epoch_ + Duration::Seconds(10.0))
                );
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Generator} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const Generator generator(referenceTrajectory_);

        EXPECT_THROW(
            try {
                generator.computeCloseApproaches(
                    Trajectory::Undefined(), Interval::Closed(epoch_, epoch_ + Duration::Seconds(10.0))
                );
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Other object trajectory} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        const Trajectory otherTrajectory =
            Orbit::Circular(epoch_, Length::Meters(7.1e6), Angle::Degrees(0.0), earthSPtr_);

        const Generator generator(referenceTrajectory_);

        EXPECT_THROW(
            try {
                generator.computeCloseApproaches(otherTrajectory, Interval::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Search interval} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator, Undefined)
{
    {
        const Generator generator = Generator::Undefined();

        EXPECT_FALSE(generator.isDefined());
    }
}
