/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_CloseApproach_Generator__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/TemporalConditionSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace closeapproach
{

using ostk::core::container::Array;
using ostk::core::type::Real;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::solver::TemporalConditionSolver;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::State;

/// @brief Compute close approaches to a reference trajectory
///
/// @details This class computes close approach events between a reference trajectory and other object trajectories
/// over a specified time interval. It uses a temporal condition solver to identify time periods when objects
/// are approaching and then determines the exact time of closest approach.
class Generator
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              Trajectory referenceTrajectory = { ... } ;
    ///              Duration step = Duration::Minutes(20.0) ;
    ///              Duration tolerance = Duration::Milliseconds(1.0) ;
    ///              Generator generator = { referenceTrajectory, step, tolerance };
    /// @endcode
    ///
    /// @param aReferenceTrajectory The reference trajectory for which to compute close approaches (Object 1)
    /// @param aStep The step to use during the close approach search. Set it to a duration smaller than the minimum
    /// possible interval where both objects can be moving apart - which is about a quarter of an orbital period.
    /// Defaults to 20 minutes - but it should be set lower for low velocity conjunctions as they tend to exhibit more
    /// than two close approaches per orbit in a non determinstic manner.
    /// @param aTolerance The tolerance to use during the close approach search. Defaults to 1 millisecond - which means
    /// that objects moving at 7km/s will be up to 7m away from their “true” position.
    Generator(
        const Trajectory& aReferenceTrajectory,
        const Duration& aStep = Duration::Minutes(20.0),
        const Duration& aTolerance = Duration::Milliseconds(1.0)
    );

    /// @brief Check if the generator is defined
    ///
    /// @code{.cpp}
    ///              Generator generator = { ... } ;
    ///              generator.isDefined() ;
    /// @endcode
    ///
    /// @return True if generator is defined
    bool isDefined() const;

    /// @brief Get the reference trajectory
    ///
    /// @code{.cpp}
    ///              Generator generator = { ... } ;
    ///              Trajectory referenceTrajectory = generator.getReferenceTrajectory() ;
    /// @endcode
    ///
    /// @return The reference trajectory
    Trajectory getReferenceTrajectory() const;

    /// @brief Get the step
    ///
    /// @code{.cpp}
    ///              Generator generator = { ... } ;
    ///              Duration step = generator.getStep() ;
    /// @endcode
    ///
    /// @return The step
    Duration getStep() const;

    /// @brief Get the tolerance
    ///
    /// @code{.cpp}
    ///              Generator generator = { ... } ;
    ///              Duration tolerance = generator.getTolerance() ;
    /// @endcode
    ///
    /// @return The tolerance
    Duration getTolerance() const;

    /// @brief Compute close approaches between the reference trajectory and another object over a search interval
    ///
    /// @code{.cpp}
    ///              Generator generator = { ... } ;
    ///              Trajectory otherObjectTrajectory = { ... } ;
    ///              Interval searchInterval = { ... } ;
    ///              Array<CloseApproach> closeApproaches =
    ///                  generator.computeCloseApproaches(otherObjectTrajectory, searchInterval) ;
    /// @endcode
    ///
    /// @param aTrajectory The trajectory of the other object (Object 2)
    /// @param aSearchInterval The interval over which close approaches are searched
    /// @return Array of close approaches over the search interval (with Object 1 being the reference trajectory)
    Array<CloseApproach> computeCloseApproaches(const Trajectory& aTrajectory, const Interval& aSearchInterval) const;

    /// @brief Set the step
    ///
    /// @code{.cpp}
    ///              Generator generator = { ... } ;
    ///              generator.setStep(Duration::Seconds(30.0)) ;
    /// @endcode
    ///
    /// @param aStep The step
    void setStep(const Duration& aStep);

    /// @brief Set the tolerance
    ///
    /// @code{.cpp}
    ///              Generator generator = { ... } ;
    ///              generator.setTolerance(Duration::Microseconds(100.0)) ;
    /// @endcode
    ///
    /// @param aTolerance The tolerance
    void setTolerance(const Duration& aTolerance);

    /// @brief Construct an undefined generator
    ///
    /// @code{.cpp}
    ///              Generator generator = Generator::Undefined() ;
    /// @endcode
    ///
    /// @return Undefined generator
    static Generator Undefined();

   private:
    Trajectory referenceTrajectory_;
    Duration step_;
    Duration tolerance_;

    /// @brief Compute the relative distance time derivative between two states
    ///
    /// @param aState1 The first state
    /// @param aState2 The second state
    /// @return The relative distance time derivative (in m/s)
    static Real ComputeRelativeDistanceDerivative_(const State& aState1, const State& aState2);
};

}  // namespace closeapproach
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
