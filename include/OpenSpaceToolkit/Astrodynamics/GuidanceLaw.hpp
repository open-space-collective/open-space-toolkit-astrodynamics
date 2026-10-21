/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw__

#include <memory>
#include <optional>

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

/// @brief An interface for a Guidance Law that can compute an acceleration contribution. To be
/// used in conjunction with a Thruster class to propagate a satellite to a target orbit.
class GuidanceLaw : public std::enable_shared_from_this<GuidanceLaw>
{
   public:
    /// @brief A maneuver that a guidance law commits to when it starts.
    struct CommittedManeuver
    {
        /// @brief The guidance law to follow during the maneuver.
        Shared<const GuidanceLaw> guidanceLaw;

        /// @brief The maneuver duration.
        Duration duration;
    };

    /// @brief Constructor
    ///
    /// @param aName A name
    GuidanceLaw(const String& aName);

    /// @brief Destructor
    virtual ~GuidanceLaw();

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << GuidanceLaw(...);
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aGuidanceLaw A guidance Law
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const GuidanceLaw& aGuidanceLaw);

    /// @brief Get the guidance law name.
    ///
    /// @code{.cpp}
    ///     GuidanceLaw guidanceLaw = { ... } ;
    ///     String name = guidanceLaw.getName() ;
    /// @endcode
    ///
    /// @return The name.
    String getName() const;

    /// @brief Print guidance law
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Compute the acceleration at the provided coordinates based on logic specific to the
    /// guidance law
    ///
    /// @param anInstant An instant
    /// @param aPositionCoordinates The position coordinates
    /// @param aVelocityCoordinates The velocity coordinates
    /// @param aThrustAcceleration The thrust acceleration
    /// @param outputFrameSPtr The frame in which the acceleration is expressed
    ///
    /// @return The acceleration at the provided coordinates
    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const = 0;

    /// @brief Commit to a maneuver starting at the provided coordinates.
    ///
    /// Guidance laws that plan discrete maneuvers (e.g. with model predictive control) decide at every evaluation
    /// whether a maneuver is worth starting. Re-evaluated along a maneuver, they would change their decision, as the
    /// maneuver itself erodes the reasons to perform it. When such a guidance law starts a maneuver, a maneuvering
    /// Segment asks it for the guidance law to follow during the whole maneuver and for the maneuver duration, instead
    /// of re-evaluating it along the maneuver.
    ///
    /// The default implementation does not commit.
    ///
    /// @param anInstant The start instant of the maneuver
    /// @param aPositionCoordinates The position coordinates at the start of the maneuver
    /// @param aVelocityCoordinates The velocity coordinates at the start of the maneuver
    /// @param aFrameSPtr The frame of the coordinates
    /// @return The committed maneuver, if any.
    virtual std::optional<CommittedManeuver> commitManeuverAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Shared<const Frame>& aFrameSPtr
    ) const;

    /// @brief Construct an ungated version of the instance.
    ///
    /// An 'ungated' guidance law is one that always returns a non-zero acceleration vector.
    ///
    /// The default implementation returns the original instance.
    ///
    /// @return The ungated guidance law instance.
    virtual Shared<GuidanceLaw> constructUngatedGuidanceLaw() const;

   protected:
    const String name_;
};

}  // namespace astrodynamics
}  // namespace ostk

#endif
