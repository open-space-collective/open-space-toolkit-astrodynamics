/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider__

#include <ostream>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::astrodynamics::trajectory::State;

/// @brief Abstract interface for an "effectivity" provider: a strategy that decides whether a
///    proposed thrust action at a given state should actually be applied (gate), and optionally
///    supplies a scalar score allowing guidance laws to rank alternative thrust directions.
///
/// Effectivity is intentionally generic: a provider may model a Lyapunov-based progress metric
/// (e.g. QLaw), eclipse/power constraints, custom unavailability windows, or any combination
/// thereof. Multiple providers can be combined via the @ref Composite provider.
///
/// All providers operate on a thrust direction expressed in the theta-R-H (along-track, radial,
/// normal) frame so that direction-aware (QLaw) and direction-agnostic (eclipse, power) providers
/// share the same signature.
class EffectivityProvider
{
   public:
    /// @brief Per-direction result of a batched provider query.
    ///
    /// Bundles the gate decision and the optional ranking score so that callers wanting both
    /// (e.g. `InTrack` selecting prograde vs. retrograde and then gating the winner) can amortise
    /// any per-state precomputation across the candidate set.
    struct Evaluation
    {
        Real score;      ///< Ranking score (higher = better). `Real::Undefined()` if the provider
                         ///< has no directional preference.
        bool effective;  ///< Gate decision for the corresponding direction.
    };

    /// @brief Constructor.
    ///
    /// @param aName A name.
    explicit EffectivityProvider(const String& aName);

    /// @brief Destructor.
    virtual ~EffectivityProvider();

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aProvider An EffectivityProvider.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const EffectivityProvider& aProvider);

    /// @brief Get the provider name.
    ///
    /// @return The provider name.
    String getName() const;

    /// @brief Print the provider.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Gate: should the proposed thrust action be applied at this state?
    ///
    /// @param aState The current state. Must be defined.
    /// @param aThrustDirectionThetaRH Proposed thrust direction in theta-R-H [θ, R, H]. Need not
    ///    be normalized; providers normalize internally as needed. Direction-agnostic providers
    ///    (eclipse, power) may ignore this argument.
    /// @param aThrustAcceleration Proposed thrust acceleration magnitude.
    /// @return True if the action should be applied, false to gate it out (zero thrust).
    virtual bool isEffective(
        const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration
    ) const = 0;

    /// @brief Optional scalar score for ranking candidate directions; higher is better.
    ///
    /// Used by guidance laws that pick between multiple candidate directions per step (e.g.
    /// `InTrack` selecting prograde vs. retrograde). Providers without an opinion on ranking
    /// should return Real::Undefined() (the default), which the consumer interprets as "no
    /// preference".
    ///
    /// @param aState The current state.
    /// @param aThrustDirectionThetaRH Proposed thrust direction in theta-R-H.
    /// @param aThrustAcceleration Proposed thrust acceleration magnitude.
    /// @return Scalar score (higher = better) or Real::Undefined() if the provider has no
    ///    directional ranking.
    virtual Real computeScore(
        const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration
    ) const;

    /// @brief Batched gate + score evaluation across multiple candidate directions.
    ///
    /// Equivalent to calling `computeScore` and `isEffective` once per direction, but allows
    /// providers whose per-direction queries share work (e.g. the QLaw provider's per-anomaly
    /// thrust-vector grid, which depends only on the state and acceleration) to amortise that
    /// work across the candidate set. The default implementation simply loops, preserving the
    /// per-direction semantics for providers without an override.
    ///
    /// @param aState The current state.
    /// @param aThrustDirectionsThetaRH Candidate thrust directions in theta-R-H. Each is
    ///    interpreted by the same rules as the single-direction `computeScore`/`isEffective`
    ///    overloads.
    /// @param aThrustAcceleration Proposed thrust acceleration magnitude.
    /// @return One `Evaluation` per input direction, in the same order.
    virtual Array<Evaluation> evaluate(
        const State& aState,
        const Array<Vector3d>& aThrustDirectionsThetaRH,
        const Real& aThrustAcceleration
    ) const;

   protected:
    const String name_;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
