/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_GenericPropagator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_GenericPropagator__

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/AbstractState.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Unique.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Unique ;
using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::ctnr::Array ;

using ostk::math::obj::Vector3d ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;
using ostk::physics::coord::Position ;
using ostk::physics::coord::Velocity ;

using ostk::astro::NumericalSolver ;
using ostk::astro::trajectory::AbstractState ;
using ostk::astro::flight::system::Dynamics ;

/// @brief                      Defines a generic propagator to be used for numerical integration

class GenericPropagator
{

    public:

        /// @brief              Constructor
        ///
        /// @code
        ///                     Propagator propagator = { aDynamics, aNumericalSolver } ;
        /// @endcode
        ///
        /// @param              [in] aDynamics A dynamics object
        /// @param              [in] aNumericalSolver A numerical solver

                                GenericPropagator                           (   const   Dynamics&                   aDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver                            ) ;

        /// @brief              Clone propagator
        ///
        /// @return             Pointer to cloned propagator

        // GenericPropagator*      clone                                       ( ) const ;

        /// @brief              Output stream operator
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] aGenericPropagator A propagator
        /// @return             A reference to output stream

        // friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
        //                                                                         const   GenericPropagator&          aGenericPropagator                          ) ;

        /// @brief              Check if propagator is defined
        ///
        /// @return             True if propagator is defined

        bool                    isDefined                                   ( ) const ;

        /// @brief              Calculate the state at an instant, given initial state
        /// @code
        ///                     AbstractState state = propagator.calculateStateAt(aState, anInstant) ;
        /// @endcode
        /// @param              [in] anAbstractState An initial state
        /// @param              [in] anInstant An instant
        /// @return             State

        AbstractState           calculateStateAt                            (   const   AbstractState&              aState,
                                                                                const   Instant&                    anInstant                                   ) const ;

        /// @brief              Calculate the states at an array of instants, given an initial state
        /// @brief              Can only be used with sorted instant array
        /// @code
        ///                     Array<anAbstractState> states = propagator.calculateStatesAt(aState, anInstantArray) ;
        /// @endcode
        /// @param              [in] anInstantArray An instant array
        /// @return             Array<anAbstractState>

        Array<AbstractState>    calculateStatesAt                           (   const   AbstractState&              aState,
                                                                                const   Array<Instant>&             anInstantArray                              ) const ;

    private:

        Unique<Dynamics>        dynamicsUPtr_ ;
        mutable NumericalSolver numericalSolver_ ;

} ;

}
}
}

#endif
