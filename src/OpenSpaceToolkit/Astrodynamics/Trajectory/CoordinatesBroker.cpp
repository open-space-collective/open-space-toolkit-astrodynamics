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
    : nextIndex_(0),
      map_({})
{
}

CoordinatesBroker::CoordinatesBroker(const Array<CoordinatesSubset>& aCoordinatesSubsetArray)
    : nextIndex_(0),
      map_({})
{
    for (CoordinatesSubset coordinatesSubset : aCoordinatesSubsetArray)
    {
        this->addSubset(coordinatesSubset);
    }
}

Index CoordinatesBroker::addSubset(const CoordinatesSubset& aCoordinatesSubset)
{
    auto search = this->map_.find(aCoordinatesSubset);

    if (search != this->map_.end())
    {
        return search->second;
    }

    Index preAdditionNextIndex = this->nextIndex_;
    this->map_.insert({aCoordinatesSubset, this->nextIndex_});
    this->nextIndex_ += aCoordinatesSubset.getSize();

    return preAdditionNextIndex;
}

bool CoordinatesBroker::hasSubset(const CoordinatesSubset& aCoordinatesSubset) const
{
    auto search = this->map_.find(aCoordinatesSubset);

    return search != this->map_.end();
}

Index CoordinatesBroker::getSubsetIndex(const CoordinatesSubset& aCoordinatesSubset) const
{
    auto search = this->map_.find(aCoordinatesSubset);

    if (search == this->map_.end())
    {
        throw ostk::core::error::runtime::Wrong("Coordinates subset not found");
    }

    return search->second;
}

Size CoordinatesBroker::getNumberOfCoordinates() const
{
    return this->nextIndex_;
}

Size CoordinatesBroker::getNumberOfSubsets() const
{
    return (size_t)this->map_.size();
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
