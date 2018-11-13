////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_Astrodynamics_Trajectory__
#define __Library_Astrodynamics_Trajectory__

#include <Library/Astrodynamics/Trajectory/State.hpp>
#include <Library/Astrodynamics/Trajectory/Model.hpp>

#include <Library/Physics/Coordinate/Position.hpp>
#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Instant.hpp>

#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/String.hpp>
#include <Library/Core/Types/Index.hpp>
#include <Library/Core/Types/Unique.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Unique ;
using library::core::types::Index ;
using library::core::types::String ;
using library::core::ctnr::Array ;

using library::physics::time::Instant ;
using library::physics::time::Interval ;

using library::astro::trajectory::Model ;
using library::astro::trajectory::State ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief                      Path followed by an object through space as a function of time
///
/// @ref                        https://en.wikipedia.org/wiki/Trajectory

class Trajectory
{

    public:

        /// @brief              Constructor (model)
        ///
        /// @code
        ///                     Tabulated model = Tabulated::Load(File::Path(Path::Parse("/path/to/trajectory.csv"))) ;
        ///                     Trajectory trajectory = { model } ;
        /// @endcode
        ///
        /// @param              [in] aModel A trajectory model

                                Trajectory                                  (   const   Model&                      aModel                                      ) ;

        /// @brief              Constructor (state array)
        ///
        /// @code
        ///                     Array<State> stateArray = { ... } ;
        ///                     Trajectory trajectory = { stateArray } ;
        /// @endcode
        ///
        /// @param              [in] aStateArray An array of states

                                Trajectory                                  (   const   Array<State>&               aStateArray                                 ) ;

        /// @brief              Copy constructor
        ///
        /// @param              [in] aTrajectory A trajectory

                                Trajectory                                  (   const   Trajectory&                 aTrajectory                                 ) ;

        /// @brief              Copy assignment operator (deleted)

        Trajectory&             operator =                                  (   const   Trajectory&                 aTrajectory                                 ) = delete ;

        /// @brief              Equal to operator
        ///
        /// @code
        ///                     Trajectory(...) == Trajectory(...) ;
        /// @endcode
        ///
        /// @param              [in] aTrajectory A trajectory
        /// @return             True if trajectories are equal

        bool                    operator ==                                 (   const   Trajectory&                 aTrajectory                                 ) const ;

        /// @brief              Not equal to operator
        ///
        /// @code
        ///                     Trajectory(...) != Trajectory(...) ;
        /// @endcode
        ///
        /// @param              [in] aTrajectory A trajectory
        /// @return             True if trajectories are not equal

        bool                    operator !=                                 (   const   Trajectory&                 aTrajectory                                 ) const ;

        /// @brief              Output stream operator
        ///
        /// @code
        ///                     std::cout << Trajectory(...) ;
        /// @endcode
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] aTrajectory A trajectory
        /// @return             A reference to output stream

        friend std::ostream&    operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Trajectory&                 aTrajectory                                 ) ;

        /// @brief              Check if trajectory is defined
        ///
        /// @code
        ///                     Trajectory(...).isDefined() ;
        /// @endcode
        ///
        /// @return             True if trajectory is defined

        bool                    isDefined                                   ( ) const ;

        /// @brief              Access trajectory model
        ///
        /// @return             Reference to trajectory model

        const Model&            accessModel                                 ( ) const ;

        /// @brief              Get state at a given instant
        ///
        /// @code
        ///                     Trajectory trajectory = { ... } ;
        ///                     Instant instant = { ... } ;
        ///                     State state = trajectory.getStateAt(instant) ;
        /// @endcode
        ///
        /// @param              [in] anInstant An instant
        /// @return             State

        State                   getStateAt                                  (   const   Instant&                    anInstant                                   ) const ;

        /// @brief              Get states at a given instants
        ///
        /// @code
        ///                     Trajectory trajectory = { ... } ;
        ///                     Array<Instant> instants = { ... } ;
        ///                     Array<State> state = trajectory.getStatesAt(instants) ;
        /// @endcode
        ///
        /// @param              [in] anInstantArray An array of instants
        /// @return             Array of states

        Array<State>            getStatesAt                                 (   const   Array<Instant>&             anInstantArray                              ) const ;

        /// @brief              Print trajectory to output stream
        ///
        /// @code
        ///                     Trajectory trajectory = { ... } ;
        ///                     trajectory.print(std::cout, true) ;
        /// @endcode
        ///
        /// @param              [in] anOutputStream An output stream
        /// @param              [in] displayDecorator If true, display decorator

        virtual void            print                                       (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            =   true ) const ;

        /// @brief              Constructs an undefined trajectory
        ///
        /// @code
        ///                     Trajectory trajectory = Trajectory::Undefined() ; // Undefined
        /// @endcode
        ///
        /// @return             Undefined trajectory

        static Trajectory       Undefined                                   ( ) ;

        /// @brief              Constructs a trajectory from a given position
        ///
        /// @code
        ///                     Position position = Position::Meters({ 0.0, 0.0, 0.0 }, Frame::GCRF()) ;
        ///                     Trajectory trajectory = Trajectory::Position(position) ;
        /// @endcode
        ///
        /// @param              [in] aPosition A position
        /// @return             Static trajectory

        static Trajectory       Position                                    (   const   physics::coord::Position&   aPosition                                   ) ;

    private:

        Unique<Model>           modelUPtr_ ;

                                Trajectory                                  ( ) ;

} ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////