/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

CoordinatesSubset::CoordinatesSubset(const String& anId, const Integer& aSize)
    : id_(anId),
      size_(aSize)
{
    if (aSize <= 0)
    {
        throw ostk::core::error::runtime::Wrong("Size");
    }
}

CoordinatesSubset CoordinatesSubset::Undefined()
{
    return {String::Empty(), Integer::Undefined()};
}

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
    return !this->id_.isEmpty() && this->size_.isDefined();
}

String CoordinatesSubset::getId() const
{
    return id_;
}

Integer CoordinatesSubset::getSize() const
{
    return size_;
}

CoordinatesSubset CoordinatesSubset::Position()
{
    return {"POSITION", 3};
}

CoordinatesSubset CoordinatesSubset::Velocity()
{
    return {"VELOCITY", 3};
}

CoordinatesSubset CoordinatesSubset::Mass()
{
    return {"MASS", 1};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
