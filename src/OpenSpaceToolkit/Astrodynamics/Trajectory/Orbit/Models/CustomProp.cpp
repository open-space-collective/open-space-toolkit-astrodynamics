////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/CustomProp.hpp>

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

using namespace boost::numeric::odeint;

static const Real Tolerance = 1e-8 ;
static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;
static const double mu = 3.986004418e14;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                CustomProp::CustomProp                      (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Length&                     anEquatorialRadius,
                                                                                const   Real&                       aJ2,
                                                                                const   Real&                       aJ4,
                                                                                const   CustomProp::PerturbationType&   aPerturbationType                           )
                                :   Model(),
                                    state_(aState),
                                    epoch_(anEpoch),
                                    gravitationalParameter_(aGravitationalParameter),
                                    equatorialRadius_(anEquatorialRadius),
                                    j2_(aJ2),
                                    j4_(aJ4),
                                    perturbationType_(aPerturbationType)
{

}

// CustomProp*                         CustomProp::clone                               ( ) const
// {
//     return new CustomProp(*this) ;
// }

bool                            CustomProp::operator ==                         (   const   CustomProp&                     aCustomPropModel                             ) const
{

    if ((!this->isDefined()) || (!aCustomPropModel.isDefined()))
    {
        return false ;
    }

    return (state_ == aCustomPropModel.state_)
        && (epoch_ == aCustomPropModel.epoch_)
        && (gravitationalParameter_ == aCustomPropModel.gravitationalParameter_)
        && (perturbationType_ == aCustomPropModel.perturbationType_) ;

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
    return state_.isDefined() && epoch_.isDefined() && gravitationalParameter_.isDefined() ;
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

    switch (perturbationType_)
    {

        case CustomProp::PerturbationType::None:
            return CustomProp::CalculateNoneStateAt(state_, epoch_, gravitationalParameter_, anInstant) ;

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

    switch (perturbationType_)
    {

        case CustomProp::PerturbationType::None:
            return CustomProp::CalculateNoneRevolutionNumberAt(state_, epoch_, gravitationalParameter_, anInstant) ;

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
    ostk::core::utils::Print::Line(anOutputStream) << "Gravitational parameter:" << (gravitationalParameter_.isDefined() ? gravitationalParameter_.toString() : "Undefined") ;
    ostk::core::utils::Print::Line(anOutputStream) << "Type:"                << CustomProp::StringFromPerturbationType(perturbationType_) ;

    ostk::core::utils::Print::Separator(anOutputStream, "State") ;

    state_.print(anOutputStream, false) ;

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void () ;

}

String                          CustomProp::StringFromPerturbationType          (   const   CustomProp::PerturbationType&   aPerturbationType                           )
{

    switch (aPerturbationType)
    {

        case CustomProp::PerturbationType::None:
            return "None" ;

        case CustomProp::PerturbationType::J2:
            return "J2" ;

        case CustomProp::PerturbationType::J4:
            return "J4" ;

        default:
            throw ostk::core::error::runtime::Wrong("Perturbation type") ;

    }

    return String::Empty() ;

}

// RHS of differential state space system
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

    dxdt[3] = -(mu/positionMagnitudeCube) * x[0];
    dxdt[4] = -(mu/positionMagnitudeCube) * x[1];
    dxdt[5] = -(mu/positionMagnitudeCube) * x[2];

}


void CustomProp::PropLog( const CustomProp::state_type &x , const double t )
{
    
    std::cout << t << "\t\t" << x[0] << "\t\t" << x[1] << "\t\t" << x[2]  << "\t\t" << x[3] << "\t\t" << x[4] << "\t\t" << x[5] << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

State                           CustomProp::CalculateNoneStateAt            (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   )
{

    using ostk::physics::units::Time ;
    using ostk::physics::units::Derived ;
    using ostk::physics::units::Angle ;
    using ostk::physics::time::Duration ;

    // // Duration from epoch

    // const Real durationFromEpoch_s = Duration::Between(anEpoch, anInstant).inSeconds() ;

    // // Orbital parameters

    // const Real semiMajorAxis_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters() ;
    // const Real eccentricity = aClassicalOrbitalElementSet.getEccentricity() ;
    // const Real inclination_rad = aClassicalOrbitalElementSet.getInclination().inRadians() ;
    // const Real raan_rad = aClassicalOrbitalElementSet.getRaan().inRadians() ;
    // const Real aop_rad = aClassicalOrbitalElementSet.getAop().inRadians() ;

    // Real trueAnomaly_rad = aClassicalOrbitalElementSet.getTrueAnomaly().inRadians() ;

    // // Mean motion

    // const Real meanMotion_radSec = aClassicalOrbitalElementSet.getMeanMotion(aGravitationalParameter).in(Derived::Unit::AngularVelocity(Angle::Unit::Radian, Time::Unit::Second)) ;

    // if (eccentricity.abs() < Tolerance) // Circular orbit
    // {
    //     trueAnomaly_rad += meanMotion_radSec * durationFromEpoch_s ;
    // }
    // else
    // {

    //     // Mean anomaly

    //     Real meanAnomaly_rad = aClassicalOrbitalElementSet.getMeanAnomaly().inRadians() ;

    //     meanAnomaly_rad += meanMotion_radSec * durationFromEpoch_s ;
    //     meanAnomaly_rad = std::fmod(meanAnomaly_rad, 2.0 * M_PI) ;

    //     // Eccentric anomaly

    //     const Angle eccentricAnomaly = State::EccentricAnomalyFromMeanAnomaly(Angle::Radians(meanAnomaly_rad), eccentricity, Tolerance) ;

    //     // True anomaly

    //     trueAnomaly_rad = State::TrueAnomalyFromEccentricAnomaly(eccentricAnomaly, eccentricity).inRadians() ;

    // }

    // const State coe = { Length::Meters(semiMajorAxis_m), eccentricity, Angle::Radians(inclination_rad), Angle::Radians(raan_rad), Angle::Radians(aop_rad), Angle::Radians(trueAnomaly_rad) } ;

    // static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

    // const State::CartesianState cartesianState = coe.getCartesianState(aGravitationalParameter, gcrfSPtr) ;

    // const Position& position = cartesianState.first ;
    // const Velocity& velocity = cartesianState.second ;

    const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

    const State state = { anInstant, Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) };

    return state ;

}

Integer                         CustomProp::CalculateNoneRevolutionNumberAt (   const   State&                      aState,
                                                                                const   Instant&                    anEpoch,
                                                                                const   Derived&                    aGravitationalParameter,
                                                                                const   Instant&                    anInstant                                   )
{

    using ostk::physics::time::Duration ;

    // const Duration orbitalPeriod = aClassicalOrbitalElementSet.getOrbitalPeriod(aGravitationalParameter) ;

    // const Duration durationFromEpoch = Duration::Between(anEpoch, anInstant) ;

    return 3 ; // (durationFromEpoch.inSeconds() / orbitalPeriod.inSeconds()).floor() + 1 ;

}



// Calculate J2 state at a desired time (code straight form CustomProp.cpp)

// State                           CustomProp::CalculateJ2StateAt                  (   const   State&                        aState,
//                                                                                 const   Instant&                    anEpoch,
//                                                                                 const   Derived&                    aGravitationalParameter,
//                                                                                 const   Instant&                    anInstant,
//                                                                                 const   Length&                     anEquatorialRadius,
//                                                                                 const   Real&                       aJ2                                         )
// {

//     using ostk::physics::units::Mass ;
//     using ostk::physics::units::Time ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::time::Duration ;

//     // Setup

//     const Real equatorialRadius_m = anEquatorialRadius.inMeters() ;

//     const Real gravitationalParameter_SI = aGravitationalParameter.in(GravitationalParameterSIUnit) ;

//     // Duration from epoch

//     const Real durationFromEpoch_s = Duration::Between(anEpoch, anInstant).inSeconds() ;

//     // Orbital parameters at epoch

//     const Real semiMajorAxisAtEpoch_m = aClassicalOrbitalElementSet.getSemiMajorAxis().inMeters() ;
//     const Real eccentricityAtEpoch = aClassicalOrbitalElementSet.getEccentricity() ;
//     const Real inclinationAtEpoch_rad = aClassicalOrbitalElementSet.getInclination().inRadians() ;
//     const Real raanAtEpoch_rad = aClassicalOrbitalElementSet.getRaan().inRadians() ;
//     const Real aopAtEpoch_rad = aClassicalOrbitalElementSet.getAop().inRadians() ;
//     const Real meanAnomalyAtEpoch_rad = aClassicalOrbitalElementSet.getMeanAnomaly().inRadians() ;

//     // Calculation
//     // Ref: http://www.s3l.be/usr/files/di/fi/2/Lecture06_AnalyticNumeric_2018-2019_201811142121.pdf

//     const Real n = std::sqrt(gravitationalParameter_SI / (semiMajorAxisAtEpoch_m * semiMajorAxisAtEpoch_m * semiMajorAxisAtEpoch_m)) ;
//     const Real p = semiMajorAxisAtEpoch_m * (1.0 - eccentricityAtEpoch * eccentricityAtEpoch) ;

//     const Real cosInclination = std::cos(inclinationAtEpoch_rad) ;
//     const Real sinInclination = std::sin(inclinationAtEpoch_rad) ;
//     const Real sinInclinationSquared = sinInclination * sinInclination ;

//     const Real expr = (3.0 / 2.0) * aJ2 * std::pow((equatorialRadius_m / p), 2) ;

//     const Real n_bar = n * (1.0 + expr * std::sqrt(1.0 - eccentricityAtEpoch * eccentricityAtEpoch) * (1.0 - (3.0 / 2.0) * sinInclinationSquared)) ;

//     const Real aop_bar_rad = aopAtEpoch_rad + expr * (2.0 - (5.0 / 2.0) * sinInclinationSquared) * n_bar * durationFromEpoch_s ;
//     const Real raan_bar_rad = raanAtEpoch_rad - expr * cosInclination * n_bar * durationFromEpoch_s ;
//     const Real meanAnomaly_rad = meanAnomalyAtEpoch_rad + n_bar * durationFromEpoch_s ;

//     // Orbital parameters at instant

//     const Real semiMajorAxis_m = semiMajorAxisAtEpoch_m ;
//     const Real eccentricity = eccentricityAtEpoch ;
//     const Real inclination_rad = inclinationAtEpoch_rad ;
//     const Real raan_rad = raan_bar_rad ;
//     const Real aop_rad = aop_bar_rad ;
//     const Real trueAnomaly_rad = COE::TrueAnomalyFromEccentricAnomaly(COE::EccentricAnomalyFromMeanAnomaly(Angle::Radians(meanAnomaly_rad), eccentricity, Tolerance), eccentricity).inRadians() ;

//     const COE coe = { Length::Meters(semiMajorAxis_m), eccentricity, Angle::Radians(inclination_rad), Angle::Radians(raan_rad), Angle::Radians(aop_rad), Angle::Radians(trueAnomaly_rad) } ;

//     static const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

//     const COE::CartesianState cartesianState = coe.getCartesianState(aGravitationalParameter, gcrfSPtr) ;

//     const Position& position = cartesianState.first ;
//     const Velocity& velocity = cartesianState.second ;

//     const State state = { anInstant, position, velocity } ;

//     return state ;

// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
