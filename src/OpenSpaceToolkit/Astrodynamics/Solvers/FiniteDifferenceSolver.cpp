/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Solvers/FiniteDifferenceSolver.hpp>

namespace ostk
{
namespace astro
{
namespace solvers
{

FiniteDifferenceSolver::FiniteDifferenceSolver(const Real& aStepPercentage)
    : stepPercentage_(aStepPercentage)
{
    if (!aStepPercentage.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Step Percentage.");
    }
}

std::ostream& operator<<(std::ostream& anOutputStream, const FiniteDifferenceSolver& aFiniteDifferenceSolver)
{
    aSeaFiniteDifferenceSolverquence.print(anOutputStream, false);

    return anOutputStream;
}

MatrixXd FiniteDifferenceSolver::computeJacobian(
    const State& aState,
    const Array<Instant>& anInstantArray,
    std::function<Array<State>(const State&, const Array<Instant>&)> getStates,
)
{
    const Size stateVectorDimension = aState.getSize();

    MatrixXd A = MatrixXd::Zero(stateVectorDimension, stateVectorDimension * anInstantArray.size());

    const auto computeSteps = [&anInstantArray, &stateVectorDimension](const State& aState) -> MatrixXd
    {
        Array<State> propagatedSteps = getStates(aState, anInstantArray);
        MatrixXd perturbedCoordinates(stateVectorDimension, anInstantArray.size());
        for (int j = 0; j < anInstantArray.size(); ++j)
        {
            perturbedCoordinates.col(j) = propagatedSteps[j].accessCoordinates();
        }

        return perturbedCoordinates;
    }

    for (int i = 0; i < stateVectorDimension; ++i)
    {
        VectorXd perturbedCoordinates = aState.accessCoordinates();

        const Real stepSize = (perturbedCoordinates(i) * stepPercentage_ != 0.0)
                                ? perturbedCoordinates(i) * stepPercentage_
                                : stepPercentage_;

        // Perturb state forward
        perturbedCoordinates(i) += stepSize;
        const MatrixXd forwardCoordinates = computeSteps(perturbedCoordinates);

        // Perturb state backward
        perturbedCoordinates(i) -= 2 * stepSize;
        const MatrixXd backwardCoordinates = computeSteps(perturbedCoordinates);

        // Central difference
        const MatrixXd differencedCoordinates = (forwardCoordinates - backwardCoordinates) / (2.0 * stepSize);

        for (int j = 0; j < anInstantArray.size(); ++j)
        {
            A.block(0, i + j * stateVectorDimension, stateVectorDimension, 1) = differencedCoordinates.col(j);
        }
    }
    return A;
}

MatrixXd FiniteDifferenceSolver::computeJacobian(
    const State& aState, const Instant& anInstant, std::function<State(const State&, const Instant&)> getState,
)
{
    const auto getStates = [&getState](const State& aState, const Array<Instant>& anInstantArray) -> Array<State>
    {
        Array<State> states = Array::Empty<State>();
        states.reserve(anInstantArray.getSize());

        for (const Instant& anInstant : anInstantArray)
        {
            states.add(getState(aState, anInstant));
        }

        return states;
    };

    return computeJacobian(aState, {anInstant}, getStates);
}

VectorXd FiniteDifferenceSolver::computeGradient(
    const State& aState, std::function<VectorXd(const State&, const Instant&)> getStates,
)
{
    const Instant forwardInstant = aState.accessInstant() + Duration::Seconds(stepPercentage_);
    const Instant backwardInstant = aState.accessInstant() - Duration::Seconds(stepPercentage_);

    const VectorXd forwardCoordinates = getStates(aState, forwardInstant);
    const VectorXd backwardCoordinates = getStates(aState, backwardInstant);

    return (forwardCoordinates - backwardCoordinates) / (2.0 * stepPercentage_);
}

void FiniteDifferenceSolver::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Finite Difference Solver");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Step percentage:" << stepPercentage_.toString();

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

}  // namespace solvers
}  // namespace astro
}  // namespace ostk
