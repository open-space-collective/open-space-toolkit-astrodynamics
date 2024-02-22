/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Time.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Pair;
using ostk::core::type::Index;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::physics::Environment;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::StateBuilder;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

/// @brief Define a propagator to be used for numerical propagation
class Propagator
{
   public:
    /// @brief Default integrator frame
    static const Shared<const Frame> IntegrationFrameSPtr;
    /// @brief Map of valid dynamics set for propagation
    ///
    /// @code{.cpp}
    ///              Map<String, Pair<Size, Size>> validDynamicsSet ={"TypeOfDynamics", {Min number allowed, Max number
    ///              allowed}};
    /// @endcode
    static const Map<String, Pair<Size, Size>> ValidDynamicsSet;  // Map<String, Pair<Size, Size>>

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              Propagator propagator = { aNumericalSolver, aDynamicsArray } ;
    /// @endcode
    ///
    /// @param aNumericalSolver A numerical solver
    /// @param aDynamicsArray A dynamics array
    Propagator(
        const NumericalSolver& aNumericalSolver,
        const Array<Shared<Dynamics>>& aDynamicsArray = Array<Shared<Dynamics>>::Empty()
    );

    /// @brief Copy constructor
    ///
    /// @param aPropagator A propagator (deep copy)
    Propagator(const Propagator& aPropagator);

    /// @brief Copy assignment operator
    ///
    /// @param aPropagator A propagator (deep copy)
    /// @return A reference to this propagator
    Propagator& operator=(const Propagator& aPropagator);

    /// @brief Equal to operator
    ///
    /// @param aPropagator A propagator
    /// @return True if propagators are equal
    bool operator==(const Propagator& aPropagator) const;

    /// @brief Not equal to operator
    ///
    /// @param aPropagator A propagator
    /// @return True if propagators are not equal
    bool operator!=(const Propagator& aPropagator) const;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aPropagator A propagator
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Propagator& aPropagator);

    /// @brief Check if propagator is defined
    ///
    /// @return True if propagator is defined
    bool isDefined() const;

    /// @brief Access the coordinate broker
    ///
    /// @return The coordinate broker
    const Shared<CoordinateBroker>& accessCoordinateBroker() const;

    /// @brief Access the numerical solver
    ///
    /// @return The numerical solver
    const NumericalSolver& accessNumericalSolver() const;

    /// @brief Get the number of propagated coordinates
    ///
    /// @return The number of propagated coordinates
    Size getNumberOfCoordinates() const;

    /// @brief Get the dynamics array
    /// @code{.cpp}
    ///              Array<Shared<Dynamics>> dynamics = propagator.getDynamics();
    /// @endcode
    /// @return An array of dynamics shared pointers
    Array<Shared<Dynamics>> getDynamics() const;

    /// @brief Set the dynamics array
    /// @code{.cpp}
    ///              propagator.setDynamics(aDynamicsArray);
    /// @endcode
    /// @param aDynamicsArray A dynamics array
    void setDynamics(const Array<Shared<Dynamics>>& aDynamicsArray);

    /// @brief Add a dynamics to the array of shared pointers to dynamics
    /// @code{.cpp}
    ///              propagator.addDynamics(aDynamicsSPtr);
    /// @endcode
    /// @param aDynamicsSPtr A Dynamics shared pointer
    void addDynamics(const Shared<Dynamics>& aDynamicsSPtr);

    /// @brief Clear the dynamics array
    /// @code{.cpp}
    ///              propagator.clearDynamics();
    /// @endcode
    void clearDynamics();

    /// @brief Calculate the state at an instant, given initial state
    /// @code{.cpp}
    ///              State state = propagator.calculateStateAt(aState, anInstant);
    /// @endcode
    /// @param aState An initial state
    /// @param anInstant An instant
    /// @return State
    State calculateStateAt(const State& aState, const Instant& anInstant) const;

    /// @brief Calculate the state subject to an Event Condition, given initial state and maximum end time
    /// @code{.cpp}
    ///              NumericalSolver::ConditionSolution state = propagator.calculateStateToCondition(aState, anInstant,
    ///              anEventCondition);
    /// @endcode
    /// @param aState An initial state
    /// @param anInstant An instant
    /// @param anEventCondition An event condition
    /// @return NumericalSolver::ConditionSolution
    NumericalSolver::ConditionSolution calculateStateToCondition(
        const State& aState, const Instant& anInstant, const EventCondition& anEventCondition
    ) const;

    /// @brief Calculate the states at an array of instants, given an initial state
    /// @brief Can only be used with sorted instants array
    ///
    /// @code{.cpp}
    ///              Array<State> states = propagator.calculateStatesAt(aState, anInstantArray);
    /// @endcode
    /// @param anInstantArray An instant array
    /// @return Array<State>
    Array<State> calculateStatesAt(const State& aState, const Array<Instant>& anInstantArray) const;

    /// @brief Print propagator
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Undefined
    ///
    /// @return An undefined propagator
    static Propagator Undefined();

    /// @brief Default
    ///
    /// @return A default propagator (with position derivative dynamics included)
    static Propagator Default();

    /// @brief Default from environment
    ///
    /// @param anEnvironment An environment
    ///
    /// @return A default propagator from environment
    static Propagator Default(const Environment& anEnvironment);

    /// @brief From environment
    ///
    /// @param aNumericalSolver A numerical solver
    /// @param anEnvironment An environment
    ///
    /// @return A propagator from environment
    static Propagator FromEnvironment(const NumericalSolver& aNumericalSolver, const Environment& anEnvironment);

   private:
    Shared<CoordinateBroker> coordinatesBrokerSPtr_ = std::make_shared<CoordinateBroker>();
    Array<Dynamics::Context> dynamicsContexts_ = Array<Dynamics::Context>::Empty();
    mutable NumericalSolver numericalSolver_;

    void validateDynamicsSet() const;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
