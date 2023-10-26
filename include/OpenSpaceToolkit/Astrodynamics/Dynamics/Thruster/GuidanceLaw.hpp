/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{
namespace thruster
{

using ostk::core::types::Real;

using ostk::math::obj::Vector3d;

using ostk::physics::time::Instant;
using ostk::physics::units::Derived;

using ostk::astro::trajectory::orbit::models::kepler::COE;

/// @brief                      An interface for a Guidance Law that can computes an acceleration contribution. To be
/// used in conjunction with a Thruster class to propagate a satellite to a target orbit.

class GuidanceLaw
{
   public:
    /// @brief                  Constructor
    ///
    /// @param                  [in] aName A name

    GuidanceLaw(const COE& aCOE, const Derived& aGravitationalParameter);

    /// @brief                  Destructor

    virtual ~GuidanceLaw();

    /// @brief                  Output stream operator
    ///
    /// @code
    ///                         std::cout << GuidanceLaw(...) ;
    /// @endcode
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] aGuidanceLaw A guidance Law
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const GuidanceLaw& aGuidanceLaw);

    /// @brief                  Get Classical Orbital Elements
    ///
    /// @return                 The Classical Orbital Elements

    COE getCOE() const;

    /// @brief                  Print guidance law
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Compute the acceleration at the provided coordinates based on logic specific to the
    /// guidance law
    ///
    /// @param                  [in] anInstant An instant
    /// @param                  [in] aPositionCoordinates The position coordinates
    /// @param                  [in] aVelocityCoordinates The velocity coordinates
    /// @param                  [in] aThrustAcceleration The thrust acceleration
    ///
    /// @return                 The acceleration at the provided coordinates

    virtual Vector3d computeAcceleration(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration
    ) const = 0;

   protected:
    const COE targetCOE_;
    const Derived gravitationalParameter_;
};

}  // namespace thruster
}  // namespace dynamics
}  // namespace astro
}  // namespace ostk

#endif
