/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Solvers_TemporalConditionSolver__
#define __OpenSpaceToolkit_Astrodynamics_Solvers_TemporalConditionSolver__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

namespace ostk
{
namespace astro
{
namespace solvers
{

using ostk::core::types::Size;
using ostk::core::ctnr::Array;

using ostk::physics::time::Instant;
using ostk::physics::time::Duration;
using ostk::physics::time::Interval;

#define DEFAULT_MAXIMUM_ITERATION_COUNT 500

/// @brief Given a set of conditions and a time interval,
///                      this solver computes all sub-intervals over which conditions are met.
class TemporalConditionSolver
{
   public:
    typedef std::function<bool(const Instant&)> Condition;

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  TemporalConditionSolver temporalConditionSolver = { Duration::Minutes(1.0),
    ///                  Duration::Microseconds(1.0) };
    /// @endcode
    ///
    /// @param aTimeStep A time step used to generate the temporal grid.
    /// @param aTolerance A temporal tolerance used to determine the switching instant.
    /// @param aMaximumIterationCount The maximum iteration count for the solver.
    TemporalConditionSolver(
        const Duration& aTimeStep,
        const Duration& aTolerance,
        const Size& aMaximumIterationCount = DEFAULT_MAXIMUM_ITERATION_COUNT
    );

    /// @brief Get the time step.
    ///
    /// @return Time step.
    Duration getTimeStep() const;

    /// @brief Get the tolerance.
    ///
    /// @return Tolerance.
    Duration getTolerance() const;

    /// @brief Get the maximum iteration count.
    ///
    /// @return Maximum iteration count.
    Size getMaximumIterationCount() const;

    /// @brief Find the intervals over which the provided condition is true.
    ///
    /// @param aCondition A temporal condition.
    /// @param anInterval A time interval within which to perform the search.
    ///
    /// @return An array of time intervals.
    Array<Interval> solve(const TemporalConditionSolver::Condition& aCondition, const Interval& anInterval) const;

    /// @brief Find the intervals over which all provided conditions are true.
    ///
    /// @param aConditionArray An array of temporal conditions.
    /// @param anInterval A time interval within which to perform the search.
    ///
    /// @return An array of time intervals.
    Array<Interval> solve(const Array<TemporalConditionSolver::Condition>& aConditionArray, const Interval& anInterval)
        const;

   private:
    Duration timeStep_;
    Duration tolerance_;
    Size maximumIterationCount_;

    Instant findSwitchingInstant(
        const Instant& aPreviousInstant,
        const Instant& aNextInstant,
        const Array<TemporalConditionSolver::Condition>& aConditionArray
    ) const;

    static bool EvaluateConditionAt(
        const Instant& anInstant, const Array<TemporalConditionSolver::Condition>& aConditionArray
    );
};

}  // namespace solvers
}  // namespace astro
}  // namespace ostk

#endif
