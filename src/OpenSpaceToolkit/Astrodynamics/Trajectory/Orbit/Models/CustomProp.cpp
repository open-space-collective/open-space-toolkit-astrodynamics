////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/CustomProp.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <iostream>
#include <cmath>

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
static const Derived theGravitationalParameter = Earth::Models::EGM2008::GravitationalParameter ;
static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;
static const Real mu_SI = theGravitationalParameter.in(GravitationalParameterSIUnit) ;
static const Real J2 = Earth::Models::EGM2008::J2 ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                CustomProp::CustomProp                      (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   CustomProp::GravPerturbationType&       aGravPerturbationType,                       
                                                                                const   CustomProp::AtmosPerturbationType&      aAtmosPerturbationType,                       
                                                                                const   CustomProp::ThirdBodyPerturbationType&  aThirdBodyPerturbationType       ) 
                                :   Model(),
                                    state_(aState),
                                    epoch_(anEpoch),
                                    gravPerturbationType_(aGravPerturbationType),
                                    atmosPerturbationType_(aAtmosPerturbationType),
                                    thirdBodyPerturbationType_(aThirdBodyPerturbationType)
                                    
{

}

CustomProp*                     CustomProp::clone                               ( ) const
{
    return new CustomProp(*this) ;
}

bool                            CustomProp::operator ==                         (   const   CustomProp&                     aCustomPropModel                             ) const
{

    if ((!this->isDefined()) || (!aCustomPropModel.isDefined()))
    {
        return false ;
    }

    return (state_ == aCustomPropModel.state_)
        && (epoch_ == aCustomPropModel.epoch_)
        && (gravPerturbationType_ == aCustomPropModel.gravPerturbationType_)
        && (atmosPerturbationType_ == aCustomPropModel.atmosPerturbationType_)
        && (thirdBodyPerturbationType_ == aCustomPropModel.thirdBodyPerturbationType_) ;

}

bool                            CustomProp::operator !=                         (   const   CustomProp&                     aCustomPropModel                             ) const
{
    return !((*this) == aCustomPropModel) ;
}

std::ostream&                   operator <<                                 (           std::ostream&               anOutputStream,
                                                                                const   CustomProp&                 aCustomPropModel                            )
{

    aCustomPropModel.print(anOutputStream) ;

    return anOutputStream ;

}

bool                            CustomProp::isDefined                           ( ) const
{
    return state_.isDefined() && epoch_.isDefined() ;
}

Instant                         CustomProp::getEpoch                            ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CustomProp") ;
    }

    return epoch_ ;

}

Integer                         CustomProp::getRevolutionNumberAtEpoch          ( ) const
{

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CustomProp") ;
    }

    return 1 ; 

}

State                           CustomProp::calculateStateAt                    (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CustomProp") ;
    }

    switch (gravPerturbationType_)
    {

        case CustomProp::GravPerturbationType::None:
            return CustomProp::CalculateNoneStateAt(state_, epoch_, anInstant) ;

        // case CustomProp::PerturbationType::J2:
        //     return CustomProp::CalculateJ2StateAt(state_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_) ;

        // case CustomProp::PerturbationType::J4:
        //     return CustomProp::CalculateJ4StateAt(state_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_, j4_) ;

        default:
            throw ostk::core::error::runtime::Wrong("Perturbation type") ;

    }

    return State::Undefined() ;

}

Integer                         CustomProp::calculateRevolutionNumberAt         (   const   Instant&                    anInstant                                   ) const
{

    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant") ;
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("CustomProp") ;
    }

    if (anInstant == epoch_)
    {
        return this->getRevolutionNumberAtEpoch() ;
    }

    switch (gravPerturbationType_)
    {

        case CustomProp::GravPerturbationType::None:
            return CustomProp::CalculateNoneRevolutionNumberAt(state_, epoch_, anInstant) ;

        // case CustomProp::PerturbationType::J2:
        //     return CustomProp::CalculateJ2RevolutionNumberAt(state_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_) ;

        // case CustomProp::PerturbationType::J4:
        //     return CustomProp::CalculateJ4RevolutionNumberAt(state_, epoch_, gravitationalParameter_, anInstant, equatorialRadius_, j2_, j4_) ;

        default:
            throw ostk::core::error::runtime::Wrong("Perturbation type") ;

    }

    return Integer::Undefined() ;

}

void                            CustomProp::print                               (       std::ostream&               anOutputStream,
                                                                                        bool                        displayDecorator                            ) const
{

    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "CustomProp") : void () ;

    ostk::core::utils::Print::Line(anOutputStream) << "Epoch:"               << (epoch_.isDefined() ? epoch_.toString() : "Undefined") ;
    // ostk::core::utils::Print::Line(anOutputStream) << "Gravitational parameter:" << (gravitationalParameter_.isDefined() ? gravitationalParameter_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Grav perturbation type:"                << CustomProp::StringFromGravPerturbationType(gravPerturbationType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Atmospheric perturbation type:"                << CustomProp::StringFromAtmosPerturbationType(atmosPerturbationType_) ;
    ostk::core::utils::Print::Line(anOutputStream) << "Third body perturbation type:"                << CustomProp::StringFromThirdBodyPerturbationType(thirdBodyPerturbationType_) ;

    ostk::core::utils::Print::Separator(anOutputStream, "State") ;

    state_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

String                          CustomProp::StringFromGravPerturbationType          (   const   CustomProp::GravPerturbationType&   aGravPerturbationType                           )
{

    switch (aGravPerturbationType)
    {

        case CustomProp::GravPerturbationType::None:
            return "None" ;

        case CustomProp::GravPerturbationType::J2:
            return "J2" ;

        case CustomProp::GravPerturbationType::TenByTen:
            return "TenByTen" ;
        
        case CustomProp::GravPerturbationType::FourtyByFourty:
            return "FourtyByFourty" ;

        default:
            throw ostk::core::error::runtime::Wrong("Grav Perturbation type") ;

    }

    return String::Empty() ;

}

String                          CustomProp::StringFromAtmosPerturbationType          (   const   CustomProp::AtmosPerturbationType&   aAtmosPerturbationType                           )
{

    switch (aAtmosPerturbationType)
    {

        case CustomProp::AtmosPerturbationType::None:
            return "None" ;

        case CustomProp::AtmosPerturbationType::Exponential:
            return "Exponential" ;

        case CustomProp::AtmosPerturbationType::JacchiaRoberts:
            return "JacchiaRoberts" ;

        case CustomProp::AtmosPerturbationType::NRLMISIS00:
            return "NRLMISIS00" ;

        default:
            throw ostk::core::error::runtime::Wrong("Atmos Perturbation type") ;

    }

    return String::Empty() ;

}

String                          CustomProp::StringFromThirdBodyPerturbationType          (   const   CustomProp::ThirdBodyPerturbationType&   aThirdBodyPerturbationType                           )
{

    switch (aThirdBodyPerturbationType)
    {

        case CustomProp::ThirdBodyPerturbationType::None:
            return "None" ;

        case CustomProp::ThirdBodyPerturbationType::Luni:
            return "Luni" ;

        case CustomProp::ThirdBodyPerturbationType::Solar:
            return "Solar" ;

        case CustomProp::ThirdBodyPerturbationType::LuniSolar:
            return "LuniSolar" ;

        default:
            throw ostk::core::error::runtime::Wrong("Third Body Perturbation type") ;

    }

    return String::Empty() ;

}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool                            CustomProp::operator ==                         (   const   trajectory::Model&          aModel                                      ) const
{

    const CustomProp* customPropModelPtr = dynamic_cast<const CustomProp*>(&aModel) ;

    return (customPropModelPtr != nullptr) && this->operator == (*customPropModelPtr) ;

}

bool                            CustomProp::operator !=                         (   const   trajectory::Model&          aModel                                      ) const
{
    return !((*this) == aModel) ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CustomProp::TwoBodyDynamics( const CustomProp::state_type &x , CustomProp::state_type &dxdt , const double )
{
    // Find position magnitude
    double positionMagnitude = (double) sqrt(pow(x[0],2) + pow(x[1],2) + pow(x[2],2));
    
    // Integrate position states
    dxdt[0] = x[3];  
    dxdt[1] = x[4];
    dxdt[2] = x[5]; 

    // Integrate velocity states
    double positionMagnitudeCube = pow(positionMagnitude,3);

    dxdt[3] = -(mu_SI/positionMagnitudeCube) * x[0];
    dxdt[4] = -(mu_SI/positionMagnitudeCube) * x[1];
    dxdt[5] = -(mu_SI/positionMagnitudeCube) * x[2];

}

void CustomProp::PropLog( const CustomProp::state_type &x , const double t )
{
    
    std::cout << t << "\t\t" << x[0] << "\t\t" << x[1] << "\t\t" << x[2]  << "\t\t" << x[3] << "\t\t" << x[4] << "\t\t" << x[5] << std::endl;
}

State                           CustomProp::CalculateNoneStateAt            (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Instant&                    anInstant                                   )
{

    using ostk::physics::units::Time ;
    using ostk::physics::units::Derived ;
    using ostk::physics::units::Angle ;
    using ostk::physics::time::Duration ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;

    using ostk::math::obj::Vector3d ;

    double initialTimeStep = 5.0; // Default value for now

    // Get state related parameters
    Position currPosition = aState.getPosition();
    Velocity currVelocity = aState.getVelocity();

    Vector3d currPositionCoords = currPosition.inUnit(Position::Unit::Meter).accessCoordinates();
    Vector3d currVelocityCoords = currVelocity.inUnit(Velocity::Unit::MeterPerSecond).accessCoordinates();

    CustomProp::state_type fullState(6);
    fullState[0] = currPositionCoords[0];
    fullState[1] = currPositionCoords[1];
    fullState[2] = currPositionCoords[2];

    fullState[3] = currVelocityCoords[0];
    fullState[4] = currVelocityCoords[1];
    fullState[5] = currVelocityCoords[2];

    Duration propDuration = anInstant - aState.getInstant();
    double propDurationInSecs = (double) propDuration.inSeconds();

    integrate ( CustomProp::TwoBodyDynamics , fullState , (0.0) , propDurationInSecs , initialTimeStep ); 

    static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;  // Not sure about the static

    const State state = { anInstant, Position::Meters({ fullState[0], fullState[1], fullState[2] }, gcrfSPtr), Velocity::MetersPerSecond({ fullState[3], fullState[4], fullState[5] }, gcrfSPtr) };

    return state ;

}

Integer                         CustomProp::CalculateNoneRevolutionNumberAt (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Instant&                    anInstant                                   )
{

    using ostk::physics::time::Duration ;

    // const Duration orbitalPeriod = aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter) ;

    // const Duration durationFromEpoch = Duration::Between(anEpoch, anInstant) ;

    return 3 ; // (durationFromEpoch.inSeconds() / orbitalPeriod.inSeconds()).floor() + 1 ;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
