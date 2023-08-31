/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Tuple;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::time::Duration;
using ostk::physics::time::DateTime;
using ostk::physics::time::Scale;
using ostk::physics::coord::Frame;

using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::eventcondition::RealEventCondition;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::State;

// Simple duration based condition

struct DurationCondition : public RealEventCondition
{
    DurationCondition(const Duration &aDuration, const RealEventCondition::Criteria &aCriteria)
        : RealEventCondition(
              "test",
              aCriteria,
              []([[maybe_unused]] const VectorXd &aStateVector, const Real &aTime) -> Real
              {
                  return aTime;
              },
              aDuration.inSeconds()
          )
    {
    }
};

// Simple state based condition

struct XCrossingCondition : public RealEventCondition
{
    XCrossingCondition(const Real &aTarget)
        : RealEventCondition(
              "test",
              RealEventCondition::Criteria::AnyCrossing,
              [](const VectorXd &aStateVector, [[maybe_unused]] const double &aTime) -> Real
              {
                  return aStateVector[0];
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
            defaultCoordinatesBroker_,
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
    const Shared<CoordinatesBroker> defaultCoordinatesBroker_ =
        std::make_shared<CoordinatesBroker>(CoordinatesBroker({std::make_shared<CoordinatesSubset>("Test", 2)}));
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
            defaultCoordinatesBroker_,
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

    EXPECT_TRUE(defaultRKD5_.getObservedStates().isEmpty());

    {
        const Array<Tuple<Duration, RealEventCondition::Criteria>> testCases = {
            {defaultDuration_, RealEventCondition::Criteria::AnyCrossing},
            {-defaultDuration_, RealEventCondition::Criteria::AnyCrossing},
        };

        for (const auto &testCase : testCases)
        {
            const Duration duration = std::get<0>(testCase);
            const RealEventCondition::Criteria criteria = std::get<1>(testCase);

            const Instant endInstant = defaultStartInstant_ + duration;

            {
                const NumericalSolver::ConditionSolution conditionSolution = defaultRKD5_.integrateTime(
                    state,
                    endInstant,
                    systemOfEquations_,
                    DurationCondition(((endInstant - defaultStartInstant_) + duration / 2.0), criteria)
                );
                const State finalState = conditionSolution.state;

                EXPECT_LT((finalState.accessInstant() - endInstant).inSeconds(), 1e-12);
                EXPECT_FALSE(conditionSolution.conditionIsSatisfied);
                EXPECT_EQ(conditionSolution.iterationCount, 0);
                EXPECT_TRUE(defaultRKD5_.getObservedStates().isEmpty());
            }

            const DurationCondition condition = DurationCondition(duration / 2.0, criteria);

            const NumericalSolver::ConditionSolution conditionSolution =
                defaultRKD5_.integrateTime(state, endInstant, systemOfEquations_, condition);

            const NumericalSolver::StateVector propagatedStateVector = conditionSolution.state.accessCoordinates();
            const Real propagatedTime = (conditionSolution.state.accessInstant() - defaultStartInstant_).inSeconds();

            // Ensure that integration terminates at condition if condition is met

            EXPECT_NEAR(propagatedTime, condition.getTarget(), 1e-6);
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
                defaultRKD5_.integrateTime(state, endInstant, systemOfEquations_, condition);

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
