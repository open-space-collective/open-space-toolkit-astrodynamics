/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Size;

using ostk::physics::Environment;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::trajectory::orbit::model::Propagated;
using ostk::astrodynamics::trajectory::Segment;
using ostk::astrodynamics::trajectory::State;

/// @brief Represent a sequence of trajectory segments executed in order.
class Sequence
{
   public:
    /// @brief Once a sequence is set up with one or more segments, it can be solved, resulting in this sequences's
    /// Solution.
    struct Solution
    {
       public:
        /// @brief Constructor
        ///
        /// @param aSegmentSolutionArray An array of segment solutions.
        /// @param executionIsComplete True if the sequence was executed completely, false otherwise.
        /// @return An instance of Solution
        Solution(const Array<Segment::Solution>& aSegmentSolutionArray, const bool& executionIsComplete);

        /// @brief Access Start Instant
        /// @return Start Instant
        const Instant& accessStartInstant() const;

        /// @brief Access end instant
        /// @return End Instant
        const Instant& accessEndInstant() const;

        /// @brief Get all states (at variable intervals) that were computed when solving the sequence.
        /// @return Array of states.
        Array<State> getStates() const;

        /// @brief Get initial mass
        /// @return Initial mass
        Mass getInitialMass() const;

        /// @brief Get final mass
        /// @return Final mass
        Mass getFinalMass() const;

        /// @brief Get propagation duration
        /// @return Propagation duration
        Duration getPropagationDuration() const;

        /// @brief Compute delta V
        ///
        /// @param aSpecificImpulse Specific impulse
        /// @return Delta V
        Real computeDeltaV(const Real& aSpecificImpulse) const;

        /// @brief Compute delta mass
        /// @return Delta mass
        Mass computeDeltaMass() const;

        /// @brief Calculate states in this sequence's solution at the provided instants.
        ///
        /// @param anInstantArray An array of instants.
        /// @param aNumericalSolver A numerical solver to be used for the propagation.
        /// @return Array of states at provided instants.
        Array<State> calculateStatesAt(const Array<Instant>& anInstantArray, const NumericalSolver& aNumericalSolver)
            const;

        /// @brief Print the sequence solution
        ///
        /// @param anOutputStream An output stream
        /// @param (optional) displayDecorators If true, display decorators
        void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        /// @brief Output stream operator
        ///
        /// @param anOutputStream An output stream
        /// @param aSolution A Solution
        /// @return An output stream
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Solution& aSolution);

        Array<Segment::Solution> segmentSolutions;  // Array of segment soln contained within this sequence soln
        bool executionIsComplete;                   // True if the sequence was executed completely, false otherwise
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  const Array<Segment> segmentArray = {};
    ///                  const NumericalSolver numericalSolver = NumericalSolver::Default();
    ///                  const Array<Shared<Dynamics>> dynamicsArray =
    ///                  {std::make_shared<CentralBodyGravity>(Earth::GravitationalParameter())};
    ///                  const Duration maximumPropagationDuration = Duration::Days(7.0);
    ///                  const Size verbosity = 0;
    ///
    ///                  Sequence sequence = {segmentArray, numericalSolver, dynamicsArray,
    ///                  maximumPropagationDuration, verbosity};
    ///
    /// @endcode
    ///
    /// @param aSegmentArray An array of segments. Defaults to empty.
    /// @param aNumericalSolver A Numerical Solver. Defaults to Undefined.
    /// @param aDynamicsArray An array of shared dynamics. Defaults to empty.
    /// @param segmentPropagationDurationLimit Maximum duration for propagation. Defaults to 7.0
    /// days.
    /// @param verbosity Verbosity level for the solver [0 (low) - 5 (high)]. Defaults to 0.
    Sequence(
        const Array<Segment>& aSegmentArray = Array<Segment>::Empty(),
        const NumericalSolver& aNumericalSolver = NumericalSolver::Undefined(),
        const Array<Shared<Dynamics>>& aDynamicsArray = Array<Shared<Dynamics>>::Empty(),
        const Duration& segmentPropagationDurationLimit = Duration::Days(7.0),
        const Size& verbosity = 0
    );

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream.
    /// @param aSequence A sequence.
    /// @return An output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Sequence& aSequence);

    /// @brief Get segments.
    ///
    /// @return Array of segments.
    Array<Segment> getSegments() const;

    /// @brief Get numerical solver.
    ///
    /// @return Numerical solver.
    NumericalSolver getNumericalSolver() const;

    /// @brief Get dynamics.
    ///
    /// @return Dynamics.
    Array<Shared<Dynamics>> getDynamics() const;

    /// @brief Get maximum propagation duration.
    ///
    /// @return Maximum propagation duration.
    Duration getMaximumPropagationDuration() const;

    /// @brief Add a trajectory segment.
    ///
    /// @param aTrajectorySegment A trajectory segment.
    void addSegment(const Segment& aTrajectorySegment);

    /// @brief Add multiple segments.
    ///
    /// @param aTrajectorySegmentArray An array of trajectory segments.
    void addSegments(const Array<Segment>& aTrajectorySegmentArray);

    /// @brief Add a coast segment.
    ///
    /// @param anEventConditionSPtr An event condition.
    void addCoastSegment(const Shared<EventCondition>& anEventConditionSPtr);

    /// @brief Add a maneuver segment.
    ///
    /// @param anEventConditionSPtr An event condition.
    /// @param aThruster A thruster dynamics.
    void addManeuverSegment(const Shared<EventCondition>& anEventConditionSPtr, const Shared<Thruster>& aThruster);

    /// @brief Solve the sequence given an initial state, for a number of reptitions.
    ///
    /// @param aState Initial state for the sequence.
    /// @param aRepetitionCount Number of repetitions. Defaults to 1, i.e. execute sequence once.
    /// @return A Solution that contains solutions for each segment.
    Solution solve(const State& aState, const Size& aRepetitionCount = 1) const;

    /// @brief Solve the sequence given an initial state.
    ///
    /// @param aState Initial state for the sequence.
    /// @param anEventCondition An event condition.
    /// @param aMaximumPropagationDuration Maximum duration for sequence propagation.
    /// @return A Solution that contains solutions for each segment.
    Solution solveToCondition(
        const State& aState,
        const EventCondition& anEventCondition,
        const Duration& aMaximumPropagationDuration = Duration::Days(30.0)
    ) const;

    /// @brief Print the sequence.
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

   private:
    Array<Segment> segments_;
    NumericalSolver numericalSolver_;
    Array<Shared<Dynamics>> dynamics_;
    Duration segmentPropagationDurationLimit_;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
