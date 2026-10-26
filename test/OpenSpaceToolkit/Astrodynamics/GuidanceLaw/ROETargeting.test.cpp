/// Apache License 2.0

#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
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
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ROETargeting.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Propagated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
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

using ostk::mathematics::curvefitting::Interpolator;
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
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::Matrix63d;
using ostk::astrodynamics::guidancelaw::Matrix6d;
using ostk::astrodynamics::guidancelaw::ROETargeting;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::Propagated;
using ostk::astrodynamics::trajectory::orbit::model::Tabulated;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::Segment;
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

    // State whose ROE with respect to the reference (computed from Brouwer-Lyddane mean elements) are provided, in m
    State offsetState(const Vector6d& aRelativeOrbitalElements) const
    {
        const BrouwerLyddaneMeanLong referenceMeanCOE = BrouwerLyddaneMeanLong::Cartesian(
            {referenceState_.getPosition(), referenceState_.getVelocity()}, gravitationalParameter_
        );

        const double a = referenceMeanCOE.getSemiMajorAxis().inMeters();
        const double e = referenceMeanCOE.getEccentricity();
        const double inclination = referenceMeanCOE.getInclination().inRadians();
        const double raan = referenceMeanCOE.getRaan().inRadians();
        const double aop = referenceMeanCOE.getAop().inRadians();
        const double meanAnomaly = referenceMeanCOE.getMeanAnomaly().inRadians();

        const Vector6d roe = aRelativeOrbitalElements / a;

        const double eccentricityX = e * std::cos(aop) + roe[2];
        const double eccentricityY = e * std::sin(aop) + roe[3];
        const double offsetAop = std::atan2(eccentricityY, eccentricityX);
        const double raanOffset = roe[5] / std::sin(inclination);
        const double meanArgumentOfLatitude = aop + meanAnomaly + roe[1] - raanOffset * std::cos(inclination);

        const BrouwerLyddaneMeanLong meanCOE = {
            Length::Meters(a * (1.0 + roe[0])),
            std::hypot(eccentricityX, eccentricityY),
            Angle::Radians(inclination + roe[4]),
            Angle::Radians(raan + raanOffset),
            Angle::Radians(offsetAop),
            Angle::Radians(meanArgumentOfLatitude - offsetAop),
        };

        const COE::CartesianState cartesianState = meanCOE.getCartesianState(gravitationalParameter_, Frame::GCRF());

        VectorXd coordinates(7);
        coordinates << cartesianState.first.accessCoordinates(), cartesianState.second.accessCoordinates(), wetMass_;

        return {epoch_, coordinates, Frame::GCRF(), coordinateBrokerSPtr_};
    }

    // Mean argument of latitude of the reference at an instant, in rad
    double referenceMeanArgumentOfLatitudeAt(const Instant& anInstant) const
    {
        const State state = referenceTrajectory_.getStateAt(anInstant);

        const BrouwerLyddaneMeanLong meanCOE =
            BrouwerLyddaneMeanLong::Cartesian({state.getPosition(), state.getVelocity()}, gravitationalParameter_);

        return meanCOE.getAop().inRadians() + meanCOE.getMeanAnomaly().inRadians();
    }

    // Upper bound of the period of the mean argument of latitude of the reference (its Keplerian period)
    Duration orbitalPeriod() const
    {
        const double a = referenceCOE_.getSemiMajorAxis().inMeters();

        return Duration::Seconds(
            2.0 * M_PI * std::sqrt(a * a * a / gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared()))
        );
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
            Duration::Days(1.0),
            Duration::Minutes(10.0),
            1e-4,
            3,
            Vector6d::Constant(5.0),
            0.5,
            Duration::Seconds(2.0),
            -1e-4,
            0.8,
            12,
            true,
        };

        EXPECT_EQ(Duration::Days(1.0), parameters.burnSpacing);
        EXPECT_EQ(Duration::Minutes(10.0), parameters.maximumBurnDuration);
        EXPECT_EQ(1e-4, parameters.thrustAcceleration);
        EXPECT_EQ(3, parameters.horizonBurnCount);
        EXPECT_EQ(Vector6d::Constant(5.0), parameters.toleranceVector);
        EXPECT_EQ(0.5, parameters.fuelWeight);
        EXPECT_EQ(Duration::Seconds(2.0), parameters.minimumBurnDuration);
        EXPECT_EQ(-1e-4, parameters.relativeSemiMajorAxisRate);
        EXPECT_EQ(0.8, parameters.effectivityThreshold);
        EXPECT_EQ(12, parameters.burnStartCandidateCount);
        EXPECT_TRUE(parameters.allowRadialThrust);
    }

    {
        const ROETargeting::Parameters parameters = {Duration::Days(1.0), Duration::Minutes(10.0), 1e-4};

        EXPECT_EQ(5, parameters.horizonBurnCount);
        EXPECT_EQ(Vector6d::Constant(10.0), parameters.toleranceVector);
        EXPECT_EQ(1.0, parameters.fuelWeight);
        EXPECT_EQ(Duration::Seconds(1.0), parameters.minimumBurnDuration);
        EXPECT_EQ(0.0, parameters.relativeSemiMajorAxisRate);
        EXPECT_EQ(0.9, parameters.effectivityThreshold);
        EXPECT_EQ(36, parameters.burnStartCandidateCount);
        EXPECT_FALSE(parameters.allowRadialThrust);
    }

    {
        const Duration day = Duration::Days(1.0);
        const Duration tenMinutes = Duration::Minutes(10.0);
        const Vector6d tolerances = Vector6d::Constant(10.0);
        const Duration second = Duration::Seconds(1.0);

        EXPECT_THROW(
            ROETargeting::Parameters(Duration::Undefined(), tenMinutes, 1e-4), ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(ROETargeting::Parameters(day, Duration::Zero(), 1e-4), ostk::core::error::RuntimeError);
        EXPECT_THROW(
            ROETargeting::Parameters(Duration::Minutes(15.0), tenMinutes, 1e-4), ostk::core::error::RuntimeError
        );
        EXPECT_THROW(ROETargeting::Parameters(day, tenMinutes, 0.0), ostk::core::error::RuntimeError);
        EXPECT_THROW(ROETargeting::Parameters(day, tenMinutes, 1e-4, 0), ostk::core::error::RuntimeError);
        EXPECT_THROW(
            ROETargeting::Parameters(day, tenMinutes, 1e-4, 5, Vector6d::Constant(0.0)), ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(day, tenMinutes, 1e-4, 5, tolerances, -1.0), ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(day, tenMinutes, 1e-4, 5, tolerances, 1.0, Duration::Minutes(11.0)),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(day, tenMinutes, 1e-4, 5, tolerances, 1.0, second, 0.0, 1.5),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(day, tenMinutes, 1e-4, 5, tolerances, 1.0, second, 0.0, -0.1),
            ostk::core::error::RuntimeError
        );
        EXPECT_THROW(
            ROETargeting::Parameters(day, tenMinutes, 1e-4, 5, tolerances, 1.0, second, 0.0, 0.9, 0),
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
    EXPECT_EQ(parameters_.burnSpacing, guidanceLaw.getParameters().burnSpacing);
    EXPECT_EQ(parameters_.thrustAcceleration, guidanceLaw.getParameters().thrustAcceleration);
    EXPECT_EQ(target, guidanceLaw.getTargetRelativeOrbitalElements());
    EXPECT_EQ("ROE Targeting", guidanceLaw.getName());
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

    const State state = trailingState(Length::Meters(1000.0));

    const Size burnCount = parameters_.horizonBurnCount;
    const Duration& burnSpacing = parameters_.burnSpacing;
    const Duration instantTolerance = Duration::Microseconds(1.0);
    const double maximumDeltaVelocity = thrustAcceleration_ * parameters_.maximumBurnDuration.inSeconds();

    {
        const ROETargeting::Plan plan = guidanceLaw.computePlan(state);

        EXPECT_EQ(epoch_, plan.epoch);
        EXPECT_NEAR(referenceCOE_.getSemiMajorAxis().inMeters(), plan.referenceSemiMajorAxis, 1e4);
        EXPECT_NEAR(-1000.0, plan.relativeOrbitalElements[1], 5.0);
        EXPECT_EQ(burnCount, plan.burnIntervals.getSize());
        EXPECT_EQ(burnCount, plan.deltaVelocities.getSize());
        EXPECT_EQ(2 * burnCount, plan.predictionInstants.getSize());
        EXPECT_EQ(2 * burnCount, plan.predictedRelativeOrbitalElements.getSize());
        EXPECT_GT(plan.cost, 0.0);
        EXPECT_GE(plan.iterationCount, 1);

        EXPECT_EQ(epoch_, plan.burnIntervals.accessFirst().accessStart());

        for (Size burnIndex = 0; burnIndex < burnCount; ++burnIndex)
        {
            const Interval& burnInterval = plan.burnIntervals[burnIndex];
            const double deltaVelocity = plan.deltaVelocities[burnIndex].norm();

            // Each burn is centered within one orbit after the earliest middle allowed by the previous one, which
            // keeps the burns at least the burn spacing minus the maximum burn duration apart
            if (burnIndex > 0)
            {
                const Interval& previousBurnInterval = plan.burnIntervals[burnIndex - 1];
                const Instant earliestCenter = previousBurnInterval.getCenter() + burnSpacing;

                EXPECT_GE(burnInterval.getCenter(), earliestCenter - instantTolerance);
                EXPECT_LT(burnInterval.getCenter(), earliestCenter + orbitalPeriod());
                EXPECT_GE(
                    burnInterval.accessStart() - previousBurnInterval.accessEnd(),
                    burnSpacing - parameters_.maximumBurnDuration - instantTolerance
                );
            }

            EXPECT_LE(burnInterval.getDuration(), parameters_.maximumBurnDuration);
            EXPECT_LE(deltaVelocity, maximumDeltaVelocity * (1.0 + 1e-9));
            EXPECT_NEAR(deltaVelocity / thrustAcceleration_, burnInterval.getDuration().inSeconds(), 1e-6);

            // The burns do not thrust radially
            EXPECT_EQ(0.0, plan.deltaVelocities[burnIndex][0]);
        }

        EXPECT_EQ(epoch_ + burnSpacing * 0.5, plan.predictionInstants.accessFirst());
        EXPECT_EQ(epoch_ + burnSpacing * double(burnCount), plan.predictionInstants.accessLast());

        // The first burn is used, and the along-track error is removed by the end of the horizon
        EXPECT_GT(plan.deltaVelocities.accessFirst().norm(), 0.0);
        EXPECT_LT(std::abs(plan.predictedRelativeOrbitalElements.accessLast()[1]), 50.0);
    }

    // Delayed first burn: the following burns are delayed too, the predictions are not
    {
        const Duration delay = Duration::Minutes(30.0);

        const ROETargeting::Plan plan = guidanceLaw.computePlan(state, delay);

        EXPECT_EQ(epoch_ + delay, plan.burnIntervals.accessFirst().accessStart());

        for (Size burnIndex = 1; burnIndex < burnCount; ++burnIndex)
        {
            EXPECT_GE(
                plan.burnIntervals[burnIndex].getCenter(),
                plan.burnIntervals[burnIndex - 1].getCenter() + burnSpacing - instantTolerance
            );
        }

        EXPECT_EQ(epoch_ + burnSpacing * 0.5, plan.predictionInstants.accessFirst());
        EXPECT_EQ(epoch_ + burnSpacing * double(burnCount), plan.predictionInstants.accessLast());
    }

    {
        EXPECT_THROW(guidanceLaw.computePlan(state, Duration::Seconds(-1.0)), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, ComputeBestPlan)
{
    const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

    const Duration& burnSpacing = parameters_.burnSpacing;
    const Duration instantTolerance = Duration::Microseconds(1.0);

    // Mean argument of latitude at the middle of a burn
    const auto burnMeanArgumentOfLatitude = [this](const ROETargeting::Plan& aPlan, const Size& aBurnIndex) -> double
    {
        return referenceMeanArgumentOfLatitudeAt(aPlan.burnIntervals[aBurnIndex].getCenter());
    };

    // The first burn starts within one orbit, and improves on the plan whose first burn starts now
    {
        const State state = trailingState(Length::Meters(1000.0));

        const ROETargeting::Plan bestPlan = guidanceLaw.computeBestPlan(state);
        const ROETargeting::Plan plan = guidanceLaw.computePlan(state);

        EXPECT_EQ(epoch_, bestPlan.epoch);
        EXPECT_LE(bestPlan.cost, plan.cost);

        EXPECT_GE(bestPlan.burnIntervals.accessFirst().accessStart(), epoch_);
        EXPECT_LT(bestPlan.burnIntervals.accessFirst().accessStart(), epoch_ + orbitalPeriod());

        for (Size burnIndex = 1; burnIndex < bestPlan.burnIntervals.getSize(); ++burnIndex)
        {
            const Instant earliestCenter = bestPlan.burnIntervals[burnIndex - 1].getCenter() + burnSpacing;

            EXPECT_GE(bestPlan.burnIntervals[burnIndex].getCenter(), earliestCenter - instantTolerance);
            EXPECT_LT(bestPlan.burnIntervals[burnIndex].getCenter(), earliestCenter + orbitalPeriod());
        }

        EXPECT_EQ(bestPlan.predictionInstants, plan.predictionInstants);
    }

    // Inclination vector error: a normal burn changes the inclination vector along (cos u, sin u), so that it is the
    // most effective at u = atan2(δiy, δix) + π (positive normal delta-v) or atan2(δiy, δix) (negative)
    {
        Vector6d relativeOrbitalElements = Vector6d::Zero();
        relativeOrbitalElements[5] = -30.0;

        const State state = offsetState(relativeOrbitalElements);

        ASSERT_LT(
            (guidanceLaw.computeRelativeOrbitalElements(state) - relativeOrbitalElements).cwiseAbs().maxCoeff(), 1.0
        );

        const ROETargeting::Plan plan = guidanceLaw.computeBestPlan(state);

        const Vector3d& deltaVelocity = plan.deltaVelocities.accessFirst();
        const double meanArgumentOfLatitude = burnMeanArgumentOfLatitude(plan, 0);

        // A single burn removes the error
        EXPECT_GT(deltaVelocity.norm(), 0.0);
        EXPECT_GT(std::abs(deltaVelocity[2]), 0.99 * deltaVelocity.norm());
        EXPECT_GT(deltaVelocity[2] * std::sin(meanArgumentOfLatitude), 0.0);
        EXPECT_LT(std::abs(std::cos(meanArgumentOfLatitude)), std::sin(Angle::Degrees(2.0).inRadians()));
        EXPECT_LT(plan.predictedRelativeOrbitalElements.accessLast().cwiseAbs().maxCoeff(), 2.0);
    }

    // An error larger than a burn can correct: each burn is centered where it is the most effective on the error left
    // by the previous burns (predicted at the end of the previous burn spacing), although the window of each burn
    // after the first starts at an arbitrary argument of latitude
    {
        Vector6d relativeOrbitalElements = Vector6d::Zero();
        relativeOrbitalElements[5] = -100.0;

        const ROETargeting::Plan plan = guidanceLaw.computeBestPlan(offsetState(relativeOrbitalElements));

        Size burnCount = 0;

        for (Size burnIndex = 0; burnIndex < plan.burnIntervals.getSize(); ++burnIndex)
        {
            const Vector3d& deltaVelocity = plan.deltaVelocities[burnIndex];

            if (deltaVelocity.norm() == 0.0)
            {
                continue;
            }

            ++burnCount;

            if (burnIndex > 0)
            {
                ASSERT_LT(plan.predictionInstants[2 * burnIndex - 1], plan.burnIntervals[burnIndex].getCenter());
            }

            const Vector6d& previousRelativeOrbitalElements =
                (burnIndex == 0) ? plan.relativeOrbitalElements
                                 : plan.predictedRelativeOrbitalElements[2 * burnIndex - 1];

            const double bestMeanArgumentOfLatitude =
                std::atan2(previousRelativeOrbitalElements[5], previousRelativeOrbitalElements[4]);
            const double meanArgumentOfLatitude = burnMeanArgumentOfLatitude(plan, burnIndex);

            EXPECT_GT(std::abs(deltaVelocity[2]), 0.99 * deltaVelocity.norm()) << burnIndex;
            EXPECT_LT(deltaVelocity[2] * std::cos(meanArgumentOfLatitude - bestMeanArgumentOfLatitude), 0.0)
                << burnIndex;
            EXPECT_LT(
                std::abs(std::sin(meanArgumentOfLatitude - bestMeanArgumentOfLatitude)),
                std::sin(Angle::Degrees(2.0).inRadians())
            ) << burnIndex;
        }

        EXPECT_GE(burnCount, 3);
        EXPECT_LT(plan.predictedRelativeOrbitalElements.accessLast().cwiseAbs().maxCoeff(), 5.0);
    }

    // Semi-major axis error: without radial thrust, the burn that corrects it is tangential, and changes the
    // eccentricity vector by as much (a tangential delta-v changes both by 2 Δv / n)
    {
        Vector6d relativeOrbitalElements = Vector6d::Zero();
        relativeOrbitalElements[0] = -20.0;

        const ROETargeting::Plan plan = guidanceLaw.computeBestPlan(offsetState(relativeOrbitalElements));

        const Vector3d& deltaVelocity = plan.deltaVelocities.accessFirst();
        const Vector6d& predictedRelativeOrbitalElements = plan.predictedRelativeOrbitalElements.accessFirst();

        ASSERT_LT(plan.burnIntervals.accessFirst().accessEnd(), plan.predictionInstants.accessFirst());

        EXPECT_GT(deltaVelocity[1], 0.99 * deltaVelocity.norm());
        EXPECT_LT(std::abs(predictedRelativeOrbitalElements[0]), 1.0);
        EXPECT_NEAR(20.0, predictedRelativeOrbitalElements.segment<2>(2).norm(), 1.0);
    }

    // Eccentricity vector error: a radial burn changes the eccentricity vector along (sin u, -cos u), without changing
    // the semi-major axis. Without radial thrust, correcting it takes two opposite tangential burns half an orbit
    // apart, more than a burn spacing apart here
    {
        Vector6d relativeOrbitalElements = Vector6d::Zero();
        relativeOrbitalElements[2] = 20.0;

        const State state = offsetState(relativeOrbitalElements);

        ROETargeting::Parameters radialThrustParameters = parameters_;
        radialThrustParameters.allowRadialThrust = true;

        const ROETargeting::Plan plan = guidanceLaw.computeBestPlan(state);
        const ROETargeting::Plan radialThrustPlan =
            ROETargeting(referenceTrajectory_, radialThrustParameters).computeBestPlan(state);

        for (const Vector3d& deltaVelocity : plan.deltaVelocities)
        {
            EXPECT_EQ(0.0, deltaVelocity[0]);
        }

        const Vector3d& deltaVelocity = radialThrustPlan.deltaVelocities.accessFirst();
        const double meanArgumentOfLatitude = burnMeanArgumentOfLatitude(radialThrustPlan, 0);

        EXPECT_GT(std::abs(deltaVelocity[0]), 0.99 * deltaVelocity.norm());
        EXPECT_LT(deltaVelocity[0] * std::sin(meanArgumentOfLatitude), 0.0);
        EXPECT_GT(std::abs(std::sin(meanArgumentOfLatitude)), 0.9);
        EXPECT_LT(radialThrustPlan.cost, 0.5 * plan.cost);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, ComputeEffectivity)
{
    const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

    // On the reference, no burn improves on not burning
    {
        EXPECT_EQ(0.0, guidanceLaw.computeEffectivity(referenceState_));
    }

    {
        const Real effectivity = guidanceLaw.computeEffectivity(trailingState(Length::Meters(1000.0)));

        EXPECT_GE(effectivity, 0.0);
        EXPECT_LE(effectivity, 1.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, CalculateThrustAccelerationAt)
{
    const State state = trailingState(Length::Meters(1000.0));
    const Vector3d position = state.getPosition().accessCoordinates();
    const Vector3d velocity = state.getVelocity().accessCoordinates();

    const auto parametersWithThreshold = [this](const Real& anEffectivityThreshold) -> ROETargeting::Parameters
    {
        ROETargeting::Parameters parameters = parameters_;
        parameters.effectivityThreshold = anEffectivityThreshold;
        return parameters;
    };

    const ROETargeting::Plan plan = ROETargeting(referenceTrajectory_, parameters_).computePlan(state);
    const Real effectivity = ROETargeting(referenceTrajectory_, parameters_).computeEffectivity(state);

    ASSERT_TRUE(plan.burnIntervals.accessFirst().getDuration().isStrictlyPositive());

    // On the reference, no burn
    {
        const ROETargeting guidanceLaw = {referenceTrajectory_, parametersWithThreshold(0.0)};

        EXPECT_EQ(
            Vector3d::Zero(),
            guidanceLaw.calculateThrustAccelerationAt(
                epoch_,
                referenceState_.getPosition().accessCoordinates(),
                referenceState_.getVelocity().accessCoordinates(),
                thrustAcceleration_,
                Frame::GCRF()
            )
        );
    }

    // Effective enough: thrust along the first burn direction of the plan starting now
    {
        const ROETargeting guidanceLaw = {referenceTrajectory_, parametersWithThreshold(0.0)};

        const Vector3d acceleration =
            guidanceLaw.calculateThrustAccelerationAt(epoch_, position, velocity, thrustAcceleration_, Frame::GCRF());

        Matrix3d rtnToGCRF;
        rtnToGCRF.col(0) = position.normalized();
        rtnToGCRF.col(2) = position.cross(velocity).normalized();
        rtnToGCRF.col(1) = rtnToGCRF.col(2).cross(rtnToGCRF.col(0));

        EXPECT_NEAR(thrustAcceleration_, acceleration.norm(), 1e-15);
        EXPECT_TRUE((rtnToGCRF.transpose() * acceleration / thrustAcceleration_)
                        .isApprox(plan.deltaVelocities.accessFirst().normalized(), 1e-9));
        EXPECT_NEAR(0.0, rtnToGCRF.col(0).dot(acceleration), 1e-15);

        // The thrust magnitude follows the provided one
        EXPECT_NEAR(
            1.0, guidanceLaw.calculateThrustAccelerationAt(epoch_, position, velocity, 1.0, Frame::GCRF()).norm(), 1e-12
        );
    }

    // The threshold gates the thrust
    {
        const ROETargeting belowThreshold = {referenceTrajectory_, parametersWithThreshold(effectivity * 0.99)};

        EXPECT_FALSE(belowThreshold
                         .calculateThrustAccelerationAt(epoch_, position, velocity, thrustAcceleration_, Frame::GCRF())
                         .isZero());

        if (effectivity < 0.999)
        {
            const ROETargeting aboveThreshold = {
                referenceTrajectory_, parametersWithThreshold(std::min(1.0, double(effectivity) + 1e-3))
            };

            EXPECT_TRUE(
                aboveThreshold
                    .calculateThrustAccelerationAt(epoch_, position, velocity, thrustAcceleration_, Frame::GCRF())
                    .isZero()
            );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, CommitManeuverAt)
{
    const ROETargeting guidanceLaw = {referenceTrajectory_, parameters_};

    // On the reference, no burn to commit to
    {
        EXPECT_FALSE(guidanceLaw
                         .commitManeuverAt(
                             epoch_,
                             referenceState_.getPosition().accessCoordinates(),
                             referenceState_.getVelocity().accessCoordinates(),
                             Frame::GCRF()
                         )
                         .has_value());
    }

    // The first burn of the plan starting now: a constant thrust along its RTN direction, for its duration
    {
        const State state = trailingState(Length::Meters(1000.0));
        const Vector3d position = state.getPosition().accessCoordinates();
        const Vector3d velocity = state.getVelocity().accessCoordinates();

        const ROETargeting::Plan plan = guidanceLaw.computePlan(state);

        const std::optional<ROETargeting::CommittedManeuver> committedManeuver =
            guidanceLaw.commitManeuverAt(epoch_, position, velocity, Frame::GCRF());

        ASSERT_TRUE(committedManeuver.has_value());
        EXPECT_EQ(plan.burnIntervals.accessFirst().getDuration(), committedManeuver->duration);

        Matrix3d rtnToGCRF;
        rtnToGCRF.col(0) = position.normalized();
        rtnToGCRF.col(2) = position.cross(velocity).normalized();
        rtnToGCRF.col(1) = rtnToGCRF.col(2).cross(rtnToGCRF.col(0));

        const Vector3d acceleration = committedManeuver->guidanceLaw->calculateThrustAccelerationAt(
            epoch_, position, velocity, thrustAcceleration_, Frame::GCRF()
        );

        EXPECT_NEAR(thrustAcceleration_, acceleration.norm(), 1e-15);
        EXPECT_TRUE((rtnToGCRF.transpose() * acceleration / thrustAcceleration_)
                        .isApprox(plan.deltaVelocities.accessFirst().normalized(), 1e-9));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting, Segment)
{
    // A spacecraft trailing the reference by 1 km, in a maneuvering Segment with at most 10 minutes per maneuver and
    // at least one day between the end of a maneuver and the start of the next one
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

    const Propagator coastPropagator = {numericalSolver_, freeDynamics_};

    // The guidance law queries the reference at every evaluation: tabulate it
    const Duration duration = Duration::Days(2.2);
    const Array<State> referenceStates = coastPropagator.calculateStatesAt(
        referenceState_,
        Interval::Closed(epoch_, epoch_ + duration + Duration::Hours(1.0)).generateGrid(Duration::Seconds(30.0))
    );
    const Trajectory referenceTrajectory = Trajectory(Tabulated(referenceStates, 1, Interpolator::Type::CubicSpline));

    ROETargeting::Parameters parameters = parameters_;
    parameters.burnSpacing = Duration::Days(1.0) + Duration::Minutes(10.0);
    parameters.horizonBurnCount = 3;
    parameters.burnStartCandidateCount = 12;
    parameters.minimumBurnDuration = Duration::Seconds(5.0);

    const Shared<ROETargeting> guidanceLawSPtr = std::make_shared<ROETargeting>(referenceTrajectory, parameters);

    const Segment segment = Segment::Maneuver(
        "ROE Targeting",
        std::make_shared<InstantCondition>(InstantCondition::Criterion::AnyCrossing, epoch_ + duration),
        std::make_shared<Thruster>(satelliteSystem, guidanceLawSPtr),
        freeDynamics_,
        numericalSolver_,
        Segment::ManeuverConstraints(
            Duration::Undefined(),
            Duration::Minutes(10.0),
            Duration::Days(1.0),
            Segment::MaximumManeuverDurationViolationStrategy::TruncateEnd
        )
    );

    const State initialState = trailingState(Length::Meters(1000.0));

    const Segment::Solution solution = segment.solve(initialState, duration);

    const Array<Interval>& maneuverIntervals = solution.maneuverIntervals;

    ASSERT_GE(maneuverIntervals.getSize(), 2);

    for (Size maneuverIndex = 0; maneuverIndex < maneuverIntervals.getSize(); ++maneuverIndex)
    {
        EXPECT_LE(maneuverIntervals[maneuverIndex].getDuration(), Duration::Minutes(10.0));

        if (maneuverIndex > 0)
        {
            EXPECT_GE(
                maneuverIntervals[maneuverIndex].getStart() - maneuverIntervals[maneuverIndex - 1].getEnd(),
                Duration::Days(1.0)
            );
        }
    }

    // Closer to the reference than when coasting
    const State coastState = coastPropagator.calculateStateAt(initialState, epoch_ + duration);

    EXPECT_LT(
        guidanceLawSPtr->computePlan(solution.states.accessLast()).cost, guidanceLawSPtr->computePlan(coastState).cost
    );
}
