/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Unique.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{

using ostk::core::ctnr::Array;
using ostk::core::types::Index;
using ostk::core::types::String;
using ostk::core::types::Unique;

using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astro::trajectory::Model;
using ostk::astro::trajectory::State;

/// @brief     Path followed by an object through space as a function of time
///
/// @ref                        https://en.wikipedia.org/wiki/Trajectory
class Trajectory
{
   public:
    /// @brief Constructor (model)
    ///
    /// @code{.cpp}
    ///              Tabulated model = Tabulated::Load(File::Path(Path::Parse("/path/to/trajectory.csv"))) ;
    ///              Trajectory trajectory = { model } ;
    /// @endcode
    ///
    /// @param aModel A trajectory model
    Trajectory(const Model& aModel);

    /// @brief Constructor (state array)
    ///
    /// @code{.cpp}
    ///              Array<State> stateArray = { ... } ;
    ///              Trajectory trajectory = { stateArray } ;
    /// @endcode
    ///
    /// @param aStateArray An array of states
    Trajectory(const Array<State>& aStateArray);

    /// @brief Copy constructor
    ///
    /// @param aTrajectory A trajectory
    Trajectory(const Trajectory& aTrajectory);

    /// @brief Copy assignment operator
    Trajectory& operator=(const Trajectory& aTrajectory);

    /// @brief Equal to operator
    ///
    /// @code{.cpp}
    ///              Trajectory(...) == Trajectory(...) ;
    /// @endcode
    ///
    /// @param aTrajectory A trajectory
    /// @return True if trajectories are equal
    bool operator==(const Trajectory& aTrajectory) const;

    /// @brief Not equal to operator
    ///
    /// @code{.cpp}
    ///              Trajectory(...) != Trajectory(...) ;
    /// @endcode
    ///
    /// @param aTrajectory A trajectory
    /// @return True if trajectories are not equal
    bool operator!=(const Trajectory& aTrajectory) const;

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///              std::cout << Trajectory(...) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aTrajectory A trajectory
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Trajectory& aTrajectory);

    /// @brief Check if trajectory is defined
    ///
    /// @code{.cpp}
    ///              Trajectory(...).isDefined() ;
    /// @endcode
    ///
    /// @return True if trajectory is defined
    bool isDefined() const;

    /// @brief Access trajectory model
    ///
    /// @return Reference to trajectory model
    const Model& accessModel() const;

    /// @brief Get state at a given instant
    ///
    /// @code{.cpp}
    ///              Trajectory trajectory = { ... } ;
    ///              Instant instant = { ... } ;
    ///              State state = trajectory.getStateAt(instant) ;
    /// @endcode
    ///
    /// @param anInstant An instant
    /// @return State
    State getStateAt(const Instant& anInstant) const;

    /// @brief Get states at a given instants
    ///
    /// @code{.cpp}
    ///              Trajectory trajectory = { ... } ;
    ///              Array<Instant> instants = { ... } ;
    ///              Array<State> state = trajectory.getStatesAt(instants) ;
    /// @endcode
    ///
    /// @param anInstantArray An array of instants
    /// @return Array of states
    Array<State> getStatesAt(const Array<Instant>& anInstantArray) const;

    /// @brief Print trajectory to output stream
    ///
    /// @code{.cpp}
    ///              Trajectory trajectory = { ... } ;
    ///              trajectory.print(std::cout, true) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param displayDecorator If true, display decorator
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Constructs an undefined trajectory
    ///
    /// @code{.cpp}
    ///              Trajectory trajectory = Trajectory::Undefined() ; // Undefined
    /// @endcode
    ///
    /// @return Undefined trajectory
    static Trajectory Undefined();

    /// @brief Constructs a trajectory from a given position
    ///
    /// @code{.cpp}
    ///              Position position = Position::Meters({ 0.0, 0.0, 0.0 }, Frame::GCRF()) ;
    ///              Trajectory trajectory = Trajectory::Position(position) ;
    /// @endcode
    ///
    /// @param aPosition A position
    /// @return Static trajectory
    static Trajectory Position(const physics::coord::Position& aPosition);

   private:
    Unique<Model> modelUPtr_;

    Trajectory();
};

}  // namespace astro
}  // namespace ostk

#endif
