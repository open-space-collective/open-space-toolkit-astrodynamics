/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Index;

using ostk::astro::trajectory::state::CoordinatesSubset;

StateBuilder::StateBuilder(
    const Shared<const Frame>& aFrameSPtr,
    const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray
)
    : 
      frameSPtr_(aFrameSPtr),
      coordinatesBrokerSPtr_(std::make_shared<CoordinatesBroker>(CoordinatesBroker(aCoordinatesSubsetsArray)))
{
}

StateBuilder::StateBuilder(
    const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
)
    : 
      frameSPtr_(aFrameSPtr),
      coordinatesBrokerSPtr_(aCoordinatesBrokerSPtr)
{
}

bool StateBuilder::operator==(const StateBuilder& aStateBuilder) const
{

    if ((!this->isDefined()) || (!aStateBuilder.isDefined()))
    {
        return false;
    }

    if (this->frameSPtr_ != aStateBuilder.frameSPtr_)
    {
        return false;
    }

    for (const Shared<const CoordinatesSubset>& subset : this->coordinatesBrokerSPtr_->accessSubsets())
    {
        if (!aStateBuilder.coordinatesBrokerSPtr_->hasSubset(subset))
        {
            return false;
        }
    }

    for (const Shared<const CoordinatesSubset>& subset : aStateBuilder.coordinatesBrokerSPtr_->accessSubsets())
    {
        if (!this->coordinatesBrokerSPtr_->hasSubset(subset))
        {
            return false;
        }
    }

    return true;
}

bool StateBuilder::operator!=(const StateBuilder& aStateBuilder) const
{
    return !((*this) == aStateBuilder);
}

std::ostream& operator<<(std::ostream& anOutputStream, const StateBuilder& aStateBuilder)
{
    aStateBuilder.print(anOutputStream);

    return anOutputStream;
}

bool StateBuilder::isDefined() const
{
    return (this->frameSPtr_ != nullptr) && this->frameSPtr_->isDefined() && (this->coordinatesBrokerSPtr_ != nullptr);
}

const Shared<const Frame> StateBuilder::accessFrame() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("StateBuilder");
    }

    return this->frameSPtr_;
}

const Shared<const CoordinatesBroker>& StateBuilder::accessCoordinatesBroker() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("StateBuilder");
    }

    return this->coordinatesBrokerSPtr_;
}

Shared<const Frame> StateBuilder::getFrame() const
{
    return this->accessFrame();
}

const Array<Shared<const CoordinatesSubset>> StateBuilder::getCoordinatesSubsets() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("StateBuilder");
    }

    return this->coordinatesBrokerSPtr_->getSubsets();
}

void StateBuilder::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::types::String;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory :: StateBuilder") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Frame:" << (this->frameSPtr_->isDefined() ? this->frameSPtr_->getName() : "Undefined");

    if (!this->isDefined())
    {
        ostk::core::utils::Print::Line(anOutputStream) << "Coordinates Subsets: Undefined";
    }
    else
    {
        const Array<Shared<const CoordinatesSubset>> subsets = this->coordinatesBrokerSPtr_->getSubsets();

        for (const auto& subset : subsets)
        {
            ostk::core::utils::Print::Line(anOutputStream) << subset->getName();
        }
    }
    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

StateBuilder StateBuilder::Undefined()
{
    return {Frame::Undefined(), nullptr};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
