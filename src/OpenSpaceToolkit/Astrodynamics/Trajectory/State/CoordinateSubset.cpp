/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

CoordinateSubset::CoordinateSubset(const String& aName, const Size& aSize)
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

bool CoordinateSubset::operator==(const CoordinateSubset& aCoordinateSubset) const
{
    return this->id_ == aCoordinateSubset.id_;
}

bool CoordinateSubset::operator!=(const CoordinateSubset& aCoordinateSubset) const
{
    return !((*this) == aCoordinateSubset);
}

String CoordinateSubset::getId() const
{
    return id_;
}

String CoordinateSubset::getName() const
{
    return name_;
}

Size CoordinateSubset::getSize() const
{
    return size_;
}

VectorXd CoordinateSubset::add(
    [[maybe_unused]] const Instant& anInstant,
    [[maybe_unused]] const VectorXd& aFullCoordinatesVector,
    [[maybe_unused]] const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    throw ostk::core::error::runtime::Wrong("Addition operation");
}

VectorXd CoordinateSubset::subtract(
    [[maybe_unused]] const Instant& anInstant,
    [[maybe_unused]] const VectorXd& aFullCoordinatesVector,
    [[maybe_unused]] const VectorXd& anotherFullCoordinatesVector,
    [[maybe_unused]] const Shared<const Frame>& aFrameSPtr,
    [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBrokerSPtr
) const
{
    throw ostk::core::error::runtime::Wrong("Subtraction operation");
}

VectorXd CoordinateSubset::inFrame(
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

Shared<const CoordinateSubset> CoordinateSubset::Mass()
{
    static const Shared<const CoordinateSubset> mass = std::make_shared<CoordinateSubset>("MASS", 1);
    return mass;
}

Shared<const CoordinateSubset> CoordinateSubset::SurfaceArea()
{
    static const Shared<const CoordinateSubset> surfaceArea = std::make_shared<CoordinateSubset>("SURFACE_AREA", 1);
    return surfaceArea;
}

Shared<const CoordinateSubset> CoordinateSubset::DragCoefficient()
{
    static const Shared<const CoordinateSubset> dragCoefficient =
        std::make_shared<CoordinateSubset>("DRAG_COEFFICIENT", 1);
    return dragCoefficient;
}

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
