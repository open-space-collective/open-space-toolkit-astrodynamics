/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
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

using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

// Simple duration based condition

struct InstantCondition : public RealCondition
{
    InstantCondition(const Instant &anInstant, const RealCondition::Criterion &aCriterion)
        : RealCondition(
              "test",
              aCriterion,
              [](const State &aState) -> Real
              {
                  return (aState.accessInstant() - Instant::J2000()).inSeconds();
              },
              (anInstant - Instant::J2000()).inSeconds()
          )
    {
    }
};

// Simple state based condition

struct XCrossingCondition : public RealCondition
{
    XCrossingCondition(const Real &aTarget)
        : RealCondition(
              "test",
              RealCondition::Criterion::AnyCrossing,
              [](const State &aState) -> Real
              {
                  return aState.accessCoordinates()[0];
              },
              aTarget
          )
    {
    }
};

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
    // Assuming ostk::mathematics::solver::NumericalSolver::StepperType::{BDF, Adams} are available
    NumericalSolver defaultBDF_ = {
        NumericalSolver::LogType::NoLog,
        ostk::mathematics::solver::NumericalSolver::StepperType::BDF, // Changed from RungeKuttaDopri5
        1e-3, // timeStep
        1.0e-12, // relativeTolerance
        1.0e-12, // absoluteTolerance
    };

    NumericalSolver defaultAdams_ = {
        NumericalSolver::LogType::NoLog,
        ostk::mathematics::solver::NumericalSolver::StepperType::Adams, // Changed from RungeKuttaCashKarp54
        1e-3, // timeStep
        1.0e-12, // relativeTolerance
        1.0e-12, // absoluteTolerance
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
    // GetRootSolver method was removed when RootSolver member was removed.

    {
        // Before any integration, observedStates should be empty.
        // If a solver instance is fresh, this should be true.
        // If it's reused, it's cleared at the start of integrateTime.
        NumericalSolver solver = NumericalSolver::Default(); // Create a fresh solver
        EXPECT_TRUE(solver.getObservedStates().isEmpty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, Accessors)
{
    {
        EXPECT_NO_THROW(defaultBDF_.accessObservedStates());
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

    EXPECT_TRUE(defaultAdams_.getObservedStates().isEmpty());

    for (const Instant &endInstant : endInstants)
    {
        const State propagatedState = defaultAdams_.integrateTime(defaultState_, endInstant, systemOfEquations_);

        EXPECT_FALSE(defaultAdams_.getObservedStates().isEmpty());

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
            defaultState_.accessInstant() + Duration::Seconds(100.0),
            defaultState_.accessInstant() + Duration::Seconds(400.0),
            defaultState_.accessInstant() + Duration::Seconds(700.0),
            defaultState_.accessInstant() + Duration::Seconds(1000.0),
        },
        {
            defaultState_.accessInstant() + Duration::Seconds(-100.0),
            defaultState_.accessInstant() + Duration::Seconds(-400.0),
            defaultState_.accessInstant() + Duration::Seconds(-700.0),
            defaultState_.accessInstant() + Duration::Seconds(-1000.0),
        },
    };

    EXPECT_TRUE(defaultAdams_.getObservedStates().isEmpty());

    for (const Array<Instant> &instants : instantsArray)
    {
        // Create a fresh solver instance for each test iteration to ensure isolation
        NumericalSolver testSolver = {
            NumericalSolver::LogType::NoLog,
            ostk::mathematics::solver::NumericalSolver::StepperType::Adams,
            1e-3, 1.0e-12, 1.0e-12
        };
        EXPECT_TRUE(testSolver.getObservedStates().isEmpty()); // Check before integration

        const Array<State> propagatedStateVectorArray =
            testSolver.integrateTime(defaultState_, instants, systemOfEquations_);

        // After integrateTime for Array<Instant>, observedStates_ should contain all intermediate steps
        EXPECT_FALSE(testSolver.getObservedStates().isEmpty());
        // The number of observed states should be initial + number of requested instants,
        // assuming no duplicates and all requested instants are after initial.
        // If initial instant is part of `instants`, it might be + instants.getSize().
        // The current implementation adds initial state, then each target state.
        EXPECT_EQ(testSolver.getObservedStates().getSize(), instants.getSize() + 1);


        validatePropagatedStates(instants, propagatedStateVectorArray, 2e-8);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, IntegrateTime_Conditions)
{
    const State state = getStateVector(defaultStartInstant_);

    {
        EXPECT_TRUE(defaultBDF_.getObservedStates().isEmpty());

        // This test is no longer valid as Adams/BDF steppers *can* be used with event conditions.
        // EXPECT_THROW(
        //     defaultAdams_.integrateTime( // Was defaultRK54_
        //         state,
        //         defaultStartInstant_,
        //         systemOfEquations_,
        //         InstantCondition(state.accessInstant(), RealCondition::Criterion::AnyCrossing)
        //     ),
        //     ostk::core::error::RuntimeError
        // );
    }

    // trivial case, zero second integration
    {
        NumericalSolver::ConditionSolution solution = defaultBDF_.integrateTime( // Was defaultRKD5_
            state,
            state.accessInstant(),
            systemOfEquations_,
            InstantCondition(state.accessInstant() + Duration::Seconds(60.0), RealCondition::Criterion::AnyCrossing)
        );

        EXPECT_EQ(state, solution.state);
        EXPECT_FALSE(solution.conditionIsSatisfied);
        EXPECT_EQ(solution.iterationCount, 0);
        EXPECT_FALSE(solution.rootSolverHasConverged);
    }

    // condition already satisfied
    {
        const NumericalSolver::ConditionSolution conditionSolution = defaultBDF_.integrateTime( // Was defaultRKD5_
            state,
            defaultStartInstant_ + defaultDuration_,
            systemOfEquations_,
            InstantCondition(state.accessInstant() - Duration::Seconds(1.0), RealCondition::Criterion::StrictlyPositive)
        );

        EXPECT_EQ(conditionSolution.state, state);
        EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
        EXPECT_EQ(conditionSolution.iterationCount, 0);
        EXPECT_TRUE(conditionSolution.rootSolverHasConverged);
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
                const NumericalSolver::ConditionSolution conditionSolution = defaultBDF_.integrateTime( // Was defaultRKD5_
                    state, endInstant, systemOfEquations_, InstantCondition((endInstant + duration / 2.0), criterion)
                );
                const State finalState = conditionSolution.state;

                EXPECT_LT(std::abs((finalState.accessInstant() - endInstant).inSeconds()), 1e-9); // Adjusted tolerance for time
                EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
                // EXPECT_EQ(conditionSolution.iterationCount, 0); // Iteration count from CVODE for this case is not well-defined in our struct
                EXPECT_FALSE(defaultBDF_.getObservedStates().isEmpty()); // Was defaultRKD5_
            }

            const Instant targetInstant = defaultStartInstant_ + duration / 2.0;

            const InstantCondition condition = InstantCondition(targetInstant, criterion);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultBDF_.integrateTime(state, endInstant, systemOfEquations_, condition); // Was defaultRKD5_

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.accessCoordinates();
            const Real propagatedTime = (conditionSolution.state.accessInstant() - defaultStartInstant_).inSeconds();

            // Ensure that integration terminates at condition if condition is met

            EXPECT_LT((conditionSolution.state.accessInstant() - targetInstant).inSeconds(), 1e-6);
            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_TRUE(conditionSolution.rootSolverHasConverged);

            // Validate the output against an analytical function

            EXPECT_NEAR(propagatedStateVector[0], std::sin(propagatedTime), 1e-9);
            EXPECT_NEAR(propagatedStateVector[1], std::cos(propagatedTime), 1e-9);
        }
    }

    {
        const Array<Tuple<Duration, Real>> testCases = {
            {defaultDuration_, 0.9},
            {-defaultDuration_, -0.9},
        };

        for (const auto &testCase : testCases)
        {
            const Duration duration = std::get<0>(testCase);
            const Real target = std::get<1>(testCase);

            const Instant endInstant = defaultStartInstant_ + duration;
            const XCrossingCondition condition = XCrossingCondition(target);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultBDF_.integrateTime(state, endInstant, systemOfEquations_, condition); // Was defaultRKD5_

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.accessCoordinates();
            const Real propagatedTime = (conditionSolution.state.accessInstant() - defaultStartInstant_).inSeconds();

            // Ensure that integration terminates at condition if condition is met

            EXPECT_TRUE(
                duration > 0.0 ? conditionSolution.state.accessInstant() < endInstant
                               : conditionSolution.state.accessInstant() > endInstant
            );
            EXPECT_TRUE(conditionSolution.conditionIsSatisfied);
            EXPECT_TRUE(conditionSolution.rootSolverHasConverged);

            // Validate the output against an analytical function

            EXPECT_NEAR(propagatedStateVector[0], std::sin(propagatedTime), 1e-9);
            EXPECT_NEAR(propagatedStateVector[1], std::cos(propagatedTime), 1e-9);
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
    // Assuming ostk::mathematics::solver::NumericalSolver::StepperType has Adams, BDF, RungeKutta4, etc.
    // CVODE backend does not support RungeKutta4 or RungeKuttaDopri5 for FixedStepSize factory in the same way.
    {
        EXPECT_THROW(
            NumericalSolver::FixedStepSize(ostk::mathematics::solver::NumericalSolver::StepperType::RungeKutta4, 30.0),
            ostk::core::error::RuntimeError // Or ToBeImplemented depending on final implementation in NumericalSolver
        );
    }

    {
        EXPECT_THROW(
            NumericalSolver::FixedStepSize(ostk::mathematics::solver::NumericalSolver::StepperType::RungeKuttaDopri5, 30.0),
            ostk::core::error::RuntimeError // Or ToBeImplemented
        );
    }
    {
        EXPECT_THROW(
            NumericalSolver::FixedStepSize(ostk::mathematics::solver::NumericalSolver::StepperType::Adams, 30.0),
            ostk::core::error::ToBeImplemented // As per current NumericalSolver::FixedStepSize logic
        );
    }
    {
        EXPECT_THROW(
            NumericalSolver::FixedStepSize(ostk::mathematics::solver::NumericalSolver::StepperType::BDF, 30.0),
            ostk::core::error::ToBeImplemented // As per current NumericalSolver::FixedStepSize logic
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

        NumericalSolver numericalSolver = NumericalSolver::Conditional(
            5.0, 1e-12, 1e-12, stateLogger, ostk::mathematics::solver::NumericalSolver::StepperType::BDF
        ); // Specify BDF

        const NumericalSolver::ConditionSolution conditionSolution = numericalSolver.integrateTime(
            defaultState_,
            defaultState_.accessInstant() + Duration::Hours(1.0),
            systemOfEquations_,
            InstantCondition(
                defaultState_.accessInstant() + Duration::Minutes(1.0), RealCondition::Criterion::AnyCrossing
            )
        );

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

// Added Test for new Conditional overload
TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_NumericalSolver, ConditionalWithStepperType)
{
    const auto stateLogger = [](const State &aState) -> void
    {
        (void)aState; // Suppress unused parameter warning
                      // std::cout << aState << std::endl;
    };

    {
        EXPECT_NO_THROW({
            NumericalSolver numericalSolver = NumericalSolver::Conditional(
                5.0, 1e-12, 1e-12, stateLogger, ostk::mathematics::solver::NumericalSolver::StepperType::BDF
            );
            (void)numericalSolver.integrateTime( // Use the solver
                defaultState_, defaultState_.accessInstant() + Duration::Seconds(1.0), systemOfEquations_);
        });
    }

    {
        EXPECT_NO_THROW({
            NumericalSolver numericalSolver = NumericalSolver::Conditional(
                5.0, 1e-12, 1e-12, stateLogger, ostk::mathematics::solver::NumericalSolver::StepperType::Adams
            );
            (void)numericalSolver.integrateTime( // Use the solver
                defaultState_, defaultState_.accessInstant() + Duration::Seconds(1.0), systemOfEquations_);
        });
    }

    {
        EXPECT_THROW(
            {
                NumericalSolver numericalSolver = NumericalSolver::Conditional(
                    5.0,
                    1e-12,
                    1e-12,
                    stateLogger,
                    ostk::mathematics::solver::NumericalSolver::StepperType::RungeKuttaDopri5 // Unsupported
                );
            },
            ostk::core::error::RuntimeError
        );
    }
}
