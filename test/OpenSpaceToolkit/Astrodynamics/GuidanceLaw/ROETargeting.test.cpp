/// Apache License 2.0

#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ROETargeting.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::filesystem::Directory;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::Matrix63d;
using ostk::astrodynamics::guidancelaw::Matrix6d;
using ostk::astrodynamics::guidancelaw::ROETargeting;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::Propagated;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(Earth::FromModels(
            std::make_shared<EarthGravitationalModel>(
                EarthGravitationalModel::Type::EGM96, Directory::Undefined(), 2, 0
            ),
            std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
            std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Undefined)
        ));

        freeDynamics_ = Dynamics::FromEnvironment(Environment(epoch_, {earthSPtr}));

        const COE::CartesianState referenceCartesianState =
            referenceCOE_.getCartesianState(gravitationalParameter_, Frame::GCRF());

        referenceState_ = {epoch_, referenceCartesianState.first, referenceCartesianState.second};

        referenceTrajectory_ = Trajectory(Propagated(Propagator(numericalSolver_, freeDynamics_), referenceState_));
    }

    // Osculating state of a spacecraft trailing the reference by a distance along-track
    State trailingState(const Length& aDistance) const
    {
        Vector6d coeVector = referenceCOE_.getSIVector(COE::AnomalyType::Mean);
        coeVector[5] -= aDistance.inMeters() / coeVector[0];

        const COE coe = COE::FromSIVector(coeVector, COE::AnomalyType::Mean);

        const COE::CartesianState cartesianState = coe.getCartesianState(gravitationalParameter_, Frame::GCRF());

        VectorXd coordinates(7);
        coordinates << cartesianState.first.accessCoordinates(), cartesianState.second.accessCoordinates(), wetMass_;

        return {epoch_, coordinates, Frame::GCRF(), coordinateBrokerSPtr_};
    }

    const Instant epoch_ = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
    const Derived gravitationalParameter_ = EarthGravitationalModel::EGM2008.gravitationalParameter_;

    const COE referenceCOE_ = {
        Length::Kilometers(6928.137),
        0.001,
        Angle::Degrees(53.0),
        Angle::Degrees(10.0),
        Angle::Degrees(30.0),
        Angle::Degrees(40.0),
    };

    const NumericalSolver numericalSolver_ = {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-12,
        1.0e-12,
    };

    const Real thrustAcceleration_ = 0.0151 / 200.0;
    const Real wetMass_ = 200.0;

    const ROETargeting::Parameters parameters_ = {
        epoch_,
        Duration::Days(1.0),
        Duration::Minutes(10.0),
        thrustAcceleration_,
    };

    const Shared<const CoordinateBroker> coordinateBrokerSPtr_ = std::make_shared<CoordinateBroker>(CoordinateBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinateSubset::Mass(),
    }));

    Array<Shared<Dynamics>> freeDynamics_ = Array<Shared<Dynamics>>::Empty();
    State referenceState_ = State::Undefined();
    Trajectory referenceTrajectory_ = Trajectory::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, ParametersConstructor)
{
    {
        const ROETargeting::Parameters parameters = {
            epoch_,
            Duration::Days(1.0),
            Duration::Minutes(10.0),
            1e-4,
            3,
            Vector6d::Constant(5.0),
            0.5,
            Duration::Seconds(2.0),
            -1e-4,
        };

        EXPECT_EQ(epoch_, parameters.burnWindowEpoch);
        EXPECT_EQ(Duration::Days(1.0), parameters.burnWindowPeriod);
        EXPECT_EQ(Duration::Minutes(10.0), parameters.maximumBurnDuration);
        EXPECT_EQ(1e-4, parameters.thrustAcceleration);
        EXPECT_EQ(3, parameters.horizonWindowCount);
        EXPECT_EQ(Vector6d::Constant(5.0), parameters.toleranceVector);
        EXPECT_EQ(0.5, parameters.fuelWeight);
        EXPECT_EQ(Duration::Seconds(2.0), parameters.minimumBurnDuration);
        EXPECT_EQ(-1e-4, parameters.relativeSemiMajorAxisRate);
    }

    {
        const ROETargeting::Parameters parameters = {epoch_, Duration::Days(1.0), Duration::Minutes(10.0), 1e-4};

        EXPECT_EQ(5, parameters.horizonWindowCount);
        EXPECT_EQ(Vector6d::Constant(10.0), parameters.toleranceVector);
        EXPECT_EQ(1.0, parameters.fuelWeight);
        EXPECT_EQ(Duration::Seconds(1.0), parameters.minimumBurnDuration);
        EXPECT_EQ(0.0, parameters.relativeSemiMajorAxisRate);
    }

    {
        EXPECT_THROW(
            ROETargeting::Parameters(Instant::Undefined(), Duration::Days(1.0), Duration::Minutes(10.0), 1e-4),
            ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(
            ROETargeting::Parameters(epoch_, Duration::Undefined(), Duration::Minutes(10.0), 1e-4),
            ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(
            ROETargeting::Parameters(epoch_, Duration::Days(1.0), Duration::Zero(), 1e-4),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(epoch_, Duration::Minutes(15.0), Duration::Minutes(10.0), 1e-4),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(epoch_, Duration::Days(1.0), Duration::Minutes(10.0), 0.0),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(epoch_, Duration::Days(1.0), Duration::Minutes(10.0), 1e-4, 0),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(
                epoch_, Duration::Days(1.0), Duration::Minutes(10.0), 1e-4, 5, Vector6d::Constant(0.0)
            ),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(
                epoch_, Duration::Days(1.0), Duration::Minutes(10.0), 1e-4, 5, Vector6d::Constant(10.0), -1.0
            ),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(
                epoch_,
                Duration::Days(1.0),
                Duration::Minutes(10.0),
                1e-4,
                5,
                Vector6d::Constant(10.0),
                1.0,
                Duration::Minutes(11.0)
            ),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, Constructor)
{
    {
        EXPECT_NO_THROW(ROETargeting(referenceTrajectory_, parameters_));
    }

    {
        Vector6d target;
        target << 0.0, -5000.0, 0.0, 0.0, 0.0, 0.0;

        EXPECT_NO_THROW(ROETargeting(referenceTrajectory_, parameters_, target));
    }

    {
        EXPECT_THROW(ROETargeting(Trajectory::Undefined(), parameters_), ostk::core::error::runtime::Undefined);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, StreamOperator)
{
    const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << guidanceLaw << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, Getters)
{
    Vector6d target;
    target << 0.0, -5000.0, 0.0, 0.0, 0.0, 0.0;

    const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_, target};

    EXPECT_EQ(referenceTrajectory_, guidanceLaw.accessReferenceTrajectory());
    EXPECT_EQ(parameters_.burnWindowEpoch, guidanceLaw.getParameters().burnWindowEpoch);
    EXPECT_EQ(parameters_.thrustAcceleration, guidanceLaw.getParameters().thrustAcceleration);
    EXPECT_EQ(target, guidanceLaw.getTargetRelativeOrbitalElements());
    EXPECT_EQ("ROE Targeting", guidanceLaw.getName());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, GetBurnWindowAt)
{
    const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

    const Interval firstWindow = Interval::Closed(epoch_, epoch_ + Duration::Minutes(10.0));
    const Interval secondWindow =
        Interval::Closed(epoch_ + Duration::Days(1.0), epoch_ + Duration::Days(1.0) + Duration::Minutes(10.0));

    EXPECT_EQ(firstWindow, guidanceLaw.getBurnWindowAt(epoch_ - Duration::Hours(1.0)));
    EXPECT_EQ(firstWindow, guidanceLaw.getBurnWindowAt(epoch_));
    EXPECT_EQ(firstWindow, guidanceLaw.getBurnWindowAt(epoch_ + Duration::Minutes(5.0)));
    EXPECT_EQ(secondWindow, guidanceLaw.getBurnWindowAt(epoch_ + Duration::Minutes(11.0)));
    EXPECT_EQ(secondWindow, guidanceLaw.getBurnWindowAt(epoch_ + Duration::Days(1.0)));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, ComputeRelativeOrbitalElements_Static)
{
    const double a = referenceCOE_.getSemiMajorAxis().inMeters();

    {
        EXPECT_TRUE(ROETargeting::ComputeRelativeOrbitalElements(referenceCOE_, referenceCOE_).isZero(1e-15));
    }

    {
        Vector6d offset;
        offset << 100.0, 0.0, 50.0 / a, 20.0 / a, 0.0, 1000.0 / a;

        const COE coe =
            COE::FromSIVector(referenceCOE_.getSIVector(COE::AnomalyType::Mean) + offset, COE::AnomalyType::Mean);

        const Vector6d relativeOrbitalElements = ROETargeting::ComputeRelativeOrbitalElements(coe, referenceCOE_) * a;

        const double inclination = referenceCOE_.getInclination().inRadians();

        EXPECT_NEAR(100.0, relativeOrbitalElements[0], 1e-9);
        EXPECT_NEAR(1000.0 + 20.0 * std::cos(inclination), relativeOrbitalElements[1], 1e-6);
        EXPECT_NEAR(0.0, relativeOrbitalElements[2], 1e-9);
        EXPECT_NEAR(0.0, relativeOrbitalElements[3], 1e-9);
        EXPECT_NEAR(50.0, relativeOrbitalElements[4], 1e-6);
        EXPECT_NEAR(20.0 * std::sin(inclination), relativeOrbitalElements[5], 1e-6);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, ComputeStateTransitionMatrix)
{
    {
        EXPECT_TRUE(ROETargeting::ComputeStateTransitionMatrix(referenceCOE_, Duration::Zero()).isIdentity(1e-15));
    }

    // Compare against the exact secular J2 propagation of the mean elements of both spacecraft
    {
        const double mu = gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared());
        const double equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_.inMeters();
        const double J2 = EarthGravitationalModel::EGM2008.J2_;

        const auto propagate = [&](const COE& aCOE, const double& aTau) -> COE
        {
            const Vector6d coe = aCOE.getSIVector(COE::AnomalyType::Mean);

            const double n = std::sqrt(mu / std::pow(coe[0], 3));
            const double p = coe[0] * (1.0 - coe[1] * coe[1]);
            const double factor = 0.75 * n * J2 * std::pow(equatorialRadius / p, 2);
            const double cosI = std::cos(coe[2]);

            Vector6d propagatedCOE = coe;
            propagatedCOE[3] += -2.0 * factor * cosI * aTau;
            propagatedCOE[4] += factor * (5.0 * cosI * cosI - 1.0) * aTau;
            propagatedCOE[5] += (n + factor * std::sqrt(1.0 - coe[1] * coe[1]) * (3.0 * cosI * cosI - 1.0)) * aTau;

            return COE::FromSIVector(propagatedCOE, COE::AnomalyType::Mean);
        };

        for (const double eccentricity : {0.0005, 0.01, 0.1})
        {
            Vector6d referenceCOEVector = referenceCOE_.getSIVector(COE::AnomalyType::Mean);
            referenceCOEVector[1] = eccentricity;

            Vector6d offset;
            offset << 30.0, 2e-6, 1e-6, 1e-6, 1e-5, -1.4e-4;

            const COE referenceCOE = COE::FromSIVector(referenceCOEVector, COE::AnomalyType::Mean);
            const COE coe = COE::FromSIVector(referenceCOEVector + offset, COE::AnomalyType::Mean);

            for (const double tau : {3600.0, 86400.0, 5.0 * 86400.0})
            {
                const Vector6d expectedRelativeOrbitalElements =
                    ROETargeting::ComputeRelativeOrbitalElements(propagate(coe, tau), propagate(referenceCOE, tau));

                const Vector6d relativeOrbitalElements =
                    ROETargeting::ComputeStateTransitionMatrix(referenceCOE, Duration::Seconds(tau)) *
                    ROETargeting::ComputeRelativeOrbitalElements(coe, referenceCOE);

                // Second order error only: 22 km of drift over 5 days, predicted to better than 0.2 m
                EXPECT_LT(
                    (relativeOrbitalElements - expectedRelativeOrbitalElements).cwiseAbs().maxCoeff() *
                        referenceCOEVector[0],
                    0.2
                );
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, ComputeControlInputMatrix)
{
    const double a = referenceCOE_.getSemiMajorAxis().inMeters();
    const double meanMotion =
        std::sqrt(gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared()) / (a * a * a));

    // Impulsive: compare against the change of osculating elements of a near-circular Keplerian orbit
    {
        const COE referenceCOE = {
            referenceCOE_.getSemiMajorAxis(),
            0.0001,
            referenceCOE_.getInclination(),
            referenceCOE_.getRaan(),
            referenceCOE_.getAop(),
            referenceCOE_.getTrueAnomaly(),
        };

        const COE::CartesianState cartesianState =
            referenceCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());
        const Vector3d position = cartesianState.first.accessCoordinates();
        const Vector3d velocity = cartesianState.second.accessCoordinates();

        Matrix3d rtnToGCRF;
        rtnToGCRF.col(0) = position.normalized();
        rtnToGCRF.col(2) = position.cross(velocity).normalized();
        rtnToGCRF.col(1) = rtnToGCRF.col(2).cross(rtnToGCRF.col(0));

        const Vector6d referenceCOEVector = referenceCOE.getSIVector(COE::AnomalyType::Mean);
        const double meanArgumentOfLatitude = referenceCOEVector[4] + referenceCOEVector[5];

        const Matrix63d controlInputMatrix =
            ROETargeting::ComputeControlInputMatrix(a, meanArgumentOfLatitude, meanArgumentOfLatitude);

        for (const Vector3d& deltaVelocity :
             {Vector3d(0.01, 0.0, 0.0), Vector3d(0.0, 0.01, 0.0), Vector3d(0.0, 0.0, 0.01)})
        {
            const COE perturbedCOE = COE::Cartesian(
                {Position::Meters(position, Frame::GCRF()),
                 Velocity::MetersPerSecond(velocity + rtnToGCRF * deltaVelocity, Frame::GCRF())},
                gravitationalParameter_
            );

            const Vector6d expectedRelativeOrbitalElements =
                ROETargeting::ComputeRelativeOrbitalElements(perturbedCOE, referenceCOE);
            const Vector6d relativeOrbitalElements = controlInputMatrix * deltaVelocity;

            EXPECT_LT(
                (relativeOrbitalElements - expectedRelativeOrbitalElements).norm(),
                0.01 * expectedRelativeOrbitalElements.norm()
            ) << deltaVelocity.transpose();
        }
    }

    // Arc average
    {
        const double start = 0.3;
        const double end = 1.2;
        const Size sampleCount = 100000;

        Matrix63d expectedControlInputMatrix = Matrix63d::Zero();
        for (Size index = 0; index < sampleCount; ++index)
        {
            const double u = start + (end - start) * (index + 0.5) / sampleCount;
            expectedControlInputMatrix += ROETargeting::ComputeControlInputMatrix(a, u, u) / sampleCount;
        }

        EXPECT_TRUE(ROETargeting::ComputeControlInputMatrix(a, start, end).isApprox(expectedControlInputMatrix, 1e-9));

        EXPECT_NEAR(2.0 / (meanMotion * a), ROETargeting::ComputeControlInputMatrix(a, start, end)(0, 1), 1e-15);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, ComputeRelativeOrbitalElements)
{
    const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

    {
        EXPECT_LT(guidanceLaw.computeRelativeOrbitalElements(referenceState_).cwiseAbs().maxCoeff(), 1e-6);
    }

    {
        const Vector6d relativeOrbitalElements =
            guidanceLaw.computeRelativeOrbitalElements(trailingState(Length::Meters(1000.0)));

        EXPECT_NEAR(-1000.0, relativeOrbitalElements[1], 5.0);
        EXPECT_LT(relativeOrbitalElements.cwiseAbs().maxCoeff() - std::abs(relativeOrbitalElements[1]), 5.0);
    }

    // A radial impulse shifts the mean argument of latitude by -2 dv / n (not by -4 dv / n, as reading the mean
    // anomaly of Brouwer-Lyddane elements through COE::getSIVector would)
    {
        const double deltaVelocity = 0.045;
        const double a = referenceCOE_.getSemiMajorAxis().inMeters();
        const double meanMotion =
            std::sqrt(gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared()) / (a * a * a));

        const Vector3d position = referenceState_.getPosition().accessCoordinates();
        const Vector3d velocity = referenceState_.getVelocity().accessCoordinates();

        const State state = {
            epoch_,
            Position::Meters(position, Frame::GCRF()),
            Velocity::MetersPerSecond(velocity + deltaVelocity * position.normalized(), Frame::GCRF()),
        };

        const Vector6d relativeOrbitalElements = guidanceLaw.computeRelativeOrbitalElements(state);

        EXPECT_NEAR(-2.0 * deltaVelocity / meanMotion, relativeOrbitalElements[1], 1.0);
        EXPECT_NEAR(deltaVelocity / meanMotion, relativeOrbitalElements.segment<2>(2).norm(), 1.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, ComputePlan)
{
    const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

    const ROETargeting::Plan plan = guidanceLaw.computePlan(trailingState(Length::Meters(1000.0)));

    const Size windowCount = parameters_.horizonWindowCount;

    EXPECT_EQ(epoch_, plan.epoch);
    EXPECT_NEAR(referenceCOE_.getSemiMajorAxis().inMeters(), plan.referenceSemiMajorAxis, 1e4);
    EXPECT_NEAR(-1000.0, plan.relativeOrbitalElements[1], 5.0);
    EXPECT_EQ(windowCount, plan.burnIntervals.getSize());
    EXPECT_EQ(windowCount, plan.deltaVelocities.getSize());
    EXPECT_EQ(2 * windowCount, plan.predictionInstants.getSize());
    EXPECT_EQ(2 * windowCount, plan.predictedRelativeOrbitalElements.getSize());
    EXPECT_GE(plan.iterationCount, 1);

    const double maximumDeltaVelocity = thrustAcceleration_ * parameters_.maximumBurnDuration.inSeconds();

    for (Size windowIndex = 0; windowIndex < windowCount; ++windowIndex)
    {
        const Interval& burnInterval = plan.burnIntervals[windowIndex];
        const double deltaVelocity = plan.deltaVelocities[windowIndex].norm();

        EXPECT_EQ(epoch_ + parameters_.burnWindowPeriod * double(windowIndex), burnInterval.accessStart());
        EXPECT_LE(burnInterval.getDuration(), parameters_.maximumBurnDuration);
        EXPECT_LE(deltaVelocity, maximumDeltaVelocity * (1.0 + 1e-9));
        EXPECT_NEAR(deltaVelocity / thrustAcceleration_, burnInterval.getDuration().inSeconds(), 1e-6);
    }

    EXPECT_EQ(epoch_ + parameters_.burnWindowPeriod * double(windowCount), plan.predictionInstants.accessLast());

    // The first burn is used, and the along-track error is removed by the end of the horizon
    EXPECT_GT(plan.deltaVelocities.accessFirst().norm(), 0.0);
    EXPECT_LT(std::abs(plan.predictedRelativeOrbitalElements.accessLast()[1]), 50.0);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, CalculateThrustAccelerationAt)
{
    const State state = trailingState(Length::Meters(1000.0));
    const Vector3d position = state.getPosition().accessCoordinates();
    const Vector3d velocity = state.getVelocity().accessCoordinates();

    const ROETargeting::Plan plan = ROETargeting(referenceTrajectory_, parameters_).computePlan(state);
    const Interval burnInterval = plan.burnIntervals.accessFirst();

    ASSERT_TRUE(burnInterval.getDuration().isStrictlyPositive());

    // Before the first window
    {
        const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

        EXPECT_EQ(
            Vector3d::Zero(),
            guidanceLaw.calculateThrustAccelerationAt(
                epoch_ - Duration::Seconds(1.0), position, velocity, thrustAcceleration_, Frame::GCRF()
            )
        );
    }

    // At the window start: thrust along the planned RTN direction
    {
        const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

        const Vector3d acceleration =
            guidanceLaw.calculateThrustAccelerationAt(epoch_, position, velocity, thrustAcceleration_, Frame::GCRF());

        Matrix3d rtnToGCRF;
        rtnToGCRF.col(0) = position.normalized();
        rtnToGCRF.col(2) = position.cross(velocity).normalized();
        rtnToGCRF.col(1) = rtnToGCRF.col(2).cross(rtnToGCRF.col(0));

        EXPECT_NEAR(thrustAcceleration_, acceleration.norm(), 1e-15);
        EXPECT_TRUE((rtnToGCRF.transpose() * acceleration / thrustAcceleration_)
                        .isApprox(plan.deltaVelocities.accessFirst().normalized(), 1e-9));

        // The thrust magnitude follows the provided one
        EXPECT_NEAR(
            1.0, guidanceLaw.calculateThrustAccelerationAt(epoch_, position, velocity, 1.0, Frame::GCRF()).norm(), 1e-12
        );

        // At the burn end (closed interval)
        EXPECT_NEAR(
            thrustAcceleration_,
            guidanceLaw
                .calculateThrustAccelerationAt(
                    burnInterval.accessEnd(), position, velocity, thrustAcceleration_, Frame::GCRF()
                )
                .norm(),
            1e-15
        );

        // After the burn, within the window
        EXPECT_EQ(
            Vector3d::Zero(),
            guidanceLaw.calculateThrustAccelerationAt(
                burnInterval.accessEnd() + Duration::Seconds(1.0),
                position,
                velocity,
                thrustAcceleration_,
                Frame::GCRF()
            )
        );

        // After the window
        EXPECT_EQ(
            Vector3d::Zero(),
            guidanceLaw.calculateThrustAccelerationAt(
                epoch_ + Duration::Hours(1.0), position, velocity, thrustAcceleration_, Frame::GCRF()
            )
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, Propagation)
{
    // A spacecraft trailing the reference by 1 km reaches it with one burn of at most 10 minutes per day
    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0}, {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}}, {1.0, 1.0, 1.0}
    ));

    const SatelliteSystem satelliteSystem = {
        Mass::Kilograms(150.0),
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.2,
        PropulsionSystem(0.0151, 1016.0),
    };

    const Shared<ROETargeting> guidanceLawSPtr = std::make_shared<ROETargeting>(referenceTrajectory_, parameters_);

    const Propagator coastPropagator = {numericalSolver_, freeDynamics_};
    const Propagator burnPropagator = {
        numericalSolver_,
        freeDynamics_ + Array<Shared<Dynamics>> {std::make_shared<Thruster>(satelliteSystem, guidanceLawSPtr)}
    };

    State state = trailingState(Length::Meters(1000.0));

    const Size windowCount = 4;

    for (Size windowIndex = 0; windowIndex < windowCount; ++windowIndex)
    {
        const Interval burnWindow =
            guidanceLawSPtr->getBurnWindowAt(epoch_ + parameters_.burnWindowPeriod * double(windowIndex));

        state = coastPropagator.calculateStateAt(state, burnWindow.accessStart());

        // Propagate the burn on its own, so that the integrator does not step over the thrust switch-off
        const Interval burnInterval = guidanceLawSPtr->computePlan(state).burnIntervals.accessFirst();

        if (burnInterval.getDuration().isStrictlyPositive())
        {
            state = burnPropagator.calculateStateAt(state, burnInterval.accessEnd());
        }
    }

    state = coastPropagator.calculateStateAt(state, epoch_ + Duration::Days(double(windowCount)));

    const Vector6d relativeOrbitalElements = guidanceLawSPtr->computeRelativeOrbitalElements(state);

    EXPECT_LT(std::abs(relativeOrbitalElements[0]), 5.0) << relativeOrbitalElements.transpose();
    EXPECT_LT(std::abs(relativeOrbitalElements[1]), 50.0) << relativeOrbitalElements.transpose();
    EXPECT_LT(relativeOrbitalElements.segment<2>(2).norm(), 30.0) << relativeOrbitalElements.transpose();
    EXPECT_LT(relativeOrbitalElements.segment<2>(4).norm(), 10.0) << relativeOrbitalElements.transpose();

    // Some propellant was used
    EXPECT_LT(state.extractCoordinate(CoordinateSubset::Mass())[0], wetMass_);
}
