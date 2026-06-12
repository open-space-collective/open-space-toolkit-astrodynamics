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

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/InTrack.hpp>
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

using ostk::astrodynamics::guidancelaw::InTrack;
using ostk::astrodynamics::guidancelaw::QLaw;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const COE lowCOE = {
            Length::Meters(7000.0e3),
            0.01,
            Angle::Degrees(0.05),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
        };

        const COE::CartesianState lowCartesianState = lowCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());
        positionLow_ = lowCartesianState.first.getCoordinates();
        velocityLow_ = lowCartesianState.second.getCoordinates();

        // True anomaly chosen off the QLaw sample grid (linspace(0, 2π, 50)) so that the
        // direction-aware effectivity at the current state is strictly between min and max
        // sample values. This is needed to test rejection at threshold = 1.0.
        const COE offSampleCOE = {
            Length::Meters(7000.0e3),
            0.01,
            Angle::Degrees(0.05),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Radians(0.5),
        };

        const COE::CartesianState offSampleCartesianState =
            offSampleCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());
        positionOffSample_ = offSampleCartesianState.first.getCoordinates();
        velocityOffSample_ = offSampleCartesianState.second.getCoordinates();
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

    Shared<const QLaw> makeQLaw(const COE& aTargetCOE, const Real& relativeThreshold, const Real& absoluteThreshold)
        const
    {
        return std::make_shared<const QLaw>(
            aTargetCOE,
            gravitationalParameter_,
            makeParameters(relativeThreshold, absoluteThreshold),
            QLaw::COEDomain::Osculating,
            QLaw::GradientStrategy::Analytical
        );
    }

    Shared<const QLaw> makeRaisingQLaw(const Real& relativeThreshold, const Real& absoluteThreshold) const
    {
        // Target above the low state (7 000 → 42 000 km).
        return makeQLaw(highTargetCOE_, relativeThreshold, absoluteThreshold);
    }

    const COE highTargetCOE_ = {
        Length::Meters(42000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const Derived gravitationalParameter_ = Derived(398600.49 * 1e9, Derived::Unit::MeterCubedPerSecondSquared());

    Vector3d positionLow_ = Vector3d::Zero();
    Vector3d velocityLow_ = Vector3d::Zero();
    Vector3d positionOffSample_ = Vector3d::Zero();
    Vector3d velocityOffSample_ = Vector3d::Zero();

    const Real thrustAcceleration_ = 1.0 / 300.0;

    const Vector3d prograde_ = {1.0, 0.0, 0.0};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack, Constructor)
{
    const Shared<const QLaw> qlaw = makeRaisingQLaw(Real::Undefined(), Real::Undefined());

    EXPECT_NO_THROW(InTrack {qlaw});

    // Null QLaw throws.
    EXPECT_THROW(InTrack {Shared<const QLaw>(nullptr)}, ostk::core::error::runtime::Undefined);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack, StreamOperator)
{
    const InTrack law = InTrack {makeRaisingQLaw(Real::Undefined(), Real::Undefined())};

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(std::cout << law << std::endl);
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack, Print)
{
    const InTrack law = InTrack {makeRaisingQLaw(Real::Undefined(), Real::Undefined())};

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(law.print(std::cout, true));
    EXPECT_NO_THROW(law.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack, Getters)
{
    const Shared<const QLaw> qlaw = makeRaisingQLaw(Real::Undefined(), Real::Undefined());
    const InTrack law = InTrack {qlaw};

    EXPECT_EQ(law.accessQLaw(), qlaw);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack, CalculateThrustAccelerationAt_AutoSelectsProgradeWhenRaising)
{
    // Low state (7 000 km), target above (42 000 km) → +along-track is the better candidate.
    const Shared<const QLaw> qlaw = makeRaisingQLaw(Real::Undefined(), Real::Undefined());
    const InTrack law = InTrack {qlaw};

    const Vector3d acceleration = law.calculateThrustAccelerationAt(
        Instant::J2000(), positionLow_, velocityLow_, thrustAcceleration_, Frame::GCRF()
    );

    const Matrix3d R = QLaw::ThetaRHToGCRF(positionLow_, velocityLow_);
    const Vector3d expected = thrustAcceleration_ * (R * prograde_);

    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(acceleration(i), expected(i), 1e-12);
    }

    EXPECT_NEAR(acceleration.norm(), thrustAcceleration_, 1e-12);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack, CalculateThrustAccelerationAt_IsAlongTrackOnly)
{
    // Whichever sign the law picks, the chosen direction must be ±theta_hat in theta-R-H, i.e.
    // the acceleration has zero radial and normal components when expressed in theta-R-H.
    const Shared<const QLaw> qlaw = makeRaisingQLaw(Real::Undefined(), Real::Undefined());
    const InTrack law = InTrack {qlaw};

    const Vector3d acceleration = law.calculateThrustAccelerationAt(
        Instant::J2000(), positionLow_, velocityLow_, thrustAcceleration_, Frame::GCRF()
    );

    const Matrix3d R = QLaw::ThetaRHToGCRF(positionLow_, velocityLow_);
    const Vector3d acceleration_thetaRH = R.transpose() * acceleration;

    EXPECT_NEAR(acceleration_thetaRH(1), 0.0, 1e-12);  // radial
    EXPECT_NEAR(acceleration_thetaRH(2), 0.0, 1e-12);  // normal
    EXPECT_NEAR(std::abs(acceleration_thetaRH(0)), thrustAcceleration_, 1e-12);
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack, CalculateThrustAccelerationAt_GatePassesWithPermissiveThresholds
)
{
    // Permissive thresholds (0.0). Auto-selected direction has etaRel ∈ [0.5, 1] so passes.
    const Shared<const QLaw> qlaw = makeRaisingQLaw(Real(0.0), Real(0.0));
    const InTrack law = InTrack {qlaw};

    const Vector3d acceleration = law.calculateThrustAccelerationAt(
        Instant::J2000(), positionLow_, velocityLow_, thrustAcceleration_, Frame::GCRF()
    );

    EXPECT_NEAR(acceleration.norm(), thrustAcceleration_, 1e-12);
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack, CalculateThrustAccelerationAt_GateRejectsWithStrictThresholds
)
{
    // Off-sample true anomaly (0.5 rad) ensures etaRel < 1.0 strictly. Threshold at the upper
    // bound (1.0) rejects.
    const Shared<const QLaw> qlaw = makeRaisingQLaw(Real(1.0), Real::Undefined());
    const InTrack law = InTrack {qlaw};

    const Vector3d acceleration = law.calculateThrustAccelerationAt(
        Instant::J2000(), positionOffSample_, velocityOffSample_, thrustAcceleration_, Frame::GCRF()
    );

    EXPECT_EQ(acceleration, Vector3d::Zero());
}
