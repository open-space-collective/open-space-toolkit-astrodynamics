/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Segment__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Segment__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::physics::time::Instant;
using ostk::physics::time::Duration;

using ostk::astro::trajectory::State;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::EventCondition;

/// @brief                                  Represents a segment for astrodynamic calculations.
class TrajectorySegment
{
   public:
    struct Solution
    {
       public:
        Solution(const String& aName, const Array<Shared<Dynamics>>& aDynamicsArray, const Array<State>& aStates);

        String name;                       /// Name of the segment.
        Array<Shared<Dynamics>> dynamics;  /// List of dynamics used.
        Array<State> states;               /// Array of states for the segment.
    };

    /// @brief                  Output stream operator
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] aSegment A TrajectorySegment
    /// @return                 An output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const TrajectorySegment& aSegment);

    /// @brief                  Get name
    /// @return                 Name of the segment.

    String getName() const;

    /// @brief                  Get event condition.
    /// @return                 Event condition.

    Shared<EventCondition> getEventCondition() const;

    /// @brief                  Get dynamics.
    /// @return                 Dynamics.

    Array<Shared<Dynamics>> getDynamics() const;

    /// @brief                  Get numerical solver.
    /// @return                 Numerical solver.

    NumericalSolver getNumericalSolver() const;

    /// @brief                  Solve the segment
    ///
    /// @param                  [in] aState Initial state for the segment.
    /// @param                  [in] aDynamicsArray Array of dynamics. Defaults to an empty array.
    /// @param                  [in] aNumericalSolver Numerical solver to be used. Defaults to undefined.
    /// @param                  [in] maximumPropagationDuration Maximum duration for propagation. Defaults to 30 days.
    /// @return                 A Solution representing the result of the solve.

    Solution solve(
        const State& aState,
        const Array<Shared<Dynamics>>& aDynamicsArray = Array<Shared<Dynamics>>::Empty(),
        const NumericalSolver& aNumericalSolver = NumericalSolver::Undefined(),
        const Duration& maximumPropagationDuration = Duration::Days(30.0)
    ) const;

    /// @brief                  Print the segment.
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Create a coasting segment.
    ///
    /// @param                  [in] anEventConditionSPtr An event condition.
    /// @param                  [in] aDynamicsArray Array of dynamics. Defaults to an empty array.
    /// @param                  [in] aNumericalSolver Numerical solver to be used. Defaults to undefined.
    /// @return                 A TrajectorySegment for coasting.

    static TrajectorySegment Coast(
        const Shared<EventCondition>& anEventConditionSPtr,
        const Array<Shared<Dynamics>>& aDynamicsArray = Array<Shared<Dynamics>>::Empty(),
        const NumericalSolver& aNumericalSolver = NumericalSolver::Undefined()
    );

    /// @brief                  Create a maneuvering segment.
    ///
    /// @param                  [in] anEventConditionSPtr An event condition.
    /// @param                  [in] aThrusterDynamics Dynamics for the thruster.
    /// @param                  [in] aNumericalSolver Numerical solver to be used. Defaults to undefined.
    /// @return                 A TrajectorySegment for maneuvering.

    static TrajectorySegment Maneuver(
        const Shared<EventCondition>& anEventConditionSPtr,
        const Shared<Dynamics>& aThrusterDynamics,  // TBM: This should be specifically ThrusterDynamics parent
        const NumericalSolver& aNumericalSolver = NumericalSolver::Undefined()
    );

   private:
    String name_;
    Shared<EventCondition> eventCondition_;
    Array<Shared<Dynamics>> dynamics_;
    NumericalSolver numericalSolver_;

    TrajectorySegment(
        const String& aName,
        const Shared<EventCondition>& anEventConditionSPtr,
        const Array<Shared<Dynamics>>& aDynamicsArray = Array<Shared<Dynamics>>::Empty(),
        const NumericalSolver& aNumericalSolver = NumericalSolver::Undefined()
    );
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif  // __OpenSpaceToolkit_Astrodynamics_Trajectory_Segment__
