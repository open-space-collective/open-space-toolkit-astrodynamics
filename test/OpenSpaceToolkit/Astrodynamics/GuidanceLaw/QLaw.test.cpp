/// Apache License 2.0

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/QLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;
using ostk::mathematics::object::VectorXd;
using Vector5d = Eigen::Matrix<double, 5, 1>;
using Matrix5d = Eigen::Matrix<double, 5, 5>;
using Matrix53d = Eigen::Matrix<double, 5, 3>;
using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::geometry::d3::object::Point;

using ostk::physics::coordinate::Frame;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::QLaw;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

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
    Tuple<QLaw, Vector6d, Real> getQLawFullTargeting(const QLaw::GradientStrategy& aGradientStrategy) const
    {
        const Vector6d currentCOEVector = {
            24505900,
            0.725,
            0.001047197551196598,
            0.05235987755982989,
            0.08726646259971647,
            0.0,
        };
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
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-3}},
                {COE::Element::Inclination, {1.0, 1e-4}},
                {COE::Element::Raan, {1.0, 1e-4}},
                {COE::Element::Aop, {1.0, 1e-4}},
            },
            3,
            4,
            2,
            0.01,
            100,
            1.0,
            Length::Kilometers(6578.0),
        };

        QLaw qlaw = {
            targetCOE,
            gravitationalParameter_,
            parameters,
            QLaw::COEDomain::Osculating,
            aGradientStrategy,
        };

        return std::make_tuple(qlaw, currentCOEVector, thrustAcceleration);
    }

    const QLaw::Parameters parameters_ = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
            {COE::Element::Eccentricity, {1.0, 1e-3}},
        },
        3,
        4,
        2,
        0.01,
        100,
        1.0,
        Length::Kilometers(6578.0),
    };

    const COE targetCOE_ = {
        Length::Meters(42000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const Derived gravitationalParameter_ = Derived(398600.49 * 1e9, Derived::Unit::MeterCubedPerSecondSquared());

    const Real thrustAcceleration_ = 1.0 / 300.0;

    const QLaw::GradientStrategy gradientStrategy_ = QLaw::GradientStrategy::FiniteDifference;

    const QLaw qlaw_ = {
        targetCOE_,
        gravitationalParameter_,
        parameters_,
        QLaw::COEDomain::Osculating,
        gradientStrategy_,
    };

    State initialState_ = State::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, Constructor)
{
    EXPECT_NO_THROW(QLaw qlaw(targetCOE_, gravitationalParameter_, parameters_, QLaw::COEDomain::Osculating));

    EXPECT_NO_THROW(QLaw qlaw(
        targetCOE_, gravitationalParameter_, parameters_, QLaw::COEDomain::BrouwerLyddaneMeanLong, gradientStrategy_
    ));

    EXPECT_THROW(
        QLaw qlaw(targetCOE_, gravitationalParameter_, {{}}, QLaw::COEDomain::Osculating, gradientStrategy_),
        ostk::core::error::RuntimeError
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, GetGradientStrategy)
{
    EXPECT_EQ(qlaw_.getGradientStrategy(), gradientStrategy_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, GetCOEDomain)
{
    // Default should be Osculating
    EXPECT_EQ(qlaw_.getCOEDomain(), QLaw::COEDomain::Osculating);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, ComputeOrbitalElementsMaximalChange)
{
    {
        const Tuple<QLaw, Vector6d, Real> parameters = getQLawFullTargeting(QLaw::GradientStrategy::FiniteDifference);
        const QLaw qlaw = std::get<0>(parameters);
        const Vector6d currentCOEVector = std::get<1>(parameters);
        const Real thrustAcceleration = std::get<2>(parameters);

        const Vector5d orbitalElements_xx =
            qlaw.computeOrbitalElementsMaximalChange(currentCOEVector.segment(0, 5), thrustAcceleration);

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
        const Tuple<QLaw, Vector6d, Real> parameters = getQLawFullTargeting(QLaw::GradientStrategy::FiniteDifference);
        const QLaw qlaw = std::get<0>(parameters);
        const Vector6d currentCOEVector = std::get<1>(parameters);
        const Real thrustAcceleration = std::get<2>(parameters);

        const Real Q = qlaw.computeQ(currentCOEVector.segment(0, 5), thrustAcceleration);

        const Real expectedQ = 11918884993553.0;

        EXPECT_LT(std::abs(Q - expectedQ), 50.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, Compute_dOE_dF)
{
    {
        const Tuple<QLaw, Vector6d, Real> parameters = getQLawFullTargeting(QLaw::GradientStrategy::FiniteDifference);
        const QLaw qlaw = std::get<0>(parameters);
        const Vector6d currentCOEVector = std::get<1>(parameters);

        const Matrix53d dOE_dF = QLaw::Compute_dOE_dF(currentCOEVector, gravitationalParameter_);

        Matrix53d expected_dOE_dF = Matrix53d::Zero();
        expected_dOE_dF(0, 0) = 30436.5392373584;
        expected_dOE_dF(1, 0) = 0.000341552372705085;
        expected_dOE_dF(2, 2) = 9.86239602346264e-5;
        expected_dOE_dF(3, 2) = 0.00823959150786049;
        expected_dOE_dF(4, 1) = -0.000235553360486265;
        expected_dOE_dF(4, 2) = -0.00823958698971347;

        for (Size i = 0; i < 5; ++i)
        {
            for (Size j = 0; j < 3; ++j)
            {
                EXPECT_NEAR(dOE_dF(i, j), expected_dOE_dF(i, j), 1e-10);
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, ComputeThrustDirection)
{
    {
        const Tuple<QLaw, Vector6d, Real> parameters = getQLawFullTargeting(QLaw::GradientStrategy::FiniteDifference);
        const QLaw qlaw = std::get<0>(parameters);
        const Vector6d currentCOEVector = std::get<1>(parameters);
        const Real thrustAcceleration = std::get<2>(parameters);

        const Vector3d thrustDirection = qlaw.computeThrustDirection(currentCOEVector, thrustAcceleration);

        const Vector3d expectedThrustDirection = {0.63856458, 0.00650164, -0.76954078};

        for (Size i = 0; i < 3; ++i)
        {
            EXPECT_NEAR(thrustDirection(i), expectedThrustDirection(i), 1e-8);
        }
    }

    {
        const Tuple<QLaw, Vector6d, Real> parameters = getQLawFullTargeting(QLaw::GradientStrategy::Analytical);
        const QLaw qlaw = std::get<0>(parameters);
        const Vector6d currentCOEVector = std::get<1>(parameters);
        const Real thrustAcceleration = std::get<2>(parameters);

        const Vector3d thrustDirection = qlaw.computeThrustDirection(currentCOEVector, thrustAcceleration);

        const Vector3d expectedThrustDirection = {0.65328616, 0.00639639, -0.75708406};

        for (Size i = 0; i < 3; ++i)
        {
            EXPECT_NEAR(thrustDirection(i), expectedThrustDirection(i), 1e-8);
        }
    }

    // expect no thrust when within tolerance of targeted orbital elements
    {
        const COE coe = {
            Length::Meters(26500.0e3),
            0.7,
            Angle::Degrees(116.0),
            Angle::Degrees(180.0),
            Angle::Degrees(270.0),
            Angle::Degrees(0.0),
        };
        const Vector6d currentCOEVector = coe.getSIVector(COE::AnomalyType::True);
        const Real thrustAcceleration = 0.1;
        const QLaw qlaw = {
            coe,
            gravitationalParameter_,
            {
                {
                    {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                    {COE::Element::Eccentricity, {1.0, 1e-4}},
                    {COE::Element::Inclination, {1.0, 1e-4}},
                    {COE::Element::Raan, {1.0, 1e-4}},
                    {COE::Element::Aop, {1.0, 1e-4}},
                },
                3,
                4,
                2,
                0.01,
                100,
                1.0,
                Length::Kilometers(6578.0),
            },
            QLaw::COEDomain::Osculating,
            gradientStrategy_,
        };

        const Vector3d thrustDirection = qlaw.computeThrustDirection(currentCOEVector, thrustAcceleration);

        const Vector3d expectedThrustDirection = {0.0, 0.0, 0.0};

        for (Size i = 0; i < 3; ++i)
        {
            EXPECT_NEAR(thrustDirection(i), expectedThrustDirection(i), 1e-14);
        }
    }

    /// Absolute effectivity threshold
    {
        const COE currentCOE = {
            Length::Meters(7000.0e3),
            0.01,
            Angle::Degrees(0.05),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(90.0),
        };

        const Vector6d currentCOEVector = currentCOE.getSIVector(COE::AnomalyType::True);
        const Real thrustAcceleration = 0.1;
        const QLaw qlaw = {
            targetCOE_,
            gravitationalParameter_,
            {
                {
                    {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                    {COE::Element::Eccentricity, {1.0, 1e-4}},
                    {COE::Element::Inclination, {1.0, 1e-4}},
                    {COE::Element::Raan, {1.0, 1e-4}},
                    {COE::Element::Aop, {1.0, 1e-4}},
                },
                3,
                4,
                2,
                0.01,
                100,
                1.0,
                Length::Kilometers(6578.0),
                0.8,
            },
            QLaw::COEDomain::Osculating,
            gradientStrategy_,
        };

        const Vector3d thrustDirection = qlaw.computeThrustDirection(currentCOEVector, thrustAcceleration);

        const Vector3d expectedThrustDirection = {0.0, 0.0, 0.0};

        for (Size i = 0; i < 3; ++i)
        {
            EXPECT_NEAR(thrustDirection(i), expectedThrustDirection(i), 1e-14);
        }
    }

    /// Relative effectivity threshold
    {
        const COE currentCOE = {
            Length::Meters(7000.0e3),
            0.01,
            Angle::Degrees(0.05),
            Angle::Degrees(0.0),
            Angle::Degrees(0.0),
            Angle::Degrees(90.0),
        };

        const Vector6d currentCOEVector = currentCOE.getSIVector(COE::AnomalyType::True);
        const Real thrustAcceleration = 0.1;
        const QLaw qlaw = {
            targetCOE_,
            gravitationalParameter_,
            {
                {
                    {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                    {COE::Element::Eccentricity, {1.0, 1e-4}},
                    {COE::Element::Inclination, {1.0, 1e-4}},
                    {COE::Element::Raan, {1.0, 1e-4}},
                    {COE::Element::Aop, {1.0, 1e-4}},
                },
                3,
                4,
                2,
                0.01,
                100,
                1.0,
                Length::Kilometers(6578.0),
                Real::Undefined(),
                0.8,
            },
            QLaw::COEDomain::Osculating,
            gradientStrategy_,
        };

        const Vector3d thrustDirection = qlaw.computeThrustDirection(currentCOEVector, thrustAcceleration);

        const Vector3d expectedThrustDirection = {0.0, 0.0, 0.0};

        for (Size i = 0; i < 3; ++i)
        {
            EXPECT_NEAR(thrustDirection(i), expectedThrustDirection(i), 1e-14);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw, Compute_dQ_dOE)
{
    {
        const Tuple<QLaw, Vector6d, Real> parameters = getQLawFullTargeting(QLaw::GradientStrategy::FiniteDifference);
        const QLaw qlaw = std::get<0>(parameters);
        const Vector6d currentCOEVector = std::get<1>(parameters);
        const Real thrustAcceleration = std::get<2>(parameters);

        const Vector5d dQ_dOE = qlaw.compute_dQ_dOE(currentCOEVector.segment(0, 5), thrustAcceleration);

        // finite differences manually by using sympy
        const Vector5d expected_dQ_dOE = {
            -4458973.44508479,
            308846550517104.0,
            478538133471352.0,
            -99677385.5546418,
            1306605428444.13,
        };

        for (Size i = 0; i < 5; ++i)
        {
            const double relativeError = std::abs((dQ_dOE(i) - expected_dQ_dOE(i)) / expected_dQ_dOE(i));
            EXPECT_LT(relativeError, 1e-5);
        }
    }

    {
        const Tuple<QLaw, Vector6d, Real> parameters = getQLawFullTargeting(QLaw::GradientStrategy::Analytical);
        const QLaw qlaw = std::get<0>(parameters);
        const Vector6d currentCOEVector = std::get<1>(parameters);
        const Real thrustAcceleration = std::get<2>(parameters);

        const Vector5d dQ_dOE = qlaw.compute_dQ_dOE(currentCOEVector.segment(0, 5), thrustAcceleration);

        // calculated analtyically by using sympy
        const Vector5d expected_dQ_dOE = {
            -4451831.72900846,
            304679993012117.0,
            478538188797579.0,
            -99677391.4654718,
            1306605416901.52,
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
                {COE::Element::SemiMajorAxis, {1.0, 100.0}},
                {COE::Element::Eccentricity, {1.0, 1e-4}},
                {COE::Element::Inclination, {1.0, 1e-4}},
                {COE::Element::Raan, {1.0, 1e-4}},
                {COE::Element::Aop, {1.0, 1e-4}},
            },
            3,
            4,
            2,
            0.01,
            100,
            1.0,
            Length::Kilometers(6578.0),
        };

        const QLaw qlaw = {
            targetCOE,
            gravitationalParameter_,
            parameters,
            QLaw::COEDomain::Osculating,
            gradientStrategy_,
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
            {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()}
        };

        const Vector3d acceleration = qlaw.calculateThrustAccelerationAt(
            initialState.accessInstant(),
            initialState.getPosition().getCoordinates(),
            initialState.getVelocity().getCoordinates(),
            2.0 / 2000.0,
            Frame::GCRF()
        );

        const Vector3d accelerationExpected = {
            -8.2474804263461029e-05,
            0.00063405934466644542,
            -0.00076887362687431417,
        };

        for (Size i = 0; i < 3; ++i)
        {
            EXPECT_NEAR(acceleration(i), accelerationExpected(i), 1e-12);
        }
    }
}

class OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw_Effectivity
    : public ::testing::TestWithParam<std::tuple<QLaw::GradientStrategy, QLaw::COEDomain>>
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

        initialState_ = State {
            Instant::J2000(),
            cartesianState.first,
            cartesianState.second,
        };
    }

   protected:
    const COE targetCOE_ = {
        Length::Meters(42000.0e3),
        0.01,
        Angle::Degrees(0.05),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
        Angle::Degrees(0.0),
    };

    const QLaw::Parameters parameters_ = {
        {
            {COE::Element::SemiMajorAxis, {1.0, 100.0}},
            {COE::Element::Eccentricity, {1.0, 1e-4}},
            {COE::Element::Inclination, {1.0, 1e-4}},
            {COE::Element::Raan, {1.0, 1e-4}},
            {COE::Element::Aop, {1.0, 1e-4}},
        },
        3,
        4,
        2,
        0.01,
        100,
        1.0,
        Length::Kilometers(6578.0),
        Real::Undefined(),
        Real::Undefined(),
    };

    const Derived gravitationalParameter_ = Derived(398600.49 * 1e9, Derived::Unit::MeterCubedPerSecondSquared());

    const Real thrustAcceleration_ = 1.0 / 300.0;

    State initialState_ = State::Undefined();
};

static std::string GenerateTestName(
    const ::testing::TestParamInfo<std::tuple<QLaw::GradientStrategy, QLaw::COEDomain>>& info
)
{
    const QLaw::GradientStrategy strategy = std::get<0>(info.param);
    const QLaw::COEDomain domain = std::get<1>(info.param);

    std::string strategyStr =
        (strategy == QLaw::GradientStrategy::FiniteDifference) ? "FiniteDifference" : "Analytical";
    std::string domainStr = (domain == QLaw::COEDomain::Osculating) ? "Osculating" : "BrouwerLyddaneMeanLong";
    return strategyStr + "_" + domainStr;
}

INSTANTIATE_TEST_SUITE_P(
    OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw_Effectivity,
    OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw_Effectivity,
    ::testing::Values(
        std::make_tuple(QLaw::GradientStrategy::FiniteDifference, QLaw::COEDomain::Osculating),
        std::make_tuple(QLaw::GradientStrategy::FiniteDifference, QLaw::COEDomain::BrouwerLyddaneMeanLong),
        std::make_tuple(QLaw::GradientStrategy::Analytical, QLaw::COEDomain::Osculating),
        std::make_tuple(QLaw::GradientStrategy::Analytical, QLaw::COEDomain::BrouwerLyddaneMeanLong)
    ),
    GenerateTestName
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Dynamics_Thruster_GuidanceLaw_QLaw_Effectivity, ComputeEffectivity)
{
    const QLaw::COEDomain coeDomain = std::get<1>(GetParam());
    const QLaw::GradientStrategy gradientStrategy = std::get<0>(GetParam());

    const QLaw qlaw = {
        targetCOE_,
        gravitationalParameter_,
        parameters_,
        coeDomain,
        gradientStrategy,
    };

    // Test with default true anomaly angles (empty array)
    {
        const Tuple<double, double> effectivity = qlaw.computeEffectivity(initialState_, thrustAcceleration_);

        const double etaRelative = std::get<0>(effectivity);
        const double etaAbsolute = std::get<1>(effectivity);

        // Effectivity values should be in range [0, 1] or slightly above 1 for absolute
        EXPECT_GE(etaRelative, 0.0);
        EXPECT_LE(etaRelative, 1.0);
        EXPECT_GT(etaAbsolute, 0.0);
        EXPECT_LE(etaAbsolute, 1.0);

        // Both values should be finite
        EXPECT_TRUE(std::isfinite(etaRelative));
        EXPECT_TRUE(std::isfinite(etaAbsolute));
    }

    // Test with custom true anomaly angles
    {
        const Tuple<double, double> effectivity = qlaw.computeEffectivity(initialState_, thrustAcceleration_, 15);

        const double etaRelative = std::get<0>(effectivity);
        const double etaAbsolute = std::get<1>(effectivity);

        // Effectivity values should be in range [0, 1] or slightly above 1 for absolute
        EXPECT_GE(etaRelative, 0.0);
        EXPECT_LE(etaRelative, 1.0);
        EXPECT_GT(etaAbsolute, 0.0);

        // Both values should be finite
        EXPECT_TRUE(std::isfinite(etaRelative));
        EXPECT_TRUE(std::isfinite(etaAbsolute));
    }
}
