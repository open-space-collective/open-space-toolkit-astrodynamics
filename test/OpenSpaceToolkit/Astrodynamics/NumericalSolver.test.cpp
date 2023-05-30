/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, Constructor)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        EXPECT_NO_THROW(NumericalSolver numericalsolver(
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15
        ));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, CopyConstructor)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        const NumericalSolver numericalsolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};
        EXPECT_NO_THROW(NumericalSolver numericalsolverCopy(numericalsolver));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, EqualToOperator)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        // Test equality of identical objects
        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        EXPECT_TRUE(numericalSolver == numericalSolver);

        // Test LogType
        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15};
        EXPECT_FALSE(numericalSolver == numericalSolver_1);

        // Test StepperType
        const NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};
        EXPECT_FALSE(numericalSolver == numericalSolver_2);

        // Test timeStep
        const NumericalSolver numericalSolver_3 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 4.0, 1.0e-15, 1.0e-15};

        EXPECT_FALSE(numericalSolver == numericalSolver_3);

        // Test relativeTolerance
        const NumericalSolver numericalSolver_4 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-14, 1.0e-15};

        EXPECT_FALSE(numericalSolver == numericalSolver_4);

        // Test absoluteTolerance
        const NumericalSolver numericalSolver_5 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-14};

        EXPECT_FALSE(numericalSolver == numericalSolver_5);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, NotEqualToOperator)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        // Test ineequality of identical objects
        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        EXPECT_FALSE(numericalSolver != numericalSolver);

        // Test LogType
        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15};

        EXPECT_TRUE(numericalSolver != numericalSolver_1);

        // Test StepperType
        const NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};
        EXPECT_TRUE(numericalSolver != numericalSolver_2);

        // Test timeStep
        const NumericalSolver numericalSolver_3 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 4.0, 1.0e-15, 1.0e-15};

        EXPECT_TRUE(numericalSolver != numericalSolver_3);

        // Test relativeTolerance
        const NumericalSolver numericalSolver_4 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-14, 1.0e-15};

        EXPECT_TRUE(numericalSolver != numericalSolver_4);

        // Test absoluteTolerance
        const NumericalSolver numericalSolver_5 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-14};

        EXPECT_TRUE(numericalSolver != numericalSolver_5);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IsDefined)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        EXPECT_TRUE(numericalSolver.isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, StreamOperator)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << numericalSolver << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, Print)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(numericalSolver.print(std::cout, true));
        EXPECT_NO_THROW(numericalSolver.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, GetType)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        const NumericalSolver numericalSolver_NoLog = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};
        EXPECT_EQ(numericalSolver_NoLog.getLogType(), NumericalSolver::LogType::NoLog);

        const NumericalSolver numericalSolver_LogConstant = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15};
        EXPECT_EQ(numericalSolver_LogConstant.getLogType(), NumericalSolver::LogType::LogConstant);

        const NumericalSolver numericalSolver_LogAdaptive = {
            NumericalSolver::LogType::LogAdaptive,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15};
        EXPECT_EQ(numericalSolver_LogAdaptive.getLogType(), NumericalSolver::LogType::LogAdaptive);
    }

    {
        const NumericalSolver numericalSolver_RungeKuttaKashCarp54 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};
        EXPECT_EQ(
            numericalSolver_RungeKuttaKashCarp54.getStepperType(), NumericalSolver::StepperType::RungeKuttaCashKarp54
        );

        const NumericalSolver numericalSolver_RungeKuttaFehlberg78 = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};
        EXPECT_EQ(
            numericalSolver_RungeKuttaFehlberg78.getStepperType(), NumericalSolver::StepperType::RungeKuttaFehlberg78
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, GetNumbers)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        EXPECT_EQ(numericalSolver.getTimeStep(), 5.0);
        EXPECT_EQ(numericalSolver.getRelativeTolerance(), 1.0e-15);
        EXPECT_EQ(numericalSolver.getAbsoluteTolerance(), 1.0e-15);
    }
<<<<<<< HEAD
=======

    {

        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKutta4, 5.0, 1.0e-15, 1.0e-15 } ;

        EXPECT_EQ(numericalSolver.getTimeStep(), 5.0) ;
        EXPECT_EQ(numericalSolver.getRelativeTolerance(), 1.0e-15) ;
        EXPECT_EQ(numericalSolver.getAbsoluteTolerance(), 1.0e-15) ;

    }

>>>>>>> test: add perf tests for new RK4 integrator wrapper
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, StringFromType)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    {
<<<<<<< HEAD
        EXPECT_TRUE(
            NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaCashKarp54) ==
            "RungeKuttaCashKarp54"
        );
        EXPECT_TRUE(
            NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaFehlberg78) ==
            "RungeKuttaFehlberg78"
        );
=======

        EXPECT_TRUE(NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaCashKarp54) == "RungeKuttaCashKarp54") ;
        EXPECT_TRUE(NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaFehlberg78) == "RungeKuttaFehlberg78") ;
        EXPECT_TRUE(NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKutta4) == "RungeKutta4") ;

>>>>>>> test: add perf tests for new RK4 integrator wrapper
    }

    {
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::NoLog) == "NoLog");
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::LogConstant) == "LogConstant");
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::LogAdaptive) == "LogAdaptive");
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStatesAtSortedInstants)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    // Performance test with RungeKuttaCashKarp54 and integrateStatesAtSortedInstants in forward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};

        const Instant startInstant = Instant::J2000();

        const Array<Instant> instantArray = {
            startInstant + Duration::Seconds(100),
            startInstant + Duration::Seconds(400),
            startInstant + Duration::Seconds(700),
            startInstant + Duration::Seconds(1000)};

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            numericalSolver.integrateStatesAtSortedInstants(
                currentStateVector,
                startInstant,
                instantArray,
                [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );

        // Validate the output against an analytical function

        for (size_t i = 0; i < instantArray.size(); i++)
        {
            const NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i];

            EXPECT_GT(
                2e-8, std::abs(propagatedStateVector[0] - std::sin((instantArray[i] - startInstant).inSeconds()))
            );
            EXPECT_GT(
                2e-8, std::abs(propagatedStateVector[1] - std::cos((instantArray[i] - startInstant).inSeconds()))
            );
        }
    }

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in backward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};

        const Instant startInstant = Instant::J2000();

        const Array<Instant> instantArray = {
            startInstant + Duration::Seconds(-100),
            startInstant + Duration::Seconds(-400),
            startInstant + Duration::Seconds(-700),
            startInstant + Duration::Seconds(-1000)};

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            numericalSolver.integrateStatesAtSortedInstants(
                currentStateVector,
                startInstant,
                instantArray,
                [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );

        // Validate the output against an analytical function

        for (size_t i = 0; i < instantArray.size(); i++)
        {
            const NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i];

            EXPECT_GT(
                2e-8, std::abs(propagatedStateVector[0] - std::sin((instantArray[i] - startInstant).inSeconds()))
            );
            EXPECT_GT(
                2e-8, std::abs(propagatedStateVector[1] - std::cos((instantArray[i] - startInstant).inSeconds()))
            );
        }
    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};

        const Instant startInstant = Instant::J2000();

        const Array<Instant> instantArray = {
            startInstant + Duration::Seconds(100),
            startInstant + Duration::Seconds(400),
            startInstant + Duration::Seconds(700),
            startInstant + Duration::Seconds(1000)};

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            numericalSolver.integrateStatesAtSortedInstants(
                currentStateVector,
                startInstant,
                instantArray,
                [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );

        // Validate the output against an analytical function

        for (size_t i = 0; i < instantArray.size(); i++)
        {
            const NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i];

            EXPECT_GT(
                2e-10, std::abs(propagatedStateVector[0] - std::sin((instantArray[i] - startInstant).inSeconds()))
            );
            EXPECT_GT(
                2e-10, std::abs(propagatedStateVector[1] - std::cos((instantArray[i] - startInstant).inSeconds()))
            );
        }
    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};

        const Instant startInstant = Instant::J2000();

        const Array<Instant> instantArray = {
            startInstant + Duration::Seconds(-100),
            startInstant + Duration::Seconds(-400),
            startInstant + Duration::Seconds(-700),
            startInstant + Duration::Seconds(-1000)};

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            numericalSolver.integrateStatesAtSortedInstants(
                currentStateVector,
                startInstant,
                instantArray,
                [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );

        // Validate the output against an analytical function

        for (size_t i = 0; i < instantArray.size(); i++)
        {
            const NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i];

            EXPECT_GT(
                2e-10, std::abs(propagatedStateVector[0] - std::sin((instantArray[i] - startInstant).inSeconds()))
            );
            EXPECT_GT(
                2e-10, std::abs(propagatedStateVector[1] - std::cos((instantArray[i] - startInstant).inSeconds()))
            );
        }
    }
<<<<<<< HEAD
=======

    // Performance test with RungeKutta4 in forward time
    {

        const NumericalSolver::StateVector currentStateVector = { 0, 1 } ;

        const Instant startInstant = Instant::J2000() ;

        const Array<Instant> instantArray =
        {
            startInstant + Duration::Seconds(10),
            startInstant + Duration::Seconds(40),
            startInstant + Duration::Seconds(70)
        } ;

        // needs very small step size
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKutta4, 0.001, 1.0e-15, 1.0e-15 } ;

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray = numericalSolver.integrateStatesAtSortedInstants
        (
            currentStateVector,
            startInstant,
            instantArray,
            [] (const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double ) -> void
            {
                dxdt[0] = x[1] ;
                dxdt[1] = -x[0] ;
            }
        ) ;

        // Validate the output against an analytical function

        for (size_t i = 0; i < instantArray.size(); i++)
        {

            const NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i] ;

            EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin((instantArray[i] - startInstant).inSeconds()))) ;
            EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos((instantArray[i] - startInstant).inSeconds()))) ;

        }

    }

    // Performance test with RungeKutta4 in backward time
    {

        const NumericalSolver::StateVector currentStateVector = { 0, 1 } ;

        const Instant startInstant = Instant::J2000() ;

        const Array<Instant> instantArray =
        {
            startInstant + Duration::Seconds(-10),
            startInstant + Duration::Seconds(-40),
            startInstant + Duration::Seconds(-70)
        } ;

        // needs very small step size
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKutta4, 0.001, 1.0e-15, 1.0e-15 } ;

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray = numericalSolver.integrateStatesAtSortedInstants
        (
            currentStateVector,
            startInstant,
            instantArray,
            [] (const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double ) -> void
            {
                dxdt[0] = x[1] ;
                dxdt[1] = -x[0] ;
            }
        ) ;

        // Validate the output against an analytical function

        for (size_t i = 0; i < instantArray.size(); i++)
        {

            const NumericalSolver::StateVector propagatedStateVector = propagatedStateVectorArray[i] ;

            EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin((instantArray[i] - startInstant).inSeconds()))) ;
            EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos((instantArray[i] - startInstant).inSeconds()))) ;

        }

    }

>>>>>>> test: add perf tests for new RK4 integrator wrapper
}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStateForDuration)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in forward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};

        const Duration propDuration = Duration::Seconds(10000);

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(
            currentStateVector,
            propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())));
    }

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in backward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Duration propDuration = Duration::Seconds(-10000);

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(
            currentStateVector,
            propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())));
    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Duration propDuration = Duration::Seconds(10000);

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(
            currentStateVector,
            propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())));
    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Duration propDuration = Duration::Seconds(-10000);

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration(
            currentStateVector,
            propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())));
    }

    // Performance test with RungeKutta4 in forward time
    {

        const NumericalSolver::StateVector currentStateVector = { 0, 1 } ;
        const Duration propDuration = Duration::Seconds(10) ;

        // needs very small step size
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKutta4, 0.001, 1.0e-15, 1.0e-15 } ;

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration
        (
            currentStateVector,
            propDuration,
            [] (const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double  ) -> void
            {
                dxdt[0] = x[1] ;
                dxdt[1] = -x[0] ;
            }
        ) ;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds()))) ;
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds()))) ;

    }

    // Performance test with RungeKutta4 in backward time
    {

        const NumericalSolver::StateVector currentStateVector = { 0, 1 } ;
        const Duration propDuration = Duration::Seconds(-10) ;

        // needs very small step size
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKutta4, 0.001, 1.0e-15, 1.0e-15 } ;

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateForDuration
        (
            currentStateVector,
            propDuration,
            [] (const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double  ) -> void
            {
                dxdt[0] = x[1] ;
                dxdt[1] = -x[0] ;
            }
        ) ;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds()))) ;
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds()))) ;

    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaCashKarp54
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Duration propDuration = Duration::Seconds(1000);

        NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogAdaptive,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15};
        NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15};

        testing::internal::CaptureStdout();
        const NumericalSolver::StateVector propagatedStateVector_1 = numericalSolver_1.integrateStateForDuration(
            currentStateVector,
            propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );
        const NumericalSolver::StateVector propagatedStateVector_2 = numericalSolver_2.integrateStateForDuration(
            currentStateVector,
            propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );
        String output = testing::internal::GetCapturedStdout();

        EXPECT_FALSE(output.empty());

        // Validate the output against an analytical function

        // Check to make sure state error between integrate_const and integrate_adaptive is within a reasonable
        // tolerance but that it is also not exactly zero since both integration methods do not exactly match
        EXPECT_GT(1e-9, std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]));
        EXPECT_GT(1e-9, std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]));

        EXPECT_FALSE(std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]) == 0.0);
        EXPECT_FALSE(std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]) == 0.0);
    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaFehlberg78
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Duration propDuration = Duration::Seconds(1000);

        NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogAdaptive,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};

        testing::internal::CaptureStdout();
        const NumericalSolver::StateVector propagatedStateVector_1 = numericalSolver_1.integrateStateForDuration(
            currentStateVector,
            propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );
        const NumericalSolver::StateVector propagatedStateVector_2 = numericalSolver_2.integrateStateForDuration(
            currentStateVector,
            propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );
        String output = testing::internal::GetCapturedStdout();

        EXPECT_FALSE(output.empty());

        // Validate the output against an analytical function
        // Check to make sure state error between integrate_const and integrate_adaptive is within a reasonable
        // tolerance but that it is also not exactly zero since both integration methods do not exactly match
        EXPECT_GT(1e-11, std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]));
        EXPECT_GT(1e-11, std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]));
        EXPECT_FALSE(std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]) == 0.0);
        EXPECT_FALSE(std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]) == 0.0);
    }

}

TEST(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStateFromInstantToInstant)
{
    using ostk::core::ctnr::Array;
    using ostk::core::types::Integer;
    using ostk::core::types::Real;
    using ostk::core::types::String;

    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;

    using ostk::astro::NumericalSolver;

    // Performance test with RungeKuttaCashKarp54 in forward time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Instant instant = Instant::J2000();
        const Duration propDuration = Duration::Seconds(10000);

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(
            currentStateVector,
            instant,
            instant + propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())));
    }

    // Performance test with RungeKuttaCashKarp54 in backwards time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Instant instant = Instant::J2000();
        const Duration propDuration = Duration::Seconds(-10000);

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15};

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(
            currentStateVector,
            instant,
            instant + propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())));
    }

    // Performance test with RungeKuttaFehlberg78 in forwards time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Instant instant = Instant::J2000();
        const Duration propDuration = Duration::Seconds(10000);

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(
            currentStateVector,
            instant,
            instant + propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())));
    }

    // Performance test with RungeKuttaFehlberg78 in backwards time
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Instant instant = Instant::J2000();
        const Duration propDuration = Duration::Seconds(-10000);

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-15, 1.0e-15};

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant(
            currentStateVector,
            instant,
            instant + propDuration,
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
            {
                dxdt[0] = x[1];
                dxdt[1] = -x[0];
            }
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds())));
    }

    // Performance test with RungeKutta4 in forwards time
    {

        const NumericalSolver::StateVector currentStateVector = { 0, 1 } ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(100) ;

        // needs very small step size
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKutta4, 0.001, 1.0e-15, 1.0e-15 } ;

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant
        (
            currentStateVector,
            instant,
            instant + propDuration,
            [] (const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double  ) -> void
            {
                dxdt[0] = x[1] ;
                dxdt[1] = -x[0] ;
            }
        ) ;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds()))) ;
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds()))) ;

    }

    // Performance test with RungeKutta4 in backwards time
    {

        const NumericalSolver::StateVector currentStateVector = { 0, 1 } ;
        const Instant instant = Instant::J2000() ;
        const Duration propDuration = Duration::Seconds(-100) ;

        // needs very small step size
        NumericalSolver numericalSolver = { NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKutta4, 0.001, 1.0e-15, 1.0e-15 } ;

        const NumericalSolver::StateVector propagatedStateVector = numericalSolver.integrateStateFromInstantToInstant
        (
            currentStateVector,
            instant,
            instant + propDuration,
            [] (const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double  ) -> void
            {
                dxdt[0] = x[1] ;
                dxdt[1] = -x[0] ;
            }
        ) ;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propDuration.inSeconds()))) ;
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propDuration.inSeconds()))) ;

    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaCashKarp54
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Instant instant = Instant::J2000();
        const Duration propDuration = Duration::Seconds(1000);

        NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogAdaptive,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15};
        NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15};

        testing::internal::CaptureStdout();
        const NumericalSolver::StateVector propagatedStateVector_1 =
            numericalSolver_1.integrateStateFromInstantToInstant(
                currentStateVector,
                instant,
                instant + propDuration,
                [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateStateFromInstantToInstant(
                currentStateVector,
                instant,
                instant + propDuration,
                [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );
        String output = testing::internal::GetCapturedStdout();

        EXPECT_FALSE(output.empty());

        // Validate the output against an analytical function
        // Check to make sure state error between integrate_const and integrate_adaptive is within a reasonable
        // tolerance but that it is also not exactly zero since both integration methods do not exactly match
        EXPECT_GT(1e-9, std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]));
        EXPECT_GT(1e-9, std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]));
        EXPECT_FALSE(std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]) == 0.0);
        EXPECT_FALSE(std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]) == 0.0);
    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaFehlberg78
    {
        const NumericalSolver::StateVector currentStateVector = {0, 1};
        const Instant instant = Instant::J2000();
        const Duration propDuration = Duration::Seconds(1000);

        NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogAdaptive,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};
        NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15};

        testing::internal::CaptureStdout();
        const NumericalSolver::StateVector propagatedStateVector_1 =
            numericalSolver_1.integrateStateFromInstantToInstant(
                currentStateVector,
                instant,
                instant + propDuration,
                [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateStateFromInstantToInstant(
                currentStateVector,
                instant,
                instant + propDuration,
                [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );
        String output = testing::internal::GetCapturedStdout();

        EXPECT_FALSE(output.empty());

        // Validate the output against an analytical function
        // Check to make sure state error between integrate_const and integrate_adaptive is within a reasonable
        // tolerance but that it is also not exactly zero since both integration methods do not exactly match
        EXPECT_GT(1e-11, std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]));
        EXPECT_GT(1e-11, std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]));
        EXPECT_FALSE(std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]) == 0.0);
        EXPECT_FALSE(std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]) == 0.0);
    }
}
