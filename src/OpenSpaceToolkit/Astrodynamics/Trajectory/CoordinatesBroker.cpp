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

Integer CoordinatesBroker::addSubset(const CoordinatesSubset& aCoordinatesSubset)
{
    auto search = this->map_.find(aCoordinatesSubset);

    if (search != this->map_.end())
    {
        return search->second;
    }

    Integer preAdditionNextIndex = this->nextIndex_;
    this->map_.insert({aCoordinatesSubset, this->nextIndex_});
    this->nextIndex_ += aCoordinatesSubset.getSize();

    return preAdditionNextIndex;
}

bool CoordinatesBroker::hasSubset(const CoordinatesSubset& aCoordinatesSubset) const
{
    auto search = this->map_.find(aCoordinatesSubset);

    return search != this->map_.end();
}

Integer CoordinatesBroker::getSubsetIndex(const CoordinatesSubset& aCoordinatesSubset) const
{
    auto search = this->map_.find(aCoordinatesSubset);

    if (search == this->map_.end())
    {
        throw ostk::core::error::runtime::Wrong("Coordinates subset not found");
    }

    return search->second;
}

Integer CoordinatesBroker::getNumberOfCoordinates() const
{
    return this->nextIndex_;
}

Integer CoordinatesBroker::getNumberOfSubsets() const
{
    return (int)this->map_.size();
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
