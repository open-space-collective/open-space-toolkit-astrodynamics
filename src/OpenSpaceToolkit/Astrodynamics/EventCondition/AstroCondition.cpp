/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AstroCondition.hpp>

namespace ostk
{
namespace astro
{
namespace eventcondition
{

AstroCondition::AstroCondition(
    const String& aName,
    const Criteria& aCriteria,
    const std::function<Real(const VectorXd, const Real&, const Instant&, const Shared<const Frame>&, Shared<const CoordinatesBroker>&)> anEvaluator,
    const Real& aTarget)
    : EventCondition(
        aName,
        aCriteria,
        [this, anEvaluator](const VectorXd& aStateVector, const Real& aTime) -> Real
        {
            return anEvaluator(
                aStateVector,
                aTime,
                this->referenceInstant_,
                this->referenceFrameSPtr_,
                this->referenceCoordinatesBrokerSPtr_);
        },
        aTarget)
{
}

AstroCondition::~AstroCondition() {}

void AstroCondition::setReferenceParameters(
    const Instant& aReferenceInstant,
    const Shared<const Frame>& aReferenceFrameSPtr,
    const Shared<const CoordinatesBroker>& aReferenceCoordinatesBrokerSPtr
)
{
    this->referenceInstant_ = aReferenceInstant;
    this->referenceFrameSPtr_ = aReferenceFrameSPtr;
    this->referenceCoordinatesBrokerSPtr_ = aReferenceCoordinatesBrokerSPtr;
}

}  // namespace eventcondition
}  // namespace astro
}  // namespace ostk
