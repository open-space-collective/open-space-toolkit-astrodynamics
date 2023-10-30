/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solvers/FiniteDifferenceSolver.hpp>

namespace ostk
{
namespace astro
{
namespace solvers
{

using ostk::core::types::Size;
using ostk::core::types::Index;

using ostk::astro::trajectory::StateBuilder;

FiniteDifferenceSolver::FiniteDifferenceSolver(const FiniteDifferenceSolver::Type& aType, const Real& aStepPercentage)
    : type_(aType),
      stepPercentage_(aStepPercentage)
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
    const StateBuilder stateBuilder = {aState};

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
    };

    std::function<MatrixXd(const Real&, const VectorXd&, const Index&)> computeJacobianBlock;

    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
            compute =
                [&computeSteps, &stateBuilder](const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex)
            {
                aCoordinateVector(anIndex) += aStepSize;
                const MatrixXd coordinates = computeSteps(stateBuilder.build(aCoordinateVector));

                return (coordinates - aCoordinatevector) / (aStepSize);
            };
        case FiniteDifferenceSolver::Type::Backward:
            compute =
                [&computeSteps, &stateBuilder](const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex)
            {
                aCoordinateVector(anIndex) -= aStepSize;
                const MatrixXd coordinates = computeSteps(stateBuilder.build(aCoordinateVector));

                return (coordinates - aCoordinatevector) / (aStepSize);
            };

        case FiniteDifferenceSolver::Type::Central:
            compute =
                [&computeSteps, &stateBuilder](const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex)
            {
                // Perturb state forward
                aCoordinateVectpr(anIndex) += aStepSize;
                const MatrixXd forwardCoordinates = computeSteps(perturbedCoordinates);

                // Perturb state backward
                aCoordinateVectpr(anIndex) -= 2 * aStepSize;
                const MatrixXd backwardCoordinates = computeSteps(perturbedCoordinates);

                return (forwardCoordinates - backwardCoordinates) / (2.0 * stepSize);
            };

        default:
            break;
    }

    for (Index i = 0; i < stateVectorDimension; ++i)
    {
        const Real stepSize = (aState.accessCoordinates()(i) * stepPercentage_ != 0.0)
                                ? aState.accessCoordinates()(i) * stepPercentage_
                                : stepPercentage_;

        const Matrix differencedCoordinates = computeJacobianBlock(stepSize, aState.accessCoordinates(), i);

        for (Index j = 0; j < anInstantArray.size(); ++j)
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
    const State& aState, std::function<State(const State&, const Instant&)> getState,
)
{
    const auto compute = [&aState, &getState](const Duration& aDurationStep)
    {
        const Instant instant = aState.accessInstant() + aDurationStep;
        const VectorXd coordinates = getState(aState, instant);

        return (coordinates - aState.accessCoordinates()) / (aDurationStep.inSeconds());
    };

    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
            return compute(Duration::Seconds(stepPercentage));

        case FiniteDifferenceSolver::Type::Backward:
            return compute(-Duration::Seconds(stepPercentage));

        case FiniteDifferenceSolver::Type::Central:
            const Instant forwardInstant = aState.accessInstant() + Duration::Seconds(stepPercentage_);
            const Instant backwardInstant = aState.accessInstant() - Duration::Seconds(stepPercentage_);

            const VectorXd forwardCoordinates = getStates(aState, forwardInstant);
            const VectorXd backwardCoordinates = getStates(aState, backwardInstant);

            return (forwardCoordinates - backwardCoordinates) / (2.0 * stepPercentage_);

        default:
            throw ostk::core::error::runtime::Wrong("Finite Difference Solver Type.")
    }
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
