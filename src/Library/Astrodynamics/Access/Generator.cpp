////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Access/Generator.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Access/Generator.hpp>

#include <Library/Mathematics/Geometry/3D/Objects/Segment.hpp>
#include <Library/Mathematics/Geometry/3D/Objects/Point.hpp>

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
                                    aerFilter_({}),
                                    accessFilter_({})
{

}

                                Generator::Generator                        (   const   Environment&                anEnvironment,
                                                                                const   std::function<bool (const AER&)>& anAerFilter,
                                                                                const   std::function<bool (const Access&)>& anAccessFilter                     )
                                :   environment_(anEnvironment),
                                    aerFilter_(anAerFilter),
                                    accessFilter_(anAccessFilter)
{

}

bool                            Generator::isDefined                        ( ) const
{
    return environment_.isDefined() ;
}

Array<Access>                   Generator::computeAccesses                  (   const   physics::time::Interval&    anInterval,
                                                                                const   Trajectory&                 aFromTrajectory,
                                                                                const   Trajectory&                 aToTrajectory                               ) const
{

    // [PREDICT](https://www.qsl.net/kd2bd/predict.html)
    // [Gpredict](https://github.com/csete/gpredict)
    // [Rapid Determination of Satellite Visibility Periods](http://www.dtic.mil/dtic/tr/fulltext/u2/a267281.pdf)
    // [A Fast Prediction Algorithm of Satellite Passes](https://pdfs.semanticscholar.org/6088/1d27c9bf44c59e663b359223e8ed0a91140b.pdf)
    // [Visual Contact between Two Earth’s Satellites](http://thescipub.com/pdf/10.3844/ajassp.2012.620.623)
    // [Rapid Satellite-to-Site Visibility Determination Based on Self-Adaptive Interpolation Technique](https://arxiv.org/pdf/1611.02402.pdf)

    using library::core::types::Shared ;
    using library::core::types::Real ;

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

    const auto isAccessActiveAt = [this, &environment, &aFromTrajectory, &aToTrajectory] (const Instant& anInstant) -> bool
    {

        environment.setInstant(anInstant) ;

        static const Shared<const Frame> commonFrameSPtr = Frame::GCRF() ;

        const State fromState = aFromTrajectory.getStateAt(anInstant) ;
        const State toState = aToTrajectory.getStateAt(anInstant) ;

        const Position fromPosition = fromState.accessPosition() ;
        const Position toPosition = toState.accessPosition() ;

        const Point fromPositionCoordinates = fromPosition.inFrame(commonFrameSPtr, anInstant).accessCoordinates() ;
        const Point toPositionCoordinates = toPosition.inFrame(commonFrameSPtr, anInstant).accessCoordinates() ;

        // Line of sight

        bool lineOfSight = true ;

        if (fromPositionCoordinates != toPositionCoordinates)
        {

            const Segment fromToSegment = { fromPositionCoordinates, toPositionCoordinates } ;

            const Object::Geometry fromToSegmentGeometry = { fromToSegment, commonFrameSPtr } ;

            lineOfSight = !environment.intersects(fromToSegmentGeometry) ;

            // std::cout << "lineOfSight = " << lineOfSight << std::endl ;

        }

        // Filtering

        bool filterIsOk = true ;

        if (lineOfSight)
        {

            const bool aerFilterIsOk = aerFilter_ ? aerFilter_(AER::FromPositionToPosition(fromPosition, toPosition)) : true ; // [TBM] Convert to NED frame

            filterIsOk = aerFilterIsOk ; // [TBI]

        }

        const bool accessIsActive = (lineOfSight && filterIsOk) ;

        return accessIsActive ;

    } ;

    const std::function<Instant (const Instant&, const Instant&, const Duration&, const bool)> findSwitchingInstant =
    [&findSwitchingInstant, &isAccessActiveAt]
    (const Instant& aPreviousInstant, const Instant& aNextInstant, const Duration& aThreshold, const bool isInAccess) -> Instant
    {

        const Duration step = Duration::Between(aPreviousInstant, aNextInstant) ;

        if (step <= aThreshold)
        {
            return aNextInstant ;
        }

        const Instant midInstant = aPreviousInstant + (step / 2.0) ;

        const bool accessActiveInMidInstant = isAccessActiveAt(midInstant) ;
        
        if (!isInAccess)
        {

            if (accessActiveInMidInstant)
            {
                return findSwitchingInstant(aPreviousInstant, midInstant, aThreshold, isInAccess) ;
            }

            return findSwitchingInstant(midInstant, aNextInstant, aThreshold, isInAccess) ;

        }
        
        if (!accessActiveInMidInstant)
        {
            return findSwitchingInstant(aPreviousInstant, midInstant, aThreshold, isInAccess) ;
        }

        return findSwitchingInstant(midInstant, aNextInstant, aThreshold, isInAccess) ;

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
    
    const Duration step = Duration::Minutes(1.0) ; // [TBM] Parameter

    const Array<Instant> instants = anInterval.generateGrid(step) ;

    bool inAccess = false ;

    Instant previousInstantCache = Instant::Undefined() ;
    Instant acquisitionOfSignalCache = Instant::Undefined() ;
    Instant timeOfClosestApproachCache = Instant::Undefined() ;
    Real fromToDistanceCache_m = Real::Undefined() ;

    for (const auto& instant : instants)
    {

        const bool accessIsActive = isAccessActiveAt(instant) ;

        if (accessIsActive)
        {

            timeOfClosestApproachCache = instant ; // [TBR]
            
            // const Real fromToDistance_m = (toPositionCoordinates - fromPositionCoordinates).norm() ;

            // if ((!timeOfClosestApproachCache.isDefined()) || (fromToDistanceCache_m > fromToDistance_m))
            // {

            //     timeOfClosestApproachCache = instant ;
                
            //     fromToDistanceCache_m = fromToDistance_m ;

            // }

        }

        const bool accessIsSwitching = (accessIsActive != inAccess) || (accessIsActive && (!acquisitionOfSignalCache.isDefined())) ;

        // std::cout << String::Format("@ {} - Distance: {} [m] - Intersects: {}", instant.toString(), fromToDistance_m.toString(), environment.intersects(fromToSegmentGeometry)) << std::endl ;

        if (accessIsSwitching)
        {

            const Instant switchingInstant = previousInstantCache.isDefined() ? findSwitchingInstant(previousInstantCache, instant, Duration::Milliseconds(1.0), !accessIsActive) : previousInstantCache ; // [TBM] Param

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