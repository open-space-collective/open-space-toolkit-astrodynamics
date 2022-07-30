////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Static.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

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

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Trajectory") ;
    }

    // Check if array is empty, and if not then sort it before it is looped through
    if (anInstantArray.isEmpty())
    {
        throw ostk::core::error::runtime::Wrong("Empty Instant Array supplied") ;
    }

    Array<std::pair<Instant, size_t>> instantArrayPairs  = Array<std::pair<Instant, size_t>>::Empty() ;
    instantArrayPairs.reserve(anInstantArray.getSize()) ;

    for (size_t i = 0 ; i < anInstantArray.getSize() ; i++)
    {
        instantArrayPairs.push_back(std::make_pair(anInstantArray[i], i));
    }

    // Sort instant array pairs chronologically
    std::sort(instantArrayPairs.begin(), instantArrayPairs.end(), [] (const auto& instantPairLeft, const auto& instantPairRight) { return instantPairLeft.first < instantPairRight.first ; }) ;

    Array<State> stateArraySorted = Array<State>::Empty() ;
    stateArraySorted.reserve(anInstantArray.getSize()) ;

    for (const auto& instantArrayPair : instantArrayPairs)
    {
        stateArraySorted.add(this->getStateAt(instantArrayPair.first)) ;
    }

    Array<std::pair<State, size_t>> stateArrayPairs = Array<std::pair<State, size_t>>::Empty() ;
    stateArrayPairs.reserve(anInstantArray.getSize()) ;

    for (size_t k = 0 ; k < anInstantArray.getSize() ; k++)
    {
        stateArrayPairs.push_back(std::make_pair(stateArraySorted[k], (instantArrayPairs[k]).second));
    }

    std::sort(stateArrayPairs.begin(), stateArrayPairs.end(), [] (const auto& lhs, const auto& rhs) { return lhs.second < rhs.second ; }) ;


    Array<State> stateArrayUnsorted = Array<State>::Empty() ;
    stateArrayUnsorted.reserve(anInstantArray.getSize()) ;

    for (size_t l = 0 ; l < anInstantArray.getSize() ; l++)
    {
        stateArrayUnsorted.add((stateArrayPairs[l]).first);
    }

    return stateArrayUnsorted ;

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
