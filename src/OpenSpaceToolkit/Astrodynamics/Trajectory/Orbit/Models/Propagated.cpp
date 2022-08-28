////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace models
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using ostk::physics::units::Length ;
using ostk::physics::units::Time ;
using ostk::physics::units::Derived ;

static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;
static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Propagated::Propagated                      (   const   SatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver                            )
                                :   Model(),
                                    cachedStateArray_(Array(1,aSatelliteDynamics.getState())),
                                    satelliteDynamics_(aSatelliteDynamics),
                                    numericalSolver_(aNumericalSolver)

{

}

                                Propagated::Propagated                      (   const   SatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver,
                                                                                const   Array<State>&               aCachedStateArray                           )
                                :   Model(),
                                    cachedStateArray_(aCachedStateArray),
                                    satelliteDynamics_(aSatelliteDynamics),
                                    numericalSolver_(aNumericalSolver)

{
    this->sortStateArray() ;
}

Propagated*                     Propagated::clone                           ( ) const
{
    return new Propagated(*this) ;
}

bool                            Propagated::operator ==                     (   const   Propagated&                 aPropagatedModel                            ) const
{

    if ((!this->isDefined()) || (!aPropagatedModel.isDefined()))
    {
        return false ;
    }

    return (cachedStateArray_ == aPropagatedModel.cachedStateArray_)
        && (satelliteDynamics_ == aPropagatedModel.satelliteDynamics_)
        && (numericalSolver_ == aPropagatedModel.numericalSolver_) ;

}

bool                            Propagated::operator !=                     (   const   Propagated&                 aPropagatedModel                            ) const
{
    return !((*this) == aPropagatedModel) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Propagated&                 aPropagatedModel                            )
{

    aPropagatedModel.print(anOutputStream) ;

    return anOutputStream ;

}

void                            Propagated::print                           (       std::ostream&                   anOutputStream,
                                                                                    bool                            displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propagated") : void () ;

    ostk::core::utils::Print::Separator(anOutputStream, "Cached State Array:") ;
    [&] (const Array<State>&) -> void { for(State iterState:cachedStateArray_) {iterState.print(anOutputStream, false) ;} ; } ;

    ostk::core::utils::Print::Separator(anOutputStream, "SatelliteDynamics") ;
    satelliteDynamics_.print(anOutputStream, false) ;

    ostk::core::utils::Print::Separator(anOutputStream, "NumericalSolver") ;
    numericalSolver_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

bool                            Propagated::isDefined                       ( ) const
{
    return !cachedStateArray_.isEmpty() && satelliteDynamics_.isDefined() && numericalSolver_.isDefined() ;
}

Instant                         Propagated::getEpoch                        ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    return cachedStateArray_[0].getInstant() ;

}

Integer                         Propagated::getRevolutionNumberAtEpoch      ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    return 1 ;

}

State                           Propagated::calculateStateAt                (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    // Logic to determine from which to state in the cache to propagate to the desired instant
    State startState = State::Undefined() ;
    Duration propDuration = Duration::Undefined() ;
    Array<Duration> absoluteDurationDifferenceArray = Array<Duration>::Empty() ;
    Integer propagatedStateInsertIndex = Integer::Undefined() ;

    // If cached state array has length 1 and the supplied instant is the same as the one in the cache, return the one in the cache
    if (cachedStateArray_.getSize() == 1 && cachedStateArray_[0].getInstant() == anInstant)
    {
        // std::cout << "same state as already exists" << std::endl ;
        return cachedStateArray_[0] ;
    }
    // If instant is within bounds of cached state array, find which state is closest to the instant and propagate forwards/backwards from that one
    else if ( (anInstant >= cachedStateArray_.accessFirst().getInstant()) && (anInstant <= cachedStateArray_.accessLast().getInstant()) && (cachedStateArray_.getSize() > 1) )
    {
        absoluteDurationDifferenceArray.reserve(cachedStateArray_.getSize()) ;

        for (size_t i = 0; i < cachedStateArray_.getSize(); i++) // Find the indices and duration differences of the two closest state instants in the cache
        {
            if (cachedStateArray_[i].getInstant() == anInstant)
            {
                // std::cout << "same state as already exists" << std::endl ;
                return cachedStateArray_[i] ;
            }
            else
            {
                absoluteDurationDifferenceArray.add(anInstant - cachedStateArray_[i].getInstant()) ;

                if (!absoluteDurationDifferenceArray.accessLast().isPositive())
                {
                    break ;
                }
            }
        }
        // If an instant is closer to last array element, propagate backwards from that one
        if (absoluteDurationDifferenceArray.accessLast().getAbsolute().inSeconds() < absoluteDurationDifferenceArray[absoluteDurationDifferenceArray.getSize()-2].getAbsolute().inSeconds())
        {
            propDuration = anInstant - cachedStateArray_[absoluteDurationDifferenceArray.getSize()-1].getInstant() ;
            startState = cachedStateArray_[absoluteDurationDifferenceArray.getSize()-1] ;
        }
        // If an instant is closer to the second last array element, propagate forwards from that one
        else
        {
            propDuration = anInstant - cachedStateArray_[absoluteDurationDifferenceArray.getSize()-2].getInstant() ;
            startState = cachedStateArray_[absoluteDurationDifferenceArray.getSize()-2] ;
        }

        propagatedStateInsertIndex = absoluteDurationDifferenceArray.getSize()-1 ; // Insert between second to last and last index of absoluteDurationDifferenceArray

    }
    // If instant is past last state in cache, propagate forwards
    else if (anInstant > cachedStateArray_.accessLast().getInstant())
    {
        propDuration = anInstant - cachedStateArray_.accessLast().getInstant() ;
        startState = cachedStateArray_.accessLast() ;
        absoluteDurationDifferenceArray.mergeWith(Array((size_t)cachedStateArray_.getSize(),Duration::Seconds(1.0))) ;
        propagatedStateInsertIndex = absoluteDurationDifferenceArray.getSize() ; // Insert at end of state array
    }
    // If instant is before first state in cache, propagate backwards
    else if (anInstant < cachedStateArray_.accessFirst().getInstant())
    {
        propDuration = anInstant - cachedStateArray_.accessFirst().getInstant() ;
        startState = cachedStateArray_.accessFirst() ;
        propagatedStateInsertIndex = 0 ; // Insert at beginning of state array
    }
    else
    {
        throw ostk::core::error::runtime::Undefined("Propagation Duration") ;
    }

    // std::cout << "propDuation: " << propDuration << std::endl ;
    // std::cout << "startState: " << startState << std::endl ;

    // [TBI]: Could potentially implement a way to check to see if enough instants are being requested at too small intervals to warn about outputted accuracy

    SatelliteDynamics::StateVector startStateVector(6) ;
    const Vector3d startPositionCoordinates = (startState.getPosition()).inUnit(Position::Unit::Meter).accessCoordinates() ;
    const Vector3d startVelocityCoordinates = (startState.getVelocity()).inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;

    startStateVector[0] = startPositionCoordinates[0]; startStateVector[1] = startPositionCoordinates[1]; startStateVector[2] = startPositionCoordinates[2] ;
    startStateVector[3] = startVelocityCoordinates[0]; startStateVector[4] = startVelocityCoordinates[1]; startStateVector[5] = startVelocityCoordinates[2] ;

    // Call numerical solver to perform propagation
    SatelliteDynamics::StateVector propagatedStateVector = numericalSolver_.integrateStateForDuration(startStateVector, propDuration, satelliteDynamics_.getDynamicalEquations()) ;

    const State propagatedState = { anInstant, Position::Meters({ propagatedStateVector[0], propagatedStateVector[1], propagatedStateVector[2] }, gcrfSPtr), Velocity::MetersPerSecond({ propagatedStateVector[3], propagatedStateVector[4], propagatedStateVector[5] }, gcrfSPtr) } ;
    satelliteDynamics_.setState(propagatedState) ;

    // Add propagated state to the cache
    cachedStateArray_.insert(cachedStateArray_.begin() + propagatedStateInsertIndex, propagatedState) ;

    return propagatedState ;

}

Array<State>                    Propagated::calculateStatesAt               (   const   Array<Instant>&             anInstantArray                              ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty() ;
    }

    Array<Pair<Instant, size_t>> instantArrayPairs = Array<Pair<Instant, size_t>>::Empty() ;
    instantArrayPairs.reserve(anInstantArray.getSize()) ;

    for (size_t i = 0 ; i < anInstantArray.getSize() ; i++)
    {
        instantArrayPairs.push_back(std::make_pair(anInstantArray[i], i)) ;
    }

    // Sort instant array pairs chronologically
    std::sort(instantArrayPairs.begin(), instantArrayPairs.end(), [] (const auto& instantPairLeft, const auto& instantPairRight) { return instantPairLeft.first < instantPairRight.first ; }) ;

    // Create an unpaired sorted instant array
    Array<Instant> sortedInstantArray = Array<Instant>::Empty() ;
    sortedInstantArray.reserve(anInstantArray.getSize()) ;

    for (size_t j = 0 ; j < anInstantArray.getSize() ; j++)
    {
        sortedInstantArray.push_back(instantArrayPairs[j].first) ;
    }

    Array<Pair<Integer, Duration>> nearestCachedStatePairs = Array<Pair<Integer, Duration>>::Empty() ;
    // TODO go through and add reserve words

    // Iterate through sortedInstantArray and cachedStateArray
    for (size_t i = 0; i < sortedInstantArray.getSize(); i++)
    {

        // For each desired instant, check where the closest cachedState is
        Array<Duration> cachedStatesDistanceFromInstantArray = Array<Duration>::Empty() ;
        Array<double> cachedStatesDistanceFromInstantArrayAbs = Array<double>::Empty() ;
        for (size_t j = 0; j < cachedStateArray_.getSize(); j++)
        {
            cachedStatesDistanceFromInstantArray.add((sortedInstantArray[i] - cachedStateArray_[j].getInstant())) ;
            cachedStatesDistanceFromInstantArrayAbs.add(std::abs((sortedInstantArray[i] - cachedStateArray_[j].getInstant()).inSeconds())) ;
        }

        // Find min value of cachedStatesDistanceFromInstantArray
        const auto nearestCachedStateIndex =  std::min_element(cachedStatesDistanceFromInstantArrayAbs.begin(), cachedStatesDistanceFromInstantArrayAbs.end()) - cachedStatesDistanceFromInstantArrayAbs.begin() ;
        const Duration nearestCachedStatePropDuration = cachedStatesDistanceFromInstantArray[nearestCachedStateIndex] ;

        // Add the cachedStateArray index of the nearest cachedState and the propagation duration from that cached state as a pair for each desired instant
        nearestCachedStatePairs.add(std::make_pair((int)nearestCachedStateIndex, nearestCachedStatePropDuration)) ;

    }

    // Group desired instants into subarrays based on where they are located relative to existing cached states
    Array<Pair<Integer, Integer>> duplicateCounterPairs = Array<Pair<Integer, Integer>>::Empty() ;

    // Loop through nearestCachedStatePairs to group instants with duplicate corresponding cached states together
    for (const auto& nearestcachedStatePair : nearestCachedStatePairs)
    {
        if (duplicateCounterPairs.empty() || duplicateCounterPairs.back().first != nearestcachedStatePair.first)
        {
                duplicateCounterPairs.add(std::make_pair(nearestcachedStatePair.first, 1));
        }
        else
        {
                duplicateCounterPairs.back().second++;
        }

        // std::cout << "nearestCachedStatePairs first: " << nearestcachedStatePair.first << std::endl ;
        // std::cout << "nearestCachedStatePairs second: " << nearestcachedStatePair.second << std::endl ;
    }

    // for (const auto& duplicateCounterPair : duplicateCounterPairs)
    // {
    //     std::cout << "duplicateCounterPairs first: " << duplicateCounterPair.first << std::endl ;
    //     std::cout << "duplicateCounterPairs second: " << duplicateCounterPair.second << std::endl ;
    // }

    // Now that we have grouped instants into arrays to be propagated from the nearest cachedState, can execute that propagation
    Array<State> sortedStateArray = Array<State>::Empty() ;

    size_t anInstantArrayIndexCount = 0 ;
    for (const auto& duplicateCounterPair : duplicateCounterPairs)
    {
        // Find starting state for each instant array of desired instants
        const State startState = cachedStateArray_[duplicateCounterPair.first] ;
        const Instant startInstant = startState.getInstant() ;

        // Convert starting state into raw format for NumericalSolver to use
        const Vector3d startPositionCoordinates = startState.getPosition().inUnit(Position::Unit::Meter).accessCoordinates() ;
        const Vector3d startVelocityCoordinates = startState.getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;

        SatelliteDynamics::StateVector startStateVector(6) ;
        startStateVector[0] = startPositionCoordinates[0]; startStateVector[1] = startPositionCoordinates[1]; startStateVector[2] = startPositionCoordinates[2] ;
        startStateVector[3] = startVelocityCoordinates[0]; startStateVector[4] = startVelocityCoordinates[1]; startStateVector[5] = startVelocityCoordinates[2] ;

        // Obtain array of desired instants around startState by looking through duplicateCounterPair
        Array<Instant> desiredPositiveInstantArray = Array<Instant>::Empty() ;
        Array<Instant> desiredNegativeInstantArray = Array<Instant>::Empty() ;

        for (int i = 0; i < duplicateCounterPair.second; i++)
        {
            Duration nearestCachedStatePropDuration = nearestCachedStatePairs[anInstantArrayIndexCount].second ;

            if (nearestCachedStatePropDuration.isPositive())
            {
                desiredPositiveInstantArray.add(startInstant + nearestCachedStatePropDuration) ;
            }
            else
            {
                desiredNegativeInstantArray.add(startInstant + nearestCachedStatePropDuration) ;
            }

            anInstantArrayIndexCount++ ;
        }

        // std::cout << "desiredPositiveInstantArray\n" << desiredPositiveInstantArray << std::endl ;
        // std::cout << "desiredNegativeInstantArray\n" << desiredNegativeInstantArray << std::endl ;

        Array<Array<Instant>> desiredInstantNestedArray = Array<Array<Instant>>::Empty() ;
        if (!desiredNegativeInstantArray.empty() && !desiredPositiveInstantArray.empty())
        {
            desiredInstantNestedArray.add(desiredNegativeInstantArray) ;
            desiredInstantNestedArray.add(desiredPositiveInstantArray) ;
        }
        else if (desiredNegativeInstantArray.empty())
        {
            desiredInstantNestedArray.add(desiredPositiveInstantArray) ;
        }
        else if (desiredPositiveInstantArray.empty())
        {
            desiredInstantNestedArray.add(desiredNegativeInstantArray) ;
        }
        else
        {
            throw ostk::core::error::RuntimeError("There are no instants in the desiredInstantArray") ;
        }

        for (const auto& desiredInstantArray : desiredInstantNestedArray)
        {
            // Call numerical solver to perform propagation
            Array<SatelliteDynamics::StateVector> propagatedStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants(startStateVector, startInstant, desiredInstantArray, satelliteDynamics_.getDynamicalEquations()) ;

            for (size_t k = 0; k < desiredInstantArray.getSize(); k++)
            {
                // std::cout << desiredInstantArray << std::endl ;
                // std::cout << "desiredInstantArray size: " << desiredInstantArray.getSize() << std::endl ;
                // std::cout << "propagatedStateVectorArray size: " << propagatedStateVectorArray.getSize() << std::endl ;

                // std::cout << desiredInstantNestedArray << std::endl ;

                const State propagatedState = { desiredInstantArray[k], Position::Meters({ propagatedStateVectorArray[k][0], propagatedStateVectorArray[k][1], propagatedStateVectorArray[k][2] }, gcrfSPtr), Velocity::MetersPerSecond({ propagatedStateVectorArray[k][3], propagatedStateVectorArray[k][4], propagatedStateVectorArray[k][5] }, gcrfSPtr) } ;
                // std::cout << propagatedState << std::endl ;
                satelliteDynamics_.setState(propagatedState) ;

                // Add propagatedState to sorted StateArray for propagation
                sortedStateArray.add(propagatedState) ;

                // for (size_t l = 0; l < cachedStateArray_.getSize(); l++)
                // {
                //     // Loop through cachedStateArray until you find a state after the propagated state, and insert the propagated state at that position and then break from the loop
                //     if (cachedStateArray_[l].getInstant() > propagatedState.getInstant())
                //     {
                //         cachedStateArray_.insert(cachedStateArray_.begin() + l, propagatedState) ;
                //         break ;
                //     }
                // }

            }
        }
    }

    // Desort the sorted state array to return it to user as was requested
    Array<State> unsortedStateArray = Array<State>(anInstantArray.getSize(), State::Undefined()) ;
    unsortedStateArray.reserve(anInstantArray.getSize()) ;

    for (size_t k = 0 ; k < anInstantArray.getSize() ; k++)
    {
        unsortedStateArray[instantArrayPairs[k].second] = sortedStateArray[k] ;
    }

    // Add newly propagated states to the cachedStateArray and then resort
    cachedStateArray_.add(sortedStateArray) ;
    std::sort(cachedStateArray_.begin(), cachedStateArray_.end(), [] (const auto& lhs, const auto& rhs) { return lhs.getInstant() < rhs.getInstant() ; }) ;

    // Remove duplicate states from sortedStateArray in case any were recently added during this run of desired instants
    cachedStateArray_.erase(std::unique(cachedStateArray_.begin(), cachedStateArray_.end()), cachedStateArray_.end()) ;

    return unsortedStateArray ;

}

// [TBI] add a more exact calculation of the orbital period by checking for when the sat goes above the x-y plane in GCRF
Integer                         Propagated::calculateRevolutionNumberAt     (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    if (anInstant == cachedStateArray_[0].getInstant())
    {
        return this->getRevolutionNumberAtEpoch() ;
    }

    const double gravitationalParameter_SI = 3.986004418e14 ; // [TBI] will find a way to incorporate the real graviational parameter from the shared celestial object pointer later
    // const double mu_SI = ((environment_.accessCelestialObjectWithName("Earth"))->getGravitationalParameter()).in(GravitationalParameterSIUnit) ;

    // Get state related parameters
    Position currentPosition = cachedStateArray_[0].getPosition();
    Velocity currentVelocity = cachedStateArray_[0].getVelocity();

    Vector3d currentPositionCoords = currentPosition.inUnit(Position::Unit::Meter).accessCoordinates();
    Vector3d currentVelocityCoords = currentVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

    const Real semiMajorAxis = - gravitationalParameter_SI * currentPositionCoords.norm() / (currentPositionCoords.norm() * std::pow(currentVelocityCoords.norm(),2) - 2.0 * gravitationalParameter_SI);

    const Duration orbitalPeriod = Duration::Seconds(Real::TwoPi() * std::sqrt(std::pow(semiMajorAxis,3) / gravitationalParameter_SI)) ;

    const Duration durationFromEpoch = Duration::Between(cachedStateArray_[0].getInstant(), anInstant) ;

    return (durationFromEpoch.inSeconds() / orbitalPeriod.inSeconds()).floor() + 1 ;

}

const Array<State>&             Propagated::accessCachedStateArray          ( ) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    return cachedStateArray_ ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool                            Propagated::operator ==                     (   const   trajectory::Model&          aModel                                      ) const
{

    const Propagated* propagatedModelPtr = dynamic_cast<const Propagated*>(&aModel) ;

    return (propagatedModelPtr != nullptr) && this->operator == (*propagatedModelPtr) ;

}

bool                            Propagated::operator !=                     (   const   trajectory::Model&          aModel                                      ) const
{
    return !((*this) == aModel) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            Propagated::sortStateArray                  ( )
{
    // Check if array is sorted, and if not then sort it
    if (cachedStateArray_.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Cached State Array") ;
    }
    else if (cachedStateArray_.getSize() == 1)
    {
        satelliteDynamics_.setState(cachedStateArray_[0]) ;
    }
    else
    {
        // Add state contained in satellite dynamics to the end of the array
        cachedStateArray_.add(satelliteDynamics_.getState()) ;

        // Sort cashed state array and remove duplicate states
        std::sort(cachedStateArray_.begin(), cachedStateArray_.end(), [] (const auto& lhs, const auto& rhs) { return lhs.getInstant() < rhs.getInstant() ; }) ;

        cachedStateArray_.erase(std::unique(cachedStateArray_.begin(), cachedStateArray_.end()), cachedStateArray_.end()) ;

        // Check to see if there are any duplicate instants with different positions and velocities
        Array<State> cachedStateArrayUnique(cachedStateArray_) ;
        cachedStateArrayUnique.erase(std::unique(cachedStateArrayUnique.begin(), cachedStateArrayUnique.end(), [] (const auto& lhs, const auto& rhs) { return lhs.getInstant() == rhs.getInstant() ; }), cachedStateArrayUnique.end() ) ;

        if (cachedStateArray_.getSize() != cachedStateArrayUnique.getSize())
        {
            throw ostk::core::error::runtime::Wrong("State array with States at same instant but different coordinates supplied") ;
        }

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
