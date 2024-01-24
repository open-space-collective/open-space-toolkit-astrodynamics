/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Validation_MissionSequence__
#define __OpenSpaceToolkit_Astrodynamics_Validation_MissionSequence__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Dictionary.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Astrodynamics/CrossValidator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Parser.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astro
{
namespace validation
{

using ostk::astro::validation::Parser;
using ostk::astro::validation::ToolComparison;
using ostk::astro::validation::QuantityComparison;

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Dictionary;
using ostk::core::ctnr::Table;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::object::VectorXd;

using ostk::physics::Environment;

using ostk::astro::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::trajectory::Sequence;
using ostk::astro::trajectory::State;

/// @brief Holds the OSTk objects and data required to define and run a "Mission Sequence".
class MissionSequence
{
   public:
    /// @brief Constructor
    ///
    /// @param aDataTree A dictionary containing all the data needed to define and initialize a "Mission Sequence".
    MissionSequence(const Dictionary& aDataTree);

    /// @brief Destructor
    ~MissionSequence();

    /// @brief Run the "Mission Sequence".
    void run();

    /// @brief Access the satellite system.
    const SatelliteSystem& accessSatelliteSystem() const;

    /// @brief Access the initial state.
    const State& accessInitialState() const;

    /// @brief Access the environment.
    const Environment& accessEnvironment() const;

    /// @brief Access the dynamics.
    const Array<Shared<Dynamics>>& accessDynamics() const;

    /// @brief Access the sequence.
    const Sequence& accessSequence() const;

    /// @brief Access the solved states.
    const Array<State>& accessSolvedStates() const;

    /// @brief Compare the results of the "Mission Sequence" with the results of a tool.
    ///
    /// @param aReferenceData A table containing the reference data from the tool.
    /// @param aToolComparison A tool comparison object containing the tool name and the quantities to compare.
    /// @return An array of vectors containing the differences between the results of the "Mission Sequence" and the
    /// results of the tool.
    Array<VectorXd> compareResults(const Table& referenceData, const ToolComparison& aToolComparison) const;

   private:
    Dictionary dataTree_;
    SatelliteSystem satelliteSystem_;
    State initialState_;
    Environment environment_;
    Array<Shared<Dynamics>> dynamics_;
    Sequence sequence_;
    Array<State> solvedStates_;
};

}  // namespace validation
}  // namespace astro
}  // namespace ostk

#endif
