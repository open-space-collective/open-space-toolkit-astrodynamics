/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_EffectivityGatedConstantThrust__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
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

/// @brief Apply a constant thrust direction (fixed in the theta-R-H / along-track-radial-normal
///    frame) only when the held QLaw's effectivity thresholds are met for that direction.
///
/// Steering is the ConstantThrust pattern: the direction is a unit vector in theta-R-H, rotated to
/// the requested output frame each step. Gating reuses QLaw's direction-aware effectivity: at each
/// step, the held QLaw evaluates (η_rel, η_abs) for the constant direction against the current
/// state; if either defined threshold is missed, zero thrust is applied for that step.
///
/// The theta-R-H frame axis order is [θ (along-track), R (radial), H (normal)], matching
/// QLaw::ThetaRHToGCRF.
class EffectivityGatedConstantThrust : public GuidanceLaw
{
   public:
    /// @brief Constructor.
    ///
    /// @param aThrustDirectionThetaRH Thrust direction in theta-R-H [θ, R, H]. Normalized internally. Must be non-zero.
    /// @param aQLawSPtr QLaw used to compute effectivity and supply thresholds. Must be non-null.
    EffectivityGatedConstantThrust(const Vector3d& aThrustDirectionThetaRH, const Shared<const QLaw>& aQLawSPtr);

    /// @brief Destructor.
    virtual ~EffectivityGatedConstantThrust() override;

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aLaw An EffectivityGatedConstantThrust guidance law.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const EffectivityGatedConstantThrust& aLaw);

    /// @brief Print the guidance law.
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Access the held QLaw.
    ///
    /// @return A shared pointer to the held QLaw.
    const Shared<const QLaw>& accessQLaw() const;

    /// @brief Get the thrust direction in theta-R-H.
    ///
    /// @return The normalized thrust direction in theta-R-H.
    Vector3d getThrustDirectionThetaRH() const;

    /// @brief Compute the thrust acceleration at the provided coordinates.
    ///
    /// Returns aThrustAcceleration * R_thetaRH→GCRF * thrustDirectionThetaRH_ when both effectivity
    /// thresholds (where defined on the held QLaw) are met; Vector3d::Zero() otherwise.
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
    Vector3d thrustDirectionThetaRH_;
    Shared<const QLaw> qLawSPtr_;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
