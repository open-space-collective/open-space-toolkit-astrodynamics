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

State                           State::operator -                           (   const   State&                      aState                                      ) const
{

    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    if (this->accessPosition().accessFrame() != aState.accessPosition().accessFrame())
    {
        throw ostk::core::error::runtime::Wrong("Position Frame") ;
    }

    if (this->accessVelocity().accessFrame() != aState.accessVelocity().accessFrame())
    {
        throw ostk::core::error::runtime::Wrong("Velocity Frame") ;
    }

    if (this->instant_ != aState.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant") ;
    }

    if (this->accessPosition().getUnit() != aState.accessPosition().getUnit())
    {
        throw ostk::core::error::runtime::Wrong("Position Unit") ;
    }

    if (this->accessVelocity().getUnit() != aState.accessVelocity().getUnit())
    {
        throw ostk::core::error::runtime::Wrong("Velocity Unit") ;
    }

    Position newPosition = Position(this->getPosition().getCoordinates() - aState.getPosition().getCoordinates(), this->accessPosition().getUnit(), this->accessPosition().accessFrame()) ;
    Velocity newVelocity = Velocity(this->getVelocity().getCoordinates() - aState.getVelocity().getCoordinates(), this->accessVelocity().getUnit(), this->accessVelocity().accessFrame()) ;

    return {this->instant_, newPosition, newVelocity} ;

}

State                           State::operator +                           (   const   State&                      aState                                      ) const
{

    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    if (this->accessPosition().accessFrame() != aState.accessPosition().accessFrame())
    {
        throw ostk::core::error::runtime::Wrong("Position Frame") ;
    }

    if (this->accessVelocity().accessFrame() != aState.accessVelocity().accessFrame())
    {
        throw ostk::core::error::runtime::Wrong("Velocity Frame") ;
    }

    if (this->instant_ != aState.instant_)
    {
        throw ostk::core::error::runtime::Wrong("Instant") ;
    }

    if (this->accessPosition().getUnit() != aState.accessPosition().getUnit())
    {
        throw ostk::core::error::runtime::Wrong("Position Unit") ;
    }

    if (this->accessVelocity().getUnit() != aState.accessVelocity().getUnit())
    {
        throw ostk::core::error::runtime::Wrong("Velocity Unit") ;
    }

    Position newPosition = Position(this->getPosition().getCoordinates() + aState.getPosition().getCoordinates(), this->accessPosition().getUnit(), this->accessPosition().accessFrame()) ;
    Velocity newVelocity = Velocity(this->getVelocity().getCoordinates() + aState.getVelocity().getCoordinates(), this->accessVelocity().getUnit(), this->accessVelocity().accessFrame()) ;

    return {this->instant_, newPosition, newVelocity} ;

}

bool                            State::operator ==                          (   const   State&                      aState                                      ) const
{

    if ((!this->isDefined()) || (!aState.isDefined()))
    {
        return false ;
    }

    return (this->instant_ == aState.instant_) && (this->position_ == aState.position_) && (this->velocity_ == aState.velocity_) ;

}

bool                            State::operator !=                          (   const   State&                      aState                                      ) const
{
    return !((*this) == aState) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   State&                      aState                                      )
{

    aState.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            State::isDefined                            ( ) const
{
    return this->instant_.isDefined() && this->position_.isDefined() && this->velocity_.isDefined() ;
}

const Instant&                  State::accessInstant                        ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return this->instant_ ;

}

const Position&                 State::accessPosition                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return this->position_ ;

}

const Velocity&                 State::accessVelocity                       ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    return this->velocity_ ;

}

const VectorXd&                 State::accessCoordinates                    ( ) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    VectorXd coordinates(6);
    coordinates.segment(0, 3) = this->accessPosition().accessCoordinates() ;
    coordinates.segment(3, 6) = this->accessVelocity().accessCoordinates() ;

    return coordinates ;
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

VectorXd                        State::getCoordinates                       ( ) const
{
    return this->accessCoordinates() ;
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

    const Position position = position_.inFrame(aFrameSPtr, this->instant_) ;
    const Velocity velocity = velocity_.inFrame(this->position_, aFrameSPtr, this->instant_) ;

    return { this->instant_, position, velocity } ;

}

void                            State::print                                  (         std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    using ostk::core::types::String ;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory :: State") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Instant:" << (this->instant_.isDefined() ? this->instant_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Position:" << (this->position_.isDefined() ? this->position_.toString(12) : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Velocity:" << (this->velocity_.isDefined() ? this->velocity_.toString(12) : "Undefined") ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

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
