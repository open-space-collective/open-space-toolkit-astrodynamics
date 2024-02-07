/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Validation_CrossValidator__
#define __OpenSpaceToolkit_Astrodynamics_Validation_CrossValidator__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace validation
{

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coord::Frame;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesSubset;

enum Quantity  // Enum values here correspond to index of each output in the csv file
{
    CARTESIAN_POSITION_GCRF,
    CARTESIAN_VELOCITY_GCRF,
    CARTESIAN_ACCELERATION_GCRF,
    MASS,
    MANEUVER_ACCELERATION_J2000,
};

const std::map<Quantity, Size> QuantityDimensionMap = {
    {Quantity::CARTESIAN_POSITION_GCRF, 3},
    {Quantity::CARTESIAN_VELOCITY_GCRF, 3},
    {Quantity::CARTESIAN_ACCELERATION_GCRF, 3},
    {Quantity::MASS, 1},
    {Quantity::MANEUVER_ACCELERATION_J2000, 3},
};

enum class Tool
{
    GMAT,
    OREKIT,
};

struct QuantityComparison  // TBI add logic to check for empty or non-matching length arrays
{
    Quantity quantity;
    Real tolerance;
};

struct ToolComparison  // TBI add logic to check for empty or non-matching length arrays
{
    Tool tool;
    Array<QuantityComparison> quantityComparisons;
};

/// @brief Help comparing the results of the OSTk "Mission Sequence" with the results of a comparison tool.
class CrossValidator
{
   public:
    /// @brief Create an array of outputs quantities from a table of reference data
    ///
    /// @param aTable A table containing the reference data from the tool.
    /// @param aToolComparison A tool comparison object containing the tool name and the quantities to compare.
    /// @return An array of vectors containing the reference data from the tool.
    static Array<Array<VectorXd>> IngestOutputQuantities(const Table& aTable, const ToolComparison& aToolComparison);

    /// @brief Compare the results of the "Mission Sequence" with the results of a tool.
    ///
    /// @param anOSTkState An OSTk state to compare.
    /// @param aReferenceCoordinates An array of vectors containing the reference data from the tool.
    /// @param aQuantityComparisons An array of quantity comparison objects containing the quantities to compare.
    /// @return A vector containing the differences between the results of the "Mission Sequence" and the
    /// results of the tool at the current tool, for all output quantities.
    static VectorXd CompareOutputQuantities(
        const State& anOSTkState,
        const Array<VectorXd>& aReferenceCoordinates,
        const Array<QuantityComparison>& aQuantityComparisons
    );

    /// @brief Find the index of largest delta in a timeseries array of a particular coordinate subset's delta
    ///
    /// @param allDeltasWithTool An array of vectors containing the differences between the results of the
    /// "Mission Sequence" and the results of the tool, for all output quantities.
    /// @param aCoordinateSubsetIndex The index of the coordinate subset corresponding to the current output quantity to
    /// find the largest delta for.
    /// @return The index of the largest delta.
    static Size FindMaxDeltaIndex(const Array<VectorXd>& allDeltasWithTool, const Size& aCoordinateSubsetIndex);

    /// @brief Convert a quantity enum to a string
    ///
    /// @param aQuantity A quantity enum
    static String QuantityToString(const Quantity& aQuantity);

    /// @brief Convert a tool enum to a string
    ///
    /// @param aTool A tool enum
    static String ToolToString(const Tool& aTool);

    /// @brief Convert a tool enum to a path to the tool's output file
    ///
    /// @param aTool A tool enum
    static String ToolToPath(const Tool& aTool);

   private:
    CrossValidator();
    ~CrossValidator();
};

}  // namespace validation
}  // namespace astro
}  // namespace ostk

#endif
