/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Validation_CrossValidator__
#define __OpenSpaceToolkit_Astrodynamics_Validation_CrossValidator__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>

namespace ostk
{
namespace astro
{
namespace validation
{

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::Size;
using ostk::core::types::String;

using ostk::math::object::VectorXd;

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
    /// @param aReferenceData An array of vectors containing the reference data from the comparing tool.
    /// @param aToolComparison A tool comparison object containing the tool name and the quantities to compare.
    /// @return An array of vectors containing the differences between the results of the "Mission Sequence" and the
    /// results of the tool.
    static VectorXd CompareOutputQuantities(
        const State& anOSTkState,
        const Array<VectorXd>& aReferenceCoordinates,
        const Array<QuantityComparison>& aQuantityComparisons
    );

    /// @brief Convert a quantity enum to a string
    ///
    /// @param aQuantity A quantity enum
    static String QuantityToString(const Quantity& aQuantity);

   private:
    CrossValidator();
    ~CrossValidator();
};

}  // namespace validation
}  // namespace astro
}  // namespace ostk

#endif
