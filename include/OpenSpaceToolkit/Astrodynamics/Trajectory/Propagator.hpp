/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Propagator__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>

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
using ostk::core::container::Map;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;

using ostk::physics::Environment;
using ostk::physics::time::Instant;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

#define DEFAULT_MANEUVER_PROPAGATION_INTERPOLATION_TYPE Interpolator::Type::BarycentricRational

/// @brief Define a propagator to be used for numerical propagation
class Propagator
{
   public:
    /// @brief Per-dynamics contributions recorded at each solver step accepted during a propagation.
    struct StepContributions
    {
        Array<Instant> instants = Array<Instant>::Empty();   ///< One instant per recorded step, in integration order
        Map<Shared<Dynamics>, MatrixXd> contributions = {};  ///< Per dynamics: rows align with instants, columns
                                                             ///< follow the dynamics' write coordinate subsets,
                                                             ///< expressed in GCRF
    };

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

    /// @brief Constructor with Maneuvers
    ///
    /// @code{.cpp}
    ///              Propagator propagator = { aNumericalSolver, aDynamicsArray, aManeuverArray };
    /// @endcode
    ///
    /// @param aNumericalSolver A numerical solver
    /// @param aDynamicsArray A dynamics array (only non-maneuvering dynamics should be added here)
    /// @param aManeuverArray A maneuver array (don't add tabulated dynamics to the dynamics array, add them here in the
    /// form of maneuvers instead)
    Propagator(
        const NumericalSolver& aNumericalSolver,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const Array<Maneuver>& aManeuverArray,
        const Interpolator::Type& anInterpolationType = DEFAULT_MANEUVER_PROPAGATION_INTERPOLATION_TYPE
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

    /// @brief Check if per-dynamics step contribution recording is enabled
    ///
    /// @return True if per-dynamics step contribution recording is enabled
    bool isContributionObservationEnabled() const;

    /// @brief Access the coordinate broker
    ///
    /// @return The coordinate broker
    const Shared<CoordinateBroker>& accessCoordinateBroker() const;

    /// @brief Access the numerical solver
    ///
    /// @return The numerical solver
    const NumericalSolver& accessNumericalSolver() const;

    /// @brief Access the step contributions recorded during the most recent calculate* call.
    ///        Empty unless recording is enabled. For calculateStateAt / calculateStateToCondition the
    ///        recorded steps are the solver's accepted steps (initial and final states included); for
    ///        calculateStatesAt they are the requested instants (each an accepted-step endpoint —
    ///        intermediate accepted steps are not recorded).
    ///
    /// @return The recorded step contributions
    const StepContributions& accessRecordedStepContributions() const;

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

    /// @brief Get the step contributions recorded during the most recent calculate* call
    /// @code{.cpp}
    ///              Propagator::StepContributions recorded = propagator.getRecordedStepContributions();
    /// @endcode
    /// @return The recorded step contributions
    StepContributions getRecordedStepContributions() const;

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

    /// @brief Add a maneuver to be taken into account during propagation in the form of tabulated dynamics
    /// @code{.cpp}
    ///              propagator.addManeuver(aManeuver);
    /// @endcode
    /// @param aManeuver A maneuver array
    /// @param (optional) anInterpolationType An interpolation type
    void addManeuver(
        const Maneuver& aManeuver,
        const Interpolator::Type& anInterpolationType = DEFAULT_MANEUVER_PROPAGATION_INTERPOLATION_TYPE
    );

    /// @brief Enable or disable per-dynamics step contribution recording (disabled by default).
    ///        When enabled, each calculate* call additionally evaluates every dynamics once per recorded
    ///        step (one extra RHS-equivalent evaluation per step) and stores the results. Memory grows
    ///        linearly with the number of recorded steps.
    /// @code{.cpp}
    ///              propagator.setContributionObservationEnabled(true);
    /// @endcode
    /// @param aContributionObservationEnabled True to enable recording, false to disable it
    void setContributionObservationEnabled(const bool& aContributionObservationEnabled);

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
    bool contributionObservationEnabled_ = false;
    mutable StepContributions recordedStepContributions_ = {};

    void validateDynamicsSet() const;

    /// @brief Evaluate and store per-dynamics contributions for the provided solver-layout states
    ///
    /// @param aStateArray States in the coordinate broker layout, expressed in the integration frame
    void recordStepContributions_(const Array<State>& aStateArray) const;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
