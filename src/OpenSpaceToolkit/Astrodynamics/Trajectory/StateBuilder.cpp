/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

StateBuilder::StateBuilder(
    const Shared<const Frame>& aFrameSPtr, const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray
)
    : frameSPtr_(aFrameSPtr),
      coordinatesBrokerSPtr_(std::make_shared<CoordinatesBroker>(CoordinatesBroker(aCoordinatesSubsetsArray)))
{
}

StateBuilder::StateBuilder(
    const Shared<const Frame>& aFrameSPtr, const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
)
    : frameSPtr_(aFrameSPtr),
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

    if ((*this->coordinatesBrokerSPtr_) != (*aStateBuilder.coordinatesBrokerSPtr_))
    {
        return false;
    }

    return true;
}

bool StateBuilder::operator!=(const StateBuilder& aStateBuilder) const
{
    return !((*this) == aStateBuilder);
}

const StateBuilder StateBuilder::operator+(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("StateBuilder");
    }

    if (this->coordinatesBrokerSPtr_->hasSubset(aCoordinatesSubsetSPtr))
    {
        throw ostk::core::error::RuntimeError("Duplicate CoordinatesSubset: [{}]", aCoordinatesSubsetSPtr->getName());
    }

    Array<Shared<const CoordinatesSubset>> expandedSubsets = coordinatesBrokerSPtr_->getSubsets();
    expandedSubsets.add(aCoordinatesSubsetSPtr);

    return StateBuilder(this->frameSPtr_, expandedSubsets);
}

const StateBuilder StateBuilder::operator-(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("StateBuilder");
    }

    if (!this->coordinatesBrokerSPtr_->hasSubset(aCoordinatesSubsetSPtr))
    {
        throw ostk::core::error::RuntimeError("Missing CoordinatesSubset: [{}]", aCoordinatesSubsetSPtr->getName());
    }

    Array<Shared<const CoordinatesSubset>> contractedSubsets = Array<Shared<const CoordinatesSubset>>::Empty();
    for (const auto& subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        if (subset != aCoordinatesSubsetSPtr)
        {
            contractedSubsets.add(subset);
        }
    }

    return StateBuilder(this->frameSPtr_, contractedSubsets);
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

const State StateBuilder::build(const Instant& anInstant, const VectorXd& aCoordinates) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("StateBuilder");
    }

    return {anInstant, aCoordinates, this->frameSPtr_, this->coordinatesBrokerSPtr_};
}

const State StateBuilder::expand(
    const State& aState, const VectorXd& aDefaultCoordinates
) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("StateBuilder");
    }

    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (aState.accessFrame() != this->frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    if ((Size) aDefaultCoordinates.size() != this->coordinatesBrokerSPtr_->getNumberOfCoordinates()){
        throw ostk::core::error::RuntimeError(
            "Expected default coordinates size [{}]. Got [{}].", 
            this->coordinatesBrokerSPtr_->getNumberOfCoordinates(), 
            aDefaultCoordinates.size()
        );
    }

    // this does a deep copy
    VectorXd output_coordinates = aDefaultCoordinates;

    for (const auto& subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        // If the subset can be found in the input state, use that
        if (aState.accessCoordinatesBroker()->hasSubset(subset))
        {
            output_coordinates.segment(
                this->coordinatesBrokerSPtr_->getSubsetIndex(subset->getId()),
                subset->getSize()
            ) = aState.extractCoordinates(subset);
        } 
    }

    return this->build(aState.accessInstant(), output_coordinates);
}

const State StateBuilder::reduce(const State& aState) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("StateBuilder");
    }

    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    if (aState.accessFrame() != this->frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    // this does a deep copy
    VectorXd output_coordinates(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());

    for (const auto& subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        output_coordinates.segment(
            this->coordinatesBrokerSPtr_->getSubsetIndex(subset->getId()),
            subset->getSize()
        ) = aState.extractCoordinates(subset);
    }

    return this->build(aState.accessInstant(), output_coordinates);
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
