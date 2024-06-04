/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Validation_MissionSequence__
#define __OpenSpaceToolkit_Astrodynamics_Validation_MissionSequence__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Dictionary.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Astrodynamics/CrossValidator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Parser.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace validation
{

using ostk::astrodynamics::validation::Parser;
using ostk::astrodynamics::validation::QuantityComparison;
using ostk::astrodynamics::validation::ToolComparison;

using ostk::core::container::Array;
using ostk::core::container::Dictionary;
using ostk::core::container::Table;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::Environment;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::trajectory::Sequence;
using ostk::astrodynamics::trajectory::State;

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
}  // namespace astrodynamics
}  // namespace ostk

#endif
