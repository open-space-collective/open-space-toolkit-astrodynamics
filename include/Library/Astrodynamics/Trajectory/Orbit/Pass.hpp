////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Pass.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit_Pass__
#define __Library_Astrodynamics_Trajectory_Orbit_Pass__

#include <Library/Physics/Time/Interval.hpp>

#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Integer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Integer ;
using library::core::types::String ;

using library::physics::time::Interval ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      A revolution of an orbiting object
///
/// @ref                        http://help.agi.com/stk/11.3.0/index.htm#vo/sat_pass.htm

class Pass
{

    public:

        enum class Type
        {

            Undefined,
            Complete,
            Partial

        } ;

        enum class Phase
        {

            Undefined,
            Ascending,
            Descending

        } ;

        enum class Quarter
        {

            Undefined,
            First,
            Second,
            Third,
            Fourth

        } ;

                                Pass                                        (   const   Pass::Type&                 aType,
                                                                                const   Integer&                    aRevolutionNumber,
                                                                                const   Interval&                   anInterval                                  ) ;

        bool                    operator ==                                 (   const   Pass&                       aPass                                       ) const ;

        bool                    operator !=                                 (   const   Pass&                       aPass                                       ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Pass&                       aPass                                       ) ;

        bool                    isDefined                                   ( ) const ;

        bool                    isComplete                                  ( ) const ;

        Pass::Type              getType                                     ( ) const ;

        Integer                 getRevolutionNumber                         ( ) const ;

        Interval                getInterval                                 ( ) const ;

        static Pass             Undefined                                   ( ) ;

        static String           StringFromType                              (   const   Pass::Type&                 aType                                       ) ;

        static String           StringFromPhase                             (   const   Pass::Phase&                aPhase                                      ) ;

        static String           StringFromQuarter                           (   const   Pass::Quarter&              aQuarter                                    ) ;

    private:

        Pass::Type              type_ ;

        Integer                 revolutionNumber_ ;
        Interval                interval_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////