////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/NumericalSolver.test.cpp
/// @author         Antoine Paletta <antoine.paletta@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        EXPECT_NO_THROW(NumericalSolver numericalsolver(NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15)) ;

    }

    {

        const NumericalSolver numericalsolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_NO_THROW(NumericalSolver numericalsolverCopy(numericalsolver)) ;

    }

}

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {
        // Test equality of identical objects
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver == numericalSolver) ;

        // Test LogType
        const NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_FALSE(numericalSolver == numericalSolver_1) ;

        // Test StepperType
        const NumericalSolver numericalSolver_2 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_FALSE(numericalSolver == numericalSolver_2) ;

        // Test timeStep
        const NumericalSolver numericalSolver_3 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 4.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_FALSE(numericalSolver == numericalSolver_3) ;

        // Test relativeTolerance
        const NumericalSolver numericalSolver_4 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-14, 1.0e-15 } ;

        EXPECT_FALSE(numericalSolver == numericalSolver_4) ;

         // Test absoluteTolerance
        const NumericalSolver numericalSolver_5 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-14 } ;

        EXPECT_FALSE(numericalSolver == numericalSolver_5) ;

    }

}

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {
        // Test ineequality of identical objects
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_FALSE(numericalSolver != numericalSolver) ;

        // Test LogType
        const NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver != numericalSolver_1) ;

        // Test StepperType
        const NumericalSolver numericalSolver_2 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_TRUE(numericalSolver != numericalSolver_2) ;

        // Test timeStep
        const NumericalSolver numericalSolver_3 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 4.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver != numericalSolver_3) ;

        // Test relativeTolerance
        const NumericalSolver numericalSolver_4 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-14, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver != numericalSolver_4) ;

         // Test absoluteTolerance
        const NumericalSolver numericalSolver_5 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-14 } ;

        EXPECT_TRUE(numericalSolver != numericalSolver_5) ;

    }

}

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_TRUE(numericalSolver.isDefined()) ;

    }

}

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << numericalSolver << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(numericalSolver.print(std::cout, true)) ;
        EXPECT_NO_THROW(numericalSolver.print(std::cout, false)) ;
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        const NumericalSolver numericalSolver_NoLog = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(numericalSolver_NoLog.getLogType(),NumericalSolver::LogType::NoLog) ;

        const NumericalSolver numericalSolver_LogConstant = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(numericalSolver_LogConstant.getLogType(),NumericalSolver::LogType::LogConstant) ;

        const NumericalSolver numericalSolver_LogAdaptive = { NumericalSolver::LogType::LogAdaptive, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(numericalSolver_LogAdaptive.getLogType(),NumericalSolver::LogType::LogAdaptive) ;

    }

    {
        const NumericalSolver numericalSolver_RungeKuttaKashCarp54 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(numericalSolver_RungeKuttaKashCarp54.getStepperType(),NumericalSolver::StepperType::RungeKuttaCashKarp54) ;

        const NumericalSolver numericalSolver_RungeKuttaFehlberg78 = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(numericalSolver_RungeKuttaFehlberg78.getStepperType(),NumericalSolver::StepperType::RungeKuttaFehlberg78) ;
    }

}

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        // Define numbers
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        EXPECT_EQ(numericalSolver.getTimeStep(), 5.0) ;

        EXPECT_EQ(numericalSolver.getRelativeTolerance(), 1.0e-15) ;

        EXPECT_EQ(numericalSolver.getAbsoluteTolerance(), 1.0e-15) ;

    }


}

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

    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;

    using ostk::astro::NumericalSolver ;

    {

        EXPECT_TRUE(NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaCashKarp54) == "RungeKuttaCashKarp54") ;
        EXPECT_TRUE(NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaFehlberg78) == "RungeKuttaFehlberg78") ;

    }

    {

        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::NoLog) == "NoLog") ;
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::LogConstant) == "LogConstant") ;
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::LogAdaptive) == "LogAdaptive") ;

    }


}

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStatesAtSortedInstants)
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

    using ostk::astro::NumericalSolver ;

    // Performance test with RungeKuttaCashKarp54 and integrateStatesAtSortedInstants in forward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant startInstant = Instant::J2000() ;
        Array<Instant> instantArray = Array<Instant>::Empty() ;
        instantArray.add(startInstant + Duration::Seconds(100)) ;
        instantArray.add(startInstant + Duration::Seconds(400)) ;
        instantArray.add(startInstant + Duration::Seconds(700)) ;
        instantArray.add(startInstant + Duration::Seconds(1000)) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        Array<NumericalSolver::StateVector> propagatedStateVectorArray = numericalSolver.integrateStatesAtSortedInstants(currentStateVector, startInstant, instantArray, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;

        for (size_t i = 0; i < instantArray.size(); i++)
        {
            NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i] ;
            stateError[0] = std::abs(propagatedStateVector[0] - std::sin((instantArray[i]-startInstant).inSeconds())) ;
            stateError[1] = std::abs(propagatedStateVector[1] - std::cos((instantArray[i]-startInstant).inSeconds())) ;

            EXPECT_GT(2e-8, stateError[0]) ;
            EXPECT_GT(2e-8, stateError[1]) ;
        }

    }

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in backward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant startInstant = Instant::J2000() ;
        Array<Instant> instantArray = Array<Instant>::Empty() ;
        instantArray.add(startInstant + Duration::Seconds(-100)) ;
        instantArray.add(startInstant + Duration::Seconds(-400)) ;
        instantArray.add(startInstant + Duration::Seconds(-700)) ;
        instantArray.add(startInstant + Duration::Seconds(-1000)) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        Array<NumericalSolver::StateVector> propagatedStateVectorArray = numericalSolver.integrateStatesAtSortedInstants(currentStateVector, startInstant, instantArray, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;

        for (size_t i = 0; i < instantArray.size(); i++)
        {
            NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i] ;

            stateError[0] = std::abs(propagatedStateVector[0] - std::sin((instantArray[i]-startInstant).inSeconds())) ;
            stateError[1] = std::abs(propagatedStateVector[1] - std::cos((instantArray[i]-startInstant).inSeconds())) ;

            EXPECT_GT(2e-8, stateError[0]) ;
            EXPECT_GT(2e-8, stateError[1]) ;
        }

    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant startInstant = Instant::J2000() ;
        Array<Instant> instantArray = Array<Instant>::Empty() ;
        instantArray.add(startInstant + Duration::Seconds(100)) ;
        instantArray.add(startInstant + Duration::Seconds(400)) ;
        instantArray.add(startInstant + Duration::Seconds(700)) ;
        instantArray.add(startInstant + Duration::Seconds(1000)) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        Array<NumericalSolver::StateVector> propagatedStateVectorArray = numericalSolver.integrateStatesAtSortedInstants(currentStateVector, startInstant, instantArray, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;

        for (size_t i = 0; i < instantArray.size(); i++)
        {
            NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i] ;

            stateError[0] = std::abs(propagatedStateVector[0] - std::sin((instantArray[i]-startInstant).inSeconds())) ;
            stateError[1] = std::abs(propagatedStateVector[1] - std::cos((instantArray[i]-startInstant).inSeconds())) ;

            EXPECT_GT(2e-10, stateError[0]) ;
            EXPECT_GT(2e-10, stateError[1]) ;
        }

    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant startInstant = Instant::J2000() ;
        Array<Instant> instantArray = Array<Instant>::Empty() ;
        instantArray.add(startInstant + Duration::Seconds(-100)) ;
        instantArray.add(startInstant + Duration::Seconds(-400)) ;
        instantArray.add(startInstant + Duration::Seconds(-700)) ;
        instantArray.add(startInstant + Duration::Seconds(-1000)) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        Array<NumericalSolver::StateVector> propagatedStateVectorArray = numericalSolver.integrateStatesAtSortedInstants(currentStateVector, startInstant, instantArray, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;

        for (size_t i = 0; i < instantArray.size(); i++)
        {
            NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i] ;

            stateError[0] = std::abs(propagatedStateVector[0] - std::sin((instantArray[i]-startInstant).inSeconds())) ;
            stateError[1] = std::abs(propagatedStateVector[1] - std::cos((instantArray[i]-startInstant).inSeconds())) ;

            EXPECT_GT(2e-10, stateError[0]) ;
            EXPECT_GT(2e-10, stateError[1]) ;
        }

    }

}

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

    using ostk::astro::NumericalSolver ;

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in forward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(10000) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-8, stateError[0]) ;
        EXPECT_GT(2e-8, stateError[1]) ;

    }

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in backward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(-10000) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-8, stateError[0]) ;
        EXPECT_GT(2e-8, stateError[1]) ;

    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(10000) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double  ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-10, stateError[0]) ;
        EXPECT_GT(2e-10, stateError[1]) ;

    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(-10000) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double  ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-10,stateError[0]) ;
        EXPECT_GT(2e-10,stateError[1]) ;

    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaCashKarp54
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(1000) ;

        NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogAdaptive, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        NumericalSolver numericalSolver_2 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        testing::internal::CaptureStdout() ;
        NumericalSolver::StateVector propagatedStateVector_1 = numericalSolver_1.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;
        NumericalSolver::StateVector propagatedStateVector_2 = numericalSolver_2.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;
        testing::internal::GetCapturedStdout().empty() ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]) ;
        stateError[1] = std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]) ;

        // Check to make sure state error between integrate_const and integrate_adaptive is within a reasonable tolerance but that it is also not exactly zero since both integration methods do not exactly match
        EXPECT_GT(1e-9, stateError[0]) ;
        EXPECT_GT(1e-9, stateError[1]) ;
        EXPECT_FALSE(stateError[0] == 0.0) ;
        EXPECT_FALSE(stateError[1] == 0.0) ;

    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaFehlberg78
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Duration propDuration = Duration::Seconds(1000) ;

        NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogAdaptive, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        NumericalSolver numericalSolver_2 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        testing::internal::CaptureStdout() ;
        NumericalSolver::StateVector propagatedStateVector_1 = numericalSolver_1.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;
        NumericalSolver::StateVector propagatedStateVector_2 = numericalSolver_2.integrateStateForDuration(currentStateVector, propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;
        testing::internal::GetCapturedStdout().empty() ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]) ;
        stateError[1] = std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]) ;

        // Check to make sure state error between integrate_const and integrate_adaptive is within a reasonable tolerance but that it is also not exactly zero since both integration methods do not exactly match
        EXPECT_GT(1e-11, stateError[0]) ;
        EXPECT_GT(1e-11, stateError[1]) ;
        EXPECT_FALSE(stateError[0] == 0.0) ;
        EXPECT_FALSE(stateError[1] == 0.0) ;

    }
}

TEST (OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStateFromInstantToInstant)
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

    using ostk::astro::NumericalSolver ;

    // Performance test with RungeKuttaCashKarp54 in forward time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(10000) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-8, stateError[0]) ;
        EXPECT_GT(2e-8, stateError[1]) ;

    }

    // Performance test with RungeKuttaCashKarp54 in backwards time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(-10000) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-8, stateError[0]) ;
        EXPECT_GT(2e-8, stateError[1]) ;

    }

    // Performance test with RungeKuttaFehlberg78 in forwards time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(10000) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double  ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-10, stateError[0]) ;
        EXPECT_GT(2e-10, stateError[1]) ;

    }

    // Performance test with RungeKuttaFehlberg78 in backwards time
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(-10000) ;

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double  ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())) ;
        stateError[1] = std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())) ;

        EXPECT_GT(2e-10, stateError[0]) ;
        EXPECT_GT(2e-10, stateError[1]) ;

    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaCashKarp54
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(1000) ;

        NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogAdaptive, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;
        NumericalSolver numericalSolver_2 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15 } ;

        testing::internal::CaptureStdout() ;
        NumericalSolver::StateVector propagatedStateVector_1 = numericalSolver_1.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;
        NumericalSolver::StateVector propagatedStateVector_2 = numericalSolver_2.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;
        testing::internal::GetCapturedStdout().empty() ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]) ;
        stateError[1] = std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]) ;

        // Check to make sure state error between integrate_const and integrate_adaptive is within a reasonable tolerance but that it is also not exactly zero since both integration methods do not exactly match
        EXPECT_GT(1e-9, stateError[0]) ;
        EXPECT_GT(1e-9, stateError[1]) ;
        EXPECT_FALSE(stateError[0] == 0.0) ;
        EXPECT_FALSE(stateError[1] == 0.0) ;

    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaFehlberg78
    {

        // Setup initial state vector
        NumericalSolver::StateVector currentStateVector(2) ;
        currentStateVector[0] = 0 ; currentStateVector[1] = 1 ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(1000) ;

        NumericalSolver numericalSolver_1 = { NumericalSolver::LogType::LogAdaptive, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;
        NumericalSolver numericalSolver_2 = { NumericalSolver::LogType::LogConstant, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15 } ;

        testing::internal::CaptureStdout() ;
        NumericalSolver::StateVector propagatedStateVector_1 = numericalSolver_1.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;
        NumericalSolver::StateVector propagatedStateVector_2 = numericalSolver_2.integrateStateFromInstantToInstant(currentStateVector, instant, instant + propDuration, [=]( const NumericalSolver::StateVector &x , NumericalSolver::StateVector &dxdt , const double ) -> void { dxdt[0] = x[1]; dxdt[1] = -x[0]; } ) ;
        testing::internal::GetCapturedStdout().empty() ;

        // Validate the output against an analytical function
        NumericalSolver::StateVector stateError(2) ;
        stateError[0] = std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]) ;
        stateError[1] = std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]) ;

        // Check to make sure state error between integrate_const and integrate_adaptive is within a reasonable tolerance but that it is also not exactly zero since both integration methods do not exactly match
        EXPECT_GT(1e-11, stateError[0]) ;
        EXPECT_GT(1e-11, stateError[1]) ;
        EXPECT_FALSE(stateError[0] == 0.0) ;
        EXPECT_FALSE(stateError[1] == 0.0) ;

    }
}
