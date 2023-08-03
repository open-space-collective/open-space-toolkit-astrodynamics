/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/Conjunctive.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::Conjunctive;

class FirstCondition : public EventCondition
{
   public:
    FirstCondition()
        : EventCondition("First", EventCondition::Criteria::PositiveCrossing)
    {
    }

    virtual Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override
    {
        (void)aTime;
        return aStateVector[0];
    }
};

class SecondCondition : public EventCondition
{
   public:
    SecondCondition()
        : EventCondition("Second", EventCondition::Criteria::StrictlyNegative)
    {
    }

    virtual Real evaluate(const VectorXd& aStateVector, const Real& aTime) const override
    {
        (void)aTime;
        return aStateVector[1] - 0.1;
    }
};

class OpenSpaceToolkit_Astrodynamics_EventCondition_Conjunctive : public ::testing::Test
{
    void SetUp() override
    {
        conjuntionCondition_ = {{
            std::make_shared<FirstCondition>(firstCondition_),
            std::make_shared<SecondCondition>(secondCondition_),
        }};
    }

   protected:
    const FirstCondition firstCondition_;
    const SecondCondition secondCondition_;
    Conjunctive conjuntionCondition_ = {{}};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_Conjunctive, Constructor)
{
    {
        EXPECT_NO_THROW(Conjunctive conjunction({
            std::make_shared<FirstCondition>(firstCondition_),
            std::make_shared<SecondCondition>(secondCondition_),
        }));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_Conjunctive, isSatisfied)
{
    // x0 crossing root (true), x1 negative (true)
    {
        VectorXd currentState(2);
        currentState << 1.0, 0.0;

        VectorXd previousState(2);
        previousState << -1.0, 3.0;

        EXPECT_TRUE(conjuntionCondition_.isSatisfied(currentState, 0.0, previousState, 1.0));
    }

    // x0 crossing root (true), x1 positive (false)
    {
        VectorXd currentState(2);
        currentState << 1.0, 1.0;

        VectorXd previousState(2);
        previousState << -1.0, 3.0;

        EXPECT_FALSE(conjuntionCondition_.isSatisfied(currentState, 0.0, previousState, 1.0));
    }

    // x0 not crossing root (false), x1 positive (false)
    {
        VectorXd currentState(2);
        currentState << -0.5, 1.0;

        VectorXd previousState(2);
        previousState << -1.0, 3.0;

        EXPECT_FALSE(conjuntionCondition_.isSatisfied(currentState, 0.0, previousState, 1.0));
    }

    // x0 not crossing root (false), x1 negative (true)
    {
        VectorXd currentState(2);
        currentState << -0.5, 0.0;

        VectorXd previousState(2);
        previousState << -1.0, 3.0;

        EXPECT_FALSE(conjuntionCondition_.isSatisfied(currentState, 0.0, previousState, 1.0));
    }
}
