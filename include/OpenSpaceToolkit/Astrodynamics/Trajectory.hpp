/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>
#include <OpenSpaceToolkit/Core/Type/Unique.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::core::container::Array;
using ostk::core::type::Index;
using ostk::core::type::Real;
using ostk::core::type::String;
using ostk::core::type::Unique;

using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::trajectory::Model;
using ostk::astrodynamics::trajectory::State;

namespace trajectory
{
class Orbit;
}

/// @brief Path followed by an object through space as a function of time
///
/// @ref                        https://en.wikipedia.org/wiki/Trajectory
class Trajectory
{
   public:
    /// @brief Constructor (model)
    ///
    /// @code{.cpp}
    ///              Tabulated model = Tabulated::Load(File::Path(Path::Parse("/path/to/trajectory.csv")));
    ///              Trajectory trajectory = { model };
    /// @endcode
    ///
    /// @param aModel A trajectory model
    Trajectory(const Model& aModel);

    /// @brief Constructor (state array)
    ///
    /// @code{.cpp}
    ///              Array<State> stateArray = { ... };
    ///              Trajectory trajectory = { stateArray };
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
    ///              Trajectory(...) == Trajectory(...);
    /// @endcode
    ///
    /// @param aTrajectory A trajectory
    /// @return True if trajectories are equal
    bool operator==(const Trajectory& aTrajectory) const;

    /// @brief Not equal to operator
    ///
    /// @code{.cpp}
    ///              Trajectory(...) != Trajectory(...);
    /// @endcode
    ///
    /// @param aTrajectory A trajectory
    /// @return True if trajectories are not equal
    bool operator!=(const Trajectory& aTrajectory) const;

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///              std::cout << Trajectory(...);
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aTrajectory A trajectory
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Trajectory& aTrajectory);

    /// @brief Check if trajectory is defined
    ///
    /// @code{.cpp}
    ///              Trajectory(...).isDefined();
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
    ///              Trajectory trajectory = { ... };
    ///              Instant instant = { ... };
    ///              State state = trajectory.getStateAt(instant);
    /// @endcode
    ///
    /// @param anInstant An instant
    /// @return State
    State getStateAt(const Instant& anInstant) const;

    /// @brief Get states at a given instants
    ///
    /// @code{.cpp}
    ///              Trajectory trajectory = { ... };
    ///              Array<Instant> instants = { ... };
    ///              Array<State> state = trajectory.getStatesAt(instants);
    /// @endcode
    ///
    /// @param anInstantArray An array of instants
    /// @return Array of states
    Array<State> getStatesAt(const Array<Instant>& anInstantArray) const;

    /// @brief Print trajectory to output stream
    ///
    /// @code{.cpp}
    ///              Trajectory trajectory = { ... };
    ///              trajectory.print(std::cout, true);
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param displayDecorator If true, display decorator
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Constructs an undefined trajectory
    ///
    /// @code{.cpp}
    ///              Trajectory trajectory = Trajectory::Undefined(); // Undefined
    /// @endcode
    ///
    /// @return Undefined trajectory
    static Trajectory Undefined();

    /// @brief Constructs a trajectory from a given position
    ///
    /// @code{.cpp}
    ///              Position position = Position::Meters({ 0.0, 0.0, 0.0 }, Frame::GCRF());
    ///              Trajectory trajectory = Trajectory::Position(position);
    /// @endcode
    ///
    /// @param aPosition A position
    /// @return Static trajectory
    static Trajectory Position(const physics::coordinate::Position& aPosition);

    /// @brief Constructs a trajectory for a given strip, specified ground speed and start instant
    ///
    /// @code{.cpp}
    ///             LLA startLLA = { 0.0, 0.0, 0.0 };
    ///             LLA endLLA = { 1.0, 0.0, 0.0 };
    ///             Derived groundSpeed = Derived(1000.0, Derived::Unit::MeterPerSecond());
    ///             Instant startInstant = Instant::DateTime(DateTime::Parse("2020-01-01 00:00:00"), Scale::UTC);
    ///             Earth earth = Earth::WGS84();
    ///             Duration stepSize = Duration::Seconds(1.0);
    ///             Trajectory trajectory = Trajectory::GroundStrip(startLLA, endLLA, groundSpeed, startInstant, earth,
    ///             duration);
    /// @endcode
    ///
    /// @param aStartLLA A start LLA
    /// @param anEndLLA An end LLA
    /// @param aGroundSpeed A ground speed
    /// @param aStartInstant A start instant
    /// @param aCelestial Celestial body
    /// @return GroundStrip trajectory
    static Trajectory GroundStrip(
        const LLA& aStartLLA,
        const LLA& anEndLLA,
        const Derived& aGroundSpeed,
        const Instant& aStartInstant,
        const Celestial& aCelestial = Earth::WGS84(),
        const Duration& aStepSize = Duration::Seconds(1.0)
    );

    /// @brief Constructs a trajectory for a given strip, assuming a constant ground speed and start instant
    ///
    /// @code{.cpp}
    ///             LLA startLLA = LLA::FromVector({ 0.0, 0.0, 0.0 });
    ///             LLA endLLA = LLA::FromVector({ 1.0, 0.0, 0.0 });
    ///             Instant startInstant = Instant::DateTime(DateTime::Parse("2020-01-01 00:00:00"), Scale::UTC);
    ///             Instant endInstant = Instant::DateTime(DateTime::Parse("2020-01-01 00:10:00"), Scale::UTC);
    ///             Interval interval = Interval::Closed(startInstant, endInstant);
    ///             Array<Instant> instants = interval.generateGrid(Duration::Seconds(1.0));
    ///             Earth earth = Earth::WGS84();
    ///             Trajectory trajectory = Trajectory::GroundStrip(startLLA, endLLA, instants, earth);
    /// @endcode
    ///
    /// @param aStartLLA A start LLA
    /// @param anEndLLA An end LLA
    /// @param anInstantArray An array of instants
    /// @param aCelestial Celestial body
    /// @return GroundStrip trajectory
    static Trajectory GroundStrip(
        const LLA& aStartLLA,
        const LLA& anEndLLA,
        const Array<Instant>& anInstantArray,
        const Celestial& aCelestial = Earth::WGS84()
    );

    /// @brief Constructs a trajectory representing a ground strip that follows the geodetic nadir of the provided orbit
    ///
    /// @code{.cpp}
    ///             Instant startInstant = Instant::DateTime(DateTime::Parse("2020-01-01 00:00:00"), Scale::UTC);
    ///             Earth earth = Earth::WGS84();
    ///             Trajectory trajectory = Trajectory::GroundStripGeodeticNadir(anOrbit, anInstantArray, earth);
    /// @endcode
    ///
    /// @param anOrbit An orbit
    /// @param anInstantArray An array of instants
    /// @param aCelestial Celestial body
    /// @return Trajectory
    static Trajectory GroundStripGeodeticNadir(
        const trajectory::Orbit& anOrbit,
        const Array<Instant>& anInstantArray,
        const Celestial& aCelestial = Earth::WGS84()
    );

   private:
    Unique<Model> modelUPtr_;

    Trajectory();

    /// @brief Compute states using the provided position generator, and finite differenced velocities, at the provided
    /// instants
    ///
    /// @param aPositionGenerator A position generator
    /// @param anInstantArray An array of instants
    /// @param aStepSize A step size
    /// @return Array of states
    static Array<State> computeStates(
        const std::function<physics::coordinate::Position(const Instant&)>& aPositionGenerator,
        const Array<Instant>& anInstantArray,
        const Duration& aStepSize = Duration::Seconds(1e-6)
    );
};

}  // namespace astrodynamics
}  // namespace ostk

#endif
