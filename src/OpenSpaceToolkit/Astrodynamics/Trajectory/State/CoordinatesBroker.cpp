/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{

CoordinatesBroker::CoordinatesBroker()
    : nextCoordinatesSubsetIndex_(0),
      coordinatesSubsets_({}),
      coordinatesSubsetsIndexMap_({})
{
}

CoordinatesBroker::CoordinatesBroker(const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray)
    : CoordinatesBroker()
{
    for (Shared<const CoordinatesSubset> subset : aCoordinatesSubsetsArray)
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

    for (const auto& element : this->coordinatesSubsetsIndexMap_)
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

const Array<Shared<const CoordinatesSubset>>& CoordinatesBroker::accessSubsets() const
{
    return this->coordinatesSubsets_;
}

Size CoordinatesBroker::getNumberOfCoordinates() const
{
    return this->nextCoordinatesSubsetIndex_;
}

Size CoordinatesBroker::getNumberOfSubsets() const
{
    return (size_t)this->coordinatesSubsetsIndexMap_.size();
}

Array<Shared<const CoordinatesSubset>> CoordinatesBroker::getSubsets() const
{
    return this->accessSubsets();
}

Index CoordinatesBroker::addSubset(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr)
{
    const auto search = this->coordinatesSubsetsIndexMap_.find(aCoordinatesSubsetSPtr->getId());

    if (search != this->coordinatesSubsetsIndexMap_.end())
    {
        return search->second;
    }

    const Index coordinatesSubsetIndex = this->nextCoordinatesSubsetIndex_;

    this->coordinatesSubsets_.add(aCoordinatesSubsetSPtr);
    this->coordinatesSubsetsIndexMap_.insert({aCoordinatesSubsetSPtr->getId(), this->nextCoordinatesSubsetIndex_});
    this->nextCoordinatesSubsetIndex_ += aCoordinatesSubsetSPtr->getSize();

    return coordinatesSubsetIndex;
}

bool CoordinatesBroker::hasSubset(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const
{
    return this->hasSubset(aCoordinatesSubsetSPtr->getId());
}

VectorXd CoordinatesBroker::extractCoordinate(
    const VectorXd& aFullCoordinatesVector, const CoordinatesSubset& aCoordinatesSubset
) const
{
    return aFullCoordinatesVector.segment(
        this->getSubsetIndex(aCoordinatesSubset.getId()), aCoordinatesSubset.getSize()
    );
}

VectorXd CoordinatesBroker::extractCoordinates(
    const VectorXd& aFullCoordinatesVector, const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray
) const
{
    Size coordinatesSubsetsSize = 0;
    for (const auto& subset : aCoordinatesSubsetsArray)
    {
        coordinatesSubsetsSize += subset->getSize();
    }

    VectorXd coordinatesSubsetsVector(coordinatesSubsetsSize);

    int startIndex = 0;
    for (const auto& subset : aCoordinatesSubsetsArray)
    {
        coordinatesSubsetsVector.segment(startIndex, subset->getSize()) =
            aFullCoordinatesVector.segment(this->getSubsetIndex(subset->getId()), subset->getSize());

        startIndex += subset->getSize();
    }

    return coordinatesSubsetsVector;
}

VectorXd CoordinatesBroker::extractCoordinate(
    const VectorXd& aFullCoordinatesVector, const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr
) const
{
    return this->extractCoordinate(aFullCoordinatesVector, *aCoordinatesSubsetSPtr);
}

bool CoordinatesBroker::hasSubset(const String& anId) const
{
    return this->coordinatesSubsetsIndexMap_.count(anId) > 0;
}

Index CoordinatesBroker::getSubsetIndex(const String& anId) const
{
    auto search = this->coordinatesSubsetsIndexMap_.find(anId);

    if (search == this->coordinatesSubsetsIndexMap_.end())
    {
        throw ostk::core::error::RuntimeError("Coordinates subset [{}] not found", anId);
    }

    return search->second;
}

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
