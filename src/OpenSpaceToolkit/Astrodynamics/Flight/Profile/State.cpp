////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/State.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
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

    ostk::core::utils::Print::Header(anOutputStream, "Flight :: Profile :: State") ;

    ostk::core::utils::Print::Line(anOutputStream) << "Instant:"             << (aState.instant_.isDefined() ? aState.instant_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Position:"            << (aState.position_.isDefined() ? aState.position_.toString(12) : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Velocity:"            << (aState.velocity_.isDefined() ? aState.velocity_.toString(12) : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Attitude:"            << (aState.attitude_.isDefined() ? aState.attitude_.toString(12) : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Angular velocity:"    << (aState.angularVelocity_.isDefined() ? aState.angularVelocity_.toString(12) : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Frame:"               << (((aState.frameSPtr_ != nullptr) && aState.frameSPtr_->isDefined()) ? aState.frameSPtr_->getName() : "Undefined") ;

    ostk::core::utils::Print::Footer(anOutputStream) ;

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
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return instant_ ;

}

const Vector3d&                 State::accessPosition                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return position_ ;

}

const Vector3d&                 State::accessVelocity                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return velocity_ ;

}

const Quaternion&               State::accessAttitude                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return attitude_ ;

}

const Vector3d&                 State::accessAngularVelocity                ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
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
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return frameSPtr_ ;

}

State                           State::inFrame                              (   const   Shared<const Frame>&        aFrameSPtr                                  ) const
{

    using ostk::physics::coord::Transform ;

    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    const Transform transform_NEW_OLD = frameSPtr_->getTransformTo(aFrameSPtr, instant_) ;

    // x_NEW = T_NEW_OLD(x_OLD)

    const Vector3d position = transform_NEW_OLD.applyToPosition(position_) ;

    // v_NEW = T_NEW_OLD(v_OLD)

    const Vector3d velocity = transform_NEW_OLD.applyToVelocity(position_, velocity_) ;

    // q_B_NEW = q_B_OLD * q_OLD_NEW

    const Quaternion attitude = attitude_ * transform_NEW_OLD.getOrientation().toConjugate() ;

    // w_B_NEW_in_B = w_B_OLD_in_B + w_OLD_NEW_in_B = w_B_OLD_in_B - q_B_NEW * w_NEW_OLD_in_NEW

    const Vector3d angularVelocity = angularVelocity_ - attitude * transform_NEW_OLD.getAngularVelocity() ;

    return { instant_, position, velocity, attitude, angularVelocity, aFrameSPtr } ;

}

State                           State::Undefined                            ( )
{
    return { Instant::Undefined(), Vector3d::Undefined(), Vector3d::Undefined(), Quaternion::Undefined(), Vector3d::Undefined(), Frame::Undefined() } ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
