/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

CoordinatesSubset::CoordinatesSubset(const String& aName, const Size& aSize)
    : name_(aName),
      size_(aSize)
{
    this->id_ = aName + " | " + std::to_string(aSize);
}

// CoordinatesSubset CoordinatesSubset::Undefined()
// {
//     return {String::Empty(), 0};
// }

bool CoordinatesSubset::operator==(const CoordinatesSubset& aCoordinatesSubset) const
{
    if (!this->isDefined() || !aCoordinatesSubset.isDefined())
    {
        return false;
    }

    return this->id_ == aCoordinatesSubset.id_;
}

bool CoordinatesSubset::operator!=(const CoordinatesSubset& aCoordinatesSubset) const
{
    return !((*this) == aCoordinatesSubset);
}

bool CoordinatesSubset::isDefined() const
{
    return !this->id_.isEmpty() && (this->size_ != 0);
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

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
