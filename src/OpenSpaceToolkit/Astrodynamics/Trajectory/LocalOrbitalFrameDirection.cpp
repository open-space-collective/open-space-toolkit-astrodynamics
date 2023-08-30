/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Real;

LocalOrbitalFrameDirection::LocalOrbitalFrameDirection(
    const Vector3d& aValue, const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
)
    : value_(aValue.normalized()),
      localOrbitalFrameFactorySPtr_(aLocalOrbitalFrameFactorySPtr)
{
    if (std::abs(aValue.norm() - 1.0) > Real::Epsilon())
    {
        throw ostk::core::error::RuntimeError("LocalOrbitalFrameDirection vector is not unitary [{}].", aValue.norm());
    }
}

bool LocalOrbitalFrameDirection::operator==(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const
{
    if ((!this->isDefined()) || (!aLocalOrbitalFrameDirection.isDefined()))
    {
        return false;
    }

    return (value_ == aLocalOrbitalFrameDirection.value_) &&
           (localOrbitalFrameFactorySPtr_ == aLocalOrbitalFrameDirection.localOrbitalFrameFactorySPtr_);
}

bool LocalOrbitalFrameDirection::operator!=(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const
{
    return !((*this) == aLocalOrbitalFrameDirection);
}

bool LocalOrbitalFrameDirection::isDefined() const
{
    return value_.isDefined() &&
           (localOrbitalFrameFactorySPtr_ != nullptr);  // && localOrbitalFrameFactorySPtr_->isDefined();
}

const Shared<const LocalOrbitalFrameFactory>& LocalOrbitalFrameDirection::accessLocalOrbitalFrameFactory() const
{
    return localOrbitalFrameFactorySPtr_;
}

Vector3d LocalOrbitalFrameDirection::getValue() const
{
    return value_;
}

Shared<const LocalOrbitalFrameFactory> LocalOrbitalFrameDirection::getLocalOrbitalFrameFactory() const
{
    return this->accessLocalOrbitalFrameFactory();
}

LocalOrbitalFrameDirection LocalOrbitalFrameDirection::Undefined()
{
    return {Vector3d::Undefined(), LocalOrbitalFrameFactory::Undefined()};
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
