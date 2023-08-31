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

class OpenSpaceToolkit_Astrodynamics_COECondition
    : public ::testing::TestWithParam<Tuple<COE::Element, Real, Real, String>>
{
    void SetUp() override
    {
        defaultStateVector_.resize(6);
        defaultStateVector_ << 717094.039086306, -6872433.2241124, 46175.9696673281, -970.650826004612,
            -45.4598114773158, 7529.82424886455;
    }

   protected:
    const COECondition::Criteria defaultCriteria_ = COECondition::Criteria::AnyCrossing;
    const COE::Element defaultElement_ = COE::Element::SemiMajorAxis;
    const Real defaultTarget_ = 7000000.0;
    const Derived gravitationalParameter_ = Earth::Spherical.gravitationalParameter_;

    const COECondition defaultCondition_ = COECondition(
        "COE Condition", defaultCriteria_, defaultElement_, defaultTarget_, gravitationalParameter_
    );

    VectorXd defaultStateVector_;
};

INSTANTIATE_TEST_SUITE_P(
    COECondition,
    OpenSpaceToolkit_Astrodynamics_COECondition,
    ::testing::Values(
        std::make_tuple(COE::Element::SemiMajorAxis, 7000000.0, 6904757.8910061345, "Semi-major axis"),
        std::make_tuple(COE::Element::Eccentricity, 0.0, 0.0010116019825255468, "Eccentricity"),
        std::make_tuple(COE::Element::Inclination, 1.5, 1.6989221681582849, "Inclination"),
        std::make_tuple(COE::Element::Aop, 2.4, 2.4052654657377115, "Argument of periapsis"),
        std::make_tuple(COE::Element::Raan, 4.8, 4.8172172435680096, "Right angle of ascending node"),
        std::make_tuple(COE::Element::TrueAnomaly, 3.8, 3.8846577046593076, "True anomaly"),
        std::make_tuple(COE::Element::MeanAnomaly, 3.8, 3.8860272646567751, "Mean anomaly"),
        std::make_tuple(COE::Element::EccentricAnomaly, 3.8, 3.8853423573058397, "Eccentric anomaly")
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_COECondition, Constructor)
{
    auto parameters = GetParam();
    {
        EXPECT_NO_THROW(COECondition(
            "COE condition",
            defaultCriteria_,
            std::get<0>(parameters),
            std::get<1>(parameters),
            gravitationalParameter_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, getTarget)
{
    {
        EXPECT_TRUE(defaultCondition_.getTarget() == defaultTarget_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, getGravitationalParameter)
{
    {
        EXPECT_TRUE(defaultCondition_.getGravitationalParameter() == gravitationalParameter_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, getElement)
{
    {
        EXPECT_TRUE(defaultCondition_.getElement() == defaultElement_);
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_COECondition, evaluate)
{
    auto parameters = GetParam();

    const COE::Element element = std::get<0>(parameters);
    const Real target = std::get<1>(parameters);
    const Real expectedValue = std::get<2>(parameters);

    const COECondition condition = {
        "COE condition",
        defaultCriteria_,
        element,
        target,
        gravitationalParameter_,
    };

    {
        EXPECT_DOUBLE_EQ(condition.evaluate(defaultStateVector_, 0.0), expectedValue - target);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, SemiMajorAxis)
{
    COECondition condition = COECondition::SemiMajorAxis(
        defaultCriteria_, Length::Meters(7000000.0), gravitationalParameter_
    );
    EXPECT_NO_THROW(condition.evaluate(defaultStateVector_, 0.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, Eccentricity)
{
    COECondition condition =
        COECondition::Eccentricity(defaultCriteria_, 0.0, gravitationalParameter_);
    EXPECT_NO_THROW(condition.evaluate(defaultStateVector_, 0.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, Inclination)
{
    COECondition condition =
        COECondition::Inclination(defaultCriteria_, Angle::Degrees(0.0), gravitationalParameter_);
    EXPECT_NO_THROW(condition.evaluate(defaultStateVector_, 0.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, Aop)
{
    COECondition condition =
        COECondition::Aop(defaultCriteria_, Angle::Degrees(0.0), gravitationalParameter_);
    EXPECT_NO_THROW(condition.evaluate(defaultStateVector_, 0.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, Raan)
{
    COECondition condition =
        COECondition::Raan(defaultCriteria_, Angle::Degrees(0.0), gravitationalParameter_);
    EXPECT_NO_THROW(condition.evaluate(defaultStateVector_, 0.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, TrueAnomaly)
{
    COECondition condition =
        COECondition::TrueAnomaly(defaultCriteria_, Angle::Degrees(0.0), gravitationalParameter_);
    EXPECT_NO_THROW(condition.evaluate(defaultStateVector_, 0.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, MeanAnomaly)
{
    COECondition condition =
        COECondition::MeanAnomaly(defaultCriteria_, Angle::Degrees(0.0), gravitationalParameter_);
    EXPECT_NO_THROW(condition.evaluate(defaultStateVector_, 0.0));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_COECondition, EccentricAnomaly)
{
    COECondition condition = COECondition::EccentricAnomaly(
        defaultCriteria_, Angle::Degrees(0.0), gravitationalParameter_
    );
    EXPECT_NO_THROW(condition.evaluate(defaultStateVector_, 0.0));
}
