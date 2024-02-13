/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace system
{

SatelliteSystemBuilder::SatelliteSystemBuilder()
    : dryMass_(Mass::Undefined()),
      geometry_(Composite::Undefined()),
      inertiaTensor_(Matrix3d::Undefined()),
      crossSectionalSurfaceArea_(Real::Undefined()),
      dragCoefficient_(Real::Undefined()),
      propulsionSystem_(PropulsionSystem::Undefined())
{
}

SatelliteSystemBuilder::~SatelliteSystemBuilder() {}

SatelliteSystemBuilder* SatelliteSystemBuilder::clone() const
{
    return new SatelliteSystemBuilder(*this);
}

std::ostream& operator<<(std::ostream& anOutputStream, const SatelliteSystemBuilder& aSatelliteSystemBuilder)
{
    aSatelliteSystemBuilder.print(anOutputStream);

    return anOutputStream;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::withDryMass(const Mass& aDryMass)
{
    dryMass_ = aDryMass;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::withGeometry(const Composite& aGeometry)
{
    geometry_ = aGeometry;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::withInertiaTensor(const Matrix3d& anInertiaTensor)
{
    inertiaTensor_ = anInertiaTensor;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::withCrossSectionalSurfaceArea(const Real& aCrossSectionalSurfaceArea)
{
    crossSectionalSurfaceArea_ = aCrossSectionalSurfaceArea;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::withDragCoefficient(const Real& aDragCoefficient)
{
    dragCoefficient_ = aDragCoefficient;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::withPropulsionSystem(const PropulsionSystem& aPropulsionSystem)
{
    propulsionSystem_ = aPropulsionSystem;
    return *this;
}

const SatelliteSystem SatelliteSystemBuilder::build() const
{
    return {
        dryMass_,
        geometry_,
        inertiaTensor_,
        crossSectionalSurfaceArea_,
        dragCoefficient_,
        propulsionSystem_,
    };
}

void SatelliteSystemBuilder::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Satellite System BUILDER") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Dry Mass:" << (dryMass_.isDefined() ? dryMass_.toString() : "Undefined");
    ostk::core::utils::Print::Separator(anOutputStream, "Geometry");
    geometry_.isDefined() ? geometry_.print(anOutputStream, false) : void();
    ostk::core::utils::Print::Line(anOutputStream)
        << "Inertia Tensor:" << (inertiaTensor_.isDefined() ? inertiaTensor_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Cross Sectional Surface Area:"
        << (crossSectionalSurfaceArea_.isDefined() ? crossSectionalSurfaceArea_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Drag Coefficient:" << (dragCoefficient_.isDefined() ? dragCoefficient_.toString() : "Undefined");
    ostk::core::utils::Print::Separator(anOutputStream, "PropulsionSystem");
    propulsionSystem_.isDefined() ? propulsionSystem_.print(anOutputStream, false) : void();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

SatelliteSystemBuilder SatelliteSystemBuilder::Default()
{
    const SatelliteSystem defaultSatelliteSystem = SatelliteSystem::Default();

    return SatelliteSystemBuilder()
        .withDryMass(defaultSatelliteSystem.getMass())
        .withGeometry(defaultSatelliteSystem.getGeometry())
        .withInertiaTensor(defaultSatelliteSystem.getInertiaTensor())
        .withCrossSectionalSurfaceArea(defaultSatelliteSystem.getCrossSectionalSurfaceArea())
        .withDragCoefficient(defaultSatelliteSystem.getDragCoefficient())
        .withPropulsionSystem(defaultSatelliteSystem.getPropulsionSystem());
}

}  // namespace system
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
