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

static const Real Tolerance = 1e-8 ;
static const Derived::Unit GravitationalParameterSIUnit = Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second) ;

using namespace boost::numeric::odeint;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Implement odeint function to integrate a simple function

const double mu = 3.986004418e14;
double posMag;

/* The rhs of x' = f(x) */
void CustomProp::TwoBodyDynamics( const CustomProp::state_type &x , CustomProp::state_type &dxdt , const double )
{
    // Find position magnitude
    posMag = (double) sqrt(pow(x[0],2) + pow(x[1],2) + pow(x[2],2));
    
    // Integrate position states
    dxdt[0] = x[3];  
    dxdt[1] = x[4];
    dxdt[2] = x[5]; 

    // Integrate velocity states
    double posMagCube = pow(posMag,3);

    dxdt[3] = -(mu/posMagCube) * x[0];
    dxdt[4] = -(mu/posMagCube) * x[1];
    dxdt[5] = -(mu/posMagCube) * x[2];

}


void CustomProp::PropLog( const CustomProp::state_type &x , const double t )
{
    std::cout << t << "\t\t" << x[0] << '\t' << x[1] << '\t' << x[2]  << '\t' << x[3] << '\t' << x[4] << '\t' << x[5] << std::endl;
}

// Calculate J2 state at a desired time (code straight form Kepler.cpp)

// State                           Kepler::CalculateJ2StateAt                  (   const   COE&                        aClassicalOrbitalElementSet,
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
