/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/SemiMajorAxisCondition.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::Real;

using ostk::math::obj::VectorXd;

using ostk::physics::units::Length;
using ostk::physics::units::Derived;
using ostk::physics::environment::gravitational::Earth;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::SemiMajorAxisCondition;

class OpenSpaceToolkit_Astrodynamics_SemiMajorAxisCondition : public ::testing::Test
{
    void SetUp() override
    {
        defaultCondition_ =
            std::make_shared<SemiMajorAxisCondition>(defaultCriteria_, defaultSMA_, gravitationalParameter_);
    }

   protected:
    const SemiMajorAxisCondition::Criteria defaultCriteria_ = SemiMajorAxisCondition::Criteria::AnyCrossing;
    const Length defaultSMA_ = Length::Meters(7000000.0);
    const Derived gravitationalParameter_ = Earth::Spherical.gravitationalParameter_;
    Shared<SemiMajorAxisCondition> defaultCondition_ = nullptr;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_SemiMajorAxisCondition, Constructor)
{
    {
        EXPECT_NO_THROW(
            SemiMajorAxisCondition SemiMajorAxisCondition(defaultCriteria_, defaultSMA_, gravitationalParameter_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_SemiMajorAxisCondition, getSemiMajorAxis)
{
    {
        EXPECT_TRUE(defaultCondition_->getSemiMajorAxis() == defaultSMA_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_SemiMajorAxisCondition, getGravitationalParameter)
{
    {
        EXPECT_TRUE(defaultCondition_->getGravitationalParameter() == gravitationalParameter_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_SemiMajorAxisCondition, evaluate)
{
    const Real expectedSMA = 6904757.8910061345;
    VectorXd stateVector(6);
    stateVector << 717094.039086306, -6872433.2241124, 46175.9696673281, -970.650826004612, -45.4598114773158,
        7529.82424886455;

    {
        EXPECT_TRUE(defaultCondition_->evaluate(stateVector, 0.0) == (expectedSMA - defaultSMA_.inMeters()));
    }
}
