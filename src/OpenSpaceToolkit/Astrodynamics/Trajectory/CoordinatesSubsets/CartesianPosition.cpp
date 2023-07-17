/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/CartesianPosition.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace coordinatessubsets
{

using ostk::math::obj::Vector3d;

using ostk::physics::coord::Position;

const String CartesianPosition::DEFAULT_NAME = "CARTESIAN_POSITION";

CartesianPosition::CartesianPosition(const String& aName, const Size& aSize)
    : CoordinatesSubset(aName, aSize)
{
    if (aSize > 3)
    {
        throw ostk::core::error::runtime::Wrong("Size");
    }
}

CartesianPosition::CartesianPosition(const Size& aSize)
    : CartesianPosition(CartesianPosition::DEFAULT_NAME, aSize)
{
}

VectorXd CartesianPosition::inFrame(
    const Instant& anInstant,
    const VectorXd& allCoordinates,
    const Shared<const Frame>& fromFrame,
    const Shared<const CoordinatesBroker>& aCoordinatesBroker,
    const Shared<const Frame>& toFrame
) const
{
    if (this->getSize() != 3)
    {
        throw ostk::core::error::runtime::ToBeImplemented("Frame Transformation");
    }

    VectorXd positionCoordinates = aCoordinatesBroker->extract(allCoordinates, *this);

    Vector3d toFrameCoordinates =
        Position::Meters({positionCoordinates(0), positionCoordinates(1), positionCoordinates(2)}, fromFrame)
            .inFrame(toFrame, anInstant)
            .getCoordinates();

    return VectorXd::Map(toFrameCoordinates.data(), static_cast<Eigen::Index>(3));
}

CartesianPosition CartesianPosition::OneDimensional()
{
    return {1};
}

CartesianPosition CartesianPosition::TwoDimensional()
{
    return {2};
}

CartesianPosition CartesianPosition::ThreeDimensional()
{
    return {3};
}

}  // namespace coordinatessubsets
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
