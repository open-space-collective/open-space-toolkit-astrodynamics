/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
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
    [[maybe_unused]] const VectorXd& aFullCoordinatesVector,
    [[maybe_unused]] const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    throw ostk::core::error::runtime::Wrong("Addition operation");
}

VectorXd CoordinatesSubset::subtract(
    [[maybe_unused]] const Instant& anInstant,
    [[maybe_unused]] const VectorXd& aFullCoordinatesVector,
    [[maybe_unused]] const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    throw ostk::core::error::runtime::Wrong("Subtraction operation");
}

VectorXd CoordinatesSubset::inFrame(
    [[maybe_unused]] const Instant& anInstant,
    const VectorXd& aFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& fromFrame,
    [[maybe_unused]] const Shared<const Frame>& toFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    VectorXd coordinates = aCoordinatesBrokerSPtr->extractCoordinate(aFullCoordinatesVector, *this);

    return coordinates;
}

Shared<const CoordinatesSubset> CoordinatesSubset::Mass()
{
    static const Shared<const CoordinatesSubset> mass = std::make_shared<CoordinatesSubset>("MASS", 1);
    return mass;
}

Shared<const CoordinatesSubset> CoordinatesSubset::SurfaceArea()
{
    static const Shared<const CoordinatesSubset> surfaceArea = std::make_shared<CoordinatesSubset>("SURFACE_AREA", 1);
    return surfaceArea;
}

Shared<const CoordinatesSubset> CoordinatesSubset::DragCoefficient()
{
    static const Shared<const CoordinatesSubset> dragCoefficient =
        std::make_shared<CoordinatesSubset>("DRAG_COEFFICIENT", 1);
    return dragCoefficient;
}

}  // namespace state
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
