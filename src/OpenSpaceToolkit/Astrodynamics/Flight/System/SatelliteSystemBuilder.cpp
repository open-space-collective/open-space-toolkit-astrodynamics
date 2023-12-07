/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystemBuilder.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

SatelliteSystemBuilder::SatelliteSystemBuilder() {}

SatelliteSystemBuilder& SatelliteSystemBuilder::setDryMass(const Mass& aDryMass)
{
    dryMass_ = aDryMass;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::setGeometry(const Composite& aGeometry)
{
    geometry_ = aGeometry;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::setInertiaTensor(const Matrix3d& anInertiaTensor)
{
    inertiaTensor_ = anInertiaTensor;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::setCrossSectionalSurfaceArea(const Real& aCrossSectionalSurfaceArea)
{
    crossSectionalSurfaceArea_ = aCrossSectionalSurfaceArea;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::setDragCoefficient(const Real& aDragCoefficient)
{
    dragCoefficient_ = aDragCoefficient;
    return *this;
}

SatelliteSystemBuilder& SatelliteSystemBuilder::setPropulsionSystem(const PropulsionSystem& aPropulsionSystem)
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

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk
