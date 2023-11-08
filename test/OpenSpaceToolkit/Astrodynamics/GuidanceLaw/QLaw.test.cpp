/// Apache License 2.0

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::types::Size;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::ctnr::Array;

using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;
using ostk::math::obj::VectorXd;
using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;

using ostk::physics::data::Scalar;
using ostk::physics::units::Derived;
using ostk::physics::units::Angle;
using ostk::physics::units::Length;
using ostk::physics::units::Mass;
using ostk::physics::time::Instant;
using ostk::physics::time::Duration;
using ostk::physics::coord::Frame;
using ostk::physics::env::obj::celest::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astro::guidancelaw::QLaw;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::orbit::models::kepler::COE;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::solvers::FiniteDifferenceSolver;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::Dynamics;
using ostk::astro::dynamics::CentralBodyGravity;
using ostk::astro::dynamics::Thruster;
using ostk::astro::dynamics::PositionDerivative;

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

    const FiniteDifferenceSolver finiteDifferenceSolver_ = FiniteDifferenceSolver::Default();

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, GetParameters)
{
    EXPECT_NO_THROW(qlaw_.getParameters());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, GetTargetCOE)
{
    EXPECT_EQ(qlaw_.getTargetCOE(), targetCOE_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, CalculateThrustAccelerationAt)
{
    // Self-validated from the python code
    {
        const Vector3d acceleration = qlaw_.calculateThrustAccelerationAt(
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
