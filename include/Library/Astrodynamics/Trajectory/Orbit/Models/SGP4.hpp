////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit_Models_SGP4__
#define __Library_Astrodynamics_Trajectory_Orbit_Models_SGP4__

#include <Library/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <Library/Astrodynamics/Trajectory/State.hpp>

#include <Library/Physics/Environment/Objects/Celestial.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Length.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Integer.hpp>
#include <Library/Core/Types/Unique.hpp>

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Unique ;
using library::core::types::Integer ;
using library::core::types::Real ;
using library::core::types::String ;

using library::physics::time::Instant ;
using library::physics::units::Length ;
using library::physics::units::Derived ;
using library::physics::env::obj::Celestial ;

using library::astro::trajectory::State ;
using library::astro::trajectory::orbit::models::sgp4::TLE ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SGP4 : public library::astro::trajectory::orbit::Model
{

    public:

                                SGP4                                        (   const   TLE&                        aTle                                        ) ;

                                SGP4                                        (   const   SGP4&                       aSGP4Model                                  ) ;

                                ~SGP4                                       ( ) ;

        SGP4&                   operator =                                  (   const   SGP4&                       aSGP4Model                                  ) ;

        virtual SGP4*           clone                                       ( ) const override ;

        bool                    operator ==                                 (   const   SGP4&                       aSGP4Model                                  ) const ;

        bool                    operator !=                                 (   const   SGP4&                       aSGP4Model                                  ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   SGP4&                       aSGP4Model                                  ) ;

        virtual bool            isDefined                                   ( ) const override ;

        TLE                     getTle                                      ( ) const ;

        virtual Instant         getEpoch                                    ( ) const override ;

        virtual Integer         getRevolutionNumberAtEpoch                  ( ) const override ;

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        virtual Integer         calculateRevolutionNumberAt                 (   const   Instant&                    anInstant                                   ) const override ; // [TBR] ?

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;
                                                                                        
    protected:

        virtual bool            operator ==                                 (   const   trajectory::Model&          aModel                                      ) const override ;

        virtual bool            operator !=                                 (   const   trajectory::Model&          aModel                                      ) const override ;

    private:
    
        class Impl ;

        TLE                     tle_ ;

        Unique<SGP4::Impl>      implUPtr_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////