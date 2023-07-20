/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

CoordinatesBroker::CoordinatesBroker()
    : nextCoordinatesSubsetIndex_(0),
      coordinateSubsets_({}),
      coordinateSubsetsIndexMap_({})
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

Size CoordinatesBroker::getNumberOfCoordinates() const
{
    return this->nextCoordinatesSubsetIndex_;
}

Size CoordinatesBroker::getNumberOfSubsets() const
{
    return (size_t)this->coordinateSubsetsIndexMap_.size();
}

Array<Shared<const CoordinatesSubset>> CoordinatesBroker::getSubsets() const
{
    return this->coordinateSubsets_;
}

Index CoordinatesBroker::addSubset(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr)
{
    const auto search = this->coordinateSubsetsIndexMap_.find(aCoordinatesSubsetSPtr->getId());

    if (search != this->coordinateSubsetsIndexMap_.end())
    {
        return search->second;
    }

    Index preAdditionNextCoordinatesSubsetIndex = this->nextCoordinatesSubsetIndex_;

    this->coordinateSubsets_.add(aCoordinatesSubsetSPtr);
    this->coordinateSubsetsIndexMap_.insert({aCoordinatesSubsetSPtr->getId(), this->nextCoordinatesSubsetIndex_});
    this->nextCoordinatesSubsetIndex_ += aCoordinatesSubsetSPtr->getSize();

    return preAdditionNextCoordinatesSubsetIndex;
}

bool CoordinatesBroker::hasSubset(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const
{
    return this->hasSubset(aCoordinatesSubsetSPtr->getId());
}

Index CoordinatesBroker::getSubsetIndex(const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr) const
{
    return this->getSubsetIndex(aCoordinatesSubsetSPtr->getId());
}

VectorXd CoordinatesBroker::extract(const VectorXd& allCoordinates, const CoordinatesSubset& aCoordinatesSubset) const
{
    const Size size = aCoordinatesSubset.getSize();
    VectorXd subsetCoordinates = VectorXd(size);
    const Index indexOffset = this->getSubsetIndex(aCoordinatesSubset.getId());

    for (Index i = 0; i < size; i++)
    {
        subsetCoordinates(i) = allCoordinates(i + indexOffset);
    }

    return subsetCoordinates;
}

VectorXd CoordinatesBroker::extract(
    const VectorXd& allCoordinates, const Shared<const CoordinatesSubset>& aCoordinatesSubsetSPtr
) const
{
    return this->extract(allCoordinates, *aCoordinatesSubsetSPtr);
}

Shared<const CoordinatesBroker> CoordinatesBroker::FromSubsets(
    const Array<Shared<const CoordinatesSubset>>& aCoordinatesSubsetsArray
)
{
    const CoordinatesBroker broker = CoordinatesBroker(aCoordinatesSubsetsArray);
    return std::make_shared<CoordinatesBroker>(broker);
}

bool CoordinatesBroker::hasSubset(const String& anId) const
{
    auto search = this->coordinateSubsetsIndexMap_.find(anId);

    return search != this->coordinateSubsetsIndexMap_.end();
}

Index CoordinatesBroker::getSubsetIndex(const String& anId) const
{
    auto search = this->coordinateSubsetsIndexMap_.find(anId);

    if (search == this->coordinateSubsetsIndexMap_.end())
    {
        throw ostk::core::error::runtime::Wrong("Coordinates subset not found");
    }

    return search->second;
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
