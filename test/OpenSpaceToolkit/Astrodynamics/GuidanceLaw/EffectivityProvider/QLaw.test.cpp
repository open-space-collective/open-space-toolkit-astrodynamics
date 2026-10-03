/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Map;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::guidancelaw::QLaw;
using QLawProvider = ostk::astrodynamics::guidancelaw::effectivityprovider::QLaw;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw : public ::testing::Test
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
            Angle::Radians(0.5),  // off-sample → etaRel < 1.0 strictly
        };

        const COE::CartesianState cartesianState = lowCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());
        stateLow_ = State(Instant::J2000(), cartesianState.first, cartesianState.second);
    }

    QLaw::Parameters makeParameters(const Real& absoluteThreshold, const Real& relativeThreshold) const
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

    Shared<const QLaw> makeQLaw(const Real& absoluteThreshold, const Real& relativeThreshold) const
    {
        return std::make_shared<const QLaw>(
            highTargetCOE_,
            gravitationalParameter_,
            makeParameters(absoluteThreshold, relativeThreshold),
            QLaw::COEDomain::Osculating,
            QLaw::GradientStrategy::Analytical
        );
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

    State stateLow_ = State::Undefined();

    const Real thrustAcceleration_ = 1.0 / 300.0;

    const Vector3d prograde_ = {1.0, 0.0, 0.0};
    const Vector3d retrograde_ = {-1.0, 0.0, 0.0};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, Constructor_NullQLawThrows)
{
    EXPECT_THROW(QLawProvider {Shared<const QLaw>(nullptr)}, ostk::core::error::runtime::Undefined);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, Constructor_RejectsOutOfRangeThresholds)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());

    EXPECT_THROW(QLawProvider(qlaw, Real(-0.1), Real::Undefined()), ostk::core::error::RuntimeError);
    EXPECT_THROW(QLawProvider(qlaw, Real(1.1), Real::Undefined()), ostk::core::error::RuntimeError);
    EXPECT_THROW(QLawProvider(qlaw, Real::Undefined(), Real(-0.1)), ostk::core::error::RuntimeError);
    EXPECT_THROW(QLawProvider(qlaw, Real::Undefined(), Real(1.1)), ostk::core::error::RuntimeError);
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, Constructor_FallsBackToQLawParameterThresholds
)
{
    // Both thresholds undefined on the provider -> inherit from QLaw::Parameters.
    const Shared<const QLaw> qlaw = makeQLaw(Real(0.2), Real(0.3));
    const QLawProvider provider {qlaw};

    EXPECT_EQ(provider.getAbsoluteEffectivityThreshold(), Real(0.2));
    EXPECT_EQ(provider.getRelativeEffectivityThreshold(), Real(0.3));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, Constructor_DirectThresholdsTakePrecedence)
{
    // QLaw has no thresholds; provider supplies them directly.
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const QLawProvider provider {qlaw, Real(0.4), Real(0.5)};

    EXPECT_EQ(provider.getAbsoluteEffectivityThreshold(), Real(0.4));
    EXPECT_EQ(provider.getRelativeEffectivityThreshold(), Real(0.5));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, IsEffective_NoThresholdsAlwaysPasses)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const QLawProvider provider {qlaw};

    EXPECT_TRUE(provider.isEffective(stateLow_, prograde_, thrustAcceleration_));
    EXPECT_TRUE(provider.isEffective(stateLow_, retrograde_, thrustAcceleration_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, IsEffective_StrictThresholdRejects)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const QLawProvider provider {qlaw, Real::Undefined(), Real(1.0)};

    // Off-sample → etaRel < 1.0 strictly even for the better candidate.
    EXPECT_FALSE(provider.isEffective(stateLow_, prograde_, thrustAcceleration_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, ComputeScore_HigherForBetterDirection)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const QLawProvider provider {qlaw};

    // Raising orbit (target above current) → prograde score should beat retrograde.
    const Real progradeScore = provider.computeScore(stateLow_, prograde_, thrustAcceleration_);
    const Real retrogradeScore = provider.computeScore(stateLow_, retrograde_, thrustAcceleration_);

    EXPECT_TRUE(progradeScore.isDefined());
    EXPECT_TRUE(retrogradeScore.isDefined());
    EXPECT_GT(progradeScore, retrogradeScore);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, Evaluate_MatchesPerDirectionCalls)
{
    // Strict relative threshold so the gate result varies between the two candidate directions
    // (prograde passes, retrograde fails). Verifies that batched evaluate() returns the same
    // (score, effective) pair as the per-direction computeScore/isEffective entry points.
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const QLawProvider provider {qlaw, Real::Undefined(), Real(0.6)};

    const ostk::core::container::Array<Vector3d> directions = {prograde_, retrograde_};
    const auto evaluations = provider.evaluate(stateLow_, directions, thrustAcceleration_);

    ASSERT_EQ(evaluations.getSize(), 2u);

    const Real progradeScore = provider.computeScore(stateLow_, prograde_, thrustAcceleration_);
    const Real retrogradeScore = provider.computeScore(stateLow_, retrograde_, thrustAcceleration_);
    const bool progradeGate = provider.isEffective(stateLow_, prograde_, thrustAcceleration_);
    const bool retrogradeGate = provider.isEffective(stateLow_, retrograde_, thrustAcceleration_);

    EXPECT_DOUBLE_EQ(static_cast<double>(evaluations[0].score), static_cast<double>(progradeScore));
    EXPECT_DOUBLE_EQ(static_cast<double>(evaluations[1].score), static_cast<double>(retrogradeScore));
    EXPECT_EQ(evaluations[0].effective, progradeGate);
    EXPECT_EQ(evaluations[1].effective, retrogradeGate);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, Evaluate_EmptyDirectionsReturnsEmpty)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const QLawProvider provider {qlaw};

    const auto evaluations = provider.evaluate(stateLow_, {}, thrustAcceleration_);
    EXPECT_TRUE(evaluations.isEmpty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, AccessQLaw)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const QLawProvider provider {qlaw};

    EXPECT_EQ(provider.accessQLaw(), qlaw);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw, Print)
{
    const Shared<const QLaw> qlaw = makeQLaw(Real::Undefined(), Real::Undefined());
    const QLawProvider provider {qlaw, Real(0.1), Real(0.2)};

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(provider.print(std::cout, true));
    EXPECT_NO_THROW(provider.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}
