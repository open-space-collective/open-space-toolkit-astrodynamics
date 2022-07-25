////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/NumericalSolver.test.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteDynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, Constructor)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        EXPECT_NO_THROW(NumericalSolver()) ;
        EXPECT_NO_THROW(NumericalSolver(NumericalSolver::IntegrationLogType::NoLog)) ;
        EXPECT_NO_THROW(NumericalSolver(NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54)) ;
        EXPECT_NO_THROW(NumericalSolver(NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0)) ;
        EXPECT_NO_THROW(NumericalSolver(NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15)) ;
        EXPECT_NO_THROW(NumericalSolver(NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15)) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, EqualToOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {
        // Test default constructor
        const NumericalSolver numericalSolver = { } ;
        const NumericalSolver numericalSolver_0 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver == numericalSolver_0) ;

        // Test IntegrationLogType
        const NumericalSolver numericalSolver_1 = { NumericalSolver::IntegrationLogType::LogConstant, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_FALSE(numericalSolver == numericalSolver_1) ;

        // Test IntegrationStepperType
        const NumericalSolver numericalSolver_2 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_FALSE(numericalSolver == numericalSolver_2) ;

        // Test timeStep
        const NumericalSolver numericalSolver_3 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 4.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_FALSE(numericalSolver == numericalSolver_3) ;

        // Test relativeTolerance
        const NumericalSolver numericalSolver_4 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-14, 1.0e-15 } ;

        EXPECT_FALSE(numericalSolver == numericalSolver_4) ;

         // Test absoluteTolerance
        const NumericalSolver numericalSolver_5 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-14 } ;

        EXPECT_FALSE(numericalSolver == numericalSolver_5) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, NotEqualToOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {
        // Test default constructor
        const NumericalSolver numericalSolver = { } ;
        const NumericalSolver numericalSolver_0 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_FALSE(numericalSolver != numericalSolver_0) ;

        // Test IntegrationLogType
        const NumericalSolver numericalSolver_1 = { NumericalSolver::IntegrationLogType::LogConstant, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver != numericalSolver_1) ;

        // Test IntegrationStepperType
        const NumericalSolver numericalSolver_2 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_TRUE(numericalSolver != numericalSolver_2) ;

        // Test timeStep
        const NumericalSolver numericalSolver_3 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 4.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver != numericalSolver_3) ;

        // Test relativeTolerance
        const NumericalSolver numericalSolver_4 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-14, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver != numericalSolver_4) ;

         // Test absoluteTolerance
        const NumericalSolver numericalSolver_5 = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-14 } ;

        EXPECT_TRUE(numericalSolver != numericalSolver_5) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, IsDefined)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver.isDefined()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, StreamOperator)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        const NumericalSolver numericalSolver = { } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << numericalSolver << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, Print)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(numericalSolver.print(std::cout, true)) ;
        EXPECT_NO_THROW(numericalSolver.print(std::cout, false)) ;
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, GetType)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        const NumericalSolver numericalSolver_NoLog = { NumericalSolver::IntegrationLogType::NoLog } ;
        EXPECT_EQ(numericalSolver_NoLog.getIntegrationLogType(),NumericalSolver::IntegrationLogType::NoLog) ;

        const NumericalSolver numericalSolver_LogConstant = { NumericalSolver::IntegrationLogType::LogConstant } ;
        EXPECT_EQ(numericalSolver_LogConstant.getIntegrationLogType(),NumericalSolver::IntegrationLogType::LogConstant) ;

        const NumericalSolver numericalSolver_LogAdaptive = { NumericalSolver::IntegrationLogType::LogAdaptive } ;
        EXPECT_EQ(numericalSolver_LogAdaptive.getIntegrationLogType(),NumericalSolver::IntegrationLogType::LogAdaptive) ;

    }

    {
        const NumericalSolver numericalSolver_RungeKuttaKashCarp54 = {  NumericalSolver::IntegrationLogType::NoLog , NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54 } ;
        EXPECT_EQ(numericalSolver_RungeKuttaKashCarp54.getIntegrationStepperType(),NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54) ;

        const NumericalSolver numericalSolver_RungeKuttaFehlberg78 = {  NumericalSolver::IntegrationLogType::NoLog , NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78 } ;
        EXPECT_EQ(numericalSolver_RungeKuttaFehlberg78.getIntegrationStepperType(),NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78) ;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, GetNumbers)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        // Define numbers
        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(numericalSolver.getTimeStep(), 5.0) ;

        EXPECT_EQ(numericalSolver.getRelativeTolerance(), 1.0e-15) ;

        EXPECT_EQ(numericalSolver.getAbsoluteTolerance(), 1.0e-15) ;

    }


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, StringFromType)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Matrix3d ;
    using ostk::math::obj::Vector3d ;

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        EXPECT_TRUE(NumericalSolver::StringFromIntegrationStepperType(NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54) == "RungeKuttaCashKarp54") ;
        EXPECT_TRUE(NumericalSolver::StringFromIntegrationStepperType(NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78) == "RungeKuttaFehlberg78") ;

    }

    {

        EXPECT_TRUE(NumericalSolver::StringFromIntegrationLogType(NumericalSolver::IntegrationLogType::NoLog) == "NoLog") ;
        EXPECT_TRUE(NumericalSolver::StringFromIntegrationLogType(NumericalSolver::IntegrationLogType::LogConstant) == "LogConstant") ;
        EXPECT_TRUE(NumericalSolver::StringFromIntegrationLogType(NumericalSolver::IntegrationLogType::LogAdaptive) == "LogAdaptive") ;

    }


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStateForDuration)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::flight::system::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;


    {
        // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in forward time

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(10000) ;

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-8, stateError[0]) ;
        EXPECT_GT(2e-8, stateError[1]) ;

    }

    {
        // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in backward time

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(-10000) ;

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-8, stateError[0]) ;
        EXPECT_GT(2e-8, stateError[1]) ;

    }

    {
        // Performance test with RungeKuttaFehlberg78 in forward time

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(10000) ;

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double  ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-10, stateError[0]) ;
        EXPECT_GT(2e-10, stateError[1]) ;

    }

    {
        // Performance test with RungeKuttaFehlberg78 in backward time

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(-10000) ;

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double  ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-10,stateError[0]) ;
        EXPECT_GT(2e-10,stateError[1]) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateInstantToInstant)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;
    using ostk::core::types::String ;
    using ostk::core::types::Integer ;

    using ostk::math::obj::Vector3d ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;
    using ostk::physics::env::obj::Celestial ;

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::flight::SatelliteSystem ;
    using ostk::astro::flight::system::SatelliteDynamics ;
    using ostk::astro::NumericalSolver ;


    {
        // Performance test with RungeKuttaCashKarp54 in forward time

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(10000) ;

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-8, stateError[0]) ;
        EXPECT_GT(2e-8, stateError[1]) ;

    }

    {
        // Performance test with RungeKuttaCashKarp54 in backwards time

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(-10000) ;

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-8, stateError[0]) ;
        EXPECT_GT(2e-8, stateError[1]) ;

    }

    {
        // Performance test with RungeKuttaFehlberg78 in forwards time

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(10000) ;

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double  ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-10, stateError[0]) ;
        EXPECT_GT(2e-10, stateError[1]) ;

    }

    {
        // Performance test with RungeKuttaFehlberg78 in backwards time

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(-10000) ;

        const NumericalSolver numericalSolver = { NumericalSolver::IntegrationLogType::NoLog, NumericalSolver::IntegrationStepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double  ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-10, stateError[0]) ;
        EXPECT_GT(2e-10, stateError[1]) ;

    }

}
