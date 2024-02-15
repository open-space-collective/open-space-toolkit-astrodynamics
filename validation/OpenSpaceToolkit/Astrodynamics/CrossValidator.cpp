/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/CrossValidator.hpp>

namespace ostk
{
namespace astro
{
namespace validation
{

Array<Array<VectorXd>> CrossValidator::IngestOutputQuantities(
    const Table& aTable, const ToolComparison& aToolComparison
)
{
    const Array<QuantityComparison>& quantityComparisons = aToolComparison.quantityComparisons;

    Array<Array<VectorXd>> groupedCoordinatesArray = Array<Array<VectorXd>>::Empty();

    for (const auto& row : aTable)
    {
        Array<VectorXd> coordinateSubsets = Array<VectorXd>::Empty();

        Size rowReadIndex = 0;
        for (Size coordinateSubsetIndex = 0; coordinateSubsetIndex < quantityComparisons.getSize();
             coordinateSubsetIndex++)
        {
            const Quantity& quantity = quantityComparisons[coordinateSubsetIndex].quantity;
            const Size& quantityDimension = QuantityDimensionMap.at(quantity);

            VectorXd coordinateSubset(quantityDimension);

            for (Size quantityDimensionIndex = 0; quantityDimensionIndex < quantityDimension; quantityDimensionIndex++)
            {
                if ((aToolComparison.tool == Tool::GMAT) && (quantity != Quantity::MASS))  // Brittle here
                {
                    coordinateSubset(quantityDimensionIndex) = row[1 + rowReadIndex].accessReal() * 1.0e3;
                }
                else
                {
                    coordinateSubset(quantityDimensionIndex) = row[1 + rowReadIndex].accessReal();  // Gmat and Orekit using different scales?
                }
                rowReadIndex++;
            }
            coordinateSubsets.add(coordinateSubset);
        }
        groupedCoordinatesArray.add(coordinateSubsets);
    }

    // TBI: read the "seconds elapsed" from reference data and check to make sure its within tolerance of ostk
    // timestamps
    return groupedCoordinatesArray;
}

VectorXd CrossValidator::CompareOutputQuantities(
    const State& anOSTkState,
    const Vector3d& anAccelerationVector,
    const Array<VectorXd>& aReferenceCoordinates,
    const Array<QuantityComparison>& aQuantityComparisons
)
{
    VectorXd deltaWithTool(aQuantityComparisons.getSize());

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    std::cout << "Quantity comparisons size" << aQuantityComparisons.getSize() << std::endl;

    for (Size coordinateSubsetIndex = 0; coordinateSubsetIndex < aQuantityComparisons.getSize();  // TBI: Should rename as we store also deltas on accelerations (not related to a coordinateSubset)
         coordinateSubsetIndex++)
    {
        // Get the desired output and the solved state
        VectorXd referenceCoordinateSubset = aReferenceCoordinates[coordinateSubsetIndex];

        switch (aQuantityComparisons[coordinateSubsetIndex].quantity)
        {
            case Quantity::CARTESIAN_POSITION_GCRF:
            {
                const VectorXd ostkCoordinateSubset = anOSTkState.inFrame(gcrfSPtr_).getPosition().getCoordinates();

                deltaWithTool[coordinateSubsetIndex] = (referenceCoordinateSubset - ostkCoordinateSubset).norm();
                break;
            }
            case Quantity::CARTESIAN_VELOCITY_GCRF:
            {
                const VectorXd ostkCoordinateSubset = anOSTkState.inFrame(gcrfSPtr_).getVelocity().getCoordinates();

                deltaWithTool[coordinateSubsetIndex] = (referenceCoordinateSubset - ostkCoordinateSubset).norm();
                break;
            }
            case Quantity::MASS:
            // {
            //     const VectorXd ostkCoordinateSubset = anOSTkState.inFrame(gcrfSPtr_).extractCoordinates(CoordinatesSubset::Mass());

            //     deltaWithTool[coordinateSubsetIndex] = (referenceCoordinateSubset - ostkCoordinateSubset).norm();
            //     break;
            // }
            case Quantity::CARTESIAN_ACCELERATION_GCRF:
            {
                std::cout << "HERE" << std::endl;
                deltaWithTool[coordinateSubsetIndex] = (referenceCoordinateSubset - anAccelerationVector).norm();
                break;
            }
            case Quantity::MANEUVER_ACCELERATION_J2000:
            default:
                throw ostk::core::error::runtime::Wrong("Quantity");
        }
    }

    return deltaWithTool;
}

Size CrossValidator::FindMaxDeltaIndex(const Array<VectorXd>& allDeltasWithTool, const Size& aCoordinateSubsetIndex)
{
    if (aCoordinateSubsetIndex >= allDeltasWithTool[0].size())
    {
        throw ostk::core::error::runtime::Wrong("Coordinate subset index out of bounds");
    };

    const auto currentDeltaCompare = [&aCoordinateSubsetIndex](const VectorXd& a, const VectorXd& b) -> bool
    {
        return a[aCoordinateSubsetIndex] < b[aCoordinateSubsetIndex];
    };

    const auto maxElementIt = std::max_element(allDeltasWithTool.begin(), allDeltasWithTool.end(), currentDeltaCompare);

    return std::distance(allDeltasWithTool.begin(), maxElementIt);
}

String CrossValidator::QuantityToString(const Quantity& aQuantity)
{
    switch (aQuantity)
    {
        case CARTESIAN_POSITION_GCRF:
            return "CARTESIAN_POSITION_GCRF";
        case CARTESIAN_VELOCITY_GCRF:
            return "CARTESIAN_VELOCITY_GCRF";
        case CARTESIAN_ACCELERATION_GCRF:
            return "CARTESIAN_ACCELERATION_GCRF";
        case MASS:
            return "MASS";
        case MANEUVER_ACCELERATION_J2000:
            return "MANEUVER_ACCELERATION_J2000";
        default:
            throw ostk::core::error::runtime::Wrong("Quantity not recognized.");
    }
}

String CrossValidator::ToolToString(const Tool& aTool)
{
    switch (aTool)
    {
        case Tool::GMAT:
            return "GMAT";
        case Tool::OREKIT:
            return "OREKIT";
        default:
            throw ostk::core::error::runtime::Wrong("Tool not recognized.");
    }
}

String CrossValidator::ToolToPath(const Tool& aTool)
{
    switch (aTool)
    {
        case Tool::GMAT:
            return "gmat_astrodynamics";
        case Tool::OREKIT:
            return "orekit_astrodynamics";
        default:
            throw ostk::core::error::runtime::Wrong("Tool not recognized.");
    }
}

}  // namespace validation
}  // namespace astro
}  // namespace ostk
