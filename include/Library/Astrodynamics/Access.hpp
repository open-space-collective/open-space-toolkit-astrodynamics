////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Access.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Access__
#define __Library_Astrodynamics_Access__

#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Duration.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/String.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::String ;
using library::core::ctnr::Array ;

using library::physics::time::Instant ;
using library::physics::time::Duration ;
using library::physics::time::Interval ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Object-to-object visibility

class Access
{

    public:

        enum class Type
        {

            Undefined,
            Complete,
            Partial

        } ;

                                Access                                      (   const   Access::Type&               aType,
                                                                                const   Instant&                    anAcquisitionOfSignal,
                                                                                const   Instant&                    aTimeOfClosestApproach,
                                                                                const   Instant&                    aLossOfSignal                               ) ;

        bool                    operator ==                                 (   const   Access&                     anAccess                                    ) const ;

        bool                    operator !=                                 (   const   Access&                     anAccess                                    ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Access&                     anAccess                                    ) ;

        bool                    isDefined                                   ( ) const ;

        bool                    isComplete                                  ( ) const ;

        Access::Type            getType                                     ( ) const ;

        Instant                 getAcquisitionOfSignal                      ( ) const ;

        Instant                 getTimeOfClosestApproach                    ( ) const ;

        Instant                 getLossOfSignal                             ( ) const ;

        Interval                getInterval                                 ( ) const ;

        Duration                getDuration                                 ( ) const ;

        static Access           Undefined                                   ( ) ;

        static String           StringFromType                              (   const   Access::Type&               aType                                       ) ;

    private:

        Access::Type            type_ ;

        Instant                 acquisitionOfSignal_ ;
        Instant                 timeOfClosestApproach_ ;
        Instant                 lossOfSignal_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////