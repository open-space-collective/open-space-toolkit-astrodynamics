/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace solver
{

using ostk::core::type::Index;
using ostk::core::type::Size;

using ostk::physics::time::Duration;

using ostk::astrodynamics::trajectory::StateBuilder;

FiniteDifferenceSolver::FiniteDifferenceSolver(
    const FiniteDifferenceSolver::Type& aType, const Real& aStepPercentage, const Duration& aStepDuration
)
    : type_(aType),
      stepPercentage_(aStepPercentage),
      stepDuration_(aStepDuration)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const FiniteDifferenceSolver& aFiniteDifferenceSolver)
{
    aFiniteDifferenceSolver.print(anOutputStream, false);

    return anOutputStream;
}

FiniteDifferenceSolver::Type FiniteDifferenceSolver::getType() const
{
    return type_;
}

Real FiniteDifferenceSolver::getStepPercentage() const
{
    return stepPercentage_;
}

Duration FiniteDifferenceSolver::getStepDuration() const
{
    return stepDuration_;
}

Array<MatrixXd> FiniteDifferenceSolver::computeStateTransitionMatrix(
    const State& aState,
    const Array<Instant>& anInstantArray,
    const std::function<MatrixXd(const State&, const Array<Instant>&)>& generateStatesCoordinates
) const
{
    const StateBuilder stateBuilder = {aState};

    const Size stateVectorDimension = aState.getSize();
    const Size numberOfInstants = anInstantArray.getSize();

    std::function<MatrixXd(const Real&, VectorXd, const Index&)> computeBlock;

    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
            computeBlock =
                [&generateStatesCoordinates, &stateBuilder, anInstant = aState.accessInstant(), &anInstantArray](
                    const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                ) -> MatrixXd
            {
                const MatrixXd coordinates =
                    generateStatesCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                aCoordinateVector(anIndex) += aStepSize;
                const MatrixXd forwardCoordinates =
                    generateStatesCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                return (forwardCoordinates - coordinates) / aStepSize;
            };
            break;

        case FiniteDifferenceSolver::Type::Backward:
            computeBlock =
                [&generateStatesCoordinates, &stateBuilder, anInstant = aState.accessInstant(), &anInstantArray](
                    const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                ) -> MatrixXd
            {
                const MatrixXd coordinates =
                    generateStatesCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                aCoordinateVector(anIndex) -= aStepSize;
                const MatrixXd backwardCoordinates =
                    generateStatesCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                return (coordinates - backwardCoordinates) / aStepSize;
            };
            break;

        case FiniteDifferenceSolver::Type::Central:
            computeBlock =
                [&generateStatesCoordinates, &stateBuilder, anInstant = aState.accessInstant(), &anInstantArray](
                    const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex
                ) -> MatrixXd
            {
                aCoordinateVector(anIndex) += aStepSize;
                const MatrixXd forwardCoordinates =
                    generateStatesCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                aCoordinateVector(anIndex) -= 2.0 * aStepSize;
                const MatrixXd backwardCoordinates =
                    generateStatesCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                return (forwardCoordinates - backwardCoordinates) / (2.0 * aStepSize);
            };
            break;

        default:
            throw ostk::core::error::runtime::Wrong("Finite Difference Solver Type.");
    }

    const MatrixXd matrixResult = generateStatesCoordinates(aState, {aState.accessInstant()});

    const Size coordinateDimension = matrixResult.rows();

    Array<MatrixXd> stateTransitionMatrices(
        numberOfInstants, MatrixXd::Zero(coordinateDimension, stateVectorDimension)
    );

    for (Index i = 0; i < stateVectorDimension; ++i)
    {
        const Real stepSize = (aState.accessCoordinates()(i) * stepPercentage_ != 0.0)
                                ? aState.accessCoordinates()(i) * stepPercentage_
                                : stepPercentage_;

        const MatrixXd differencedCoordinates = computeBlock(stepSize, aState.accessCoordinates(), i);

        for (Index k = 0; k < numberOfInstants; ++k)
        {
            stateTransitionMatrices[k].col(i) = differencedCoordinates.col(k);
        }
    }

    return stateTransitionMatrices;
}

MatrixXd FiniteDifferenceSolver::computeStateTransitionMatrix(
    const State& aState,
    const Instant& anInstant,
    const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates
) const
{
    const auto generateStatesCoordinates =
        [&generateStateCoordinates](const State& anInputState, const Array<Instant>& anInstantArray) -> MatrixXd
    {
        const VectorXd coordinates = generateStateCoordinates(anInputState, anInstantArray[0]);

        const Eigen::Map<const MatrixXd> coordinatesMatrix(coordinates.data(), coordinates.size(), 1);

        return coordinatesMatrix;
    };

    return computeStateTransitionMatrix(aState, {anInstant}, generateStatesCoordinates)[0];
}

VectorXd FiniteDifferenceSolver::computeGradient(
    const State& aState, const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates
) const
{
    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
        {
            const VectorXd& coordinates = generateStateCoordinates(aState, aState.getInstant());

            const Instant instant = aState.accessInstant() + stepDuration_;
            const VectorXd forwardCoordinates = generateStateCoordinates(aState, instant);

            return (forwardCoordinates - coordinates) / (stepDuration_.inSeconds());
        }

        case FiniteDifferenceSolver::Type::Backward:
        {
            const VectorXd& coordinates = generateStateCoordinates(aState, aState.getInstant());

            const Instant instant = aState.accessInstant() - stepDuration_;
            const VectorXd backwardCoordinates = generateStateCoordinates(aState, instant);

            return (coordinates - backwardCoordinates) / (stepDuration_.inSeconds());
        }

        case FiniteDifferenceSolver::Type::Central:
        {
            const Instant forwardInstant = aState.accessInstant() + stepDuration_;
            const Instant backwardInstant = aState.accessInstant() - stepDuration_;

            const VectorXd forwardCoordinates = generateStateCoordinates(aState, forwardInstant);
            const VectorXd backwardCoordinates = generateStateCoordinates(aState, backwardInstant);

            return (forwardCoordinates - backwardCoordinates) / (2.0 * stepDuration_.inSeconds());
        }

        default:
            throw ostk::core::error::runtime::Wrong("Finite Difference Solver Type.");
    }
}

MatrixXd FiniteDifferenceSolver::computeJacobian(
    const State& aState, const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates
) const
{
    const Size stateVectorDimension = aState.getSize();
    const VectorXd baseCoordinates = generateStateCoordinates(aState, aState.accessInstant());
    const Size coordinatesDimension = baseCoordinates.size();

    MatrixXd jacobian = MatrixXd::Zero(coordinatesDimension, stateVectorDimension);

    const StateBuilder stateBuilder = {aState};

    std::function<VectorXd(const Real&, const Index&)> computeBlock;

    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
            computeBlock = [&generateStateCoordinates, &stateBuilder, &aState, this](
                               const Real& aStepSize, const Index& anIndex
                           ) -> VectorXd
            {
                VectorXd perturbedCoordinates = aState.accessCoordinates();

                perturbedCoordinates(anIndex) += aStepSize;
                const VectorXd forwardDerivatives = computeGradient(
                    stateBuilder.build(aState.accessInstant(), perturbedCoordinates), generateStateCoordinates
                );

                const VectorXd currentDerivatives = computeGradient(aState, generateStateCoordinates);

                return (forwardDerivatives - currentDerivatives) / aStepSize;
            };
            break;

        case FiniteDifferenceSolver::Type::Backward:
            computeBlock = [&generateStateCoordinates, &stateBuilder, &aState, this](
                               const Real& aStepSize, const Index& anIndex
                           ) -> VectorXd
            {
                VectorXd perturbedCoordinates = aState.accessCoordinates();

                perturbedCoordinates(anIndex) -= aStepSize;
                const VectorXd backwardDerivatives = computeGradient(
                    stateBuilder.build(aState.accessInstant(), perturbedCoordinates), generateStateCoordinates
                );

                const VectorXd currentDerivatives = computeGradient(aState, generateStateCoordinates);

                return (currentDerivatives - backwardDerivatives) / aStepSize;
            };
            break;

        case FiniteDifferenceSolver::Type::Central:
            computeBlock = [&generateStateCoordinates, &stateBuilder, &aState, this](
                               const Real& aStepSize, const Index& anIndex
                           ) -> VectorXd
            {
                VectorXd perturbedCoordinates = aState.accessCoordinates();

                perturbedCoordinates(anIndex) += aStepSize;
                const VectorXd forwardDerivatives = computeGradient(
                    stateBuilder.build(aState.accessInstant(), perturbedCoordinates), generateStateCoordinates
                );

                perturbedCoordinates(anIndex) -= 2.0 * aStepSize;
                const VectorXd backwardDerivatives = computeGradient(
                    stateBuilder.build(aState.accessInstant(), perturbedCoordinates), generateStateCoordinates
                );

                return (forwardDerivatives - backwardDerivatives) / (2.0 * aStepSize);
            };
            break;

        default:
            throw ostk::core::error::runtime::Wrong("Finite Difference Solver Type.");
    }

    for (Index i = 0; i < stateVectorDimension; ++i)
    {
        const Real stepSize = (aState.accessCoordinates()(i) * stepPercentage_ != 0.0)
                                ? aState.accessCoordinates()(i) * stepPercentage_
                                : stepPercentage_;

        jacobian.col(i) = computeBlock(stepSize, i);
    }

    return jacobian;
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

FiniteDifferenceSolver FiniteDifferenceSolver::Default()
{
    return {
        FiniteDifferenceSolver::Type::Central,
        1e-3,
        Duration::Seconds(1e-6),
    };
}

}  // namespace solver
}  // namespace astrodynamics
}  // namespace ostk
