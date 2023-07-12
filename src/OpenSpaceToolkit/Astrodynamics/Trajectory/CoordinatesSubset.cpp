/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

CoordinatesSubset::CoordinatesSubset(const String& anId, const Size& aSize)
    : id_(anId),
      size_(aSize)
{
}

CoordinatesSubset CoordinatesSubset::Undefined()
{
    return {String::Empty(), 0};
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
    return !this->id_.isEmpty() && (this->size_ != 0);
}

String CoordinatesSubset::getId() const
{
    return id_;
}

Size CoordinatesSubset::getSize() const
{
    return size_;
}

CoordinatesSubset CoordinatesSubset::PositionCartesian()
{
    return {"POSITION_CARTESIAN", 3};
}

CoordinatesSubset CoordinatesSubset::VelocityCartesian()
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
