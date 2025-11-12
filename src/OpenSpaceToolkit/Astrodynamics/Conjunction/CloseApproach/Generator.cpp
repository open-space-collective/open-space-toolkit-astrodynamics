/// Apache License 2.0

#include <algorithm>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace closeapproach
{

using ostk::core::container::Array;

using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

// Define static state builder for relative distance derivative computation
static const Shared<const Frame> relativeDistanceDerivativeFrame = Frame::GCRF();
static const Array<Shared<const CoordinateSubset>> relativeStateCoordinateSubsets = {
    CartesianPosition::Default(),
    CartesianVelocity::Default(),
};
static const StateBuilder relativeDistanceDerivativeStateBuilder =
    StateBuilder(relativeDistanceDerivativeFrame, relativeStateCoordinateSubsets);

Generator::Generator(const Trajectory& aReferenceTrajectory, const Duration& aStep, const Duration& aTolerance)
    : referenceTrajectory_(aReferenceTrajectory),
      step_(aStep),
      tolerance_(aTolerance)
{
    this->setStep(aStep);
    this->setTolerance(aTolerance);
}

bool Generator::isDefined() const
{
    return referenceTrajectory_.isDefined() && step_.isDefined() && tolerance_.isDefined();
}

Trajectory Generator::getReferenceTrajectory() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return referenceTrajectory_;
}

Duration Generator::getStep() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return step_;
}

Duration Generator::getTolerance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return tolerance_;
}

Array<CloseApproach> Generator::computeCloseApproaches(const Trajectory& aTrajectory, const Interval& aSearchInterval)
    const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    if (!aTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Other object trajectory");
    }

    if (!aSearchInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Search interval");
    }

    const TemporalConditionSolver temporalConditionSolver = TemporalConditionSolver(step_, tolerance_);

    // Define condition: objects are moving apart (relative distance derivative > 0)
    const auto objectsAreMovingApart = [this, &aTrajectory](const Instant& anInstant) -> bool
    {
        const State state1 = referenceTrajectory_.getStateAt(anInstant);
        const State state2 = aTrajectory.getStateAt(anInstant);
        return Generator::ComputeRelativeDistanceDerivative_(state1, state2) > 0.0;
    };

    // Solve for intervals where objects are moving apart
    const Array<Interval> intervals = temporalConditionSolver.solve(objectsAreMovingApart, aSearchInterval);

    // Build close approaches from intervals
    Array<CloseApproach> closeApproaches = Array<CloseApproach>::Empty();

    for (const auto& interval : intervals)
    {
        // Skip if objects are moving apart from the beginning of the search interval
        if (interval.getStart() == aSearchInterval.getStart())
        {
            continue;
        }

        // Create close approach at the start of the interval (when objects transition from approaching to moving
        // apart)
        const Instant closeApproachInstant = interval.getStart();
        const State object1State = referenceTrajectory_.getStateAt(closeApproachInstant);
        const State object2State = aTrajectory.getStateAt(closeApproachInstant);

        closeApproaches.add(CloseApproach(object1State, object2State));
    }

    std::sort(
        closeApproaches.begin(),
        closeApproaches.end(),
        [](const CloseApproach& a, const CloseApproach& b)
        {
            return a.getInstant() < b.getInstant();
        }
    );

    return closeApproaches;
}

void Generator::setStep(const Duration& aStep)
{
    if (!aStep.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Step");
    }

    if (!aStep.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Step");
    }

    step_ = aStep;
}

void Generator::setTolerance(const Duration& aTolerance)
{
    if (!aTolerance.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tolerance");
    }

    if (!aTolerance.isStrictlyPositive())
    {
        throw ostk::core::error::runtime::Wrong("Tolerance");
    }

    tolerance_ = aTolerance;
}

Generator Generator::Undefined()
{
    return Generator(Trajectory::Undefined());
}

Real Generator::ComputeRelativeDistanceDerivative_(const State& aState1, const State& aState2)
{
    if (!aState1.isDefined() || !aState2.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State");
    }

    // Reduce states to position and velocity in GCRF frame
    const State reducedState1InFrame =
        relativeDistanceDerivativeStateBuilder.reduce(aState1.inFrame(relativeDistanceDerivativeFrame));
    const State reducedState2InFrame =
        relativeDistanceDerivativeStateBuilder.reduce(aState2.inFrame(relativeDistanceDerivativeFrame));

    // Compute relative state
    const State relativeStateInFrame = reducedState2InFrame - reducedState1InFrame;

    // Get relative position and velocity coordinates
    const Position relativePosition = relativeStateInFrame.getPosition();
    const Velocity relativeVelocity = relativeStateInFrame.getVelocity();

    const Vector3d relativePositionCoordinates = relativePosition.inMeters().getCoordinates();
    const Vector3d relativeVelocityCoordinates =
        relativeVelocity.inUnit(Velocity::Unit::MeterPerSecond).getCoordinates();

    const Real dotProduct = relativePositionCoordinates.dot(relativeVelocityCoordinates);
    const Real relativePositionNorm = relativePositionCoordinates.norm();

    if (relativePositionNorm == 0.0)
    {
        throw ostk::core::error::RuntimeError("Cannot compute relative distance derivative: states are co-located.");
    }

    // Relative distance derivative: dot(r, v) / ||r||
    return dotProduct / relativePositionNorm;
}

}  // namespace closeapproach
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk
