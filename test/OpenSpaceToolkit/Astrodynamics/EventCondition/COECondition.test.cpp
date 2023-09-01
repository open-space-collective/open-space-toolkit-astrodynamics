/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Tuple;
using ostk::core::types::Shared;
using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::units::Angle;
using ostk::physics::units::Length;
using ostk::physics::units::Derived;
using ostk::physics::coord::Frame;
using ostk::physics::environment::gravitational::Earth;

using ostk::astro::eventcondition::COECondition;
using ostk::astro::trajectory::orbit::models::kepler::COE;

class OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition : public ::testing::TestWithParam<Tuple<COE::Element, Real, Real>>
{
    void SetUp() override
    {
        defaultStateVector_.resize(6);
        defaultStateVector_ << 717094.039086306, -6872433.2241124, 46175.9696673281, -970.650826004612,
            -45.4598114773158, 7529.82424886455;
    }

   protected:
    const String defaultName_ = "COE Condition";
    const COECondition::Criteria defaultCriteria_ = COECondition::Criteria::AnyCrossing;
    const COE::Element defaultElement_ = COE::Element::SemiMajorAxis;
    const Real defaultTarget_ = 7000000.0;
    const Derived gravitationalParameter_ = Earth::Spherical.gravitationalParameter_;

    const COECondition defaultCondition_ =
        COECondition(defaultName_, defaultCriteria_, defaultElement_, defaultTarget_, gravitationalParameter_);

    VectorXd defaultStateVector_;
    Real defaultTime_ = 0.0;
};

INSTANTIATE_TEST_SUITE_P(
    COECondition,
    OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition,
    ::testing::Values(
        std::make_tuple(COE::Element::SemiMajorAxis, 7000000.0, 6904757.8910061345),
        std::make_tuple(COE::Element::Eccentricity, 0.0, 0.0010116019825255468),
        std::make_tuple(COE::Element::Inclination, 1.5, 1.6989221681582849),
        std::make_tuple(COE::Element::Aop, 2.4, 2.4052654657377115),
        std::make_tuple(COE::Element::Raan, 4.8, 4.8172172435680096),
        std::make_tuple(COE::Element::TrueAnomaly, 3.8, 3.8846577046593076),
        std::make_tuple(COE::Element::MeanAnomaly, 3.8, 3.8860272646567751),
        std::make_tuple(COE::Element::EccentricAnomaly, 3.8, 3.8853423573058397)
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Constructor)
{
    {
        EXPECT_NO_THROW(
            COECondition(defaultName_, defaultCriteria_, defaultElement_, defaultTarget_, gravitationalParameter_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, getTarget)
{
    {
        EXPECT_TRUE(defaultCondition_.getTarget() == defaultTarget_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, getGravitationalParameter)
{
    {
        EXPECT_TRUE(defaultCondition_.getGravitationalParameter() == gravitationalParameter_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, getElement)
{
    {
        EXPECT_TRUE(defaultCondition_.getElement() == defaultElement_);
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, evaluate)
{
    auto parameters = GetParam();

    const COE::Element element = std::get<0>(parameters);
    const Real target = std::get<1>(parameters);
    const Real expectedValue = std::get<2>(parameters);

    const COECondition condition = {
        defaultName_,
        defaultCriteria_,
        element,
        target,
        gravitationalParameter_,
    };

    {
        EXPECT_DOUBLE_EQ(condition.evaluate(defaultStateVector_, defaultTime_), expectedValue - target);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, SemiMajorAxis)
{
    COECondition condition =
        COECondition::SemiMajorAxis(defaultCriteria_, Length::Meters(7000000.0), gravitationalParameter_);

    EXPECT_EQ(condition.getName(), "Semi-major axis Condition");
    EXPECT_EQ(condition.getCriteria(), defaultCriteria_);
    EXPECT_EQ(condition.getElement(), COE::Element::SemiMajorAxis);
    EXPECT_EQ(condition.getTarget(), 7000000.0);
    EXPECT_EQ(condition.evaluate(defaultStateVector_, defaultTime_), 6904757.8910061345 - 7000000.0);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Eccentricity)
{
    COECondition condition = COECondition::Eccentricity(defaultCriteria_, 0.0005, gravitationalParameter_);

    EXPECT_EQ(condition.getName(), "Eccentricity Condition");
    EXPECT_EQ(condition.getCriteria(), defaultCriteria_);
    EXPECT_EQ(condition.getElement(), COE::Element::Eccentricity);
    EXPECT_EQ(condition.getTarget(), 0.0005);
    EXPECT_EQ(condition.evaluate(defaultStateVector_, defaultTime_), 0.0010116019825255468 - 0.0005);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Inclination)
{
    COECondition condition = COECondition::Inclination(defaultCriteria_, Angle::Degrees(90), gravitationalParameter_);
    EXPECT_EQ(condition.getName(), "Inclination Condition");
    EXPECT_EQ(condition.getCriteria(), defaultCriteria_);
    EXPECT_EQ(condition.getElement(), COE::Element::Inclination);
    EXPECT_EQ(condition.getTarget(), Angle::HalfPi().inRadians());
    EXPECT_EQ(condition.evaluate(defaultStateVector_, defaultTime_), 1.6989221681582849 - Angle::HalfPi().inRadians());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Aop)
{
    COECondition condition = COECondition::Aop(defaultCriteria_, Angle::Degrees(90), gravitationalParameter_);
    EXPECT_EQ(condition.getName(), "Argument of periapsis Condition");
    EXPECT_EQ(condition.getCriteria(), defaultCriteria_);
    EXPECT_EQ(condition.getElement(), COE::Element::Aop);
    EXPECT_EQ(condition.getTarget(), Angle::HalfPi().inRadians());
    EXPECT_EQ(condition.evaluate(defaultStateVector_, defaultTime_), 2.4052654657377115 - Angle::HalfPi().inRadians());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Raan)
{
    COECondition condition = COECondition::Raan(defaultCriteria_, Angle::Degrees(90), gravitationalParameter_);
    EXPECT_EQ(condition.getName(), "Right angle of ascending node Condition");
    EXPECT_EQ(condition.getCriteria(), defaultCriteria_);
    EXPECT_EQ(condition.getElement(), COE::Element::Raan);
    EXPECT_EQ(condition.getTarget(), Angle::HalfPi().inRadians());
    EXPECT_EQ(condition.evaluate(defaultStateVector_, defaultTime_), 4.8172172435680096 - Angle::HalfPi().inRadians());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, TrueAnomaly)
{
    COECondition condition = COECondition::TrueAnomaly(defaultCriteria_, Angle::Degrees(90), gravitationalParameter_);
    EXPECT_EQ(condition.getName(), "True anomaly Condition");
    EXPECT_EQ(condition.getCriteria(), defaultCriteria_);
    EXPECT_EQ(condition.getElement(), COE::Element::TrueAnomaly);
    EXPECT_EQ(condition.getTarget(), Angle::HalfPi().inRadians());
    EXPECT_EQ(condition.evaluate(defaultStateVector_, defaultTime_), 3.8846577046593076 - Angle::HalfPi().inRadians());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, MeanAnomaly)
{
    COECondition condition = COECondition::MeanAnomaly(defaultCriteria_, Angle::Degrees(90), gravitationalParameter_);
    EXPECT_EQ(condition.getName(), "Mean anomaly Condition");
    EXPECT_EQ(condition.getCriteria(), defaultCriteria_);
    EXPECT_EQ(condition.getElement(), COE::Element::MeanAnomaly);
    EXPECT_EQ(condition.getTarget(), Angle::HalfPi().inRadians());
    EXPECT_EQ(condition.evaluate(defaultStateVector_, defaultTime_), 3.8860272646567751 - Angle::HalfPi().inRadians());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, EccentricAnomaly)
{
    COECondition condition =
        COECondition::EccentricAnomaly(defaultCriteria_, Angle::Degrees(90), gravitationalParameter_);
    EXPECT_EQ(condition.getName(), "Eccentric anomaly Condition");
    EXPECT_EQ(condition.getCriteria(), defaultCriteria_);
    EXPECT_EQ(condition.getElement(), COE::Element::EccentricAnomaly);
    EXPECT_EQ(condition.getTarget(), Angle::HalfPi().inRadians());
    EXPECT_EQ(condition.evaluate(defaultStateVector_, defaultTime_), 3.8853423573058397 - Angle::HalfPi().inRadians());
}
