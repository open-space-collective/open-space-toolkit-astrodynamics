/// Apache License 2.0

#include <nlopt.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Segment.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>                   // [TBR]
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>  // [TBR]

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solvers/TemporalConditionSolver.hpp>

using ostk::core::ctnr::Pair;
using ostk::core::types::Real;
using ostk::core::types::Shared;

using ostk::math::geom::d3::objects::Point;
using ostk::math::geom::d3::objects::Segment;

using ostk::physics::coord::Frame;

using ostk::physics::coord::spherical::LLA;  // [TBR]
using ostk::physics::env::Object;
using ostk::physics::env::obj::celest::Earth;  // [TBR]
using ostk::astro::trajectory::State;
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
    return environment_.isDefined() && step_.isDefined() && tolerance_.isDefined();
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

Array<Access> Generator::computeAccesses(
    const physics::time::Interval& anInterval, const Trajectory& aFromTrajectory, const Trajectory& aToTrajectory
) const
{
    // The following code is a first attempt at calculating precise accesses.
    // It should be further optimized for speed.
    //
    // Potentially useful resources:
    //
    // - [PREDICT](https://www.qsl.net/kd2bd/predict.html)
    // - [Gpredict](https://github.com/csete/gpredict)
    // - [Rapid Determination of Satellite Visibility Periods](http://www.dtic.mil/dtic/tr/fulltext/u2/a267281.pdf)
    // - [A Fast Prediction Algorithm of Satellite
    // Passes](https://pdfs.semanticscholar.org/6088/1d27c9bf44c59e663b359223e8ed0a91140b.pdf)
    // - [Visual Contact between Two Earth’s Satellites](http://thescipub.com/pdf/10.3844/ajassp.2012.620.623)
    // - [Rapid Satellite-to-Site Visibility Determination Based on Self-Adaptive Interpolation
    // Technique](https://arxiv.org/pdf/1611.02402.pdf)

    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval");
    }

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

    GeneratorContext generatorContext =
        GeneratorContext(anInterval, aFromTrajectory, aToTrajectory, environment_, *this);

    const auto isAccessSelected = [this](const Access& anAccess) -> bool
    {
        return this->accessFilter_ ? this->accessFilter_(anAccess) : true;
    };

    const TemporalConditionSolver temporalConditionSolver = {this->step_, this->tolerance_};

    const Array<physics::time::Interval> accessIntervals = temporalConditionSolver.solve(
        std::bind(&GeneratorContext::isAccessActiveAt, generatorContext, std::placeholders::_1), anInterval
    );

    return accessIntervals
        .map<Access>(std::bind(&GeneratorContext::generateAccess, generatorContext, std::placeholders::_1))
        .getWhere(isAccessSelected);
}

void Generator::setStep(const Duration& aStep)
{
    if (!aStep.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Step");
    }

    step_ = aStep;
}

void Generator::setTolerance(const Duration& aTolerance)
{
    if (!aTolerance.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tolerance");
    }

    tolerance_ = aTolerance;
}

void Generator::setAerFilter(const std::function<bool(const AER&)>& anAerFilter)
{
    aerFilter_ = anAerFilter;
}

void Generator::setAccessFilter(const std::function<bool(const Access&)>& anAccessFilter)
{
    accessFilter_ = anAccessFilter;
}

void Generator::setStateFilter(const std::function<bool(const State&, const State&)>& aStateFilter)
{
    stateFilter_ = aStateFilter;
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
    using ostk::core::types::Real;

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
    using ostk::core::ctnr::Map;
    using ostk::core::types::Real;

    using ostk::math::obj::Vector2d;

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

GeneratorContext::GeneratorContext(
    const physics::time::Interval& anInterval,
    const Trajectory& aFromTrajectory,
    const Trajectory& aToTrajectory,
    const Environment& anEnvironment,
    const Generator& aGenerator
)
    : interval_(anInterval),
      fromTrajectory_(aFromTrajectory),
      toTrajectory_(aToTrajectory),
      environment_(anEnvironment),
      generator_(aGenerator),
      earthSPtr_(environment_.accessCelestialObjectWithName("Earth"))  // [TBR] This is Earth specific
{
}

bool GeneratorContext::isAccessActiveAt(const Instant& anInstant)
{
    const auto [fromState, toState] = this->getStatesAt(anInstant);
    return this->isAccessActive(anInstant, fromState, toState);
}

Access GeneratorContext::generateAccess(const physics::time::Interval& anAccessInterval) const
{
    const Access::Type type = ((this->interval_.accessStart() != anAccessInterval.accessStart()) &&
                               (this->interval_.accessEnd() != anAccessInterval.accessEnd()))
                                ? Access::Type::Complete
                                : Access::Type::Partial;

    const Instant acquisitionOfSignal = anAccessInterval.getStart();
    const Instant timeOfClosestApproach = this->findTimeOfClosestApproach(anAccessInterval);
    const Instant lossOfSignal = anAccessInterval.getEnd();

    const Angle maxElevation =
        timeOfClosestApproach.isDefined() ? this->calculateElevationAt(timeOfClosestApproach) : Angle::Undefined();

    return Access {type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation};
}

Pair<State, State> GeneratorContext::getStatesAt(const Instant& anInstant) const
{
    const State fromState = this->fromTrajectory_.getStateAt(anInstant);
    const State toState = this->toTrajectory_.getStateAt(anInstant);

    return {fromState, toState};
}

Pair<Position, Position> GeneratorContext::getPositionsFromStates(const State& aFromState, const State& aToState) const
{
    static const Shared<const Frame> commonFrameSPtr = Frame::GCRF();

    if (aFromState.accessInstant() != aToState.accessInstant())
    {
        // TBI: Throw
    }

    const Position fromPosition = aFromState.getPosition().inFrame(commonFrameSPtr, aFromState.accessInstant());
    const Position toPosition = aToState.getPosition().inFrame(commonFrameSPtr, aFromState.accessInstant());

    return {fromPosition, toPosition};
}

AER GeneratorContext::calculateAer(const Instant& anInstant, const Position& aFromPosition, const Position& aToPosition)
    const
{
    const Point referencePoint_ITRF = Point::Vector(aFromPosition.inFrame(Frame::ITRF(), anInstant).accessCoordinates()
    );  // [TBR] This is Earth specific

    const LLA referencePoint_LLA = LLA::Cartesian(
        referencePoint_ITRF.asVector(), this->earthSPtr_->getEquatorialRadius(), this->earthSPtr_->getFlattening()
    );

    const Shared<const Frame> nedFrameSPtr =
        this->earthSPtr_->getFrameAt(referencePoint_LLA, Earth::FrameType::NED);  // [TBR] This is Earth specific

    const Position fromPosition_NED = aFromPosition.inFrame(nedFrameSPtr, anInstant);
    const Position toPosition_NED = aToPosition.inFrame(nedFrameSPtr, anInstant);

    return AER::FromPositionToPosition(fromPosition_NED, toPosition_NED, true);
}

bool GeneratorContext::isAccessActive(const Instant& anInstant, const State& aFromState, const State& aToState)
{
    this->environment_.setInstant(anInstant);

    if (this->generator_.stateFilter_ && (!this->generator_.stateFilter_(aFromState, aToState)))
    {
        return false;
    }

    const auto [fromPosition, toPosition] = this->getPositionsFromStates(aFromState, aToState);

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

    if (this->generator_.aerFilter_)
    {
        const AER aer = this->calculateAer(anInstant, fromPosition, toPosition);

        if (!this->generator_.aerFilter_(aer))
        {
            return false;
        }
    }

    return true;
}

Instant GeneratorContext::findTimeOfClosestApproach(const physics::time::Interval& anAccessInterval) const
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
        std::bind(&GeneratorContext::getStatesAt, this, std::placeholders::_1),
        std::bind(&GeneratorContext::getPositionsFromStates, this, std::placeholders::_1, std::placeholders::_2)};

    nlopt::opt optimizer = {nlopt::LN_COBYLA, 1};

    const std::vector<double> lowerBound = {0.0};
    const std::vector<double> upperBound = {anAccessInterval.getDuration().inSeconds()};

    optimizer.set_lower_bounds(lowerBound);
    optimizer.set_upper_bounds(upperBound);

    optimizer.set_min_objective(calculateRange, &context);

    optimizer.set_xtol_rel(this->generator_.tolerance_.inSeconds());

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

Angle GeneratorContext::calculateElevationAt(const Instant& anInstant) const
{
    const auto [fromState, toState] = this->getStatesAt(anInstant);
    const auto [fromPosition, toPosition] = this->getPositionsFromStates(fromState, toState);

    const AER aer = this->calculateAer(anInstant, fromPosition, toPosition);

    return aer.getElevation();
}

}  // namespace access
}  // namespace astro
}  // namespace ostk
