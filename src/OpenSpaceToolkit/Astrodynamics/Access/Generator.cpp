////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Access/Generator.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Access/Generator.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp> // [TBR]
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp> // [TBR]

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Segment.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>

#include <OpenSpaceToolkit/Core/Containers/Pair.hpp>

#include <nlopt.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace access
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Generator::Generator                        (   const   Environment&                anEnvironment,
                                                                                const   Duration&                   aStep,
                                                                                const   Duration&                   aTolerance                                  )
                                :   environment_(anEnvironment),
                                    step_(aStep),
                                    tolerance_(aTolerance),
                                    aerFilter_({}),
                                    accessFilter_({}),
                                    stateFilter_({})
{

}

                                Generator::Generator                        (   const   Environment&                anEnvironment,
                                                                                const   std::function<bool (const AER&)>& anAerFilter,
                                                                                const   std::function<bool (const Access&)>& anAccessFilter,
                                                                                const   std::function<bool (const State&, const State&)>& aStateFilter,
                                                                                const   Duration&                   aStep,
                                                                                const   Duration&                   aTolerance                                  )
                                :   environment_(anEnvironment),
                                    step_(aStep),
                                    tolerance_(aTolerance),
                                    aerFilter_(anAerFilter),
                                    accessFilter_(anAccessFilter),
                                    stateFilter_(aStateFilter)
{

}

bool                            Generator::isDefined                        ( ) const
{
    return environment_.isDefined() && step_.isDefined() && tolerance_.isDefined() ;
}

Duration                        Generator::getStep                          ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator") ;
    }

    return step_ ;

}

Duration                        Generator::getTolerance                     ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator") ;
    }

    return tolerance_ ;

}

Array<Access>                   Generator::computeAccesses                  (   const   physics::time::Interval&    anInterval,
                                                                                const   Trajectory&                 aFromTrajectory,
                                                                                const   Trajectory&                 aToTrajectory                               ) const
{

    // The following code is a first attempt at calculating precise accesses.
    // It should be further optimized for speed.
    //
    // Potentially useful resources:
    //
    // - [PREDICT](https://www.qsl.net/kd2bd/predict.html)
    // - [Gpredict](https://github.com/csete/gpredict)
    // - [Rapid Determination of Satellite Visibility Periods](http://www.dtic.mil/dtic/tr/fulltext/u2/a267281.pdf)
    // - [A Fast Prediction Algorithm of Satellite Passes](https://pdfs.semanticscholar.org/6088/1d27c9bf44c59e663b359223e8ed0a91140b.pdf)
    // - [Visual Contact between Two Earth’s Satellites](http://thescipub.com/pdf/10.3844/ajassp.2012.620.623)
    // - [Rapid Satellite-to-Site Visibility Determination Based on Self-Adaptive Interpolation Technique](https://arxiv.org/pdf/1611.02402.pdf)

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Pair ;

    using ostk::math::geom::d3::objects::Point ;
    using ostk::math::geom::d3::objects::Segment ;

    using ostk::physics::time::Duration ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::spherical::LLA ; // [TBR]
    using ostk::physics::env::Object ;
    using ostk::physics::env::obj::celest::Earth ; // [TBR]

    using ostk::astro::trajectory::State ;

    if (!anInterval.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Interval") ;
    }

    if (!aFromTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("From Trajectory") ;
    }

    if (!aToTrajectory.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("To Trajectory") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Generator") ;
    }

    Environment environment = environment_ ;

    const auto earthSPtr = environment.accessCelestialObjectWithName("Earth") ; // [TBR] This is Earth specific

    const auto getStatesAt = [&aFromTrajectory, &aToTrajectory] (const Instant& anInstant) -> Pair<State, State>
    {

        const State fromState = aFromTrajectory.getStateAt(anInstant) ;
        const State toState = aToTrajectory.getStateAt(anInstant) ;

        return { fromState, toState } ;

    } ;

    const auto getPositionsFromStates = [] (const State& aFromState, const State& aToState) -> Pair<Position, Position>
    {

        static const Shared<const Frame> commonFrameSPtr = Frame::GCRF() ;

        if (aFromState.accessInstant() != aToState.accessInstant())
        {
            // TBI: Throw
        }

        const Position fromPosition = aFromState.accessPosition().inFrame(commonFrameSPtr, aFromState.accessInstant()) ;
        const Position toPosition = aToState.accessPosition().inFrame(commonFrameSPtr, aFromState.accessInstant()) ;

        return { fromPosition, toPosition } ;

    } ;

    const auto calculateAer = [&earthSPtr] (const Instant& anInstant, const Position& aFromPosition, const Position& aToPosition) -> AER
    {

        const Point referencePoint_ITRF = Point::Vector(aFromPosition.inFrame(Frame::ITRF(), anInstant).accessCoordinates()) ; // [TBR] This is Earth specific

        const LLA referencePoint_LLA = LLA::Cartesian(referencePoint_ITRF.asVector(), earthSPtr->getEquatorialRadius(), earthSPtr->getFlattening()) ;

        const Shared<const Frame> nedFrameSPtr = earthSPtr->getFrameAt(referencePoint_LLA, Earth::FrameType::NED) ; // [TBR] This is Earth specific

        const Position fromPosition_NED = aFromPosition.inFrame(nedFrameSPtr, anInstant) ;
        const Position toPosition_NED = aToPosition.inFrame(nedFrameSPtr, anInstant) ;

        const AER aer = AER::FromPositionToPosition(fromPosition_NED, toPosition_NED, true) ;

        return aer ;

    } ;

    const auto isAccessActive = [this, &environment, &earthSPtr, getPositionsFromStates, calculateAer] (const Instant& anInstant, const State& aFromState, const State& aToState) -> bool
    {

        environment.setInstant(anInstant) ;

        if (stateFilter_ && (!stateFilter_(aFromState, aToState)))
        {
            return false ;
        }

        const auto [ fromPosition, toPosition ] = getPositionsFromStates(aFromState, aToState) ;

        // Debug

        // {

        //     const Point fromPoint_ITRF = Point::Vector(fromPosition.inFrame(Frame::ITRF(), anInstant).accessCoordinates()) ;
        //     const Point toPoint_ITRF = Point::Vector(toPosition.inFrame(Frame::ITRF(), anInstant).accessCoordinates()) ;

        //     const LLA fromPoint_LLA = LLA::Cartesian(fromPoint_ITRF.asVector(), earthSPtr->getEquatorialRadius(), earthSPtr->getFlattening()) ;
        //     const LLA toPoint_LLA = LLA::Cartesian(toPoint_ITRF.asVector(), earthSPtr->getEquatorialRadius(), earthSPtr->getFlattening()) ;

        //     const AER aer = calculateAer(anInstant, fromPosition, toPosition) ;

        //     std::cout   << anInstant.getDateTime(ostk::physics::time::Scale::UTC).toString(ostk::physics::time::DateTime::Format::ISO8601) << ", "
        //                 << fromPoint_LLA.getLatitude().inDegrees().toString(12) << ", "
        //                 << fromPoint_LLA.getLongitude().inDegrees().toString(12) << ", "
        //                 << fromPoint_LLA.getAltitude().inMeters().toString(12) << ", "
        //                 << toPoint_LLA.getLatitude().inDegrees().toString(12) << ", "
        //                 << toPoint_LLA.getLongitude().inDegrees().toString(12) << ", "
        //                 << toPoint_LLA.getAltitude().inMeters().toString(12) << ", "
        //                 << aer.getAzimuth().inDegrees(-180.0, +180.0) << ", "
        //                 << aer.getElevation().inDegrees(-180.0, +180.0) << std::endl ;

        // }

        // Line of sight

        static const Shared<const Frame> commonFrameSPtr = Frame::GCRF() ;

        const Point fromPositionCoordinates = Point::Vector(fromPosition.accessCoordinates()) ;
        const Point toPositionCoordinates = Point::Vector(toPosition.accessCoordinates()) ;

        if (fromPositionCoordinates != toPositionCoordinates)
        {

            const Segment fromToSegment = { fromPositionCoordinates, toPositionCoordinates } ;

            const Object::Geometry fromToSegmentGeometry = { fromToSegment, commonFrameSPtr } ;

            const bool lineOfSight = !environment.intersects(fromToSegmentGeometry) ;

            if (!lineOfSight)
            {
                return false ;
            }

        }

        // AER filtering

        if (aerFilter_)
        {

            const AER aer = calculateAer(anInstant, fromPosition, toPosition) ;

            if (!aerFilter_(aer))
            {
                return false ;
            }

        }

        return true ;

    } ;

    const auto isAccessActiveAt = [getStatesAt, isAccessActive] (const Instant& aNextInstant) -> bool
    {

        const auto [ fromState, toState ] = getStatesAt(aNextInstant) ;

        const bool accessIsActive = isAccessActive(aNextInstant, fromState, toState) ;

        return accessIsActive ;

    } ;

    const auto elevationAt = [calculateAer, getStatesAt, getPositionsFromStates] (const Instant& anInstant) -> Angle
    {

        const auto [ fromState, toState ] = getStatesAt(anInstant) ;
        const auto [ fromPosition, toPosition ] = getPositionsFromStates(fromState, toState) ;

        const AER aer = calculateAer(anInstant, fromPosition, toPosition) ;

        const Angle elevationAngle = aer.getElevation() ;

        return elevationAngle ;

    } ;

    const std::function<Instant (const Instant&, const Instant&, const Duration&, const bool, const std::function<bool (const Instant&)>&)> findSwitchingInstant =
    [&findSwitchingInstant] (const Instant& aPreviousInstant, const Instant& aNextInstant, const Duration& aTolerance, const bool isConditionActiveAtPreviousInstant, const std::function<bool (const Instant&)>& aCondition) -> Instant
    {

        const Duration step = Duration::Between(aPreviousInstant, aNextInstant) ;

        if (step <= aTolerance)
        {
            return aNextInstant ;
        }

        const Instant midInstant = aPreviousInstant + (step / 2.0) ;

        const bool conditionIsActiveInMidInstant = aCondition(midInstant) ;

        if (isConditionActiveAtPreviousInstant != conditionIsActiveInMidInstant)
        {
            return findSwitchingInstant(aPreviousInstant, midInstant, aTolerance, isConditionActiveAtPreviousInstant, aCondition) ;
        }

        return findSwitchingInstant(midInstant, aNextInstant, aTolerance, isConditionActiveAtPreviousInstant, aCondition) ;

    } ;

    Array<Access> accesses = Array<Access>::Empty() ;

    const auto addAccess = [this, &accesses] (const Access::Type& aType, const Instant& anAcquisitionOfSignal, const Instant& aTimeOfClosestApproach, const Instant& aLossOfSignal, const Angle& aMaxElevation)
    {

        const Access access = { aType, anAcquisitionOfSignal, aTimeOfClosestApproach, aLossOfSignal, aMaxElevation } ;

        if (accessFilter_ ? accessFilter_(access) : true)
        {
            accesses.add(access) ;
        }

    } ;

    const Array<Instant> instants = anInterval.generateGrid(step_) ;

    bool inAccess = false ;

    Instant previousInstantCache = Instant::Undefined() ;
    Instant acquisitionOfSignalCache = Instant::Undefined() ;
    Instant timeOfClosestApproachCache = Instant::Undefined() ;
    Real fromToDistanceCache_m = Real::Undefined() ;

    for (const auto& instant : instants)
    {

        const auto [ fromState, toState ] = getStatesAt(instant) ;

        const bool accessIsActive = isAccessActive(instant, fromState, toState) ;

        const bool accessIsSwitching = (accessIsActive != inAccess) || (accessIsActive && (!acquisitionOfSignalCache.isDefined())) ;

        if (accessIsSwitching)
        {

            const Instant switchingInstant = previousInstantCache.isDefined() ? findSwitchingInstant(previousInstantCache, instant, tolerance_, !accessIsActive, isAccessActiveAt) : previousInstantCache ;

            if (accessIsActive)
            {

                acquisitionOfSignalCache = switchingInstant ;

                inAccess = true ;

            }
            else
            {

                const Access::Type type = previousInstantCache.isDefined() ? Access::Type::Complete : Access::Type::Partial ;
                const Instant acquisitionOfSignal = acquisitionOfSignalCache ;
                const Instant timeOfClosestApproach = timeOfClosestApproachCache ;
                const Instant lossOfSignal = switchingInstant ;
                const Angle maxElevation = timeOfClosestApproach.isDefined() ? elevationAt(timeOfClosestApproach) : Angle::Undefined() ; // TBR: Approximate

                addAccess(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation) ;

                acquisitionOfSignalCache = Instant::Undefined() ;
                timeOfClosestApproachCache = Instant::Undefined() ;
                fromToDistanceCache_m = Real::Undefined() ;

                inAccess = false ;

            }

        }

        if (inAccess)
        {

            const auto [ fromPosition, toPosition ] = getPositionsFromStates(fromState, toState) ;

            const Point fromPositionCoordinates = Point::Vector(fromPosition.accessCoordinates()) ;
            const Point toPositionCoordinates = Point::Vector(toPosition.accessCoordinates()) ;

            const Real fromToDistance_m = (toPositionCoordinates - fromPositionCoordinates).norm() ;

            if (!timeOfClosestApproachCache.isDefined())
            {

                timeOfClosestApproachCache = acquisitionOfSignalCache ;

                fromToDistanceCache_m = fromToDistance_m ;

            }
            else if (fromToDistanceCache_m.isDefined())
            {

                if (fromToDistance_m < fromToDistanceCache_m)
                {

                    timeOfClosestApproachCache = instant ;

                    fromToDistanceCache_m = fromToDistance_m ;

                }
                else if (fromToDistance_m >= fromToDistanceCache_m)
                {

                    struct Context
                    {

                        const Instant& instant ;
                        const std::function<Pair<State, State> (const Instant& anInstant)>& getStatesAt ;
                        const std::function<Pair<Position, Position> (const State& aFromState, const State& aToState)>& getPositionsFromStates ;

                    } ;

                    const auto calculateRange = [] (const std::vector<double>& x, std::vector<double>& aGradient, void* aDataContext) -> double
                    {

                        (void) aGradient ;

                        if (aDataContext == nullptr)
                        {
                            throw ostk::core::error::runtime::Wrong("Data context") ;
                        }

                        const Context* contextPtr = static_cast<const Context*>(aDataContext) ;

                        const Duration offset = Duration::Seconds(x[0]) ;
                        const Instant queryInstant = contextPtr->instant + offset ;

                        const auto [ queryFromState, queryToState ] = contextPtr->getStatesAt(queryInstant) ;
                        const auto [ queryFromPosition, queryToPosition ] = contextPtr->getPositionsFromStates(queryFromState, queryToState) ;

                        const Real squaredRange_m = (queryToPosition.accessCoordinates() - queryFromPosition.accessCoordinates()).squaredNorm() ;

                        // std::cout << String::Format("{} [s] @ {} => {} [m]", x[0], queryInstant.toString(), squaredRange_m.toString()) << std::endl ;

                        return squaredRange_m ;

                    } ;

                    Context context = { instant, getStatesAt, getPositionsFromStates } ;

                    nlopt::opt optimizer = { nlopt::LN_COBYLA, 1 } ;

                    const std::vector<double> lowerBound = { -2.0 * step_.inSeconds() } ;
                    const std::vector<double> upperBound = { +2.0 * step_.inSeconds() } ;

                    optimizer.set_lower_bounds(lowerBound) ;
                    optimizer.set_upper_bounds(upperBound) ;

                    optimizer.set_min_objective(calculateRange, &context) ;

                    optimizer.set_xtol_rel(tolerance_.inSeconds()) ;

                    std::vector<double> x = { 0.0 } ;

                    try
                    {

                        double minimumSquaredRange ;

                        nlopt::result result = optimizer.optimize(x, minimumSquaredRange) ;

                        switch (result)
                        {

                            case nlopt::STOPVAL_REACHED:
                            case nlopt::FTOL_REACHED:
                            case nlopt::XTOL_REACHED:
                            case nlopt::MAXEVAL_REACHED:
                            case nlopt::MAXTIME_REACHED:
                                timeOfClosestApproachCache = instant + Duration::Seconds(x[0]) ;
                                break ;

                            case nlopt::FAILURE:
                            case nlopt::INVALID_ARGS:
                            case nlopt::OUT_OF_MEMORY:
                            case nlopt::ROUNDOFF_LIMITED:
                            case nlopt::FORCED_STOP:
                            default:
                                throw ostk::core::error::RuntimeError("Cannot find TCA (solution did not converge).") ;
                                break ;

                        }

                    }
                    catch (const std::exception& anException)
                    {
                        throw ostk::core::error::RuntimeError("Cannot find TCA (algorithm failed): [{}].", anException.what()) ;
                    }

                    if (timeOfClosestApproachCache < acquisitionOfSignalCache)
                    {
                        timeOfClosestApproachCache = acquisitionOfSignalCache ;
                    }

                    fromToDistanceCache_m = Real::Undefined() ;

                }

            }

        }

        previousInstantCache = instant ;

    }

    if (inAccess)
    {

        const Access::Type type = Access::Type::Partial ;
        const Instant acquisitionOfSignal = acquisitionOfSignalCache ;
        const Instant timeOfClosestApproach = timeOfClosestApproachCache ;
        const Instant lossOfSignal = instants.accessLast() ;
        const Angle maxElevation = elevationAt(timeOfClosestApproach) ; // TBR: Approximate

        addAccess(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal, maxElevation) ;

    }

    return accesses ;

}

void                            Generator::setStep                          (   const   Duration&                   aStep                                       )
{

    if (!aStep.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Step") ;
    }

    step_ = aStep ;

}

void                            Generator::setTolerance                     (   const   Duration&                   aTolerance                                  )
{

    if (!aTolerance.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tolerance") ;
    }

    tolerance_ = aTolerance ;

}

void                            Generator::setAerFilter                     (   const   std::function<bool (const AER&)>& anAerFilter                           )
{
    aerFilter_ = anAerFilter ;
}

void                            Generator::setAccessFilter                  (   const   std::function<bool (const Access&)>& anAccessFilter                     )
{
    accessFilter_ = anAccessFilter ;
}

void                            Generator::setStateFilter                  (   const   std::function<bool (const State&, const State&)>& aStateFilter           )
{
    stateFilter_ = aStateFilter ;
}

Generator                       Generator::Undefined                        ( )
{
    return { Environment::Undefined() } ;
}

Generator                       Generator::AerRanges                        (   const   Interval<Real>&             anAzimuthRange,
                                                                                const   Interval<Real>&             anElevationRange,
                                                                                const   Interval<Real>&             aRangeRange,
                                                                                const   Environment&                anEnvironment                               )
{

    using ostk::core::types::Real ;

    const Interval<Real> azimuthRange_deg = anAzimuthRange ;
    const Interval<Real> elevationRange_deg = anElevationRange ;
    const Interval<Real> rangeRange_m = aRangeRange ;

    // const Interval<Real> azimuthRange_deg = anAzimuthRange ; // anAzimuthRange.isDefined() ? Interval<Real>(anAzimuthRange.accessLowerBound().inDegrees(0.0, +360.0), anAzimuthRange.accessUpperBound().inDegrees(0.0, +360.0), anAzimuthRange.getType()) : Interval<Real>::Undefined() ;
    // const Interval<Real> elevationRange_deg = anElevationRange ; // anElevationRange.isDefined() ? Interval<Real>(anElevationRange.accessLowerBound().inDegrees(-180.0, +180.0), anElevationRange.accessUpperBound().inDegrees(-180.0, +180.0), anElevationRange.getType()) : Interval<Real>::Undefined() ;
    // const Interval<Real> rangeRange_m = aRangeRange ; // aRangeRange.isDefined() ? Interval<Real>(aRangeRange.accessLowerBound().inMeters(), aRangeRange.accessUpperBound().inMeters(), aRangeRange.getType()) : Interval<Real>::Undefined() ;

    const std::function<bool (const AER&)> aerFilter = [azimuthRange_deg, elevationRange_deg, rangeRange_m] (const AER& anAER) -> bool
    {

        return ((!azimuthRange_deg.isDefined()) || azimuthRange_deg.contains(anAER.getAzimuth().inDegrees(0.0, +360.0)))
            && ((!elevationRange_deg.isDefined()) || elevationRange_deg.contains(anAER.getElevation().inDegrees(-180.0, +180.0)))
            && ((!rangeRange_m.isDefined()) || rangeRange_m.contains(anAER.getRange().inMeters())) ;

    } ;

    return { anEnvironment, aerFilter } ;

}

Generator                       Generator::AerMask                           (  const   Map<Real, Real>&            anAzimuthElevationMask,
                                                                                const   Interval<Real>&             aRangeRange,
                                                                                const   Environment&                anEnvironment                               )
{

    using ostk::core::types::Real ;
    using ostk::core::ctnr::Map ;

    using ostk::math::obj::Vector2d ;

    if ((anAzimuthElevationMask.empty()) || (anAzimuthElevationMask.begin()->first < 0.0) || (anAzimuthElevationMask.rbegin()->first > 360.0))
    {
        throw ostk::core::error::runtime::Wrong("Azimuth-Elevation Mask") ;
    }

    for (const auto& azimuthElevationPair : anAzimuthElevationMask)
    {
        if ((azimuthElevationPair.second).abs() > 90.0)
        {
            throw ostk::core::error::runtime::Wrong("Azimuth-Elevation Mask") ;
        }
    }

    Map<Real, Real> anAzimuthElevationMask_deg = anAzimuthElevationMask ;
    const Interval<Real> rangeRange_m = aRangeRange ;

    if (anAzimuthElevationMask_deg.begin()->first != 0.0)
    {
        anAzimuthElevationMask_deg.insert({ 0.0, anAzimuthElevationMask_deg.begin()->second }) ;
    }

    if (anAzimuthElevationMask_deg.rbegin()->first != 360.0)
    {
        anAzimuthElevationMask_deg.insert({ 360.0, anAzimuthElevationMask_deg.begin()->second }) ;
    }

    const std::function<bool (const AER&)> aerFilter = [anAzimuthElevationMask_deg, rangeRange_m] (const AER& anAER) -> bool
    {

        const Real azimuth = anAER.getAzimuth().inDegrees(0.0, +360.0) ;
        const Real elevation = anAER.getElevation().inDegrees(-180.0, +180.0) ;

        auto itLow = anAzimuthElevationMask_deg.lower_bound(azimuth) ; itLow-- ;
        auto itUp = anAzimuthElevationMask_deg.upper_bound(azimuth) ;

        // Vector between the two successive mask data points with bounding azimuth values

        const Vector2d lowToUpVector = { itUp->first - itLow->first, itUp->second - itLow->second } ;

        // Vector from data point with azimuth lower bound to tested point

        const Vector2d lowToPointVector = { azimuth - itLow->first, elevation - itLow->second } ;

        // If the determinant of these two vectors is positive, the tested point lies above the function defined by the mask

        return (lowToUpVector[0] * lowToPointVector[1] - lowToUpVector[1] * lowToPointVector[0] >= 0.0)
            && ((!rangeRange_m.isDefined()) || rangeRange_m.contains(anAER.getRange().inMeters())) ;

    } ;

    return { anEnvironment, aerFilter } ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
