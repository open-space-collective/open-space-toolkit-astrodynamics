/// Apache License 2.0

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
    const std::optional<double>& aCrossSectionalSurfaceArea,
    const std::optional<double>& aDragCoefficient,
    const std::optional<PropulsionSystem>& aPropulsionSystem
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

void SatelliteSystem::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Satellite System") : void();

    System::print(anOutputStream, false);

    ostk::core::utils::Print::Line(anOutputStream) << "Inertia Tensor:" << inertiaTensor_.toString();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Cross Sectional Surface Area:"
        << (crossSectionalSurfaceArea_.has_value() ? utility::DoubleToString(crossSectionalSurfaceArea_.value())
                                                   : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Drag Coefficient:"
        << (dragCoefficient_.has_value() ? utility::DoubleToString(dragCoefficient_.value()) : "Undefined");

    ostk::core::utils::Print::Separator(anOutputStream, "PropulsionSystem");
    propulsionSystem_.has_value() ? propulsionSystem_.value().print(anOutputStream, false) : void();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

const Matrix3d& SatelliteSystem::getInertiaTensor() const
{
    return inertiaTensor_;
}

double SatelliteSystem::getCrossSectionalSurfaceArea() const
{
    if (!crossSectionalSurfaceArea_.has_value())
    {
        throw ostk::core::error::runtime::Undefined("Cross-Sectional Surface Area");
    }

    return crossSectionalSurfaceArea_.value();
}

double SatelliteSystem::getDragCoefficient() const
{
    if (!dragCoefficient_.has_value())
    {
        throw ostk::core::error::runtime::Undefined("Drag Coefficient");
    }

    return dragCoefficient_.value();
}

const PropulsionSystem& SatelliteSystem::getPropulsionSystem() const
{
    if (!propulsionSystem_.has_value())
    {
        throw ostk::core::error::runtime::Undefined("Propulsion System");
    }

    return propulsionSystem_.value();
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
