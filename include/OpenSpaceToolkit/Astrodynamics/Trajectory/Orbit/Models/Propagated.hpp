////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagated__

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/SatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>
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

using ostk::core::types::Integer ;
using ostk::core::types::Real ;
using ostk::core::types::String ;
using ostk::core::ctnr::Array ;
using ostk::core::ctnr::Pair ;

using ostk::math::obj::Vector3d ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;
using ostk::physics::coord::Position ;
using ostk::physics::coord::Velocity ;

using ostk::astro::NumericalSolver ;
using ostk::astro::trajectory::State ;
using ostk::astro::trajectory::orbit::Model ;
using ostk::astro::flight::system::dynamics::SatelliteDynamics ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Defines an orbit model that is computed using numerical propagation

class Propagated : public ostk::astro::trajectory::orbit::Model
{

    public:

        /// @brief              Constructor
        ///
        /// @code
        ///                     Propagated propagated = { aSatelliteDynamics, aNumericalSolver } ;
        /// @endcode
        ///
        /// @param              [in] aSatelliteDynamics A satellite dynamics object
        /// @param              [in] aNumericalSolver A numerical solver

                                Propagated                                  (   const   SatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver                            ) ;

        /// @brief              Constructor with additional option of passing in an existing array of states
        ///
        /// @code
        ///                     Propagated propagated = { aSatelliteDynamics, aNumericalSolver, aCachedStateArray } ;
        /// @endcode
        ///
        /// @param              [in] aSatelliteDynamics A satellite dynamics object
        /// @param              [in] aNumericalSolver A numerical solver
        /// @param              [in] aCachedStateArray A state array

                                Propagated                                  (   const   SatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver,
                                                                                const   Array<State>&               aCachedStateArray                           ) ;

        /// @brief              Clone propagated
        ///
        /// @return             Pointer to cloned propagated

        virtual Propagated*     clone                                       ( ) const override ;

        /// @brief              Equal to operator
        ///
        /// @param              [in] aPropagatedModel A propagated
        /// @return             True if propagateds are equal

        bool                    operator ==                                 (   const   Propagated&                 aPropagatedModel                            ) const ;

        /// @brief              Not equal to operator
        ///
        /// @param              [in] aPropagatedModel A propagated
        /// @return             True if propagateds are not equal

        bool                    operator !=                                 (   const   Propagated&                 aPropagatedModel                            ) const ;

        /// @brief              Output stream operator
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] aPropagatedModel A propagated
        /// @return             A reference to output stream

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Propagated&                 aPropagatedModel                            ) ;

        /// @brief              Check if propagated is defined
        ///
        /// @return             True if propagated is defined

        virtual bool            isDefined                                   ( ) const override ;

        /// @brief              Get epoch (in this case it is the first instant in the cached state array)
        ///
        /// @code
        ///                     Instant instant = propagated.getEpoch() ;
        /// @endcode
        ///
        /// @return             Instant

        virtual Instant         getEpoch                                    ( ) const override ;

        /// @brief              Get revolution number at epoch (it is equal to 1)
        ///
        /// @code
        ///                     Real real = propagated.getRevolutionNumberAtEpoch() ;
        /// @endcode
        ///
        /// @return             Integer

        virtual Integer         getRevolutionNumberAtEpoch                  ( ) const override ;

        /// @brief              Calculate a state at an instant, utilizing internal cached state array to propagated shortest amount of time
        /// @brief              Does not have macro-level sorting optimization, should not be used with disorded instant array
        /// @code
        ///                     State state = propagated.calculateStateAt(anInstant) ;
        /// @endcode
        /// @param              [in] anInstant An instant
        /// @return             State

        virtual State           calculateStateAt                            (   const   Instant&                    anInstant                                   ) const override ;

        /// @brief              Calculate a state at an array of instants, utilizing internal cached state array to propagated shortest amount of time
        /// @brief              Has macro-level sorting optimization, can be used with disorded instant array
        /// @code
        ///                     State state = propagated.calculateStateAt(anInstant) ;
        /// @endcode
        /// @param              [in] anInstant An instant
        /// @return             State

        virtual Array<State>    calculateStatesAt                           (   const   Array<Instant>&             anInstantArray                              ) const override ;

        /// @brief              Calculate a revolution number at an instant
        ///
        /// @code
        ///                     Integer integer = propagated.calculateRevolutionNumberAt(anInstant) ;
        /// @endcode
        /// @param              [in] anInstant An instant
        /// @return             Integer

        virtual Integer         calculateRevolutionNumberAt                 (   const   Instant&                    anInstant                                   ) const override ;

        /// @brief              Fetch internal cached state array
        ///
        /// @code
        ///                     Array<State> stateArray = propagated.accessCachedStateArray() ;
        /// @endcode

        /// @return             Array<State>&

        const Array<State>&     accessCachedStateArray                      ( ) const ;

        /// @brief              Print propagated
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] (optional) displayDecorators If true, display decorators

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        /// @brief              Create a Medium fidelity Propagated object with recommended settings
        ///
        /// @code
        ///                     Propagated propagated = Propagated::MediumFidelity(aState) ;
        /// @endcode
        /// @param              [in] aState A State
        /// @return             Propagated

        static Propagated       MediumFidelity                              (   const   State&                      aState                                      ) ;

        /// @brief              Create a High fidelity Propagated object with recommended settings
        ///
        /// @code
        ///                     Propagated propagated = Propagated::HighFidelity(aState) ;
        /// @endcode
        /// @param              [in] aState A State
        /// @return             Propagated

        static Propagated       HighFidelity                                (   const   State&                      aState                                      ) ;

    protected:

        /// @brief              Equal to operator
        ///
        /// @param              [in] aModel A model
        /// @return             True if models are equal

        virtual bool            operator ==                                 (   const   trajectory::Model&          aModel                                      ) const override ;

        /// @brief              Not equal to operator
        ///
        /// @param              [in] aModel A model
        /// @return             True if models are not equal

        virtual bool            operator !=                                 (   const   trajectory::Model&          aModel                                      ) const override ;

    private:

        mutable Array<State>    cachedStateArray_ ;
        mutable SatelliteDynamics satelliteDynamics_ ;
        mutable NumericalSolver numericalSolver_ ;

        void                    sortAndSanitizeStateArray                   ( ) const ;

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
