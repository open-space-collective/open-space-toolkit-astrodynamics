////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Profile.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Flight_Profile__
#define __Library_Astrodynamics_Flight_Profile__

#include <Library/Astrodynamics/Flight/Profile/State.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>
#include <Library/Astrodynamics/Trajectory.hpp>

#include <Library/Physics/Coordinate/Frame/Providers/Dynamic.hpp>
#include <Library/Physics/Coordinate/Axes.hpp>
#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Duration.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <Library/Mathematics/Objects/Vector.hpp>

#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Shared.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace flight
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Shared ;
using library::core::types::String ;
using library::core::ctnr::Array ;

using library::math::obj::Vector3d ;
using library::math::geom::d3::trf::rot::Quaternion ;

using library::physics::time::Instant ;
using library::physics::time::Duration ;
using library::physics::time::Interval ;
using library::physics::coord::Frame ;
using library::physics::coord::Axes ;
using DynamicProvider = library::physics::coord::frame::provider::Dynamic ;

using library::astro::Trajectory ;
using library::astro::flight::profile::State ;

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