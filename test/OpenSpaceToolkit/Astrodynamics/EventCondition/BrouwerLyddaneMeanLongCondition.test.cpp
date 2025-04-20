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

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/BrouwerLyddaneMeanLongCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::EventCondition;
using ostk::astrodynamics::eventcondition::AngularCondition;
using ostk::astrodynamics::eventcondition::BrouwerLyddaneMeanLongCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition
    : public ::testing::TestWithParam<Tuple<COE::Element, Real, Real>>
{
    void SetUp() override
    {
        Position currentPosition = Position::Undefined();
        Position previousPosition = Position::Undefined();
        Velocity currentVelocity = Velocity::Undefined();
        Velocity previousVelocity = Velocity::Undefined();

        std::tie(currentPosition, currentVelocity) =
            BrouwerLyddaneMeanLong(
                Length::Kilometers(551.0) + Earth::EGM2008.equatorialRadius_,
                0.00021,
                Angle::Degrees(16.0),
                Angle::Degrees(1.0),
                Angle::Degrees(1.0),
                Angle::Degrees(1.0)
            )
                .getCartesianState(Earth::EGM2008.gravitationalParameter_, defaultFrame_);

        std::tie(previousPosition, previousVelocity) =
            BrouwerLyddaneMeanLong(
                Length::Kilometers(549.0) + Earth::EGM2008.equatorialRadius_,
                0.00019,
                Angle::Degrees(15.0),
                Angle::Degrees(359.0),
                Angle::Degrees(359.0),
                Angle::Degrees(359.0)
            )
                .getCartesianState(Earth::EGM2008.gravitationalParameter_, defaultFrame_);

        currentState_ = {defaultInstant_, currentPosition, currentVelocity};
        previousState_ = {defaultInstant_, previousPosition, previousVelocity};
    }

   protected:
    const Derived gravitationalParameter_ = Earth::Spherical.gravitationalParameter_;
    const Instant defaultInstant_ = Instant::J2000();
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    State currentState_ = State::Undefined();
    State previousState_ = State::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition, SemiMajorAxis)
{
    RealCondition condition = BrouwerLyddaneMeanLongCondition::SemiMajorAxis(
        RealCondition::Criterion::PositiveCrossing,
        defaultFrame_,
        Length::Meters(550000.0) + Earth::EGM2008.equatorialRadius_,
        gravitationalParameter_
    );

    EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition, Eccentricity)
{
    const Real target = 0.0002;

    {
        RealCondition condition = BrouwerLyddaneMeanLongCondition::Eccentricity(
            RealCondition::Criterion::PositiveCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        RealCondition condition = BrouwerLyddaneMeanLongCondition::Eccentricity(
            RealCondition::Criterion::NegativeCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        RealCondition condition = BrouwerLyddaneMeanLongCondition::Eccentricity(
            RealCondition::Criterion::AnyCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition, Inclination)
{
    const Angle targetAngle = Angle::Degrees(15.5);
    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Inclination(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Inclination(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Inclination(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's inclination (16 deg) but not the previous state's (15 deg)
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(15.5), Angle::Degrees(17.0)};

        AngularCondition condition =
            BrouwerLyddaneMeanLongCondition::Inclination(defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition, Aop)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Aop(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Aop(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Aop(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's AoP (1 deg) but not the previous state's (359 deg)
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            BrouwerLyddaneMeanLongCondition::Aop(defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition, Raan)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Raan(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Raan(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::Raan(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's RAAN (1 deg) but not the previous state's (359 deg)
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            BrouwerLyddaneMeanLongCondition::Raan(defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition, TrueAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::TrueAnomaly(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::TrueAnomaly(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::TrueAnomaly(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's true anomaly (1 deg) but not the previous state's (359 deg)
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            BrouwerLyddaneMeanLongCondition::TrueAnomaly(defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition, MeanAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::MeanAnomaly(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::MeanAnomaly(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::MeanAnomaly(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's mean anomaly (approximately 1 deg) but not the previous state's
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            BrouwerLyddaneMeanLongCondition::MeanAnomaly(defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_EventCondition_BrouwerLyddaneMeanLongCondition, EccentricAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::EccentricAnomaly(
            AngularCondition::Criterion::PositiveCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::EccentricAnomaly(
            AngularCondition::Criterion::NegativeCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = BrouwerLyddaneMeanLongCondition::EccentricAnomaly(
            AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's eccentric anomaly (approximately 1 deg) but not the previous state's
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            BrouwerLyddaneMeanLongCondition::EccentricAnomaly(defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}
