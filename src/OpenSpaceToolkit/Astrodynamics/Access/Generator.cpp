/// Apache License 2.0

#include <nlopt.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Segment.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solvers/TemporalConditionSolver.hpp>

using ostk::mathematics::geometry::d3::object::Point;
using ostk::mathematics::geometry::d3::object::Segment;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::celestial::Earth;
using ostk::astro::solvers::TemporalConditionSolver;

namespace ostk
{
namespace astro
{
namespace access
{

Generator::Generator(const Environment& anEnvironment, const Duration& aStep, const Duration& aTolerance)
    : environment_(anEnvironment),
      step_(aStep),
      tolerance_(aTolerance),
      aerFilter_({}),
      accessFilter_({}),
      stateFilter_({})
{
}

Generator::Generator(
    const Environment& anEnvironment,
    const std::function<bool(const AER&)>& anAerFilter,
    const std::function<bool(const Access&)>& anAccessFilter,
    const std::function<bool(const State&, const State&)>& aStateFilter,
    const Duration& aStep,
    const Duration& aTolerance
)
    : environment_(anEnvironment),
      step_(aStep),
      tolerance_(aTolerance),
      aerFilter_(anAerFilter),
      accessFilter_(anAccessFilter),
      stateFilter_(aStateFilter)
{
}

bool Generator::isDefined() const
{
    return this->environment_.isDefined() && this->step_.isDefined() && this->tolerance_.isDefined();
}

Duration Generator::getStep() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->step_;
}

Duration Generator::getTolerance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->tolerance_;
}

std::function<bool(const AER&)> Generator::getAerFilter() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->aerFilter_;
}

std::function<bool(const Access&)> Generator::getAccessFilter() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->accessFilter_;
}

std::function<bool(const State&, const State&)> Generator::getStateFilter() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    return this->stateFilter_;
}

std::function<bool(const Instant&)> Generator::getConditionFunction(
    const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
) const
{
    if (!aFromTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("From Trajectory");
    }

    if (!aToTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("To Trajectory");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator");
    }

    GeneratorContext generatorContext = GeneratorContext(aFromTrajectory, aToTrajectory, environment_, *this);

    return [generatorContext](const Instant& anInstant) mutable -> bool
    {
        return generatorContext.isAccessActive(anInstant);
    };
}

Array<Access> Generator::computeAccesses(
    const physics::time::Interval& anInterval, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
) const
{
    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

    const TemporalConditionSolver temporalConditionSolver = {this->step_, this->tolerance_};

    const Array<physics::time::Interval> accessIntervals =
        temporalConditionSolver.solve(this->getConditionFunction(aFromTrajectory, aToTrajectory), anInterval);

    const Shared<const Celestial> earthSPtr = this->environment_.accessCelestialObjectWithName("Earth");

    return accessIntervals
        .map<Access>(
            [&anInterval, &aFromTrajectory, &aToTrajectory, &earthSPtr, this](
                const physics::time::Interval& anAccessInterval
            ) -> Access
            {
                return Generator::GenerateAccess(
                    anAccessInterval, anInterval, aFromTrajectory, aToTrajectory, earthSPtr, this->tolerance_
                );
            }
        )
        .getWhere(
            [this](const Access& anAccess) -> bool
            {
                return this->accessFilter_ ? this->accessFilter_(anAccess) : true;
            }
        );
}

void Generator::setStep(const Duration& aStep)
{
    if (!aStep.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Step");
    }

    this->step_ = aStep;
}

void Generator::setTolerance(const Duration& aTolerance)
{
    if (!aTolerance.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tolerance");
    }

    this->tolerance_ = aTolerance;
}

void Generator::setAerFilter(const std::function<bool(const AER&)>& anAerFilter)
{
    this->aerFilter_ = anAerFilter;
}

void Generator::setAccessFilter(const std::function<bool(const Access&)>& anAccessFilter)
{
    this->accessFilter_ = anAccessFilter;
}

void Generator::setStateFilter(const std::function<bool(const State&, const State&)>& aStateFilter)
{
    this->stateFilter_ = aStateFilter;
}

Generator Generator::Undefined()
{
    return {Environment::Undefined()};
}

Generator Generator::AerRanges(
    const Interval<Real>& anAzimuthRange,
    const Interval<Real>& anElevationRange,
    const Interval<Real>& aRangeRange,
    const Environment& anEnvironment
)
{
    using ostk::core::type::Real;

    const Interval<Real> azimuthRange_deg = anAzimuthRange;
    const Interval<Real> elevationRange_deg = anElevationRange;
    const Interval<Real> rangeRange_m = aRangeRange;

    // const Interval<Real> azimuthRange_deg = anAzimuthRange ; // anAzimuthRange.isDefined() ?
    // Interval<Real>(anAzimuthRange.accessLowerBound().inDegrees(0.0, +360.0),
    // anAzimuthRange.accessUpperBound().inDegrees(0.0, +360.0), anAzimuthRange.getType()) : Interval<Real>::Undefined()
    // ; const Interval<Real> elevationRange_deg = anElevationRange ; // anElevationRange.isDefined() ?
    // Interval<Real>(anElevationRange.accessLowerBound().inDegrees(-180.0, +180.0),
    // anElevationRange.accessUpperBound().inDegrees(-180.0, +180.0), anElevationRange.getType()) :
    // Interval<Real>::Undefined() ; const Interval<Real> rangeRange_m = aRangeRange ; // aRangeRange.isDefined() ?
    // Interval<Real>(aRangeRange.accessLowerBound().inMeters(), aRangeRange.accessUpperBound().inMeters(),
    // aRangeRange.getType()) : Interval<Real>::Undefined() ;

    const std::function<bool(const AER&)> aerFilter =
        [azimuthRange_deg, elevationRange_deg, rangeRange_m](const AER& anAER) -> bool
    {
        return ((!azimuthRange_deg.isDefined()) || azimuthRange_deg.contains(anAER.getAzimuth().inDegrees(0.0, +360.0))
               ) &&
               ((!elevationRange_deg.isDefined()) ||
                elevationRange_deg.contains(anAER.getElevation().inDegrees(-180.0, +180.0))) &&
               ((!rangeRange_m.isDefined()) || rangeRange_m.contains(anAER.getRange().inMeters()));
    };

    return {anEnvironment, aerFilter};
}

Generator Generator::AerMask(
    const Map<Real, Real>& anAzimuthElevationMask, const Interval<Real>& aRangeRange, const Environment& anEnvironment
)
{
    using ostk::core::container::Map;
    using ostk::core::type::Real;

    using ostk::mathematics::object::Vector2d;

    if ((anAzimuthElevationMask.empty()) || (anAzimuthElevationMask.begin()->first < 0.0) ||
        (anAzimuthElevationMask.rbegin()->first > 360.0))
    {
        throw ostk::core::error::runtime::Wrong("Azimuth-Elevation Mask");
    }

    for (const auto& azimuthElevationPair : anAzimuthElevationMask)
    {
        if ((azimuthElevationPair.second).abs() > 90.0)
        {
            throw ostk::core::error::runtime::Wrong("Azimuth-Elevation Mask");
        }
    }

    Map<Real, Real> anAzimuthElevationMask_deg = anAzimuthElevationMask;
    const Interval<Real> rangeRange_m = aRangeRange;

    if (anAzimuthElevationMask_deg.begin()->first != 0.0)
    {
        anAzimuthElevationMask_deg.insert({0.0, anAzimuthElevationMask_deg.begin()->second});
    }

    if (anAzimuthElevationMask_deg.rbegin()->first != 360.0)
    {
        anAzimuthElevationMask_deg.insert({360.0, anAzimuthElevationMask_deg.begin()->second});
    }

    const std::function<bool(const AER&)> aerFilter = [anAzimuthElevationMask_deg,
                                                       rangeRange_m](const AER& anAER) -> bool
    {
        const Real azimuth = anAER.getAzimuth().inDegrees(0.0, +360.0);
        const Real elevation = anAER.getElevation().inDegrees(-180.0, +180.0);

        auto itLow = anAzimuthElevationMask_deg.lower_bound(azimuth);
        itLow--;
        auto itUp = anAzimuthElevationMask_deg.upper_bound(azimuth);

        // Vector between the two successive mask data points with bounding azimuth values

        const Vector2d lowToUpVector = {itUp->first - itLow->first, itUp->second - itLow->second};

        // Vector from data point with azimuth lower bound to tested point

        const Vector2d lowToPointVector = {azimuth - itLow->first, elevation - itLow->second};

        // If the determinant of these two vectors is positive, the tested point lies above the function defined by the
        // mask

        return (lowToUpVector[0] * lowToPointVector[1] - lowToUpVector[1] * lowToPointVector[0] >= 0.0) &&
               ((!rangeRange_m.isDefined()) || rangeRange_m.contains(anAER.getRange().inMeters()));
    };

    return {anEnvironment, aerFilter};
}

Access Generator::GenerateAccess(
    const physics::time::Interval& anAccessInterval,
    const physics::time::Interval& aGlobalInterval,
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Shared<const Celestial> anEarthSPtr,
    const Duration& aTolerance
)
{
    const Access::Type type = ((aGlobalInterval.accessStart() != anAccessInterval.accessStart()) &&
                               (aGlobalInterval.accessEnd() != anAccessInterval.accessEnd()))
                                ? Access::Type::Complete
                                : Access::Type::Partial;

    const Instant acquisitionOfSignal = anAccessInterval.getStart();
    const Instant timeOfClosestApproach =
        Generator::FindTimeOfClosestApproach(anAccessInterval, aFromTrajectory, aToTrajectory, aTolerance);
    const Instant lossOfSignal = anAccessInterval.getEnd();

    const Angle maxElevation =
        timeOfClosestApproach.isDefined()
            ? Generator::CalculateElevationAt(timeOfClosestApproach, aFromTrajectory, aToTrajectory, anEarthSPtr)
            : Angle::Undefined();

    return Access {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};
}

Instant Generator::FindTimeOfClosestApproach(
    const physics::time::Interval& anAccessInterval,
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Duration& aTolerance
)
{
    struct Context
    {
        const Instant& startInstant;
        const std::function<Pair<State, State>(const Instant& anInstant)>& getStatesAt;
        const std::function<Pair<Position, Position>(const State& aFromState, const State& aToState)>&
            getPositionsFromStates;
    };

    const auto calculateRange = [](const std::vector<double>& x, std::vector<double>& aGradient, void* aDataContext
                                ) -> double
    {
        (void)aGradient;

        if (aDataContext == nullptr)
        {
            throw ostk::core::error::runtime::Wrong("Data context");
        }

        const Context* contextPtr = static_cast<const Context*>(aDataContext);

        const Instant queryInstant = contextPtr->startInstant + Duration::Seconds(x[0]);

        const auto [queryFromState, queryToState] = contextPtr->getStatesAt(queryInstant);
        const auto [queryFromPosition, queryToPosition] =
            contextPtr->getPositionsFromStates(queryFromState, queryToState);

        const Real squaredRange_m =
            (queryToPosition.accessCoordinates() - queryFromPosition.accessCoordinates()).squaredNorm();

        return squaredRange_m;
    };

    Context context = {
        anAccessInterval.getStart(),
        [&aFromTrajectory, &aToTrajectory](const Instant& anInstant) -> Pair<State, State>
        {
            return GeneratorContext::GetStatesAt(anInstant, aFromTrajectory, aToTrajectory);
        },
        GeneratorContext::GetPositionsFromStates
    };

    nlopt::opt optimizer = {nlopt::LN_COBYLA, 1};

    const std::vector<double> lowerBound = {0.0};
    const std::vector<double> upperBound = {anAccessInterval.getDuration().inSeconds()};

    optimizer.set_lower_bounds(lowerBound);
    optimizer.set_upper_bounds(upperBound);

    optimizer.set_min_objective(calculateRange, &context);

    optimizer.set_xtol_rel(aTolerance.inSeconds());

    std::vector<double> x = {0.0};

    try
    {
        double minimumSquaredRange;

        nlopt::result result = optimizer.optimize(x, minimumSquaredRange);

        switch (result)
        {
            case nlopt::STOPVAL_REACHED:
            case nlopt::FTOL_REACHED:
            case nlopt::XTOL_REACHED:
            case nlopt::MAXEVAL_REACHED:
            case nlopt::MAXTIME_REACHED:
                return anAccessInterval.getStart() + Duration::Seconds(x[0]);

            case nlopt::FAILURE:
            case nlopt::INVALID_ARGS:
            case nlopt::OUT_OF_MEMORY:
            case nlopt::ROUNDOFF_LIMITED:
            case nlopt::FORCED_STOP:
            default:
                throw ostk::core::error::RuntimeError("Cannot find TCA (solution did not converge).");
        }
    }
    catch (const std::exception& anException)
    {
        throw ostk::core::error::RuntimeError("Cannot find TCA (algorithm failed): [{}].", anException.what());
    }
}

Angle Generator::CalculateElevationAt(
    const Instant& anInstant,
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Shared<const Celestial> anEarthSPtr
)
{
    const auto [fromState, toState] = GeneratorContext::GetStatesAt(anInstant, aFromTrajectory, aToTrajectory);
    const auto [fromPosition, toPosition] = GeneratorContext::GetPositionsFromStates(fromState, toState);

    const AER aer = GeneratorContext::CalculateAer(anInstant, fromPosition, toPosition, anEarthSPtr);

    return aer.getElevation();
}

GeneratorContext::GeneratorContext(
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Environment& anEnvironment,
    const Generator& aGenerator
)
    : fromTrajectory_(aFromTrajectory),
      toTrajectory_(aToTrajectory),
      environment_(anEnvironment),
      earthSPtr_(environment_.accessCelestialObjectWithName("Earth")),  // [TBR] This is Earth specific
      generator_(aGenerator)
{
}

bool GeneratorContext::isAccessActive(const Instant& anInstant)
{
    this->environment_.setInstant(anInstant);

    const auto [fromState, toState] =
        GeneratorContext::GetStatesAt(anInstant, this->fromTrajectory_, this->toTrajectory_);

    if (this->generator_.getStateFilter() && (!this->generator_.getStateFilter()(fromState, toState)))
    {
        return false;
    }

    const auto [fromPosition, toPosition] = GeneratorContext::GetPositionsFromStates(fromState, toState);

    // Line of sight

    static const Shared<const Frame> commonFrameSPtr = Frame::GCRF();

    const Point fromPositionCoordinates = Point::Vector(fromPosition.accessCoordinates());
    const Point toPositionCoordinates = Point::Vector(toPosition.accessCoordinates());

    if (fromPositionCoordinates != toPositionCoordinates)
    {
        const Segment fromToSegment = {fromPositionCoordinates, toPositionCoordinates};

        const Object::Geometry fromToSegmentGeometry = {fromToSegment, commonFrameSPtr};

        const bool lineOfSight = !this->environment_.intersects(fromToSegmentGeometry);

        if (!lineOfSight)
        {
            return false;
        }
    }

    // AER filtering

    if (this->generator_.getAerFilter())
    {
        const AER aer = GeneratorContext::CalculateAer(anInstant, fromPosition, toPosition, this->earthSPtr_);

        if (!this->generator_.getAerFilter()(aer))
        {
            return false;
        }
    }

    return true;
}

Pair<State, State> GeneratorContext::GetStatesAt(
    const Instant& anInstant, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
)
{
    const State fromState = aFromTrajectory.getStateAt(anInstant);
    const State toState = aToTrajectory.getStateAt(anInstant);

    return {fromState, toState};
}

Pair<Position, Position> GeneratorContext::GetPositionsFromStates(const State& aFromState, const State& aToState)
{
    if (aFromState.accessInstant() != aToState.accessInstant())
    {
        throw ostk::core::error::RuntimeError("Cannot get positions from states at different instants.");
    }

    static const Shared<const Frame> commonFrameSPtr = Frame::GCRF();

    const Position fromPosition = aFromState.getPosition().inFrame(commonFrameSPtr, aFromState.accessInstant());
    const Position toPosition = aToState.getPosition().inFrame(commonFrameSPtr, aFromState.accessInstant());

    return {fromPosition, toPosition};
}

AER GeneratorContext::CalculateAer(
    const Instant& anInstant,
    const Position& aFromPosition,
    const Position& aToPosition,
    const Shared<const Celestial> anEarthSPtr
)
{
    // [TBM] This logic is Earth-specific
    const Point referencePoint_ITRF =
        Point::Vector(aFromPosition.inFrame(Frame::ITRF(), anInstant).accessCoordinates());

    const LLA referencePoint_LLA = LLA::Cartesian(
        referencePoint_ITRF.asVector(), anEarthSPtr->getEquatorialRadius(), anEarthSPtr->getFlattening()
    );

    const Shared<const Frame> nedFrameSPtr = anEarthSPtr->getFrameAt(referencePoint_LLA, Earth::FrameType::NED);

    const Position fromPosition_NED = aFromPosition.inFrame(nedFrameSPtr, anInstant);
    const Position toPosition_NED = aToPosition.inFrame(nedFrameSPtr, anInstant);

    return AER::FromPositionToPosition(fromPosition_NED, toPosition_NED, true);
}

}  // namespace access
}  // namespace astro
}  // namespace ostk
