/// Apache License 2.0

#include <random>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/RANSAC.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{
namespace observationfilter
{

using ostk::mathematics::object::MatrixXd;

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;

RANSAC::Analysis::Analysis(
    const VectorXi& anInlierMask,
    const Size& anIterationCount,
    const bool isEarlyTermination,
    const Size& aBestInlierCount,
    const Size& aSeedFailureCount
)
    : ObservationFilter::Analysis(anInlierMask),
      iterationCount(anIterationCount),
      earlyTermination(isEarlyTermination),
      bestInlierCount(aBestInlierCount),
      seedFailureCount(aSeedFailureCount)
{
}

void RANSAC::Analysis::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "RANSAC Observation Filter Analysis");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Inlier count: " << getInlierCount();
    ostk::core::utils::Print::Line(anOutputStream) << "Outlier count: " << getOutlierCount();
    ostk::core::utils::Print::Line(anOutputStream) << "Iteration count: " << iterationCount;
    ostk::core::utils::Print::Line(anOutputStream) << "Early termination: " << (earlyTermination ? "true" : "false");
    ostk::core::utils::Print::Line(anOutputStream) << "Best inlier count: " << bestInlierCount;
    ostk::core::utils::Print::Line(anOutputStream) << "Seed failure count: " << seedFailureCount;

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

RANSAC::RANSAC(
    const Propagator& aPropagator,
    const Real& anInlierThreshold,
    const Size& aMaxIterationCount,
    const Real& aMinInlierRatio,
    const Size& aRandomSeed
)
    : propagator_(aPropagator),
      inlierThreshold_(anInlierThreshold),
      maxIterationCount_(aMaxIterationCount),
      minInlierRatio_(aMinInlierRatio),
      randomSeed_(aRandomSeed)
{
    if (!inlierThreshold_.isDefined() || inlierThreshold_ <= 0.0)
    {
        throw ostk::core::error::RuntimeError("Inlier threshold must be strictly positive.");
    }

    if (maxIterationCount_ == 0)
    {
        throw ostk::core::error::RuntimeError("Max iteration count must be strictly positive.");
    }

    if (!minInlierRatio_.isDefined())
    {
        throw ostk::core::error::RuntimeError("Min inlier ratio must be defined.");
    }
}

RANSAC::RANSAC(
    const Environment& anEnvironment,
    const NumericalSolver& aNumericalSolver,
    const Real& anInlierThreshold,
    const Size& aMaxIterationCount,
    const Real& aMinInlierRatio,
    const Size& aRandomSeed
)
    : RANSAC(
          Propagator::FromEnvironment(aNumericalSolver, anEnvironment),
          anInlierThreshold,
          aMaxIterationCount,
          aMinInlierRatio,
          aRandomSeed
      )
{
}

const Propagator& RANSAC::accessPropagator() const
{
    return propagator_;
}

Real RANSAC::getInlierThreshold() const
{
    return inlierThreshold_;
}

Size RANSAC::getMaxIterationCount() const
{
    return maxIterationCount_;
}

Real RANSAC::getMinInlierRatio() const
{
    return minInlierRatio_;
}

Size RANSAC::getRandomSeed() const
{
    return randomSeed_;
}

Shared<const ObservationFilter::Analysis> RANSAC::filter(const Array<State>& anObservationStateArray) const
{
    const Size observationCount = anObservationStateArray.getSize();

    if (observationCount == 0)
    {
        return std::make_shared<const RANSAC::Analysis>(VectorXi(), Size(0), false, Size(0), Size(0));
    }

    const auto positionSubsetSPtr = CartesianPosition::Default();

    // Cache instants and observed positions in a 3xN Eigen matrix for vectorized residuals.
    Array<Instant> observationInstants = Array<Instant>::Empty();
    observationInstants.reserve(observationCount);

    MatrixXd observationPositions(3, observationCount);

    for (Size i = 0; i < observationCount; ++i)
    {
        const State& observation = anObservationStateArray[i];
        observationInstants.add(observation.accessInstant());

        if (!observation.hasSubset(positionSubsetSPtr))
        {
            throw ostk::core::error::RuntimeError(
                "Observations passed to RANSAC must contain a Cartesian position subset."
            );
        }

        observationPositions.col(static_cast<Eigen::Index>(i)) = observation.extractCoordinate(positionSubsetSPtr);
    }

    // RNG initialization
    std::mt19937 rng;
    if (randomSeed_ == 0)
    {
        std::random_device device;
        rng.seed(device());
    }
    else
    {
        rng.seed(static_cast<std::mt19937::result_type>(randomSeed_));
    }
    std::uniform_int_distribution<Size> indexDistribution(0, observationCount - 1);

    const Size earlyTerminationCount =
        static_cast<Size>(std::ceil(static_cast<double>(observationCount) * static_cast<double>(minInlierRatio_)));

    const Eigen::Index observationCountIdx = static_cast<Eigen::Index>(observationCount);

    VectorXi bestInlierMask = VectorXi::Zero(observationCountIdx);
    Size bestInlierCount = 0;
    Size seedFailureCount = 0;
    Size iterationsRun = 0;
    bool earlyTerminated = false;

    const double thresholdSquared = static_cast<double>(inlierThreshold_) * static_cast<double>(inlierThreshold_);

    for (Size iteration = 0; iteration < maxIterationCount_; ++iteration)
    {
        ++iterationsRun;

        const Size seedIndex = indexDistribution(rng);
        const State& seedObservation = anObservationStateArray[seedIndex];

        Array<State> propagatedStates = Array<State>::Empty();
        try
        {
            propagatedStates = propagator_.calculateStatesAt(seedObservation, observationInstants);
        }
        catch (const std::exception&)
        {
            // A seed that fails to propagate (e.g. unphysical state) is just skipped.
            ++seedFailureCount;
            continue;
        }

        if (propagatedStates.getSize() != observationCount)
        {
            ++seedFailureCount;
            continue;
        }

        // Pack propagated positions into a 3xN matrix and compute per-observation squared residuals
        // as a single vectorized expression.
        MatrixXd propagatedPositions(3, observationCount);
        for (Size i = 0; i < observationCount; ++i)
        {
            propagatedPositions.col(static_cast<Eigen::Index>(i)) =
                propagatedStates[i].extractCoordinate(positionSubsetSPtr);
        }

        const Eigen::VectorXd squaredResiduals =
            (propagatedPositions - observationPositions).colwise().squaredNorm().transpose();

        const VectorXi iterationInlierMask = (squaredResiduals.array() <= thresholdSquared).cast<int>().matrix();
        const Size iterationInlierCount = static_cast<Size>(iterationInlierMask.sum());

        if (iterationInlierCount > bestInlierCount)
        {
            bestInlierCount = iterationInlierCount;
            bestInlierMask = iterationInlierMask;

            if (bestInlierCount >= earlyTerminationCount)
            {
                earlyTerminated = true;
                break;
            }
        }
    }

    return std::make_shared<const RANSAC::Analysis>(
        bestInlierMask, iterationsRun, earlyTerminated, bestInlierCount, seedFailureCount
    );
}

}  // namespace observationfilter
}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk
