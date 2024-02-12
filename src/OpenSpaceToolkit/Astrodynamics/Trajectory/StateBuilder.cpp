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

StateBuilder::StateBuilder(const State& aState)
    : frameSPtr_(aState.accessFrame()),
      coordinatesBrokerSPtr_(aState.accessCoordinatesBroker())
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
        throw ostk::core::error::runtime::Wrong("State Frame");
    }

    VectorXd coordinates = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Integer nextIndex = 0;

    for (const auto& subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        if (!aState.accessCoordinatesBroker()->hasSubset(subset))
        {
            throw ostk::core::error::RuntimeError("Missing CoordinatesSubset: [{}]", subset->getName());
        }

        const VectorXd subsetCoordinates = aState.extractCoordinate(subset);
        coordinates.segment(nextIndex, subsetCoordinates.size()) = subsetCoordinates;
        nextIndex += subsetCoordinates.size();
    }

    return this->build(aState.accessInstant(), coordinates);
}

const State StateBuilder::expand(const State& aState, const State& defaultState) const
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
        throw ostk::core::error::runtime::Wrong("State Frame");
    }

    if (!defaultState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Default State");
    }

    if (defaultState.accessFrame() != this->frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Default State Frame");
    }

    VectorXd coordinates = VectorXd(this->coordinatesBrokerSPtr_->getNumberOfCoordinates());
    Integer nextIndex = 0;
    Integer nonDefaultSubsetDetections = 0;

    for (const auto& subset : this->coordinatesBrokerSPtr_->getSubsets())
    {
        bool subsetDetected = false;
        VectorXd subsetCoordinates;

        if (aState.accessCoordinatesBroker()->hasSubset(subset))
        {
            subsetDetected = true;
            nonDefaultSubsetDetections++;
            subsetCoordinates = aState.extractCoordinate(subset);
        }

        if (!subsetDetected && defaultState.accessCoordinatesBroker()->hasSubset(subset))
        {
            subsetDetected = true;
            subsetCoordinates = defaultState.extractCoordinate(subset);
        }

        if (!subsetDetected)
        {
            throw ostk::core::error::RuntimeError("Missing CoordinatesSubset: [{}]", subset->getName());
        }

        coordinates.segment(nextIndex, subsetCoordinates.size()) = subsetCoordinates;
        nextIndex += subsetCoordinates.size();
    }

    if (Size(nonDefaultSubsetDetections) != aState.accessCoordinatesBroker()->getNumberOfSubsets())
    {
        throw ostk::core::error::RuntimeError("The operation is not an expansion");
    }

    return this->build(aState.accessInstant(), coordinates);
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
    using ostk::core::type::String;

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
