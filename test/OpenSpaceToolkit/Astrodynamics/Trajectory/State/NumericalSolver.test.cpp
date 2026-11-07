/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
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
using ostk::core::type::Shared;
using ostk::core::type::Size;
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

    const VectorXd defaultStateVector_ = (VectorXd(2) << 0.0, 1.0).finished();
    const Shared<CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(CoordinateBroker({std::make_shared<CoordinateSubset>("Test", 2)}));
    const Instant defaultStartInstant_ = Instant::DateTime(DateTime(2018, 1, 2, 0, 0, 0), Scale::UTC);
    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    const State defaultState_ = {
        defaultStartInstant_,
        defaultStateVector_,
        gcrfSPtr_,
        defaultCoordinateBroker_,
    };

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
            const NumericalSolver::StateVector propagatedStateVector = aSolutionArray[i].getCoordinates();

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
    }

    {
        EXPECT_TRUE(defaultRKD5_.getObservedStates().isEmpty());
        EXPECT_EQ(&defaultRKD5_.getObservedStates(), &defaultRKD5_.getObservedStates());
    }

    {
        // Returns the configured max step size (initially not set)
        EXPECT_NO_THROW(defaultRKD5_.getMaxStepSize());
        EXPECT_FALSE(defaultRKD5_.getMaxStepSize().has_value());

        // Round-trip via setter
        NumericalSolver solver = defaultRKD5_;
        solver.setMaxStepSize(10.0);
        ASSERT_TRUE(solver.getMaxStepSize().has_value());
        EXPECT_EQ(solver.getMaxStepSize().value(), 10.0);

        // Removing the limit
        solver.setMaxStepSize(std::nullopt);
        EXPECT_FALSE(solver.getMaxStepSize().has_value());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime)
{
    const Array<Instant> endInstants = {
        defaultState_.getInstant() + defaultDuration_,
        defaultState_.getInstant() - defaultDuration_,
    };

    EXPECT_TRUE(defaultRK54_.getObservedStates().isEmpty());

    for (const Instant &endInstant : endInstants)
    {
        const State propagatedState = defaultRK54_.integrateTime(defaultState_, endInstant, systemOfEquations_);

        EXPECT_FALSE(defaultRK54_.getObservedStates().isEmpty());

        // Validate the output against an analytical function

        EXPECT_GT(
            2e-8,
            std::abs(propagatedState.getCoordinates()[0] - std::sin((endInstant - defaultStartInstant_).inSeconds()))
        );
        EXPECT_GT(
            2e-8,
            std::abs(propagatedState.getCoordinates()[1] - std::cos((endInstant - defaultStartInstant_).inSeconds()))
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_StateLogger)
{
    const Instant endInstant = defaultState_.getInstant() + defaultDuration_;

    // Logger called for each observed state when logging is enabled
    {
        Array<State> loggedStates = Array<State>::Empty();

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::LogAdaptive,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            1e-3,
            1.0e-15,
            1.0e-15,
            RootSolver::Default(),
            [&loggedStates](const State &aState) -> void
            {
                loggedStates.add(aState);
            },
        };

        testing::internal::CaptureStdout();

        numericalSolver.integrateTime(defaultState_, endInstant, systemOfEquations_);

        testing::internal::GetCapturedStdout();

        EXPECT_FALSE(loggedStates.isEmpty());
        EXPECT_EQ(numericalSolver.getObservedStates().getSize(), loggedStates.getSize());
    }

    // Logger not called when logging is disabled
    {
        Array<State> loggedStates = Array<State>::Empty();

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            1e-3,
            1.0e-15,
            1.0e-15,
            RootSolver::Default(),
            [&loggedStates](const State &aState) -> void
            {
                loggedStates.add(aState);
            },
        };

        numericalSolver.integrateTime(defaultState_, endInstant, systemOfEquations_);

        EXPECT_FALSE(numericalSolver.getObservedStates().isEmpty());
        EXPECT_TRUE(loggedStates.isEmpty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_AppendObservedStates)
{
    const Instant middleInstant = defaultStartInstant_ + Duration::Seconds(5.0);
    const Instant endInstant = defaultStartInstant_ + Duration::Seconds(10.0);

    const auto expectStrictlyIncreasingInstants = [](const Array<State> &aStateArray)
    {
        for (Size i = 1; i < aStateArray.getSize(); ++i)
        {
            EXPECT_LT(aStateArray[i - 1].getInstant(), aStateArray[i].getInstant()) << "state index " << i;
        }
    };

    // By default, the observed states are the ones of the last integration only
    {
        NumericalSolver numericalSolver = defaultRK54_;

        const State middleState = numericalSolver.integrateTime(defaultState_, middleInstant, systemOfEquations_);
        numericalSolver.integrateTime(middleState, endInstant, systemOfEquations_);

        EXPECT_EQ(numericalSolver.getObservedStates().getFirst().getInstant(), middleInstant);
        EXPECT_EQ(numericalSolver.getObservedStates().getLast().getInstant(), endInstant);
    }

    // Without an event condition: the second integration continues the observed states of the first one
    {
        NumericalSolver numericalSolver = defaultRK54_;

        const State middleState = numericalSolver.integrateTime(defaultState_, middleInstant, systemOfEquations_);
        const Array<State> firstObservedStates = numericalSolver.getObservedStates();

        EXPECT_EQ(firstObservedStates.getFirst().getInstant(), defaultStartInstant_);
        EXPECT_EQ(firstObservedStates.getLast().getInstant(), middleInstant);

        const State endState = numericalSolver.integrateTime(middleState, endInstant, systemOfEquations_, true);

        const Array<State> observedStates = numericalSolver.getObservedStates();

        EXPECT_GT(observedStates.getSize(), firstObservedStates.getSize());
        EXPECT_EQ(observedStates.getFirst().getInstant(), defaultStartInstant_);
        EXPECT_EQ(observedStates.getLast().getInstant(), endInstant);
        EXPECT_EQ(observedStates.getLast(), endState);
        expectStrictlyIncreasingInstants(observedStates);

        // The first integration's states are untouched, and the middle state is observed only once
        for (Size i = 0; i < firstObservedStates.getSize(); ++i)
        {
            EXPECT_EQ(observedStates[i], firstObservedStates[i]);
        }
    }

    // Without an event condition: the states observed beyond the initial state are discarded
    {
        NumericalSolver numericalSolver = defaultRK54_;

        const State middleState = numericalSolver.integrateTime(defaultState_, middleInstant, systemOfEquations_);
        numericalSolver.integrateTime(middleState, endInstant, systemOfEquations_, true);

        const Array<State> firstObservedStates = numericalSolver.getObservedStates();

        // Integrate again from the middle state: everything observed after it is replaced
        const State shorterEndState = numericalSolver.integrateTime(
            middleState, middleInstant + Duration::Seconds(2.0), systemOfEquations_, true
        );

        const Array<State> observedStates = numericalSolver.getObservedStates();

        EXPECT_EQ(observedStates.getFirst().getInstant(), defaultStartInstant_);
        EXPECT_EQ(observedStates.getLast(), shorterEndState);
        EXPECT_LT(observedStates.getSize(), firstObservedStates.getSize());
        expectStrictlyIncreasingInstants(observedStates);

        unsigned int middleStateCount = 0;
        for (const State &observedState : observedStates)
        {
            middleStateCount += observedState.getInstant() == middleInstant ? 1 : 0;
        }

        EXPECT_EQ(middleStateCount, 1);
    }

    // Without an event condition: backward continuation
    {
        NumericalSolver numericalSolver = defaultRK54_;

        const State backwardState = numericalSolver.integrateTime(
            defaultState_, defaultStartInstant_ - Duration::Seconds(3.0), systemOfEquations_
        );
        numericalSolver.integrateTime(
            backwardState, defaultStartInstant_ - Duration::Seconds(6.0), systemOfEquations_, true
        );

        const Array<State> observedStates = numericalSolver.getObservedStates();

        EXPECT_EQ(observedStates.getFirst().getInstant(), defaultStartInstant_);
        EXPECT_EQ(observedStates.getLast().getInstant(), defaultStartInstant_ - Duration::Seconds(6.0));

        for (Size i = 1; i < observedStates.getSize(); ++i)
        {
            EXPECT_GT(observedStates[i - 1].getInstant(), observedStates[i].getInstant()) << "state index " << i;
        }
    }

    // With an event condition: the event is reached during the second integration
    {
        NumericalSolver numericalSolver = defaultRK54_;

        const InstantCondition eventCondition = {
            InstantCondition::Criterion::StrictlyPositive,
            defaultStartInstant_ + Duration::Seconds(8.0),
        };

        const NumericalSolver::ConditionSolution firstSolution =
            numericalSolver.integrateTime(defaultState_, middleInstant, systemOfEquations_, eventCondition);
        const Array<State> firstObservedStates = numericalSolver.getObservedStates();

        EXPECT_FALSE(firstSolution.conditionIsSatisfied);
        EXPECT_EQ(firstObservedStates.getFirst().getInstant(), defaultStartInstant_);
        EXPECT_EQ(firstObservedStates.getLast().getInstant(), middleInstant);

        const NumericalSolver::ConditionSolution secondSolution =
            numericalSolver.integrateTime(firstSolution.state, endInstant, systemOfEquations_, eventCondition, true);

        const Array<State> observedStates = numericalSolver.getObservedStates();

        EXPECT_TRUE(secondSolution.conditionIsSatisfied);
        EXPECT_GT(observedStates.getSize(), firstObservedStates.getSize());
        EXPECT_EQ(observedStates.getFirst().getInstant(), defaultStartInstant_);
        EXPECT_EQ(observedStates.getLast(), secondSolution.state);
        expectStrictlyIncreasingInstants(observedStates);

        for (Size i = 0; i < firstObservedStates.getSize(); ++i)
        {
            EXPECT_EQ(observedStates[i], firstObservedStates[i]);
        }
    }

    // With an event condition that is satisfied at the initial state: nothing is added
    {
        NumericalSolver numericalSolver = defaultRK54_;

        const State middleState = numericalSolver.integrateTime(defaultState_, middleInstant, systemOfEquations_);
        const Array<State> firstObservedStates = numericalSolver.getObservedStates();

        const InstantCondition satisfiedCondition = {
            InstantCondition::Criterion::StrictlyPositive,
            defaultStartInstant_ + Duration::Seconds(1.0),
        };

        const NumericalSolver::ConditionSolution solution =
            numericalSolver.integrateTime(middleState, endInstant, systemOfEquations_, satisfiedCondition, true);

        EXPECT_TRUE(solution.conditionIsSatisfied);
        EXPECT_EQ(numericalSolver.getObservedStates(), firstObservedStates);
    }

    // The logger is not called again for the state the integration starts from
    {
        Array<State> loggedStates = Array<State>::Empty();

        NumericalSolver numericalSolver = {
            NumericalSolver::LogType::LogAdaptive,
            NumericalSolver::StepperType::RungeKuttaCashKarp54,
            1e-3,
            1.0e-15,
            1.0e-15,
            RootSolver::Default(),
            [&loggedStates](const State &aState) -> void
            {
                loggedStates.add(aState);
            },
        };

        testing::internal::CaptureStdout();

        const State middleState = numericalSolver.integrateTime(defaultState_, middleInstant, systemOfEquations_);
        numericalSolver.integrateTime(
            middleState, endInstant, systemOfEquations_, numericalSolver.getObservedStates().getSize()
        );

        testing::internal::GetCapturedStdout();

        EXPECT_EQ(loggedStates, numericalSolver.getObservedStates());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_Array)
{
    const Array<Array<Instant>> instantsArray = {
        {
            defaultState_.getInstant() + Duration::Seconds(1.0),
            defaultState_.getInstant() + Duration::Seconds(4.0),
            defaultState_.getInstant() + Duration::Seconds(7.0),
            defaultState_.getInstant() + Duration::Seconds(10.0),
        },
        {
            defaultState_.getInstant() + Duration::Seconds(-1.0),
            defaultState_.getInstant() + Duration::Seconds(-4.0),
            defaultState_.getInstant() + Duration::Seconds(-7.0),
            defaultState_.getInstant() + Duration::Seconds(-10.0),
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
            state.getInstant(),
            systemOfEquations_,
            InstantCondition(RealCondition::Criterion::AnyCrossing, state.getInstant() + Duration::Seconds(60.0))
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
            InstantCondition(RealCondition::Criterion::StrictlyPositive, state.getInstant() - Duration::Seconds(1.0))
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
        EXPECT_EQ(conditionSolution.state, defaultRKD5_.getObservedStates().getLast());
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

                EXPECT_LT((finalState.getInstant() - endInstant).inSeconds(), 1e-12);
                EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
                EXPECT_EQ(conditionSolution.iterationCount, 0);
                EXPECT_FALSE(defaultRKD5_.getObservedStates().isEmpty());
            }

            const Instant targetInstant = defaultStartInstant_ + duration / 2.0;

            const InstantCondition condition = InstantCondition(criterion, targetInstant);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateTime(state, endInstant, systemOfEquations_, condition);

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.getCoordinates();
            const double propagatedTime = (conditionSolution.state.getInstant() - defaultStartInstant_).inSeconds();

            // Ensure that integration terminates at condition if condition is met

            EXPECT_LT((conditionSolution.state.getInstant() - targetInstant).inSeconds(), 1e-6);
            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
            EXPECT_EQ(conditionSolution.state, defaultRKD5_.getObservedStates().getLast());

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
        const Array<Tuple<double, double, RealCondition::Criterion, bool>> testCases = {
            {1.0, 0.0, RealCondition::Criterion::AnyCrossing, true},     // Condition happens at the start (forwards)
            {-1.0, 0.0, RealCondition::Criterion::AnyCrossing, true},    // Condition happens at the end (backwards)
            {1.0, 1.0, RealCondition::Criterion::AnyCrossing, true},     // Condition happens at the end (forwards)
            {-1.0, -1.0, RealCondition::Criterion::AnyCrossing, true},   // Condition happens at the end (backwards)
            {1.0, 2.0, RealCondition::Criterion::AnyCrossing, false},    // Condition never happens (forwards)
            {-1.0, -2.0, RealCondition::Criterion::AnyCrossing, false},  // Condition never happens (backwards)
        };

        for (const auto &testCase : testCases)
        {
            const double maximumPropagationTime = std::get<0>(testCase);
            const double target = std::get<1>(testCase);
            const RealCondition::Criterion criterion = std::get<2>(testCase);
            const InstantCondition condition =
                InstantCondition(criterion, defaultStartInstant_ + Duration::Seconds(target));

            const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
                state, defaultStartInstant_ + Duration::Seconds(maximumPropagationTime), systemOfEquations_, condition
            );

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.getCoordinates();
            const double propagatedTime = (conditionSolution.state.getInstant() - defaultStartInstant_).inSeconds();

            const bool expectedConditionIsSatisfied = std::get<3>(testCase);

            EXPECT_EQ(conditionSolution.conditionIsSatisfied, expectedConditionIsSatisfied);
            EXPECT_EQ(conditionSolution.rootSolverHasConverged, expectedConditionIsSatisfied);
            EXPECT_EQ(conditionSolution.state, defaultRKD5_.getObservedStates().getLast());
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
        const Array<Tuple<double, double, RealCondition::Criterion, double, bool>> testCases = {
            {2.0 * M_PI, 1.0, RealCondition::Criterion::AnyCrossing, 2.0 * M_PI, false
            },  // Almost meets the condition, but never does (forwards)
            {-2.0 * M_PI, 1.0, RealCondition::Criterion::AnyCrossing, -2.0 * M_PI, false
            },  // Almost meets the condition, but never does (backwards)
            {2.0 * M_PI, 0.5, RealCondition::Criterion::AnyCrossing, std::asin(0.5), true},  // Any crossing (forwards)
            {-2.0 * M_PI, 0.5, RealCondition::Criterion::AnyCrossing, -M_PI - std::asin(0.5), true
            },  // Any crossing (backwards)
            {2.0 * M_PI, 0.5, RealCondition::Criterion::NegativeCrossing, M_PI - std::asin(0.5), true
            },  // Negative crossing (forwards)
            {-2.0 * M_PI, 0.5, RealCondition::Criterion::NegativeCrossing, -2.0 * M_PI + std::asin(0.5), true
            },  // Negative crossing (backwards)
            {2.0 * M_PI, -0.5, RealCondition::Criterion::PositiveCrossing, 2.0 * M_PI - std::asin(0.5), true
            },  // Positive crossing (forwards)
            {-2.0 * M_PI, -0.5, RealCondition::Criterion::PositiveCrossing, -M_PI + std::asin(0.5), true
            },  // Positive crossing (backwards)
            {2.0 * M_PI, -0.5, RealCondition::Criterion::StrictlyNegative, M_PI + std::asin(0.5), true
            },  // Strictly negative (forwards)
            {-2.0 * M_PI, -0.5, RealCondition::Criterion::StrictlyNegative, -std::asin(0.5), true
            },  // Strictly negative (backwards)
            {2.0 * M_PI, 0.5, RealCondition::Criterion::StrictlyPositive, std::asin(0.5), true
            },  // Strictly positive (forwards)
            {-2.0 * M_PI, 0.5, RealCondition::Criterion::StrictlyPositive, -M_PI - std::asin(0.5), true
            },  // Strictly positive (backwards)
        };

        for (const auto &testCase : testCases)
        {
            const Duration duration = Duration::Seconds(std::get<0>(testCase));
            const double target = std::get<1>(testCase);
            const RealCondition::Criterion criterion = std::get<2>(testCase);
            const Instant endInstant = defaultStartInstant_ + duration;
            const RealCondition condition = RealCondition(
                "X Crossing Condition",
                criterion,
                [](const State &aState) -> double
                {
                    return aState.getCoordinates()[0];
                },
                target
            );
            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateTime(state, endInstant, systemOfEquations_, condition);

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.getCoordinates();
            const double propagatedTime = (conditionSolution.state.getInstant() - defaultStartInstant_).inSeconds();

            const double expectedPropagatedTime = std::get<3>(testCase);
            const bool expectedConditionIsSatisfied = std::get<4>(testCase);

            EXPECT_NEAR(propagatedTime, expectedPropagatedTime, 1e-8);
            EXPECT_EQ(conditionSolution.conditionIsSatisfied, expectedConditionIsSatisfied);
            EXPECT_EQ(conditionSolution.rootSolverHasConverged, expectedConditionIsSatisfied);
            EXPECT_EQ(conditionSolution.state, defaultRKD5_.getObservedStates().getLast());
            EXPECT_NEAR(propagatedStateVector[0], std::sin(propagatedTime), 1e-8);
            EXPECT_NEAR(propagatedStateVector[1], std::cos(propagatedTime), 1e-8);
        }
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
    EXPECT_LT(std::abs((conditionSolution.state.getInstant() - targetInstant).inSeconds()), 1e-6);
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
    EXPECT_LT(std::abs((conditionSolution.state.getInstant() - targetInstant).inSeconds()), 1e-6);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_Conditions_BracketReintegration)
{
    // The bracket around a detected crossing is re-integrated from the pre-step state. FSAL steppers
    // (Dormand-Prince 5) cache the derivative at their post-step state, so the re-integration must start from a
    // reset stepper; otherwise the first stage is evaluated at the wrong state and biases the crossing state.
    const State state = getStateVector(defaultStartInstant_);

    for (const double targetTime : {2.7, 3.7, 5.3})
    {
        NumericalSolver solver = {
            NumericalSolver::LogType::NoLog,
            NumericalSolver::StepperType::RungeKuttaDopri5,
            1.0,
            1.0e-10,
            1.0e-10,
        };

        const Instant targetInstant = defaultStartInstant_ + Duration::Seconds(targetTime);

        const NumericalSolver::ConditionSolution conditionSolution = solver.integrateTime(
            state,
            defaultStartInstant_ + Duration::Seconds(10.0),
            systemOfEquations_,
            InstantCondition(RealCondition::Criterion::AnyCrossing, targetInstant)
        );

        ASSERT_TRUE(conditionSolution.conditionIsSatisfied);

        const double solutionTime = (conditionSolution.state.getInstant() - defaultStartInstant_).inSeconds();
        const NumericalSolver::StateVector solutionStateVector = conditionSolution.state.getCoordinates();

        EXPECT_NEAR(std::sin(solutionTime), solutionStateVector[0], 3.0e-9) << "target " << targetTime;
        EXPECT_NEAR(std::cos(solutionTime), solutionStateVector[1], 3.0e-9) << "target " << targetTime;
    }
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

    for (const double durationSeconds : {1.0, -1.0})
    {
        const Duration duration = Duration::Seconds(durationSeconds);
        const Instant endInstant = defaultStartInstant_ + duration;
        const Instant targetInstant = defaultStartInstant_ + duration / 2.0;
        const InstantCondition condition = InstantCondition(RealCondition::Criterion::AnyCrossing, targetInstant);

        const NumericalSolver::ConditionSolution conditionSolution =
            solver.integrateTime(state, endInstant, systemOfEquations_, condition);

        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
        EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
        EXPECT_LT(std::abs((conditionSolution.state.getInstant() - targetInstant).inSeconds()), 1e-3);

        // Sanity: observed states are non-empty and last entry matches the solution state.
        EXPECT_FALSE(solver.getObservedStates().isEmpty());
        EXPECT_EQ(conditionSolution.state, solver.getObservedStates().getLast());
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

    for (const double durationSeconds : {1.0, -1.0})
    {
        const Duration duration = Duration::Seconds(durationSeconds);
        const Instant endInstant = defaultStartInstant_ + duration;

        // Place the condition target *outside* [start, end] so it is never reached.
        const Instant outsideTarget = defaultStartInstant_ + duration * 2.0;
        const InstantCondition condition = InstantCondition(RealCondition::Criterion::AnyCrossing, outsideTarget);

        const NumericalSolver::ConditionSolution conditionSolution =
            solver.integrateTime(state, endInstant, systemOfEquations_, condition);

        EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
        EXPECT_LT(std::abs((conditionSolution.state.getInstant() - endInstant).inSeconds()), 1e-9);

        const Array<State> observed = solver.getObservedStates();
        ASSERT_GE(observed.getSize(), 2u);

        // Strict monotonicity in the integration direction.
        for (size_t i = 1; i < observed.getSize(); ++i)
        {
            const double dt = (observed[i].getInstant() - observed[i - 1].getInstant()).inSeconds();
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
    EXPECT_LT(std::abs((conditionSolution.state.getInstant() - targetInstant).inSeconds()), 1e-6);
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
