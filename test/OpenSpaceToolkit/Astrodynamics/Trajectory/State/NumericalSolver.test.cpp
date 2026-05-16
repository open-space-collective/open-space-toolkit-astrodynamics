/// Apache License 2.0

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/LogicalCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::eventcondition::LogicalCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::RootSolver;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver : public ::testing::Test
{
    void SetUp() override
    {
        defaultStateVector_.resize(2);
        defaultStateVector_ << 0.0, 1.0;

        defaultState_ = {
            defaultStartInstant_,
            defaultStateVector_,
            gcrfSPtr_,
            defaultCoordinateBroker_,
        };
    }

   protected:
    NumericalSolver defaultRKD5_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        1e-3,
        1.0e-15,
        1.0e-15,
    };

    NumericalSolver defaultRK54_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaCashKarp54,
        1e-3,
        1.0e-15,
        1.0e-15,
    };

    VectorXd defaultStateVector_;
    const Shared<CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(CoordinateBroker({std::make_shared<CoordinateSubset>("Test", 2)}));
    const Instant defaultStartInstant_ = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);
    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    State defaultState_ = State::Undefined();

    const Duration defaultDuration_ = Duration::Seconds(10.0);

    const std::function<void(const NumericalSolver::StateVector &, NumericalSolver::StateVector &, const double)>
        systemOfEquations_ =
            [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
    {
        dxdt[0] = x[1];
        dxdt[1] = -x[0];
    };

    void validatePropagatedStates(
        const Array<Instant> &instants, const Array<State> &aSolutionArray, const double &aTolerance
    )
    {
        for (size_t i = 0; i < instants.size(); i++)
        {
            const NumericalSolver::StateVector propagatedStateVector = aSolutionArray[i].accessCoordinates();

            EXPECT_GT(
                aTolerance,
                std::abs(propagatedStateVector[0] - std::sin((instants[i] - defaultStartInstant_).inSeconds()))
            );
            EXPECT_GT(
                aTolerance,
                std::abs(propagatedStateVector[1] - std::cos((instants[i] - defaultStartInstant_).inSeconds()))
            );
        }
    }

    State getStateVector(const Instant &anInstant)
    {
        const double time = (anInstant - defaultStartInstant_).inSeconds();
        VectorXd stateVector(2);
        stateVector << std::sin(time), std::cos(time);
        return {
            anInstant,
            stateVector,
            gcrfSPtr_,
            defaultCoordinateBroker_,
        };
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, Getters)
{
    {
        EXPECT_NO_THROW(defaultRKD5_.getRootSolver());
        EXPECT_THROW(NumericalSolver::Undefined().getRootSolver(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_TRUE(defaultRKD5_.getObservedStates().isEmpty());
    }

    {
        // Defined solver: no throw, returns the configured (initially Undefined) max step size
        EXPECT_NO_THROW(defaultRKD5_.getMaxStepSize());
        EXPECT_FALSE(defaultRKD5_.getMaxStepSize().isDefined());

        // Undefined solver: throws like the other getters
        EXPECT_THROW(NumericalSolver::Undefined().getMaxStepSize(), ostk::core::error::runtime::Undefined);

        // Round-trip via setter
        NumericalSolver solver = defaultRKD5_;
        solver.setMaxStepSize(10.0);
        EXPECT_EQ(solver.getMaxStepSize(), 10.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, Accessors)
{
    {
        EXPECT_NO_THROW(defaultRKD5_.accessObservedStates());
    }

    {
        EXPECT_THROW(NumericalSolver::Undefined().accessObservedStates(), ostk::core::error::runtime::Undefined);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime)
{
    const Array<Instant> endInstants = {
        defaultState_.accessInstant() + defaultDuration_,
        defaultState_.accessInstant() - defaultDuration_,
    };

    EXPECT_TRUE(defaultRK54_.getObservedStates().isEmpty());

    for (const Instant &endInstant : endInstants)
    {
        const State propagatedState = defaultRK54_.integrateTime(defaultState_, endInstant, systemOfEquations_);

        EXPECT_FALSE(defaultRK54_.getObservedStates().isEmpty());

        // Validate the output against an analytical function

        EXPECT_GT(
            2e-8,
            std::abs(propagatedState.accessCoordinates()[0] - std::sin((endInstant - defaultStartInstant_).inSeconds()))
        );
        EXPECT_GT(
            2e-8,
            std::abs(propagatedState.accessCoordinates()[1] - std::cos((endInstant - defaultStartInstant_).inSeconds()))
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_Array)
{
    const Array<Array<Instant>> instantsArray = {
        {
            defaultState_.accessInstant() + Duration::Seconds(1.0),
            defaultState_.accessInstant() + Duration::Seconds(4.0),
            defaultState_.accessInstant() + Duration::Seconds(7.0),
            defaultState_.accessInstant() + Duration::Seconds(10.0),
        },
        {
            defaultState_.accessInstant() + Duration::Seconds(-1.0),
            defaultState_.accessInstant() + Duration::Seconds(-4.0),
            defaultState_.accessInstant() + Duration::Seconds(-7.0),
            defaultState_.accessInstant() + Duration::Seconds(-10.0),
        },
    };

    EXPECT_TRUE(defaultRK54_.getObservedStates().isEmpty());

    for (const Array<Instant> &instants : instantsArray)
    {
        const Array<State> propagatedStateVectorArray =
            defaultRK54_.integrateTime(defaultState_, instants, systemOfEquations_);

        EXPECT_TRUE(defaultRK54_.getObservedStates().isEmpty());

        validatePropagatedStates(instants, propagatedStateVectorArray, 2e-8);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_Conditions)
{
    const State state = getStateVector(defaultStartInstant_);

    // trivial case, zero second integration
    {
        NumericalSolver::ConditionSolution solution = defaultRKD5_.integrateTime(
            state,
            state.accessInstant(),
            systemOfEquations_,
            InstantCondition(RealCondition::Criterion::AnyCrossing, state.accessInstant() + Duration::Seconds(60.0))
        );

        EXPECT_EQ(state, solution.state);
        EXPECT_FALSE(solution.conditionIsSatisfied);
        EXPECT_EQ(solution.iterationCount, 0);
        EXPECT_FALSE(solution.rootSolverHasConverged);
    }

    // condition already satisfied
    {
        const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
            state,
            defaultStartInstant_ + defaultDuration_,
            systemOfEquations_,
            InstantCondition(RealCondition::Criterion::StrictlyPositive, state.accessInstant() - Duration::Seconds(1.0))
        );

        EXPECT_EQ(conditionSolution.state, state);
        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
        EXPECT_EQ(conditionSolution.iterationCount, 0);
        EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
    }

    // condition not satisfied
    {
        const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
            state,
            defaultStartInstant_ + defaultDuration_,
            systemOfEquations_,
            InstantCondition(
                RealCondition::Criterion::StrictlyPositive,
                defaultStartInstant_ + defaultDuration_ + Duration::Seconds(1.0)
            )
        );

        EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
        EXPECT_EQ(conditionSolution.state, defaultRKD5_.getObservedStates().accessLast());
    }

    {
        const Array<Tuple<Duration, RealCondition::Criterion>> testCases = {
            {defaultDuration_, RealCondition::Criterion::AnyCrossing},
            {-defaultDuration_, RealCondition::Criterion::AnyCrossing},
        };

        for (const auto &testCase : testCases)
        {
            const Duration duration = std::get<0>(testCase);
            const RealCondition::Criterion criterion = std::get<1>(testCase);

            const Instant endInstant = defaultStartInstant_ + duration;

            {
                const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
                    state, endInstant, systemOfEquations_, InstantCondition(criterion, endInstant + duration / 2.0)
                );
                const State finalState = conditionSolution.state;

                EXPECT_LT((finalState.accessInstant() - endInstant).inSeconds(), 1e-12);
                EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
                EXPECT_EQ(conditionSolution.iterationCount, 0);
                EXPECT_FALSE(defaultRKD5_.getObservedStates().isEmpty());
            }

            const Instant targetInstant = defaultStartInstant_ + duration / 2.0;

            const InstantCondition condition = InstantCondition(criterion, targetInstant);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateTime(state, endInstant, systemOfEquations_, condition);

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.accessCoordinates();
            const Real propagatedTime = (conditionSolution.state.accessInstant() - defaultStartInstant_).inSeconds();

            // Ensure that integration terminates at condition if condition is met

            EXPECT_LT((conditionSolution.state.accessInstant() - targetInstant).inSeconds(), 1e-6);
            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
            EXPECT_EQ(conditionSolution.state, defaultRKD5_.getObservedStates().accessLast());

            // Validate the output against an analytical function

            EXPECT_NEAR(propagatedStateVector[0], std::sin(propagatedTime), 1e-9);
            EXPECT_NEAR(propagatedStateVector[1], std::cos(propagatedTime), 1e-9);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_ConditionEdgeCases)
{
    // This test uses a deterministic condition (as it's based on the independent variable time,
    // which is not really affected by the systems of equations).
    //
    // It checks edge cases mostly at the leading and trailing edges of the propagation.
    const State state = getStateVector(defaultStartInstant_);

    {
        const Array<Tuple<Real, Real, RealCondition::Criterion, bool>> testCases = {
            {1.0, 0.0, RealCondition::Criterion::AnyCrossing, true},     // Condition happens at the start (forwards)
            {-1.0, 0.0, RealCondition::Criterion::AnyCrossing, true},    // Condition happens at the end (backwards)
            {1.0, 1.0, RealCondition::Criterion::AnyCrossing, true},     // Condition happens at the end (forwards)
            {-1.0, -1.0, RealCondition::Criterion::AnyCrossing, true},   // Condition happens at the end (backwards)
            {1.0, 2.0, RealCondition::Criterion::AnyCrossing, false},    // Condition never happens (forwards)
            {-1.0, -2.0, RealCondition::Criterion::AnyCrossing, false},  // Condition never happens (backwards)
        };

        for (const auto &testCase : testCases)
        {
            const Real maximumPropagationTime = std::get<0>(testCase);
            const Real target = std::get<1>(testCase);
            const RealCondition::Criterion criterion = std::get<2>(testCase);
            const InstantCondition condition =
                InstantCondition(criterion, defaultStartInstant_ + Duration::Seconds(target));

            const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
                state, defaultStartInstant_ + Duration::Seconds(maximumPropagationTime), systemOfEquations_, condition
            );

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.accessCoordinates();
            const Real propagatedTime = (conditionSolution.state.accessInstant() - defaultStartInstant_).inSeconds();

            const bool expectedConditionIsSatisfied = std::get<3>(testCase);

            EXPECT_EQ(conditionSolution.conditionIsSatisfied, expectedConditionIsSatisfied);
            EXPECT_EQ(conditionSolution.rootSolverHasConverged, expectedConditionIsSatisfied);
            EXPECT_EQ(conditionSolution.state, defaultRKD5_.getObservedStates().accessLast());
            if (expectedConditionIsSatisfied)
            {
                EXPECT_NEAR(propagatedTime, target, 1e-8);
            }
            else
            {
                EXPECT_NEAR(propagatedTime, maximumPropagationTime, 1e-8);
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_ConditionRootFinding)
{
    // This test uses a non-deterministic condition (i.e. dependent on the systems of equations and the numerical
    // solver).
    //
    // We purposely create cases where the condition is met multiple times over the propagation, to assert that we
    // correctly stop at the first instance.
    const State state = getStateVector(defaultStartInstant_);

    {
        const Array<Tuple<Real, Real, RealCondition::Criterion, Real, bool>> testCases = {
            {Real::TwoPi(), 1.0, RealCondition::Criterion::AnyCrossing, Real::TwoPi(), false
            },  // Almost meets the condition, but never does (forwards)
            {-Real::TwoPi(), 1.0, RealCondition::Criterion::AnyCrossing, -Real::TwoPi(), false
            },  // Almost meets the condition, but never does (backwards)
            {Real::TwoPi(), 0.5, RealCondition::Criterion::AnyCrossing, std::asin(0.5), true
            },  // Any crossing (forwards)
            {-Real::TwoPi(), 0.5, RealCondition::Criterion::AnyCrossing, -Real::Pi() - std::asin(0.5), true
            },  // Any crossing (backwards)
            {Real::TwoPi(), 0.5, RealCondition::Criterion::NegativeCrossing, Real::Pi() - std::asin(0.5), true
            },  // Negative crossing (forwards)
            {-Real::TwoPi(), 0.5, RealCondition::Criterion::NegativeCrossing, -Real::TwoPi() + std::asin(0.5), true
            },  // Negative crossing (backwards)
            {Real::TwoPi(), -0.5, RealCondition::Criterion::PositiveCrossing, Real::TwoPi() - std::asin(0.5), true
            },  // Positive crossing (forwards)
            {-Real::TwoPi(), -0.5, RealCondition::Criterion::PositiveCrossing, -Real::Pi() + std::asin(0.5), true
            },  // Positive crossing (backwards)
            {Real::TwoPi(), -0.5, RealCondition::Criterion::StrictlyNegative, Real::Pi() + std::asin(0.5), true
            },  // Strictly negative (forwards)
            {-Real::TwoPi(), -0.5, RealCondition::Criterion::StrictlyNegative, -std::asin(0.5), true
            },  // Strictly negative (backwards)
            {Real::TwoPi(), 0.5, RealCondition::Criterion::StrictlyPositive, std::asin(0.5), true
            },  // Strictly positive (forwards)
            {-Real::TwoPi(), 0.5, RealCondition::Criterion::StrictlyPositive, -Real::Pi() - std::asin(0.5), true
            },  // Strictly positive (backwards)
        };

        for (const auto &testCase : testCases)
        {
            const Duration duration = Duration::Seconds(std::get<0>(testCase));
            const Real target = std::get<1>(testCase);
            const RealCondition::Criterion criterion = std::get<2>(testCase);
            const Instant endInstant = defaultStartInstant_ + duration;
            const RealCondition condition = RealCondition(
                "X Crossing Condition",
                criterion,
                [](const State &aState) -> Real
                {
                    return aState.accessCoordinates()[0];
                },
                target
            );
            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateTime(state, endInstant, systemOfEquations_, condition);

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.accessCoordinates();
            const Real propagatedTime = (conditionSolution.state.accessInstant() - defaultStartInstant_).inSeconds();

            const Real expectedPropagatedTime = std::get<3>(testCase);
            const bool expectedConditionIsSatisfied = std::get<4>(testCase);

            EXPECT_NEAR(propagatedTime, expectedPropagatedTime, 1e-8);
            EXPECT_EQ(conditionSolution.conditionIsSatisfied, expectedConditionIsSatisfied);
            EXPECT_EQ(conditionSolution.rootSolverHasConverged, expectedConditionIsSatisfied);
            EXPECT_EQ(conditionSolution.state, defaultRKD5_.getObservedStates().accessLast());
            EXPECT_NEAR(propagatedStateVector[0], std::sin(propagatedTime), 1e-8);
            EXPECT_NEAR(propagatedStateVector[1], std::cos(propagatedTime), 1e-8);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, Undefined)
{
    {
        EXPECT_NO_THROW(NumericalSolver::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, Default)
{
    {
        EXPECT_NO_THROW(NumericalSolver::Default());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, FixedStepSize)
{
    {
        EXPECT_NO_THROW(NumericalSolver::FixedStepSize(NumericalSolver::StepperType::RungeKutta4, 30.0));
    }

    {
        EXPECT_THROW(
            NumericalSolver::FixedStepSize(NumericalSolver::StepperType::RungeKuttaDopri5, 30.0),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, DefaultConditional)
{
    {
        EXPECT_NO_THROW(NumericalSolver::DefaultConditional());
    }

    {
        const auto stateLogger = [](const State &aState) -> void
        {
            std::cout << aState << std::endl;
        };

        EXPECT_NO_THROW(NumericalSolver::DefaultConditional(stateLogger));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, Conditional)
{
    {
        const auto stateLogger = [](const State &aState) -> void
        {
            std::cout << aState << std::endl;
        };

        testing::internal::CaptureStdout();

        NumericalSolver numericalSolver = NumericalSolver::Conditional(5.0, 1e-12, 1e-12, stateLogger);

        const NumericalSolver::ConditionSolution conditionSolution = numericalSolver.integrateTime(
            defaultState_,
            defaultState_.accessInstant() + Duration::Hours(1.0),
            systemOfEquations_,
            InstantCondition(
                RealCondition::Criterion::AnyCrossing, defaultState_.accessInstant() + Duration::Minutes(1.0)
            )
        );

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_LogicalCondition)
{
    const State state = getStateVector(defaultStartInstant_);
    const Instant targetInstant = defaultStartInstant_ + Duration::Seconds(5.0);
    const Instant endInstant = defaultStartInstant_ + Duration::Seconds(10.0);

    // Create a LogicalCondition with OR type containing two InstantConditions
    const Array<Shared<EventCondition>> eventConditions = {
        std::make_shared<InstantCondition>(RealCondition::Criterion::AnyCrossing, targetInstant),
        std::make_shared<InstantCondition>(
            RealCondition::Criterion::AnyCrossing, defaultStartInstant_ + Duration::Seconds(15.0)
        ),
    };

    const LogicalCondition logicalCondition = {
        "Test Logical Condition",
        LogicalCondition::Type::Or,
        eventConditions,
    };

    const NumericalSolver::ConditionSolution conditionSolution =
        defaultRKD5_.integrateTime(state, endInstant, systemOfEquations_, logicalCondition);

    EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
    EXPECT_LT(std::abs((conditionSolution.state.accessInstant() - targetInstant).inSeconds()), 1e-6);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_Conditions_Refinement)
{
    // Test the unified dense-output refinement with Fehlberg78 stepper.
    NumericalSolver solver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        1e-3,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
    };

    const State state = getStateVector(defaultStartInstant_);
    const Instant targetInstant = defaultStartInstant_ + defaultDuration_ / 2.0;
    const InstantCondition condition = InstantCondition(RealCondition::Criterion::AnyCrossing, targetInstant);

    const NumericalSolver::ConditionSolution conditionSolution =
        solver.integrateTime(state, defaultStartInstant_ + defaultDuration_, systemOfEquations_, condition);

    // Dense-output refinement should be very accurate.
    EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
    EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
    EXPECT_LT(std::abs((conditionSolution.state.accessInstant() - targetInstant).inSeconds()), 1e-6);
}

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver_StepperConditional
    : public OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver,
      public ::testing::WithParamInterface<NumericalSolver::StepperType>
{
};

INSTANTIATE_TEST_SUITE_P(
    AllSteppers,
    OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver_StepperConditional,
    ::testing::Values(
        // Adams-Bashforth-Moulton steppers are excluded — conditional integration is not
        // supported with multistep methods. See `IntegrateTime_Conditions_ABM_Rejected`.
        NumericalSolver::StepperType::RungeKuttaCashKarp54,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        NumericalSolver::StepperType::BulirschStoer
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver_StepperConditional, ForwardAndBackwardCrossing)
{
    // Exercise every stepper with the unified dense-output refinement, in both directions. The
    // forward case is the canonical conditional integration; the backward case is critical
    // regression coverage for the multistep (ABM) fallback path and for the sign-handling in the
    // root refinement helpers, which historically had bugs masked by tolerance-friendly forward
    // runs.
    const NumericalSolver::StepperType stepperType = GetParam();

    NumericalSolver solver = {
        NumericalSolver::LogType::NoLog,
        stepperType,
        1e-3,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
    };

    const State state = getStateVector(defaultStartInstant_);

    for (const Real durationSeconds : {Real(1.0), Real(-1.0)})
    {
        const Duration duration = Duration::Seconds(durationSeconds);
        const Instant endInstant = defaultStartInstant_ + duration;
        const Instant targetInstant = defaultStartInstant_ + duration / 2.0;
        const InstantCondition condition = InstantCondition(RealCondition::Criterion::AnyCrossing, targetInstant);

        const NumericalSolver::ConditionSolution conditionSolution =
            solver.integrateTime(state, endInstant, systemOfEquations_, condition);

        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
        EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
        EXPECT_LT(std::abs((conditionSolution.state.accessInstant() - targetInstant).inSeconds()), 1e-3);

        // Sanity: observed states are non-empty and last entry matches the solution state.
        EXPECT_FALSE(solver.getObservedStates().isEmpty());
        EXPECT_EQ(conditionSolution.state, solver.getObservedStates().accessLast());
    }
}

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver_StepperConditional, ConditionNeverSatisfiedMonotonic
)
{
    // when the condition is never satisfied, the trim back to endTime
    // must keep observedStates_ monotonic in time. Run forward and backward to exercise both
    // sign branches of the trim integration.
    const NumericalSolver::StepperType stepperType = GetParam();

    NumericalSolver solver = {
        NumericalSolver::LogType::NoLog,
        stepperType,
        1e-3,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
    };

    const State state = getStateVector(defaultStartInstant_);

    for (const Real durationSeconds : {Real(1.0), Real(-1.0)})
    {
        const Duration duration = Duration::Seconds(durationSeconds);
        const Instant endInstant = defaultStartInstant_ + duration;

        // Place the condition target *outside* [start, end] so it is never reached.
        const Instant outsideTarget = defaultStartInstant_ + duration * Real(2.0);
        const InstantCondition condition = InstantCondition(RealCondition::Criterion::AnyCrossing, outsideTarget);

        const NumericalSolver::ConditionSolution conditionSolution =
            solver.integrateTime(state, endInstant, systemOfEquations_, condition);

        EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
        EXPECT_LT(std::abs((conditionSolution.state.accessInstant() - endInstant).inSeconds()), 1e-9);

        const Array<State> observed = solver.getObservedStates();
        ASSERT_GE(observed.getSize(), 2u);

        // Strict monotonicity in the integration direction.
        for (size_t i = 1; i < observed.getSize(); ++i)
        {
            const double dt = (observed[i].accessInstant() - observed[i - 1].accessInstant()).inSeconds();
            if (durationSeconds > 0.0)
            {
                EXPECT_GT(dt, 0.0) << "Forward integration produced non-monotonic observed states at index " << i;
            }
            else
            {
                EXPECT_LT(dt, 0.0) << "Backward integration produced non-monotonic observed states at index " << i;
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_Conditions_BackwardIntegration)
{
    // Regression coverage for signedTimeStep < 0 (backward integration) to a known event.
    // Confirms the dense-output refinement and the bisection lower-bound selection produce the
    // correct crossing time when integrating in reverse.
    NumericalSolver solver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaDopri5,
        1e-3,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
    };

    const State state = getStateVector(defaultStartInstant_);
    const Duration duration = -defaultDuration_;
    const Instant endInstant = defaultStartInstant_ + duration;
    const Instant targetInstant = defaultStartInstant_ + duration / 2.0;
    const InstantCondition condition = InstantCondition(RealCondition::Criterion::AnyCrossing, targetInstant);

    const NumericalSolver::ConditionSolution conditionSolution =
        solver.integrateTime(state, endInstant, systemOfEquations_, condition);

    EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
    EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
    EXPECT_LT(std::abs((conditionSolution.state.accessInstant() - targetInstant).inSeconds()), 1e-6);
}

// Test that mimics the QLaw "thrust toggles when threshold crossed" scenario.
// Uses a custom system where the dynamics has a smooth oscillating component combined with a
// thresholded "thrust-like" term that turns on/off based on the state. The event condition
// fires when the thresholded term turns off. We compare the spline-evaluated stop state vs.
// the result of re-integrating with `integrate_adaptive` to the same time.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, SplineVsReintegration_ThresholdedDynamics)
{
    // Dynamics: y'' + y = thrust where thrust is non-zero only when y' > 0.3 (a thresholded term).
    // Initial: y(0) = 0, y'(0) = 1. The "thrust" introduces a kink each time y'(t) crosses 0.3.
    const auto systemWithThresholdedThrust =
        [](const NumericalSolver::StateVector &x, NumericalSolver::StateVector &dxdt, const double) -> void
    {
        dxdt[0] = x[1];
        // Thrust-like contribution: 1.0 when x[1] > 0.3, 0.0 otherwise. Combined with -x[0].
        const double thrust = (x[1] > 0.3) ? 1.0 : 0.0;
        dxdt[1] = -x[0] + thrust;
    };

    // Event condition: detects the moment thrust turns off (x[1] crosses below 0.3).
    const auto thrustOffEvaluator = [](const State &state) -> Real
    {
        return state.accessCoordinates()[1] - 0.3;  // value > 0 means thrust on, < 0 means thrust off
    };

    RealCondition thrustOffCondition("Thrust Off", RealCondition::Criterion::StrictlyNegative, thrustOffEvaluator, 0.0);

    NumericalSolver solver = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
    };
    solver.setMaxStepSize(2.0);  // limit step so we don't fly past the event

    VectorXd init(2);
    init << 0.0, 1.0;
    const State initial(defaultStartInstant_, init, gcrfSPtr_, defaultCoordinateBroker_);

    const Duration totalDuration = Duration::Seconds(20.0);
    const Instant endInstant = defaultStartInstant_ + totalDuration;

    const NumericalSolver::ConditionSolution conditionSolution =
        solver.integrateTime(initial, endInstant, systemWithThresholdedThrust, thrustOffCondition);

    ASSERT_TRUE(conditionSolution.conditionIsSatisfied);

    const double solvedTimeSeconds = (conditionSolution.state.accessInstant() - defaultStartInstant_).inSeconds();
    const double splineY0 = conditionSolution.state.accessCoordinates()[0];
    const double splineY1 = conditionSolution.state.accessCoordinates()[1];

    // Reintegrate from initial state to solvedTimeSeconds with adaptive stepper for ground truth.
    using stepper_t = boost::numeric::odeint::runge_kutta_fehlberg78<NumericalSolver::StateVector>;
    auto controlled = boost::numeric::odeint::make_controlled<stepper_t>(1.0e-15, 1.0e-15);
    NumericalSolver::StateVector reintState = init;
    if (solvedTimeSeconds > 0.0)
    {
        boost::numeric::odeint::integrate_adaptive(
            controlled, systemWithThresholdedThrust, reintState, 0.0, solvedTimeSeconds, 1e-6
        );
    }

    const double reY0 = reintState[0];
    const double reY1 = reintState[1];

    std::cout << "[SplineVsReint] solvedTime=" << solvedTimeSeconds
              << " spline y=[" << splineY0 << ", " << splineY1 << "]"
              << " reint y=[" << reY0 << ", " << reY1 << "]"
              << " dY0=" << (splineY0 - reY0)
              << " dY1=" << (splineY1 - reY1)
              << std::endl;

    // Spline should be close to reintegration, but the discontinuous derivative means there's
    // some interpolation error. Print so we can see the magnitude.
    EXPECT_LT(std::abs(splineY1 - reY1), 1e-3);
}

// Minimal test characterising the cubic-spline state-evaluation accuracy used by the
// post-crossing refinement path in `integrateTimeWithStepperImpl_`.
//
// The harmonic oscillator y'' + y = 0 with y(0)=0, y'(0)=1 has the closed form y(t)=sin(t),
// y'(t)=cos(t). An InstantCondition fires at a known instant t*. The conditional integrator:
//   1. steps forward with the adaptive stepper and detects the crossing in
//      [previousTime, currentTime];
//   2. samples that bracket with `integrate_times` and fits a per-dimension cubic spline;
//   3. bisects on the spline-evaluated state to locate the exact crossing time;
//   4. evaluates the *final state* via the spline (NOT via re-integration to that exact time).
//
// We compare the spline-evaluated state at the solution time against the analytic state at the
// same time. The delta is the spline-interpolation error introduced by step (4) — the path the
// guidance/Segment logic downstream consumes. Larger step spans amplify this error.
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, SplineRefinement_AccuracyVsStepSpan)
{
    // We push the stepper into taking large steps by setting a large initial time step and
    // loose-ish absolute tolerance on a slowly-varying state span. This widens the bracket and
    // exercises the spline-fit branch.
    for (const double initialStep : {1.0, 5.0, 30.0, 60.0})
    {
        NumericalSolver solver = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaFehlberg78,
            initialStep,
            1.0e-12,
            1.0e-12,
            RootSolver::Default(),
        };

        const State state = getStateVector(defaultStartInstant_);
        const Duration totalDuration = Duration::Seconds(120.0);
        const Instant endInstant = defaultStartInstant_ + totalDuration;
        // Target somewhere where sin and cos are both ~O(1); avoids zero-crossing degeneracy.
        const Instant targetInstant = defaultStartInstant_ + Duration::Seconds(0.7);
        const InstantCondition condition(RealCondition::Criterion::AnyCrossing, targetInstant);

        const NumericalSolver::ConditionSolution conditionSolution =
            solver.integrateTime(state, endInstant, systemOfEquations_, condition);

        ASSERT_TRUE(conditionSolution.conditionIsSatisfied) << "Condition not satisfied at initialStep=" << initialStep;
        ASSERT_TRUE(conditionSolution.rootSolverHasConverged) << "Bisection did not converge at initialStep=" << initialStep;

        const double solvedTimeSeconds = (conditionSolution.state.accessInstant() - defaultStartInstant_).inSeconds();
        const double splineY0 = conditionSolution.state.accessCoordinates()[0];
        const double splineY1 = conditionSolution.state.accessCoordinates()[1];

        const double analyticY0 = std::sin(solvedTimeSeconds);
        const double analyticY1 = std::cos(solvedTimeSeconds);

        std::cout << "[Spline test] initialStep=" << initialStep
                  << " solvedTime=" << solvedTimeSeconds
                  << " splineY0=" << splineY0
                  << " analyticY0=" << analyticY0
                  << " err0=" << (splineY0 - analyticY0)
                  << " splineY1=" << splineY1
                  << " analyticY1=" << analyticY1
                  << " err1=" << (splineY1 - analyticY1)
                  << std::endl;

        // For the harmonic oscillator the cubic-spline interpolation over a step ≤60 s should be
        // very accurate (O(h^4)) so the residual is tiny. We just print it for visibility — we
        // don't assert tight bounds (the values are state-/stepper-dependent).
        EXPECT_LT(std::abs(splineY0 - analyticY0), 1.0e-6);
        EXPECT_LT(std::abs(splineY1 - analyticY1), 1.0e-6);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_Conditions_ABM_Rejected)
{
    // Multistep Adams-Bashforth-Moulton steppers are not supported for conditional integration.
    // Both AdamsBashforthMoulton5 and AdamsBashforthMoulton8 must raise a RuntimeError up-front
    // rather than silently falling through to a path that would warm-up multistep history on
    // every bracket and stall under tight tolerances.
    const State state = getStateVector(defaultStartInstant_);
    const Instant endInstant = defaultStartInstant_ + defaultDuration_;
    const Instant targetInstant = defaultStartInstant_ + defaultDuration_ / 2.0;
    const InstantCondition condition = InstantCondition(RealCondition::Criterion::AnyCrossing, targetInstant);

    for (const NumericalSolver::StepperType stepperType :
         {NumericalSolver::StepperType::AdamsBashforthMoulton5, NumericalSolver::StepperType::AdamsBashforthMoulton8})
    {
        NumericalSolver solver = {
            NumericalSolver::LogType::NoLog,
            stepperType,
            1e-3,
            1.0e-12,
            1.0e-12,
            RootSolver::Default(),
        };

        EXPECT_THROW(
            solver.integrateTime(state, endInstant, systemOfEquations_, condition), ostk::core::error::RuntimeError
        );
    }
}
