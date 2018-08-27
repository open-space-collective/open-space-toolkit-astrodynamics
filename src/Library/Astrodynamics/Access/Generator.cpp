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

    using library::core::types::Real ;
    
    using library::physics::time::Duration ;

    using library::math::geom::d3::objects::Point ;
    using library::math::geom::d3::objects::Segment ;

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

    Array<Access> accesses ;
    
    const Duration step = Duration::Seconds(1.0) ; // [TBM] Parameter

    const Array<Instant> instants = anInterval.generateGrid(step) ;

    bool isFirst = true ;
    
    bool inAccess = false ;

    Instant acquisitionOfSignalCache = Instant::Undefined() ;
    Instant timeOfClosestApproachCache = Instant::Undefined() ;
    Real fromToDistanceCache_m = Real::Undefined() ;

    for (const auto& instant : instants)
    {

        environment.setInstant(instant) ;

        // std::cout << "instant = " << instant.toString() << std::endl ;

        const State fromState = aFromTrajectory.getStateAt(instant) ;
        const State toState = aToTrajectory.getStateAt(instant) ;

        const Position fromPosition = fromState.accessPosition() ;
        const Position toPosition = toState.accessPosition() ;

        const Point fromPositionCoordinates = fromPosition.inFrame(Frame::GCRF(), instant).accessCoordinates() ;
        const Point toPositionCoordinates = toPosition.inFrame(Frame::GCRF(), instant).accessCoordinates() ;

        const Real fromToDistance_m = (toPositionCoordinates - fromPositionCoordinates).norm() ;

        // std::cout << "fromToDistance_m = " << fromToDistance_m.toString() << std::endl ;

        // [TBI] Check reference frames are equal

        const Segment fromToSegment = { fromPositionCoordinates, toPositionCoordinates } ;

        const Object::Geometry fromToSegmentGeometry = { fromToSegment, Frame::GCRF() } ; // [TBM] Param

        // [TBI] Apply filters

        const bool aerFilterResult = aerFilter_ ? aerFilter_(AER::FromPositionToPosition(fromPosition, toPosition)) : true ; // [TBM] Convert to NED frame

        bool filterIsOk = aerFilterResult ; // [TBI]

        if ((!fromToDistance_m.isZero()) && (environment.intersects(fromToSegmentGeometry) || (!filterIsOk)))
        {

            // std::cout << "environment.intersects(fromToSegmentGeometry)..." << std::endl ;

            if (inAccess)
            {

                const Access::Type type = isFirst ? Access::Type::Partial : Access::Type::Complete ;
                const Instant acquisitionOfSignal = acquisitionOfSignalCache ;
                const Instant timeOfClosestApproach = timeOfClosestApproachCache ;
                const Instant lossOfSignal = instant ;

                const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

                if (accessFilter_ ? accessFilter_(access) : true)
                {
                    accesses.add(access) ;
                }

                acquisitionOfSignalCache = Instant::Undefined() ;
                timeOfClosestApproachCache = Instant::Undefined() ;

                inAccess = false ;
            
            }

        }
        else
        {

            if (!inAccess)
            {

                acquisitionOfSignalCache = instant ;

                // std::cout << "inAccess = true" << std::endl ;

                inAccess = true ;

            }

            if (inAccess && ((!fromToDistanceCache_m.isDefined()) || (fromToDistanceCache_m > fromToDistance_m)))
            {

                timeOfClosestApproachCache = instant ;
                
                fromToDistanceCache_m = fromToDistance_m ;

            }

        }

        if (isFirst)
        {
            isFirst = false ;
        }

    }

    if (inAccess)
    {

        const Access::Type type = Access::Type::Partial ;
        const Instant acquisitionOfSignal = acquisitionOfSignalCache ;
        const Instant timeOfClosestApproach = timeOfClosestApproachCache ;
        const Instant lossOfSignal = instants.accessLast() ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        if (accessFilter_ ? accessFilter_(access) : true)
        {
            accesses.add(access) ;
        }

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
    return Generator(Environment::Undefined(), {}, {}) ;
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

    return Generator(anEnvironment, aerFilter, {}) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////