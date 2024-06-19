/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace orbit
{

using ostk::core::container::Array;

using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::Pass;

Model::Model() {}

Model::~Model() {}

Integer Model::calculateRevolutionNumberAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Model");
    }

    if (anInstant < this->getEpoch())
    {
        throw ostk::core::error::runtime::ToBeImplemented("Backward propagation.");
    }

    if (anInstant == this->getEpoch())
    {
        return this->getRevolutionNumberAtEpoch();
    }

    const Instant startInstant = this->getEpoch();
    const Instant endInstant = anInstant;

    const Array<Pass> passes = Orbit::ComputePassesWithModel(*this, Interval::Closed(startInstant, endInstant));

    if (passes.isEmpty())
    {
        throw ostk::core::error::RuntimeError("No pass found");
    }

    return passes.accessLast().getRevolutionNumber();
}

}  // namespace orbit
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
