/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

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
    if (aName.isEmpty())
    {
        throw ostk::core::error::runtime::Wrong("Name");
    }

    if (aSize == 0)
    {
        throw ostk::core::error::runtime::Wrong("Size");
    }

    this->id_ = aName + " | " + std::to_string(aSize);
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
    [[maybe_unused]] const VectorXd& allCoordinates_1,
    [[maybe_unused]] const VectorXd& allCoordinates_2,
    [[maybe_unused]] const Shared<const Frame>& aFrame,
    [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    throw ostk::core::error::runtime::ToBeImplemented("Addition");
}

VectorXd CoordinatesSubset::subtract(
    [[maybe_unused]] const Instant& anInstant,
    [[maybe_unused]] const VectorXd& allCoordinates_1,
    [[maybe_unused]] const VectorXd& allCoordinates_2,
    [[maybe_unused]] const Shared<const Frame>& aFrame,
    [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    throw ostk::core::error::runtime::ToBeImplemented("Subtraction");
}

VectorXd CoordinatesSubset::inFrame(
    [[maybe_unused]] const Instant& anInstant,
    [[maybe_unused]] const VectorXd& allCoordinates,
    [[maybe_unused]] const Shared<const Frame>& fromFrame,
    [[maybe_unused]] const Shared<const Frame>& toFrame,
    [[maybe_unused]] const Shared<const CoordinatesBroker>& aCoordinatesBroker
) const
{
    throw ostk::core::error::runtime::ToBeImplemented("Frame transformation");
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
