////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           Library/Astrodynamics/Trajectory.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Models/Tabulated.hpp>
#include <Library/Astrodynamics/Trajectory/Models/Static.hpp>
#include <Library/Astrodynamics/Trajectory.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::astro::trajectory::models::Tabulated ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Trajectory::Trajectory                      (   const   Model&                      aModel                                      )
                                :   modelUPtr_(aModel.clone())
{

}

                                Trajectory::Trajectory                      (   const   Array<State>&               aStateArray                                 )
                                :   modelUPtr_(std::make_unique<Tabulated>(aStateArray))
{

}

                                Trajectory::Trajectory                      (   const   Trajectory&                 aTrajectory                                 )
                                :   modelUPtr_((aTrajectory.modelUPtr_) != nullptr ? aTrajectory.modelUPtr_->clone() : nullptr)
{

}

bool                            Trajectory::operator ==                     (   const   Trajectory&                 aTrajectory                                 ) const
{

    if ((!this->isDefined()) || (!aTrajectory.isDefined()))
    {
        return false ;
    }

    return (*modelUPtr_) == (*aTrajectory.modelUPtr_) ;

}

bool                            Trajectory::operator !=                     (   const   Trajectory&                 aTrajectory                                 ) const
{
    return !((*this) == aTrajectory) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Trajectory&                 aTrajectory                                 )
{

    aTrajectory.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            Trajectory::isDefined                       ( ) const
{
    return (modelUPtr_ != nullptr) && modelUPtr_->isDefined() ;
}

const Model&                    Trajectory::accessModel                     ( ) const
{

    // if (!this->isDefined())
    // {
    //     throw ostk::core::error::runtime::Undefined("Trajectory") ;
    // }

    if (modelUPtr_ == nullptr)
    {
        throw ostk::core::error::runtime::Undefined("Model") ;
    }

    return *modelUPtr_ ;

}

State                           Trajectory::getStateAt                      (   const   Instant&                    anInstant                                   ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory") ;
    }

    return modelUPtr_->calculateStateAt(anInstant) ;

}

Array<State>                    Trajectory::getStatesAt                     (   const   Array<Instant>&             anInstantArray                              ) const
{

    Array<State> stateArray = Array<State>::Empty() ;

    stateArray.reserve(anInstantArray.getSize()) ;

    for (const auto& instant : anInstantArray)
    {
        stateArray.add(this->getStateAt(instant)) ;
    }

    return stateArray ;

}

void                            Trajectory::print                           (           std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Trajectory") : void () ;

    ostk::core::utils::Print::Separator(anOutputStream, "Model") ;

    modelUPtr_->print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

Trajectory                      Trajectory::Undefined                       ( )
{
    return Trajectory() ;
}

Trajectory                      Trajectory::Position                        (   const   physics::coord::Position&   aPosition                                   )
{

    using ostk::astro::trajectory::models::Static ;

    if (!aPosition.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Position") ;
    }

    return Trajectory(Static(aPosition)) ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Trajectory::Trajectory                      ( )
                                :   modelUPtr_(nullptr)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
