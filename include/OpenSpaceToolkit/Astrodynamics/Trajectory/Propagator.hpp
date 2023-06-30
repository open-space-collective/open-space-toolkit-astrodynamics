/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::ctnr::Array;
using ostk::core::types::Integer;
using ostk::core::types::Real;

using ostk::physics::Environment;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astro::NumericalSolver;
using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::CoordinatesSubset;
using ostk::astro::trajectory::State;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;

/// @brief                      Define a propagator to be used for numerical propagation

class Propagator
{
   public:
    /// @brief              Constructor
    ///
    /// @code
    ///                     Propagator propagator = { aNumericalSolver, aDynamicsArray } ;
    /// @endcode
    ///
    /// @param              [in] aNumericalSolver A numerical solver
    /// @param              [in] aDynamicsArray A dynamics array

    Propagator(
        const NumericalSolver& aNumericalSolver,
        const Array<Shared<Dynamics>>& aDynamicsArray = Array<Shared<Dynamics>>::Empty()
    );

    /// @brief              Equal to operator
    ///
    /// @param              [in] aPropagator A propagator
    /// @return             True if propagators are equal

    bool operator==(const Propagator& aPropagator) const;

    /// @brief              Not equal to operator
    ///
    /// @param              [in] aPropagator A propagator
    /// @return             True if propagators are not equal

    bool operator!=(const Propagator& aPropagator) const;

    /// @brief              Output stream operator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] aPropagator A propagator
    /// @return             A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Propagator& aPropagator);

    /// @brief              Check if propagator is defined
    ///
    /// @return             True if propagator is defined

    bool isDefined() const;

    /// @brief              Get the dynamics array
    /// @code
    ///                     Array<Shared<Dynamics>> dynamics = propagator.getDynamics();
    /// @endcode
    /// @return             An array of dynamics shared pointers

    Array<Shared<Dynamics>> getDynamics() const;

    /// @brief              Set the dynamics array
    /// @code
    ///                     propagator.setDynamics(aDynamicsArray);
    /// @endcode
    /// @param              [in] aDynamicsArray A dynamics array

    void setDynamics(const Array<Shared<Dynamics>>& aDynamicsArray);

    /// @brief              Add a dynamics to the array of shared pointers to dynamics
    /// @code
    ///                     propagator.addDynamics(aDynamics);
    /// @endcode
    /// @param              [in] aDynamics A Dynamics shared pointer

    void addDynamics(const Shared<Dynamics>& aDynamics);

    /// @brief              Clear the dynamics array
    /// @code
    ///                     propagator.clearDynamics();
    /// @endcode

    void clearDynamics();

    /// @brief              Calculate the state at an instant, given initial state
    /// @code
    ///                     State state = propagator.calculateStateAt(aState, anInstant) ;
    /// @endcode
    /// @param              [in] aState An initial state
    /// @param              [in] anInstant An instant
    /// @return             State

    State calculateStateAt(const State& aState, const Instant& anInstant) const;

    /// @brief              Calculate the states at an array of instants, given an initial state
    /// @brief              Can only be used with sorted instants array
    /// @code
    ///                     Array<State> states = propagator.calculateStatesAt(aState, anInstantArray) ;
    /// @endcode
    /// @param              [in] anInstantArray An instant array
    /// @return             Array<State>

    Array<State> calculateStatesAt(const State& aState, const Array<Instant>& anInstantArray) const;

    /// @brief              Print propagator
    ///
    /// @param              [in] anOutputStream An output stream
    /// @param              [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief              Undefined
    ///
    /// @return             An undefined propagator

    static Propagator Undefined();

    /// @brief              Default
    ///
    /// @return             A default propagator (with position derivative dynamics included)

    static Propagator Default();

    /// @brief              Default from environment
    ///
    /// @return             A default propagator from environment

    static Propagator Default(
        const Environment& anEnvironment, const SatelliteSystem& aSatelliteSystem = SatelliteSystem::Undefined()
    );

    /// @brief              From environment
    ///
    /// @return             A propagator from environment

    static Propagator FromEnvironment(
        const NumericalSolver& aNumericalSolver,
        const Environment& anEnvironment,
        const SatelliteSystem& aSatelliteSystem = SatelliteSystem::Undefined()
    );

   private:
    Shared<CoordinatesBroker> coordinatesBroker_ = std::make_shared<CoordinatesBroker>();
    Array<Shared<Dynamics>> dynamics_;
    mutable NumericalSolver numericalSolver_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
