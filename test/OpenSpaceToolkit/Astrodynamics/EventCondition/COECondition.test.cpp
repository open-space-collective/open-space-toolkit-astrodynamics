/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Tuple;
using ostk::core::types::Shared;
using ostk::core::types::Real;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Instant;
using ostk::physics::units::Angle;
using ostk::physics::units::Length;
using ostk::physics::units::Derived;

using ostk::astro::eventcondition::COECondition;
using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

const auto getCrossZ = [](const Real& anAngle, const Real& aTarget) -> Real
{
    return (std::sin(anAngle) * std::cos(aTarget) - std::cos(anAngle) * std::sin(aTarget));
};

class OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition
    : public ::testing::TestWithParam<Tuple<COE::Element, Real, Real>>
{
    void SetUp() override
    {
        VectorXd posVelInFrame;
        posVelInFrame.resize(6);
        posVelInFrame << 717094.039086306, -6872433.2241124, 46175.9696673281, -970.650826004612, -45.4598114773158,
            7529.82424886455;
        const State defaultStateInFrame =
            State(defaultInstant_, posVelInFrame, defaultFrame_, defaultCoordinatesBroker_);

        defaultStateInIntegrationFrame_ = defaultStateInFrame.inFrame(defaultIntegrationFrame_);

        Position currentPosition = Position::Undefined();
        Position previousPosition = Position::Undefined();
        Velocity currentVelocity = Velocity::Undefined();
        Velocity previousVelocity = Velocity::Undefined();

        std::tie(currentPosition, currentVelocity) =
            COE(Length::Kilometers(550.0) + Earth::EGM2008.equatorialRadius_,
                0.2,
                Angle::Degrees(16.0),
                Angle::Degrees(1.0),
                Angle::Degrees(1.0),
                Angle::Degrees(1.0))
                .getCartesianState(Earth::EGM2008.gravitationalParameter_, defaultFrame_);

        std::tie(previousPosition, previousVelocity) =
            COE(Length::Kilometers(550.0) + Earth::EGM2008.equatorialRadius_,
                0.2,
                Angle::Degrees(15.0),
                Angle::Degrees(359.0),
                Angle::Degrees(359.0),
                Angle::Degrees(359.0))
                .getCartesianState(Earth::EGM2008.gravitationalParameter_, defaultFrame_);

        currentState_ = {defaultInstant_, currentPosition, currentVelocity};
        previousState_ = {defaultInstant_, previousPosition, previousVelocity};
    }

   protected:
    const String defaultName_ = "COE Condition";
    const COECondition::Criterion defaultCriterion_ = COECondition::Criterion::AnyCrossing;
    const COE::Element defaultElement_ = COE::Element::SemiMajorAxis;
    const Real defaultTarget_ = 7000000.0;
    const Derived gravitationalParameter_ = Earth::Spherical.gravitationalParameter_;
    const Instant defaultInstant_ = Instant::J2000();
    const Shared<const Frame> defaultFrame_ = Frame::TEME();
    const Shared<const Frame> defaultIntegrationFrame_ = Frame::GCRF();
    const Shared<const CoordinatesBroker> defaultCoordinatesBroker_ = std::make_shared<CoordinatesBroker>(
        CoordinatesBroker({CartesianPosition::Default(), CartesianVelocity::Default()})
    );
    const COECondition defaultCondition_ = COECondition(
        defaultName_, defaultCriterion_, defaultElement_, defaultFrame_, defaultTarget_, gravitationalParameter_
    );
    State defaultStateInIntegrationFrame_ = State::Undefined();
    State currentState_ = State::Undefined();
    State previousState_ = State::Undefined();
};

INSTANTIATE_TEST_SUITE_P(
    COECondition,
    OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition,
    ::testing::Values(
        std::make_tuple(COE::Element::SemiMajorAxis, 7000000.0, -95242.10899386555),
        std::make_tuple(COE::Element::Eccentricity, 0.0, 0.0010116019825255468),
        std::make_tuple(COE::Element::Inclination, 1.5, getCrossZ(1.6989221681582849, 1.5)),
        std::make_tuple(COE::Element::Aop, 2.4, getCrossZ(2.4052654657377115, 2.4)),
        std::make_tuple(COE::Element::Raan, 4.8, getCrossZ(4.8172172435680096, 4.8)),
        std::make_tuple(COE::Element::TrueAnomaly, 3.8, getCrossZ(3.8846577046593076, 3.8)),
        std::make_tuple(COE::Element::MeanAnomaly, 3.8, getCrossZ(3.8860272646567751, 3.8)),
        std::make_tuple(COE::Element::EccentricAnomaly, 3.8, getCrossZ(3.8853423573058397, 3.8))
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Constructor)
{
    {
        EXPECT_NO_THROW(COECondition(
            defaultName_, defaultCriterion_, defaultElement_, defaultFrame_, defaultTarget_, gravitationalParameter_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Getters)
{
    {
        EXPECT_TRUE(defaultCondition_.getTarget() == defaultTarget_);
    }

    {
        EXPECT_TRUE(defaultCondition_.getGravitationalParameter() == gravitationalParameter_);
    }

    {
        EXPECT_TRUE(defaultCondition_.getFrame() == defaultFrame_);
    }

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
        defaultCriterion_,
        element,
        defaultFrame_,
        target,
        gravitationalParameter_,
    };

    EXPECT_NEAR(condition.evaluate(defaultStateInIntegrationFrame_), expectedValue, 1e-14);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, SemiMajorAxis)
{
    COECondition condition = COECondition::SemiMajorAxis(
        defaultCriterion_, defaultFrame_, Length::Meters(7000000.0), gravitationalParameter_
    );

    EXPECT_EQ(condition.getName(), "Semi-major axis Condition");
    EXPECT_EQ(condition.getCriterion(), defaultCriterion_);
    EXPECT_EQ(condition.getElement(), COE::Element::SemiMajorAxis);
    EXPECT_EQ(condition.getTarget(), 7000000.0);
    EXPECT_NEAR(condition.evaluate(defaultStateInIntegrationFrame_), 6904757.8910061345 - 7000000.0, 1e-14);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Eccentricity)
{
    COECondition condition =
        COECondition::Eccentricity(defaultCriterion_, defaultFrame_, 0.0005, gravitationalParameter_);

    EXPECT_EQ(condition.getName(), "Eccentricity Condition");
    EXPECT_EQ(condition.getCriterion(), defaultCriterion_);
    EXPECT_EQ(condition.getElement(), COE::Element::Eccentricity);
    EXPECT_EQ(condition.getTarget(), 0.0005);
    EXPECT_NEAR(condition.evaluate(defaultStateInIntegrationFrame_), 0.0010116019825255468 - 0.0005, 1e-14);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Inclination)
{
    {
        const Angle target = Angle::Degrees(90.0);
        const Real currentAngle = 1.6989221681582849;
        COECondition condition =
            COECondition::Inclination(defaultCriterion_, defaultFrame_, target, gravitationalParameter_);
        EXPECT_EQ(condition.getName(), "Inclination Condition");
        EXPECT_EQ(condition.getCriterion(), defaultCriterion_);
        EXPECT_EQ(condition.getElement(), COE::Element::Inclination);
        EXPECT_EQ(condition.getTarget(), target.inRadians());
        EXPECT_NEAR(
            condition.evaluate(defaultStateInIntegrationFrame_), getCrossZ(currentAngle, target.inRadians()), 1e-14
        );
    }

    {
        COECondition condition = COECondition::Inclination(
            COECondition::Criterion::PositiveCrossing, defaultFrame_, Angle::Degrees(15.5), gravitationalParameter_
        );
        EXPECT_LT(condition.evaluate(previousState_), 0.0);
        EXPECT_GT(condition.evaluate(currentState_), 0.0);
        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Aop)
{
    {
        const Angle target = Angle::Degrees(90.0);
        const Real currentAngle = 2.4052654657377115;
        COECondition condition = COECondition::Aop(defaultCriterion_, defaultFrame_, target, gravitationalParameter_);
        EXPECT_EQ(condition.getName(), "Argument of periapsis Condition");
        EXPECT_EQ(condition.getCriterion(), defaultCriterion_);
        EXPECT_EQ(condition.getElement(), COE::Element::Aop);
        EXPECT_EQ(condition.getTarget(), target.inRadians());
        EXPECT_NEAR(
            condition.evaluate(defaultStateInIntegrationFrame_), getCrossZ(currentAngle, target.inRadians()), 1e-14
        );
    }

    {
        COECondition condition = COECondition::Aop(
            COECondition::Criterion::PositiveCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
        );
        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Raan)
{
    {
        const Angle target = Angle::Degrees(90.0);
        const Real currentAngle = 4.8172172435680096;
        COECondition condition = COECondition::Raan(defaultCriterion_, defaultFrame_, target, gravitationalParameter_);
        EXPECT_EQ(condition.getName(), "Right angle of ascending node Condition");
        EXPECT_EQ(condition.getCriterion(), defaultCriterion_);
        EXPECT_EQ(condition.getElement(), COE::Element::Raan);
        EXPECT_EQ(condition.getTarget(), target.inRadians());
        EXPECT_NEAR(
            condition.evaluate(defaultStateInIntegrationFrame_), getCrossZ(currentAngle, target.inRadians()), 1e-14
        );
    }

    {
        COECondition condition = COECondition::Raan(
            COECondition::Criterion::PositiveCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
        );
        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        Position currentPosition = Position::Undefined();
        Position previousPosition = Position::Undefined();
        Velocity currentVelocity = Velocity::Undefined();
        Velocity previousVelocity = Velocity::Undefined();

        std::tie(currentPosition, currentVelocity) =
            COE(Length::Kilometers(550.0) + Earth::EGM2008.equatorialRadius_,
                0.2,
                Angle::Degrees(16.0),
                Angle::Degrees(181.0),
                Angle::Degrees(1.0),
                Angle::Degrees(1.0))
                .getCartesianState(Earth::EGM2008.gravitationalParameter_, defaultFrame_);

        std::tie(previousPosition, previousVelocity) =
            COE(Length::Kilometers(550.0) + Earth::EGM2008.equatorialRadius_,
                0.2,
                Angle::Degrees(15.0),
                Angle::Degrees(179.0),
                Angle::Degrees(359.0),
                Angle::Degrees(359.0))
                .getCartesianState(Earth::EGM2008.gravitationalParameter_, defaultFrame_);

        const State currentState = {defaultInstant_, currentPosition, currentVelocity};
        const State previousState = {defaultInstant_, previousPosition, previousVelocity};

        {
            COECondition condition = COECondition::Raan(
                COECondition::Criterion::AnyCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
            );
            EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
            EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        }

        {
            COECondition condition = COECondition::Raan(
                COECondition::Criterion::PositiveCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
            );
            EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
            EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, TrueAnomaly)
{
    {
        const Angle target = Angle::Degrees(90.0);
        const Real currentAngle = 3.8846577046593076;
        COECondition condition =
            COECondition::TrueAnomaly(defaultCriterion_, defaultFrame_, target, gravitationalParameter_);
        EXPECT_EQ(condition.getName(), "True anomaly Condition");
        EXPECT_EQ(condition.getCriterion(), defaultCriterion_);
        EXPECT_EQ(condition.getElement(), COE::Element::TrueAnomaly);
        EXPECT_EQ(condition.getTarget(), target.inRadians());
        EXPECT_NEAR(
            condition.evaluate(defaultStateInIntegrationFrame_), getCrossZ(currentAngle, target.inRadians()), 1e-14
        );
    }

    {
        COECondition condition = COECondition::TrueAnomaly(
            COECondition::Criterion::PositiveCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
        );
        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, MeanAnomaly)
{
    {
        const Angle target = Angle::Degrees(90.0);
        const Real currentAngle = 3.8860272646567751;
        COECondition condition =
            COECondition::MeanAnomaly(defaultCriterion_, defaultFrame_, target, gravitationalParameter_);
        EXPECT_EQ(condition.getName(), "Mean anomaly Condition");
        EXPECT_EQ(condition.getCriterion(), defaultCriterion_);
        EXPECT_EQ(condition.getElement(), COE::Element::MeanAnomaly);
        EXPECT_EQ(condition.getTarget(), target.inRadians());
        EXPECT_NEAR(
            condition.evaluate(defaultStateInIntegrationFrame_), getCrossZ(currentAngle, target.inRadians()), 1e-14
        );
    }

    {
        COECondition condition = COECondition::MeanAnomaly(
            COECondition::Criterion::PositiveCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
        );
        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, EccentricAnomaly)
{
    {
        const Angle target = Angle::Degrees(90.0);
        const Real currentAngle = 3.8853423573058397;
        COECondition condition =
            COECondition::EccentricAnomaly(defaultCriterion_, defaultFrame_, target, gravitationalParameter_);
        EXPECT_EQ(condition.getName(), "Eccentric anomaly Condition");
        EXPECT_EQ(condition.getCriterion(), defaultCriterion_);
        EXPECT_EQ(condition.getElement(), COE::Element::EccentricAnomaly);
        EXPECT_EQ(condition.getTarget(), target.inRadians());
        EXPECT_NEAR(
            condition.evaluate(defaultStateInIntegrationFrame_), getCrossZ(currentAngle, target.inRadians()), 1e-14
        );
    }

    {
        COECondition condition = COECondition::EccentricAnomaly(
            COECondition::Criterion::PositiveCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
        );
        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        COECondition condition = COECondition::EccentricAnomaly(
            COECondition::Criterion::NegativeCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
        );
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        COECondition condition = COECondition::EccentricAnomaly(
            COECondition::Criterion::AnyCrossing, defaultFrame_, Angle::Degrees(0.0), gravitationalParameter_
        );
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    }
}
