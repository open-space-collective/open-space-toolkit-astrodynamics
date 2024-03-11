/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

namespace ostk
{
namespace astrodynamics
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

using ostk::physics::unit::Mass;

using ostk::astrodynamics::flight::System;
using ostk::astrodynamics::flight::system::PropulsionSystem;

/// @brief Define the dynamics system who's motion is being studied, in particular this is a
/// satellite system
class SatelliteSystem : public System
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  Mass mass = { ... } ;
    ///                  Composite composite ( ... ) ;
    ///                  Matrix3d intertiaTensor ( ... ) ;
    ///                  Real crossSectionalSurfaceArea = 0.8 ;
    ///                  Real dragCoefficient = 2.2 ;
    ///                  PropulsionSystem propulsionSystem = { ... } ;
    ///                  System system = { mass, composite, intertiaTensor, crossSectionalSurfaceArea,
    ///                  dragCoefficient, propulsionSystem } ;
    /// @endcode
    ///
    /// @param aDryMass A dry mass (without propellant)
    /// @param aCompositeGeometry A composite geometry
    /// @param anInertiaTensor An inertia tensor
    /// @param aCrossSectionalSurfaceArea A cross sectional surface area
    /// @param aDragCoefficient A drag coefficient
    /// @param aPropulsionSystem A propulsion system (optional)
    SatelliteSystem(
        const Mass& aDryMass,
        const Composite& aSatelliteGeometry,
        const Matrix3d& anInertiaTensor,
        const Real& aCrossSectionalSurfaceArea,
        const Real& aDragCoefficient,
        const PropulsionSystem& aPropulsionSystem = PropulsionSystem::Undefined()
    );

    /// @brief Destructor
    virtual ~SatelliteSystem() override;

    /// @brief Clone satellite system
    ///
    /// @return Pointer to cloned satellite system
    SatelliteSystem* clone() const;

    /// @brief Equal to operator
    ///
    /// @param aSatelliteSystem A satellite system
    /// @return True if satellite systems are equal
    bool operator==(const SatelliteSystem& aSatelliteSystem) const;

    /// @brief Not equal to operator
    ///
    /// @param aSatelliteSystem A satellite system
    /// @return True if satellite systems are not equal
    bool operator!=(const SatelliteSystem& aSatelliteSystem) const;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aSatelliteSystem A satellite system
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const SatelliteSystem& aSatelliteSystem);

    /// @brief Check if satellite system is defined
    ///
    /// @return True if satellite system is defined
    virtual bool isDefined() const override;

    /// @brief Get satellite system's inertia tensor
    ///
    /// @code{.cpp}
    ///                  Matrix3d inertiaTensor = satelliteSystem.getInertiaTensor() ;
    /// @endcode
    ///
    /// @return Matrix3d
    Matrix3d getInertiaTensor() const;

    /// @brief Get satellite system's surface area
    ///
    /// @code{.cpp}
    ///                  Real surfaceArea = satelliteSystem.getCrossSectionalSurfaceArea() ;
    /// @endcode
    ///
    /// @return Real
    Real getCrossSectionalSurfaceArea() const;

    /// @brief Get satellite system's drag coefficient
    ///
    /// @code{.cpp}
    ///                  Real dragCoefficient = satelliteSystem.getDragCoefficient() ;
    /// @endcode
    ///
    /// @return Real
    Real getDragCoefficient() const;

    /// @brief Get satellite system's propulsion system
    ///
    /// @code{.cpp}
    ///                  PropulsionSystem propulsionSystem = satelliteSystem.getPropulsionSystem() ;
    /// @endcode
    ///
    /// @return PropulsionSystem
    PropulsionSystem getPropulsionSystem() const;

    /// @brief Print satellite system
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Undefined satellite system
    ///
    /// @code{.cpp}
    ///                  SatelliteSystem satelliteSystem = SatelliteSystem::Undefined();
    /// @endcode
    ///
    /// @return Undefined satellite system
    static SatelliteSystem Undefined();

    /// @brief Default satellite system
    ///
    /// @code{.cpp}
    ///                  SatelliteSystem satelliteSystem = SatelliteSystem::Default();
    /// @endcode
    ///
    /// @return Default satellite system
    static SatelliteSystem Default();

   private:
    Matrix3d inertiaTensor_;
    Real crossSectionalSurfaceArea_;
    Real dragCoefficient_;
    PropulsionSystem propulsionSystem_;
};

}  // namespace system
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
