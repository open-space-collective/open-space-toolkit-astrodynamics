////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/FullPropagator.hpp
/// @author         Vishwa Shah <vishwa@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_FullPropagator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_FullPropagator__

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/FullSatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>
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
#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace trajectory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::ctnr::Array ;
using ostk::core::types::Shared ;

using ostk::math::obj::Vector3d ;
using ostk::math::obj::VectorXd ;
using ostk::math::geom::d3::trf::rot::Quaternion ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;
using ostk::physics::coord::Position ;
using ostk::physics::coord::Velocity ;
using ostk::physics::coord::Frame ;

using ostk::astro::NumericalSolver ;
using FullState = ostk::astro::flight::profile::State ;
using ostk::astro::flight::system::dynamics::FullSatelliteDynamics ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines a propagator to be used for numerical integration

class FullPropagator
{

    public:

        /// @brief              Constructor
        ///
        /// @code
        ///                     FullPropagator propagator = { aSatelliteDynamics, aNumericalSolver } ;
        /// @endcode
        ///
        /// @param              [in] aSatelliteDynamics A satellite dynamics object
        /// @param              [in] aNumericalSolver A numerical solver

                                FullPropagator                              (   const   FullSatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver                            ) ;

        /// @brief              Clone propagator
        ///
        /// @return             Pointer to cloned propagator

        FullPropagator*             clone                                       ( ) const ;

        /// @brief              Equal to operator
        ///
        /// @param              [in] aFullPropagator A propagator
        /// @return             True if propagators are equal

        bool                    operator ==                                 (   const   FullPropagator&                 aFullPropagator                                 ) const ;

        /// @brief              Not equal to operator
        ///
        /// @param              [in] aFullPropagator A propagator
        /// @return             True if propagators are not equal

        bool                    operator !=                                 (   const   FullPropagator&                 aFullPropagator                                 ) const ;

        /// @brief              Output stream operator
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] aFullPropagator A propagator
        /// @return             A reference to output stream

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   FullPropagator&                 aFullPropagator                                 ) ;

        /// @brief              Check if propagator is defined
        ///
        /// @return             True if propagator is defined

        bool                    isDefined                                   ( ) const ;

        /// @brief              Calculate the state at an instant, given initial state
        /// @code
        ///                     State state = propagator.calculateStateAt(aState, anInstant) ;
        /// @endcode
        /// @param              [in] aState An initial state
        /// @param              [in] anInstant An instant
        /// @return             State

        FullState                   calculateStateAt                            (   const   FullState&                      aState,
                                                                                const   Instant&                    anInstant                                   ) const ;

        /// @brief              Calculate the states at an array of instants, given an initial state
        /// @brief              Can only be used with sorted instant array
        /// @code
        ///                     Array<State> states = propagator.calculateStatesAt(aState, anInstantArray) ;
        /// @endcode
        /// @param              [in] anInstantArray An instant array
        /// @return             Array<State>

        Array<FullState>            calculateStatesAt                           (   const   FullState&                      aState,
                                                                                const   Array<Instant>&             anInstantArray                              ) const ;

        /// @brief              Print propagator
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] (optional) displayDecorators If true, display decorators

        void                    print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        /// @brief              Create a medium fidelity FullPropagator object with recommended settings
        ///
        /// @code
        ///                     FullPropagator propagator = FullPropagator::MediumFidelity(aState) ;
        /// @endcode
        /// @param              [in] aState A State
        /// @return             FullPropagator

        static FullPropagator       MediumFidelity                              (   ) ;

        /// @brief              Create a high fidelity FullPropagator object with recommended settings
        ///
        /// @code
        ///                     FullPropagator propagator = FullPropagator::HighFidelity(aState) ;
        /// @endcode
        /// @param              [in] aState A State
        /// @return             FullPropagator

        static FullPropagator       HighFidelity                                (   ) ;

    private:

        mutable FullSatelliteDynamics satelliteDynamics_ ;
        mutable NumericalSolver numericalSolver_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
