/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

using ostk::core::type::Real;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;

using ostk::physics::units::Mass;

using ostk::astro::flight::System;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;

/// @brief Satellite System builder, meant to simplify creation of a SatelliteSystem, by allowing
/// you to only specify the parameters you want. There are two ways of doing this:
///
/// @code{.cpp}
///                      SatelliteSystemBuilder().withDryMass(X).withArea(Y)
///
///                      SatelliteSystemBuilder::Default().withDryMass(X)
/// @endcode

class SatelliteSystemBuilder
{
   public:
    /// @brief Constructor
    SatelliteSystemBuilder();

    /// @brief Destructor
    ~SatelliteSystemBuilder();

    /// @brief Clone satellite system builder
    ///
    /// @return Pointer to cloned satellite system builder
    SatelliteSystemBuilder* clone() const;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aSatelliteSystemBuilder A satellite system builder
    /// @return A reference to output stream
    friend std::ostream& operator<<(
        std::ostream& anOutputStream, const SatelliteSystemBuilder& aSatelliteSystemBuilder
    );

    /// @brief Set the dry mass
    ///
    /// @param aDryMass A dry mass
    /// @return The builder
    SatelliteSystemBuilder& withDryMass(const Mass& aDryMass);

    /// @brief Set the geometry
    ///
    /// @param aGeometry A geometry
    /// @return The builder
    SatelliteSystemBuilder& withGeometry(const Composite& aGeometry);

    /// @brief Set the inertia tensor
    ///
    /// @param anInertiaTensor An inertia tensor
    /// @return The builder
    SatelliteSystemBuilder& withInertiaTensor(const Matrix3d& anInertiaTensor);

    /// @brief Set the cross-sectional surface area
    ///
    /// @param aCrossSectionalSurfaceArea A cross-sectional surface area
    /// @return The builder
    SatelliteSystemBuilder& withCrossSectionalSurfaceArea(const Real& aCrossSectionalSurfaceArea);

    /// @brief Set the drag coefficient
    ///
    /// @param aDragCoefficient A drag coefficient
    /// @return The builder
    SatelliteSystemBuilder& withDragCoefficient(const Real& aDragCoefficient);

    /// @brief Set the propulsion system
    ///
    /// @param aPropulsionSystem A Propulsion System
    /// @return The builder
    SatelliteSystemBuilder& withPropulsionSystem(const PropulsionSystem& aPropulsionSystem);

    /// @brief Builds the satellite system
    ///
    /// @return A new satellite system
    const SatelliteSystem build() const;

    /// @brief Print satellite system builder
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Satellite system builder with default values
    ///
    /// @code{.cpp}
    /// SatelliteSystemBuilder satelliteSystemBuilder = SatelliteSystemBuilder::Default();
    /// @endcode
    ///
    /// @return Satellite system builder with default values
    static SatelliteSystemBuilder Default();

   private:
    Mass dryMass_;
    Composite geometry_;
    Matrix3d inertiaTensor_;
    Real crossSectionalSurfaceArea_;
    Real dragCoefficient_;
    PropulsionSystem propulsionSystem_;
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
