////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace trajectory
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                State::State                                (   const   Instant&                    anInstant,
                                                                                const   Position&                   aPosition,
                                                                                const   Velocity&                   aVelocity                                   )
                                :   instant_(anInstant),
                                    position_(aPosition),
                                    velocity_(aVelocity)
{

}

bool                            State::operator ==                          (   const   State&                      aState                                      ) const
{

    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        return false ;
    }

    return (instant_ == aState.instant_) && (position_ == aState.position_) && (velocity_ == aState.velocity_) ;

}

bool                            State::operator !=                          (   const   State&                      aState                                      ) const
{
    return !((*this) == aState) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   State&                      aState                                      )
{

    ostk::core::utils::Print::Header(anOutputStream, "Trajectory :: State") ;

    ostk::core::utils::Print::Line(anOutputStream) << "Instant:" << (aState.instant_.isDefined() ? aState.instant_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Position:" << (aState.position_.isDefined() ? aState.position_.toString(12) : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Velocity:" << (aState.velocity_.isDefined() ? aState.velocity_.toString(12) : "Undefined") ;

    ostk::core::utils::Print::Footer(anOutputStream) ;

    return anOutputStream ;

}

bool                            State::isDefined                            ( ) const
{
    return instant_.isDefined() && position_.isDefined() && velocity_.isDefined() ;
}

const Instant&                  State::accessInstant                        ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return instant_ ;

}

const Position&                 State::accessPosition                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return position_ ;

}

const Velocity&                 State::accessVelocity                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return velocity_ ;

}

Instant                         State::getInstant                           ( ) const
{
    return this->accessInstant() ;
}

Position                        State::getPosition                          ( ) const
{
    return this->accessPosition() ;
}

Velocity                        State::getVelocity                          ( ) const
{
    return this->accessVelocity() ;
}

State                           State::inFrame                              (   const   Shared<const Frame>&        aFrameSPtr                                  ) const
{

    if ((aFrameSPtr == nullptr) || (!aFrameSPtr->isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("Frame") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    const Position position = position_.inFrame(aFrameSPtr, instant_) ;
    const Velocity velocity = velocity_.inFrame(position_, aFrameSPtr, instant_) ;

    return { instant_, position, velocity } ;

}

State                           State::Undefined                            ( )
{
    return { Instant::Undefined(), Position::Undefined(), Velocity::Undefined() } ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
