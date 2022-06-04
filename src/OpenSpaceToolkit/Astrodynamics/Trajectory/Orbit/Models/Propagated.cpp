////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <boost/numeric/odeint.hpp>
#include <boost/array.hpp>
#include <boost/operators.hpp>

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
using ostk::physics::units::Unit ;
using ostk::physics::env::obj::celest::Earth ;

using namespace boost::numeric::odeint;

static const Real Tolerance = 1e-8 ;
// static const Derived theGravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;
// static const Real mu_SI = theGravitationalParameter.in(GravitationalParameterSIUnit) ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                Propagated::Propagated                      (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Propagated::GravitationalPerturbationType&       aGravitationalPerturbationType,                       
                                                                                const   Propagated::AtmosphericPerturbationType&      aAtmosphericPerturbationType,                       
                                                                                const   Propagated::ThirdBodyPerturbationType&  aThirdBodyPerturbationType,      
                                                                                const   bool&                                   aPropagationLogStatus                 ) 
                                :   Model(),
                                    state_(aState),
                                    epoch_(anEpoch),
                                    gravitationalParameter_(aGravitationalParameter),
                                    equatorialRadius_(anEquatorialRadius),
                                    gravitationalPerturbationType_(aGravitationalPerturbationType),
                                    atmosphericPerturbationType_(aAtmosphericPerturbationType),
                                    thirdBodyPerturbationType_(aThirdBodyPerturbationType),
                                    propagationLogStatus_ (aPropagationLogStatus)
                                    
{

}

Propagated*                     Propagated::clone                               ( ) const
{
    return new Propagated(*this) ;
}

bool                            Propagated::operator ==                         (   const   Propagated&                     aPropagatedModel                    ) const
{

    if ((!this->isDefined()) || (!aPropagatedModel.isDefined()))
    {
        return false ;
    }

    return (state_ == aPropagatedModel.state_)
        && (epoch_ == aPropagatedModel.epoch_)
        && (gravitationalParameter_ == aPropagatedModel.gravitationalParameter_)
        && (gravitationalPerturbationType_ == aPropagatedModel.gravitationalPerturbationType_)
        && (atmosphericPerturbationType_ == aPropagatedModel.atmosphericPerturbationType_)
        && (thirdBodyPerturbationType_ == aPropagatedModel.thirdBodyPerturbationType_) ;

}

bool                            Propagated::operator !=                         (   const   Propagated&                     aPropagatedModel                    ) const
{
    return !((*this) == aPropagatedModel) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   Propagated&                 aPropagatedModel                            )
{

    aPropagatedModel.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            Propagated::isDefined                           ( ) const
{
    return state_.isDefined() && epoch_.isDefined() && gravitationalParameter_.isDefined() ;
}

Derived                         Propagated::getGravitationalParameter           ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    return gravitationalParameter_ ;

}

Length                          Propagated::getEquatorialRadius                 ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    return equatorialRadius_ ;

}

Instant                         Propagated::getEpoch                            ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    return epoch_ ;

}

Integer                         Propagated::getRevolutionNumberAtEpoch          ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    return 1 ; 

}

State                           Propagated::calculateStateAt                    (   const   Instant&                anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    switch (gravitationalPerturbationType_)
    {

        case Propagated::GravitationalPerturbationType::None:
            return Propagated::CalculateNoneStateAt(state_, gravitationalParameter_, anInstant, propagationLogStatus_) ;

        // case Propagated::GravitationalPerturbationType::J2:
        //     return Propagated::CalculateJ2StateAt(state_, epoch_, anInstant) ;

        // case Propagated::GravitationalPerturbationType::TenByTen:
        //     return Propagated::CalculateTenByTenStateAt(state_, epoch_, anInstant) ;

        // case Propagated::GravitationalPerturbationType::FourtyByFourty:
        //     return Propagated::CalculateFourtyByFourtyStateAt(state_, epoch_, anInstant) ;

        default:
            throw ostk::core::error::runtime::Wrong("Grav Perturbation type") ;

    }

    //     switch (atmosphericPerturbationType_)
    // {

    //     case Propagated::AtmosphericPerturbationType::None:
    //         return Propagated::CalculateNoneStateAt(state_, epoch_, anInstant) ;

    //     // case Propagated::PerturbationType::J2:
    //     //     return Propagated::CalculateJ2StateAt(state_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_) ;

    //     // case Propagated::PerturbationType::J4:
    //     //     return Propagated::CalculateJ4StateAt(state_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_, j4_) ;

    //     default:
    //         throw ostk::core::error::runtime::Wrong("Perturbation type") ;

    // }

    //     switch (thirdBodyPerturbationType_)
    // {

    //     case Propagated::GravitationalPerturbationType::None:
    //         return Propagated::CalculateNoneStateAt(state_, epoch_, anInstant) ;

    //     // case Propagated::PerturbationType::J2:
    //     //     return Propagated::CalculateJ2StateAt(state_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_) ;

    //     // case Propagated::PerturbationType::J4:
    //     //     return Propagated::CalculateJ4StateAt(state_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_, j4_) ;

    //     default:
    //         throw ostk::core::error::runtime::Wrong("Perturbation type") ;

    // }
    
    return State::Undefined() ;

}

Integer                         Propagated::calculateRevolutionNumberAt         (   const   Instant&                anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Propagated") ;
    }

    if (anInstant == epoch_)
    {
        return this->getRevolutionNumberAtEpoch() ;
    }

    switch (gravitationalPerturbationType_)
    {

        case Propagated::GravitationalPerturbationType::None:
            return Propagated::CalculateNoneRevolutionNumberAt(state_, epoch_, gravitationalParameter_, anInstant) ;

        // case Propagated::GravitationalPerturbationType::J2:
        //     return Propagated::CalculateJ2RevolutionNumberAt(state_, epoch_, anInstant) ;

        // case Propagated::GravitationalPerturbationType::TenByTen:
        //     return Propagated::CalculateTenByTenRevolutionNumberAt(state_, epoch_, anInstant) ;

        // case Propagated::GravitationalPerturbationType::FourtyByFourty:
        //     return Propagated::CalculateFourtyByFourtyRevolutionNumberAt(state_, epoch_, anInstant) ;

        default:
            throw ostk::core::error::runtime::Wrong("Grav Perturbation type") ;

    }

    return Integer::Undefined() ;

}

void                            Propagated::print                               (       std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Propagated") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Epoch:"               << (epoch_.isDefined() ? epoch_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Gravitational parameter:" << (gravitationalParameter_.isDefined() ? gravitationalParameter_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Grav perturbation type:"                << Propagated::StringFromGravitationalPerturbationType(gravitationalPerturbationType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Atmospheric perturbation type:"                << Propagated::StringFromAtmosphericPerturbationType(atmosphericPerturbationType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Third body perturbation type:"                << Propagated::StringFromThirdBodyPerturbationType(thirdBodyPerturbationType_) ;

    ostk::core::utils::Print::Separator(anOutputStream, "State") ;

    state_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

String                          Propagated::StringFromGravitationalPerturbationType      (   const   Propagated::GravitationalPerturbationType&       aGravitationalPerturbationType       )
{

    switch (aGravitationalPerturbationType)
    {

        case Propagated::GravitationalPerturbationType::None:
            return "None" ;

        case Propagated::GravitationalPerturbationType::J2:
            return "J2" ;

        case Propagated::GravitationalPerturbationType::TenByTen:
            return "TenByTen" ;
        
        case Propagated::GravitationalPerturbationType::FourtyByFourty:
            return "FourtyByFourty" ;

        default:
            throw ostk::core::error::runtime::Wrong("Grav Perturbation type") ;

    }

    return String::Empty() ;

}

String                          Propagated::StringFromAtmosphericPerturbationType     (   const   Propagated::AtmosphericPerturbationType&      aAtmosphericPerturbationType      )
{

    switch (aAtmosphericPerturbationType)
    {

        case Propagated::AtmosphericPerturbationType::None:
            return "None" ;

        case Propagated::AtmosphericPerturbationType::Exponential:
            return "Exponential" ;

        case Propagated::AtmosphericPerturbationType::JacchiaRoberts:
            return "JacchiaRoberts" ;

        case Propagated::AtmosphericPerturbationType::NRLMISIS00:
            return "NRLMISIS00" ;

        default:
            throw ostk::core::error::runtime::Wrong("Atmos Perturbation type") ;

    }

    return String::Empty() ;

}

String                          Propagated::StringFromThirdBodyPerturbationType (   const   Propagated::ThirdBodyPerturbationType&  aThirdBodyPerturbationType  )
{

    switch (aThirdBodyPerturbationType)
    {

        case Propagated::ThirdBodyPerturbationType::None:
            return "None" ;

        case Propagated::ThirdBodyPerturbationType::Luni:
            return "Luni" ;

        case Propagated::ThirdBodyPerturbationType::Solar:
            return "Solar" ;

        case Propagated::ThirdBodyPerturbationType::LuniSolar:
            return "LuniSolar" ;

        default:
            throw ostk::core::error::runtime::Wrong("Third Body Perturbation type") ;

    }

    return String::Empty() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool                            Propagated::operator ==                         (   const   trajectory::Model&      aModel                                      ) const
{

    const Propagated* propagatedModelPtr = dynamic_cast<const Propagated*>(&aModel) ;

    return (propagatedModelPtr != nullptr) && this->operator == (*propagatedModelPtr) ;

}

bool                            Propagated::operator !=                         (   const   trajectory::Model&      aModel                                      ) const
{
    return !((*this) == aModel) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            Propagated::TwoBodyDynamics                     (   const   Propagated::state_type& x, 
                                                                                            Propagated::state_type& dxdt,
                                                                                    const   double                  ,
                                                                                    const   double&                 mu_SI                                       )
{
    // Find position magnitude
    const double& positionMagnitude = std::sqrt(std::pow(x[0],2) + std::pow(x[1],2) + std::pow(x[2],2));  // @BOSS for calculation speed would it make more sense to just put the whole sqrt expression in the velocity state equation or instead do what I have right now by defining a reference to pass into the std::pow function?
    
    // Integrate position states
    dxdt[0] = x[3];  
    dxdt[1] = x[4];
    dxdt[2] = x[5]; 

    // Integrate velocity states
    dxdt[3] = -(mu_SI/std::pow(positionMagnitude,3)) * x[0];
    dxdt[4] = -(mu_SI/std::pow(positionMagnitude,3)) * x[1];
    dxdt[5] = -(mu_SI/std::pow(positionMagnitude,3)) * x[2];

}

void                            Propagated::PropObserver                        (   const   Propagated::state_type& x,
                                                                                    const   double                  t                                           )
{
    std::cout.precision(2);
    std::cout.setf(std::ios::fixed,std::ios::floatfield);

    std::cout << std::left << std::setw(15) << t ;
    std::cout.precision(8);
    std::cout.setf(std::ios::scientific,std::ios::floatfield);
    
    std::cout << std::internal << std::setw(16) << x[0] << "     " << std::internal << std::setw(16) << x[1] << "     " << std::internal << std::setw(16) << x[2] << "     " << std::internal << std::setw(16) << x[3] << "     " << std::internal << std::setw(16) << x[4] << "     " << std::internal << std::setw(16) << x[5] << std::endl;
    std::cout.setf(std::ios::fixed,std::ios::floatfield);
}

State                           Propagated::CalculateNoneStateAt                (   const   State&                      aState,
                                                                                    const   Derived&                    aGravitationalParameter,
                                                                                    const   Instant&                    anInstant,                               
                                                                                    const   bool&                       aPropagationLogStatus                   )
{

    using ostk::physics::units::Time ;
    using ostk::physics::units::Derived ;
    using ostk::physics::units::Angle ;
    using ostk::physics::time::Duration ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;

    using ostk::math::obj::Vector3d ;

    const double mu_SI = aGravitationalParameter.in(GravitationalParameterSIUnit) ;

    // Get state related parameters
    Position currPosition = aState.getPosition();
    Velocity currVelocity = aState.getVelocity();

    Vector3d currPositionCoords = currPosition.inUnit(Position::Unit::Meter).accessCoordinates();
    Vector3d currVelocityCoords = currVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

    Propagated::state_type fullState(6);
    fullState[0] = currPositionCoords[0];
    fullState[1] = currPositionCoords[1];
    fullState[2] = currPositionCoords[2];
    fullState[3] = currVelocityCoords[0];
    fullState[4] = currVelocityCoords[1];
    fullState[5] = currVelocityCoords[2];

    Duration propDuration = anInstant - aState.getInstant();
    double propDurationInSecs = (double) propDuration.inSeconds();

    const double timeStep = 5.0; // Default value for now
    // const double largestTimeStep = 60.0;

    // define odeint controlled error stepper 
    typedef runge_kutta_cash_karp54< Propagated::state_type > error_stepper_type; // modify later to take stepper type as an input to this func 
    const double absErrTol = 1.0e-15; 
    const double relErrTol = 1.0e-15; 
    

    // Call odeint function to perform numerical integration
    if (aPropagationLogStatus)
    {
        integrate_adaptive (make_controlled( absErrTol , relErrTol , error_stepper_type() ) , [=] (const Propagated::state_type& x, Propagated::state_type& dxdt, const double t) -> void { return Propagated::TwoBodyDynamics(x, dxdt, t, mu_SI); } , fullState , (0.0) , propDurationInSecs , timeStep, Propagated::PropObserver); 
        // integrate_const (make_controlled( absErrTol , relErrTol , error_stepper_type() ) , [=] (const Propagated::state_type& x, Propagated::state_type& dxdt, const double t) -> void { return Propagated::TwoBodyDynamics(x, dxdt, t, mu_SI); } , fullState , (0.0) , propDurationInSecs , timeStep, Propagated::PropObserver); 
    }
    else
    {
        integrate_adaptive (make_controlled( absErrTol , relErrTol , error_stepper_type() ) , [=] (const Propagated::state_type& x, Propagated::state_type& dxdt, const double t) -> void { return Propagated::TwoBodyDynamics(x, dxdt, t, mu_SI); } , fullState , (0.0) , propDurationInSecs , timeStep); 
        // integrate_const (make_controlled( absErrTol , relErrTol , error_stepper_type() ) , [=] (const Propagated::state_type& x, Propagated::state_type& dxdt, const double t) -> void { return Propagated::TwoBodyDynamics(x, dxdt, t, mu_SI); } , fullState , (0.0) , propDurationInSecs , timeStep); 
    }    
          
    static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;  // Not sure about using this frame 

    const State state = { anInstant, Position::Meters({ fullState[0], fullState[1], fullState[2] }, gcrfSPtr), Velocity::MetersPerSecond({ fullState[3], fullState[4], fullState[5] }, gcrfSPtr) };

    return state ;

}

Integer                         Propagated::CalculateNoneRevolutionNumberAt     (   const   State&                      aState,
                                                                                    const   Instant&                    anEpoch,
                                                                                    const   Derived&                    aGravitationalParameter,
                                                                                    const   Instant&                    anInstant                                   )
{

    using ostk::physics::time::Duration ;

    const Real gravitationalParameter_SI = aGravitationalParameter.in(GravitationalParameterSIUnit) ;
    
    // Get state related parameters
    Position currPosition = aState.getPosition();
    Velocity currVelocity = aState.getVelocity();

    Vector3d currPositionCoords = currPosition.inUnit(Position::Unit::Meter).accessCoordinates();
    Vector3d currVelocityCoords = currVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

    const Real semiMajorAxis = - gravitationalParameter_SI * currPositionCoords.norm() / (currPositionCoords.norm() * std::pow(currVelocityCoords.norm(),2) - 2.0 * gravitationalParameter_SI);
    
    const Duration orbitalPeriod = Duration::Seconds(Real::TwoPi() * std::sqrt(std::pow(semiMajorAxis,3) / gravitationalParameter_SI)) ;

    const Duration durationFromEpoch = Duration::Between(anEpoch, anInstant) ;

    return (durationFromEpoch.inSeconds() / orbitalPeriod.inSeconds()).floor() + 1 ;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
