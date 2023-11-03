/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster/GuidanceLaw/QLaw.hpp>

#include <Global.test.hpp>

using ostk::core::types::Size;
using ostk::core::types::Real;

using ostk::math::obj::Vector3d;

using ostk::physics::units::Derived;
using ostk::physics::units::Angle;
using ostk::physics::units::Length;
using ostk::physics::time::Instant;
using ostk::physics::coord::Frame;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astro::dynamics::thruster::guidancelaw::QLaw;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::solvers::FiniteDifferenceSolver;

class OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw : public ::testing::Test
{
    void SetUp() override
    {
        const COE currentCOE = {
            Length::Meters(7000.0e3),
            0.01,
            Angle::Degrees(0.05),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
        };

        const COE::CartesianState cartesianState = currentCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());

        initialState_ = {
            Instant::J2000(),
            cartesianState.first,
            cartesianState.second,
        };
    }

   protected:
    const QLaw::Parameters parameters_ = {
        {
            {COE::Element::SemiMajorAxis, 1.0},
            {COE::Element::Eccentricity, 1.0},
        },
    };

    const COE targetCOE_ = {
        Length::Meters(42000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const Derived gravitationalParameter_ =
        Derived(398600.49 * 1e9, EarthGravitationalModel::EGM2008.gravitationalParameter_.getUnit());

    const Real thrustAcceleration_ = 1.0 / 300.0;

    const FiniteDifferenceSolver finiteDifferenceSolver_ =
        FiniteDifferenceSolver(FiniteDifferenceSolver::Type::Central);

    const QLaw qlaw_ = {
        targetCOE_,
        gravitationalParameter_,
        parameters_,
        finiteDifferenceSolver_,
    };

    State initialState_ = State::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, Constructor)
{
    EXPECT_NO_THROW(QLaw qlaw(targetCOE_, gravitationalParameter_, parameters_, finiteDifferenceSolver_));

    EXPECT_THROW(
        QLaw qlaw(targetCOE_, gravitationalParameter_, {{}}, finiteDifferenceSolver_), ostk::core::error::RuntimeError
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, ComputeAcceleration)
{
    // Self-validated from the python code
    {
        const Vector3d acceleration = qlaw_.computeAcceleration(
            initialState_.accessInstant(),
            initialState_.getPosition().getCoordinates(),
            initialState_.getVelocity().getCoordinates(),
            thrustAcceleration_,
            Frame::GCRF()
        );

        const Vector3d accelerationExpected = {0.0, 0.0033333320640941645, 2.9088817174504986e-06};

        EXPECT_TRUE(acceleration.isNear(accelerationExpected, 1e-12));
    }
}
