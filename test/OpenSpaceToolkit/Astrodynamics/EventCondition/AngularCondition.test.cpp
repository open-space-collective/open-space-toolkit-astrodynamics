/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/AngularCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Pair;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::object::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;
using ostk::physics::units::Angle;
using ostk::physics::units::Length;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::AngularCondition;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::StateBuilder;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;

class OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition : public ::testing::Test
{
   protected:
    const AngularCondition::Criterion defaultCriterion_ = AngularCondition::Criterion::PositiveCrossing;
    const String defaultName_ = "Test Angular Condition";
    const std::function<Real(const State&)> defaultEvaluator_ = [](const State& aState) -> Real
    {
        return aState.accessCoordinates()[0];
    };
    const Angle defaultTargetAngle_ = Angle::Degrees(1.0);
    const Pair<Angle, Angle> defaultTargetRange_ = {Angle::Degrees(0.0), Angle::Degrees(2.0)};
    const AngularCondition defaultCondition_ = {
        defaultName_, defaultCriterion_, defaultEvaluator_, defaultTargetAngle_
    };

    const Array<Shared<const CoordinatesSubset>> defaultSubsets_ = {
        std::make_shared<CoordinatesSubset>(CoordinatesSubset("ANGLE", 1))
    };
    const StateBuilder defaultStateBuilder_ = StateBuilder(Frame::GCRF(), defaultSubsets_);

    const State generateState(const Real& coordinate)
    {
        VectorXd coordinates(1);
        coordinates << coordinate;

        return defaultStateBuilder_.build(Instant::J2000(), coordinates);
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition, Constructor)
{
    {
        EXPECT_NO_THROW(AngularCondition(defaultName_, defaultCriterion_, defaultEvaluator_, defaultTargetAngle_));
        EXPECT_NO_THROW(AngularCondition(
            defaultName_, defaultCriterion_, defaultEvaluator_, EventCondition::Target(defaultTargetAngle_)
        ));
    }

    {
        EXPECT_NO_THROW(AngularCondition::WithinRange(defaultName_, defaultEvaluator_, defaultTargetRange_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition, Print)
{
    {
        testing::internal::CaptureStdout();
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, true));
        EXPECT_NO_THROW(defaultCondition_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        testing::internal::CaptureStdout();
        const AngularCondition angularCondition =
            AngularCondition::WithinRange(defaultName_, defaultEvaluator_, defaultTargetRange_);
        EXPECT_NO_THROW(angularCondition.print(std::cout, true));
        EXPECT_NO_THROW(angularCondition.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition, Getters)
{
    {
        EXPECT_TRUE(defaultCondition_.getName() == defaultName_);
        EXPECT_TRUE(defaultCondition_.getCriterion() == defaultCriterion_);
        EXPECT_TRUE(defaultCondition_.getTargetAngle() == defaultTargetAngle_);
        EXPECT_THROW(defaultCondition_.getTargetRange(), ostk::core::error::RuntimeError);
    }

    {
        const AngularCondition angularCondition =
            AngularCondition::WithinRange(defaultName_, defaultEvaluator_, defaultTargetRange_);
        EXPECT_TRUE(angularCondition.getTargetRange() == defaultTargetRange_);
        EXPECT_THROW(angularCondition.getTargetAngle(), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition, isSatisfied)
{
    // Positive Crossing
    {
        // Some target value
        {
            const Angle targetAngle = Angle::Degrees(45.0);

            AngularCondition condition = {
                "PositiveCrossing",
                AngularCondition::Criterion::PositiveCrossing,
                defaultEvaluator_,
                targetAngle,
            };

            // Target crossing
            {
                const State previousState = generateState(targetAngle.inRadians() - 0.1);
                const State currentState = generateState(targetAngle.inRadians() + 0.1);

                EXPECT_TRUE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }

            // Target + 180.0 crossing
            {
                const State previousState = generateState(targetAngle.inRadians() - 0.1 + 180.0);
                const State currentState = generateState(targetAngle.inRadians() + 0.1 + 180.0);

                EXPECT_FALSE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }
        }

        // 0.0 target value
        {
            const Angle targetAngle = Angle::Degrees(0.0);

            AngularCondition condition = {
                "PositiveCrossing",
                AngularCondition::Criterion::PositiveCrossing,
                defaultEvaluator_,
                targetAngle,
            };

            // Target crossing
            {
                const State previousState = generateState(targetAngle.inRadians() - 0.1);
                const State currentState = generateState(targetAngle.inRadians() + 0.1);

                EXPECT_TRUE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }

            // Target + 180.0 crossing
            {
                const State previousState = generateState(targetAngle.inRadians() - 0.1 + 180.0);
                const State currentState = generateState(targetAngle.inRadians() + 0.1 + 180.0);

                EXPECT_FALSE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }
        }
    }

    // Negative Crossing
    {
        // Some target value
        {
            const Angle targetAngle = Angle::Degrees(45.0);

            AngularCondition condition = {
                "NegativeCrossing",
                AngularCondition::Criterion::NegativeCrossing,
                defaultEvaluator_,
                targetAngle,
            };

            // Target crossing
            {
                const State previousState = generateState(targetAngle.inRadians() + 0.1);
                const State currentState = generateState(targetAngle.inRadians() - 0.1);

                EXPECT_TRUE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }

            // Target + 180.0 crossing
            {
                const State previousState = generateState(targetAngle.inRadians() + 0.1 + 180.0);
                const State currentState = generateState(targetAngle.inRadians() - 0.1 + 180.0);

                EXPECT_FALSE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }
        }

        // 0.0 target value
        {
            const Angle targetAngle = Angle::Degrees(0.0);

            AngularCondition condition = {
                "NegativeCrossing",
                AngularCondition::Criterion::NegativeCrossing,
                defaultEvaluator_,
                targetAngle,
            };

            // Target crossing
            {
                const State previousState = generateState(targetAngle.inRadians() + 0.1);
                const State currentState = generateState(targetAngle.inRadians() - 0.1);

                EXPECT_TRUE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }

            // Target + 180.0 crossing
            {
                const State previousState = generateState(targetAngle.inRadians() + 0.1 + 180.0);
                const State currentState = generateState(targetAngle.inRadians() - 0.1 + 180.0);

                EXPECT_FALSE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }
        }
    }

    // Any Crossing
    {
        // Some target value
        {
            const Angle targetAngle = Angle::Degrees(45.0);

            AngularCondition condition = {
                "AnyCrossing",
                AngularCondition::Criterion::AnyCrossing,
                defaultEvaluator_,
                targetAngle,
            };

            // Target crossing
            {
                const State previousState = generateState(targetAngle.inRadians() - 0.1);
                const State currentState = generateState(targetAngle.inRadians() + 0.1);

                EXPECT_TRUE(condition.isSatisfied(currentState, previousState));
                EXPECT_TRUE(condition.isSatisfied(previousState, currentState));
            }

            // Target + 180.0 crossing
            {
                const State previousState = generateState(targetAngle.inRadians() - 0.1 + 180.0);
                const State currentState = generateState(targetAngle.inRadians() + 0.1 + 180.0);

                EXPECT_FALSE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }
        }

        // 0.0 target value
        {
            const Angle targetAngle = Angle::Degrees(0.0);

            AngularCondition condition = {
                "AnyCrossing",
                AngularCondition::Criterion::AnyCrossing,
                defaultEvaluator_,
                targetAngle,
            };

            // Target crossing
            {
                const State previousState = generateState(targetAngle.inRadians() - 0.1);
                const State currentState = generateState(targetAngle.inRadians() + 0.1);

                EXPECT_TRUE(condition.isSatisfied(currentState, previousState));
                EXPECT_TRUE(condition.isSatisfied(previousState, currentState));
            }

            // Target + 180.0 crossing
            {
                const State previousState = generateState(targetAngle.inRadians() - 0.1 + 180.0);
                const State currentState = generateState(targetAngle.inRadians() + 0.1 + 180.0);

                EXPECT_FALSE(condition.isSatisfied(currentState, previousState));
                EXPECT_FALSE(condition.isSatisfied(previousState, currentState));
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_AngularCondition, StringFromCriterion)
{
    EXPECT_TRUE(
        AngularCondition::StringFromCriterion(AngularCondition::Criterion::PositiveCrossing) == "Positive Crossing"
    );
    EXPECT_TRUE(
        AngularCondition::StringFromCriterion(AngularCondition::Criterion::NegativeCrossing) == "Negative Crossing"
    );
    EXPECT_TRUE(AngularCondition::StringFromCriterion(AngularCondition::Criterion::AnyCrossing) == "Any Crossing");
    EXPECT_TRUE(AngularCondition::StringFromCriterion(AngularCondition::Criterion::WithinRange) == "Within Range");
}
