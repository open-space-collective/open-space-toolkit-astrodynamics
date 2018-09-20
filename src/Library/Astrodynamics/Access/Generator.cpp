////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Access/Generator.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Access/Generator.hpp>

#include <Library/Mathematics/Geometry/3D/Objects/Segment.hpp>
#include <Library/Mathematics/Geometry/3D/Objects/Point.hpp>

#include <Library/Core/Containers/Pair.hpp>

#include <nlopt.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace access
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Generator::Generator                        (   const   Environment&                anEnvironment                               )
                                :   environment_(anEnvironment),
                                    step_(Duration::Minutes(1.0)),
                                    tolerance_(Duration::Microseconds(1.0)),
                                    aerFilter_({}),
                                    accessFilter_({})
{

}

                                Generator::Generator                        (   const   Environment&                anEnvironment,
                                                                                const   std::function<bool (const AER&)>& anAerFilter,
                                                                                const   std::function<bool (const Access&)>& anAccessFilter                     )
                                :   environment_(anEnvironment),
                                    step_(Duration::Minutes(1.0)),
                                    tolerance_(Duration::Microseconds(1.0)),
                                    aerFilter_(anAerFilter),
                                    accessFilter_(anAccessFilter)
{

}

bool                            Generator::isDefined                        ( ) const
{
    return environment_.isDefined() && step_.isDefined() && tolerance_.isDefined() ;
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

    using library::core::types::Shared ;
    using library::core::types::Real ;
    using library::core::ctnr::Pair ;

    using library::math::geom::d3::objects::Point ;
    using library::math::geom::d3::objects::Segment ;

    using library::physics::time::Duration ;
    using library::physics::coord::Position ;
    using library::physics::coord::Frame ;
    using library::physics::env::Object ;
    
    using library::astro::trajectory::State ;

    if (!anInterval.isDefined())
    {
        throw library::core::error::runtime::Undefined("Interval") ;
    }

    if (!aFromTrajectory.isDefined())
    {
        throw library::core::error::runtime::Undefined("From Trajectory") ;
    }

    if (!aToTrajectory.isDefined())
    {
        throw library::core::error::runtime::Undefined("To Trajectory") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Generator") ;
    }

    Environment environment = environment_ ;

    const auto getPositionsAt = [&aFromTrajectory, &aToTrajectory] (const Instant& anInstant) -> Pair<Position, Position>
    {

        static const Shared<const Frame> commonFrameSPtr = Frame::GCRF() ;

        const State fromState = aFromTrajectory.getStateAt(anInstant) ;
        const State toState = aToTrajectory.getStateAt(anInstant) ;

        const Position fromPosition = fromState.accessPosition().inFrame(commonFrameSPtr, anInstant) ;
        const Position toPosition = toState.accessPosition().inFrame(commonFrameSPtr, anInstant) ;

        return { fromPosition, toPosition } ;

    } ;

    const auto isAccessActive = [this, &environment] (const Instant& anInstant, const Position& aFromPosition, const Position& aToPosition) -> bool
    {

        environment.setInstant(anInstant) ;

        static const Shared<const Frame> commonFrameSPtr = Frame::GCRF() ;

        const Point fromPositionCoordinates = Point::Vector(aFromPosition.accessCoordinates()) ;
        const Point toPositionCoordinates = Point::Vector(aToPosition.accessCoordinates()) ;

        // Line of sight

        bool lineOfSight = true ;

        if (fromPositionCoordinates != toPositionCoordinates)
        {

            const Segment fromToSegment = { fromPositionCoordinates, toPositionCoordinates } ;

            const Object::Geometry fromToSegmentGeometry = { fromToSegment, commonFrameSPtr } ;

            lineOfSight = !environment.intersects(fromToSegmentGeometry) ;

        }

        // Filtering

        bool filterIsOk = true ;

        if (lineOfSight)
        {

            const bool aerFilterIsOk = aerFilter_ ? aerFilter_(AER::FromPositionToPosition(aFromPosition, aToPosition)) : true ; // [TBM] Convert to NED frame

            filterIsOk = aerFilterIsOk ;

        }

        const bool accessIsActive = (lineOfSight && filterIsOk) ;

        return accessIsActive ;

    } ;

    const auto isAccessActiveAt = [getPositionsAt, isAccessActive] (const Instant& aNextInstant) -> bool
    {

        const Pair<Position, Position> positions = getPositionsAt(aNextInstant) ;

        const Position& fromPosition = positions.first ;
        const Position& toPosition = positions.second ;

        const bool accessIsActive = isAccessActive(aNextInstant, fromPosition, toPosition) ;

        return accessIsActive ;

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

    const auto addAccess = [this, &accesses] (const Access::Type& aType, const Instant& anAcquisitionOfSignal, const Instant& aTimeOfClosestApproach, const Instant& aLossOfSignal)
    {

        const Access access = { aType, anAcquisitionOfSignal, aTimeOfClosestApproach, aLossOfSignal } ;

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

        const Pair<Position, Position> position = getPositionsAt(instant) ;

        const Position& fromPosition = position.first ;
        const Position& toPosition = position.second ;

        const bool accessIsActive = isAccessActive(instant, fromPosition, toPosition) ;

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

                addAccess(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal) ;

                acquisitionOfSignalCache = Instant::Undefined() ;
                timeOfClosestApproachCache = Instant::Undefined() ;
                fromToDistanceCache_m = Real::Undefined() ;

                inAccess = false ;

            }

        }

        if (inAccess)
        {

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
                        const std::function<Pair<Position, Position> (const Instant& anInstant)>& getPositionsAt ;

                    } ;

                    const auto calculateRange = [] (const std::vector<double>& x, std::vector<double>& aGradient, void* aDataContext) -> double
                    {

                        (void) aGradient ;

                        if (aDataContext == nullptr)
                        {
                            throw library::core::error::runtime::Wrong("Data context") ;
                        }

                        const Context* contextPtr = static_cast<const Context*>(aDataContext) ;

                        const Duration offset = Duration::Seconds(x[0]) ;
                        const Instant queryInstant = contextPtr->instant + offset ;

                        const Pair<Position, Position> positions = contextPtr->getPositionsAt(queryInstant) ;

                        const Real squaredRange_m = (positions.second.accessCoordinates() - positions.first.accessCoordinates()).squaredNorm() ;

                        // std::cout << String::Format("{} [s] @ {} => {} [m]", x[0], queryInstant.toString(), squaredRange_m.toString()) << std::endl ;
                        
                        return squaredRange_m ;

                    } ;

                    Context context = { instant, getPositionsAt } ;

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
                                throw library::core::error::RuntimeError("Cannot find TCA (solution did not converge).") ;
                                break ;

                        }

                    }
                    catch (const std::exception& anException)
                    {
                        throw library::core::error::RuntimeError("Cannot find TCA (algorithm failed): [{}].", anException.what()) ;                        
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

        addAccess(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal) ;

    }

    return accesses ;

}

void                            Generator::setStep                          (   const   Duration&                   aStep                                       )
{

    if (!aStep.isDefined())
    {
        throw library::core::error::runtime::Undefined("Step") ;
    }

    step_ = aStep ;

}

void                            Generator::setTolerance                     (   const   Duration&                   aTolerance                                  )
{

    if (!aTolerance.isDefined())
    {
        throw library::core::error::runtime::Undefined("Tolerance") ;
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

Generator                       Generator::Undefined                        ( )
{
    return { Environment::Undefined(), {}, {} } ;
}

Generator                       Generator::AerRanges                        (   const   Interval<Angle>&            anAzimuthRange,
                                                                                const   Interval<Angle>&            anElevationRange,
                                                                                const   Interval<Length>&           aRangeRange,
                                                                                const   Environment&                anEnvironment                               )
{

    using library::core::types::Real ;

    const Interval<Real> azimuthRange_deg = anAzimuthRange.isDefined() ? Interval<Real>(anAzimuthRange.accessLowerBound().inDegrees(0.0, +360.0), anAzimuthRange.accessUpperBound().inDegrees(0.0, +360.0), anAzimuthRange.getType()) : Interval<Real>::Undefined() ;
    const Interval<Real> elevationRange_deg = anElevationRange.isDefined() ? Interval<Real>(anElevationRange.accessLowerBound().inDegrees(-180.0, +180.0), anElevationRange.accessUpperBound().inDegrees(-180.0, +180.0), anElevationRange.getType()) : Interval<Real>::Undefined() ;
    const Interval<Real> rangeRange_m = aRangeRange.isDefined() ? Interval<Real>(aRangeRange.accessLowerBound().inMeters(), aRangeRange.accessUpperBound().inMeters(), aRangeRange.getType()) : Interval<Real>::Undefined() ;

    const std::function<bool (const AER&)> aerFilter = [azimuthRange_deg, elevationRange_deg, rangeRange_m] (const AER& anAER) -> bool
    {
        
        return ((!azimuthRange_deg.isDefined()) || azimuthRange_deg.contains(anAER.getAzimuth().inDegrees(0.0, +360.0)))
            && ((!elevationRange_deg.isDefined()) || elevationRange_deg.contains(anAER.getElevation().inDegrees(-180.0, +180.0)))
            && ((!rangeRange_m.isDefined()) || rangeRange_m.contains(anAER.getRange().inMeters())) ;
        
    } ;

    return { anEnvironment, aerFilter, {} } ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////