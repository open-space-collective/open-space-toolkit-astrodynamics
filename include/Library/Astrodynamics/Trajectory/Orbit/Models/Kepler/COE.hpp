////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE__
#define __Library_Astrodynamics_Trajectory_Orbit_Models_Kepler_COE__

#include <Library/Physics/Coordinate/Frame.hpp>
#include <Library/Physics/Coordinate/Velocity.hpp>
#include <Library/Physics/Coordinate/Position.hpp>
#include <Library/Physics/Time/Duration.hpp>
#include <Library/Physics/Units/Derived/Angle.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Length.hpp>

#include <Library/Core/Containers/Pair.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Shared.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{
namespace kepler
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Shared ;
using library::core::types::Real ;
using library::core::ctnr::Pair ;

using library::physics::units::Length ;
using library::physics::units::Derived ;
using library::physics::units::Angle ;
using library::physics::time::Duration ;
using library::physics::coord::Position ;
using library::physics::coord::Velocity ;
using library::physics::coord::Frame ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Classical Orbital Elements (COE)
///
/// @ref                        https://en.wikipedia.org/wiki/Orbital_elements
/// @ref                        http://help.agi.com/stk/index.htm#stk/vehSat_coordType_classical.htm

class COE
{

    public:

        typedef                 Pair<Position, Velocity>                        CartesianState ;

                                COE                                         (   const   Length&                     aSemiMajorAxis,
                                                                                const   Real&                       anEccentricity,
                                                                                const   Angle&                      anInclination,
                                                                                const   Angle&                      aRaan,
                                                                                const   Angle&                      anAop,
                                                                                const   Angle&                      aTrueAnomaly                                ) ;

        bool                    operator ==                                 (   const   COE&                        aCOE                                        ) const ;

        bool                    operator !=                                 (   const   COE&                        aCOE                                        ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   COE&                        aCOE                                        ) ;

        bool                    isDefined                                   ( ) const ;

        Length                  getSemiMajorAxis                            ( ) const ;

        Real                    getEccentricity                             ( ) const ;

        Angle                   getInclination                              ( ) const ;

        Angle                   getRaan                                     ( ) const ;

        Angle                   getAop                                      ( ) const ;

        Angle                   getTrueAnomaly                              ( ) const ;

        Angle                   getMeanAnomaly                              ( ) const ;
        
        Angle                   getEccentricAnomaly                         ( ) const ;

        Derived                 getMeanMotion                               (   const   Derived&                    aGravitationalParameter                     ) const ;

        Duration                getOrbitalPeriod                            (   const   Derived&                    aGravitationalParameter                     ) const ;

        COE::CartesianState     getCartesianState                           (   const   Derived&                    aGravitationalParameter,
                                                                                const   Shared<const Frame>&        aFrameSPtr                                  ) const ;

        void                    print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        static COE              Undefined                                   ( ) ;

        static COE              Cartesian                                   (   const   COE::CartesianState&        aCartesianState,
                                                                                const   Derived&                    aGravitationalParameter                     ) ;

        static Angle            EccentricAnomalyFromTrueAnomaly             (   const   Angle&                      aTrueAnomaly,
                                                                                const   Real&                       anEccentricity                              ) ;

        static Angle            TrueAnomalyFromEccentricAnomaly             (   const   Angle&                      anEccentricAnomaly,
                                                                                const   Real&                       anEccentricity                              ) ;

        static Angle            MeanAnomalyFromEccentricAnomaly             (   const   Angle&                      anEccentricAnomaly,
                                                                                const   Real&                       anEccentricity                              ) ;

        static Angle            EccentricAnomalyFromMeanAnomaly             (   const   Angle&                      aMeanAnomaly,
                                                                                const   Real&                       anEccentricity,
                                                                                const   Real&                       aTolerance                                  ) ;

    private:

        Length                  semiMajorAxis_ ;
        Real                    eccentricity_ ;
        Angle                   inclination_ ;
        Angle                   raan_ ;
        Angle                   aop_ ;
        Angle                   trueAnomaly_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////