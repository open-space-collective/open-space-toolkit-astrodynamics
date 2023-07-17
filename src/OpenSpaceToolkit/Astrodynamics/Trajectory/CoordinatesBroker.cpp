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
      coordinatesSubsetIndexMap_({})
{
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

    for (const auto& element : this->coordinatesSubsetIndexMap_)
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

Index CoordinatesBroker::addSubset(const CoordinatesSubset& aCoordinatesSubset)
{
    auto search = this->coordinatesSubsetIndexMap_.find(aCoordinatesSubset.getId());

    if (search != this->coordinatesSubsetIndexMap_.end())
    {
        return search->second;
    }

    Index preAdditionNextCoordinatesSubsetIndex = this->nextCoordinatesSubsetIndex_;
    this->coordinatesSubsetIndexMap_.insert({aCoordinatesSubset.getId(), this->nextCoordinatesSubsetIndex_});
    this->nextCoordinatesSubsetIndex_ += aCoordinatesSubset.getSize();

    return preAdditionNextCoordinatesSubsetIndex;
}

bool CoordinatesBroker::hasSubset(const String& anId) const
{
    auto search = this->coordinatesSubsetIndexMap_.find(anId);

    return search != this->coordinatesSubsetIndexMap_.end();
}

bool CoordinatesBroker::hasSubset(const CoordinatesSubset& aCoordinatesSubset) const
{
    return this->hasSubset(aCoordinatesSubset.getId());
}

Index CoordinatesBroker::getSubsetIndex(const String& anId) const
{
    auto search = this->coordinatesSubsetIndexMap_.find(anId);

    if (search == this->coordinatesSubsetIndexMap_.end())
    {
        throw ostk::core::error::runtime::Wrong("Coordinates subset not found");
    }

    return search->second;
}

Index CoordinatesBroker::getSubsetIndex(const CoordinatesSubset& aCoordinatesSubset) const
{
    return this->getSubsetIndex(aCoordinatesSubset.getId());
}

Size CoordinatesBroker::getNumberOfCoordinates() const
{
    return this->nextCoordinatesSubsetIndex_;
}

Size CoordinatesBroker::getNumberOfSubsets() const
{
    return (size_t)this->coordinatesSubsetIndexMap_.size();
}

VectorXd CoordinatesBroker::extract(const VectorXd& allCoordinates, const CoordinatesSubset& aCoordinatesSubset) const
{
    VectorXd subsetCoordinates = VectorXd(aCoordinatesSubset.getSize());
    Index indexOffset = this->getSubsetIndex(aCoordinatesSubset);

    for (Index i = 0; i < aCoordinatesSubset.getSize(); i++)
    {
        subsetCoordinates(i) = allCoordinates(i + indexOffset);
    }

    return subsetCoordinates;
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
