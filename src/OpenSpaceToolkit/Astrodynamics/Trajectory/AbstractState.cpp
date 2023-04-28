////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/AbstractState.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/AbstractState.hpp>

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

                                AbstractState::AbstractState                (   const   Instant&                    anInstant,
                                                                                const   std::vector<double>&        aCoordinates                                )
                                :   instant_(anInstant),
                                    coordinates_(aCoordinates)
{

}

bool                            AbstractState::isDefined                            ( ) const
{
    return this->instant_.isDefined() ; // TBI
}

const Instant&                  AbstractState::accessInstant                        ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("AbstractState") ;
    }

    return this->instant_ ;

}

const std::vector<double>&                  AbstractState::accessCoordinates            ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("AbstractState") ;
    }

    return this->coordinates_ ;

}

Instant                         AbstractState::getInstant                           ( ) const
{
    return this->accessInstant() ;
}


std::vector<double>             AbstractState::getCoordinates                       ( ) const
{

    return this->accessCoordinates() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
