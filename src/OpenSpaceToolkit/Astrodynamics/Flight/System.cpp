/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>

namespace ostk
{
namespace astro
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
    if ((!this->isDefined()) || (!aSystem.isDefined()))
    {
        return false;
    }

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

bool System::isDefined() const
{
    return mass_.isDefined() && geometry_.isDefined();
}

Mass System::getMass() const
{
    if (!mass_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Mass");
    }

    return mass_;
}

void System::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "System") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Mass:" << (mass_.isDefined() ? mass_.toString() : "Undefined");

    ostk::core::utils::Print::Separator(anOutputStream, "Geometry");
    geometry_.print(anOutputStream, false);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Composite System::getGeometry() const
{
    if (!geometry_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Geometry");
    }

    return geometry_;
}

System System::Undefined()
{
    return {Mass::Undefined(), Composite::Undefined()};
}

}  // namespace flight
}  // namespace astro
}  // namespace ostk
