/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;

using ostk::physics::data::Scalar;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using ostk::physics::units::Time;
using ostk::physics::units::ElectricCurrent;
using ostk::physics::units::Angle;
using ostk::physics::units::Derived;
using ostk::physics::Unit;

using ostk::physics::environment::gravitational::Earth;

using ostk::astro::flight::system::PropulsionSystem;

Unit thrustSIUnit = Unit::Derived(Derived::Unit(
    Length::Unit::Meter,
    {1},
    Mass::Unit::Kilogram,
    {1},
    Time::Unit::Second,
    {-2},
    ElectricCurrent::Unit::Undefined,
    {0},
    Angle::Unit::Undefined,
    {0}
));
Unit specificImpulseSIUnit = Unit::Derived(Derived::Unit(
    Length::Unit::Undefined,
    {0},
    Mass::Unit::Undefined,
    {0},
    Time::Unit::Second,
    {1},
    ElectricCurrent::Unit::Undefined,
    {0},
    Angle::Unit::Undefined,
    {0}
));
Unit massFlowRateSIUnit = Unit::Derived(Derived::Unit(
    Length::Unit::Undefined,
    {0},
    Mass::Unit::Kilogram,
    {1},
    Time::Unit::Second,
    {-1},
    ElectricCurrent::Unit::Undefined,
    {0},
    Angle::Unit::Undefined,
    {0}
));

class OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        thrust_ = Scalar(0.01, thrustSIUnit);
        specificImpulse_ = Scalar(100.0, specificImpulseSIUnit);

        propulsionSystem_ = {
            thrust_, specificImpulse_
        };
    }

    Scalar thrust_ = Scalar::Undefined();
    Scalar specificImpulse_ = Scalar::Undefined();
    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();

};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Constructor)
{
    {
        EXPECT_NO_THROW(PropulsionSystem propulsionSystem(thrust_, specificImpulse_));
    }

    {
        const Scalar thrust = Scalar(0.01, thrustSIUnit);
        const Scalar specificImpulse = Scalar(100.0, specificImpulseSIUnit);

        EXPECT_NO_THROW(PropulsionSystem propulsionSystem(thrust, specificImpulse));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, EqualToOperator)
{
    {
        const Scalar thrust = Scalar(0.01, thrustSIUnit);
        const Scalar specificImpulse = Scalar(100.0, specificImpulseSIUnit);

        const PropulsionSystem propulsionSystem = {thrust, specificImpulse};

        EXPECT_TRUE(propulsionSystem == propulsionSystem_);
    }

    {
        const Scalar thrust = Scalar(0.02, thrustSIUnit);
        const Scalar specificImpulse = Scalar(100.0, specificImpulseSIUnit);

        const PropulsionSystem propulsionSystem = {thrust, specificImpulse};

        EXPECT_FALSE(propulsionSystem == propulsionSystem_);
    }

    {
        const Scalar thrust = Scalar(0.01, thrustSIUnit);
        const Scalar specificImpulse = Scalar(100.45, specificImpulseSIUnit);

        const PropulsionSystem propulsionSystem = {thrust, specificImpulse};

        EXPECT_FALSE(propulsionSystem == propulsionSystem_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, NotEqualToOperator)
{
    {
        const Scalar thrust = Scalar(0.01, thrustSIUnit);
        const Scalar specificImpulse = Scalar(100.0, specificImpulseSIUnit);

        const PropulsionSystem propulsionSystem = {thrust, specificImpulse};

        EXPECT_FALSE(propulsionSystem != propulsionSystem_);
    }

    {
        const Scalar thrust = Scalar(0.02, thrustSIUnit);
        const Scalar specificImpulse = Scalar(100.0, specificImpulseSIUnit);

        const PropulsionSystem propulsionSystem = {thrust, specificImpulse};

        EXPECT_TRUE(propulsionSystem != propulsionSystem_);
    }

    {
        const Scalar thrust = Scalar(0.01, thrustSIUnit);
        const Scalar specificImpulse = Scalar(100.45, specificImpulseSIUnit);

        const PropulsionSystem propulsionSystem = {thrust, specificImpulse};

        EXPECT_TRUE(propulsionSystem != propulsionSystem_);
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, GetThrust)
{
    {
        EXPECT_TRUE(propulsionSystem_.getThrust() == thrust_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, GetSpecificImpulse)
{
    {
        EXPECT_TRUE(propulsionSystem_.getSpecificImpulse() == specificImpulse_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, GetMassFlowRate)
{
    {
        const Scalar massFlowRate = {propulsionSystem_.getThrust().getValue() / (propulsionSystem_.getSpecificImpulse().getValue() * Earth::gravityConstant), massFlowRateSIUnit};
        EXPECT_TRUE(propulsionSystem_.getMassFlowRate() == massFlowRate);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, GetAcceleration)
{
    {
        const Mass mass = Mass(0.05, Mass::Unit::Kilogram);
        EXPECT_NO_THROW(propulsionSystem_.getAcceleration(mass));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_PropulsionSystem, Undefined)
{
    {
        EXPECT_NO_THROW(PropulsionSystem::Undefined());
    }
}
