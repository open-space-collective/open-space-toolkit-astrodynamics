/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_InTrack__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

/// @brief Apply thrust along ±along-track (prograde or retrograde) only, auto-selecting the
///    better of the two each step via an effectivity provider's score, and gating against the
///    same provider.
///
/// At each step the law evaluates the provider's score for both [+1, 0, 0] and [-1, 0, 0] in the
/// theta-R-H (along-track, radial, normal) frame and picks the candidate with the higher score
/// (ties default to prograde). The chosen direction is then gated against the provider; if it
/// fails, zero thrust is applied for that step. Otherwise, the chosen unit vector is rotated from
/// theta-R-H to GCRF and scaled by the requested thrust acceleration.
///
/// The provider must supply a directional score (i.e. `computeScore` must return a defined real
/// for the candidate directions); a score-less provider would leave InTrack unable to choose
/// between prograde and retrograde and is rejected at runtime.
class InTrack : public GuidanceLaw
{
   public:
    /// @brief Constructor (provider-based).
    ///
    /// @param aProviderSPtr Effectivity provider that supplies both the directional score (used
    ///    to pick prograde vs. retrograde) and the gate. Must be non-null and must produce a
    ///    defined score for the candidate directions.
    explicit InTrack(const Shared<EffectivityProvider>& aProviderSPtr);

    /// @brief Constructor (QLaw-based, kept for backwards compatibility).
    ///
    /// Equivalent to constructing an `effectivityprovider::QLaw` from `aQLawSPtr` (inheriting any
    /// thresholds set on its `Parameters`) and forwarding to the provider-based constructor. New
    /// code should prefer the provider-based form.
    ///
    /// @param aQLawSPtr QLaw used to compute effectivity and supply thresholds. Must be non-null.
    explicit InTrack(const Shared<const QLaw>& aQLawSPtr);

    /// @brief Destructor.
    virtual ~InTrack() override;

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aLaw An InTrack guidance law.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const InTrack& aLaw);

    /// @brief Print the guidance law.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Access the held effectivity provider.
    ///
    /// @return Shared pointer to the held provider.
    const Shared<EffectivityProvider>& accessProvider() const;

    /// @brief Access the held QLaw, if the provider wraps one (legacy accessor).
    ///
    /// Returns the QLaw embedded in an `effectivityprovider::QLaw` when present, otherwise a
    /// null shared pointer. Kept for backwards compatibility with the QLaw-based constructor;
    /// new code should prefer `accessProvider()`.
    ///
    /// @return Shared pointer to the held QLaw, or null if the held provider is not a QLaw
    ///    provider.
    Shared<const QLaw> accessQLaw() const;

    /// @brief Compute the thrust acceleration at the provided coordinates.
    ///
    /// Auto-selects between prograde and retrograde along-track based on the provider's score,
    /// then applies the provider's gate to the chosen direction. Returns
    /// aThrustAcceleration * R_thetaRH→GCRF * chosenDirection when the gate passes;
    /// Vector3d::Zero() otherwise.
    ///
    /// @param anInstant An instant.
    /// @param aPositionCoordinates The position coordinates.
    /// @param aVelocityCoordinates The velocity coordinates.
    /// @param aThrustAcceleration The thrust acceleration magnitude.
    /// @param outputFrameSPtr The frame in which the acceleration is expressed.
    /// @return The thrust acceleration in the output frame.
    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override;

   private:
    Shared<EffectivityProvider> providerSPtr_;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
