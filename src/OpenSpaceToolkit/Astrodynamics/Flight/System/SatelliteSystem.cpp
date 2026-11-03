/// Apache License 2.0

#include <cmath>
#include <limits>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Utility/String.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace system
{

SatelliteSystem::SatelliteSystem(
    const Mass& aDryMass,
    const Composite& aSatelliteGeometry,
    const Matrix3d& anInertiaTensor,
    double aCrossSectionalSurfaceArea,
    double aDragCoefficient,
    const PropulsionSystem& aPropulsionSystem
)
    : System(aDryMass, aSatelliteGeometry),
      inertiaTensor_(anInertiaTensor),
      crossSectionalSurfaceArea_(aCrossSectionalSurfaceArea),
      dragCoefficient_(aDragCoefficient),
      propulsionSystem_(aPropulsionSystem)
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
           (dragCoefficient_ == aSatelliteSystem.dragCoefficient_ &&
            propulsionSystem_ == aSatelliteSystem.propulsionSystem_);
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
    return System::isDefined() && inertiaTensor_.isDefined() && !std::isnan(crossSectionalSurfaceArea_) &&
           !std::isnan(dragCoefficient_) && propulsionSystem_.isDefined();
}

void SatelliteSystem::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Satellite System") : void();

    System::print(anOutputStream, false);

    ostk::core::utils::Print::Line(anOutputStream)
        << "Inertia Tensor:" << (inertiaTensor_.isDefined() ? inertiaTensor_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream
    ) << "Cross Sectional Surface Area:"
      << (!std::isnan(crossSectionalSurfaceArea_) ? utility::DoubleToString(crossSectionalSurfaceArea_) : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Drag Coefficient:"
        << (!std::isnan(dragCoefficient_) ? utility::DoubleToString(dragCoefficient_) : "Undefined");

    ostk::core::utils::Print::Separator(anOutputStream, "PropulsionSystem");
    propulsionSystem_.isDefined() ? propulsionSystem_.print(anOutputStream, false) : void();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const PropulsionSystem& SatelliteSystem::accessPropulsionSystem() const
{
    if (!propulsionSystem_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propulsion System");
    }

    return this->propulsionSystem_;
}

Matrix3d SatelliteSystem::getInertiaTensor() const
{
    if (!inertiaTensor_.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Inertia Tensor");
    }

    return inertiaTensor_;
}

double SatelliteSystem::getCrossSectionalSurfaceArea() const
{
    if (std::isnan(crossSectionalSurfaceArea_))
    {
        throw ostk::core::error::runtime::Undefined("Cross-Sectional Surface Area");
    }

    return crossSectionalSurfaceArea_;
}

double SatelliteSystem::getDragCoefficient() const
{
    if (std::isnan(dragCoefficient_))
    {
        throw ostk::core::error::runtime::Undefined("Drag Coefficient");
    }

    return dragCoefficient_;
}

PropulsionSystem SatelliteSystem::getPropulsionSystem() const
{
    return this->accessPropulsionSystem();
}

SatelliteSystem SatelliteSystem::Undefined()
{
    return {
        Mass::Undefined(),
        Composite::Undefined(),
        Matrix3d::Undefined(),
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::quiet_NaN(),
        PropulsionSystem::Undefined(),
    };
}

SatelliteSystem SatelliteSystem::Default()
{
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 2.0, 3.0}
    ));

    return {
        Mass::Kilograms(100.0),
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.2,
        PropulsionSystem::Default(),
    };
}

}  // namespace system
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
