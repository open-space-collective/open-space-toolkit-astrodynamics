// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// /// @project        Open Space Toolkit ▸ Astrodynamics
// /// @file           OpenSpaceToolkit/Astrodynamics/FullDynamicsNumericalSolver.test.cpp
// /// @author         Remy Derollez <remy@loftorbital.com>
// /// @license        Apache License 2.0

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

// #include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
// #include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

// #include <OpenSpaceToolkit/Core/Containers/Array.hpp>
// #include <OpenSpaceToolkit/Core/Types/Real.hpp>
// #include <OpenSpaceToolkit/Core/Types/String.hpp>
// #include <OpenSpaceToolkit/Core/Types/Integer.hpp>

// #include <Global.test.hpp>

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStateFromInstantToInstant)
// {

//     using ostk::core::types::Real ;
//     using ostk::core::ctnr::Array ;
//     using ostk::core::types::String ;
//     using ostk::core::types::Integer ;

//     using ostk::physics::time::Instant ;
//     using ostk::physics::time::Duration ;

//     using ostk::astro::NumericalSolver ;

//     // Performance test with RungeKuttaCashKarp54 in forward time
//     {

//         const NumericalSolver::StateVector currentStateVector = { 0, 1 } ;
//         const Instant instant = Instant::J2000() ;
//         const Duration propDuration = Duration::Seconds(10000) ;

//         NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

//         const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant
//         (
//             currentStateVector,
//             instant,
//             instant + propDuration,
//             [] (const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double ) -> void
//             {

//                 // dxdt[0] = x[3] ;
//                 // dxdt[1] = x[4] ;
//                 // dxdt[2] = x[5] ;
//                 // // Velocity Derivative
//                 // dxdt[3] = totalGravitationalAcceleration_SI[0] ;
//                 // dxdt[4] = totalGravitationalAcceleration_SI[1] ;
//                 // dxdt[5] = totalGravitationalAcceleration_SI[2] ;

//                 // Quaternions and Angular Velocity

//                 // Quaternion Derivative (i.e. Quaternion Rates)
//                 dxdt[0] = -0.5 * (x[10]*x[7] + x[11]*x[8] + x[12]*x[9]) ;
//                 dxdt[1] = 0.5 * (x[10]*x[6] + x[12]*x[8] - x[11]*x[9]) ;
//                 dxdt[2] = 0.5 * (x[11]*x[6] - x[12]*x[7] + x[10]*x[9]) ;
//                 dxdt[3] = 0.5 * (x[12]*x[6] + x[11]*x[7] - x[10]*x[8]) ;
//                 // Angular Velocity Derivative
//                 dxdt[4] = (externalTorque / Ix) - ((Iz - Iy) / Ix) * x[11] * x[12] ;
//                 dxdt[5] = (externalTorque / Iy) - ((Ix - Iz) / Iy) * x[11] * x[12] ;
//                 dxdt[6] = (externalTorque / Iz) - ((Iy - Ix) / Iz) * x[11] * x[12] ;
//             }
//         ) ;

//         // Validate the output against an analytical function

//         EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds()))) ;
//         EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds()))) ;

//     }

//     // Performance test with RungeKuttaFehlberg78 in forwards time
//     {

//         const NumericalSolver::StateVector currentStateVector = { 0, 1 } ;
//         const Instant instant = Instant::J2000() ;
//         const Duration propDuration = Duration::Seconds(10000) ;

//         NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

//         const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant
//         (
//             currentStateVector,
//             instant,
//             instant + propDuration,
//             [] (const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double  ) -> void
//             {
//                 dxdt[0] = x[1] ;
//                 dxdt[1] = -x[0] ;
//             }
//         ) ;

//         // Validate the output against an analytical function

//         EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds()))) ;
//         EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds()))) ;

//     }
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
