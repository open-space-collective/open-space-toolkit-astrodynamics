////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Dynamics/System.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Flight_Dynamics_System__
#define __Library_Astrodynamics_Flight_Dynamics_System__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Physics/Time/Instant.hpp>
#include <Library/Physics/Units/Mass.hpp>

#include <Library/Core/Types/Real.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace flight
{
namespace dynamics
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Real ;

using library::physics::units::Mass ;

using library::physics::time::Instant ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Flight dynamics system

class System
{

    public:

                                System                                      (   const   Mass&                       aMass                                       ) ;

                                System                                      (   const   Mass&                       aMass,
                                                                                const   Instant&                    anInstant,
                                                                                const   std::vector<double>&        aState                                      ) ;

                                System                                      (   const   System&                     aSystem                                     ) ;

                                ~System                                     ( ) ;

        System&                 operator =                                  (   const   System&                     aSystem                                     ) ;

        void                    operator ()                                 (   const   std::vector<double>&        aState,
                                                                                        std::vector<double>&        aDerivative,
                                                                                const   double                      aTime                                       ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   System&                     aSystem                                     ) ;

        bool                    isDefined                                   ( ) const ;

        Mass                    getMass                                     ( ) const ;

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
