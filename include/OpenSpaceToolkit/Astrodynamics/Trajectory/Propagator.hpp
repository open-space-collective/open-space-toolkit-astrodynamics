/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::physics::Environment;
using ostk::physics::time::Instant;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

/// @brief Define a propagator to be used for numerical propagation
///
/// @details Maneuvers are given to each propagation call, they are not stored in the propagator. When propagating
/// across a maneuver, the integration is split at the exact start and end of the maneuver: the propagator coasts up to
/// the start of the maneuver, applies the maneuver dynamics until its end, then continues. A maneuver is therefore
/// never stepped over by the integrator, however short it is.
///
/// @note Each stretch of integration (between maneuver starts / ends) is a separate call to the numerical solver, so
/// when a maneuver falls within the propagated interval, the numerical solver's observed states only cover the last
/// stretch. Without maneuvers there is a single stretch and nothing changes.
class Propagator
{
   public:
    /// @brief Default integrator frame
    static const Shared<const Frame> IntegrationFrameSPtr;

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              Propagator propagator = { aNumericalSolver, aDynamicsArray };
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
    ///              State stateWithManeuvers = propagator.calculateStateAt(aState, anInstant, aManeuverArray);
    /// @endcode
    /// @param aState An initial state. If there are maneuvers, it must contain the mass.
    /// @param anInstant An instant
    /// @param aManeuverArray (optional) Maneuvers to apply, exactly between their first and last instants. They must
    /// not overlap in time, nor be duplicated.
    /// @return State
    State calculateStateAt(
        const State& aState, const Instant& anInstant, const Array<Maneuver>& aManeuverArray = Array<Maneuver>::Empty()
    ) const;

    /// @brief Calculate the state subject to an Event Condition, given initial state and maximum end time
    /// @code{.cpp}
    ///              NumericalSolver::ConditionSolution state = propagator.calculateStateToCondition(aState, anInstant,
    ///              anEventCondition, aManeuverArray);
    /// @endcode
    /// @param aState An initial state. If there are maneuvers, it must contain the mass.
    /// @param anInstant An instant
    /// @param anEventCondition An event condition
    /// @param aManeuverArray (optional) Maneuvers to apply, exactly between their first and last instants. They must
    /// not overlap in time, nor be duplicated.
    /// @return NumericalSolver::ConditionSolution
    NumericalSolver::ConditionSolution calculateStateToCondition(
        const State& aState,
        const Instant& anInstant,
        const EventCondition& anEventCondition,
        const Array<Maneuver>& aManeuverArray = Array<Maneuver>::Empty()
    ) const;

    /// @brief Calculate the states at an array of instants, given an initial state
    /// @brief Can only be used with sorted instants array
    ///
    /// @code{.cpp}
    ///              Array<State> states = propagator.calculateStatesAt(aState, anInstantArray, aManeuverArray);
    /// @endcode
    /// @param aState An initial state. If there are maneuvers, it must contain the mass.
    /// @param anInstantArray An instant array
    /// @param aManeuverArray (optional) Maneuvers to apply, exactly between their first and last instants. They must
    /// not overlap in time, nor be duplicated.
    /// @return Array<State>
    Array<State> calculateStatesAt(
        const State& aState,
        const Array<Instant>& anInstantArray,
        const Array<Maneuver>& aManeuverArray = Array<Maneuver>::Empty()
    ) const;

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
    /// @brief A maneuver, together with the context needed to apply it during integration
    struct ScheduledManeuver
    {
        Instant startInstant;
        Instant endInstant;
        Dynamics::Context context;
    };

    /// @brief The maneuvers of a propagation call, and the coordinate broker that includes what they write to
    struct ManeuverSchedule
    {
        Shared<CoordinateBroker> coordinateBrokerSPtr;
        Array<ScheduledManeuver> maneuvers;  ///< Sorted by start instant
    };

    /// @brief A stretch of integration between two consecutive breakpoints (maneuver starts / ends), over which the set
    /// of applied dynamics is constant
    struct Arc
    {
        Instant startInstant;
        Instant endInstant;
        bool hasManeuver;
        Array<Dynamics::Context> contexts;
    };

    Shared<CoordinateBroker> coordinatesBrokerSPtr_ = std::make_shared<CoordinateBroker>();
    Array<Dynamics::Context> dynamicsContexts_ = Array<Dynamics::Context>::Empty();
    mutable NumericalSolver numericalSolver_;

    void validateDynamicsSet() const;

    /// @brief Validate the maneuvers of a propagation call, and prepare them to be applied
    ///
    /// @details The propagator's coordinate broker is left untouched: if there are maneuvers, the schedule carries a
    /// copy of it that also includes the coordinate subsets the maneuvers write to.
    ManeuverSchedule scheduleManeuvers(const Array<Maneuver>& aManeuverArray) const;

    /// @brief Split the propagation between two instants into arcs, in propagation order (also when going backwards)
    ///
    /// @details Coasting arcs only carry the regular dynamics. Maneuver arcs span exactly the (clipped) interval of a
    /// maneuver, and additionally carry the maneuver's dynamics. There is always at least one arc.
    Array<Arc> generateArcs(
        const Instant& aStartInstant, const Instant& anEndInstant, const Array<ScheduledManeuver>& aManeuverArray
    ) const;

    /// @brief Propagate through the arcs up to each of the instants
    ///
    /// @param aState A state in the integration frame, reduced to the coordinates of the schedule's broker
    /// @param anInstantArray Instants sorted in the direction of propagation, all on the same side of the state's
    /// instant (an instant equal to the state's instant is allowed)
    /// @param aManeuverArray The scheduled maneuvers
    /// @return The states at the instants
    Array<State> propagateThroughArcs(
        const State& aState, const Array<Instant>& anInstantArray, const Array<ScheduledManeuver>& aManeuverArray
    ) const;

    /// @brief Create the context of a dynamics, registering the coordinate subsets it reads and writes with a
    /// coordinate broker
    static Dynamics::Context CreateContext(
        const Shared<Dynamics>& aDynamicsSPtr, const Shared<CoordinateBroker>& aCoordinateBrokerSPtr
    );
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
