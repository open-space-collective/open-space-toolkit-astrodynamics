/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystemBuilder__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

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

using ostk::core::types::Real;

using ostk::math::geometry::d3::objects::Composite;
using ostk::math::geometry::d3::objects::Cuboid;
using ostk::math::geometry::d3::objects::Point;
using ostk::math::object::Matrix3d;
using ostk::math::object::Vector3d;

using ostk::physics::units::Mass;

using ostk::astro::flight::System;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;

/// @brief                      Satellite System builder

class SatelliteSystemBuilder
{
   public:
    /// @brief                  Constructor

    SatelliteSystemBuilder();

    /// @brief                  Destructor

    ~SatelliteSystemBuilder();

    /// @brief                  Clone satellite system builder
    ///
    /// @return                 Pointer to cloned satellite system builder

    SatelliteSystemBuilder* clone() const;

    /// @brief                  Output stream operator
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] aSatelliteSystemBuilder A satellite system builder
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(
        std::ostream& anOutputStream, const SatelliteSystemBuilder& aSatelliteSystemBuilder
    );

    /// @brief                  Set the dry mass
    ///
    /// @param                  [in] aDryMass A dry mass
    /// @return                 The builder

    SatelliteSystemBuilder& withDryMass(const Mass& aDryMass);

    /// @brief                  Set the geometry
    ///
    /// @param                  [in] aGeometry A geometry
    /// @return                 The builder

    SatelliteSystemBuilder& withGeometry(const Composite& aGeometry);

    /// @brief                  Set the inertia tensor
    ///
    /// @param                  [in] aInertiaTensor A inertia tensor
    /// @return                 The builder

    SatelliteSystemBuilder& withInertiaTensor(const Matrix3d& anInertiaTensor);

    /// @brief                  Set the cross-sectional surface area
    ///
    /// @param                  [in] aCrossSectionalSurfaceArea A cross-sectional surface area
    /// @return                 The builder

    SatelliteSystemBuilder& withCrossSectionalSurfaceArea(const Real& aCrossSectionalSurfaceArea);

    /// @brief                  Set the drag coefficient
    ///
    /// @param                  [in] aDragCoefficient A drag coefficient
    /// @return                 The builder

    SatelliteSystemBuilder& withDragCoefficient(const Real& aDragCoefficient);

    /// @brief                  Set the propulsion system
    ///
    /// @param                  [in] aPropulsionSystem A Propulsion System
    /// @return                 The builder

    SatelliteSystemBuilder& withPropulsionSystem(const PropulsionSystem& aPropulsionSystem);

    /// @brief                  Builds the satellite system
    ///
    /// @return                 A new satellite system

    const SatelliteSystem build() const;

    /// @brief                  Print satellite system builder
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Satellite system builder with default values
    ///
    /// @code
    ///                         SatelliteSystemBuilder satelliteSystemBuilder = SatelliteSystemBuilder::Default();
    /// @endcode
    ///
    /// @return                 Satellite system builder with default values

    static SatelliteSystemBuilder Default();

   private:
    Mass dryMass_ = Mass::Undefined();
    Composite geometry_ = Composite::Undefined();
    Matrix3d inertiaTensor_ = Matrix3d::Undefined();
    Real crossSectionalSurfaceArea_ = Real::Undefined();
    Real dragCoefficient_ = Real::Undefined();
    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
