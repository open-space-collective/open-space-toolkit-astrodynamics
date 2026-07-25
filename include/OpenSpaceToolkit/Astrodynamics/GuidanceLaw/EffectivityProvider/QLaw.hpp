/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_QLaw__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{
namespace effectivityprovider
{

using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::astrodynamics::guidancelaw::EffectivityProvider;
using ostk::astrodynamics::trajectory::State;

/// @brief Effectivity provider backed by a QLaw's direction-aware effectivity metric.
///
/// Wraps a `guidancelaw::QLaw` and exposes:
///   - **Gate**: passes when both the relative and absolute effectivity (where defined) clear
///     their thresholds for the proposed direction.
///   - **Score**: returns the relative effectivity for the proposed direction, suitable for
///     ranking direction candidates (higher is better).
///
/// Thresholds are supplied to the provider directly. As a transitional convenience, if both
/// thresholds are passed as `Real::Undefined()`, the provider falls back to thresholds carried
/// on `QLaw::Parameters` (deprecated path — see QLaw deprecation warning).
class QLaw : public EffectivityProvider
{
   public:
    /// @brief Constructor.
    ///
    /// @param aQLawSPtr QLaw whose direction-aware effectivity is consulted. Must be non-null.
    /// @param anAbsoluteEffectivityThreshold Optional absolute-effectivity threshold (∈ [0, 1]).
    ///    `Real::Undefined()` disables that gate.
    /// @param aRelativeEffectivityThreshold Optional relative-effectivity threshold (∈ [0, 1]).
    ///    `Real::Undefined()` disables that gate.
    QLaw(
        const Shared<const guidancelaw::QLaw>& aQLawSPtr,
        const Real& anAbsoluteEffectivityThreshold = Real::Undefined(),
        const Real& aRelativeEffectivityThreshold = Real::Undefined()
    );

    /// @brief Destructor.
    virtual ~QLaw() override;

    /// @brief Print the provider.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Access the held QLaw.
    ///
    /// @return Shared pointer to the held QLaw.
    const Shared<const guidancelaw::QLaw>& accessQLaw() const;

    /// @brief Get the absolute-effectivity threshold (or `Real::Undefined()` if disabled).
    Real getAbsoluteEffectivityThreshold() const;

    /// @brief Get the relative-effectivity threshold (or `Real::Undefined()` if disabled).
    Real getRelativeEffectivityThreshold() const;

    /// @brief Gate the proposed direction against the absolute/relative effectivity thresholds.
    ///
    /// @param aState State at which to evaluate effectivity.
    /// @param aThrustDirectionThetaRH Proposed direction in theta-R-H. Normalized internally.
    /// @param aThrustAcceleration Thrust acceleration magnitude.
    /// @return True if both defined thresholds are met (or none are defined); false otherwise.
    virtual bool isEffective(
        const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration
    ) const override;

    /// @brief Compute the relative effectivity of the proposed direction (higher = better).
    ///
    /// @param aState State at which to evaluate effectivity.
    /// @param aThrustDirectionThetaRH Proposed direction in theta-R-H. Normalized internally.
    /// @param aThrustAcceleration Thrust acceleration magnitude.
    /// @return Relative effectivity ∈ [0, 1].
    virtual Real computeScore(
        const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration
    ) const override;

    /// @brief Batched gate + score for several directions.
    ///
    /// Forwards to `guidancelaw::QLaw::computeEffectivities` so that the per-anomaly thrust-
    /// vector grid (the dominant cost) is computed once per state and reused across all
    /// candidate directions and across the gate query for the chosen direction. Used by
    /// `InTrack` to amortise prograde- and retrograde-scoring (and the post-pick gate check)
    /// across a single grid sweep.
    virtual Array<Evaluation> evaluate(
        const State& aState,
        const Array<Vector3d>& aThrustDirectionsThetaRH,
        const Real& aThrustAcceleration
    ) const override;

   private:
    Shared<const guidancelaw::QLaw> qLawSPtr_;
    Real absoluteEffectivityThreshold_;
    Real relativeEffectivityThreshold_;
};

}  // namespace effectivityprovider
}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
