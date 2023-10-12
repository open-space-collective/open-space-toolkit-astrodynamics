/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/BrouwerLyddaneMeanShort.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::Index;
using ostk::core::ctnr::Tuple;

using ostk::math::obj::Vector6d;
using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::coord::Frame;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::trajectory::orbit::models::kepler::BrouwerLyddaneMeanShort;
using ostk::astro::trajectory::orbit::models::kepler::COE;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort, Constructor)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle meanAnomaly = Angle::Degrees(40.0);

        EXPECT_NO_THROW(BrouwerLyddaneMeanShort(semiMajorAxis, eccentricity, inclination, raan, aop, meanAnomaly););
    }
}

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort_Parametrized
    : public ::testing::TestWithParam<std::tuple<Position, Velocity, Vector6d>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Cartesian,
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort_Parametrized,
    ::testing::Values(std::make_tuple(
        Position::Meters({6596065.624114551, 2282234.953292401, -18030.93992064121}, Frame::GCRF()),
        Velocity::MetersPerSecond({345.4716519563907, -967.0404288726759, 7488.686029827369}, Frame::GCRF()),
        Vector6d {
            6973743.736075629,
            0.001202049190851806,
            1.7071571281899047,
            0.3327524364104959,
            1.568634993642334,
            4.7143446820819745,
        }
    ))
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort_Parametrized, Cartesian)
{
    const auto& [position, velocity, expectedMeanElements] = GetParam();
    const BrouwerLyddaneMeanShort brouwerMeanShortOE = BrouwerLyddaneMeanShort::Cartesian(
        {position, velocity}, EarthGravitationalModel::EGM2008.gravitationalParameter_
    );

    EXPECT_LT(std::abs(brouwerMeanShortOE.getSemiMajorAxis().inMeters() - expectedMeanElements[0]), 1e-2);
    EXPECT_LT(std::abs(brouwerMeanShortOE.getEccentricity() - expectedMeanElements[1]), 1e-5);
    EXPECT_LT(std::abs(brouwerMeanShortOE.getInclination().inRadians() - expectedMeanElements[2]), 1e-5);
    EXPECT_LT(std::abs(brouwerMeanShortOE.getRaan().inRadians() - expectedMeanElements[3]), 1e-5);
    EXPECT_LT(std::abs(brouwerMeanShortOE.getAop().inRadians() - expectedMeanElements[4]), 1e-5);
    EXPECT_LT(std::abs(brouwerMeanShortOE.getMeanAnomaly().inRadians() - expectedMeanElements[5]), 1e-5);
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort, ToCOE)
{
    {
        const BrouwerLyddaneMeanShort brouwerMeanShortOE = BrouwerLyddaneMeanShort({
            Length::Meters(6983041.66751785),
            0.001288522395593299,
            Angle::Degrees(97.80765762597238),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        });

        const COE coe = brouwerMeanShortOE.toCOE();

        EXPECT_NEAR(6992336.344873916, coe.getSemiMajorAxis().inMeters(), 1e-2);
        EXPECT_NEAR(0.001521740203201997, coe.getEccentricity(), 1e-5);
        EXPECT_NEAR(97.80243093402868, coe.getInclination().inDegrees(), 1e-5);
        EXPECT_NEAR(19.0652807832696, coe.getRaan().inDegrees(), 1e-5);
        EXPECT_NEAR(51.7953392456179, coe.getAop().inDegrees(), 1e-5);
        EXPECT_NEAR(308.1926015482581, coe.getMeanAnomaly().inDegrees(), 1e-5);
    }

    {
        const BrouwerLyddaneMeanShort brouwerMeanShortOE = BrouwerLyddaneMeanShort({
            Length::Meters(6983041.66751785),
            1.0,
            Angle::Degrees(179.0),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        });

        EXPECT_ANY_THROW(brouwerMeanShortOE.toCOE());
    }

    {
        const BrouwerLyddaneMeanShort brouwerMeanShortOE = BrouwerLyddaneMeanShort({
            Length::Meters(6178000.66751785),
            0.001288522395593299,
            Angle::Degrees(179.0),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        });

        testing::internal::CaptureStdout();

        brouwerMeanShortOE.toCOE();

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        const BrouwerLyddaneMeanShort brouwerMeanShortOE = BrouwerLyddaneMeanShort({
            Length::Meters(200000.66751785),
            0.001288522395593299,
            Angle::Degrees(181.0),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        });

        EXPECT_ANY_THROW(brouwerMeanShortOE.toCOE());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort, GetCartesianState)
{
    {
        const Length semiMajorAxis = Length::Kilometers(6973.743736075629);
        const Real eccentricity = 0.001202049190851806;
        const Angle inclination = Angle::Degrees(97.8128984109556);
        const Angle raan = Angle::Degrees(19.06531022901672);
        const Angle aop = Angle::Degrees(89.87616473223646);
        const Angle meanAnomaly = Angle::Degrees(270.112053453241);

        const BrouwerLyddaneMeanShort brouwerLyddaneMeanShort = {
            semiMajorAxis,
            eccentricity,
            inclination,
            raan,
            aop,
            meanAnomaly,
        };

        Position position = Position::Undefined();
        Velocity velocity = Velocity::Undefined();
        std::tie(position, velocity) = brouwerLyddaneMeanShort.getCartesianState(
            EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF()
        );

        const Vector3d expectedPositionCoordinates = {6596065.624113991, 2282234.953292145, -18030.93992043403};
        const Vector3d expectedVelocityCoordinates = {345.4716519562875, -967.0404288726488, 7488.686029827191};

        for (Index i = 0; i < 3; ++i)
        {
            EXPECT_LT(std::abs(position.accessCoordinates()(i) - expectedPositionCoordinates[i]), 1e-2);
            EXPECT_LT(std::abs(velocity.accessCoordinates()(i) - expectedVelocityCoordinates[i]), 1e-2);
        }

        const BrouwerLyddaneMeanShort brouwerLyddaneMeanShortConverted = BrouwerLyddaneMeanShort::Cartesian(
            {position, velocity}, EarthGravitationalModel::EGM2008.gravitationalParameter_
        );

        EXPECT_NEAR(
            brouwerLyddaneMeanShort.getSemiMajorAxis().inMeters(),
            brouwerLyddaneMeanShortConverted.getSemiMajorAxis().inMeters(),
            1e-2
        );
        EXPECT_NEAR(
            brouwerLyddaneMeanShort.getEccentricity(), brouwerLyddaneMeanShortConverted.getEccentricity(), 1e-6
        );
        EXPECT_NEAR(
            brouwerLyddaneMeanShort.getInclination().inRadians(),
            brouwerLyddaneMeanShortConverted.getInclination().inRadians(),
            1e-6
        );
        EXPECT_NEAR(
            brouwerLyddaneMeanShort.getRaan().inRadians(), brouwerLyddaneMeanShortConverted.getRaan().inRadians(), 1e-6
        );
        EXPECT_NEAR(
            brouwerLyddaneMeanShort.getAop().inRadians(), brouwerLyddaneMeanShortConverted.getAop().inRadians(), 1e-6
        );
        EXPECT_NEAR(
            brouwerLyddaneMeanShort.getMeanAnomaly().inRadians(),
            brouwerLyddaneMeanShortConverted.getMeanAnomaly().inRadians(),
            1e-6
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanShort, Undefined)
{
    {
        EXPECT_NO_THROW(BrouwerLyddaneMeanShort::Undefined());
    }
}
