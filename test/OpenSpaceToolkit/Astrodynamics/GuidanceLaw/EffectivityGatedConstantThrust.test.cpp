/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityGatedConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Map;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::guidancelaw::EffectivityGatedConstantThrust;
using ostk::astrodynamics::guidancelaw::QLaw;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const COE currentCOE = {
            Length::Meters(7000.0e3),
            0.01,
            Angle::Degrees(0.05),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
        };

        const COE::CartesianState cartesianState =
            currentCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());

        position_ = cartesianState.first.getCoordinates();
        velocity_ = cartesianState.second.getCoordinates();
    }

    QLaw::Parameters makeParameters(const Real& relativeThreshold, const Real& absoluteThreshold) const
    {
        return QLaw::Parameters(
            {
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-4}},
                {COE::Element::Inclination, {1.0, 1e-4}},
                {COE::Element::Raan, {1.0, 1e-4}},
                {COE::Element::Aop, {1.0, 1e-4}},
            },
            3,
            4,
            2,
            0.01,
            100,
            1.0,
            Length::Kilometers(6578.0),
            absoluteThreshold,
            relativeThreshold
        );
    }

    Shared<const QLaw> makeQLaw(const Real& relativeThreshold, const Real& absoluteThreshold) const
    {
        return std::make_shared<const QLaw>(
            targetCOE_,
            gravitationalParameter_,
            makeParameters(relativeThreshold, absoluteThreshold),
            QLaw::COEDomain::Osculating,
            QLaw::GradientStrategy::Analytical
        );
    }

    const COE targetCOE_ = {
        Length::Meters(42000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const Derived gravitationalParameter_ = Derived(398600.49 * 1e9, Derived::Unit::MeterCubedPerSecondSquared());

    Vector3d position_ = Vector3d::Zero();
    Vector3d velocity_ = Vector3d::Zero();

    const Real thrustAcceleration_ = 1.0 / 300.0;

    const Vector3d tangential_ = {1.0, 0.0, 0.0};  // along-track in theta-R-H
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust, Constructor)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());

    EXPECT_NO_THROW(EffectivityGatedConstantThrust(tangential_, qlaw));

    // Zero-norm direction throws.
    EXPECT_THROW(
        EffectivityGatedConstantThrust(Vector3d::Zero(), qlaw), ostk::core::error::runtime::Undefined
    );

    // Null QLaw throws.
    EXPECT_THROW(
        EffectivityGatedConstantThrust(tangential_, nullptr), ostk::core::error::runtime::Undefined
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust, StreamOperator)
{
    const EffectivityGatedConstantThrust law = {tangential_, makeQLaw(Real::Undefined(), Real::Undefined())};

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(std::cout << law << std::endl);
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust, Print)
{
    const EffectivityGatedConstantThrust law = {tangential_, makeQLaw(Real::Undefined(), Real::Undefined())};

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(law.print(std::cout, true));
    EXPECT_NO_THROW(law.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust, Getters)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());

    // Non-unit input is normalized on construction.
    const Vector3d nonUnit = {0.0, 2.0, 0.0};
    const EffectivityGatedConstantThrust law = {nonUnit, qlaw};

    const Vector3d stored = law.getThrustDirectionThetaRH();
    EXPECT_NEAR(stored.norm(), 1.0, 1e-12);
    EXPECT_NEAR(stored[0], 0.0, 1e-12);
    EXPECT_NEAR(stored[1], 1.0, 1e-12);
    EXPECT_NEAR(stored[2], 0.0, 1e-12);

    EXPECT_EQ(law.accessQLaw(), qlaw);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust, CalculateThrustAccelerationAt_Ungated)
{
    // With neither threshold defined, the gate short-circuits and behaves like a bare constant-thrust
    // rotation from theta-R-H to GCRF.
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const EffectivityGatedConstantThrust law = {tangential_, qlaw};

    const Vector3d acceleration = law.calculateThrustAccelerationAt(
        Instant::J2000(), position_, velocity_, thrustAcceleration_, Frame::GCRF()
    );

    const Matrix3d R = QLaw::ThetaRHToGCRF(position_, velocity_);
    const Vector3d expected = thrustAcceleration_ * (R * tangential_);

    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(acceleration(i), expected(i), 1e-12);
    }

    EXPECT_GT(acceleration.norm(), 0.0);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust, CalculateThrustAccelerationAt_GatePassesWithPermissiveThresholds)
{
    // Very permissive thresholds (0.0) — any non-pathological direction passes.
    const Shared<const QLaw> qlaw = makeQLaw(Real(0.0), Real(0.0));
    const EffectivityGatedConstantThrust law = {tangential_, qlaw};

    const Vector3d acceleration = law.calculateThrustAccelerationAt(
        Instant::J2000(), position_, velocity_, thrustAcceleration_, Frame::GCRF()
    );

    EXPECT_GT(acceleration.norm(), 0.0);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust, CalculateThrustAccelerationAt_GateRejectsAntiOptimal)
{
    // Anti-tangential: dQdt_best >= 0 (direction never reduces Q); eta_abs is guarded to 0. Even a
    // very permissive absolute threshold (> 0) rejects; and eta_rel = 0 rejects any relative
    // threshold > 0.
    const Shared<const QLaw> qlaw = makeQLaw(Real(0.1), Real(0.1));
    const EffectivityGatedConstantThrust law = {-tangential_, qlaw};

    const Vector3d acceleration = law.calculateThrustAccelerationAt(
        Instant::J2000(), position_, velocity_, thrustAcceleration_, Frame::GCRF()
    );

    EXPECT_EQ(acceleration, Vector3d::Zero());
}
