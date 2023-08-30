/// Apache License 2.0

#include <boost/numeric/odeint.hpp>

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth/Exponential.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Ephemerides/Analytical.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>
#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/Thruster/ConstantThrustThruster.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;
using ostk::core::types::String;

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
using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::dynamics::thruster::ConstantThrustThruster;

using namespace boost::numeric::odeint;

static const Derived::Unit GravitationalParameterSIUnit =
    Derived::Unit::GravitationalParameter(Length::Unit::Meter, Time::Unit::Second);

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Composite satelliteGeometry(Cuboid(
            {0.0, 0.0, 0.0},
            {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
            {1.0, 2.0, 3.0}
        ));

        const Scalar thrust_ = Scalar(1.0, PropulsionSystem::thrustSIUnit);
        const Scalar specificImpulse_ = Scalar(1000.0, PropulsionSystem::specificImpulseSIUnit);

        propulsionSystem_ = PropulsionSystem(
            thrust_,  // Thrust
            specificImpulse_  // Isp
        );

        satelliteSystem_ = {
            Mass::Kilograms(100.0),
            satelliteGeometry,
            Matrix3d::Identity(),
            1.2,
            2.1,
            propulsionSystem_,
        };

        localOrbitalFrameDirection_ = LocalOrbitalFrameDirection({1.0, 0.0, 0.0}, LocalOrbitalFrameFactory::VNC(Frame::GCRF()));

        startStateVector_.resize(7);
        startStateVector_ << 7000000.0, 0.0, 0.0, 0.0, 7546.05329, 0.0, 200.0;

        earthSPtr_ = std::make_shared<Celestial>(earth_);
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

    LocalOrbitalFrameDirection localOrbitalFrameDirection_ = LocalOrbitalFrameDirection::Undefined();

    PropulsionSystem propulsionSystem_ = PropulsionSystem::Undefined();
    SatelliteSystem satelliteSystem_ = SatelliteSystem::Undefined();

    NumericalSolver::StateVector startStateVector_;

    Shared<Celestial> earthSPtr_ = nullptr;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster, Constructor)
{
    {
        EXPECT_NO_THROW(ConstantThrustThruster constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_, "aThrusterDynamicsName"));
    }

    {
        EXPECT_NO_THROW(ConstantThrustThruster constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster, StreamOperator)
{
    {
        ConstantThrustThruster constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << constantThrustThrusterDynamics << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster, IsDefined)
{
    {
        ConstantThrustThruster constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);
        EXPECT_TRUE(constantThrustThrusterDynamics.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster, Print)
{
    {
        ConstantThrustThruster constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(constantThrustThrusterDynamics.print(std::cout, true));
        EXPECT_NO_THROW(constantThrustThrusterDynamics.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster, Getters)
{
    {
        ConstantThrustThruster constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);
        EXPECT_TRUE(constantThrustThrusterDynamics.getName() == String::Empty());
        EXPECT_TRUE(constantThrustThrusterDynamics.getSatelliteSystem() == satelliteSystem_);
        EXPECT_TRUE(constantThrustThrusterDynamics.getThrust() == satelliteSystem_.getPropulsionSystem().getThrust());
    }

    {
        const String thrusterDynamicsName = "aThrusterDynamicsName";
        ConstantThrustThruster constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_, thrusterDynamicsName);
        EXPECT_TRUE(constantThrustThrusterDynamics.getName() == thrusterDynamicsName);
        EXPECT_TRUE(constantThrustThrusterDynamics.getSatelliteSystem() == satelliteSystem_);
        EXPECT_TRUE(constantThrustThrusterDynamics.getThrust() == satelliteSystem_.getPropulsionSystem().getThrust());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_Thruster_ConstantThrustThruster, ComputeContribution)
{
    ConstantThrustThruster constantThrustThrusterDynamics(satelliteSystem_, localOrbitalFrameDirection_);
    const VectorXd contribution = constantThrustThrusterDynamics.computeContribution(startInstant_, startStateVector_.segment(3, 4), Frame::GCRF());

    EXPECT_EQ(4, contribution.size());
    EXPECT_GT(1e-15, contribution[0]);
    EXPECT_GT(1e-15, contribution[1]);
    EXPECT_GT(1e-15, contribution[2]);
    EXPECT_GT(1e-15, -0.000101972 - contribution[3]);
}
