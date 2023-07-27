/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace state
{

CoordinatesSubset::CoordinatesSubset(const String& aName, const Size& aSize)
    : name_(aName),
      size_(aSize),
      id_(aName + " | " + std::to_string(aSize))
{
    if (aName.isEmpty())
    {
        throw ostk::core::error::runtime::Wrong("Name");
    }

    if (aSize == 0)
    {
        throw ostk::core::error::runtime::Wrong("Size");
    }
}

bool CoordinatesSubset::operator==(const CoordinatesSubset& aCoordinatesSubset) const
{
    return this->id_ == aCoordinatesSubset.id_;
}

bool CoordinatesSubset::operator!=(const CoordinatesSubset& aCoordinatesSubset) const
{
    return !((*this) == aCoordinatesSubset);
}

String CoordinatesSubset::getId() const
{
    return id_;
}

String CoordinatesSubset::getName() const
{
    return name_;
}

Size CoordinatesSubset::getSize() const
{
    return size_;
}

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
