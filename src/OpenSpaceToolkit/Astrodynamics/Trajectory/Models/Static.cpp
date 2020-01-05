////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Static.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Static.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace models
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Static::Static                              (   const   Position&                   aPosition                                   )
                                :   Model(),
                                    position_(aPosition)
{

}

Static*                         Static::clone                               ( ) const
{
    return new Static(*this) ;
}

bool                            Static::operator ==                         (   const   Static&                     aStaticModel                                ) const
{

    if ((!this->isDefined()) || (!aStaticModel.isDefined()))
    {
        return false ;
    }

    return position_ == aStaticModel.position_ ;

}

bool                            Static::operator !=                         (   const   Static&                     aStaticModel                                ) const
{
    return !((*this) == aStaticModel) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Static&                     aStaticModel                                )
{

    aStaticModel.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            Static::isDefined                           ( ) const
{
    return position_.isDefined() ;
}

State                           Static::calculateStateAt                    (   const   Instant&                    anInstant                                   ) const
{

    using ostk::physics::coord::Position ;

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Static") ;
    }

    return State(anInstant, position_, Velocity::MetersPerSecond({ 0.0, 0.0, 0.0 }, position_.accessFrame())) ;

}

void                            Static::print                               (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    using ostk::core::types::String ;

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Static") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Position:"            << (position_.isDefined() ? position_.toString() : "Undefined") ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool                            Static::operator ==                         (   const   Model&                      aModel                                      ) const
{

    const Static* staticModelPtr = dynamic_cast<const Static*>(&aModel) ;

    return (staticModelPtr != nullptr) && this->operator == (*staticModelPtr) ;

}

bool                            Static::operator !=                         (   const   Model&                      aModel                                      ) const
{
    return !((*this) == aModel) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
