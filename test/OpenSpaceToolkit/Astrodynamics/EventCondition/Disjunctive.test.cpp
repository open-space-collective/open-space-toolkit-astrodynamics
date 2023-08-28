/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Disjunctive.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealEventCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::eventcondition::RealEventCondition;
using ostk::astro::eventcondition::Disjunctive;

class OpenSpaceToolkit_Astrodynamics_EventCondition_Disjunctive : public ::testing::Test
{
   protected:
    const RealEventCondition firstCondition_ = {
        "First",
        RealEventCondition::Criteria::PositiveCrossing,
        [](const VectorXd& aStateVector, [[maybe_unused]] const Real& aTime) -> Real
        {
            return aStateVector[0];
        },
        0.0,
    };
    const RealEventCondition secondCondition_ = {
        "Second",
        RealEventCondition::Criteria::StrictlyNegative,
        [](const VectorXd& aStateVector, [[maybe_unused]] const Real& aTime) -> Real
        {
            return aStateVector[1];
        },
        0.1,
    };
    Disjunctive disjunctionCondition_ = {{
        std::make_shared<RealEventCondition>(firstCondition_),
        std::make_shared<RealEventCondition>(secondCondition_),
    }};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_Disjunctive, Constructor)
{
    {
        EXPECT_NO_THROW(Disjunctive Disjunctive({
            std::make_shared<RealEventCondition>(firstCondition_),
            std::make_shared<RealEventCondition>(secondCondition_),
        }));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_Disjunctive, isSatisfied)
{
    // x0 crossing root (true), x1 negative (true)
    {
        VectorXd currentState(2);
        currentState << 1.0, 0.0;

        VectorXd previousState(2);
        previousState << -1.0, 3.0;

        EXPECT_TRUE(disjunctionCondition_.isSatisfied(currentState, 0.0, previousState, 1.0));
    }

    // x0 crossing root (true), x1 positive (false)
    {
        VectorXd currentState(2);
        currentState << 1.0, 1.0;

        VectorXd previousState(2);
        previousState << -1.0, 3.0;

        EXPECT_TRUE(disjunctionCondition_.isSatisfied(currentState, 0.0, previousState, 1.0));
    }

    // x0 not crossing root (false), x1 positive (false)
    {
        VectorXd currentState(2);
        currentState << -0.5, 1.0;

        VectorXd previousState(2);
        previousState << -1.0, 3.0;

        EXPECT_FALSE(disjunctionCondition_.isSatisfied(currentState, 0.0, previousState, 1.0));
    }

    // x0 not crossing root (false), x1 negative (true)
    {
        VectorXd currentState(2);
        currentState << -0.5, 0.0;

        VectorXd previousState(2);
        previousState << -1.0, 3.0;

        EXPECT_TRUE(disjunctionCondition_.isSatisfied(currentState, 0.0, previousState, 1.0));
    }
}
