/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::type::String;
using ostk::core::type::Shared;
using ostk::core::type::Real;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;

using ostk::physics::time::Instant;

/// @brief An interface for a Guidance Law that can compute an acceleration contribution. To be
/// used in conjunction with a Thruster class to propagate a satellite to a target orbit.
class GuidanceLaw
{
   public:
    /// @brief Constructor
    ///
    /// @param aName A name
    GuidanceLaw(const String& aName);

    /// @brief Destructor
    virtual ~GuidanceLaw();

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << GuidanceLaw(...) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aGuidanceLaw A guidance Law
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const GuidanceLaw& aGuidanceLaw);

    /// @brief Get name
    ///
    /// @return The name
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

   protected:
    const String name_;
};

}  // namespace astro
}  // namespace ostk

#endif
