/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedGuidanceLaw__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/EffectivityProvider.hpp>

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

using ostk::astrodynamics::GuidanceLaw;

/// @brief Generic decorator that gates any guidance law with any effectivity provider.
///
/// Per step:
///   1. Delegate to the base guidance law to compute the proposed thrust acceleration.
///   2. If the base law produced ~zero, propagate that.
///   3. Express the proposed direction in the theta-R-H (along-track, radial, normal) frame and
///      consult the effectivity provider's gate.
///   4. Pass through the base law's output if the gate passes; otherwise return zero.
///
/// Position/velocity inputs are taken to be GCRF (matching the convention of QLaw and the
/// existing `EffectivityGatedConstantThrust`/`InTrack` laws): theta-R-H is built from those
/// vectors and the provider sees a GCRF state.
class EffectivityGatedGuidanceLaw : public GuidanceLaw
{
   public:
    /// @brief Constructor.
    ///
    /// @param aBaseLawSPtr Base guidance law to gate. Must be non-null.
    /// @param aProviderSPtr Effectivity provider supplying the gate. Must be non-null.
    EffectivityGatedGuidanceLaw(
        const Shared<const GuidanceLaw>& aBaseLawSPtr, const Shared<EffectivityProvider>& aProviderSPtr
    );

    /// @brief Destructor.
    virtual ~EffectivityGatedGuidanceLaw() override;

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aLaw An EffectivityGatedGuidanceLaw.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const EffectivityGatedGuidanceLaw& aLaw);

    /// @brief Print the guidance law.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Access the wrapped base guidance law.
    const Shared<const GuidanceLaw>& accessBaseLaw() const;

    /// @brief Access the held effectivity provider.
    const Shared<EffectivityProvider>& accessProvider() const;

    /// @brief Compute the gated thrust acceleration.
    ///
    /// @param anInstant An instant.
    /// @param aPositionCoordinates The position coordinates (GCRF).
    /// @param aVelocityCoordinates The velocity coordinates (GCRF).
    /// @param aThrustAcceleration The proposed thrust acceleration magnitude.
    /// @param outputFrameSPtr The frame the acceleration is expressed in.
    /// @return The base law's output, or zero if the provider's gate fails.
    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override;

   private:
    Shared<const GuidanceLaw> baseLawSPtr_;
    Shared<EffectivityProvider> providerSPtr_;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
