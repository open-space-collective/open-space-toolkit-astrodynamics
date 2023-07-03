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

CoordinatesBroker::CoordinatesBroker(const Array<CoordinatesSubset>& aCoordinatesSubsetArray)
    : nextCoordinatesSubsetIndex_(0),
      coordinatesSubsetIndexMap_({})
{
    for (CoordinatesSubset coordinatesSubset : aCoordinatesSubsetArray)
    {
        this->addSubset(coordinatesSubset);
    }
}

Index CoordinatesBroker::addSubset(const CoordinatesSubset& aCoordinatesSubset)
{
    auto search = this->coordinatesSubsetIndexMap_.find(aCoordinatesSubset);

    if (search != this->coordinatesSubsetIndexMap_.end())
    {
        return search->second;
    }

    Index preAdditionNextCoordinatesSubsetIndex = this->nextCoordinatesSubsetIndex_;
    this->coordinatesSubsetIndexMap_.insert({aCoordinatesSubset, this->nextCoordinatesSubsetIndex_});
    this->nextCoordinatesSubsetIndex_ += aCoordinatesSubset.getSize();

    return preAdditionNextCoordinatesSubsetIndex;
}

bool CoordinatesBroker::hasSubset(const CoordinatesSubset& aCoordinatesSubset) const
{
    auto search = this->coordinatesSubsetIndexMap_.find(aCoordinatesSubset);

    return search != this->coordinatesSubsetIndexMap_.end();
}

Index CoordinatesBroker::getSubsetIndex(const CoordinatesSubset& aCoordinatesSubset) const
{
    auto search = this->coordinatesSubsetIndexMap_.find(aCoordinatesSubset);

    if (search == this->coordinatesSubsetIndexMap_.end())
    {
        throw ostk::core::error::runtime::Wrong("Coordinates subset not found");
    }

    return search->second;
}

Size CoordinatesBroker::getNumberOfCoordinates() const
{
    return this->nextCoordinatesSubsetIndex_;
}

Size CoordinatesBroker::getNumberOfSubsets() const
{
    return (size_t)this->coordinatesSubsetIndexMap_.size();
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
