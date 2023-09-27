/// Apache License 2.0

#include <boost/numeric/odeint.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Tuple.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Tuple;
using ostk::core::ctnr::Table;
using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::fs::File;
using ostk::core::fs::Path;

using ostk::math::geom::d3::objects::Composite;
using ostk::math::geom::d3::objects::Cuboid;
using ostk::math::geom::d3::objects::Point;
using ostk::math::obj::VectorXd;
using ostk::math::obj::Matrix3d;
using ostk::math::obj::Vector3d;

using ostk::physics::data::Scalar;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::env::obj::Celestial;
using ostk::physics::env::obj::celest::Earth;
using ostk::physics::env::obj::celest::Moon;
using ostk::physics::env::obj::celest::Sun;
using ostk::physics::env::ephem::Analytical;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::units::Mass;
using ostk::physics::units::Length;
using ostk::physics::units::Derived;
using ostk::physics::units::Time;
using ostk::physics::data::Direction;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;

using ostk::astro::trajectory::LocalOrbitalFrameDirection;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;
using ostk::astro::trajectory::LocalOrbitalFrameTransformProvider;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::dynamics::thruster::ConstantThrust;

using namespace boost::numeric::odeint;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        localOrbitalFrameDirection_ =
            LocalOrbitalFrameDirection({1.0, 0.0, 0.0}, LocalOrbitalFrameFactory::VNC(Frame::GCRF()));

        this->satelliteSystem_ = {
            satelliteDryMass_, satelliteGeometry, Matrix3d::Identity(), 1.2, 2.1, propulsionSystem_
        };

        startStateVector_.resize(7);
        startStateVector_ << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;
    }

    // Current state and instant setup, choose equinox as instant to make geometry simple
    // Earth pulls in the -X direction, Sun pulls in the +X direction, and Moon in the +Y direction
    const Instant startInstant_ = Instant::DateTime(DateTime(2021, 3, 20, 12, 0, 0), Scale::UTC);

    const Earth earth_ = {
        EarthGravitationalModel::Spherical.gravitationalParameter_,
        EarthGravitationalModel::Spherical.equatorialRadius_,
        EarthGravitationalModel::Spherical.flattening_,
        EarthGravitationalModel::Spherical.J2_,
        EarthGravitationalModel::Spherical.J4_,
        std::make_shared<Analytical>(Frame::ITRF()),
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Undefined),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential),
    };

    const Shared<const Frame> gcrfSPtr_ = Frame::GCRF();

    LocalOrbitalFrameDirection localOrbitalFrameDirection_ = LocalOrbitalFrameDirection::Undefined();

    const Scalar thrust_ = Scalar(0.1, PropulsionSystem::thrustSIUnit);
    const Scalar specificImpulse_ = Scalar(1500.0, PropulsionSystem::specificImpulseSIUnit);

    const Mass satelliteDryMass_ = Mass::Kilograms(100.0);

    const PropulsionSystem propulsionSystem_ = PropulsionSystem(
        thrust_,          // Thrust
        specificImpulse_  // Isp
    );

    NumericalSolver::StateVector startStateVector_;
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();
    Shared<Celestial> earthSPtr_ = std::make_shared<Celestial>(earth_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust, Constructor)
{
    {
        EXPECT_NO_THROW(ConstantThrust constantThrustThrusterDynamics(
            satelliteSystem_, localOrbitalFrameDirection_, "aThrusterDynamicsName"
        ));
    }

    {
        EXPECT_NO_THROW(ConstantThrust constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust, StreamOperator)
{
    {
        ConstantThrust constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << constantThrustThrusterDynamics << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust, IsDefined)
{
    {
        ConstantThrust constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);
        EXPECT_TRUE(constantThrustThrusterDynamics.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust, Print)
{
    {
        ConstantThrust constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(constantThrustThrusterDynamics.print(std::cout, true));
        EXPECT_NO_THROW(constantThrustThrusterDynamics.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust, Getters)
{
    {
        ConstantThrust constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);
        EXPECT_TRUE(constantThrustThrusterDynamics.getName() == String::Empty());
        EXPECT_TRUE(constantThrustThrusterDynamics.getSatelliteSystem() == satelliteSystem_);
        EXPECT_TRUE(constantThrustThrusterDynamics.getThrust() == satelliteSystem_.getPropulsionSystem().getThrust());
        EXPECT_TRUE(constantThrustThrusterDynamics.getLocalThrustDirection() == localOrbitalFrameDirection_);
    }

    {
        const String thrusterDynamicsName = "aThrusterDynamicsName";
        ConstantThrust constantThrustThrusterDynamics(
            satelliteSystem_, localOrbitalFrameDirection_, thrusterDynamicsName
        );
        EXPECT_TRUE(constantThrustThrusterDynamics.getName() == thrusterDynamicsName);
        EXPECT_TRUE(constantThrustThrusterDynamics.getSatelliteSystem() == satelliteSystem_);
        EXPECT_TRUE(constantThrustThrusterDynamics.getThrust() == satelliteSystem_.getPropulsionSystem().getThrust());
    }
}

/* Contribution computation validation test */
TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust, ComputeContribution)
{
    {
        // Test Case (thrust direction, local orbital frame, reference data)
        const Array<Tuple<Shared<const LocalOrbitalFrameFactory>, Vector3d, String>> testCases = {
            {LocalOrbitalFrameFactory::VNC(gcrfSPtr_),
             Vector3d({1.0, 0.0, 0.0}),
             "Orekit_ConstantThrustManeuver_VNC_inclined_1hr_run.csv"},
            {LocalOrbitalFrameFactory::QSW(gcrfSPtr_),
             Vector3d({0.0, 1.0, 0.0}),
             "Orekit_ConstantThrustManeuver_QSW_inclined_1hr_run.csv"},
            {LocalOrbitalFrameFactory::TNW(gcrfSPtr_),
             Vector3d({1.0, 0.0, 0.0}),
             "Orekit_ConstantThrustManeuver_TNW_inclined_1hr_run.csv"},
            {LocalOrbitalFrameFactory::LVLH(gcrfSPtr_),
             Vector3d({1.0, 0.0, 0.0}),
             "Orekit_ConstantThrustManeuver_LVLH_inclined_1hr_run.csv"},
            {LocalOrbitalFrameFactory::VVLH(gcrfSPtr_),
             Vector3d({1.0, 0.0, 0.0}),
             "Orekit_ConstantThrustManeuver_VVLH_inclined_1hr_run.csv"}
        };

        // Loop through test cases
        for (const auto testCase : testCases)
        {
            // Extract test case input data
            const Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactory = std::get<0>(testCase);
            const Vector3d localOrbitalFrameThrustVector = std::get<1>(testCase);
            const String referenceDataFileName = std::get<2>(testCase);

            // Define constant thrust thruster dynamics
            LocalOrbitalFrameDirection localOrbitalFrameDirection =
                LocalOrbitalFrameDirection(localOrbitalFrameThrustVector, localOrbitalFrameFactory);
            Shared<ConstantThrust> thrusterDynamicsSPtr =
                std::make_shared<ConstantThrust>(satelliteSystem_, localOrbitalFrameDirection);

            // Reference data setup
            const Table referenceData = Table::Load(
                File::Path(Path::Parse(
                    "/app/test/OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/" + referenceDataFileName
                )),
                Table::Format::CSV,
                true
            );

            // Initialize reference data arrays
            Array<Instant> instantArray = Array<Instant>::Empty();
            Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
            Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
            Array<double> referenceMassArray = Array<double>::Empty();
            Array<Vector3d> referenceManeuverAccelerationArrayGCRF = Array<Vector3d>::Empty();

            for (const auto& referenceRow : referenceData)
            {
                instantArray.add(Instant::DateTime(
                    DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
                ));
                referencePositionArrayGCRF.add(
                    Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
                );
                referenceVelocityArrayGCRF.add(
                    Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
                );
                referenceManeuverAccelerationArrayGCRF.add(Vector3d(
                    referenceRow[16].accessReal(), referenceRow[17].accessReal(), referenceRow[18].accessReal()
                ));
                referenceMassArray.add(referenceRow[22].accessReal());
            }

            // Validation loop
            for (size_t i = 1; i < instantArray.getSize() - 1; i++)
            {
                // Current state and instant setup from Orekit test case
                VectorXd OrekitStateCoordinates(7);
                OrekitStateCoordinates << referencePositionArrayGCRF[i], referenceVelocityArrayGCRF[i],
                    referenceMassArray[i];  // Check size input Dynamics

                const VectorXd maneuverContributionFromOrekitStateGCRF =
                    thrusterDynamicsSPtr->computeContribution(instantArray[i], OrekitStateCoordinates, gcrfSPtr_);

                const double referenceStepSize = 30.0;

                const double maneuverComputationErrorAccelerationContributionXGCRF =
                    std::abs(maneuverContributionFromOrekitStateGCRF[0] - referenceManeuverAccelerationArrayGCRF[i][0]);
                const double maneuverComputationErrorAccelerationContributionYGCRF =
                    std::abs(maneuverContributionFromOrekitStateGCRF[1] - referenceManeuverAccelerationArrayGCRF[i][1]);
                const double maneuverComputationErrorAccelerationContributionZGCRF =
                    std::abs(maneuverContributionFromOrekitStateGCRF[2] - referenceManeuverAccelerationArrayGCRF[i][2]);
                const double maneuverComputationErrorMassContributionGCRF = std::abs(
                    maneuverContributionFromOrekitStateGCRF[3] -
                    ((referenceMassArray[i + 1] - referenceMassArray[i]) / referenceStepSize)
                );
                const double maneuverComputationAccelerationContributionErrorGCRF =
                    (maneuverContributionFromOrekitStateGCRF.segment(0, 3) - referenceManeuverAccelerationArrayGCRF[i])
                        .norm();

                ASSERT_GT(5e-10, maneuverComputationErrorAccelerationContributionXGCRF);
                ASSERT_GT(5e-10, maneuverComputationErrorAccelerationContributionYGCRF);
                ASSERT_GT(5e-10, maneuverComputationErrorAccelerationContributionZGCRF);
                ASSERT_GT(1e-9, maneuverComputationErrorMassContributionGCRF);
                ASSERT_GT(1e-9, maneuverComputationAccelerationContributionErrorGCRF);

                // Results console output
                // std::cout << "**************************************" << std::endl;
                // std::cout.setf(std::ios::scientific,std::ios::floatfield);
                // std::cout << "Instant is: " << instantArray[i] << std::endl;
                // std::cout << "Maneuver contribution computation X GCRF error is: " <<
                // maneuverComputationErrorAccelerationContributionXGCRF << "m/s^2" << std::endl; std::cout << "Maneuver
                // contribution computation Y GCRF error is: " << maneuverComputationErrorAccelerationContributionYGCRF
                // << "m/s^2" << std::endl; std::cout << "Maneuver contribution computation Z GCRF error is: " <<
                // maneuverComputationErrorAccelerationContributionZGCRF << "m/s^2" << std::endl; std::cout << "Maneuver
                // contribution computation Mass error is: " << maneuverComputationErrorMassContributionGCRF << "kg" <<
                // std::endl; std::cout << "Maneuver contribution computation Acceleration GCRF Norm error is: " <<
                // maneuverComputationAccelerationContributionErrorGCRF << "m/s^2" << std::endl;
                // std::cout.setf(std::ios::fixed,std::ios::floatfield);
                // std::cout << "**************************************" << std::endl;
            }
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrust, Intrack)
{
    {
        EXPECT_NO_THROW(ConstantThrust::Intrack(satelliteSystem_));
    }

    {
        ConstantThrust constantThrust = ConstantThrust::Intrack(satelliteSystem_, true);
        Vector3d direction = {1.0, 0.0, 0.0};
        EXPECT_TRUE(constantThrust.getLocalThrustDirection().getValue() == direction);
        EXPECT_TRUE(constantThrust.getLocalThrustDirection().accessLocalOrbitalFrameFactory()->getProviderType() == LocalOrbitalFrameTransformProvider::Type::VNC);
    }

    {
        Vector3d direction = {-1.0, 0.0, 0.0};
        ConstantThrust constantThrust = ConstantThrust::Intrack(satelliteSystem_, false);
        EXPECT_TRUE(constantThrust.getLocalThrustDirection().getValue() == direction);
        EXPECT_TRUE(constantThrust.getLocalThrustDirection().accessLocalOrbitalFrameFactory()->getProviderType() == LocalOrbitalFrameTransformProvider::Type::VNC);
    }

    {
        EXPECT_NO_THROW(ConstantThrust::Intrack(satelliteSystem_, true, Frame::GCRF()));
    }

}