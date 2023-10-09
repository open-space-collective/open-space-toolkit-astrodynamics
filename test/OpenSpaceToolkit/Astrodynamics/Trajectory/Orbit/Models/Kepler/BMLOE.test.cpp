/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/BMLOE.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::ctnr::Array;
using ostk::core::ctnr::Tuple;

using ostk::math::obj::Vector6d;

using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::coord::Frame;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Duration;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::units::Length;

using ostk::astro::trajectory::orbit::models::kepler::BMLOE;
using ostk::astro::trajectory::orbit::models::kepler::COE;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE, Constructor)
{
    {
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.1;
        const Angle inclination = Angle::Degrees(10.0);
        const Angle raan = Angle::Degrees(20.0);
        const Angle aop = Angle::Degrees(30.0);
        const Angle meanAnomaly = Angle::Degrees(40.0);

        EXPECT_NO_THROW(BMLOE(semiMajorAxis, eccentricity, inclination, raan, aop, meanAnomaly););
    }
}

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE_Parametrized
    : public ::testing::TestWithParam<std::tuple<Position, Velocity, Vector6d>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Cartesian,
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE_Parametrized,
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
            Position::Meters({3069727.653946085, 152795.0493150251, 6255299.697473039}, Frame::GCRF()),
            Velocity::MetersPerSecond({-6334.622885945246, -2650.157689208724, 3166.240597652546}, Frame::GCRF()),
            Vector6d {
                6973701.460024448,
                5.827419003295505e-05,
                1.704897179172191,
                0.3328377583409925,
                1.5915494309189535,
                5.825745573394756
            }
        )
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE_Parametrized, Cartesian)
{
    const auto& [position, velocity, expectedMeanElements] = GetParam();
    const BMLOE bmloe = BMLOE::Cartesian({position, velocity}, Earth::EGM2008.gravitationalParameter_);

    EXPECT_LT(std::abs(bmloe.getVector()[0] - expectedMeanElements[0]), 1.0);
    EXPECT_LT(std::abs(bmloe.getVector()[1] - expectedMeanElements[1]), 1e-2);
    EXPECT_LT(std::abs(bmloe.getVector()[2] - expectedMeanElements[2]), 1e-2);
    EXPECT_LT(std::abs(bmloe.getVector()[3] - expectedMeanElements[3]), 1e-2);
    EXPECT_LT(std::abs(bmloe.getVector()[4] - expectedMeanElements[4]), 1e-2);
    EXPECT_LT(std::abs(bmloe.getVector()[5] - expectedMeanElements[5]), 1e-2);
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Kepler_BMLOE, ToCOE)
{
    {
        const Length semiMajorAxis = Length::Meters(6972365.96849981);
        const Real eccentricity = 0.001677656315248244;
        const Angle inclination = Angle::Radians(1.705584427014695);
        const Angle raan = Angle::Radians(1.149997444146928);
        const Angle aop = Angle::Radians(1.570796346108476);
        const Angle meanAnomaly = Angle::Radians(6.283185292358735);

        const BMLOE bmloe = {
            semiMajorAxis,
            eccentricity,
            inclination,
            raan,
            aop,
            meanAnomaly,
        };

        const Vector6d coeVectorExpected = {
            6963000.047336794,
            0.001060006293925325,
            1.705675370985273,
            1.149997444147743,
            1.570796346211503,
            6.283185292246888,
        };

        EXPECT_TRUE((coeVectorExpected - bmloe.toCOE().getVector(COE::AnomalyType::Mean)).all() < 1e-15);
    }

    {
        const BMLOE bmloe = BMLOE::FromVector({
            6983041.676067771,
            0.00128990139546395,
            1.707065635206913,
            0.3327522117680815,
            1.196081507607826,
            5.083175854163714,
        });
    }
}
