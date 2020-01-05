////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory_Orbit__
#define __Library_Astrodynamics_Trajectory_Orbit__

#include <Library/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Pass.hpp>
#include <Library/Astrodynamics/Trajectory/State.hpp>
#include <Library/Astrodynamics/Trajectory.hpp>

#include <Library/Physics/Environment/Objects/Celestial.hpp>
#include <Library/Physics/Coordinate/Frame.hpp>
#include <Library/Physics/Time/Time.hpp>
#include <Library/Physics/Time/Instant.hpp>
#include <Library/Physics/Units/Derived/Angle.hpp>
#include <Library/Physics/Units/Length.hpp>

#include <Library/Core/Containers/Map.hpp>
#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Integer.hpp>
#include <Library/Core/Types/Index.hpp>
#include <Library/Core/Types/Shared.hpp>
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
using library::core::types::Shared ;
using library::core::types::Index ;
using library::core::types::Integer ;
using library::core::types::Real ;
using library::core::ctnr::Array ;
using library::core::ctnr::Map ;

using library::physics::units::Length ;
using library::physics::units::Angle ;
using library::physics::time::Instant ;
using library::physics::time::Time ;
using library::physics::coord::Frame ;
using library::physics::env::obj::Celestial ;

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

        enum class FrameType
        {

            Undefined,          ///< Undefined frame
            NED,                ///< North-East-Down (NED) frame
            LVLH,               ///< Local Vertical, Local Horizontal (LVLH) frame (X axis aligned with position, Z axis aligned with orbital momentum)
            VVLH,               ///< Vehicle Velocity, Local Horizontal (VVLH) frame (Z axis aligned with opposite of position, Y axis aligned with opposite of orbital momentum)
            LVLHGD,             ///< Local Vertical, Local Horizontal GeoDetic (LVLHGD) frame
            QSW,                ///< QSW frame (X axis aligned with position, Z axis aligned with orbital momentum)
            TNW,                ///< TNW frame (X axis aligned with velocity, Z axis aligned with orbital momentum)
            VNC                 ///< Velocity - Normal - Co-normal (VNC) frame (X axis aligned with velocity, Y axis aligned with orbital momentum)

        } ;

        typedef                 Array<Pass>::ConstIterator                      ConstPassIterator ;

                                Orbit                                       (   const   orbit::Model&               aModel,
                                                                                const   Shared<const Celestial>&    aCelestialObjectSPtr                        ) ;

                                Orbit                                       (   const   Array<State>&               aStateArray,
                                                                                const   Integer&                    anInitialRevolutionNumber,
                                                                                const   Shared<const Celestial>&    aCelestialObjectSPtr                        ) ;

                                Orbit                                       (   const   Orbit&                      anOrbit                                     ) ;

                                ~Orbit                                      ( ) ;

        Orbit&                  operator =                                  (   const   Orbit&                      anOrbit                                     ) = delete ;

        bool                    operator ==                                 (   const   Orbit&                      anOrbit                                     ) const ;

        bool                    operator !=                                 (   const   Orbit&                      anOrbit                                     ) const ;

        bool                    isDefined                                   ( ) const ;

        Integer                 getRevolutionNumberAt                       (   const   Instant&                    anInstant                                   ) const ;

        Pass                    getPassAt                                   (   const   Instant&                    anInstant                                   ) const ;

        Pass                    getPassWithRevolutionNumber                 (   const   Integer&                    aRevolutionNumber                           ) const ;

        Shared<const Frame>     getOrbitalFrame                             (   const   Orbit::FrameType&           aFrameType                                  ) const ;

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const override ;

        /// @brief              Constructs an undefined orbit
        ///
        ///                     Undefined orbit

        static Orbit            Undefined                                   ( ) ;

        /// @brief              Constructs a circular orbit
        ///
        ///                     Model: Kepler (No Perturbation).
        ///
        /// @param              [in] anEpoch An orbit epoch
        /// @param              [in] anAltitude An orbit altitude (wrt. equatorial radius)
        /// @param              [in] anInclination An orbit inclination
        /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
        /// @return             Circular orbit

        static Orbit            Circular                                    (   const   Instant&                    anEpoch,
                                                                                const   Length&                     anAltitude,
                                                                                const   Angle&                      anInclination,
                                                                                const   Shared<const Celestial>&    aCelestialObjectSPtr                        ) ;

        /// @brief              Constructs an equatorial orbit
        ///
        ///                     Model: Kepler (No Perturbation).
        ///
        /// @param              [in] anEpoch An orbit epoch
        /// @param              [in] anApoapsisAltitude An orbit apoapsis altitude (wrt. equatorial radius)
        /// @param              [in] aPeriapsisAltitude An orbit periapsis altitude (wrt. equatorial radius)
        /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
        /// @return             Equatorial orbit

        static Orbit            Equatorial                                  (   const   Instant&                    anEpoch,
                                                                                const   Length&                     anApoapsisAltitude,
                                                                                const   Length&                     aPeriapsisAltitude,
                                                                                const   Shared<const Celestial>&    aCelestialObjectSPtr                        ) ;

        /// @brief              Constructs a circular-equatorial orbit
        ///
        ///                     Model: Kepler (No Perturbation).
        ///
        /// @param              [in] anEpoch An orbit epoch
        /// @param              [in] anAltitude An orbit altitude (wrt. equatorial radius)
        /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
        /// @return             Circular-equatorial orbit

        static Orbit            CircularEquatorial                          (   const   Instant&                    anEpoch,
                                                                                const   Length&                     anAltitude,
                                                                                const   Shared<const Celestial>&    aCelestialObjectSPtr                        ) ;

        /// @brief              Constructs a Sun-synchronous orbit
        ///
        ///                     Model: Kepler (J2 Perturbation).
        ///
        /// @param              [in] anEpoch An orbit epoch
        /// @param              [in] anAltitude An orbit altitude (wrt. equatorial radius)
        /// @param              [in] aLocalTimeAtDescendingNode A local time at descending node
        /// @param              [in] aCelestialObjectSPtr A shared pointer to a central celestial body
        /// @return             Sun-synchronous orbit

        static Orbit            SunSynchronous                              (   const   Instant&                    anEpoch,
                                                                                const   Length&                     anAltitude,
                                                                                const   Time&                       aLocalTimeAtDescendingNode,
                                                                                const   Shared<const Celestial>&    aCelestialObjectSPtr                        ) ;

        static String           StringFromFrameType                         (   const   Orbit::FrameType&           aFrameType                                  ) ;

    private:

        const orbit::Model&     model_ ;

        Shared<const Celestial> celestialObjectSPtr_ ;

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
