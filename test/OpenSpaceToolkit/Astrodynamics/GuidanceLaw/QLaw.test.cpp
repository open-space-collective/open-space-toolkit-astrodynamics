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
using ostk::math::obj::Vector6d;
using ostk::math::obj::VectorXd;
using Vector5d = Eigen::Matrix<double, 5, 1>;
using Matrix5d = Eigen::Matrix<double, 5, 5>;
using Matrix53d = Eigen::Matrix<double, 5, 3>;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, ComputeOrbitalElementsMaximalChange)
{
    {
        const COE currentCOE = {
            Length::Meters(24505.9e3),
            0.725,
            Angle::Degrees(0.06),
            Angle::Degrees(3.0),
            Angle::Degrees(5.0),
            Angle::Degrees(0.0),
        };

        const Vector5d currentCOEVector = currentCOE.getSIVector(COE::AnomalyType::True).segment(0, 5);
        const Real thrustAcceleration = 2.0 / 2000.0;

        const COE targetCOE = {
            Length::Meters(26500.0e3),
            0.7,
            Angle::Degrees(116.0),
            Angle::Degrees(180.0),
            Angle::Degrees(270.0),
            Angle::Degrees(0.0),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, 1.0},
                {COE::Element::Eccentricity, 1.0},
                {COE::Element::Inclination, 1.0},
                {COE::Element::Raan, 1.0},
                {COE::Element::Aop, 1.0},
            },
        };

        const QLaw qlaw = {
            targetCOE,
            gravitationalParameter_,
            parameters,
            finiteDifferenceSolver_,
        };

        const Vector5d orbitalElements_xx =
            qlaw.computeOrbitalElementsMaximalChange(currentCOEVector, thrustAcceleration);

        const Vector5d expectedOrbitalElements_xx = {
            30.4365392373584,
            3.41552372705085E-7,
            6.19291711265385E-7,
            0.000259493025307140,
            0.00000310465071929705,
        };

        for (Size i = 0; i < 5; ++i)
        {
            EXPECT_NEAR(orbitalElements_xx(i), expectedOrbitalElements_xx(i), 1e-12);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, ComputeQ)
{
    {
        const COE currentCOE = {
            Length::Meters(24505.9e3),
            0.725,
            Angle::Degrees(0.06),
            Angle::Degrees(3.0),
            Angle::Degrees(5.0),
            Angle::Degrees(0.0),
        };

        const Vector5d currentCOEVector = currentCOE.getSIVector(COE::AnomalyType::True).segment(0, 5);
        const Real thrustAcceleration = 2.0 / 2000.0;

        const COE targetCOE = {
            Length::Meters(26500.0e3),
            0.7,
            Angle::Degrees(116.0),
            Angle::Degrees(180.0),
            Angle::Degrees(270.0),
            Angle::Degrees(0.0),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, 1.0},
                {COE::Element::Eccentricity, 1.0},
                {COE::Element::Inclination, 1.0},
                {COE::Element::Raan, 1.0},
                {COE::Element::Aop, 1.0},
            },
        };

        const QLaw qlaw = {
            targetCOE,
            gravitationalParameter_,
            parameters,
            finiteDifferenceSolver_,
        };

        const Real Q = qlaw.computeQ(currentCOEVector, thrustAcceleration);

        const Real expectedQ = 10971555799477.6;

        EXPECT_NEAR(Q, expectedQ, 1e3);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, Compute_dOE_dF)
{
    {
        const COE currentCOE = {
            Length::Meters(24505.9e3),
            0.725,
            Angle::Degrees(0.06),
            Angle::Degrees(3.0),
            Angle::Degrees(5.0),
            Angle::Degrees(0.0),
        };

        const Vector6d currentCOEVector = currentCOE.getSIVector(COE::AnomalyType::True);

        const Matrix53d dOE_dF = QLaw::Compute_dOE_dF(currentCOEVector, gravitationalParameter_);

        Matrix53d expected_dOE_dF = Matrix53d::Zero();
        expected_dOE_dF(0, 0) = 30436.5392373584;
        expected_dOE_dF(1, 0) = 0.000341552372705085;
        expected_dOE_dF(2, 2) = 9.86239602346264e-5;
        expected_dOE_dF(3, 2) = 0.00823959150786049;
        expected_dOE_dF(4, 1) = -0.000235553360486265;
        expected_dOE_dF(4, 2) = -0.00823958698999931;

        for (Size i = 0; i < 5; ++i)
        {
            for (Size j = 0; j < 3; ++j)
            {
                EXPECT_NEAR(dOE_dF(i, j), expected_dOE_dF(i, j), 1e-10);
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, Compute_dQ_dOE)
{
    {
        const COE currentCOE = {
            Length::Meters(24505.9e3),
            0.725,
            Angle::Degrees(0.06),
            Angle::Degrees(3.0),
            Angle::Degrees(5.0),
            Angle::Degrees(0.0),
        };

        const Vector5d currentCOEVector = currentCOE.getSIVector(COE::AnomalyType::True).segment(0, 5);
        const Real thrustAcceleration = 2.0 / 2000.0;

        const COE targetCOE = {
            Length::Meters(26500.0e3),
            0.7,
            Angle::Degrees(116.0),
            Angle::Degrees(180.0),
            Angle::Degrees(270.0),
            Angle::Degrees(0.0),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, 1.0},
                {COE::Element::Eccentricity, 1.0},
                {COE::Element::Inclination, 1.0},
                {COE::Element::Raan, 1.0},
                {COE::Element::Aop, 1.0},
            },
        };

        const QLaw qlaw = {
            targetCOE,
            gravitationalParameter_,
            parameters,
            finiteDifferenceSolver_,
        };

        const Vector5d dQ_dOE = qlaw.compute_dQ_dOE(currentCOEVector, thrustAcceleration);

        // analytically calculated using sympy
        const Vector5d expected_dQ_dOE = {
            -452366.239974152,
            -44406901695950.7,
            440503322527533,
            -91754896.7814982,
            1202754640828.99,
        };

        for (Size i = 0; i < 5; ++i)
        {
            const double relativeError = std::abs((dQ_dOE(i) - expected_dQ_dOE(i)) / expected_dQ_dOE(i));
            EXPECT_LT(relativeError, 1e-5);
        }
    }
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

    // Self-validated from the python code
    {
        const COE targetCOE = {
            Length::Meters(26500.0e3),
            0.7,
            Angle::Degrees(116.0),
            Angle::Degrees(180.0),
            Angle::Degrees(270.0),
            Angle::Degrees(0.0),
        };

        const QLaw::Parameters parameters = {
            {
                {COE::Element::SemiMajorAxis, 1.0},
                {COE::Element::Eccentricity, 1.0},
                {COE::Element::Inclination, 1.0},
                {COE::Element::Raan, 1.0},
                {COE::Element::Aop, 1.0},
            },
        };

        const QLaw qlaw = {
            targetCOE,
            gravitationalParameter_,
            parameters,
            finiteDifferenceSolver_,
        };

        const COE currentCOE = {
            Length::Meters(24505.9e3),
            0.725,
            Angle::Degrees(0.06),
            Angle::Degrees(3.0),
            Angle::Degrees(5.0),
            Angle::Degrees(0.0),
        };

        const COE::CartesianState cartesianState = currentCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());

        VectorXd coordinates(7);
        coordinates << cartesianState.first.getCoordinates(), cartesianState.second.getCoordinates(), 2000.0;

        const State initialState = {
            Instant::J2000(),
            coordinates,
            Frame::GCRF(),
            {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinatesSubset::Mass()}
        };

        const Vector3d acceleration = qlaw.calculateThrustAccelerationAt(
            initialState.accessInstant(),
            initialState.getPosition().getCoordinates(),
            initialState.getVelocity().getCoordinates(),
            2.0 / 2000.0,
            Frame::GCRF()
        );

        const Vector3d accelerationExpected = {0.0, 0.0033333320640941645, 2.9088817174504986e-06};

        EXPECT_TRUE(acceleration.isNear(accelerationExpected, 1e-12));
    }
}
