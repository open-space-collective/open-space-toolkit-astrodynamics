/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Sequence__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Real;
using ostk::core::types::Size;
using ostk::core::ctnr::Array;

using ostk::physics::Environment;
using ostk::physics::units::Mass;

using ostk::astro::trajectory::Segment;
using ostk::astro::trajectory::State;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::dynamics::Thruster;

/// @brief     Represents a sequence of trajectory segments executed in order.
class Sequence
{
   public:
    struct Solution
    {
        Array<Segment::Solution> segmentSolutions;
        bool executionIsComplete;

        const Instant& accessStartInstant() const;
        const Instant& accessEndInstant() const;

        Array<State> getStates() const;
        Mass getInitialMass() const;
        Mass getFinalMass() const;
        Duration getPropagationDuration() const;

        Mass computeDeltaMass() const;
        Real computeDeltaV(const Real& aSpecificImpulse) const;

        void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        friend std::ostream& operator<<(std::ostream& anOutputStream, const Solution& aSolution);
    };

    /// @brief Constructor
    ///
    /// @code
    ///                         const NumericalSolver numericalSolver = NumericalSolver::Default();
    ///                         const Array<Shared<Dynamics>> dynamicsArray =
    ///                         {std::make_shared<CentralBodyGravity>(Earth::GravitationalParameter())};
    ///                         const Duration maximumPropagationDuration = Duration::Days(7.0);
    ///                         const Size verbosity = 0;
    ///
    ///                         Sequence sequence = {numericalSolver, dynamicsArray,
    ///                         maximumPropagationDuration, verbosity};
    ///
    /// @endcode
    ///
    /// @param                  [in] aSegmentArray An array of segments. Defaults to empty.
    /// @param                  [in] aNumericalSolver A Numerical Solver. Defaults to Undefined.
    /// @param                  [in] aDynamicsArray An array of shared dynamics. Defaults to empty.
    /// @param                  [in] segmentPropagationDurationLimit Maximum duration for propagation. Defaults to 7.0
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
}  // namespace astro
}  // namespace ostk

#endif
