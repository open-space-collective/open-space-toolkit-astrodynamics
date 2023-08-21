/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Conjunctive.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Disjunctive.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Tuple;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::Conjunctive;
using ostk::astro::eventcondition::Disjunctive;
using ostk::astro::NumericalSolver;

// Simple duration based condition

struct DurationCondition : public EventCondition
{
    DurationCondition(const Real &aTarget, const EventCondition::Criteria &aCriteria)
        : EventCondition("test", aCriteria, aTarget)
    {
    }

    Real compute(const VectorXd &stateVector, const Real &aTime) const override
    {
        (void)stateVector;
        return aTime;
    }
};

// Simple value based struct

struct XCrossingCondition : public EventCondition
{
    XCrossingCondition(const Real &aTarget)
        : EventCondition("test", XCrossingCondition::Criteria::AnyCrossing, aTarget)
    {
    }

    Real compute(const VectorXd &aStateVector, const Real &aTime) const
    {
        (void)aTime;
        return aStateVector[0];
    }
};

class OpenSpaceToolkit_Astrodynamics_NumericalSolver : public ::testing::Test
{
    void SetUp() override
    {
        defaultStateVector_.resize(2);
        defaultStateVector_ << 0.0, 1.0;
    }

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

    NumericalSolver defaultRKD5_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        1e-3,
        1.0e-15,
        1.0e-15,
    };

    NumericalSolver::StateVector defaultStateVector_;
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

    NumericalSolver::StateVector getStateVector(const double &aTime)
    {
        VectorXd stateVector(2);
        stateVector << std::sin(aTime), std::cos(aTime);
        return stateVector;
    }
};

class OpenSpaceToolkit_Astrodynamics_NumericalSolver_Parametrized
    : public OpenSpaceToolkit_Astrodynamics_NumericalSolver,
      public ::testing::WithParamInterface<Tuple<NumericalSolver::StepperType>>
{
};

INSTANTIATE_TEST_SUITE_P(
    Integration,
    OpenSpaceToolkit_Astrodynamics_NumericalSolver_Parametrized,
    ::testing::Values(
        std::make_tuple(NumericalSolver::StepperType::RungeKutta4),
        std::make_tuple(NumericalSolver::StepperType::RungeKuttaCashKarp54),
        std::make_tuple(NumericalSolver::StepperType::RungeKuttaFehlberg78)
    )
);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, Accessors)
{
    {
        EXPECT_NO_THROW(defaultRK54_.accessObservedStates());
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
        EXPECT_EQ(defaultRK54_.getTimeStep(), 5.0);
        EXPECT_EQ(defaultRK54_.getRelativeTolerance(), 1.0e-15);
        EXPECT_EQ(defaultRK54_.getAbsoluteTolerance(), 1.0e-15);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, GetRootSolver)
{
    {
        EXPECT_NO_THROW(defaultRK54_.getRootSolver());
    }

    {
        EXPECT_ANY_THROW(NumericalSolver::Undefined().getRootSolver());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, GetObservedStates)
{
    {
        EXPECT_NO_THROW(defaultRK54_.getObservedStates());
    }

    {
        EXPECT_TRUE(defaultRK54_.getObservedStates().isEmpty());
    }

    {
        EXPECT_ANY_THROW(NumericalSolver::Undefined().getObservedStates());
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
        EXPECT_TRUE(
            NumericalSolver::StringFromStepperType(NumericalSolver::StepperType::RungeKuttaDopri5) == "RungeKuttaDopri5"
        );
    }

    {
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::NoLog) == "NoLog");
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::LogConstant) == "LogConstant");
        EXPECT_TRUE(NumericalSolver::StringFromLogType(NumericalSolver::LogType::LogAdaptive) == "LogAdaptive");
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_NumericalSolver_Parametrized, integrateDuration)
{
    const auto parameters = GetParam();

    NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        std::get<0>(parameters),
        1e-3,
        1.0e-15,
        1.0e-15,
    };

    const Array<Real> durations = {
        defaultDuration_,
        -defaultDuration_,
    };

    for (const Real &duration : durations)
    {
        const NumericalSolver::StateVector propagatedStateVector =
            numericalSolver.integrateDuration(defaultStateVector_, duration, systemOfEquations_).first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(duration)));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(duration)));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_NumericalSolver_Parametrized, IntegrateDuration_Comparison)
{
    const auto parameters = GetParam();

    // Performance test comparing results of integrate_adaptive and integrate_const for RungeKuttaCashKarp54
    {
        NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogAdaptive,
            std::get<0>(parameters),
            1e-3,
            1.0e-15,
            1.0e-15,
        };
        NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::LogConstant,
            std::get<0>(parameters),
            1e-3,
            1.0e-15,
            1.0e-15,
        };

        testing::internal::CaptureStdout();
        const NumericalSolver::StateVector propagatedStateVector_1 =
            numericalSolver_1.integrateDuration(defaultStateVector_, defaultDuration_, systemOfEquations_).first;
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2.integrateDuration(defaultStateVector_, defaultDuration_, systemOfEquations_).first;
        String output = testing::internal::GetCapturedStdout();

        EXPECT_FALSE(output.empty());

        // Validate the output against an analytical function

        // Ensure state error between integrate_const and integrate_adaptive is within a reasonable tolerance
        EXPECT_GT(1e-9, std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]));
        EXPECT_GT(1e-9, std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_NumericalSolver_Parametrized, IntegrateDuration_Array)
{
    const auto parameters = GetParam();

    NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        std::get<0>(parameters),
        1e-3,
        1.0e-15,
        1.0e-15,
    };

    const Array<Array<Real>> durationArrays = {
        {10.0, 40.0, 70.0, 1000.0},
        {-10.0, -40.0, -70.0, -1000.0},
    };

    for (const Array<Real> &durationArray : durationArrays)
    {
        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            numericalSolver.integrateDuration(defaultStateVector_, durationArray, systemOfEquations_);

        validatePropagatedStates(durationArray, propagatedStateVectorArray, 2e-8);
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_NumericalSolver_Parametrized, IntegrateTime)
{
    const auto parameters = GetParam();

    NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        std::get<0>(parameters),
        1e-3,
        1.0e-15,
        1.0e-15,
    };

    const Array<Real> endTimes = {
        defaultStartTime_ + defaultDuration_,
        defaultStartTime_ - defaultDuration_,
    };

    for (const Real &endTime : endTimes)
    {
        const NumericalSolver::StateVector propagatedStateVector =
            numericalSolver.integrateTime(defaultStateVector_, defaultStartTime_, endTime, systemOfEquations_).first;

        // Validate the output against an analytical function

        EXPECT_GT(2e-8, std::abs(propagatedStateVector[0] - std::sin(endTime)));
        EXPECT_GT(2e-8, std::abs(propagatedStateVector[1] - std::cos(endTime)));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_NumericalSolver_Parametrized, IntegrateTime_Comparison)
{
    const auto parameters = GetParam();
    // Performance test comparing results of integrate_adaptive and integrate_const
    {
        NumericalSolver numericalSolver_1 = {
            NumericalSolver::LogType::LogAdaptive,
            std::get<0>(parameters),
            1e-3,
            1.0e-15,
            1.0e-15,
        };
        NumericalSolver numericalSolver_2 = {
            NumericalSolver::LogType::LogConstant,
            std::get<0>(parameters),
            1e-3,
            1.0e-15,
            1.0e-15,
        };

        testing::internal::CaptureStdout();
        const NumericalSolver::StateVector propagatedStateVector_1 =
            numericalSolver_1
                .integrateTime(
                    defaultStateVector_, defaultStartTime_, defaultStartTime_ + defaultDuration_, systemOfEquations_
                )
                .first;
        const NumericalSolver::StateVector propagatedStateVector_2 =
            numericalSolver_2
                .integrateTime(
                    defaultStateVector_, defaultStartTime_, defaultStartTime_ + defaultDuration_, systemOfEquations_
                )
                .first;
        String output = testing::internal::GetCapturedStdout();

        EXPECT_FALSE(output.empty());

        // Validate the output against an analytical function
        // Ensure state error between integrate_const and integrate_adaptive is within a reasonable tolerance
        EXPECT_GT(1e-9, std::abs(propagatedStateVector_1[0] - propagatedStateVector_2[0]));
        EXPECT_GT(1e-9, std::abs(propagatedStateVector_1[1] - propagatedStateVector_2[1]));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_NumericalSolver_Parametrized, IntegrateTime_Array)
{
    const auto parameters = GetParam();

    NumericalSolver numericalSolver = {
        NumericalSolver::LogType::NoLog,
        std::get<0>(parameters),
        1e-3,
        1.0e-15,
        1.0e-15,
    };

    const Array<Array<Real>> timeArrays = {
        {100.0, 400.0, 700.0, 1000.0},
        {-100.0, -400.0, -700.0, -1000.0},
    };

    for (const Array<Real> &timeArray : timeArrays)
    {
        const Array<NumericalSolver::Solution> propagatedStateVectorArray =
            numericalSolver.integrateTime(defaultStateVector_, defaultStartTime_, timeArray, systemOfEquations_);

        validatePropagatedStates(timeArray, propagatedStateVectorArray, 2e-8);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateDuration_Conditions)
{
    // Invalid stepper type
    {
        EXPECT_THROW(
            {
                try
                {
                    defaultRK54_.integrateDuration(
                        defaultStateVector_,
                        defaultDuration_,
                        systemOfEquations_,
                        DurationCondition(0.0, EventCondition::Criteria::StrictlyPositive)
                    );
                }
                catch (const ostk::core::error::runtime::ToBeImplemented &e)
                {
                    const String expectedMessage =
                        "Integrating with conditions is only supported with RungeKuttaDopri5 stepper type";

                    EXPECT_TRUE((e.getMessage().find(expectedMessage) != std::string::npos));
                    throw;
                }
            },
            ostk::core::error::runtime::ToBeImplemented
        );
    }

    // 0 duration
    {
        const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateDuration(
            defaultStateVector_,
            0.0,
            systemOfEquations_,
            DurationCondition(0.0, EventCondition::Criteria::StrictlyPositive)
        );
        const NumericalSolver::Solution solution = conditionSolution.solution;

        EXPECT_TRUE(solution.first == defaultStateVector_);
        EXPECT_TRUE(solution.second == 0.0);
        EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
        EXPECT_FALSE(conditionSolution.rootSolverHasConverged);
    }

    // condition already satisfied
    {
        const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateDuration(
            defaultStateVector_,
            5.0,
            systemOfEquations_,
            DurationCondition(-1.0, EventCondition::Criteria::StrictlyPositive)
        );
        const NumericalSolver::Solution solution = conditionSolution.solution;

        EXPECT_TRUE(solution.first == defaultStateVector_);
        EXPECT_TRUE(solution.second == 0.0);
        EXPECT_EQ(conditionSolution.iterationCount, 0);
        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
        EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
    }

    {
        const Array<Tuple<Real, EventCondition::Criteria>> testCases = {
            {defaultDuration_, EventCondition::Criteria::StrictlyPositive},
            {-defaultDuration_, EventCondition::Criteria::StrictlyNegative},
        };

        for (const auto &testCase : testCases)
        {
            const Real duration = std::get<0>(testCase);
            const EventCondition::Criteria criteria = std::get<1>(testCase);
            // Ensure that integration terminates at maximum duration if condition is not met

            {
                const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateDuration(
                    defaultStateVector_,
                    duration,
                    systemOfEquations_,
                    DurationCondition(duration + duration / 2.0, criteria)
                );
                const NumericalSolver::Solution solution = conditionSolution.solution;

                EXPECT_DOUBLE_EQ(duration, solution.second);
                EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
                EXPECT_EQ(conditionSolution.iterationCount, 0);
            }

            const DurationCondition condition = DurationCondition(duration / 2.0, criteria);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateDuration(defaultStateVector_, duration, systemOfEquations_, condition);
            const NumericalSolver::Solution solution = conditionSolution.solution;

            const NumericalSolver::StateVector propagatedStateVector = solution.first;
            const Real propagatedTime = solution.second;

            // Ensure that integration terminates at condition if condition is met

            EXPECT_NEAR(propagatedTime, condition.getTarget(), 1e-6);
            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_TRUE(conditionSolution.rootSolverHasConverged);

            // Validate the output against an analytical function

            EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propagatedTime)));
            EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propagatedTime)));
        }
    }

    {
        const Array<Tuple<Real, Real>> testCases = {
            {defaultDuration_, 0.9},
            {-defaultDuration_, -0.9},
        };

        for (const auto testCase : testCases)
        {
            const Real duration = std::get<0>(testCase);
            const Real target = std::get<1>(testCase);

            const XCrossingCondition condition = XCrossingCondition(target);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateDuration(defaultStateVector_, duration, systemOfEquations_, condition);
            const NumericalSolver::Solution solution = conditionSolution.solution;

            const NumericalSolver::StateVector propagatedStateVector = solution.first;
            const Real propagatedTime = solution.second;

            // Ensure that integration terminates at condition if condition is met

            EXPECT_TRUE(std::abs(propagatedTime) < std::abs(duration));
            EXPECT_NEAR(propagatedTime, std::asin(condition.getTarget()), 1e-6);
            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_TRUE(conditionSolution.rootSolverHasConverged);

            // Validate the output against an analytical function

            EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propagatedTime)));
            EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propagatedTime)));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateTime_Conditions)
{
    const Real startTime = 500.0;

    const NumericalSolver::StateVector stateVector = getStateVector(startTime);

    {
        const Array<Tuple<Real, EventCondition::Criteria>> testCases = {
            {defaultDuration_, EventCondition::Criteria::AnyCrossing},
            {-defaultDuration_, EventCondition::Criteria::AnyCrossing},
        };

        for (const auto &testCase : testCases)
        {
            const Real duration = std::get<0>(testCase);
            const EventCondition::Criteria criteria = std::get<1>(testCase);

            const Real endTime = startTime + duration;

            {
                const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
                    stateVector,
                    startTime,
                    endTime,
                    systemOfEquations_,
                    DurationCondition(endTime + duration / 2.0, criteria)
                );
                const NumericalSolver::Solution solution = conditionSolution.solution;

                EXPECT_DOUBLE_EQ(solution.second, endTime);
                EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
                EXPECT_EQ(conditionSolution.iterationCount, 0);
            }

            const DurationCondition condition = DurationCondition(duration / 2.0, criteria);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateTime(stateVector, startTime, endTime, systemOfEquations_, condition);
            const NumericalSolver::Solution solution = conditionSolution.solution;

            const NumericalSolver::StateVector propagatedStateVector = solution.first;
            const Real propagatedTime = solution.second;

            // Ensure that integration terminates at condition if condition is met

            EXPECT_NEAR(propagatedTime, startTime + condition.getTarget(), 1e-6);
            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_TRUE(conditionSolution.rootSolverHasConverged);

            // Validate the output against an analytical function

            EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propagatedTime)));
            EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propagatedTime)));
        }
    }

    {
        const Array<Tuple<Real, Real>> testCases = {
            {defaultDuration_, 0.9},
            {-defaultDuration_, -0.9},
        };

        for (const auto &testCase : testCases)
        {
            const Real duration = std::get<0>(testCase);
            const Real target = std::get<1>(testCase);

            const Real endTime = startTime + duration;
            const XCrossingCondition condition = XCrossingCondition(target);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateTime(stateVector, startTime, endTime, systemOfEquations_, condition);
            const NumericalSolver::Solution solution = conditionSolution.solution;

            const NumericalSolver::StateVector propagatedStateVector = solution.first;
            const Real propagatedTime = solution.second;

            // Ensure that integration terminates at condition if condition is met

            EXPECT_TRUE(duration > 0.0 ? propagatedTime < endTime : propagatedTime > endTime);
            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_TRUE(conditionSolution.rootSolverHasConverged);

            // Validate the output against an analytical function

            EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propagatedTime)));
            EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propagatedTime)));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateTime_Conjunctive)
{
    const Array<Tuple<Real, EventCondition::Criteria>> testCases = {
        {defaultDuration_, EventCondition::Criteria::StrictlyPositive},
        {-defaultDuration_, EventCondition::Criteria::StrictlyNegative},
    };

    for (const auto testCase : testCases)
    {
        const Real duration = std::get<0>(testCase);
        const EventCondition::Criteria criteria = std::get<1>(testCase);

        const Real endTime = defaultStartTime_ + duration;

        const Shared<DurationCondition> durationCondition =
            std::make_shared<DurationCondition>(duration / 2.0, criteria);
        const Shared<XCrossingCondition> xCrossingCondition = std::make_shared<XCrossingCondition>(0.5);
        const Conjunctive conjunctiveCondition = Conjunctive({
            durationCondition,
            xCrossingCondition,
        });

        const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
            defaultStateVector_, defaultStartTime_, endTime, systemOfEquations_, conjunctiveCondition
        );
        const NumericalSolver::Solution solution = conditionSolution.solution;

        const NumericalSolver::StateVector propagatedStateVector = solution.first;
        const Real propagatedTime = solution.second;

        // Ensure that integration terminates at condition if condition is met
        EXPECT_TRUE(propagatedTime != endTime);
        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propagatedTime)));
        EXPECT_GT(1e-7, std::abs(propagatedStateVector[0] - 0.5));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propagatedTime)));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, IntegrateTime_Disjunctive)
{
    const Array<Tuple<Real, EventCondition::Criteria>> testCases = {
        {defaultDuration_, EventCondition::Criteria::StrictlyPositive},
        {-defaultDuration_, EventCondition::Criteria::StrictlyNegative},
    };

    for (const auto testCase : testCases)
    {
        const Real duration = std::get<0>(testCase);
        const EventCondition::Criteria criteria = std::get<1>(testCase);

        const Real endTime = defaultStartTime_ + duration;

        const Shared<DurationCondition> durationCondition =
            std::make_shared<DurationCondition>(duration / 2.0, criteria);
        const Shared<XCrossingCondition> xCrossingCondition = std::make_shared<XCrossingCondition>(0.5);
        const Disjunctive conjunctiveCondition = Disjunctive({
            durationCondition,
            xCrossingCondition,
        });

        const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
            defaultStateVector_, defaultStartTime_, endTime, systemOfEquations_, conjunctiveCondition
        );
        const NumericalSolver::Solution solution = conditionSolution.solution;

        const NumericalSolver::StateVector propagatedStateVector = solution.first;
        const Real propagatedTime = solution.second;

        // Ensure that integration terminates at condition if condition is met
        EXPECT_TRUE(propagatedTime != endTime);
        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);

        EXPECT_GT(2e-10, std::abs(propagatedStateVector[0] - std::sin(propagatedTime)));
        EXPECT_GT(1e-7, std::abs(propagatedStateVector[0] - 0.5));
        EXPECT_GT(2e-10, std::abs(propagatedStateVector[1] - std::cos(propagatedTime)));
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

TEST_F(OpenSpaceToolkit_Astrodynamics_NumericalSolver, DefaultConditional)
{
    {
        EXPECT_NO_THROW(NumericalSolver::DefaultConditional());
    }

    {
        EXPECT_TRUE(NumericalSolver::DefaultConditional().isDefined());
    }
}
