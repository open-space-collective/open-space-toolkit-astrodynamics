////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/State.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_State__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_State__

#include <Library/Physics/Coordinate/Frame.hpp>
#include <Library/Physics/Coordinate/Velocity.hpp>
#include <Library/Physics/Coordinate/Position.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Types/Shared.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace trajectory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Shared ;

using ostk::physics::time::Instant ;
using ostk::physics::coord::Position ;
using ostk::physics::coord::Velocity ;
using ostk::physics::coord::Frame ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Trajectory state

class State
{

    public:

                                State                                       (   const   Instant&                    anInstant,
                                                                                const   Position&                   aPosition,
                                                                                const   Velocity&                   aVelocity                                   ) ;

        bool                    operator ==                                 (   const   State&                      aState                                      ) const ;

        bool                    operator !=                                 (   const   State&                      aState                                      ) const ;

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   State&                      aState                                      ) ;

        bool                    isDefined                                   ( ) const ;

        const Instant&          accessInstant                               ( ) const ;

        const Position&         accessPosition                              ( ) const ;

        const Velocity&         accessVelocity                              ( ) const ;

        Instant                 getInstant                                  ( ) const ;

        Position                getPosition                                 ( ) const ;

        Velocity                getVelocity                                 ( ) const ;

        State                   inFrame                                     (   const   Shared<const Frame>&        aFrameSPtr                                  ) const ;

        static State            Undefined                                   ( ) ;

    private:

        Instant                 instant_ ;
        Position                position_ ;
        Velocity                velocity_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
