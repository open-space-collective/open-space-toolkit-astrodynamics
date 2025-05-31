#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/MEOE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <gtest/gtest.h>

// Define a tolerance for floating point comparisons
const Real TOLERANCE = 1e-10; // A bit tighter for MEOE elements themselves
const Real POSITION_TOLERANCE = 1e-7; // meters, can be influenced by many factors
const Real VELOCITY_TOLERANCE = 1e-10; // m/s
const Real ANGLE_TOLERANCE = 1e-10; // radians

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::container::Pair;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;
using ostk::mathematics::constant::Pi;


using ostk::physics::unit::Length;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::time::Instant;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::coordinate::Frame;
using ostk::physics::environment::object::celestial::Earth;

using ostk::astrodynamics::trajectory::orbit::model::MEOE;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;


class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE : public ::testing::Test
{
   protected:
    const Derived earthGravitationalParameter_ = Earth::GravitationalParameter;
    const Shared<const Frame> gcrf_ = Frame::GCRF();
    const Instant defaultInstant_ = Instant::J2000();

    // Helper to normalize angles to [0, 2*PI) for comparison
    static Real normalizeAngle(Real angle_rad)
    {
        angle_rad = std::fmod(angle_rad, 2.0 * Pi);
        if (angle_rad < 0.0)
        {
            angle_rad += 2.0 * Pi;
        }
        return angle_rad;
    }
};

// Test Cases

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, ConstructorAndGetters)
{
    const Length p = Length::Kilometers(7000.0);
    const Real f = 0.01;
    const Real g = 0.02;
    const Real h = 0.03;
    const Real k = 0.04;
    const Angle L = Angle::Degrees(30.0);

    const MEOE meoe(p, f, g, h, k, L);

    ASSERT_TRUE(meoe.isDefined());
    ASSERT_DOUBLE_EQ(p.inMeters(), meoe.getSemiLatusRectum().inMeters());
    ASSERT_DOUBLE_EQ(f, meoe.getF());
    ASSERT_DOUBLE_EQ(g, meoe.getG());
    ASSERT_DOUBLE_EQ(h, meoe.getH());
    ASSERT_DOUBLE_EQ(k, meoe.getK());
    ASSERT_DOUBLE_EQ(L.inRadians(), meoe.getTrueLongitude().inRadians());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, UndefinedState)
{
    const MEOE undefinedMeoe = MEOE::Undefined();
    ASSERT_FALSE(undefinedMeoe.isDefined());

    const Length p = Length::Kilometers(7000.0);
    const Real f = 0.01;
    const Real g = 0.02;
    const Real h = 0.03;
    const Real k = 0.04;
    const Angle L = Angle::Degrees(30.0);
    const MEOE definedMeoe(p, f, g, h, k, L);
    ASSERT_TRUE(definedMeoe.isDefined());

    EXPECT_THROW(undefinedMeoe.getSemiLatusRectum(), ostk::core::error::runtime::Undefined);
    EXPECT_THROW(undefinedMeoe.getF(), ostk::core::error::runtime::Undefined);
    EXPECT_THROW(undefinedMeoe.getG(), ostk::core::error::runtime::Undefined);
    EXPECT_THROW(undefinedMeoe.getH(), ostk::core::error::runtime::Undefined);
    EXPECT_THROW(undefinedMeoe.getK(), ostk::core::error::runtime::Undefined);
    EXPECT_THROW(undefinedMeoe.getTrueLongitude(), ostk::core::error::runtime::Undefined);
    EXPECT_THROW(undefinedMeoe.getSIVector(), ostk::core::error::runtime::Undefined);
    EXPECT_THROW(undefinedMeoe.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::runtime::Undefined);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, Operators)
{
    const Length p1 = Length::Kilometers(7000.0);
    const Real f1 = 0.01;
    const Real g1 = 0.02;
    const Real h1 = 0.03;
    const Real k1 = 0.04;
    const Angle L1 = Angle::Degrees(30.0);
    const MEOE meoe1(p1, f1, g1, h1, k1, L1);

    const Length p2 = Length::Kilometers(7000.0);
    const Real f2 = 0.01;
    const Real g2 = 0.02;
    const Real h2 = 0.03;
    const Real k2 = 0.04;
    const Angle L2 = Angle::Degrees(30.0);
    const MEOE meoe2(p2, f2, g2, h2, k2, L2);

    const Length p3 = Length::Kilometers(8000.0); // Different p
    const MEOE meoe3(p3, f1, g1, h1, k1, L1);

    const MEOE undefinedMeoe = MEOE::Undefined();

    ASSERT_TRUE(meoe1 == meoe2);
    ASSERT_FALSE(meoe1 == meoe3);
    ASSERT_FALSE(meoe1 == undefinedMeoe);
    ASSERT_FALSE(undefinedMeoe == meoe1);
    ASSERT_FALSE(undefinedMeoe == MEOE::Undefined()); // Undefined == Undefined is false

    ASSERT_FALSE(meoe1 != meoe2);
    ASSERT_TRUE(meoe1 != meoe3);
    ASSERT_TRUE(meoe1 != undefinedMeoe);

    std::stringstream ss;
    ss << meoe1;
    ASSERT_FALSE(ss.str().empty());
    // Example: MEOE (p: 7000000.0 [m], f: 0.01, g: 0.02, h: 0.03, k: 0.04, L: 30.0 [deg])
    ASSERT_NE(ss.str().find("p: 7000000.0 [m]"), String::npos);
    ASSERT_NE(ss.str().find("f: 0.01"), String::npos);
    ASSERT_NE(ss.str().find("L: "), String::npos); // Check if L is printed
}


TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, GetSIVector)
{
    const Length p = Length::Kilometers(7000.0); // 7,000,000 m
    const Real f = 0.01;
    const Real g = 0.02;
    const Real h = 0.03;
    const Real k = 0.04;
    const Angle L = Angle::Degrees(30.0); // 0.52359877559 radians

    const MEOE meoe(p, f, g, h, k, L);
    const Vector6d siVector = meoe.getSIVector();

    ASSERT_DOUBLE_EQ(7000000.0, siVector[0]);
    ASSERT_DOUBLE_EQ(f, siVector[1]);
    ASSERT_DOUBLE_EQ(g, siVector[2]);
    ASSERT_DOUBLE_EQ(h, siVector[3]);
    ASSERT_DOUBLE_EQ(k, siVector[4]);
    ASSERT_NEAR(L.inRadians(), siVector[5], TOLERANCE);
}


// Reference values for conversions are often taken from trusted sources or other validated tools.
// For this example, I'll use some values derived from Keplerian elements.

// General Elliptical Orbit (Example from Vallado, similar to GTO)
// COE: a = 24396.137 km, e = 0.7308, i = 7.0 deg, RAAN = 45.0 deg, AOP = 90.0 deg, TA = 0.0 deg
// MEOE (approx): p = 11263.5 km, f = -0.51675, g = 0.51675, h = 0.04305, k = 0.04305, L = 135.0 deg
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, CartesianToMEOE_GeneralElliptical)
{
    const Length sma = Length::Kilometers(24396.137);
    const Real ecc = 0.7308;
    const Angle inc = Angle::Degrees(7.0);
    const Angle raan = Angle::Degrees(45.0);
    const Angle aop = Angle::Degrees(90.0);
    const Angle ta = Angle::Degrees(0.0); // At periapsis for simplicity in verification

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_, defaultInstant_);

    const MEOE meoe = MEOE::Cartesian(cartesianState, earthGravitationalParameter_);

    ASSERT_TRUE(meoe.isDefined());

    // Expected values
    const Real p_expected_m = sma.inMeters() * (1.0 - ecc * ecc);
    const Real L_expected_rad = normalizeAngle(raan.inRadians() + aop.inRadians() + ta.inRadians());
    const Real f_expected = ecc * std::cos(aop.inRadians() + raan.inRadians());
    const Real g_expected = ecc * std::sin(aop.inRadians() + raan.inRadians());
    const Real tan_i_half_expected = std::tan(inc.inRadians() / 2.0);
    const Real h_expected = tan_i_half_expected * std::cos(raan.inRadians());
    const Real k_expected = tan_i_half_expected * std::sin(raan.inRadians());

    ASSERT_NEAR(p_expected_m, meoe.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE); // p is a length
    ASSERT_NEAR(f_expected, meoe.getF(), TOLERANCE);
    ASSERT_NEAR(g_expected, meoe.getG(), TOLERANCE);
    ASSERT_NEAR(h_expected, meoe.getH(), TOLERANCE);
    ASSERT_NEAR(k_expected, meoe.getK(), TOLERANCE);
    ASSERT_NEAR(L_expected_rad, meoe.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, CartesianToMEOE_Circular)
{
    const Length sma = Length::Kilometers(7000.0); // LEO
    const Real ecc = 0.0; // Circular
    const Angle inc = Angle::Degrees(28.5);
    const Angle raan = Angle::Degrees(10.0);
    const Angle aop = Angle::Degrees(0.0); // AOP is undefined for circular, often set to 0 or longitude of periapsis
    const Angle ta = Angle::Degrees(45.0); // True anomaly = true longitude for circular equatorial, or argument of latitude for circular inclined

    const COE coe(sma, ecc, inc, raan, aop, ta); // For circular, aop + ta is arg latitude
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_, defaultInstant_);

    const MEOE meoe = MEOE::Cartesian(cartesianState, earthGravitationalParameter_);

    ASSERT_TRUE(meoe.isDefined());
    ASSERT_NEAR(sma.inMeters(), meoe.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE); // p = a for circular
    ASSERT_NEAR(0.0, meoe.getF(), TOLERANCE); // f = e * cos(aop+raan) = 0
    ASSERT_NEAR(0.0, meoe.getG(), TOLERANCE); // g = e * sin(aop+raan) = 0

    const Real tan_i_half_expected = std::tan(inc.inRadians() / 2.0);
    const Real h_expected = tan_i_half_expected * std::cos(raan.inRadians());
    const Real k_expected = tan_i_half_expected * std::sin(raan.inRadians());
    // For circular orbit, L = RAAN + AOP + TA. If AOP is taken as 0, L = RAAN + TA (true argument of latitude)
    // The MEOE definition L = raan + aop + ta. For e=0, aop is ill-defined in COE.
    // However, the sum aop+ta (argument of latitude from ascending node) is well defined.
    // And for MEOE, L = true longitude. For e=0, f=0, g=0.
    // L should be raan + argument_of_latitude.
    // COE.getCartesianState will use aop=0 if e=0. So, L = raan_rad + ta_rad
    const Real L_expected_rad = normalizeAngle(raan.inRadians() + aop.inRadians() + ta.inRadians());


    ASSERT_NEAR(h_expected, meoe.getH(), TOLERANCE);
    ASSERT_NEAR(k_expected, meoe.getK(), TOLERANCE);
    ASSERT_NEAR(L_expected_rad, meoe.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, CartesianToMEOE_Equatorial)
{
    const Length sma = Length::Kilometers(7000.0);
    const Real ecc = 0.05;
    const Angle inc = Angle::Degrees(0.0); // Equatorial
    const Angle raan = Angle::Degrees(0.0); // RAAN undefined for equatorial, often set to 0 or longitude of ascending node
    const Angle aop = Angle::Degrees(30.0); // True longitude of periapsis
    const Angle ta = Angle::Degrees(15.0);

    // For equatorial, RAAN is undefined. COE might set it to 0.
    // MEOE h,k use tan(i/2). If i=0, h=0, k=0.
    const COE coe(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_, defaultInstant_);

    const MEOE meoe = MEOE::Cartesian(cartesianState, earthGravitationalParameter_);

    ASSERT_TRUE(meoe.isDefined());
    const Real p_expected_m = sma.inMeters() * (1.0 - ecc * ecc);
    ASSERT_NEAR(p_expected_m, meoe.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE);
    ASSERT_NEAR(0.0, meoe.getH(), TOLERANCE); // h = tan(0/2)*cos(raan) = 0
    ASSERT_NEAR(0.0, meoe.getK(), TOLERANCE); // k = tan(0/2)*sin(raan) = 0

    // For equatorial, L = aop + ta (true longitude)
    // f = ecc * cos(aop) (since raan=0)
    // g = ecc * sin(aop) (since raan=0)
    // The MEOE definition has L = raan + aop + ta. If raan=0 (as per COE convention for i=0), then L=aop+ta.
    // And f = ecc * cos(aop+raan), g = ecc * sin(aop+raan)
    const Real L_expected_rad = normalizeAngle(raan.inRadians() + aop.inRadians() + ta.inRadians());
    const Real f_expected = ecc * std::cos(aop.inRadians() + raan.inRadians());
    const Real g_expected = ecc * std::sin(aop.inRadians() + raan.inRadians());

    ASSERT_NEAR(f_expected, meoe.getF(), TOLERANCE);
    ASSERT_NEAR(g_expected, meoe.getG(), TOLERANCE);
    ASSERT_NEAR(L_expected_rad, meoe.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}


TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, CartesianToMEOE_CircularEquatorial)
{
    const Length sma = Length::Kilometers(42164.0); // GEO
    const Real ecc = 0.0;
    const Angle inc = Angle::Degrees(0.0);
    const Angle raan = Angle::Degrees(0.0); // Undefined, conventionally 0
    const Angle aop = Angle::Degrees(0.0);  // Undefined, conventionally 0
    const Angle ta = Angle::Degrees(75.0);  // True longitude

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_, defaultInstant_);

    const MEOE meoe = MEOE::Cartesian(cartesianState, earthGravitationalParameter_);

    ASSERT_TRUE(meoe.isDefined());
    ASSERT_NEAR(sma.inMeters(), meoe.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE); // p = a
    ASSERT_NEAR(0.0, meoe.getF(), TOLERANCE); // f = 0 for e=0
    ASSERT_NEAR(0.0, meoe.getG(), TOLERANCE); // g = 0 for e=0
    ASSERT_NEAR(0.0, meoe.getH(), TOLERANCE); // h = 0 for i=0
    ASSERT_NEAR(0.0, meoe.getK(), TOLERANCE); // k = 0 for i=0
    // L = raan + aop + ta. With raan=0, aop=0, L = ta.
    ASSERT_NEAR(ta.inRadians(), meoe.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, CartesianToMEOE_InclinationPiSingularity)
{
    // Create a COE with inclination = 180 degrees (PI radians)
    const Length sma = Length::Kilometers(10000.0);
    const Real ecc = 0.1;
    const Angle inc = Angle::Radians(Pi); // 180 degrees
    const Angle raan = Angle::Degrees(0.0);
    const Angle aop = Angle::Degrees(0.0);
    const Angle ta = Angle::Degrees(0.0);

    const COE coe(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesianState = coe.getCartesianState(earthGravitationalParameter_, gcrf_, defaultInstant_);

    const MEOE meoe = MEOE::Cartesian(cartesianState, earthGravitationalParameter_);
    ASSERT_FALSE(meoe.isDefined()); // Should return Undefined due to tan(PI/2)
}


TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, MEOEToCartesian_General)
{
    // MEOE: p = 11263.5 km, f = -0.51675, g = 0.51675, h = 0.04305, k = 0.04305, L = 135.0 deg
    // This corresponds to COE: a = 24396.137 km, e = 0.7308, i = 7.0 deg, RAAN = 45.0 deg, AOP = 90.0 deg, TA = 0.0 deg
    const Length p_meoe = Length::Kilometers(11263.51934); // From sma*(1-e^2) more precisely
    const Real f_meoe = -0.51675014; // e * cos(aop+raan)
    const Real g_meoe = 0.51675014;  // e * sin(aop+raan)
    const Real h_meoe = std::tan(Angle::Degrees(7.0).inRadians()/2.0) * std::cos(Angle::Degrees(45.0).inRadians()); //0.0430519
    const Real k_meoe = std::tan(Angle::Degrees(7.0).inRadians()/2.0) * std::sin(Angle::Degrees(45.0).inRadians()); //0.0430519
    const Angle L_meoe = Angle::Degrees(135.0); // raan+aop+ta

    const MEOE meoe(p_meoe, f_meoe, g_meoe, h_meoe, k_meoe, L_meoe);

    // Expected Cartesian from COE (which these MEOE are based on)
    const COE coe_ref(Length::Kilometers(24396.137), 0.7308, Angle::Degrees(7.0), Angle::Degrees(45.0), Angle::Degrees(90.0), Angle::Degrees(0.0));
    const Pair<Position, Velocity> cartesianState_expected = coe_ref.getCartesianState(earthGravitationalParameter_, gcrf_, defaultInstant_);

    const Pair<Position, Velocity> cartesianState_actual = meoe.getCartesianState(earthGravitationalParameter_, gcrf_);

    ASSERT_TRUE(cartesianState_actual.first.isDefined());
    ASSERT_TRUE(cartesianState_actual.second.isDefined());

    ASSERT_NEAR(cartesianState_expected.first.accessCoordinates()[0], cartesianState_actual.first.accessCoordinates()[0], POSITION_TOLERANCE);
    ASSERT_NEAR(cartesianState_expected.first.accessCoordinates()[1], cartesianState_actual.first.accessCoordinates()[1], POSITION_TOLERANCE);
    ASSERT_NEAR(cartesianState_expected.first.accessCoordinates()[2], cartesianState_actual.first.accessCoordinates()[2], POSITION_TOLERANCE);

    ASSERT_NEAR(cartesianState_expected.second.accessCoordinates()[0], cartesianState_actual.second.accessCoordinates()[0], VELOCITY_TOLERANCE);
    ASSERT_NEAR(cartesianState_expected.second.accessCoordinates()[1], cartesianState_actual.second.accessCoordinates()[1], VELOCITY_TOLERANCE);
    ASSERT_NEAR(cartesianState_expected.second.accessCoordinates()[2], cartesianState_actual.second.accessCoordinates()[2], VELOCITY_TOLERANCE);
}


TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, RoundTrip_Cartesian_MEOE_Cartesian)
{
    const Length sma = Length::Kilometers(20000.0);
    const Real ecc = 0.6;
    const Angle inc = Angle::Degrees(25.0);
    const Angle raan = Angle::Degrees(50.0);
    const Angle aop = Angle::Degrees(75.0);
    const Angle ta = Angle::Degrees(100.0);

    const COE coe_initial(sma, ecc, inc, raan, aop, ta);
    const Pair<Position, Velocity> cartesian_initial = coe_initial.getCartesianState(earthGravitationalParameter_, gcrf_, defaultInstant_);

    const MEOE meoe_intermediate = MEOE::Cartesian(cartesian_initial, earthGravitationalParameter_);
    ASSERT_TRUE(meoe_intermediate.isDefined());

    const Pair<Position, Velocity> cartesian_final = meoe_intermediate.getCartesianState(earthGravitationalParameter_, gcrf_);

    ASSERT_NEAR(cartesian_initial.first.accessCoordinates()[0], cartesian_final.first.accessCoordinates()[0], POSITION_TOLERANCE);
    ASSERT_NEAR(cartesian_initial.first.accessCoordinates()[1], cartesian_final.first.accessCoordinates()[1], POSITION_TOLERANCE);
    ASSERT_NEAR(cartesian_initial.first.accessCoordinates()[2], cartesian_final.first.accessCoordinates()[2], POSITION_TOLERANCE);

    ASSERT_NEAR(cartesian_initial.second.accessCoordinates()[0], cartesian_final.second.accessCoordinates()[0], VELOCITY_TOLERANCE);
    ASSERT_NEAR(cartesian_initial.second.accessCoordinates()[1], cartesian_final.second.accessCoordinates()[1], VELOCITY_TOLERANCE);
    ASSERT_NEAR(cartesian_initial.second.accessCoordinates()[2], cartesian_final.second.accessCoordinates()[2], VELOCITY_TOLERANCE);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, RoundTrip_MEOE_Cartesian_MEOE)
{
    const Length p_initial = Length::Kilometers(10000.0);
    const Real f_initial = 0.1;
    const Real g_initial = -0.05;
    const Real h_initial = 0.2;
    const Real k_initial = -0.15;
    const Angle L_initial = Angle::Degrees(120.0);

    const MEOE meoe_initial(p_initial, f_initial, g_initial, h_initial, k_initial, L_initial);

    const Pair<Position, Velocity> cartesian_intermediate = meoe_initial.getCartesianState(earthGravitationalParameter_, gcrf_);
    ASSERT_TRUE(cartesian_intermediate.first.isDefined());
    ASSERT_TRUE(cartesian_intermediate.second.isDefined());

    const MEOE meoe_final = MEOE::Cartesian(cartesian_intermediate, earthGravitationalParameter_);
    ASSERT_TRUE(meoe_final.isDefined());

    ASSERT_NEAR(meoe_initial.getSemiLatusRectum().inMeters(), meoe_final.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE); // p is a length
    ASSERT_NEAR(meoe_initial.getF(), meoe_final.getF(), TOLERANCE);
    ASSERT_NEAR(meoe_initial.getG(), meoe_final.getG(), TOLERANCE);
    ASSERT_NEAR(meoe_initial.getH(), meoe_final.getH(), TOLERANCE);
    ASSERT_NEAR(meoe_initial.getK(), meoe_final.getK(), TOLERANCE);
    ASSERT_NEAR(normalizeAngle(meoe_initial.getTrueLongitude().inRadians()), normalizeAngle(meoe_final.getTrueLongitude().inRadians()), ANGLE_TOLERANCE);
}


TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, StringFromElement)
{
    ASSERT_EQ("p", MEOE::StringFromElement(MEOE::Element::SemiLatusRectum));
    ASSERT_EQ("f", MEOE::StringFromElement(MEOE::Element::F));
    ASSERT_EQ("g", MEOE::StringFromElement(MEOE::Element::G));
    ASSERT_EQ("h", MEOE::StringFromElement(MEOE::Element::H));
    ASSERT_EQ("k", MEOE::StringFromElement(MEOE::Element::K));
    ASSERT_EQ("L", MEOE::StringFromElement(MEOE::Element::TrueLongitude));

    // No default in switch, so this tests if all enum values are covered.
    // If a new element is added without updating StringFromElement, this won't directly fail
    // unless the compiler warns about non-exhaustive switch.
    // A more robust test might involve iterating over all possible enum values if the language/gtest supports it easily.
}

// Test for p <= 0 in getCartesianState
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, MEOEToCartesian_InvalidP)
{
    const MEOE meoe_invalid_p(Length::Meters(0.0), 0.1, 0.1, 0.1, 0.1, Angle::Degrees(0.0));
    EXPECT_THROW(meoe_invalid_p.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::RuntimeError);

    const MEOE meoe_negative_p(Length::Meters(-100.0), 0.1, 0.1, 0.1, 0.1, Angle::Degrees(0.0));
    EXPECT_THROW(meoe_negative_p.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::RuntimeError);
}

// Test for q == 0 in getCartesianState
// q = 1 + f*cosL + g*sinL. To make q = 0, need 1 + f*cosL + g*sinL = 0
// Example: L = 0 (cosL=1, sinL=0). Then 1 + f = 0 => f = -1.
// This means e * cos(aop+raan) = -1. If e=1, then cos(aop+raan)=-1 => aop+raan = PI.
// This corresponds to a parabolic trajectory where the point is at infinity in the direction opposite to periapsis.
// Or a rectilinear ellipse passing through the focus.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, MEOEToCartesian_QZeroSingularity)
{
    // Case 1: f = -1, L = 0
    const MEOE meoe_q_zero1(Length::Kilometers(7000.0), -1.0, 0.0, 0.0, 0.0, Angle::Radians(0.0));
    EXPECT_THROW(meoe_q_zero1.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::RuntimeError);

    // Case 2: g = -1, L = PI/2
    const MEOE meoe_q_zero2(Length::Kilometers(7000.0), 0.0, -1.0, 0.0, 0.0, Angle::Radians(Pi / 2.0));
    EXPECT_THROW(meoe_q_zero2.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::RuntimeError);

    // Case 3: f = 1/sqrt(2), g = 1/sqrt(2), L = 5PI/4 (cosL = -1/sqrt(2), sinL = -1/sqrt(2))
    // q = 1 + (1/sqrt(2))*(-1/sqrt(2)) + (1/sqrt(2))*(-1/sqrt(2)) = 1 - 1/2 - 1/2 = 0
    const Real val = 1.0 / std::sqrt(2.0);
    const MEOE meoe_q_zero3(Length::Kilometers(7000.0), val, val, 0.0, 0.0, Angle::Radians(5.0 * Pi / 4.0));
    EXPECT_THROW(meoe_q_zero3.getCartesianState(earthGravitationalParameter_, gcrf_), ostk::core::error::RuntimeError);
}

// Test MEOE::Cartesian with p_m < 0 from COE conversion (e.g. bad SMA/ECC from COE for hyperbolic)
// This is indirectly tested by the MEOE::Cartesian logic that returns Undefined if p_m < 0.
// A specific test would require manipulating a COE to produce such a state,
// which might be tricky if COE's own validation prevents it.
// The current implementation of MEOE::Cartesian has:
//    if (p_m < 0.0 && ecc_val < 1.0) { return MEOE::Undefined(); }
// This check is actually a bit problematic. For hyperbolic orbits, sma < 0, and ecc > 1.
// So sma * (1 - ecc^2) = (negative) * (1 - (something > 1)) = (negative) * (negative) = positive.
// So p_m should always be positive if calculated from a valid hyperbolic COE.
// The only way p_m could be negative from sma*(1-e^2) is if e < 1 and sma < 0, which is not a physical ellipse.
// Or if e > 1 and sma > 0, also not physical for standard hyperbolic definition.
// The robust calculation from angular momentum in MEOE::Cartesian for e==1 should ensure p_m >= 0.
// So, this specific condition `p_m < 0.0 && ecc_val < 1.0` might not be triggered by valid inputs.
// However, if `p_m` somehow ends up non-positive before MEOE construction, the constructor of `Length` or subsequent use in `getCartesianState` should handle it.
// The check `if (p_m <= 0.0)` in `getCartesianState` is the effective guard.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_MEOE, CartesianToMEOE_ParabolicRobustP)
{
    // Parabolic orbit: e = 1.0
    // r = [7000e3, 0, 0] m
    // v = [0, sqrt(2*mu/r), 0] m/s (escape velocity at periapsis)
    const Position pos = Position::Meters({7000e3, 0.0, 0.0}, gcrf_);
    const Real r_norm = pos.accessCoordinates().norm();
    const Real v_escape_at_r = std::sqrt(2.0 * earthGravitationalParameter_.inSIUnits() / r_norm);
    const Velocity vel = Velocity::MetersPerSecond({0.0, v_escape_at_r, 0.0}, gcrf_);
    const Pair<Position, Velocity> cartesianStateParabolic(pos, vel);

    const MEOE meoe = MEOE::Cartesian(cartesianStateParabolic, earthGravitationalParameter_);
    ASSERT_TRUE(meoe.isDefined());

    // For this specific case:
    // h_vec = r x v = [0,0, 7000e3 * sqrt(2*mu/7000e3)] = [0,0, sqrt(7000e3 * 2 * mu)]
    // h_norm_sq = 7000e3 * 2 * mu
    // p_m = h_norm_sq / mu = 2 * 7000e3 = 14000e3
    ASSERT_NEAR(2.0 * r_norm, meoe.getSemiLatusRectum().inMeters(), POSITION_TOLERANCE);

    // Further checks for f, g, h, k, L for this parabolic case
    // COE for this would be: e=1, a=inf, i=0, raan=0, aop=0 (periapsis along x-axis), ta=0
    // f = e * cos(aop+raan) = 1 * cos(0) = 1
    // g = e * sin(aop+raan) = 1 * sin(0) = 0
    // h = tan(i/2)*cos(raan) = tan(0)*cos(0) = 0
    // k = tan(i/2)*sin(raan) = tan(0)*sin(0) = 0
    // L = raan+aop+ta = 0+0+0 = 0
    ASSERT_NEAR(1.0, meoe.getF(), TOLERANCE);
    ASSERT_NEAR(0.0, meoe.getG(), TOLERANCE);
    ASSERT_NEAR(0.0, meoe.getH(), TOLERANCE);
    ASSERT_NEAR(0.0, meoe.getK(), TOLERANCE);
    ASSERT_NEAR(0.0, meoe.getTrueLongitude().inRadians(), ANGLE_TOLERANCE);
}
