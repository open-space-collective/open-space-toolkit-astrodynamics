////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/System.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Dynamics_System__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Dynamics_System__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{
namespace dynamics
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Real ;

using ostk::physics::units::Mass ;
using ostk::physics::time::Instant ;
using ostk::physics::Environment ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Flight dynamics system

class System
{

    public:

                                System                                      (   const   Mass&                       aMass,
                                                                                const   Environment&                anEnvironment                               ) ;

                                System                                      (   const   System&                     aSystem                                     ) ;

                                ~System                                     ( ) ;

        System&                 operator =                                  (   const   System&                     aSystem                                     ) ;

        void                    operator ()                                 (   const   std::vector<double>&        aState,
                                                                                        std::vector<double>&        aDerivative,
                                                                                const   double                      aTime                                       ) ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   System&                     aSystem                                     ) ;

        bool                    isDefined                                   ( ) const ;

        Mass                    getMass                                     ( ) const ;

        Environment             getEnvironment                              ( ) const ;

        Instant                 getInstant                                  ( ) const ;

        std::vector<double>     getState                                    ( ) const ;

        // VectorXd                calculateDerivative                         (   const   Instant&                    anInstant,
        //                                                                         const   VectorXd&                   aState                                      ) const ;

        void                    print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        std::vector<double>&    accessState                                 ( ) ;

        void                    setInstant                                  (   const   Instant&                    anInstant                                   ) ;

        void                    setState                                    (   const   std::vector<double>&        aState                                      ) ;

        static System           Undefined                                   ( ) ;

    private:

        Mass                    mass_ ;
        Environment             environment_ ;

        Instant                 instant_ ;
        std::vector<double>     state_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
