/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::String;

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
    const Real defaultDuration_ = 10.0;
    const Real defaultStartTime_ = 0.0;

    const std::function<void(const NumericalSolver::StateVector &, NumericalSolver::StateVector &, const double)>
        systemOfEquations_ =
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
    {
        dxdt[0] = x[1];
        dxdt[1] = -x[0];
    };

    void validatePropagatedStates(
        const Array<Real> &aTimeArray, const Array<NumericalSolver::Solution> &aSolutionArray, const double &aTolerance
    )
    {
        for (size_t i = 0; i < aTimeArray.size(); i++)
        {
            const NumericalSolver::StateVector propagatedStateVector = aSolutionArray[i].first;

            EXPECT_GT(aTolerance, std::abs(propagatedStateVector[0] - std::sin(aTimeArray[i])));
            EXPECT_GT(aTolerance, std::abs(propagatedStateVector[1] - std::cos(aTimeArray[i])));
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

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, integrateTimes)
{
    // Performance test with RungeKutta4 in forward time
    {
        const Array<Real> aTimeArray = {
            10.0,
            40.0,
            70.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRK4_.integrateTimes(defaultStateVector_, defaultStartTime_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKutta4 in backward time
    {
        const Array<Real> aTimeArray = {
            -10.0,
            -40.0,
            -70.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRK4_.integrateTimes(defaultStateVector_, defaultStartTime_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKuttaCashKarp54 and integrateTimes in forward time
    {
        const Array<Real> aTimeArray = {
            100.0,
            400.0,
            700.0,
            1000.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRK54_.integrateTimes(defaultStateVector_, defaultStartTime_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-8);
    }

    // Performance test with RungeKuttaCashKarp54 and integrateDurations in backward time
    {
        const Array<Real> aTimeArray = {
            -100.0,
            -400.0,
            -700.0,
            -1000.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRK54_.integrateTimes(defaultStateVector_, defaultStartTime_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-8);
    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {
        const Array<Real> aTimeArray = {
            100.0,
            400.0,
            700.0,
            1000.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRKF78_.integrateTimes(defaultStateVector_, defaultStartTime_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {
        const Array<Real> aTimeArray = {
            -100.0,
            -400.0,
            -700.0,
            -1000.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRKF78_.integrateTimes(defaultStateVector_, defaultStartTime_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-10);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, integrateDuration)
{
    // Performance test with RungeKutta4 in forward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK4_.integrateDurations(defaultStateVector_, defaultDuration_, systemOfEquations_).first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_)));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_)));
    }

    // Performance test with RungeKutta4 in backward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK4_.integrateDurations(defaultStateVector_, -defaultDuration_, systemOfEquations_).first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_)));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_)));
    }

    // Performance test with RungeKuttaCashKarp54 and integrateDurations in forward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK54_.integrateDurations(defaultStateVector_, defaultDuration_, systemOfEquations_).first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_)));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_)));
    }

    // Performance test with RungeKuttaCashKarp54 and integrateDurations in backward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK54_.integrateDurations(defaultStateVector_, -defaultDuration_, systemOfEquations_).first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_)));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_)));
    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRKF78_.integrateDurations(defaultStateVector_, defaultDuration_, systemOfEquations_).first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_)));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_)));
    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRKF78_.integrateDurations(defaultStateVector_, -defaultDuration_, systemOfEquations_).first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_)));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_)));
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
            numericalSolver_1.integrateDurations(defaultStateVector_, defaultDuration_, systemOfEquations_).first;
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateDurations(defaultStateVector_, defaultDuration_, systemOfEquations_).first;
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
            numericalSolver_1.integrateDurations(defaultStateVector_, defaultDuration_, systemOfEquations_).first;
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateDurations(defaultStateVector_, defaultDuration_, systemOfEquations_).first;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, integrateTime)
{
    // Performance test with RungeKutta4 in forwards time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK4_
                .integrateTimes(
                    defaultStateVector_, defaultStartTime_, defaultStartTime_ + defaultDuration_, systemOfEquations_
                )
                .first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_)));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_)));
    }

    // Performance test with RungeKutta4 in backwards time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK4_
                .integrateTimes(
                    defaultStateVector_, defaultStartTime_, defaultStartTime_ - defaultDuration_, systemOfEquations_
                )
                .first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_)));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_)));
    }

    // Performance test with RungeKuttaCashKarp54 in forward time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK54_
                .integrateTimes(
                    defaultStateVector_, defaultStartTime_, defaultStartTime_ + defaultDuration_, systemOfEquations_
                )
                .first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_)));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_)));
    }

    // Performance test with RungeKuttaCashKarp54 in backwards time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRK54_
                .integrateTimes(
                    defaultStateVector_, defaultStartTime_, defaultStartTime_ - defaultDuration_, systemOfEquations_
                )
                .first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_)));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_)));
    }

    // Performance test with RungeKuttaFehlberg78 in forwards time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRKF78_
                .integrateTimes(
                    defaultStateVector_, defaultStartTime_, defaultStartTime_ + defaultDuration_, systemOfEquations_
                )
                .first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(defaultDuration_)));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(defaultDuration_)));
    }

    // Performance test with RungeKuttaFehlberg78 in backwards time
    {
        const NumericalSolver::StateVector propagatedStateVector =
            defaultRKF78_
                .integrateTimes(
                    defaultStateVector_, defaultStartTime_, defaultStartTime_ - defaultDuration_, systemOfEquations_
                )
                .first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(-defaultDuration_)));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(-defaultDuration_)));
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
            numericalSolver_1
                .integrateTimes(
                    defaultStateVector_,
                    defaultStartTime_,
                    defaultStartTime_ + defaultDuration_,
                    [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                    {
                        dxdt[0] = x[1];
                        dxdt[1] = -x[0];
                    }
                )
                .first;
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2
                .integrateTimes(
                    defaultStateVector_,
                    defaultStartTime_,
                    defaultStartTime_ + defaultDuration_,
                    [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                    {
                        dxdt[0] = x[1];
                        dxdt[1] = -x[0];
                    }
                )
                .first;
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
            numericalSolver_1
                .integrateTimes(
                    defaultStateVector_,
                    defaultStartTime_,
                    defaultStartTime_ + defaultDuration_,
                    [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                    {
                        dxdt[0] = x[1];
                        dxdt[1] = -x[0];
                    }
                )
                .first;
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2
                .integrateTimes(
                    defaultStateVector_,
                    defaultStartTime_,
                    defaultStartTime_ + defaultDuration_,
                    [=](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
                    {
                        dxdt[0] = x[1];
                        dxdt[1] = -x[0];
                    }
                )
                .first;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, integrateDurations)
{
    // Performance test with RungeKutta4 in forward time
    {
        const Array<Real> aTimeArray = {
            10.0,
            40.0,
            70.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRK4_.integrateDurations(defaultStateVector_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKutta4 in backward time
    {
        const Array<Real> aTimeArray = {
            -10.0,
            -40.0,
            -70.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRK4_.integrateDurations(defaultStateVector_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKuttaCashKarp54 and integrateDurations in forward time
    {
        const Array<Real> aTimeArray = {
            100.0,
            400.0,
            700.0,
            1000.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRK54_.integrateDurations(defaultStateVector_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-8);
    }

    // Performance test with RungeKuttaCashKarp54 and integrateDurations in backward time
    {
        const Array<Real> aTimeArray = {
            -100.0,
            -400.0,
            -700.0,
            -1000.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRK54_.integrateDurations(defaultStateVector_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-8);
    }

    // Performance test with RungeKuttaFehlberg78 in forward time
    {
        const Array<Real> aTimeArray = {
            100.0,
            400.0,
            700.0,
            1000.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRKF78_.integrateDurations(defaultStateVector_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-10);
    }

    // Performance test with RungeKuttaFehlberg78 in backward time
    {
        const Array<Real> aTimeArray = {
            -100.0,
            -400.0,
            -700.0,
            -1000.0,
        };

        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            defaultRKF78_.integrateDurations(defaultStateVector_, aTimeArray, systemOfEquations_);

        validatePropagatedStates(aTimeArray, propagatedStateVectorArray, 2e-10);
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
