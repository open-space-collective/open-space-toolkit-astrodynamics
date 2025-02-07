/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/Estimator.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace solver
{


using ostk::physics::coordinate::Frame;

Estimator::Analysis::Analysis(
    const Real& anRmsError,
    const Array<State>& anEstimatedStatesArray,
    const MatrixXd& anEstimatedCovariance,
    const Array<State>& aComputedObservationsStateArray,
)
    : rmsError(anRmsError),
      observationCount(aComputedObservationsStateArray.getSize()),
      estimatedStates(anEstimatedStatesArray),
      estimatedCovariance(anEstimatedCovariance),
      computedObservationStates(aComputedObservationsStateArray),
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const Estimator::Analysis& anAnalysis)
{
    anAnalysis.print(anOutputStream);

    return anOutputStream;
}

void Estimator::Analysis::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Solver Analysis") : void();

    ostk::core::utils::Print::Line(anOutputStream) << "RMS Error: " << rmsError;

    ostk::core::utils::Print::Separator(anOutputStream, "Estimated State");
    for (const auto& state : estimatedStates)
    {
        estimatedState.print(anOutputStream, false);
    }

    ostk::core::utils::Print::Footer(anOutputStream);

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Array<State> Estimator::Analysis::computeResidualStates(const Array<State>& anObservationStateArray) const
{
    if (computedObservationStates.getSize() != anObservationStateArray.getSize())
    {
        throw ostk::core::error::RuntimeError(
            "Computed observation states and observation states arrays must have the same length."
        );
    }

    Array<State> residualStates;
    residualStates.reserve(anObservationStateArray.getSize());

    for (Size i = 0; i < anObservationStateArray.getSize(); ++i)
    {
        residualStates.add(
            anObservationStateArray[i] - computedObservationStates[i].inFrame(anObservationStateArray[i].accessFrame())
        );
    }

    return residualStates;
}

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk
