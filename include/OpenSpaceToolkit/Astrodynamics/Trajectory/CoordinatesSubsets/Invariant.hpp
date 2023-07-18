/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_Invariant__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_Invariant__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace coordinatessubsets
{

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::CoordinatesSubset;
using ostk::astro::trajectory::CoordinatesBroker;

/// @brief  Invariant (frame-indepdentent) coordinate subset.

class Invariant : public CoordinatesSubset
{
   public:
    static const String MASS_DEFAULT_NAME;

    /// @brief              Constructor
    ///
    /// @param              [in] aName a name
    /// @param              [in] aSize a size

    Invariant(const String& aName, const Size& aSize);

    virtual VectorXd inFrame(
        const Instant& anInstant,
        const VectorXd& allCoordinates,
        const Shared<const Frame>& fromFrame,
        const Shared<const CoordinatesBroker>& aCoordinatesBroker,
        const Shared<const Frame>& toFrame
    ) const override;

    /// @brief              Returns the default mass instance
    ///
    /// @return             The defaul mass instance
    static Shared<const Invariant> Mass();

   private:
    static const Shared<const Invariant> MASS;
};

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
