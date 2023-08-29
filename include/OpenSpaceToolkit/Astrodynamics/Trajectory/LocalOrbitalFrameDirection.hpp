/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Shared;
using ostk::core::types::Integer;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::LocalOrbitalFrameFactory;

/// @brief                      Local Orbital Frame Direction
///
///                             A unit vector, expressed along a Local Orbital Frame Factory

class LocalOrbitalFrameDirection
{
   public:
    LocalOrbitalFrameDirection(const Vector3d& aValue, const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr);

    bool operator==(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const;

    bool operator!=(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const;

    bool isDefined() const;

    static LocalOrbitalFrameDirection Undefined();

   private:
    Vector3d value_;
    Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactorySPtr_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
