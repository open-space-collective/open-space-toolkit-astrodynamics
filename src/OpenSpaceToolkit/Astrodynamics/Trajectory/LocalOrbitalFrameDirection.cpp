/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::type::Real;

LocalOrbitalFrameDirection::LocalOrbitalFrameDirection(
    const Vector3d& aValue, const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
)
    : value_(aValue.normalized()),
      localOrbitalFrameFactorySPtr_(aLocalOrbitalFrameFactorySPtr)
{
    if (std::abs(value_.norm() - 1.0) > Real::Epsilon())
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

std::ostream& operator<<(std::ostream& anOutputStream, const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection)
{
    aLocalOrbitalFrameDirection.print(anOutputStream);

    return anOutputStream;
}

bool LocalOrbitalFrameDirection::isDefined() const
{
    return value_.isDefined() && (localOrbitalFrameFactorySPtr_ != nullptr) &&
           localOrbitalFrameFactorySPtr_->isDefined();
}

void LocalOrbitalFrameDirection::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Local Orbital Frame Direction") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Value:" << (this->getValue().isDefined() ? this->getValue().toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream
    ) << "Local Orbital Frame Factory Type:"
      << (((this->getLocalOrbitalFrameFactory() != nullptr) && (this->getLocalOrbitalFrameFactory()->isDefined()))
              ? LocalOrbitalFrameTransformProvider::StringFromType(this->getLocalOrbitalFrameFactory()->getProviderType(
                ))
              : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Shared<const LocalOrbitalFrameFactory>& LocalOrbitalFrameDirection::accessLocalOrbitalFrameFactory() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Local orbital frame direction");
    }

    return localOrbitalFrameFactorySPtr_;
}

Vector3d LocalOrbitalFrameDirection::getValue() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Local orbital frame direction");
    }

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
