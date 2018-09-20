////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Profile/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Profile/State.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace flight
{
namespace profile
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                State::State                                (   const   Instant&                    anInstant,
                                                                                const   Vector3d&                   aPosition,
                                                                                const   Vector3d&                   aVelocity,
                                                                                const   Quaternion&                 anAttitude,
                                                                                const   Vector3d&                   anAngularVelocity,
                                                                                const   Shared<const Frame>&        aReferenceFrame                             )
                                :   instant_(anInstant),
                                    position_(aPosition),
                                    velocity_(aVelocity),
                                    attitude_(anAttitude),
                                    angularVelocity_(anAngularVelocity),
                                    frameSPtr_(aReferenceFrame)
{

}

bool                            State::operator ==                          (   const   State&                      aState                                      ) const
{

    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        return false ;
    }

    return (instant_ == aState.instant_) 
        && (position_ == aState.position_) 
        && (velocity_ == aState.velocity_) 
        && (attitude_ == aState.attitude_) 
        && (angularVelocity_ == aState.angularVelocity_) 
        && (frameSPtr_ == aState.frameSPtr_) ;

}

bool                            State::operator !=                          (   const   State&                      aState                                      ) const
{
    return !((*this) == aState) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   State&                      aState                                      )
{

    library::core::utils::Print::Header(anOutputStream, "Flight :: Profile :: State") ;

    library::core::utils::Print::Line(anOutputStream) << "Instant:"             << (aState.instant_.isDefined() ? aState.instant_.toString() : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Position:"            << (aState.position_.isDefined() ? aState.position_.toString(12) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Velocity:"            << (aState.velocity_.isDefined() ? aState.velocity_.toString(12) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Attitude:"            << (aState.attitude_.isDefined() ? aState.attitude_.toString(12) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Angular velocity:"    << (aState.angularVelocity_.isDefined() ? aState.angularVelocity_.toString(12) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Frame:"               << (((aState.frameSPtr_ != nullptr) && aState.frameSPtr_->isDefined()) ? aState.frameSPtr_->getName() : "Undefined") ;

    library::core::utils::Print::Footer(anOutputStream) ;

    return anOutputStream ;

}

bool                            State::isDefined                            ( ) const
{

    return instant_.isDefined() 
        && position_.isDefined() 
        && velocity_.isDefined()
        && attitude_.isDefined()
        && angularVelocity_.isDefined()
        && (frameSPtr_ != nullptr)
        && frameSPtr_->isDefined() ;

}

const Instant&                  State::accessInstant                        ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return instant_ ;

}

const Vector3d&                 State::accessPosition                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return position_ ;

}

const Vector3d&                 State::accessVelocity                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return velocity_ ;

}

const Quaternion&               State::accessAttitude                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return attitude_ ;

}

const Vector3d&                 State::accessAngularVelocity                ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return angularVelocity_ ;

}

Instant                         State::getInstant                           ( ) const
{
    return this->accessInstant() ;
}

Vector3d                        State::getPosition                          ( ) const
{
    return this->accessPosition() ;
}

Vector3d                        State::getVelocity                          ( ) const
{
    return this->accessVelocity() ;
}

Quaternion                      State::getAttitude                          ( ) const
{
    return this->accessAttitude() ;
}

Vector3d                        State::getAngularVelocity                   ( ) const
{
    return this->accessAngularVelocity() ;
}

Shared<const Frame>             State::getFrame                             ( ) const
{
    
    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return frameSPtr_ ;

}

State                           State::inFrame                              (   const   Shared<const Frame>&        aFrameSPtr                                  ) const
{

    using library::physics::coord::Transform ;

    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw library::core::error::runtime::Undefined("Frame") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }

    const Transform transform_NEW_OLD = frameSPtr_->getTransformTo(aFrameSPtr, instant_) ;

    const Vector3d position = transform_NEW_OLD.applyToPosition(position_) ;
    const Vector3d velocity = transform_NEW_OLD.applyToVelocity(position_, velocity_) ;
    const Quaternion attitude = attitude_ * transform_NEW_OLD.getOrientation().toConjugate() ;
    const Vector3d angularVelocity = { 0.0, 0.0, 0.0 } ; // transform_NEW_OLD.getAngularVelocity() [TBI] ;

    return { instant_, position, velocity, attitude, angularVelocity, aFrameSPtr } ;

}

State                           State::Undefined                            ( )
{
    return { Instant::Undefined(), Vector3d::Undefined(), Vector3d::Undefined(), Quaternion::Undefined(), Vector3d::Undefined(), nullptr } ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////