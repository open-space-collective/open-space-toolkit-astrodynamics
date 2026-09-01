/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityProvider_Composite__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{
namespace effectivityprovider
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::Vector3d;

using ostk::astrodynamics::guidancelaw::EffectivityProvider;
using ostk::astrodynamics::trajectory::State;

/// @brief Effectivity provider that combines several providers.
///
/// Gate semantics: AND. The composite passes only when *every* sub-provider passes — short-
/// circuits on the first failing gate.
///
/// Score semantics: the score comes from a single nominated provider. By default, that's the
/// first provider in the array; pass `aPrimaryIndex` to nominate another. If the primary returns
/// `Real::Undefined()`, the composite returns `Real::Undefined()` (it does not fall back through
/// the array — averaging or summing scores from heterogeneous providers is rarely meaningful, so
/// the user is expected to designate which provider supplies the ranking).
class Composite : public EffectivityProvider
{
   public:
    /// @brief Constructor.
    ///
    /// @param aProviders Sub-providers to compose. Must contain at least one entry; entries must
    ///    be non-null.
    /// @param aPrimaryIndex Index into `aProviders` of the provider supplying the score. Defaults
    ///    to 0. Must be a valid index into `aProviders`.
    explicit Composite(const Array<Shared<EffectivityProvider>>& aProviders, const Size& aPrimaryIndex = 0);

    /// @brief Destructor.
    virtual ~Composite() override;

    /// @brief Print the provider.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Access the held providers.
    ///
    /// @return The held providers (in construction order).
    const Array<Shared<EffectivityProvider>>& accessProviders() const;

    /// @brief Get the index of the provider that supplies the score.
    ///
    /// @return The primary index.
    Size getPrimaryIndex() const;

    /// @brief Gate: AND of the sub-providers' gates.
    virtual bool isEffective(
        const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration
    ) const override;

    /// @brief Score: delegated to the primary provider.
    virtual Real computeScore(
        const State& aState, const Vector3d& aThrustDirectionThetaRH, const Real& aThrustAcceleration
    ) const override;

    /// @brief Batched gate + score across sub-providers.
    ///
    /// Each sub-provider's batched `evaluate` is called once; gate decisions are AND-combined
    /// per direction, and scores are taken from the primary provider. Sub-providers that share
    /// per-state work across directions therefore amortise it once across the whole candidate
    /// set rather than once per direction.
    virtual Array<Evaluation> evaluate(
        const State& aState,
        const Array<Vector3d>& aThrustDirectionsThetaRH,
        const Real& aThrustAcceleration
    ) const override;

   private:
    Array<Shared<EffectivityProvider>> providers_;
    Size primaryIndex_;
};

}  // namespace effectivityprovider
}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
