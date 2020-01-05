////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4__

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Unique.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
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

using ostk::core::types::Unique ;
using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::types::String ;

using ostk::physics::time::Instant ;
using ostk::physics::units::Length ;
using ostk::physics::units::Derived ;
using ostk::physics::env::obj::Celestial ;

using ostk::astro::trajectory::State ;
using ostk::astro::trajectory::orbit::models::sgp4::TLE ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SGP4 : public ostk::astro::trajectory::orbit::Model
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
