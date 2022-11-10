////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.cpp
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

using ostk::core::types::Size ;

using ostk::physics::units::Length ;
using ostk::physics::units::Time ;
using ostk::physics::units::Derived ;

static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;
static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Propagated::Propagated                      (   const   SatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver                            )
                                :   Model(),
                                    cachedStateArray_(Array(1, aSatelliteDynamics.getState())),
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
    this->sortAndSanitizeStateArray() ;
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

    return 1 ; // [TBI] With param

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

    const Array<State> propagatedStateArray = this->calculateStatesAt(Array(1, anInstant)) ;

    if (propagatedStateArray.getSize() != 1)
    {
        throw ostk::core::error::runtime::Wrong("Propagated state array size") ;
    }

    return propagatedStateArray[0] ;

}

State                           Propagated::calculateStateAt                (   const   State&                      aState,
                                                                                const   Instant&                    anInstant                                   ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    const Vector3d startPositionCoordinates = aState.getPosition().inUnit(Position::Unit::Meter).accessCoordinates() ;
    const Vector3d startVelocityCoordinates = aState.getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;

    SatelliteDynamics::StateVector startStateVector(6) ;
    startStateVector[0] = startPositionCoordinates[0]; startStateVector[1] = startPositionCoordinates[1]; startStateVector[2] = startPositionCoordinates[2] ;
    startStateVector[3] = startVelocityCoordinates[0]; startStateVector[4] = startVelocityCoordinates[1]; startStateVector[5] = startVelocityCoordinates[2] ;

    SatelliteDynamics::StateVector endStateVector = numericalSolver_.integrateStateFromInstantToInstant(startStateVector, aState.getInstant(), anInstant, satelliteDynamics_.getDynamicalEquations()) ;

    return {anInstant, Position::Meters({ endStateVector[0], endStateVector[1], endStateVector[2] }, gcrfSPtr), Velocity::MetersPerSecond({ endStateVector[3], endStateVector[4], endStateVector[5] }, gcrfSPtr)} ;

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

    Array<Pair<Instant, Size>> instantArrayPairs = Array<Pair<Instant, Size>>::Empty() ;
    instantArrayPairs.reserve(anInstantArray.getSize()) ;

    for (Size i = 0 ; i < anInstantArray.getSize() ; i++)
    {
        instantArrayPairs.push_back(std::make_pair(anInstantArray[i], i)) ;
    }

    // Sort instant array pairs chronologically
    std::sort
    (
        instantArrayPairs.begin(),
        instantArrayPairs.end(),
        [] (const auto& instantPairLeft, const auto& instantPairRight) { return instantPairLeft.first < instantPairRight.first ; }
    ) ;

    // Create an unpaired sorted instant array
    Array<Instant> sortedInstantArray = Array<Instant>::Empty() ;
    sortedInstantArray.reserve(anInstantArray.getSize()) ;

    for (Size i = 0; i < anInstantArray.getSize(); i++)
    {
        sortedInstantArray.push_back(instantArrayPairs[i].first) ;
    }

    Array<Pair<Integer, Duration>> nearestCachedStatePairs = Array<Pair<Integer, Duration>>::Empty() ;
    nearestCachedStatePairs.reserve(sortedInstantArray.getSize()) ;

    // Iterate through sortedInstantArray and cachedStateArray
    for (Size i = 0; i < sortedInstantArray.getSize(); i++)
    {

        // For each desired instant, check where the closest cachedState is
        Array<Duration> cachedStatesDistanceFromInstantArray = Array<Duration>::Empty() ;
        cachedStatesDistanceFromInstantArray.reserve(cachedStateArray_.getSize()) ;
        Array<double> cachedStatesDistanceFromInstantArrayAbs = Array<double>::Empty() ;
        cachedStatesDistanceFromInstantArrayAbs.reserve(cachedStateArray_.getSize()) ;

        for (Size j = 0; j < cachedStateArray_.getSize(); j++)
        {
            cachedStatesDistanceFromInstantArray.add((sortedInstantArray[i] - cachedStateArray_[j].getInstant())) ;
            cachedStatesDistanceFromInstantArrayAbs.add(std::abs((sortedInstantArray[i] - cachedStateArray_[j].getInstant()).inSeconds())) ;
        }

        // Find min value of cachedStatesDistanceFromInstantArray
        const double nearestCachedStateIndex =  std::min_element(cachedStatesDistanceFromInstantArrayAbs.begin(), cachedStatesDistanceFromInstantArrayAbs.end()) - cachedStatesDistanceFromInstantArrayAbs.begin() ;
        const Duration nearestCachedStatePropDuration = cachedStatesDistanceFromInstantArray[nearestCachedStateIndex] ;

        // Add the cachedStateArray index of the nearest cachedState and the propagation duration from that cached state as a pair for each desired instant
        nearestCachedStatePairs.add(std::make_pair(static_cast<int>(nearestCachedStateIndex), nearestCachedStatePropDuration)) ;

    }

    // Group desired instants into subarrays based on where they are located relative to existing cached states: Array<Pair<Index of cachedState, Number of time that cachedState is closed to a desired instant in the desiredInstantArray)
    Array<Pair<Integer, Integer>> duplicateCounterPairs = Array<Pair<Integer, Integer>>::Empty() ;

    // Loop through nearestCachedStatePairs to group instants with duplicate corresponding cached states together
    for (const auto& nearestcachedStatePair : nearestCachedStatePairs)
    {

        if (duplicateCounterPairs.empty() || duplicateCounterPairs.back().first != nearestcachedStatePair.first)
        {
            duplicateCounterPairs.add(std::make_pair(nearestcachedStatePair.first, 1)) ;
        }
        else
        {
            duplicateCounterPairs.back().second++ ;
        }

    }

    // Now that we have grouped instants into arrays to be propagated from the nearest cachedState, we can execute that propagation
    Array<State> sortedStateArray = Array<State>::Empty() ;
    sortedStateArray.reserve(sortedInstantArray.getSize()) ;
    Size anInstantArrayIndexCount = 0 ;

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

        // Obtain array of desired instants around startState by looking through duplicateCounterPair, segregate by positive and negative propagation durations
        Array<Instant> desiredPositiveInstantArray = Array<Instant>::Empty() ;
        desiredPositiveInstantArray.reserve(duplicateCounterPair.second) ;
        Array<Instant> desiredNegativeInstantArray = Array<Instant>::Empty() ;
        desiredNegativeInstantArray.reserve(duplicateCounterPair.second) ;

        for (int i = 0; i < duplicateCounterPair.second; i++)
        {

            Duration nearestCachedStatePropDuration = nearestCachedStatePairs[anInstantArrayIndexCount].second ;

            // Duration is positive or zero put into desiredPositiveInstantArray and if negative put into desiredNegativeInstantArray
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

        // Execute propagation in reverse time
        if (!desiredNegativeInstantArray.empty())
        {

            // Reverse direction of desiredNegativeInstantArray so that integrate_times can integrate it in negative time and reverse chronological order
            std::reverse(desiredNegativeInstantArray.begin(), desiredNegativeInstantArray.end()) ;

            // Call numerical solver to perform propagation
            Array<SatelliteDynamics::StateVector> propagatedNegativeStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants
            (
                startStateVector,
                startInstant,
                desiredNegativeInstantArray,
                satelliteDynamics_.getDynamicalEquations()
            ) ;

            // Create orbital states from integration solution and add them to sortedStateArray by looping backwards through
            for (int k = desiredNegativeInstantArray.getSize() - 1; k >= 0; k--)
            {

                const State propagatedNegativeState =
                {
                    desiredNegativeInstantArray[k],
                    Position::Meters({ propagatedNegativeStateVectorArray[k][0], propagatedNegativeStateVectorArray[k][1], propagatedNegativeStateVectorArray[k][2] }, gcrfSPtr),
                    Velocity::MetersPerSecond({ propagatedNegativeStateVectorArray[k][3], propagatedNegativeStateVectorArray[k][4], propagatedNegativeStateVectorArray[k][5] }, gcrfSPtr)
                } ;

                satelliteDynamics_.setState(propagatedNegativeState) ;

                // Add propagatedNegativeState to sorted StateArray for propagation
                sortedStateArray.add(propagatedNegativeState) ;

            }

        }

        // Execute propagation in forwards time
        if (!desiredPositiveInstantArray.empty())
        {

            // Call numerical solver to perform propagation
            Array<SatelliteDynamics::StateVector> propagatedPositiveStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants
            (
                startStateVector,
                startInstant,
                desiredPositiveInstantArray,
                satelliteDynamics_.getDynamicalEquations()
            ) ;

            // Create orbital states from integration solution and add them to sortedStateArray
            for (Size k = 0; k < desiredPositiveInstantArray.getSize(); k++)
            {

                const State propagatedPositiveState =
                {
                    desiredPositiveInstantArray[k],
                    Position::Meters({ propagatedPositiveStateVectorArray[k][0], propagatedPositiveStateVectorArray[k][1], propagatedPositiveStateVectorArray[k][2] }, gcrfSPtr),
                    Velocity::MetersPerSecond({ propagatedPositiveStateVectorArray[k][3], propagatedPositiveStateVectorArray[k][4], propagatedPositiveStateVectorArray[k][5] }, gcrfSPtr)
                } ;

                satelliteDynamics_.setState(propagatedPositiveState) ;

                sortedStateArray.add(propagatedPositiveState) ;  // Add propagatedPositiveState to sorted StateArray for propagation

            }

        }

    }

    // Desort the sorted state array to return it to user as was requested
    Array<State> unsortedStateArray = Array<State>(anInstantArray.getSize(), State::Undefined()) ;

    for (Size k = 0; k < anInstantArray.getSize(); k++)
    {
        unsortedStateArray[instantArrayPairs[k].second] = sortedStateArray[k] ;
    }

    // Add newly propagated states to the cachedStateArray and then resort
    cachedStateArray_.add(sortedStateArray) ;

    // Re-sort and sanitize cachedStateArray of duplicates and duplicate instants that have different positions/velocities
    this->sortAndSanitizeStateArray() ;

    return unsortedStateArray ;

}

Array<State>                    Propagated::calculateStatesAt               (   const   State&                      aState,
                                                                                const   Array<Instant>&             anInstantArray                              ) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty() ;
    }

    for (Size k = 0 ; k < anInstantArray.getSize() - 1; ++k)
    {
        if (anInstantArray[k] > anInstantArray[k + 1])
        {
            throw ostk::core::error::runtime::Wrong("Unsorted Instant Array") ;
        }
    }

    const Vector3d startPositionCoordinates = aState.getPosition().inUnit(Position::Unit::Meter).accessCoordinates();
    const Vector3d startVelocityCoordinates = aState.getVelocity().inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;

    SatelliteDynamics::StateVector startStateVector(6) ;
    startStateVector[0] = startPositionCoordinates[0]; startStateVector[1] = startPositionCoordinates[1]; startStateVector[2] = startPositionCoordinates[2] ;
    startStateVector[3] = startVelocityCoordinates[0]; startStateVector[4] = startVelocityCoordinates[1]; startStateVector[5] = startVelocityCoordinates[2] ;

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
    Array<SatelliteDynamics::StateVector> propagatedForwardStateVectorArray ;
    if (!forwardInstants.isEmpty())
    {

        propagatedForwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants
        (
            startStateVector,
            aState.getInstant(),
            forwardInstants,
            satelliteDynamics_.getDynamicalEquations()
        ) ;

    }
    
    // backward propagation only
    Array<SatelliteDynamics::StateVector> propagatedBackwardStateVectorArray ;
    if (!backwardInstants.isEmpty())
    {

        std::reverse(backwardInstants.begin(), backwardInstants.end()) ;

        propagatedBackwardStateVectorArray = numericalSolver_.integrateStatesAtSortedInstants
        (
            startStateVector,
            aState.getInstant(),
            backwardInstants,
            satelliteDynamics_.getDynamicalEquations()
        ) ;

        std::reverse(propagatedBackwardStateVectorArray.begin(), propagatedBackwardStateVectorArray.end()) ;

    }
    

    Array<State> propagatedStates ;
    propagatedStates.reserve(anInstantArray.getSize()) ;

    Size k = 0 ;
    for (const SatelliteDynamics::StateVector& stateVector : (propagatedBackwardStateVectorArray + propagatedForwardStateVectorArray))
    {

        State propagatedState = {anInstantArray[k], Position::Meters( {stateVector[0], stateVector[1], stateVector[2] }, gcrfSPtr), Velocity::MetersPerSecond({ stateVector[3], stateVector[4], stateVector[5] }, gcrfSPtr)} ;

        propagatedStates.add(propagatedState) ;

        ++k ;

    }


    return propagatedStates;
}

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

    // Calculate gravitational parameter (Spherical earth has the most modern value which is the correct one)
    using ostk::physics::env::obj::celest::Earth ;

    const Derived gravitationalParameter = Earth::Models::Spherical::GravitationalParameter ;
    const Real gravitationalParameter_SI = gravitationalParameter.in(GravitationalParameterSIUnit) ;

    // Obtain state at position of cachedStateArray
    Instant currentInstant = cachedStateArray_[0].getInstant() ;
    Position currentPosition = cachedStateArray_[0].getPosition() ;
    Velocity currentVelocity = cachedStateArray_[0].getVelocity() ;

    Vector3d currentPositionCoordinates = currentPosition.inUnit(Position::Unit::Meter).accessCoordinates() ;
    Vector3d currentVelocityCoordinates = currentVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates() ;

    SatelliteDynamics::StateVector currentStateVector(6) ;
    currentStateVector[0] = currentPositionCoordinates[0]; currentStateVector[1] = currentPositionCoordinates[1]; currentStateVector[2] = currentPositionCoordinates[2] ;
    currentStateVector[3] = currentVelocityCoordinates[0]; currentStateVector[4] = currentVelocityCoordinates[1]; currentStateVector[5] = currentVelocityCoordinates[2] ;

    // Determine whether to count revolution numbers in forwards or backwards time and return function if duration is 0
    const double durationInSecs = (anInstant - currentInstant).inSeconds() ;
    if (durationInSecs == 0.0) { return 1 ; }
    Integer durationSign = (durationInSecs > 0) - (durationInSecs < 0) ;

    // Propagate towards desired instant a fraction of an orbit at a time in while loop, exit when arrived at desired instant
    Integer revolutionNumber = 0 ;
    while (true)
    {

        // Calculate orbital period
        const double semiMajorAxis = - gravitationalParameter_SI * currentPositionCoordinates.norm() / (currentPositionCoordinates.norm() * std::pow(currentVelocityCoordinates.norm(), 2) - 2.0 * gravitationalParameter_SI);
        const Duration orbitalPeriod = Duration::Seconds(Real::TwoPi() * std::sqrt(std::pow(semiMajorAxis, 3) / gravitationalParameter_SI)) ;

        // If we have passed the desired instant during our progration, break from the loop
        if (durationSign.isPositive() && currentInstant > anInstant) break ;
        if (durationSign.isNegative() && currentInstant < anInstant) break ;

        // Increase or decrease revolution number by 1
        revolutionNumber += durationSign ;

        // Propagate for duration of this orbital period
        currentStateVector = numericalSolver_.integrateStateForDuration(currentStateVector, durationSign * orbitalPeriod, satelliteDynamics_.getDynamicalEquations()) ;

        // Update the current instant position and velocity coordinates
        currentInstant += durationSign * orbitalPeriod ;
        currentPositionCoordinates = { currentStateVector[0], currentStateVector[1], currentStateVector[2] } ;
        currentVelocityCoordinates = { currentStateVector[3], currentStateVector[4], currentStateVector[5] } ;

    }

    return revolutionNumber ;

}

const Array<State>&             Propagated::accessCachedStateArray          ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    return cachedStateArray_ ;

}

void                            Propagated::print                           (       std::ostream&                   anOutputStream,
                                                                                    bool                            displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propagated") : void () ;

    ostk::core::utils::Print::Separator(anOutputStream, "Cached State Array") ;
    [&] (const Array<State>&) -> void { for(State iterState:cachedStateArray_) {iterState.print(anOutputStream, false) ;} ; } ;

    ostk::core::utils::Print::Separator(anOutputStream, "Satellite Dynamics") ;
    satelliteDynamics_.print(anOutputStream, false) ;

    ostk::core::utils::Print::Separator(anOutputStream, "Numerical Solver") ;
    numericalSolver_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

Propagated                      Propagated::MediumFidelity                  (   const   State&                      aState                                      )
{

    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

    using ostk::physics::Environment ;
    using ostk::physics::units::Mass ;
    using ostk::physics::env::Object ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::celest::Sun ;
    using ostk::physics::env::obj::celest::Moon ;

    using ostk::astro::flight::system::SatelliteSystem ;

    // Create environment
    const Array<Shared<Object>> objects = // [TBI] Add drag to environment once it is added in ostk-physics
    {
        std::make_shared<Earth>(Earth::EGM2008(35, 35))
    } ;

    const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

    // Satellite system setup
    const Composite satelliteGeometry = Composite { Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } ) } ;
    const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

    // Satellite dynamics setup
    const SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, aState } ;

    // Construct default numerical solver
    const NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

    return { satelliteDynamics, numericalSolver } ;

}

Propagated                      Propagated::HighFidelity                    (   const   State&                      aState                                      )
{

    using ostk::math::obj::Matrix3d ;
    using ostk::math::geom::d3::objects::Cuboid ;
    using ostk::math::geom::d3::objects::Composite ;
    using ostk::math::geom::d3::objects::Point ;

    using ostk::physics::Environment ;
    using ostk::physics::units::Mass ;
    using ostk::physics::env::Object ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::celest::Sun ;
    using ostk::physics::env::obj::celest::Moon ;

    using ostk::astro::flight::system::SatelliteSystem ;

    // Create environment
    const Array<Shared<Object>> objects = // [TBI] Add drag to environment once it is added in ostk-physics
    {
        std::make_shared<Earth>(Earth::EGM2008(100, 100)),
        std::make_shared<Sun>(Sun::Spherical()),
        std::make_shared<Moon>(Moon::Spherical())
    } ;

    const Environment customEnvironment = Environment(Instant::J2000(), objects) ;

    // Satellite system setup
    const Composite satelliteGeometry = Composite { Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } ) } ;
    const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

    // Satellite dynamics setup
    const SatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem, aState } ;

    // Construct default numerical solver
    const NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

    return { satelliteDynamics, numericalSolver } ;

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

void                            Propagated::sortAndSanitizeStateArray                  ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    if (cachedStateArray_.getSize() == 1)
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

        // Check to see if there are any duplicated instants with different positions and velocities
        Array<State> cachedStateArrayUnique(cachedStateArray_) ;
        cachedStateArrayUnique.erase(std::unique(cachedStateArrayUnique.begin(), cachedStateArrayUnique.end(), [] (const auto& lhs, const auto& rhs) { return lhs.getInstant() == rhs.getInstant() ; }), cachedStateArrayUnique.end() ) ;

        if (cachedStateArray_.getSize() != cachedStateArrayUnique.getSize())
        {
            throw ostk::core::error::runtime::Wrong("State array with States at same instant but different position/velocity were found in cachedStateArray") ;
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
