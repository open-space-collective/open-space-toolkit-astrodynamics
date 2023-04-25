////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/FullPropagator.cpp
/// @author         Vishwa Shah <vishwa@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Full/FullPropagator.hpp>

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

static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                FullPropagator::FullPropagator                      (   const   FullSatelliteDynamics&          aSatelliteDynamics,
                                                                                const   NumericalSolver&            aNumericalSolver                            )
                                :   satelliteDynamics_(aSatelliteDynamics),
                                    numericalSolver_(aNumericalSolver)

{

}

FullPropagator*                     FullPropagator::clone                           ( ) const
{
    return new FullPropagator(*this) ;
}

bool                            FullPropagator::operator ==                     (   const   FullPropagator&                 aFullPropagator                                 ) const
{

    if ((!this->isDefined()) || (!aFullPropagator.isDefined()))
    {
        return false ;
    }

    return (satelliteDynamics_ == aFullPropagator     .satelliteDynamics_) && (numericalSolver_ == aFullPropagator     .numericalSolver_) ;

}

bool                            FullPropagator::operator !=                     (   const   FullPropagator&                 aFullPropagator                                 ) const
{
    return !((*this) == aFullPropagator     ) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   FullPropagator&                 aFullPropagator                                 )
{

    aFullPropagator     .print(anOutputStream) ;

    return anOutputStream ;

}

bool                            FullPropagator::isDefined                       ( ) const
{
    return satelliteDynamics_.isDefined() && numericalSolver_.isDefined() ;
}

FullState                           FullPropagator::calculateStateAt                (   const   FullState&                      aState,
                                                                                const   Instant&                    anInstant                                   ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("FullPropagator") ;
    }

    const VectorXd stateCoordinates = aState.getCoordinates() ;

    FullSatelliteDynamics::StateVector startStateVector(stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size()) ;

    satelliteDynamics_.setInstant(aState.getInstant()) ;

    FullSatelliteDynamics::StateVector endStateVector = numericalSolver_.integrateStateFromInstantToInstant(startStateVector, aState.getInstant(), anInstant, satelliteDynamics_.getDynamicalEquations()) ;

    return {
        anInstant,
        Position::Meters({ endStateVector[0], endStateVector[1], endStateVector[2] }, gcrfSPtr),
        Velocity::MetersPerSecond({ endStateVector[3], endStateVector[4], endStateVector[5] }, gcrfSPtr),
        Quaternion::XYZS(endStateVector[6], endStateVector[7], endStateVector[8], endStateVector[9]),
        Vector3d({ endStateVector[10], endStateVector[11], endStateVector[12 ]}),
        gcrfSPtr
    } ;

}

Array<FullState>                    FullPropagator::calculateStatesAt           (   const   FullState&                      aState,
                                                                                const   Array<Instant>&             anInstantArray                              ) const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("FullPropagator") ;
    }

    if (!aState.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("State") ;
    }

    if (anInstantArray.isEmpty())
    {
        return Array<FullState>::Empty() ;
    }

    for (Size k = 0 ; k < anInstantArray.getSize() - 1; ++k)
    {

        if (anInstantArray[k] > anInstantArray[k + 1])
        {
            throw ostk::core::error::runtime::Wrong("Unsorted Instant Array") ;
        }

    }

    const VectorXd stateCoordinates = aState.getCoordinates() ;
    FullSatelliteDynamics::StateVector startStateVector(stateCoordinates.data(), stateCoordinates.data() + stateCoordinates.size()) ;

    satelliteDynamics_.setInstant(aState.getInstant()) ;

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
    Array<FullSatelliteDynamics::StateVector> propagatedForwardStateVectorArray ;
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
    Array<FullSatelliteDynamics::StateVector> propagatedBackwardStateVectorArray ;
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

    Array<FullState> propagatedStates ;
    propagatedStates.reserve(anInstantArray.getSize()) ;

    Size k = 0 ;
    for (const FullSatelliteDynamics::StateVector& stateVector : (propagatedBackwardStateVectorArray + propagatedForwardStateVectorArray))
    {

        FullState propagatedState = {
            anInstantArray[k],
            Position::Meters( {stateVector[0], stateVector[1], stateVector[2] }, gcrfSPtr),
            Velocity::MetersPerSecond({ stateVector[3], stateVector[4], stateVector[5] }, gcrfSPtr),
            Quaternion::XYZS(stateVector[6], stateVector[7], stateVector[8], stateVector[9]),
            Vector3d({ stateVector[10], stateVector[11], stateVector[12 ]}),
            gcrfSPtr
        } ;

        propagatedStates.add(propagatedState) ;

        ++k ;

    }

    return propagatedStates;

}

void                            FullPropagator::print                          (    std::ostream&                   anOutputStream,
                                                                                    bool                            displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "FullPropagator") : void () ;

    ostk::core::utils::Print::Separator(anOutputStream, "Satellite Dynamics") ;
    satelliteDynamics_.print(anOutputStream, false) ;

    ostk::core::utils::Print::Separator(anOutputStream, "Numerical Solver") ;
    numericalSolver_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

FullPropagator                      FullPropagator::MediumFidelity                  ( )
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

    const Instant instant = Instant::J2000() ;

    const Environment customEnvironment = Environment(instant, objects) ;

    // Satellite system setup
    const Composite satelliteGeometry = Composite { Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } ) } ;
    const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

    // Satellite dynamics setup
    const FullSatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem } ;

    // Construct default numerical solver
    const NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

    return { satelliteDynamics, numericalSolver } ;

}

FullPropagator                      FullPropagator::HighFidelity                    ( )
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

    const Instant instant = Instant::J2000() ;

    const Environment customEnvironment = Environment(instant, objects) ;

    // Satellite system setup
    const Composite satelliteGeometry = Composite { Cuboid({ 0.0, 0.0, 0.0 }, { Vector3d { 1.0, 0.0, 0.0 }, Vector3d { 0.0, 1.0, 0.0 }, Vector3d { 0.0, 0.0, 1.0 } }, { 1.0, 2.0, 3.0 } ) } ;
    const SatelliteSystem satelliteSystem = { Mass(100.0, Mass::Unit::Kilogram), satelliteGeometry, Matrix3d::Identity(), 0.8, 2.2 } ;

    // Satellite dynamics setup
    const FullSatelliteDynamics satelliteDynamics = { customEnvironment, satelliteSystem } ;

    // Construct default numerical solver
    const NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

    return { satelliteDynamics, numericalSolver } ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
