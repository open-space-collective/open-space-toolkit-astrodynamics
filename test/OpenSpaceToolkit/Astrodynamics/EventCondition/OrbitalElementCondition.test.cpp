/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/EventCondition/OrbitalElementCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanShort.hpp>
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
using ostk::astrodynamics::eventcondition::OrbitalElementCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanShort;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition
    : public ::testing::TestWithParam<OrbitalElementCondition::Theory>
{
   public:
    static COE::CartesianState ComputeCartesianState(
        const OrbitalElementCondition::Theory& aTheory,
        const Length& aSemiMajorAxis,
        const Real& anEccentricity,
        const Angle& anInclination,
        const Angle& aRaan,
        const Angle& anAop,
        const Angle& anAnomaly
    )
    {
        switch (aTheory)
        {
            case OrbitalElementCondition::Theory::Osculating:
                return COE(aSemiMajorAxis, anEccentricity, anInclination, aRaan, anAop, anAnomaly)
                    .getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());
            case OrbitalElementCondition::Theory::BrouwerLyddaneMeanShort:
                return BrouwerLyddaneMeanShort(aSemiMajorAxis, anEccentricity, anInclination, aRaan, anAop, anAnomaly)
                    .getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());
            case OrbitalElementCondition::Theory::BrouwerLyddaneMeanLong:
                return BrouwerLyddaneMeanLong(aSemiMajorAxis, anEccentricity, anInclination, aRaan, anAop, anAnomaly)
                    .getCartesianState(Earth::EGM2008.gravitationalParameter_, Frame::GCRF());
            default:
                throw ostk::core::error::runtime::Wrong("Theory");
        }
    }

   protected:
    void SetUp() override
    {
        const OrbitalElementCondition::Theory theory = GetParam();

        // Brouwer-Lyddane mean theories interpret the last angle as a mean anomaly, so the previous state is built
        // one degree further along than for the osculating (true anomaly based) theory, to preserve the intended
        // crossing behavior across all three theories.
        const Angle previousAngle =
            (theory == OrbitalElementCondition::Theory::Osculating) ? Angle::Degrees(358.0) : Angle::Degrees(359.0);

        Position currentPosition = Position::Undefined();
        Position previousPosition = Position::Undefined();
        Velocity currentVelocity = Velocity::Undefined();
        Velocity previousVelocity = Velocity::Undefined();

        std::tie(currentPosition, currentVelocity) = ComputeCartesianState(
            theory,
            Length::Kilometers(551.0) + Earth::EGM2008.equatorialRadius_,
            0.00021,
            Angle::Degrees(16.0),
            Angle::Degrees(1.0),
            Angle::Degrees(1.0),
            Angle::Degrees(1.0)
        );

        std::tie(previousPosition, previousVelocity) = ComputeCartesianState(
            theory,
            Length::Kilometers(549.0) + Earth::EGM2008.equatorialRadius_,
            0.00019,
            Angle::Degrees(15.0),
            previousAngle,
            previousAngle,
            previousAngle
        );

        currentState_ = {defaultInstant_, currentPosition, currentVelocity};
        previousState_ = {defaultInstant_, previousPosition, previousVelocity};
    }

    const Derived gravitationalParameter_ = Earth::Spherical.gravitationalParameter_;
    const Instant defaultInstant_ = Instant::J2000();
    const Shared<const Frame> defaultFrame_ = Frame::GCRF();
    State currentState_ = State::Undefined();
    State previousState_ = State::Undefined();
};

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, SemiMajorAxis)
{
    RealCondition condition = OrbitalElementCondition::SemiMajorAxis(
        GetParam(),
        RealCondition::Criterion::PositiveCrossing,
        defaultFrame_,
        Length::Meters(550000.0) + Earth::EGM2008.equatorialRadius_,
        gravitationalParameter_
    );

    EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, Eccentricity)
{
    const Real target = 0.0002;

    {
        RealCondition condition = OrbitalElementCondition::Eccentricity(
            GetParam(), RealCondition::Criterion::PositiveCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        RealCondition condition = OrbitalElementCondition::Eccentricity(
            GetParam(), RealCondition::Criterion::NegativeCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        RealCondition condition = OrbitalElementCondition::Eccentricity(
            GetParam(), RealCondition::Criterion::AnyCrossing, defaultFrame_, target, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, Inclination)
{
    const Angle targetAngle = Angle::Degrees(15.5);
    {
        AngularCondition condition = OrbitalElementCondition::Inclination(
            GetParam(),
            AngularCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::Inclination(
            GetParam(),
            AngularCondition::Criterion::NegativeCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::Inclination(
            GetParam(), AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's inclination (16 deg) but not the previous state's (15 deg)
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(15.5), Angle::Degrees(17.0)};

        AngularCondition condition =
            OrbitalElementCondition::Inclination(GetParam(), defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, Aop)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = OrbitalElementCondition::Aop(
            GetParam(),
            AngularCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::Aop(
            GetParam(),
            AngularCondition::Criterion::NegativeCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::Aop(
            GetParam(), AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's AoP (1 deg) but not the previous state's (358/359 deg)
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            OrbitalElementCondition::Aop(GetParam(), defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, Raan)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = OrbitalElementCondition::Raan(
            GetParam(),
            AngularCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::Raan(
            GetParam(),
            AngularCondition::Criterion::NegativeCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::Raan(
            GetParam(), AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's RAAN (1 deg) but not the previous state's (358/359 deg)
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            OrbitalElementCondition::Raan(GetParam(), defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, TrueAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = OrbitalElementCondition::TrueAnomaly(
            GetParam(),
            AngularCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::TrueAnomaly(
            GetParam(),
            AngularCondition::Criterion::NegativeCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::TrueAnomaly(
            GetParam(), AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's true anomaly (approximately 1 deg) but not the previous state's
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            OrbitalElementCondition::TrueAnomaly(GetParam(), defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, MeanAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = OrbitalElementCondition::MeanAnomaly(
            GetParam(),
            AngularCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::MeanAnomaly(
            GetParam(),
            AngularCondition::Criterion::NegativeCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::MeanAnomaly(
            GetParam(), AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's mean anomaly (approximately 1 deg) but not the previous state's
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            OrbitalElementCondition::MeanAnomaly(GetParam(), defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, EccentricAnomaly)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = OrbitalElementCondition::EccentricAnomaly(
            GetParam(),
            AngularCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::EccentricAnomaly(
            GetParam(),
            AngularCondition::Criterion::NegativeCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::EccentricAnomaly(
            GetParam(), AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's eccentric anomaly (approximately 1 deg) but not the previous
        // state's
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition =
            OrbitalElementCondition::EccentricAnomaly(GetParam(), defaultFrame_, targetRange, gravitationalParameter_);

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, ArgumentOfLatitude)
{
    const Angle targetAngle = Angle::Degrees(0.0);

    {
        AngularCondition condition = OrbitalElementCondition::ArgumentOfLatitude(
            GetParam(),
            AngularCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::ArgumentOfLatitude(
            GetParam(),
            AngularCondition::Criterion::NegativeCrossing,
            defaultFrame_,
            targetAngle,
            gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
        EXPECT_FALSE(condition.isSatisfied(currentState_, previousState_));
    }

    {
        AngularCondition condition = OrbitalElementCondition::ArgumentOfLatitude(
            GetParam(), AngularCondition::Criterion::AnyCrossing, defaultFrame_, targetAngle, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_TRUE(condition.isSatisfied(previousState_, currentState_));
    }

    {
        // Range that includes the current state's argument of latitude (approximately 1 deg) but not the previous
        // state's
        const Pair<Angle, Angle> targetRange = {Angle::Degrees(0.5), Angle::Degrees(10.0)};

        AngularCondition condition = OrbitalElementCondition::ArgumentOfLatitude(
            GetParam(), defaultFrame_, targetRange, gravitationalParameter_
        );

        EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
        EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
    }
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, ConstructReal)
{
    RealCondition condition = OrbitalElementCondition::Construct(
        GetParam(),
        COE::Element::SemiMajorAxis,
        RealCondition::Criterion::PositiveCrossing,
        defaultFrame_,
        Length::Meters(550000.0) + Earth::EGM2008.equatorialRadius_,
        gravitationalParameter_
    );

    EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, ConstructAngular)
{
    const Angle targetAngle = Angle::Degrees(15.5);

    AngularCondition condition = OrbitalElementCondition::Construct(
        GetParam(),
        COE::Element::Inclination,
        AngularCondition::Criterion::PositiveCrossing,
        defaultFrame_,
        targetAngle,
        gravitationalParameter_
    );

    EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, ConstructAngularWithinRange)
{
    // Range that includes the current state's inclination (16 deg) but not the previous state's (15 deg)
    const Pair<Angle, Angle> targetRange = {Angle::Degrees(15.5), Angle::Degrees(17.0)};

    AngularCondition condition = OrbitalElementCondition::Construct(
        GetParam(), COE::Element::Inclination, defaultFrame_, targetRange, gravitationalParameter_
    );

    EXPECT_TRUE(condition.isSatisfied(currentState_, previousState_));
    EXPECT_FALSE(condition.isSatisfied(previousState_, currentState_));
}

TEST_P(OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition, ConstructThrowsForMismatchedElement)
{
    EXPECT_THROW(
        OrbitalElementCondition::Construct(
            GetParam(),
            COE::Element::Inclination,
            RealCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            Angle::Degrees(15.5),
            gravitationalParameter_
        ),
        ostk::core::error::RuntimeError
    );

    EXPECT_THROW(
        OrbitalElementCondition::Construct(
            GetParam(),
            COE::Element::SemiMajorAxis,
            AngularCondition::Criterion::PositiveCrossing,
            defaultFrame_,
            Length::Meters(550000.0) + Earth::EGM2008.equatorialRadius_,
            gravitationalParameter_
        ),
        ostk::core::error::RuntimeError
    );

    EXPECT_THROW(
        OrbitalElementCondition::Construct(
            GetParam(),
            COE::Element::Eccentricity,
            defaultFrame_,
            Pair<Angle, Angle> {Angle::Degrees(0.0), Angle::Degrees(10.0)},
            gravitationalParameter_
        ),
        ostk::core::error::RuntimeError
    );
}

INSTANTIATE_TEST_SUITE_P(
    Theories,
    OpenSpaceToolkit_Astrodynamics_EventCondition_OrbitalElementCondition,
    ::testing::Values(
        OrbitalElementCondition::Theory::Osculating,
        OrbitalElementCondition::Theory::BrouwerLyddaneMeanShort,
        OrbitalElementCondition::Theory::BrouwerLyddaneMeanLong
    )
);
