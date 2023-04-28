////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/GenericPropagator.cpp
/// @author         Vishwa Shah <vishwa@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/GenericPropagator.hpp>

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

using ostk::core::types::Size ;
using ostk::core::types::Unique ;

static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                GenericPropagator::GenericPropagator        (   const   Dynamics&                   aDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver                            )
                                :   dynamicsUPtr_(aDynamics.clone()),
                                    numericalSolver_(aNumericalSolver)
{

}

// GenericPropagator*              GenericPropagator::clone                    ( ) const
// {
//     return new GenericPropagator(*this) ;
// }

// std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
//                                                                                 const   GenericPropagator&          aGenericPropagator                          )
// {

//     aGenericPropagator.print(anOutputStream) ;

//     return anOutputStream ;

// }

bool                            GenericPropagator::isDefined                ( ) const
{
    return dynamicsUPtr_->isDefined() && numericalSolver_.isDefined() ;
}

AbstractState                   GenericPropagator::calculateStateAt         (   const   AbstractState&              aState,
                                                                                const   Instant&                    anInstant                                   ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("GenericPropagator") ;
    }

    const std::vector<double> stateCoordinates = aState.getCoordinates() ;

    Dynamics::StateVector startStateVector(stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size()) ;

    dynamicsUPtr_->setInstant(aState.getInstant()) ;

    Dynamics::StateVector endStateVector = numericalSolver_.integrateStateFromInstantToInstant(startStateVector, aState.getInstant(), anInstant, dynamicsUPtr_->getDynamicalEquations()) ;

    return { anInstant, endStateVector } ;

}

Array<AbstractState>            GenericPropagator::calculateStatesAt        (   const   AbstractState&              aState,
                                                                                const   Array<Instant>&             anInstantArray                              ) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("GenericPropagator") ;
    }

    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    if (anInstantArray.isEmpty())
    {
        return Array<AbstractState>::Empty() ;
    }

    for (Size k = 0 ; k < anInstantArray.getSize() - 1; ++k)
    {

        if (anInstantArray[k] > anInstantArray[k + 1])
        {
            throw ostk::core::error::runtime::Wrong("Unsorted Instant Array") ;
        }

    }

    const std::vector<double> stateCoordinates = aState.getCoordinates() ;
    Dynamics::StateVector startStateVector(stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size()) ;

    dynamicsUPtr_->setInstant(aState.getInstant()) ;

    Array<Instant> forwardInstants ;
    Array<Instant> backwardInstants ;

    for (const Instant& anInstant : anInstantArray)
    {

        if (anInstant <= aState.getInstant())
        {
            backwardInstants.add(anInstant) ;
        }
        else
        {
            forwardInstants.add(anInstant) ;
        }

    }

    // forward propagation only
    Array<Dynamics::StateVector> propagatedForwardStateVectorArray ;
    if (!forwardInstants.isEmpty())
    {

        propagatedForwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants
        (
            startStateVector,
            aState.getInstant(),
            forwardInstants,
            dynamicsUPtr_->getDynamicalEquations()
        ) ;

    }

    // backward propagation only
    Array<Dynamics::StateVector> propagatedBackwardStateVectorArray ;
    if (!backwardInstants.isEmpty())
    {

        std::reverse(backwardInstants.begin(), backwardInstants.end()) ;

        propagatedBackwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants
        (
            startStateVector,
            aState.getInstant(),
            backwardInstants,
            dynamicsUPtr_->getDynamicalEquations()
        ) ;

        std::reverse(propagatedBackwardStateVectorArray.begin(), propagatedBackwardStateVectorArray.end()) ;

    }

    Array<AbstractState> propagatedStates ;
    propagatedStates.reserve(anInstantArray.getSize()) ;

    Size k = 0 ;
    for (const Dynamics::StateVector& stateVector : (propagatedBackwardStateVectorArray + propagatedForwardStateVectorArray))
    {

        AbstractState propagatedState = { anInstantArray[k], stateVector } ;

        propagatedStates.add(propagatedState) ;

        ++k ;

    }

    return propagatedStates;

}

// void                            GenericPropagator::print                    (           std::ostream&               anOutputStream,
//                                                                                         bool                        displayDecorator                            ) const
// {

//     displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "GenericPropagator") : void () ;

//     ostk::core::utils::Print::Separator(anOutputStream, "Satellite Dynamics") ;
//     dynamicsUPtr_->print(anOutputStream, false) ;

//     ostk::core::utils::Print::Separator(anOutputStream, "Numerical Solver") ;
//     numericalSolver_.print(anOutputStream, false) ;

//     displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
