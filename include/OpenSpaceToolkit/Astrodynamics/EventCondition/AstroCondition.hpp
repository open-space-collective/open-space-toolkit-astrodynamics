/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_EventCondition_AstroCondition__
#define __OpenSpaceToolkit_Astrodynamics_EventCondition_AstroCondition__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Eventcondition/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::state::CoordinatesBroker;

class AstroCondition: public EventCondition
{
   public:
    AstroCondition(
        const String& aName,
        const Criteria& aCriteria,
        const std::function<Real(const VectorXd, const Real&, const Instant&, const Shared<const Frame>&, Shared<const CoordinatesBroker>&)> anEvaluator,
        const Real& aTarget = 0.0
    );

    /// @brief                  Virtual destructor

    virtual ~AstroCondition();

    void setReferenceParameters(
        const Instant& aReferenceInstant,
        const Shared<const Frame>& aReferenceFrameSPtr,
        const Shared<const CoordinatesBroker>& aReferenceCoordinatesBrokerSPtr
    );

   private:
    Instant referenceInstant_;
    Shared<const Frame> referenceFrameSPtr_;
    Shared<const CoordinatesBroker> referenceCoordinatesBrokerSPtr_;
    
};

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk

#endif
