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

VectorXd CoordinatesSubset::add(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    return aCoordinatesBrokerSPtr->extractCoordinates(aFullCoordinatesVector, *this) +
           aCoordinatesBrokerSPtr->extractCoordinates(anotherFullCoordinatesVector, *this);
}

VectorXd CoordinatesSubset::subtract(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    return aCoordinatesBrokerSPtr->extractCoordinates(aFullCoordinatesVector, *this) -
           aCoordinatesBrokerSPtr->extractCoordinates(anotherFullCoordinatesVector, *this);
}

VectorXd CoordinatesSubset::inFrame(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& fromFrame,
    [[maybe_unused]] const Shared<const Frame>& toFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    VectorXd coordinates = aCoordinatesBrokerSPtr->extractCoordinates(aFullCoordinatesVector, *this);

    return coordinates;
}

Shared<const CoordinatesSubset> CoordinatesSubset::Mass()
{
    static const Shared<const CoordinatesSubset> mass =
        std::make_shared<CoordinatesSubset>("MASS", 1);
    return mass;
}

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
