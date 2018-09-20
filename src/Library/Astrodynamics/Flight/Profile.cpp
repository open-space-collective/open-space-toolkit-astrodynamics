////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Profile.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Profile.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace flight
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Profile::Profile                            (   const   DynamicProvider&            aDynamicTransformProvider,
                                                                                const   Shared<const Frame>&        aFrameSPtr                                  )
                                :   transformProvider_(aDynamicTransformProvider),
                                    frameSPtr_(aFrameSPtr)
{

}

//                                 Profile::Profile                            (   const   Array<State>&               aStateArray                                 )
// {

// }

//                                 Profile::Profile                            (   const   Profile&                    aProfile                                    )
// {

// }

// Profile&                        Profile::operator =                         (   const   Profile&                    aProfile                                    )
// {

// }

// bool                            Profile::operator ==                        (   const   Profile&                    aProfile                                    ) const
// {

//     if ((!this->isDefined()) || (!aProfile.isDefined()))
//     {
//         return false ;
//     }

//     return (transformProvider_ == aProfile.transformProvider_) && (frameSPtr_ == aProfile.frameSPtr_) ;

// }

// bool                            Profile::operator !=                        (   const   Profile&                    aProfile                                    ) const
// {
//     return !((*this) == aProfile) ;
// }

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Profile&                    aProfile                                    )
{

    aProfile.print(anOutputStream, true) ;

    return anOutputStream ;

}

bool                            Profile::isDefined                          ( ) const
{
    return transformProvider_.isDefined() && (frameSPtr_ != nullptr) && frameSPtr_->isDefined() ;
}

State                           Profile::getStateAt                         (   const   Instant&                    anInstant                                   ) const
{

    using library::physics::coord::Transform ;

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Profile") ;
    }

    const Transform transform = transformProvider_.getTransformAt(anInstant) ;

    const Vector3d position = transform.applyToPosition({ 0.0, 0.0, 0.0 }) ;
    const Vector3d velocity = transform.applyToVelocity({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }) ;
    const Quaternion attitude = transform.getOrientation().toConjugate() ;
    const Vector3d angularVelocity = transform.getAngularVelocity() ; // [TBC]

    return { anInstant, position, velocity, attitude, angularVelocity, frameSPtr_ } ;

}

Array<State>                    Profile::getStatesAt                        (   const   Array<Instant>&             anInstantArray                              ) const
{

    Array<State> stateArray = Array<State>::Empty() ;

    stateArray.reserve(anInstantArray.getSize()) ;

    for (const auto& instant : anInstantArray)
    {
        stateArray.add(this->getStateAt(instant)) ;
    }

    return stateArray ;

}

Axes                            Profile::getAxesAt                          (   const   Instant&                    anInstant                                   ) const
{

    using library::physics::coord::Transform ;

    if (!anInstant.isDefined())
    {
        throw library::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("Profile") ;
    }

    const Transform transform = transformProvider_.getTransformAt(anInstant) ;

    const Quaternion q_GCRF_BODY = transform.getOrientation() ;

    const Vector3d xAxis = q_GCRF_BODY * Vector3d::X() ;
    const Vector3d yAxis = q_GCRF_BODY * Vector3d::Y() ;
    const Vector3d zAxis = q_GCRF_BODY * Vector3d::Z() ;

    return { xAxis, yAxis, zAxis, frameSPtr_ } ;

}

void                            Profile::print                              (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? library::core::utils::Print::Header(anOutputStream, "Flight Profile") : void () ;

    library::core::utils::Print::Line(anOutputStream) << "Frame:"               << (((frameSPtr_ != nullptr) && frameSPtr_->isDefined()) ? frameSPtr_->getName() : "Undefined") ;

    displayDecorator ? library::core::utils::Print::Footer(anOutputStream) : void () ;

}

Profile                         Profile::Undefined                          ( )
{
    return { DynamicProvider::Undefined(), nullptr } ;
}

Profile                         Profile::InertialPointing                   (   const   Trajectory&                 aTrajectory,
                                                                                const   Quaternion&                 aQuaternion                                 )
{

    using library::physics::coord::Transform ;

    if (!aTrajectory.isDefined())
    {
        throw library::core::error::runtime::Undefined("Trajectory") ;
    }

    if (!aQuaternion.isDefined())
    {
        throw library::core::error::runtime::Undefined("Quaternion") ;
    }

    const DynamicProvider dynamicTransformProvider =
    {
        [aTrajectory, aQuaternion] (const Instant& anInstant) -> Transform
        {

            static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            const trajectory::State trajectoryState = aTrajectory.getStateAt(anInstant).inFrame(gcrfSPtr) ;

            const Vector3d x_GCRF = trajectoryState.getPosition().getCoordinates() ;
            const Vector3d v_GCRF = trajectoryState.getVelocity().getCoordinates() ;
            
            return Transform::Passive(anInstant, x_GCRF, v_GCRF, aQuaternion, { 0.0, 0.0, 0.0 }) ;

        }
    } ;

    return { dynamicTransformProvider, Frame::GCRF() } ;

}

Profile                         Profile::NadirPointing                      (   const   trajectory::Orbit&          anOrbit,
                                                                                const   trajectory::Orbit::FrameType& anOrbitalFrameType                        )
{

    using library::physics::coord::Transform ;

    if (!anOrbit.isDefined())
    {
        throw library::core::error::runtime::Undefined("Orbit") ;
    }

    if (anOrbitalFrameType == trajectory::Orbit::FrameType::Undefined)
    {
        throw library::core::error::runtime::Undefined("Orbital frame type") ;
    }

    const Shared<const Frame> orbitalFrameSPtr = anOrbit.getOrbitalFrame(anOrbitalFrameType) ;

    const DynamicProvider dynamicTransformProvider =
    {
        [anOrbit, orbitalFrameSPtr] (const Instant& anInstant) -> Transform
        {

            static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

            return orbitalFrameSPtr->getTransformTo(gcrfSPtr, anInstant) ;

        }
    } ;

    return { dynamicTransformProvider, Frame::GCRF() } ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////