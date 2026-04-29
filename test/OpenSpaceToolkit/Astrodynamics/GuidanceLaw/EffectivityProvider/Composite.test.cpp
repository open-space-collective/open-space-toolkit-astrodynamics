/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider/Composite.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Instant;

using ostk::astrodynamics::guidancelaw::EffectivityProvider;
using ostk::astrodynamics::guidancelaw::effectivityprovider::Composite;
using ostk::astrodynamics::trajectory::State;

namespace
{

/// @brief Test stub provider with configurable gate and score.
class StubProvider : public EffectivityProvider
{
   public:
    StubProvider(const String& aName, bool aGate, const Real& aScore)
        : EffectivityProvider(aName),
          gate_(aGate),
          score_(aScore)
    {
    }

    bool isEffective(const State&, const Vector3d&, const Real&) const override
    {
        return gate_;
    }

    Real computeScore(const State&, const Vector3d&, const Real&) const override
    {
        return score_;
    }

   private:
    bool gate_;
    Real score_;
};

State makeDummyState()
{
    return State(
        Instant::J2000(),
        Position::Meters({7000.0e3, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({0.0, 7500.0, 0.0}, Frame::GCRF())
    );
}

}  // namespace

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite : public ::testing::Test
{
   protected:
    const State state_ = makeDummyState();
    const Vector3d direction_ = {1.0, 0.0, 0.0};
    const Real thrustAcceleration_ = 1.0 / 300.0;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite, Constructor_RejectsEmpty)
{
    EXPECT_THROW(Composite {Array<Shared<EffectivityProvider>>::Empty()}, ostk::core::error::RuntimeError);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite, Constructor_RejectsNullSubProvider)
{
    const Array<Shared<EffectivityProvider>> providers = {
        std::make_shared<StubProvider>("a", true, Real(0.5)),
        nullptr,
    };

    EXPECT_THROW(Composite {providers}, ostk::core::error::runtime::Undefined);
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite, Constructor_RejectsOutOfRangePrimaryIndex
)
{
    const Array<Shared<EffectivityProvider>> providers = {
        std::make_shared<StubProvider>("a", true, Real(0.5)),
    };

    EXPECT_THROW(Composite(providers, 1), ostk::core::error::RuntimeError);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite, IsEffective_AndOfSubGates)
{
    const Array<Shared<EffectivityProvider>> allPass = {
        std::make_shared<StubProvider>("a", true, Real(0.5)),
        std::make_shared<StubProvider>("b", true, Real(0.7)),
    };
    const Composite passingComposite {allPass};
    EXPECT_TRUE(passingComposite.isEffective(state_, direction_, thrustAcceleration_));

    const Array<Shared<EffectivityProvider>> oneFails = {
        std::make_shared<StubProvider>("a", true, Real(0.5)),
        std::make_shared<StubProvider>("b", false, Real(0.7)),
    };
    const Composite failingComposite {oneFails};
    EXPECT_FALSE(failingComposite.isEffective(state_, direction_, thrustAcceleration_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite, ComputeScore_DelegatesToPrimary)
{
    const Array<Shared<EffectivityProvider>> providers = {
        std::make_shared<StubProvider>("a", true, Real(0.2)),
        std::make_shared<StubProvider>("b", true, Real(0.8)),
    };

    const Composite primaryFirst {providers};
    EXPECT_EQ(primaryFirst.computeScore(state_, direction_, thrustAcceleration_), Real(0.2));

    const Composite primarySecond {providers, 1};
    EXPECT_EQ(primarySecond.computeScore(state_, direction_, thrustAcceleration_), Real(0.8));
    EXPECT_EQ(primarySecond.getPrimaryIndex(), 1u);
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite,
    ComputeScore_PropagatesUndefinedFromPrimary
)
{
    const Array<Shared<EffectivityProvider>> providers = {
        std::make_shared<StubProvider>("a", true, Real::Undefined()),
        std::make_shared<StubProvider>("b", true, Real(0.8)),
    };

    // Primary defaults to index 0; its score is Undefined → composite returns Undefined (no
    // fallback through the array).
    const Composite composite {providers};
    EXPECT_FALSE(composite.computeScore(state_, direction_, thrustAcceleration_).isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite, Evaluate_AndsGatesAndUsesPrimaryScore)
{
    const Array<Shared<EffectivityProvider>> providers = {
        std::make_shared<StubProvider>("a", true, Real(0.2)),
        std::make_shared<StubProvider>("b", false, Real(0.8)),
    };
    const Composite composite {providers, 1};

    const Array<Vector3d> directions = {direction_, {-1.0, 0.0, 0.0}};
    const auto evaluations = composite.evaluate(state_, directions, thrustAcceleration_);

    ASSERT_EQ(evaluations.getSize(), 2u);
    // Sub-provider b's gate is false → composite gate is false for every direction.
    EXPECT_FALSE(evaluations[0].effective);
    EXPECT_FALSE(evaluations[1].effective);
    // Primary index = 1 → score is sub-provider b's (0.8) for every direction.
    EXPECT_EQ(evaluations[0].score, Real(0.8));
    EXPECT_EQ(evaluations[1].score, Real(0.8));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite, AccessProviders)
{
    const Array<Shared<EffectivityProvider>> providers = {
        std::make_shared<StubProvider>("a", true, Real(0.5)),
        std::make_shared<StubProvider>("b", true, Real(0.7)),
    };
    const Composite composite {providers};

    EXPECT_EQ(composite.accessProviders().getSize(), 2u);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite, Print)
{
    const Array<Shared<EffectivityProvider>> providers = {
        std::make_shared<StubProvider>("a", true, Real(0.5)),
    };
    const Composite composite {providers};

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(composite.print(std::cout, true));
    EXPECT_NO_THROW(composite.print(std::cout, false));
    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}
