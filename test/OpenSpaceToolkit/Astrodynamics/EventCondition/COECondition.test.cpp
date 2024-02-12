/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/COECondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Shared;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Length;
using ostk::physics::unit::Derived;

using ostk::astro::EventCondition;
using ostk::astro::eventcondition::COECondition;
using ostk::astro::eventcondition::RealCondition;
using ostk::astro::eventcondition::AngularCondition;
using ostk::astro::trajectory::orbit::model::kepler::COE;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition
    : public ::testing::TestWithParam<Tuple<COE::Element, Real, Real>>
{
    void SetUp() override
    {
        Position currentPosition = Position::Undefined();
        Position previousPosition = Position::Undefined();
        Velocity currentVelocity = Velocity::Undefined();
        Velocity previousVelocity = Velocity::Undefined();

        std::tie(currentPosition, currentVelocity) =
            COE(Length::Kilometers(551.0) + Earth::EGM2008.equatorialRadius_,
                0.00021,
                Angle::Degrees(16.0),
                Angle::Degrees(1.0),
                Angle::Degrees(1.0),
                Angle::Degrees(1.0))
                .getCartesianState(Earth::EGM2008.gravitationalParameter_, defaultFrame_);

        std::tie(previousPosition, previousVelocity) =
            COE(Length::Kilometers(549.0) + Earth::EGM2008.equatorialRadius_,
                0.00019,
                Angle::Degrees(15.0),
                Angle::Degrees(359.0),
                Angle::Degrees(359.0),
                Angle::Degrees(359.0))
                .getCartesianState(Earth::EGM2008.gravitationalParameter_, defaultFrame_);

        currentState_ = {defaultInstant_, currentPosition, currentVelocity};
        previousState_ = {defaultInstant_, previousPosition, previousVelocity};
    }

   protected:
    const Derived gravitationalParameter_ = Earth::Spherical.gravitationalParameter_;
    const Instant defaultInstant_ = Instant::J2000();
    const Shared<const Frame> defaultFrame_ = Frame::TEME();
    State currentState_ = State::Undefined();
    State previousState_ = State::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, SemiMajorAxis)
{
    RealCondition condition = COECondition::SemiMajorAxis(
        RealCondition::Criterion::PositiveCrossing,
        defaultFrame_,
        Length::Meters(550000.0) + Earth::EGM2008.equatorialRadius_,
        gravitationalParameter_
    );

    EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Eccentricity)
{
    const Real target = 0.0002;

    {
        RealCondition condition = COECondition::Eccentricity(
            RealCondition::Criterion::PositiveCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        RealCondition condition = COECondition::Eccentricity(
            RealCondition::Criterion::NegativeCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        RealCondition condition = COECondition::Eccentricity(
            RealCondition::Criterion::AnyCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Inclination)
{
    const Angle targetAngle = Angle::Degrees(15.5);
    {
        AngularCondition condition = COECondition::Inclination(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = COECondition::Inclination(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = COECondition::Inclination(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Aop)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = COECondition::Aop(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = COECondition::Aop(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = COECondition::Aop(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, Raan)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = COECondition::Raan(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = COECondition::Raan(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = COECondition::Raan(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, TrueAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = COECondition::TrueAnomaly(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = COECondition::TrueAnomaly(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = COECondition::TrueAnomaly(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, MeanAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = COECondition::MeanAnomaly(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = COECondition::MeanAnomaly(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = COECondition::MeanAnomaly(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_COECondition, EccentricAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = COECondition::EccentricAnomaly(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = COECondition::EccentricAnomaly(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = COECondition::EccentricAnomaly(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}
