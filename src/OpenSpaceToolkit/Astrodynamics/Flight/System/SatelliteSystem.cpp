/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

SatelliteSystem::SatelliteSystem(
    const Mass& aMass,
    const Composite& aSatelliteGeometry,
    const Matrix3d& anInertiaTensor,
    const Real& aCrossSectionalSurfaceArea,
    const Real& aDragCoefficient,
    const PropulsionSystem& aPropulsionSystem
)
    : System(aMass, aSatelliteGeometry),
      inertiaTensor_(anInertiaTensor),
      crossSectionalSurfaceArea_(aCrossSectionalSurfaceArea),
      dragCoefficient_(aDragCoefficient),
      propulsionModel_(aPropulsionSystem)
{
}

SatelliteSystem::~SatelliteSystem() {}

SatelliteSystem* SatelliteSystem::clone() const
{
    return new SatelliteSystem(*this);
}

bool SatelliteSystem::operator==(const SatelliteSystem& aSatelliteSystem) const
{
    if ((!this->isDefined()) || (!aSatelliteSystem.isDefined()))
    {
        return false;
    }

    return (System::operator==(aSatelliteSystem)) && (inertiaTensor_ == aSatelliteSystem.inertiaTensor_) &&
           (crossSectionalSurfaceArea_ == aSatelliteSystem.crossSectionalSurfaceArea_) &&
           (dragCoefficient_ == aSatelliteSystem.dragCoefficient_);
}

bool SatelliteSystem::operator!=(const SatelliteSystem& aSatelliteSystem) const
{
    return !((*this) == aSatelliteSystem);
}

std::ostream& operator<<(std::ostream& anOutputStream, const SatelliteSystem& aSatelliteSystem)
{
    aSatelliteSystem.print(anOutputStream);

    return anOutputStream;
}

bool SatelliteSystem::isDefined() const
{
    return System::isDefined() && inertiaTensor_.isDefined() && crossSectionalSurfaceArea_.isDefined() &&
           dragCoefficient_.isDefined();
}

void SatelliteSystem::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Satellite System") : void();

    System::print(anOutputStream, false);

    ostk::core::utils::Print::Line(anOutputStream)
        << "Inertia Tensor:" << (inertiaTensor_.isDefined() ? inertiaTensor_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Cross Sectional Surface Area:"
        << (crossSectionalSurfaceArea_.isDefined() ? crossSectionalSurfaceArea_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Drag Coefficient:" << (dragCoefficient_.isDefined() ? dragCoefficient_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Matrix3d SatelliteSystem::getInertiaTensor() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteSystem");
    }

    return inertiaTensor_;
}

Real SatelliteSystem::getCrossSectionalSurfaceArea() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteSystem");
    }

    return crossSectionalSurfaceArea_;
}

Real SatelliteSystem::getDragCoefficient() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("SatelliteSystem");
    }

    return dragCoefficient_;
}

PropulsionSystem SatelliteSystem::getPropulsionSystem() const
{
    if (!propulsionModel_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("PropulsionSystem");
    }

    return propulsionModel_;
}

SatelliteSystem SatelliteSystem::Undefined()
{
    return {
        Mass::Undefined(),
        Composite::Undefined(),
        Matrix3d::Zero(),
        Real::Undefined(),
        Real::Undefined(),
        PropulsionSystem::Undefined()
    };
}

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
