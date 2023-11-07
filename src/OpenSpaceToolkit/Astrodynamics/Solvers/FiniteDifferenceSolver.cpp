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

MatrixXd FiniteDifferenceSolver::computeJacobian(
    const State& aState,
    const Array<Instant>& anInstantArray,
    const std::function<MatrixXd(const State&, const Array<Instant>&)>& generateStateCoordinates,
    const Size& aCoordinatesDimension
) const
{
    const StateBuilder stateBuilder = {aState};

    const Size stateVectorDimension = aState.getSize();
    const Size numberOfInstants = anInstantArray.getSize();

    std::function<MatrixXd(const Real&, VectorXd, const Index&)> computeBlock;

    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
            computeBlock = [&generateStateCoordinates,
                            &stateBuilder,
                            anInstant = aState.accessInstant(),
                            &anInstantArray](const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex)
            {
                const MatrixXd coordinates =
                    generateStateCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                aCoordinateVector(anIndex) += aStepSize;
                const MatrixXd forwardCoordinates =
                    generateStateCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                const MatrixXd differencedCoordinates = (forwardCoordinates - coordinates) / aStepSize;

                return differencedCoordinates;
            };
            break;

        case FiniteDifferenceSolver::Type::Backward:
            computeBlock = [&generateStateCoordinates,
                            &stateBuilder,
                            anInstant = aState.accessInstant(),
                            &anInstantArray](const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex)
            {
                const MatrixXd coordinates =
                    generateStateCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                aCoordinateVector(anIndex) -= aStepSize;
                const MatrixXd backwardCoordinates =
                    generateStateCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                const MatrixXd differencedCoordinates = (coordinates - backwardCoordinates) / aStepSize;

                return differencedCoordinates;
            };
            break;

        case FiniteDifferenceSolver::Type::Central:
            computeBlock = [&generateStateCoordinates,
                            &stateBuilder,
                            anInstant = aState.accessInstant(),
                            &anInstantArray](const Real& aStepSize, VectorXd aCoordinateVector, const Index& anIndex)
            {
                // Perturb state forward
                aCoordinateVector(anIndex) += aStepSize;
                const MatrixXd forwardCoordinates =
                    generateStateCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                // Perturb state backward
                aCoordinateVector(anIndex) -= 2.0 * aStepSize;
                const MatrixXd backwardCoordinates =
                    generateStateCoordinates(stateBuilder.build(anInstant, aCoordinateVector), anInstantArray);

                const MatrixXd differencedCoordinates = (forwardCoordinates - backwardCoordinates) / (2.0 * aStepSize);

                return differencedCoordinates;
            };
            break;

        default:
            throw ostk::core::error::runtime::Wrong("Finite Difference Solver Type.");
    }

    MatrixXd A = MatrixXd::Zero(aCoordinatesDimension * numberOfInstants, stateVectorDimension);

    for (Index i = 0; i < stateVectorDimension; ++i)
    {
        const Real stepSize = (aState.accessCoordinates()(i) * stepPercentage_ != 0.0)
                                ? aState.accessCoordinates()(i) * stepPercentage_
                                : stepPercentage_;

        MatrixXd differencedCoordinates = computeBlock(stepSize, aState.getCoordinates(), i);

        const VectorXd columnStackedCoordinates =
            Eigen::Map<VectorXd>(differencedCoordinates.data(), differencedCoordinates.size());

        A.col(i) = columnStackedCoordinates;
    }

    return A;
}

MatrixXd FiniteDifferenceSolver::computeJacobian(
    const State& aState,
    const Instant& anInstant,
    const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates,
    const Size& aCoordinatesDimension

) const
{
    const auto generateStatesCoordinates =
        [&generateStateCoordinates](const State& anInputState, const Array<Instant>& anInstantArray) -> MatrixXd
    {
        const VectorXd coordinates = generateStateCoordinates(anInputState, anInstantArray[0]);

        const MatrixXd coordinatesMatrix = {coordinates};

        return coordinatesMatrix;
    };

    return computeJacobian(aState, {anInstant}, generateStatesCoordinates, aCoordinatesDimension);
}

VectorXd FiniteDifferenceSolver::computeGradient(
    const State& aState, const std::function<VectorXd(const State&, const Instant&)>& generateStateCoordinates
) const
{
    switch (type_)
    {
        case FiniteDifferenceSolver::Type::Forward:
        {
            const VectorXd coordinates = generateStateCoordinates(aState, aState.accessInstant());

            const Instant instant = aState.accessInstant() + stepDuration_;
            const VectorXd forwardCoordinates = generateStateCoordinates(aState, instant);

            const VectorXd differencedCoordinates = (forwardCoordinates - coordinates) / (stepDuration_.inSeconds());

            return differencedCoordinates;
        }

        case FiniteDifferenceSolver::Type::Backward:
        {
            const VectorXd coordinates = generateStateCoordinates(aState, aState.accessInstant());

            const Instant instant = aState.accessInstant() - stepDuration_;
            const VectorXd backwardCoordinates = generateStateCoordinates(aState, instant);

            const VectorXd differencedCoordinates = (coordinates - backwardCoordinates) / (stepDuration_.inSeconds());

            return differencedCoordinates;
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

}  // namespace solvers
}  // namespace astro
}  // namespace ostk
