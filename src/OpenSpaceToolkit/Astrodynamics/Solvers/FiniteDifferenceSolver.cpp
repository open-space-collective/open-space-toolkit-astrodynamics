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

MatrixXd FiniteDifferenceSolver::computeJacobian(
    const State& aState,
    const Array<Instant>& anInstantArray,
    std::function<Array<State>(const State&, const Array<Instant>&)> getStates,
    const Real& aStepPercentage
) const
{
    const StateBuilder stateBuilder = {aState};

    const Size stateVectorDimension = aState.getSize();

    MatrixXd A = MatrixXd::Zero(stateVectorDimension, stateVectorDimension * anInstantArray.size());

    const auto computeSteps = [&getStates, &anInstantArray, &stateVectorDimension](const State& perturbedState
                              ) -> MatrixXd
    {
        Array<State> propagatedSteps = getStates(perturbedState, anInstantArray);
        MatrixXd perturbedCoordinates(stateVectorDimension, anInstantArray.size());
        for (Index j = 0; j < anInstantArray.size(); ++j)
        {
            perturbedCoordinates.col(j) = propagatedSteps[j].getCoordinates();
        }

        std::cout << "perturbed coordinates: \n" << perturbedCoordinates << std::endl;

        return perturbedCoordinates;
    };

    std::function<MatrixXd(const Real&, VectorXd, const Index&)> computeJacobianBlock;

    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
            computeJacobianBlock = [&computeSteps, &stateBuilder, anInstant = aState.accessInstant()](
                                       const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                                   )
            {
                aCoordinateVector(anIndex) += aStepSize;
                const MatrixXd coordinates = computeSteps(stateBuilder.build(anInstant, aCoordinateVector));

                return (coordinates - aCoordinateVector) / (aStepSize);
            };
            break;
        case FiniteDifferenceSolver::Type::Backward:
            computeJacobianBlock = [&computeSteps, &stateBuilder, anInstant = aState.accessInstant()](
                                       const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                                   )
            {
                aCoordinateVector(anIndex) -= aStepSize;
                const MatrixXd coordinates = computeSteps(stateBuilder.build(anInstant, aCoordinateVector));

                return (coordinates - aCoordinateVector) / (aStepSize);
            };
            break;

        case FiniteDifferenceSolver::Type::Central:
            computeJacobianBlock = [&computeSteps, &stateBuilder, anInstant = aState.accessInstant()](
                                       const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                                   )
            {
                // Perturb state forward
                aCoordinateVector(anIndex) += aStepSize;
                std::cout << "forward step: " << aCoordinateVector.transpose() << std::endl;
                const MatrixXd forwardCoordinates = computeSteps(stateBuilder.build(anInstant, aCoordinateVector));

                // Perturb state backward
                aCoordinateVector(anIndex) -= 2.0 * aStepSize;
                std::cout << "backward step: " << aCoordinateVector.transpose() << std::endl;
                const MatrixXd backwardCoordinates = computeSteps(stateBuilder.build(anInstant, aCoordinateVector));

                std::cout << "forward coordinates: " << forwardCoordinates.transpose() << std::endl;
                std::cout << "backward coordinates: " << backwardCoordinates.transpose() << std::endl;

                return (forwardCoordinates - backwardCoordinates) / (2.0 * aStepSize);
            };
            break;

        default:
            throw ostk::core::error::runtime::Wrong("Finite Difference Solver Type.");
    }

    for (Index i = 0; i < stateVectorDimension; ++i)
    {
        const Real stepSize = (aState.accessCoordinates()(i) * aStepPercentage != 0.0)
                                ? aState.accessCoordinates()(i) * aStepPercentage
                                : aStepPercentage;

        const MatrixXd differencedCoordinates = computeJacobianBlock(stepSize, aState.accessCoordinates(), i);

        std::cout << "differenced coordinates: " << differencedCoordinates << std::endl;

        for (Index j = 0; j < anInstantArray.size(); ++j)
        {
            A.block(0, i + j * stateVectorDimension, stateVectorDimension, 1) = differencedCoordinates.col(j);
        }
    }
    return A;
}

MatrixXd FiniteDifferenceSolver::computeJacobian(
    const State& aState,
    const Instant& anInstant,
    std::function<State(const State&, const Instant&)> getState,
    const Real& aStepPercentage
) const
{
    const auto getStates = [&getState](const State& state, const Array<Instant>& anInstantArray) -> Array<State>
    {
        Array<State> states = Array<State>::Empty();
        states.reserve(anInstantArray.getSize());

        for (const Instant& instant : anInstantArray)
        {
            states.add(getState(state, instant));
        }

        return states;
    };

    return computeJacobian(aState, {anInstant}, getStates, aStepPercentage);
}

VectorXd FiniteDifferenceSolver::computeGradient(
    const State& aState, std::function<State(const State&, const Instant&)> getState, const Duration& aStepSize
) const
{
    const auto compute = [&aState, &getState](const Duration& aDurationStep)
    {
        const Instant instant = aState.accessInstant() + aDurationStep;
        const VectorXd coordinates = getState(aState, instant).getCoordinates();

        return (coordinates - aState.accessCoordinates()) / (aDurationStep.inSeconds());
    };

    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
            return compute(aStepSize);

        case FiniteDifferenceSolver::Type::Backward:
            return compute(-aStepSize);

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

String FiniteDifferenceSolver::StringFromType(const FiniteDifferenceSolver::Type& aType_)
{
    switch (aType_)
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
