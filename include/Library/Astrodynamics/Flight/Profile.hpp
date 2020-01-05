////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile__

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Providers/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Axes.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace flight
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::core::types::Shared ;
using ostk::core::types::String ;
using ostk::core::ctnr::Array ;

using ostk::math::obj::Vector3d ;
using ostk::math::geom::d3::trf::rot::Quaternion ;

using ostk::physics::time::Instant ;
using ostk::physics::time::Duration ;
using ostk::physics::time::Interval ;
using ostk::physics::coord::Frame ;
using ostk::physics::coord::Axes ;
using DynamicProvider = ostk::physics::coord::frame::provider::Dynamic ;

using ostk::astro::Trajectory ;
using ostk::astro::flight::profile::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Spacecraft flight profile

class Profile
{

    public:

        enum class PointingMode
        {

            Undefined,          ///< Undefined pointing mode
            Inertial,           ///< Inertial pointing mode (the spacecraft points to a celestial object)
            Nadir,              ///< Nadir pointing mode (the spacecraft points points "directly down")
            Target,             ///< Target pointing mode (the spacecraft points to a given target position)
            Custom              ///< Custom pointing mode

        } ;

        /// @brief              Constructor
        ///
        /// @param              [in] aDynamicTransformProvider A dynamic transform provider
        /// @param              [in] aFrameSPtr A shared pointer to reference frame

                                Profile                                     (   const   DynamicProvider&            aDynamicTransformProvider,
                                                                                const   Shared<const Frame>&        aFrameSPtr                                  ) ;

        /// @brief              Constructor
        ///
        /// @code
        ///                     Array<State> stateArray = { ... } ;
        ///                     Profile profile = { stateArray } ;
        /// @endcode
        ///
        /// @param              [in] aStateArray An array of states

                                Profile                                     (   const   Array<State>&               aStateArray                                 ) ;

        /// @brief              Copy constructor
        ///
        /// @param              [in] aProfile A flight profile

                                Profile                                     (   const   Profile&                    aProfile                                    ) = default ;

        /// @brief              Copy assignment operator
        ///
        /// @param              [in] aProfile A flight profile
        /// @return             Reference to flight profile

        Profile&                operator =                                  (   const   Profile&                    aProfile                                    ) = default ;

        /// @brief              Equal to operator
        ///
        /// @code
        ///                     Profile(...) == Profile(...) ;
        /// @endcode
        ///
        /// @param              [in] aProfile A flight profile
        /// @return             True if flight profiles are equal

        bool                    operator ==                                 (   const   Profile&                    aProfile                                    ) const ;

        /// @brief              Not equal to operator
        ///
        /// @code
        ///                     Profile(...) != Profile(...) ;
        /// @endcode
        ///
        /// @param              [in] aProfile A flight profile
        /// @return             True if flight profiles are not equal

        bool                    operator !=                                 (   const   Profile&                    aProfile                                    ) const ;

        /// @brief              Output stream operator
        ///
        /// @code
        ///                     std::cout << Profile(...) ;
        /// @endcode
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] aProfile A flight profile
        /// @return             A reference to output stream

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Profile&                    aProfile                                    ) ;

        /// @brief              Check if profile is defined
        ///
        /// @code
        ///                     Profile(...).isDefined() ;
        /// @endcode
        ///
        /// @return             True if profile is defined

        bool                    isDefined                                   ( ) const ;

        /// @brief              Get state at a given instant
        ///
        /// @code
        ///                     Profile profile = { ... } ;
        ///                     Instant instant = { ... } ;
        ///                     State state = profile.getStateAt(instant) ;
        /// @endcode
        ///
        /// @param              [in] anInstant An instant
        /// @return             State

        State                   getStateAt                                  (   const   Instant&                    anInstant                                   ) const ;

        /// @brief              Get states at a given instants
        ///
        /// @code
        ///                     Profile profile = { ... } ;
        ///                     Array<Instant> instants = { ... } ;
        ///                     Array<State> state = profile.getStatesAt(instants) ;
        /// @endcode
        ///
        /// @param              [in] anInstantArray An array of instants
        /// @return             Array of states

        Array<State>            getStatesAt                                 (   const   Array<Instant>&             anInstantArray                              ) const ;

        /// @brief              Get axes at a given instant
        ///
        /// @code
        ///                     Profile profile = { ... } ;
        ///                     Instant instant = { ... } ;
        ///                     Axes axes = profile.getAxesAt(instant) ;
        /// @endcode
        ///
        /// @param              [in] anInstant An instant
        /// @return             Axes

        Axes                    getAxesAt                                   (   const   Instant&                    anInstant                                   ) const ;

        /// @brief              Get body frame
        ///
        /// @param              [in] aBodyFrameName A body frame name
        /// @return             Shared pointer to body frame

        Shared<const Frame>     getBodyFrame                                (   const   String&                     aBodyFrameName                              ) const ;

        /// @brief              Print flight profile to output stream
        ///
        /// @code
        ///                     Profile profile = { ... } ;
        ///                     profile.print(std::cout, true) ;
        /// @endcode
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] displayDecorator If true, display decorator

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        /// @brief              Constructs an undefined flight profile
        ///
        /// @code
        ///                     Profile profile = Profile::Undefined() ; // Undefined
        /// @endcode
        ///
        /// @return             Undefined profile

        static Profile          Undefined                                   ( ) ;

        /// @brief              Constructs a flight profile with inertial pointing
        ///
        /// @param              [in] aTrajectory A trajectory
        /// @param              [in] aQuaternion A pointing in GCRF
        /// @return             Flight profile

        static Profile          InertialPointing                            (   const   Trajectory&                 aTrajectory,
                                                                                const   Quaternion&                 aQuaternion                                 ) ;

        /// @brief              Constructs a flight profile with nadir pointing
        ///
        /// @param              [in] anOrbit An orbit
        /// @param              [in] anOrbitalFrameType An orbital frame type
        /// @return             Flight profile

        static Profile          NadirPointing                               (   const   trajectory::Orbit&          anOrbit,
                                                                                const   trajectory::Orbit::FrameType& anOrbitalFrameType                        ) ;

        // static Profile          TargetTracking                              (   const   trajectory::Orbit&          anOrbit,
        //                                                                         const   Orbit::FrameType&           anOrbitalFrameType                          ) ;

    private:

        DynamicProvider         transformProvider_ ;
        Shared<const Frame>     frameSPtr_ ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
