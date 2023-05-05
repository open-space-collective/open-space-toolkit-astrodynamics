// Copyright © Loft Orbital Solutions Inc.

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem__
#define __OpenSpaceToolkit_Astrodynamics_Flight_System_SatelliteSystem__

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace system
{

using ostk::core::types::Real;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;

using ostk::physics::units::Mass;

using ostk::astro::flight::System;

/// @brief                      Defines the dynamics system who's motion is being studied, in particular this is a
/// satellite system

class SatelliteSystem : public System
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     Mass mass = { ... } ;
    ///                     Composite composite ( ... ) ;
    ///                     Matrix3d intertiaTensor ( ... ) ;
    ///                     Real crossSectionalSurfaceArea = 0.8 ;
    ///                     Real dragCoefficient = 2.2 ;
    ///                     System system = { mass, composite, intertiaTensor, crossSectionalSurfaceArea,
    ///                     dragCoefficient } ;
    /// @endcode
    ///
    /// @param              [in] aMass A mass
    /// @param              [in] aCompositeGeometry A composite geometry
    /// @param              [in] anInertiaTensor An inertia tensor
    /// @param              [in] aCrossSectionalSurfaceArea A cross sectional surface area
    /// @param              [in] aDragCoefficient A drag coefficient

    SatelliteSystem(
        const Mass& aMass,
        const Composite& aSatelliteGeometry,
        const Matrix3d& anInertiaTensor,
        const Real& aCrossSectionalSurfaceArea,
        const Real& aDragCoefficient
    );

    /// @brief              Copy Constructor
    ///
    /// @param              [in] aSatelliteSystem A satellite system

    SatelliteSystem(const SatelliteSystem& aSatelliteSystem);

    /// @brief              Destructor

    virtual ~SatelliteSystem() override;

    /// @brief              Clone satellite system
    ///
    /// @return             Pointer to cloned satellite system

    SatelliteSystem* clone() const;

    /// @brief              Equal to operator
    ///
    /// @param              [in] aSatelliteSystem A satellite system
    /// @return             True if satellite systems are equal

    bool operator==(const SatelliteSystem& aSatelliteSystem) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aSatelliteSystem A satellite system
    /// @return             True if satellite systems are not equal

    bool operator!=(const SatelliteSystem& aSatelliteSystem) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aSatelliteSystem A satellite system
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const SatelliteSystem& aSatelliteSystem);

    /// @brief              Check if satellite system is defined
    ///
    /// @return             True if satellite system is defined

    virtual bool isDefined() const override;

    /// @brief              Print satellite system
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief              Get satellite system's inertia tensor
    ///
    /// @code
    ///                     Matrix3d inertiaTensor = satelliteSystem.getInertiaTensor() ;
    /// @endcode
    ///
    /// @return             Matrix3d

    Matrix3d getInertiaTensor() const;

    /// @brief              Get satellite system's surface area
    ///
    /// @code
    ///                     Real surfaceArea = satelliteSystem.getCrossSectionalSurfaceArea() ;
    /// @endcode
    ///
    /// @return             Real

    Real getCrossSectionalSurfaceArea() const;

    /// @brief              Get satellite system's drag coefficient
    ///
    /// @code
    ///                     Real dragCoefficient = satelliteSystem.getDragCoefficient() ;
    /// @endcode
    ///
    /// @return             Real

    Real getDragCoefficient() const;

   private:
    Matrix3d inertiaTensor_;
    Real crossSectionalSurfaceArea_;
    Real dragCoefficient_;
};

}  // namespace system
}  // namespace flight
}  // namespace astro
}  // namespace ostk

#endif
