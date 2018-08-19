////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/State.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/State.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
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

    library::core::utils::Print::Header(anOutputStream, "Trajectory :: State") ;

    library::core::utils::Print::Line(anOutputStream) << "Instant:" << (aState.instant_.isDefined() ? aState.instant_.toString() : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Position:" << (aState.position_.isDefined() ? aState.position_.toString(12) : "Undefined") ;
    library::core::utils::Print::Line(anOutputStream) << "Velocity:" << (aState.velocity_.isDefined() ? aState.velocity_.toString(12) : "Undefined") ;

    library::core::utils::Print::Footer(anOutputStream) ;

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
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return instant_ ;

}

const Position&                 State::accessPosition                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return position_ ;

}

const Velocity&                 State::accessVelocity                       ( ) const
{

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }
    
    return velocity_ ;

}

State                           State::inFrame                              (   const   Shared<Frame>&              aFrame                                      ) const
{

    if ((aFrame == nullptr) || (!aFrame->isDefined()))
    {
        throw library::core::error::runtime::Undefined("Frame") ;
    }

    if (!this->isDefined())
    {
        throw library::core::error::runtime::Undefined("State") ;
    }

    const Position position = position_.inFrame(aFrame, instant_) ;
    const Velocity velocity = velocity_.inFrame(position_, aFrame, instant_) ;

    return { instant_, position, velocity } ;

}

State                           State::Undefined                            ( )
{
    return State(Instant::Undefined(), Position::Undefined(), Velocity::Undefined()) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////