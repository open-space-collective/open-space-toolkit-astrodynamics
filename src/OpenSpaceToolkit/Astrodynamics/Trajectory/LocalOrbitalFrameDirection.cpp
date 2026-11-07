/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

LocalOrbitalFrameDirection::LocalOrbitalFrameDirection(
    const Vector3d& aValue, const LocalOrbitalFrameFactory& aLocalOrbitalFrameFactory
)
    : value_(aValue.normalized()),
      localOrbitalFrameFactory_(aLocalOrbitalFrameFactory)
{
    if (std::abs(value_.norm() - 1.0) > 1e-15)
    {
        throw ostk::core::error::RuntimeError("LocalOrbitalFrameDirection vector is not unitary [{}].", aValue.norm());
    }
}

bool LocalOrbitalFrameDirection::operator==(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const
{
    return (value_ == aLocalOrbitalFrameDirection.value_) &&
           (localOrbitalFrameFactory_ == aLocalOrbitalFrameDirection.localOrbitalFrameFactory_);
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

void LocalOrbitalFrameDirection::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Local Orbital Frame Direction") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Value:" << this->getValue().toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Local Orbital Frame Factory Type:"
        << LocalOrbitalFrameTransformProvider::StringFromType(localOrbitalFrameFactory_.getProviderType());

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Vector3d& LocalOrbitalFrameDirection::getValue() const
{
    return value_;
}

const LocalOrbitalFrameFactory& LocalOrbitalFrameDirection::getLocalOrbitalFrameFactory() const
{
    return localOrbitalFrameFactory_;
}

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
