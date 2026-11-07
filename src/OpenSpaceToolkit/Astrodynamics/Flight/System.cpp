/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{

System::System(const Mass& aMass, const Composite& aGeometry)

    : mass_(aMass),
      geometry_(aGeometry)
{
}

System::~System() {}

System* System::clone() const
{
    return new System(*this);
}

bool System::operator==(const System& aSystem) const
{
    return (mass_ == aSystem.mass_) && (geometry_ == aSystem.geometry_);
}

bool System::operator!=(const System& aSystem) const
{
    return !((*this) == aSystem);
}

std::ostream& operator<<(std::ostream& anOutputStream, const System& aSystem)
{
    aSystem.print(anOutputStream);

    return anOutputStream;
}

const Mass& System::getMass() const
{
    return mass_;
}

void System::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "System") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Mass:" << mass_.toString();

    ostk::core::utils::Print::Separator(anOutputStream, "Geometry");
    geometry_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Composite& System::getGeometry() const
{
    return geometry_;
}

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
