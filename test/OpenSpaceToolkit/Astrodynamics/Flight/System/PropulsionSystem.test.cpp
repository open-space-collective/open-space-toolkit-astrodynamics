/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;

using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;
using ostk::physics::unit::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;

using ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::flight::system::PropulsionSystem;

class OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem : public ::testing::Test
{
   protected:
    const Real thrust_ = 0.01;
    const Real specificImpulse_ = 100.0;
    const PropulsionSystem propulsionSystem_ = PropulsionSystem(thrust_, specificImpulse_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Constructor)
{
    {
        EXPECT_NO_THROW(PropulsionSystem(thrust_, specificImpulse_));
    }

    {
        EXPECT_NO_THROW(PropulsionSystem(Real::Undefined(), specificImpulse_));
    }

    {
        EXPECT_NO_THROW(PropulsionSystem(thrust_, Real::Undefined()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, EqualToOperator)
{
    {
        EXPECT_TRUE(PropulsionSystem(thrust_, specificImpulse_) == propulsionSystem_);
    }

    {
        EXPECT_FALSE(PropulsionSystem(0.02, specificImpulse_) == propulsionSystem_);
    }

    {
        EXPECT_FALSE(PropulsionSystem(thrust_, 100.45) == propulsionSystem_);
    }

    {
        EXPECT_NO_THROW(PropulsionSystem::Undefined() == propulsionSystem_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, NotEqualToOperator)
{
    {
        EXPECT_FALSE(PropulsionSystem(thrust_, specificImpulse_) != propulsionSystem_);
    }

    {
        EXPECT_TRUE(PropulsionSystem(0.02, specificImpulse_) != propulsionSystem_);
    }

    {
        EXPECT_TRUE(PropulsionSystem(thrust_, 100.45) != propulsionSystem_);
    }

    {
        EXPECT_NO_THROW(PropulsionSystem::Undefined() != propulsionSystem_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, IsDefined)
{
    EXPECT_TRUE(propulsionSystem_.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << propulsionSystem_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(propulsionSystem_.print(std::cout, true));
        EXPECT_NO_THROW(propulsionSystem_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Getters)
{
    {
        EXPECT_TRUE(propulsionSystem_.getThrust() == thrust_);
    }

    {
        EXPECT_TRUE(propulsionSystem_.getSpecificImpulse() == specificImpulse_);
    }

    {
        const Real massFlowRate =
            propulsionSystem_.getThrust() / (propulsionSystem_.getSpecificImpulse() * Earth::gravityConstant);
        EXPECT_TRUE(propulsionSystem_.getMassFlowRate() == massFlowRate);
    }

    {
        const Mass mass = Mass(0.05, Mass::Unit::Kilogram);
        EXPECT_NO_THROW(propulsionSystem_.getAcceleration(mass));
    }

    {
        EXPECT_ANY_THROW(PropulsionSystem::Undefined().getThrust());
        EXPECT_ANY_THROW(PropulsionSystem::Undefined().getSpecificImpulse());
        EXPECT_ANY_THROW(PropulsionSystem::Undefined().getMassFlowRate());
        EXPECT_ANY_THROW(PropulsionSystem::Undefined().getAcceleration(Mass(0.05, Mass::Unit::Kilogram)));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Undefined)
{
    {
        EXPECT_NO_THROW(PropulsionSystem::Undefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Default)
{
    {
        EXPECT_NO_THROW(PropulsionSystem::Default());
    }
}
