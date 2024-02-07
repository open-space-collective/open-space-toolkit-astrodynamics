/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::Index;
using ostk::core::container::Tuple;

using ostk::mathematics::object::Vector6d;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::coord::Frame;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::trajectory::orbit::models::blm::BrouwerLyddaneMeanLong;
using ostk::astro::trajectory::orbit::models::kepler::COE;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanLong, Constructor)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle meanAnomaly = Angle::Degrees(40.0);

        EXPECT_NO_THROW(BrouwerLyddaneMeanLong(semiMajorAxis, eccentricity, inclination, raan, aop, meanAnomaly););
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanLong, ToBrouwerLyddaneMeanLong)
{
    {
        const COE coe = COE::FromSIVector(
            {
                6973741.699984478,
                0.001199999999844802,
                1.7064084094998029,
                0.3385938748853645,
                1.5707969061472655,
                4.7144827961294755,
            },
            COE::AnomalyType::Mean
        );

        const BrouwerLyddaneMeanLong blmloe = BrouwerLyddaneMeanLong::COE(coe);

        EXPECT_NEAR(6964438.543901746, blmloe.getSemiMajorAxis().inMeters(), 1e-2);
        EXPECT_NEAR(0.000473008790410975, blmloe.getEccentricity(), 1e-5);
        EXPECT_NEAR(1.7064996261909862, blmloe.getInclination().inRadians(0.0, Real::TwoPi()), 1e-5);
        EXPECT_NEAR(0.3385940722800471, blmloe.getRaan().inRadians(0.0, Real::TwoPi()), 1e-5);
        EXPECT_NEAR(3.028029950209483, blmloe.getAop().inRadians(0.0, Real::TwoPi()), 1e-5);
        EXPECT_NEAR(3.2572466384121426, blmloe.getMeanAnomaly().inRadians(0.0, Real::TwoPi()), 1e-5);
    }
}

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMean_Parametrized
    : public ::testing::TestWithParam<std::tuple<Position, Velocity, Vector6d>>
{
};

// Values taken from GMAT
INSTANTIATE_TEST_SUITE_P(
    Cartesian,
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMean_Parametrized,
    ::testing::Values(
        std::make_tuple(
            Position::Meters({853730.214, -382070.813, 6892445.528}, Frame::GCRF()),
            Velocity::MetersPerSecond({-3093.942450, -6913.357574, 0.000000}, Frame::GCRF()),
            Vector6d {
                6972365.96849981,
                0.001677656315248244,
                1.705584427014695,
                1.149997444146928,
                1.570796346108476,
                6.283185292358735
            }
        ),
        std::make_tuple(
            Position::Meters({6596407.223662058, 2281266.582975321, -10540.61521486086}, Frame::GCRF()),
            Velocity::MetersPerSecond({337.7269674273224, -969.7192552349448, 7488.702816619139}, Frame::GCRF()),
            Vector6d {
                6973743.602335568,
                5.774323043509002e-05,
                1.707157127936171,
                0.33275235532951913,
                1.5260845940226722,
                4.757975922389615
            }
        ),
        std::make_tuple(
            Position::Meters({6596065.624114551, 2282234.953292401, -18030.93992064121}, Frame::GCRF()),
            Velocity::MetersPerSecond({345.4716519563907, -967.0404288726759, 7488.686029827369}, Frame::GCRF()),
            Vector6d {
                6973743.586220524,
                5.773317909427908e-05,
                1.7071571201132782,
                0.33275215539294273,
                1.5261597937609102,
                4.7568179617632165,
            }
        )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMean_Parametrized, Cartesian)
{
    const auto& [position, velocity, expectedMeanElements] = GetParam();
    const BrouwerLyddaneMeanLong brouwerMeanLongOE = BrouwerLyddaneMeanLong::Cartesian(
        {position, velocity}, EarthGravitationalModel::EGM2008.gravitationalParameter_
    );

    EXPECT_LT(std::abs(brouwerMeanLongOE.getSemiMajorAxis().inMeters() - expectedMeanElements[0]), 1e-2);
    EXPECT_LT(std::abs(brouwerMeanLongOE.getEccentricity() - expectedMeanElements[1]), 1e-5);
    EXPECT_LT(std::abs(brouwerMeanLongOE.getInclination().inRadians() - expectedMeanElements[2]), 1e-5);
    EXPECT_LT(std::abs(brouwerMeanLongOE.getRaan().inRadians() - expectedMeanElements[3]), 1e-5);
    EXPECT_LT(std::abs(brouwerMeanLongOE.getAop().inRadians() - expectedMeanElements[4]), 1e-5);
    EXPECT_LT(std::abs(brouwerMeanLongOE.getMeanAnomaly().inRadians() - expectedMeanElements[5]), 1e-5);
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanLong, Cartesian_EdgeCases)
{
    {
        const COE coe = {
            Length::Meters(6983041.66751785),
            1.0,
            Angle::Degrees(97.80765762597238),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        };

        EXPECT_ANY_THROW(BrouwerLyddaneMeanLong::Cartesian(
            coe.getCartesianState(EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF()),
            EarthGravitationalModel::EGM2008.gravitationalParameter_
        ));
    }

    {
        const COE coe = {
            Length::Meters(2000000.0),
            0.1,
            Angle::Degrees(97.80765762597238),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        };

        EXPECT_ANY_THROW(BrouwerLyddaneMeanLong::Cartesian(
            coe.getCartesianState(EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF()),
            EarthGravitationalModel::EGM2008.gravitationalParameter_
        ));
    }

    {
        const COE coe = {
            Length::Meters(5000000.0),
            0.1,
            Angle::Degrees(97.80765762597238),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        };

        testing::internal::CaptureStdout();

        BrouwerLyddaneMeanLong::Cartesian(
            coe.getCartesianState(EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF()),
            EarthGravitationalModel::EGM2008.gravitationalParameter_
        );

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

// Values taken from GMAT
TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanLong, ToCOE)
{
    {
        const BrouwerLyddaneMeanLong brouwerMeanLongOE = BrouwerLyddaneMeanLong({
            Length::Meters(6983041.66751785),
            0.001288522395593299,
            Angle::Degrees(97.80765762597238),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        });

        const COE coe = brouwerMeanLongOE.toCOE();

        EXPECT_NEAR(6992336.344873916, coe.getSemiMajorAxis().inMeters(), 1e-2);
        EXPECT_NEAR(0.002518523228303582, coe.getEccentricity(), 1e-6);
        EXPECT_NEAR(97.80244167380563, coe.getInclination().inDegrees(), 1e-6);
        EXPECT_NEAR(19.0652692183025, coe.getRaan().inDegrees(), 1e-6);
        EXPECT_NEAR(68.06017615493676, coe.getAop().inDegrees(), 1e-6);
        EXPECT_NEAR(291.9278181964628, coe.getMeanAnomaly().inDegrees(), 1e-6);
    }

    {
        const BrouwerLyddaneMeanLong brouwerMeanLongOE = BrouwerLyddaneMeanLong({
            Length::Meters(6983041.66751785),
            1.0,
            Angle::Degrees(179.0),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        });

        EXPECT_ANY_THROW(brouwerMeanLongOE.toCOE());
    }

    {
        const BrouwerLyddaneMeanLong brouwerMeanLongOE = BrouwerLyddaneMeanLong({
            Length::Meters(6178000.66751785),
            0.001288522395593299,
            Angle::Degrees(179.0),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        });

        testing::internal::CaptureStdout();

        brouwerMeanLongOE.toCOE();

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        const BrouwerLyddaneMeanLong brouwerMeanLongOE = BrouwerLyddaneMeanLong({
            Length::Meters(200000.66751785),
            0.001288522395593299,
            Angle::Degrees(181.0),
            Angle::Degrees(19.06529544678578),
            Angle::Degrees(68.50632506660459),
            Angle::Degrees(291.4817543658902),
        });

        EXPECT_ANY_THROW(brouwerMeanLongOE.toCOE());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanLong, GetCartesianState)
{
    {
        const Length semiMajorAxis = Length::Kilometers(6973.743586220524);
        const Real eccentricity = 5.773317909427908e-05;
        const Angle inclination = Angle::Degrees(97.81289794819898);
        const Angle raan = Angle::Degrees(19.06529412789695);
        const Angle aop = Angle::Degrees(87.4425150450563);
        const Angle meanAnomaly = Angle::Degrees(272.5455931210549);

        const BrouwerLyddaneMeanLong brouwerLyddaneMeanLong = {
            semiMajorAxis,
            eccentricity,
            inclination,
            raan,
            aop,
            meanAnomaly,
        };

        Position position = Position::Undefined();
        Velocity velocity = Velocity::Undefined();
        std::tie(position, velocity) = brouwerLyddaneMeanLong.getCartesianState(
            EarthGravitationalModel::EGM2008.gravitationalParameter_, Frame::GCRF()
        );

        const Vector3d expectedPositionCoordinates = {6596065.62408577, 2282234.953311206, -18030.93998841763};
        const Vector3d expectedVelocityCoordinates = {345.4716519748895, -967.0404288569806, 7488.686029841553};

        for (Index i = 0; i < 3; ++i)
        {
            EXPECT_LT(std::abs(position.accessCoordinates()(i) - expectedPositionCoordinates[i]), 1e-2);
            EXPECT_LT(std::abs(velocity.accessCoordinates()(i) - expectedVelocityCoordinates[i]), 1e-2);
        }

        const BrouwerLyddaneMeanLong brouwerLyddaneMeanLongConverted = BrouwerLyddaneMeanLong::Cartesian(
            {position, velocity}, EarthGravitationalModel::EGM2008.gravitationalParameter_
        );

        EXPECT_NEAR(
            brouwerLyddaneMeanLong.getSemiMajorAxis().inMeters(),
            brouwerLyddaneMeanLongConverted.getSemiMajorAxis().inMeters(),
            1e-2
        );
        EXPECT_NEAR(brouwerLyddaneMeanLong.getEccentricity(), brouwerLyddaneMeanLongConverted.getEccentricity(), 1e-6);
        EXPECT_NEAR(
            brouwerLyddaneMeanLong.getInclination().inRadians(),
            brouwerLyddaneMeanLongConverted.getInclination().inRadians(),
            1e-6
        );
        EXPECT_NEAR(
            brouwerLyddaneMeanLong.getRaan().inRadians(), brouwerLyddaneMeanLongConverted.getRaan().inRadians(), 1e-6
        );
        EXPECT_NEAR(
            brouwerLyddaneMeanLong.getAop().inRadians(), brouwerLyddaneMeanLongConverted.getAop().inRadians(), 1e-6
        );
        EXPECT_NEAR(
            brouwerLyddaneMeanLong.getMeanAnomaly().inRadians(),
            brouwerLyddaneMeanLongConverted.getMeanAnomaly().inRadians(),
            1e-6
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BrouwerLyddaneMeanLong, Undefined)
{
    {
        EXPECT_NO_THROW(BrouwerLyddaneMeanLong::Undefined());
    }
}
