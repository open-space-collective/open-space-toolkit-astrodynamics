/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astro::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_NumericalSolver : public ::testing::Test
{
   protected:
    NumericalSolver defaultRK54_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaCashKarp54,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    NumericalSolver defaultRKF78_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-15,
        1.0e-15,
    };

    NumericalSolver defaultRK4_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKutta4,
        1e-3,
        1.0e-15,
        1.0e-15,
    };

    const NumericalSolver::StateVector defaultStateVector_ = {0, 1};
    const Duration defaultDuration_ = Duration::Seconds(10);
    const Instant defaultStartInstant_ = Instant::J2000();

    const std::function<void(const NumericalSolver::StateVector &, NumericalSolver::StateVector &, const double)>
        systemOfEquations_ =
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
    {
        dxdt[0] = x[1];
        dxdt[1] = -x[0];
    };

    void validatePropagatedStates(
        const Array<Instant> &anInstantArray,
        const Array<NumericalSolver::StateVector> &aStateVectorArray,
        const double &aTolerance
    )
    {
        for (size_t i = 0; i < anInstantArray.size(); i++)
        {
            const NumericalSolver::StateVector propagatedStateVector = aStateVectorArray[i];

            EXPECT_GT(
                aTolerance,
                std::abs(propagatedStateVector[0] - std::sin((anInstantArray[i] - defaultStartInstant_).inSeconds()))
            );
            EXPECT_GT(
                aTolerance,
                std::abs(propagatedStateVector[1] - std::cos((anInstantArray[i] - defaultStartInstant_).inSeconds()))
            );
        }
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, Constructor)
{
    {
        EXPECT_NO_THROW(NumericalSolver numericalsolver(
            NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaCashKarp54, 5.0, 1.0e-15, 1.0e-15
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, CopyConstructor)
{
    {
        EXPECT_NO_THROW(NumericalSolver numericalsolverCopy(defaultRK54_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultRK54_ == defaultRK54_);

        // Test LogType
        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        EXPECT_FALSE(defaultRK54_ == numericalSolver_1);

        // Test StepperType
        const NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        EXPECT_FALSE(defaultRK54_ == numericalSolver_2);

        // Test timeStep
        const NumericalSolver numericalSolver_3 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            4.0,
            1.0e-15,
            1.0e-15,
        };

        EXPECT_FALSE(defaultRK54_ == numericalSolver_3);

        // Test relativeTolerance
        const NumericalSolver numericalSolver_4 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-14,
            1.0e-15,
        };

        EXPECT_FALSE(defaultRK54_ == numericalSolver_4);

        // Test absoluteTolerance
        const NumericalSolver numericalSolver_5 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-14,
        };

        EXPECT_FALSE(defaultRK54_ == numericalSolver_5);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultRK54_ != defaultRK54_);

        // Test LogType
        const NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15,
        };

        EXPECT_TRUE(defaultRK54_ != numericalSolver_1);

        // Test StepperType
        const NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        EXPECT_TRUE(defaultRK54_ != numericalSolver_2);

        // Test timeStep
        const NumericalSolver numericalSolver_3 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            4.0,
            1.0e-15,
            1.0e-15,
        };

        EXPECT_TRUE(defaultRK54_ != numericalSolver_3);

        // Test relativeTolerance
        const NumericalSolver numericalSolver_4 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-14,
            1.0e-15,
        };

        EXPECT_TRUE(defaultRK54_ != numericalSolver_4);

        // Test absoluteTolerance
        const NumericalSolver numericalSolver_5 = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-14,
        };

        EXPECT_TRUE(defaultRK54_ != numericalSolver_5);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IsDefined)
{
    {
        EXPECT_TRUE(defaultRK54_.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultRK54_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultRK54_.print(std::cout, true));
        EXPECT_NO_THROW(defaultRK54_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, GetType)
{
    {
        EXPECT_EQ(defaultRK54_.getLogType(), NumericalSolver::LogType::NoLog);

        const NumericalSolver numericalSolver_LogConstant = {
            NumericalSolver::LogType::LogConstant,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        EXPECT_EQ(numericalSolver_LogConstant.getLogType(), NumericalSolver::LogType::LogConstant);

        const NumericalSolver numericalSolver_LogAdaptive = {
            NumericalSolver::LogType::LogAdaptive,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            5.0,
            1.0e-15,
            1.0e-15,
        };
        EXPECT_EQ(numericalSolver_LogAdaptive.getLogType(), NumericalSolver::LogType::LogAdaptive);
    }

    {
        EXPECT_EQ(defaultRK54_.getStepperType(), NumericalSolver::StepperType::RungeKuttaCashKarp54);
        EXPECT_EQ(defaultRKF78_.getStepperType(), NumericalSolver::StepperType::RungeKuttaFehlberg78);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, GetNumbers)
{
    {
        EXPECT_EQ(defaultRK4_.getTimeStep(), 1e-3);
        EXPECT_EQ(defaultRK4_.getRelativeTolerance(), 1.0e-15);
        EXPECT_EQ(defaultRK4_.getAbsoluteTolerance(), 1.0e-15);
    }

    {
        EXPECT_EQ(defaultRK54_.getTimeStep(), 5.0);
        EXPECT_EQ(defaultRK54_.getRelativeTolerance(), 1.0e-15);
        EXPECT_EQ(defaultRK54_.getAbsoluteTolerance(), 1.0e-15);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, StringFromType)
{
    {
        EXPECT_TRUE(NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKutta4) == "RungeKutta4");
        EXPECT_TRUE(
            NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaCashKarp54) ==
            "RungeKuttaCashKarp54"
        );
        EXPECT_TRUE(
            NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaFehlberg78) ==
            "RungeKuttaFehlberg78"
        );
    }

    {
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::NoLog) == "NoLog");
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::LogConstant) == "LogConstant");
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::LogAdaptive) == "LogAdaptive");
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStatesAtSortedInstants)
{
    // Performance test with RungeKutta4 in forward time
    {
        const Array<Instant> instantArray = {
            defaultStartInstant_ + Duration::Seconds(10),
            defaultStartInstant_ + Duration::Seconds(40),
            defaultStartInstant_ + Duration::Seconds(70),
        };

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            defaultRK4_.integrateStatesAtSortedInstants(
                defaultStateVector_, defaultStartInstant_, instantArray, systemOfEquations_
            );

        validatePropagatedStates(instantArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKutta4 in backward time
    {
        const Array<Instant> instantArray = {
            defaultStartInstant_ + Duration::Seconds(-10),
            defaultStartInstant_ + Duration::Seconds(-40),
            defaultStartInstant_ + Duration::Seconds(-70),
        };

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            defaultRK4_.integrateStatesAtSortedInstants(
                defaultStateVector_, defaultStartInstant_, instantArray, systemOfEquations_
            );

        validatePropagatedStates(instantArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKuttaCashKarp54 and integrateStatesAtSortedInstants in forward time
    {
        const Array<Instant> instantArray = {
            defaultStartInstant_ + Duration::Seconds(100),
            defaultStartInstant_ + Duration::Seconds(400),
            defaultStartInstant_ + Duration::Seconds(700),
            defaultStartInstant_ + Duration::Seconds(1000),
        };

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            defaultRK54_.integrateStatesAtSortedInstants(
                defaultStateVector_, defaultStartInstant_, instantArray, systemOfEquations_
            );

        validatePropagatedStates(instantArray, propagatedStateVectorArray, 2e-8);
    }

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in backward time
    {
        const Array<Instant> instantArray = {
            defaultStartInstant_ + Duration::Seconds(-100),
            defaultStartInstant_ + Duration::Seconds(-400),
            defaultStartInstant_ + Duration::Seconds(-700),
            defaultStartInstant_ + Duration::Seconds(-1000),
        };

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            defaultRK54_.integrateStatesAtSortedInstants(
                defaultStateVector_, defaultStartInstant_, instantArray, systemOfEquations_
            );

        validatePropagatedStates(instantArray, propagatedStateVectorArray, 2e-8);
    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {
        const Array<Instant> instantArray = {
            defaultStartInstant_ + Duration::Seconds(100),
            defaultStartInstant_ + Duration::Seconds(400),
            defaultStartInstant_ + Duration::Seconds(700),
            defaultStartInstant_ + Duration::Seconds(1000),
        };

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            defaultRKF78_.integrateStatesAtSortedInstants(
                defaultStateVector_, defaultStartInstant_, instantArray, systemOfEquations_
            );

        validatePropagatedStates(instantArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {
        const Array<Instant> instantArray = {
            defaultStartInstant_ + Duration::Seconds(-100),
            defaultStartInstant_ + Duration::Seconds(-400),
            defaultStartInstant_ + Duration::Seconds(-700),
            defaultStartInstant_ + Duration::Seconds(-1000),
        };

        const Array<NumericalSolver::StateVector> propagatedStateVectorArray =
            defaultRKF78_.integrateStatesAtSortedInstants(
                defaultStateVector_, defaultStartInstant_, instantArray, systemOfEquations_
            );

        validatePropagatedStates(instantArray, propagatedStateVectorArray, 2e-10);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStateForDuration)
{
    // Performance test with RungeKutta4 in forward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK4_.integrateStateForDuration(defaultStateVector_, defaultDuration_, systemOfEquations_);

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKutta4 in backward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK4_.integrateStateForDuration(defaultStateVector_, -defaultDuration_, systemOfEquations_);

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in forward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK54_.integrateStateForDuration(defaultStateVector_, defaultDuration_, systemOfEquations_);

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_.inSeconds())));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKuttaCashKarp54 and integrateStateForDuration in backward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK54_.integrateStateForDuration(defaultStateVector_, -defaultDuration_, systemOfEquations_);

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_.inSeconds())));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRKF78_.integrateStateForDuration(defaultStateVector_, defaultDuration_, systemOfEquations_);

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRKF78_.integrateStateForDuration(defaultStateVector_, -defaultDuration_, systemOfEquations_);

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_.inSeconds())));
    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaCashKarp54
    {
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
            numericalSolver_1.integrateStateForDuration(defaultStateVector_, defaultDuration_, systemOfEquations_);
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateStateForDuration(defaultStateVector_, defaultDuration_, systemOfEquations_);
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
            numericalSolver_1.integrateStateForDuration(defaultStateVector_, defaultDuration_, systemOfEquations_);
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateStateForDuration(defaultStateVector_, defaultDuration_, systemOfEquations_);
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

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateStateFromInstantToInstant)
{
    // Performance test with RungeKutta4 in forwards time
    {
        const NumericalSolver::StateVector propagatedStateVector = defaultRK4_.integrateStateFromInstantToInstant(
            defaultStateVector_, defaultStartInstant_, defaultStartInstant_ + defaultDuration_, systemOfEquations_
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKutta4 in backwards time
    {
        const NumericalSolver::StateVector propagatedStateVector = defaultRK4_.integrateStateFromInstantToInstant(
            defaultStateVector_, defaultStartInstant_, defaultStartInstant_ - defaultDuration_, systemOfEquations_
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKuttaCashKarp54 in forward time
    {
        const NumericalSolver::StateVector propagatedStateVector = defaultRK54_.integrateStateFromInstantToInstant(
            defaultStateVector_, defaultStartInstant_, defaultStartInstant_ + defaultDuration_, systemOfEquations_
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_.inSeconds())));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKuttaCashKarp54 in backwards time
    {
        const NumericalSolver::StateVector propagatedStateVector = defaultRK54_.integrateStateFromInstantToInstant(
            defaultStateVector_, defaultStartInstant_, defaultStartInstant_ - defaultDuration_, systemOfEquations_
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_.inSeconds())));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKuttaFehlberg78 in forwards time
    {
        const NumericalSolver::StateVector propagatedStateVector = defaultRKF78_.integrateStateFromInstantToInstant(
            defaultStateVector_, defaultStartInstant_, defaultStartInstant_ + defaultDuration_, systemOfEquations_
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_.inSeconds())));
    }

    // Performance test with RungeKuttaFehlberg78 in backwards time
    {
        const NumericalSolver::StateVector propagatedStateVector = defaultRKF78_.integrateStateFromInstantToInstant(
            defaultStateVector_, defaultStartInstant_, defaultStartInstant_ - defaultDuration_, systemOfEquations_
        );

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_.inSeconds())));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_.inSeconds())));
    }

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaCashKarp54
    {
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
                defaultStateVector_,
                defaultStartInstant_,
                defaultStartInstant_ + defaultDuration_,
                [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateStateFromInstantToInstant(
                defaultStateVector_,
                defaultStartInstant_,
                defaultStartInstant_ + defaultDuration_,
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
                defaultStateVector_,
                defaultStartInstant_,
                defaultStartInstant_ + defaultDuration_,
                [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                {
                    dxdt[0] = x[1];
                    dxdt[1] = -x[0];
                }
            );
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateStateFromInstantToInstant(
                defaultStateVector_,
                defaultStartInstant_,
                defaultStartInstant_ + defaultDuration_,
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

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, Undefined)
{
    {
        EXPECT_NO_THROW(NumericalSolver::Undefined());
    }

    {
        EXPECT_FALSE(NumericalSolver::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, Default)
{
    {
        EXPECT_NO_THROW(NumericalSolver::Default());
    }

    {
        EXPECT_TRUE(NumericalSolver::Default().isDefined());
    }
}
