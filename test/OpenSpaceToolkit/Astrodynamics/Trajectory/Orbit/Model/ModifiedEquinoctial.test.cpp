/// Apache License 2.0

#include <cmath>

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/ModifiedEquinoctial.hpp>

using ostk::core::container::Pair;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Time;

using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::kepler::ModifiedEquinoctial;

// Define a tolerance for floating point comparisons
const double TOLERANCE = 1e-8;            // A bit tighter for ModifiedEquinoctial elements themselves
const double POSITION_TOLERANCE = 1e-4;   // meters, can be influenced by many factors
const double VELOCITY_TOLERANCE = 1e-10;  // m/s
const double ANGLE_TOLERANCE = 1e-5;      // radians

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial : public ::testing::Test
{
   protected:
    const Derived earthGravitationalParameter_ = Earth::Default().getGravitationalParameter();
    const Shared<const Frame> gcrf_ = Frame::GCRF();
    const Instant defaultInstant_ = Instant::J2000();

    const Length semiLatusRectum_ = Length::Kilometers(7000.0);
    const double eccentricityX_ = 0.01;
    const double eccentricityY_ = 0.02;
    const double nodeX_ = 0.03;
    const double nodeY_ = 0.04;
    const Angle trueLongitude_ = Angle::Degrees(30.0);
    const ModifiedEquinoctial modifiedEquinoctial_ = {
        semiLatusRectum_, eccentricityX_, eccentricityY_, nodeX_, nodeY_, trueLongitude_
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Getters)
{
    {
        ASSERT_DOUBLE_EQ(semiLatusRectum_.inMeters(), modifiedEquinoctial_.getSemiLatusRectum().inMeters());
        ASSERT_DOUBLE_EQ(eccentricityX_, modifiedEquinoctial_.getEccentricityX());
        ASSERT_DOUBLE_EQ(eccentricityY_, modifiedEquinoctial_.getEccentricityY());
        ASSERT_DOUBLE_EQ(nodeX_, modifiedEquinoctial_.getNodeX());
        ASSERT_DOUBLE_EQ(nodeY_, modifiedEquinoctial_.getNodeY());
        ASSERT_DOUBLE_EQ(trueLongitude_.inRadians(), modifiedEquinoctial_.getTrueLongitude().inRadians());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, GetCartesianState)
{
    {
        EXPECT_THROW(
            modifiedEquinoctial_.getCartesianState(earthGravitationalParameter_, Frame::ITRF()),
            ostk::core::error::runtime::Wrong
        );
    }

    {
        EXPECT_THROW(
            modifiedEquinoctial_.getCartesianState(earthGravitationalParameter_, nullptr),
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Operators)
{
    const Length p1 = Length::Kilometers(7000.0);
    const double f1 = 0.01;
    const double g1 = 0.02;
    const double h1 = 0.03;
    const double k1 = 0.04;
    const Angle L1 = Angle::Degrees(30.0);
    const ModifiedEquinoctial meoe1(p1, f1, g1, h1, k1, L1);

    const Length p2 = Length::Kilometers(7000.0);
    const double f2 = 0.01;
    const double g2 = 0.02;
    const double h2 = 0.03;
    const double k2 = 0.04;
    const Angle L2 = Angle::Degrees(30.0);
    const ModifiedEquinoctial meoe2(p2, f2, g2, h2, k2, L2);

    const Length p3 = Length::Kilometers(8000.0);  // Different p
    const ModifiedEquinoctial meoe3(p3, f1, g1, h1, k1, L1);

    ASSERT_TRUE(meoe1 == meoe2);
    ASSERT_FALSE(meoe1 == meoe3);

    ASSERT_FALSE(meoe1 != meoe2);
    ASSERT_TRUE(meoe1 != meoe3);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, GetSIVector)
{
    const Vector6d siVector = modifiedEquinoctial_.getSIVector();

    ASSERT_DOUBLE_EQ(semiLatusRectum_.inMeters(), siVector[0]);
    ASSERT_DOUBLE_EQ(eccentricityX_, siVector[1]);
    ASSERT_DOUBLE_EQ(eccentricityY_, siVector[2]);
    ASSERT_DOUBLE_EQ(nodeX_, siVector[3]);
    ASSERT_DOUBLE_EQ(nodeY_, siVector[4]);
    EXPECT_NEAR(trueLongitude_.inRadians(), siVector[5], TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Cartesian)
{
    {
        const Position position = Position::Meters({1000000.0, 2000000.0, 3000000.0}, Frame::ITRF());
        const Velocity velocity = Velocity::MetersPerSecond({1.0, 2.0, 3.0}, Frame::ITRF());
        const Pair<Position, Velocity> cartesianState = {position, velocity};

        EXPECT_THROW(
            ModifiedEquinoctial::Cartesian(cartesianState, earthGravitationalParameter_),
            ostk::core::error::runtime::Wrong
        );
    }
}

// Test for q == 0 in getCartesianState
// q = 1 + f*cosL + g*sinL. To make q = 0, need 1 + f*cosL + g*sinL = 0
// Example: L = 0 (cosL=1, sinL=0). Then 1 + f = 0 => f = -1.
// This means e * cos(aop+raan) = -1. If e=1, then cos(aop+raan)=-1 => aop+raan = PI.
// This corresponds to a parabolic trajectory where the point is at infinity in the direction opposite to periapsis.
// Or a rectilinear ellipse passing through the focus.
TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, GetCartesianState_QZeroSingularity
)
{
    // Case 1: f = -1, L = 0
    const ModifiedEquinoctial meoe_q_zero1(Length::Kilometers(7000.0), -1.0, 0.0, 0.0, 0.0, Angle::Radians(0.0));
    EXPECT_THROW(meoe_q_zero1.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::RuntimeError);

    // Case 2: g = -1, L = PI/2
    const ModifiedEquinoctial meoe_q_zero2(Length::Kilometers(7000.0), 0.0, -1.0, 0.0, 0.0, Angle::Radians(M_PI / 2.0));
    EXPECT_THROW(meoe_q_zero2.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::RuntimeError);

    // Case 3: f = 1/sqrt(2), g = 1/sqrt(2), L = 5PI/4 (cosL = -1/sqrt(2), sinL = -1/sqrt(2))
    // q = 1 + (1/sqrt(2))*(-1/sqrt(2)) + (1/sqrt(2))*(-1/sqrt(2)) = 1 - 1/2 - 1/2 = 0
    const double val = 1.0 / std::sqrt(2.0);
    const ModifiedEquinoctial meoe_q_zero3(
        Length::Kilometers(7000.0), val, val, 0.0, 0.0, Angle::Radians(5.0 * M_PI / 4.0)
    );
    EXPECT_THROW(meoe_q_zero3.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::RuntimeError);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Cartesian_GeneralElliptical)
{
    const Length sma = Length::Kilometers(24396.137);
    const double ecc = 0.7308;
    const Angle inc = Angle::Degrees(7.0);
    const Angle raan = Angle::Degrees(45.0);
    const Angle aop = Angle::Degrees(90.0);
    const Angle ta = Angle::Degrees(0.0);  // At periapsis for simplicity in verification

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_);

    const ModifiedEquinoctial modifiedEquinoctial =
        ModifiedEquinoctial::Cartesian(cartesianState, earthGravitationalParameter_);

    // Expected values
    const double p_expected_m = sma.inMeters() * (1.0 - ecc * ecc);
    const double L_expected_rad =
        Angle::Radians(raan.inRadians() + aop.inRadians() + ta.inRadians()).inRadians(0.0, 2.0 * M_PI);
    const double f_expected = ecc * std::cos(aop.inRadians() + raan.inRadians());
    const double g_expected = ecc * std::sin(aop.inRadians() + raan.inRadians());
    const double tan_i_half_expected = std::tan(inc.inRadians() / 2.0);
    const double h_expected = tan_i_half_expected * std::cos(raan.inRadians());
    const double k_expected = tan_i_half_expected * std::sin(raan.inRadians());

    EXPECT_NEAR(
        p_expected_m, modifiedEquinoctial.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE
    );  // p is a length
    EXPECT_NEAR(f_expected, modifiedEquinoctial.getEccentricityX(), TOLERANCE);
    EXPECT_NEAR(g_expected, modifiedEquinoctial.getEccentricityY(), TOLERANCE);
    EXPECT_NEAR(h_expected, modifiedEquinoctial.getNodeX(), TOLERANCE);
    EXPECT_NEAR(k_expected, modifiedEquinoctial.getNodeY(), TOLERANCE);
    EXPECT_NEAR(L_expected_rad, modifiedEquinoctial.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Cartesian_Circular)
{
    const Length sma = Length::Kilometers(7000.0);  // LEO
    const double ecc = 0.0;                         // Circular
    const Angle inc = Angle::Degrees(28.5);
    const Angle raan = Angle::Degrees(10.0);
    const Angle aop = Angle::Degrees(0.0);  // AOP is undefined for circular, often set to 0 or longitude of periapsis
    const Angle ta = Angle::Degrees(45.0
    );  // True anomaly = true longitude for circular equatorial, or argument of latitude for circular inclined

    const COE coe(sma, ecc, inc, raan, aop, ta);  // For circular, aop + ta is arg latitude
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_);

    const ModifiedEquinoctial modifiedEquinoctial =
        ModifiedEquinoctial::Cartesian(cartesianState, earthGravitationalParameter_);

    EXPECT_NEAR(
        sma.inMeters(), modifiedEquinoctial.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE
    );                                                                    // p = a for circular
    EXPECT_NEAR(0.0, modifiedEquinoctial.getEccentricityX(), TOLERANCE);  // f = e * cos(aop+raan) = 0
    EXPECT_NEAR(0.0, modifiedEquinoctial.getEccentricityY(), TOLERANCE);  // g = e * sin(aop+raan) = 0

    const double tan_i_half_expected = std::tan(inc.inRadians() / 2.0);
    const double h_expected = tan_i_half_expected * std::cos(raan.inRadians());
    const double k_expected = tan_i_half_expected * std::sin(raan.inRadians());
    // For circular orbit, L = RAAN + AOP + TA. If AOP is taken as 0, L = RAAN + TA (true argument of latitude)
    // The ModifiedEquinoctial definition L = raan + aop + ta. For e=0, aop is ill-defined in COE.
    // However, the sum aop+ta (argument of latitude from ascending node) is well defined.
    // And for ModifiedEquinoctial, L = true longitude. For e=0, f=0, g=0.
    // L should be raan + argument_of_latitude.
    // COE.getCartesianState will use aop=0 if e=0. So, L = raan_rad + ta_rad
    const double L_expected_rad =
        Angle::Radians(raan.inRadians() + aop.inRadians() + ta.inRadians()).inRadians(0.0, 2.0 * M_PI);

    EXPECT_NEAR(h_expected, modifiedEquinoctial.getNodeX(), TOLERANCE);
    EXPECT_NEAR(k_expected, modifiedEquinoctial.getNodeY(), TOLERANCE);
    EXPECT_NEAR(L_expected_rad, modifiedEquinoctial.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Cartesian_Equatorial)
{
    const Length sma = Length::Kilometers(7000.0);
    const double ecc = 0.05;
    const Angle inc = Angle::Degrees(0.0);  // Equatorial
    const Angle raan =
        Angle::Degrees(0.0);  // RAAN undefined for equatorial, often set to 0 or longitude of ascending node
    const Angle aop = Angle::Degrees(30.0);  // True longitude of periapsis
    const Angle ta = Angle::Degrees(15.0);

    // For equatorial, RAAN is undefined. COE might set it to 0.
    // ModifiedEquinoctial h,k use tan(i/2). If i=0, h=0, k=0.
    const COE coe(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_);

    const ModifiedEquinoctial modifiedEquinoctial =
        ModifiedEquinoctial::Cartesian(cartesianState, earthGravitationalParameter_);

    const double p_expected_m = sma.inMeters() * (1.0 - ecc * ecc);
    EXPECT_NEAR(p_expected_m, modifiedEquinoctial.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE);
    EXPECT_NEAR(0.0, modifiedEquinoctial.getNodeX(), TOLERANCE);  // h = tan(0/2)*cos(raan) = 0
    EXPECT_NEAR(0.0, modifiedEquinoctial.getNodeY(), TOLERANCE);  // k = tan(0/2)*sin(raan) = 0

    // For equatorial, L = aop + ta (true longitude)
    // f = ecc * cos(aop) (since raan=0)
    // g = ecc * sin(aop) (since raan=0)
    // The ModifiedEquinoctial definition has L = raan + aop + ta. If raan=0 (as per COE convention for i=0), then
    // L=aop+ta. And f = ecc * cos(aop+raan), g = ecc * sin(aop+raan)
    const double L_expected_rad =
        Angle::Radians(raan.inRadians() + aop.inRadians() + ta.inRadians()).inRadians(0.0, 2.0 * M_PI);
    const double f_expected = ecc * std::cos(aop.inRadians() + raan.inRadians());
    const double g_expected = ecc * std::sin(aop.inRadians() + raan.inRadians());

    EXPECT_NEAR(f_expected, modifiedEquinoctial.getEccentricityX(), TOLERANCE);
    EXPECT_NEAR(g_expected, modifiedEquinoctial.getEccentricityY(), TOLERANCE);
    EXPECT_NEAR(L_expected_rad, modifiedEquinoctial.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Cartesian_CircularEquatorial)
{
    const Length sma = Length::Kilometers(42164.0);  // GEO
    const double ecc = 0.0;
    const Angle inc = Angle::Degrees(0.0);
    const Angle raan = Angle::Degrees(0.0);  // Undefined, conventionally 0
    const Angle aop = Angle::Degrees(0.0);   // Undefined, conventionally 0
    const Angle ta = Angle::Degrees(75.0);   // True longitude

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_);

    const ModifiedEquinoctial modifiedEquinoctial =
        ModifiedEquinoctial::Cartesian(cartesianState, earthGravitationalParameter_);

    EXPECT_NEAR(sma.inMeters(), modifiedEquinoctial.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE);  // p = a
    EXPECT_NEAR(0.0, modifiedEquinoctial.getEccentricityX(), TOLERANCE);  // f = 0 for e=0
    EXPECT_NEAR(0.0, modifiedEquinoctial.getEccentricityY(), TOLERANCE);  // g = 0 for e=0
    EXPECT_NEAR(0.0, modifiedEquinoctial.getNodeX(), TOLERANCE);          // h = 0 for i=0
    EXPECT_NEAR(0.0, modifiedEquinoctial.getNodeY(), TOLERANCE);          // k = 0 for i=0
    // L = raan + aop + ta. With raan=0, aop=0, L = ta.
    EXPECT_NEAR(ta.inRadians(), modifiedEquinoctial.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Cartesian_InclinationPiSingularity
)
{
    // Create a COE with inclination = 180 degrees (PI radians)
    const Length sma = Length::Kilometers(10000.0);
    const double ecc = 0.1;
    const Angle inc = Angle::Radians(M_PI);  // 180 degrees
    const Angle raan = Angle::Degrees(0.0);
    const Angle aop = Angle::Degrees(0.0);
    const Angle ta = Angle::Degrees(0.0);

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_);

    // Retrograde equatorial state is a singularity of the modified equinoctial elements
    EXPECT_THROW(
        ModifiedEquinoctial::Cartesian(cartesianState, earthGravitationalParameter_), ostk::core::error::RuntimeError
    );
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, RoundTrip_Cartesian_MEOE_Cartesian
)
{
    const Position position = Position::Meters({-29536113.0, 30329259.0, -100125.0}, gcrf_);
    const Velocity velocity = Velocity::MetersPerSecond({-2194.0, -2141.0, -8.0}, gcrf_);

    const ModifiedEquinoctial modifiedEquinoctial =
        ModifiedEquinoctial::Cartesian({position, velocity}, earthGravitationalParameter_);

    const Pair<Position, Velocity> cartesianComputed =
        modifiedEquinoctial.getCartesianState(earthGravitationalParameter_, gcrf_);

    EXPECT_NEAR(position.getCoordinates()[0], cartesianComputed.first.getCoordinates()[0], POSITION_TOLERANCE);
    EXPECT_NEAR(position.getCoordinates()[1], cartesianComputed.first.getCoordinates()[1], POSITION_TOLERANCE);
    EXPECT_NEAR(position.getCoordinates()[2], cartesianComputed.first.getCoordinates()[2], POSITION_TOLERANCE);

    EXPECT_NEAR(velocity.getCoordinates()[0], cartesianComputed.second.getCoordinates()[0], VELOCITY_TOLERANCE);
    EXPECT_NEAR(velocity.getCoordinates()[1], cartesianComputed.second.getCoordinates()[1], VELOCITY_TOLERANCE);
    EXPECT_NEAR(velocity.getCoordinates()[2], cartesianComputed.second.getCoordinates()[2], VELOCITY_TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, RoundTrip_MEOE_Cartesian_MEOE)
{
    const Length p_initial = Length::Kilometers(10000.0);
    const double f_initial = 0.1;
    const double g_initial = -0.05;
    const double h_initial = 0.2;
    const double k_initial = -0.15;
    const Angle L_initial = Angle::Degrees(120.0);

    const ModifiedEquinoctial meoe_initial(p_initial, f_initial, g_initial, h_initial, k_initial, L_initial);

    const Pair<Position, Velocity> cartesian_intermediate =
        meoe_initial.getCartesianState(earthGravitationalParameter_, gcrf_);

    const ModifiedEquinoctial meoe_final =
        ModifiedEquinoctial::Cartesian(cartesian_intermediate, earthGravitationalParameter_);

    EXPECT_NEAR(
        meoe_initial.getSemiLatusRectum().inMeters(), meoe_final.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE
    );  // p is a length
    EXPECT_NEAR(meoe_initial.getEccentricityX(), meoe_final.getEccentricityX(), TOLERANCE);
    EXPECT_NEAR(meoe_initial.getEccentricityY(), meoe_final.getEccentricityY(), TOLERANCE);
    EXPECT_NEAR(meoe_initial.getNodeX(), meoe_final.getNodeX(), TOLERANCE);
    EXPECT_NEAR(meoe_initial.getNodeY(), meoe_final.getNodeY(), TOLERANCE);
    EXPECT_NEAR(
        meoe_initial.getTrueLongitude().inRadians(0.0, 2.0 * M_PI),
        meoe_final.getTrueLongitude().inRadians(0.0, 2.0 * M_PI),
        ANGLE_TOLERANCE
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, StringFromElement)
{
    ASSERT_EQ("SemiLatusRectum", ModifiedEquinoctial::StringFromElement(ModifiedEquinoctial::Element::SemiLatusRectum));
    ASSERT_EQ("EccentricityX", ModifiedEquinoctial::StringFromElement(ModifiedEquinoctial::Element::EccentricityX));
    ASSERT_EQ("EccentricityY", ModifiedEquinoctial::StringFromElement(ModifiedEquinoctial::Element::EccentricityY));
    ASSERT_EQ("NodeX", ModifiedEquinoctial::StringFromElement(ModifiedEquinoctial::Element::NodeX));
    ASSERT_EQ("NodeY", ModifiedEquinoctial::StringFromElement(ModifiedEquinoctial::Element::NodeY));
    ASSERT_EQ("TrueLongitude", ModifiedEquinoctial::StringFromElement(ModifiedEquinoctial::Element::TrueLongitude));
}

// Test based on Orekit testEquinoctialToEquinoctialEll
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, COE_OrekitElliptical)
{
    // Values from Orekit testEquinoctialToEquinoctialEll
    const Length sma = Length::Meters(42166712.0);
    const double ecc = 0.5;  // This is quite high eccentricity
    const double inc_rad = 2.0 * std::asin(std::sqrt((1.200e-04 * 1.200e-04 + 1.16e-04 * 1.16e-04) / 4.0));
    const Angle inc = Angle::Radians(inc_rad);
    const Angle raan = Angle::Radians(5.51473467358854);  // From Orekit test
    const Angle aop = Angle::Radians(-3.87224326008837);  // From Orekit test
    const Angle ta = Angle::Radians(3.65750858649982);    // From Orekit test

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const ModifiedEquinoctial meoe = ModifiedEquinoctial::COE(coe);

    // Expected values based on Orekit test
    const double p_expected = sma.inMeters() * (1.0 - ecc * ecc);
    const double f_expected = ecc * std::cos(aop.inRadians() + raan.inRadians());
    const double g_expected = ecc * std::sin(aop.inRadians() + raan.inRadians());
    const double h_expected = std::tan(inc.inRadians() / 2.0) * std::cos(raan.inRadians());
    const double k_expected = std::tan(inc.inRadians() / 2.0) * std::sin(raan.inRadians());
    const double L_expected =
        Angle::Radians(raan.inRadians() + aop.inRadians() + ta.inRadians()).inRadians(0.0, 2.0 * M_PI);

    EXPECT_NEAR(p_expected, meoe.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE);
    EXPECT_NEAR(f_expected, meoe.getEccentricityX(), TOLERANCE);
    EXPECT_NEAR(g_expected, meoe.getEccentricityY(), TOLERANCE);
    EXPECT_NEAR(h_expected, meoe.getNodeX(), TOLERANCE);
    EXPECT_NEAR(k_expected, meoe.getNodeY(), TOLERANCE);
    EXPECT_NEAR(L_expected, meoe.getTrueLongitude().inRadians(0.0, 2.0 * M_PI), ANGLE_TOLERANCE);
}

// Test based on Orekit testEquinoctialToEquinoctialCirc
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, COE_OrekitCircular)
{
    // Values from Orekit testEquinoctialToEquinoctialCirc
    const Length sma = Length::Meters(42166712.0);
    const double ecc = 0.1e-10;  // Nearly circular
    const double inc_rad = 2.0 * std::asin(std::sqrt((1.200e-04 * 1.200e-04 + 1.16e-04 * 1.16e-04) / 4.0));
    const Angle inc = Angle::Radians(inc_rad);
    const Angle raan = Angle::Radians(5.51473467358854);
    const Angle aop = Angle::Radians(-3.87224326008837);
    const Angle ta = Angle::Radians(3.65750858649982);

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const ModifiedEquinoctial meoe = ModifiedEquinoctial::COE(coe);

    // For nearly circular orbit
    EXPECT_NEAR(sma.inMeters(), meoe.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE);
    EXPECT_NEAR(0.0, meoe.getEccentricityX(), TOLERANCE);
    EXPECT_NEAR(0.0, meoe.getEccentricityY(), TOLERANCE);

    const double h_expected = std::tan(inc.inRadians() / 2.0) * std::cos(raan.inRadians());
    const double k_expected = std::tan(inc.inRadians() / 2.0) * std::sin(raan.inRadians());
    EXPECT_NEAR(h_expected, meoe.getNodeX(), TOLERANCE);
    EXPECT_NEAR(k_expected, meoe.getNodeY(), TOLERANCE);

    const double L_expected =
        Angle::Radians(raan.inRadians() + aop.inRadians() + ta.inRadians()).inRadians(0.0, 2.0 * M_PI);
    EXPECT_NEAR(L_expected, meoe.getTrueLongitude().inRadians(0.0, 2.0 * M_PI), ANGLE_TOLERANCE);
}

// Test based on Orekit testEquinoctialToCartesian
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, COE_OrekitCartesian)
{
    const double hx = 6.000000020892001E-5;
    const double hy = -5.8000000201956006E-5;
    const double ex = -7.9E-6;
    const double ey = 1.1E-4;
    const Length p = Length::Meters(4.216671148715116E7);
    const Angle l = Angle::Radians(5.299891200518183);

    const ModifiedEquinoctial meoe = ModifiedEquinoctial(p, ex, ey, hx, hy, l);

    // Convert to Cartesian and verify against Orekit expected values
    const Pair<Position, Velocity> cartesianState = meoe.getCartesianState(earthGravitationalParameter_, gcrf_);

    // Expected position from Orekit test
    const double expectedPosX = 23374566.86787335;
    const double expectedPosY = -35099891.435266934;
    const double expectedPosZ = -1500.5372312333;

    // Expected velocity from Orekit test
    const double expectedVelX = 2558.7095644069;
    const double expectedVelY = 1704.1585429854;
    const double expectedVelZ = 0.5013093399;

    EXPECT_NEAR(expectedPosX, cartesianState.first.getCoordinates()[0], POSITION_TOLERANCE);
    EXPECT_NEAR(expectedPosY, cartesianState.first.getCoordinates()[1], POSITION_TOLERANCE);
    EXPECT_NEAR(expectedPosZ, cartesianState.first.getCoordinates()[2], POSITION_TOLERANCE);

    EXPECT_NEAR(expectedVelX, cartesianState.second.getCoordinates()[0], VELOCITY_TOLERANCE);
    EXPECT_NEAR(expectedVelY, cartesianState.second.getCoordinates()[1], VELOCITY_TOLERANCE);
    EXPECT_NEAR(expectedVelZ, cartesianState.second.getCoordinates()[2], VELOCITY_TOLERANCE);
}

// Test based on Orekit testEquinoctialToKeplerian
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, COE_OrekitKeplerian)
{
    // Values from Orekit testEquinoctialToKeplerian
    const Length sma = Length::Meters(42166712.0);
    const double ecc = 0.110283316961361e-03;
    const double inc_rad = 0.166901168553917e-03;
    const Angle inc = Angle::Radians(inc_rad);
    const Angle raan = Angle::Radians(5.51473467358854);
    const Angle aop = Angle::Radians(-3.87224326008837);
    const Angle ta = Angle::Radians(3.65750858649982);

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const ModifiedEquinoctial meoe = ModifiedEquinoctial::COE(coe);

    // Convert back to COE and verify
    const Pair<Position, Velocity> cartesianState = meoe.getCartesianState(earthGravitationalParameter_, gcrf_);
    const COE coeFinal = COE::Cartesian(cartesianState, earthGravitationalParameter_);

    // Compare with original values
    EXPECT_NEAR(sma.inMeters(), coeFinal.getSemiMajorAxis().inMeters(), POSITION_TOLERANCE);
    EXPECT_NEAR(ecc, coeFinal.getEccentricity(), TOLERANCE);
    EXPECT_NEAR(inc.inRadians(), coeFinal.getInclination().inRadians(), ANGLE_TOLERANCE);

    // For angles, normalize and compare
    const double raan_normalized = Angle::Radians(raan.inRadians()).inRadians(0.0, 2.0 * M_PI);
    const double aop_normalized = Angle::Radians(aop.inRadians()).inRadians(0.0, 2.0 * M_PI);
    const double ta_normalized = Angle::Radians(ta.inRadians()).inRadians(0.0, 2.0 * M_PI);

    const double raan_final_normalized = Angle::Radians(coeFinal.getRaan().inRadians()).inRadians(0.0, 2.0 * M_PI);
    const double aop_final_normalized = Angle::Radians(coeFinal.getAop().inRadians()).inRadians(0.0, 2.0 * M_PI);
    const double ta_final_normalized = Angle::Radians(coeFinal.getTrueAnomaly().inRadians()).inRadians(0.0, 2.0 * M_PI);

    EXPECT_NEAR(raan_normalized, raan_final_normalized, ANGLE_TOLERANCE);
    EXPECT_NEAR(aop_normalized, aop_final_normalized, ANGLE_TOLERANCE);
    EXPECT_NEAR(ta_normalized, ta_final_normalized, ANGLE_TOLERANCE);
}

// Test for hyperbolic orbits (should throw)
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, COE_Hyperbolic)
{
    // Test with negative semi-major axis (hyperbolic)
    const Length sma = Length::Meters(-42166712.0);
    const double ecc = 1.9;
    const Angle inc = Angle::Degrees(0.5);
    const Angle raan = Angle::Degrees(0.01);
    const Angle aop = Angle::Degrees(-0.02);
    const Angle ta = Angle::Degrees(5.300);

    const COE coe(sma, ecc, inc, raan, aop, ta);

    // Hyperbolic orbits cannot be represented
    EXPECT_THROW(ModifiedEquinoctial::COE(coe), ostk::core::error::RuntimeError);
}

// Test for parabolic orbits (should throw)
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, COE_Parabolic)
{
    // Test with eccentricity = 1.0 (parabolic)
    const Length sma = Length::Meters(42166712.0);
    const double ecc = 1.0;
    const Angle inc = Angle::Degrees(45.0);
    const Angle raan = Angle::Degrees(0.0);
    const Angle aop = Angle::Degrees(90.0);
    const Angle ta = Angle::Degrees(0.0);

    const COE coe(sma, ecc, inc, raan, aop, ta);

    // Parabolic orbits cannot be represented
    EXPECT_THROW(ModifiedEquinoctial::COE(coe), ostk::core::error::RuntimeError);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, ToCOE)
{
    // general elliptical orbit
    {
        const Length sma = Length::Kilometers(24396.137);
        const double ecc = 0.7308;
        const Angle inc = Angle::Degrees(7.0);
        const Angle raan = Angle::Degrees(45.0);
        const Angle aop = Angle::Degrees(90.0);
        const Angle ta = Angle::Degrees(0.0);

        const COE originalCoe(sma, ecc, inc, raan, aop, ta);
        const ModifiedEquinoctial meoe = ModifiedEquinoctial::COE(originalCoe);

        const COE convertedCoe = meoe.toCOE(earthGravitationalParameter_);

        EXPECT_NEAR(sma.inMeters(), convertedCoe.getSemiMajorAxis().inMeters(), POSITION_TOLERANCE);
        EXPECT_NEAR(ecc, convertedCoe.getEccentricity(), TOLERANCE);
        EXPECT_NEAR(inc.inRadians(), convertedCoe.getInclination().inRadians(), ANGLE_TOLERANCE);

        EXPECT_NEAR(convertedCoe.getRaan().inRadians(), raan.inRadians(), ANGLE_TOLERANCE);
        EXPECT_NEAR(convertedCoe.getAop().inRadians(), aop.inRadians(), ANGLE_TOLERANCE);
        EXPECT_NEAR(convertedCoe.getTrueAnomaly().inRadians(), ta.inRadians(), ANGLE_TOLERANCE);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << modifiedEquinoctial_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Kepler_ModifiedEquinoctial, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(modifiedEquinoctial_.print(std::cout, true));
        EXPECT_NO_THROW(modifiedEquinoctial_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}
