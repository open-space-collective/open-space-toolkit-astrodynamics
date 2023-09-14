/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{
namespace dynamics
{

Thruster::Thruster(const SatelliteSystem& aSatelliteSystem, const String& aName)
    : name_(aName),
      satelliteSystem_(aSatelliteSystem)
{
}

Thruster::~Thruster() {}

String Thruster::getName() const
{
    return name_;
}

SatelliteSystem Thruster::getSatelliteSystem() const
{
    return satelliteSystem_;
}

void Thruster::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Thruster") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "Name:" << name_;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
