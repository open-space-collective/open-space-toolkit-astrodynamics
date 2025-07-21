/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Eclipse_Generator__
#define __OpenSpaceToolkit_Astrodynamics_Eclipse_Generator__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Utility/Eclipse.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/TemporalConditionSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace eclipse
{

using ostk::core::container::Array;
using ostk::core::type::String;

using ostk::physics::Environment;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::utilities::Eclipse;
using ostk::physics::time::Duration;
using ostk::physics::time::Interval;

using ostk::astrodynamics::solver::TemporalConditionSolver;
using ostk::astrodynamics::Trajectory;

/// @brief Eclipse generator for trajectory analysis
///
/// Generates eclipses for a given trajectory over a specified analysis interval.
class Generator
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     Generator eclipseGenerator = { environment, Duration::Seconds(60.0), Duration::Milliseconds(1.0) };
    /// @endcode
    ///
    /// @param anEnvironment The Environment to use during the search. Defaults to the default environment.
    /// @param aSearchStepSize The step size to use during the search. Defaults to 60 seconds.
    /// @param aSearchTolerance The tolerance to use during the search. Defaults to 1 millisecond.
    Generator(
        const Environment& anEnvironment = Environment::Default(),
        const Duration& aSearchStepSize = Duration::Seconds(60.0),
        const Duration& aSearchTolerance = Duration::Milliseconds(1.0)
    );

    /// @brief Check if eclipse generator is defined
    ///
    /// @return True if eclipse generator is defined
    bool isDefined() const;

    /// @brief Get the environment
    ///
    /// @return Environment
    Environment getEnvironment() const;

    /// @brief Get the search step size
    ///
    /// @return Search step size
    Duration getSearchStepSize() const;

    /// @brief Get the search tolerance
    ///
    /// @return Search tolerance
    Duration getSearchTolerance() const;

    /// @brief Generate eclipses for a given trajectory over the provided analysis interval
    ///
    /// @code{.cpp}
    ///     Array<Eclipse> eclipses = eclipseGenerator.generate(trajectory, analysisInterval, "Sun", "Earth");
    /// @endcode
    ///
    /// @param aTrajectory The trajectory to search for eclipses
    /// @param anAnalysisInterval The analysis interval
    /// @param anOccultedCelestialObjectName The name of the occulted celestial object (default "Sun")
    /// @param anOccultingCelestialObjectName The name of the occulting celestial object (default "Earth")
    /// @return Array of eclipses
    Array<Eclipse> generate(
        const Trajectory& aTrajectory,
        const Interval& anAnalysisInterval,
        const String& anOccultedCelestialObjectName = "Sun",
        const String& anOccultingCelestialObjectName = "Earth"
    ) const;

   private:
    Environment environment_;
    TemporalConditionSolver temporalConditionSolver_;
};

}  // namespace eclipse
}  // namespace astrodynamics
}  // namespace ostk

#endif
