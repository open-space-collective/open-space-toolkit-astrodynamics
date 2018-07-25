////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit__
#define __Library_Astrodynamics_Trajectory_Orbit__

#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Pass.hpp>
#include <Library/Astrodynamics/Trajectory/State.hpp>
#include <Library/Astrodynamics/Trajectory.hpp>

#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Containers/Map.hpp>
#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/Integer.hpp>
#include <Library/Core/Types/Index.hpp>
#include <Library/Core/Types/Unique.hpp>

#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Unique ;
using library::core::types::Index ;
using library::core::types::Integer ;
using library::core::ctnr::Array ;
using library::core::ctnr::Map ;

using library::physics::time::Instant ;

using library::astro::Trajectory ;
using library::astro::trajectory::State ;
using library::astro::trajectory::orbit::Pass ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Gravitationally curved trajectory of an object
///
/// @ref                        https://en.wikipedia.org/wiki/Orbit

class Orbit : public Trajectory
{

    public:

        typedef                 Array<Pass>::ConstIterator                      ConstPassIterator ;

                                Orbit                                       (   const   orbit::Model&               aModel                                      ) ;

                                Orbit                                       (   const   Array<State>&               aStateArray,
                                                                                const   Integer&                    anInitialRevolutionNumber                   =   1 ) ;

                                Orbit                                       (   const   Orbit&                      anOrbit                                     ) ;

        Orbit&                  operator =                                  (   const   Orbit&                      anOrbit                                     ) = delete ;

        bool                    operator ==                                 (   const   Orbit&                      anOrbit                                     ) const ;

        bool                    operator !=                                 (   const   Orbit&                      anOrbit                                     ) const ;

        bool                    isDefined                                   ( ) const ;

        Integer                 getRevolutionNumberAt                       (   const   Instant&                    anInstant                                   ) const ;

        Pass                    getPassAt                                   (   const   Instant&                    anInstant                                   ) const ;

        Pass                    getPassWithRevolutionNumber                 (   const   Integer&                    aRevolutionNumber                           ) const ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        static Orbit            Undefined                                   ( ) ;

    private:

        const orbit::Model&     model_ ;

        mutable std::mutex      mutex_ ;
        mutable Map<Integer, Pass> passMap_ ;

        static Map<Index, Pass> GeneratePassMap                             (   const   Array<State>&               aStateArray,
                                                                                const   Integer&                    anInitialRevolutionNumber                   ) ;

        static Array<State>     GenerateStates                              (   const   Model&                      aModel,
                                                                                const   Array<Instant>&             anInstantGrid                               ) ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////