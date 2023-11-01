/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solvers/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astro
{
namespace solvers
{

using ostk::core::types::Size;
using ostk::core::types::Index;

using ostk::physics::time::Duration;

using ostk::astro::trajectory::StateBuilder;

FiniteDifferenceSolver::FiniteDifferenceSolver(const FiniteDifferenceSolver::Type& aType)
    : type_(aType)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const FiniteDifferenceSolver& aFiniteDifferenceSolver)
{
    aFiniteDifferenceSolver.print(anOutputStream, false);

    return anOutputStream;
}

MatrixXd FiniteDifferenceSolver::computeStateTransitionMatrix(
    const State& aState,
    const Array<Instant>& anInstantArray,
    std::function<Array<State>(const State&, const Array<Instant>&)> getStates,
    const Real& aStepPercentage
) const
{
    const StateBuilder stateBuilder = {aState};

    const Size stateVectorDimension = aState.getSize();

    const auto computeCoordinates = [&getStates, &anInstantArray, &stateVectorDimension](const State& perturbedState
                                    ) -> MatrixXd
    {
        const Array<State> propagatedStates = getStates(perturbedState, anInstantArray);
        MatrixXd perturbedCoordinates(stateVectorDimension, anInstantArray.size());
        for (Index j = 0; j < anInstantArray.size(); ++j)
        {
            perturbedCoordinates.col(j) = propagatedStates[j].getCoordinates();
        }

        return perturbedCoordinates;
    };

    std::function<MatrixXd(const Real&, VectorXd, const Index&)> computeBlock;

    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
            computeBlock = [&computeCoordinates, &stateBuilder, anInstant = aState.accessInstant()](
                               const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                           )
            {
                const MatrixXd coordinates = computeCoordinates(stateBuilder.build(anInstant, aCoordinateVector));

                aCoordinateVector(anIndex) += aStepSize;
                const MatrixXd forwardCoordinates =
                    computeCoordinates(stateBuilder.build(anInstant, aCoordinateVector));

                const MatrixXd differencedCoordinates = (forwardCoordinates - coordinates) / aStepSize;

                return differencedCoordinates;
            };
            break;

        case FiniteDifferenceSolver::Type::Backward:
            computeBlock = [&computeCoordinates, &stateBuilder, anInstant = aState.accessInstant()](
                               const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                           )
            {
                const MatrixXd coordinates = computeCoordinates(stateBuilder.build(anInstant, aCoordinateVector));

                aCoordinateVector(anIndex) -= aStepSize;
                const MatrixXd backwardCoordinates =
                    computeCoordinates(stateBuilder.build(anInstant, aCoordinateVector));

                const MatrixXd differencedCoordinates = (coordinates - backwardCoordinates) / aStepSize;

                return differencedCoordinates;
            };
            break;

        case FiniteDifferenceSolver::Type::Central:
            computeBlock = [&computeCoordinates, &stateBuilder, anInstant = aState.accessInstant()](
                               const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                           )
            {
                // Perturb state forward
                aCoordinateVector(anIndex) += aStepSize;
                const MatrixXd forwardCoordinates =
                    computeCoordinates(stateBuilder.build(anInstant, aCoordinateVector));

                // Perturb state backward
                aCoordinateVector(anIndex) -= 2.0 * aStepSize;
                const MatrixXd backwardCoordinates =
                    computeCoordinates(stateBuilder.build(anInstant, aCoordinateVector));

                const MatrixXd differencedCoordinates = (forwardCoordinates - backwardCoordinates) / (2.0 * aStepSize);

                return differencedCoordinates;
            };
            break;

        default:
            throw ostk::core::error::runtime::Wrong("Finite Difference Solver Type.");
    }

    MatrixXd A = MatrixXd::Zero(stateVectorDimension, stateVectorDimension * anInstantArray.size());

    for (Index i = 0; i < stateVectorDimension; ++i)
    {
        const Real stepSize = (aState.accessCoordinates()(i) * aStepPercentage != 0.0)
                                ? aState.accessCoordinates()(i) * aStepPercentage
                                : aStepPercentage;

        const VectorXd coords = aState.accessCoordinates();
        const MatrixXd differencedCoordinates = computeBlock(stepSize, aState.getCoordinates(), i);

        for (Index j = 0; j < anInstantArray.size(); ++j)
        {
            A.block(0, i + j * stateVectorDimension, stateVectorDimension, 1) = differencedCoordinates.col(j);
        }
    }

    return A;
}

MatrixXd FiniteDifferenceSolver::computeStateTransitionMatrix(
    const State& aState,
    const Instant& anInstant,
    std::function<State(const State&, const Instant&)> getState,
    const Real& aStepPercentage
) const
{
    const auto getStates = [&getState](const State& anInputState, const Array<Instant>& anInstantArray) -> Array<State>
    {
        const State outputState = getState(anInputState, anInstantArray[0]);

        return {outputState};
    };

    return computeStateTransitionMatrix(aState, {anInstant}, getStates, aStepPercentage);
}

VectorXd FiniteDifferenceSolver::computeGradient(
    const State& aState, std::function<State(const State&, const Instant&)> getState, const Duration& aStepSize
) const
{
    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
        {
            const VectorXd coordinates = getState(aState, aState.accessInstant()).getCoordinates();

            const Instant instant = aState.accessInstant() + aStepSize;
            const VectorXd forwardCoordinates = getState(aState, instant).getCoordinates();

            const VectorXd differencedCoordinates = (forwardCoordinates - coordinates) / (aStepSize.inSeconds());

            return differencedCoordinates;
        }

        case FiniteDifferenceSolver::Type::Backward:
        {
            const VectorXd coordinates = getState(aState, aState.accessInstant()).getCoordinates();

            const Instant instant = aState.accessInstant() - aStepSize;
            const VectorXd backwardCoordinates = getState(aState, instant).getCoordinates();

            const VectorXd differencedCoordinates = (coordinates - backwardCoordinates) / (aStepSize.inSeconds());

            return differencedCoordinates;
        }

        case FiniteDifferenceSolver::Type::Central:
        {
            const Instant forwardInstant = aState.accessInstant() + aStepSize;
            const Instant backwardInstant = aState.accessInstant() - aStepSize;

            const VectorXd forwardCoordinates = getState(aState, forwardInstant).accessCoordinates();
            const VectorXd backwardCoordinates = getState(aState, backwardInstant).accessCoordinates();

            return (forwardCoordinates - backwardCoordinates) / (2.0 * aStepSize.inSeconds());
        }

        default:
            throw ostk::core::error::runtime::Wrong("Finite Difference Solver Type.");
    }
}

void FiniteDifferenceSolver::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Finite Difference Solver");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Type: " << FiniteDifferenceSolver::StringFromType(type_);

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

String FiniteDifferenceSolver::StringFromType(const FiniteDifferenceSolver::Type& aType)
{
    switch (aType)
    {
        case FiniteDifferenceSolver::Type::Forward:
            return "Forward";
        case FiniteDifferenceSolver::Type::Backward:
            return "Backward";
        case FiniteDifferenceSolver::Type::Central:
            return "Central";
        default:
            throw ostk::core::error::runtime::Wrong("Type");
    }
}

}  // namespace solvers
}  // namespace astro
}  // namespace ostk
