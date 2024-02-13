/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

CoordinatesBroker::CoordinatesBroker()
    : nextCoordinateSubsetIndex_(0),
      coordinateSubsets_({}),
      coordinateSubsetsIndexMap_({})
{
}

CoordinatesBroker::CoordinatesBroker(const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray)
    : CoordinatesBroker()
{
    for (Shared<const CoordinateSubset> subset : aCoordinateSubsetsArray)
    {
        this->addSubset(subset);
    }
}

bool CoordinatesBroker::operator==(const CoordinatesBroker& aCoordinatesBroker) const
{
    if (this->getNumberOfCoordinates() != aCoordinatesBroker.getNumberOfCoordinates())
    {
        return false;
    }

    if (this->getNumberOfSubsets() != aCoordinatesBroker.getNumberOfSubsets())
    {
        return false;
    }

    for (const auto& element : this->coordinateSubsetsIndexMap_)
    {
        const String id = element.first;
        if (!aCoordinatesBroker.hasSubset(id))
        {
            return false;
        }

        if (element.second != aCoordinatesBroker.getSubsetIndex(id))
        {
            return false;
        }
    }

    return true;
}

bool CoordinatesBroker::operator!=(const CoordinatesBroker& aCoordinatesBroker) const
{
    return !((*this) == aCoordinatesBroker);
}

const Array<Shared<const CoordinateSubset>>& CoordinatesBroker::accessSubsets() const
{
    return this->coordinateSubsets_;
}

Size CoordinatesBroker::getNumberOfCoordinates() const
{
    return this->nextCoordinateSubsetIndex_;
}

Size CoordinatesBroker::getNumberOfSubsets() const
{
    return (size_t)this->coordinateSubsetsIndexMap_.size();
}

Array<Shared<const CoordinateSubset>> CoordinatesBroker::getSubsets() const
{
    return this->accessSubsets();
}

Index CoordinatesBroker::addSubset(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr)
{
    const auto search = this->coordinateSubsetsIndexMap_.find(aCoordinateSubsetSPtr->getId());

    if (search != this->coordinateSubsetsIndexMap_.end())
    {
        return search->second;
    }

    const Index coordinatesSubsetIndex = this->nextCoordinateSubsetIndex_;

    this->coordinateSubsets_.add(aCoordinateSubsetSPtr);
    this->coordinateSubsetsIndexMap_.insert({aCoordinateSubsetSPtr->getId(), this->nextCoordinateSubsetIndex_});
    this->nextCoordinateSubsetIndex_ += aCoordinateSubsetSPtr->getSize();

    return coordinatesSubsetIndex;
}

bool CoordinatesBroker::hasSubset(const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr) const
{
    return this->hasSubset(aCoordinateSubsetSPtr->getId());
}

VectorXd CoordinatesBroker::extractCoordinate(
    const VectorXd& aFullCoordinatesVector, const CoordinateSubset& aCoordinateSubset
) const
{
    return aFullCoordinatesVector.segment(
        this->getSubsetIndex(aCoordinateSubset.getId()), aCoordinateSubset.getSize()
    );
}

VectorXd CoordinatesBroker::extractCoordinates(
    const VectorXd& aFullCoordinatesVector, const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetsArray
) const
{
    Size coordinateSubsetsSize = 0;
    for (const auto& subset : aCoordinateSubsetsArray)
    {
        coordinateSubsetsSize += subset->getSize();
    }

    VectorXd coordinateSubsetsVector(coordinateSubsetsSize);

    int startIndex = 0;
    for (const auto& subset : aCoordinateSubsetsArray)
    {
        coordinateSubsetsVector.segment(startIndex, subset->getSize()) =
            aFullCoordinatesVector.segment(this->getSubsetIndex(subset->getId()), subset->getSize());

        startIndex += subset->getSize();
    }

    return coordinateSubsetsVector;
}

VectorXd CoordinatesBroker::extractCoordinate(
    const VectorXd& aFullCoordinatesVector, const Shared<const CoordinateSubset>& aCoordinateSubsetSPtr
) const
{
    return this->extractCoordinate(aFullCoordinatesVector, *aCoordinateSubsetSPtr);
}

bool CoordinatesBroker::hasSubset(const String& anId) const
{
    return this->coordinateSubsetsIndexMap_.count(anId) > 0;
}

Index CoordinatesBroker::getSubsetIndex(const String& anId) const
{
    auto search = this->coordinateSubsetsIndexMap_.find(anId);

    if (search == this->coordinateSubsetsIndexMap_.end())
    {
        throw ostk::core::error::RuntimeError("Coordinates subset [{}] not found", anId);
    }

    return search->second;
}

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
