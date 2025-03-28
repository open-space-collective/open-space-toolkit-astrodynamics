/// Apache License 2.0

#include <unordered_set>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationVector.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Transform.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Pair;
using ostk::core::container::Table;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::geometry::d3::transformation::rotation::RotationVector;
using ostk::mathematics::object::Vector3d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::time::Time;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::flight::Profile;
using ostk::astrodynamics::flight::profile::model::Tabulated;
using ostk::astrodynamics::flight::profile::model::Transform;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Flight_Profile : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        this->orbit_ = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        this->profile_ = Profile::LocalOrbitalFramePointing(this->orbit_, Orbit::FrameType::VVLH);
    }

    Orbit orbit_ = Orbit::Undefined();
    Profile profile_ = Profile::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, Constructor)
{
    {
        EXPECT_NO_THROW(Profile {Transform::Undefined()};);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, CopyConstructor)
{
    {
        EXPECT_NO_THROW(Profile profile(profile_););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, CopyAssignmentOperator)
{
    {
        EXPECT_NO_THROW(Profile profile = profile_;);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << profile_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }

    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << Profile::Undefined() << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, IsDefined)
{
    {
        EXPECT_TRUE(profile_.isDefined());
    }

    {
        EXPECT_FALSE(Profile::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, AccessModel)
{
    {
        EXPECT_TRUE(profile_.accessModel().isDefined());
    }

    {
        EXPECT_THROW(Profile::Undefined().accessModel(), ostk::core::error::runtime::Undefined);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, GetStateAt)
{
    {
        const State state = profile_.getStateAt(Instant::J2000());

        EXPECT_EQ(Instant::J2000(), state.getInstant());
        EXPECT_TRUE(state.getPosition().getCoordinates().isNear(Vector3d {1220971.34506, -6892693.88371, 0.0}, 1e-5));
        EXPECT_TRUE(state.getVelocity().getCoordinates().isNear(Vector3d {7430.37647704, 1316.21640458, 0.0}, 1e-5));
        EXPECT_TRUE(state.getAttitude().isNear(
            Quaternion::XYZS(-0.704392, -0.061906, 0.061906, 0.704392).toNormalized(), Angle::Degrees(1e-3)
        ));
        EXPECT_TRUE(state.getAngularVelocity().isNear(Vector3d {0.0, -0.00107800762584, 0.0}, 1e-5));
        EXPECT_EQ(Frame::GCRF(), state.getFrame());
    }

    {
        EXPECT_ANY_THROW(Profile::Undefined().getStateAt(Instant::Undefined()));
        EXPECT_ANY_THROW(Profile::Undefined().getStateAt(Instant::J2000()));
        EXPECT_ANY_THROW(profile_.getStateAt(Instant::Undefined()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, GetStatesAt)
{
    {
        const Array<Instant> referenceInstants = {
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 0), Scale::UTC),
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
            Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1, 0), Scale::UTC),
        };

        const Array<State> referenceStates = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({7000000.000000000000, 0.000000000000, 0.000000000000}, Frame::GCRF()),
             Velocity::MetersPerSecond({0.000000000000, 7546.053287267836, 0.000000000000}, Frame::GCRF()),
             Quaternion::XYZS(-0.500000000000, -0.500000000000, 0.500000000000, 0.500000000000),
             {0.000000000000, -0.001078007612, 0.000000000000},
             Frame::GCRF()},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
             Position::Meters({6999998.983162164688, 3773.026460940484, 0.000000000000}, Frame::GCRF()),
             Velocity::MetersPerSecond({-4.067351246933, 7546.052191108910, 0.000000000000}, Frame::GCRF()),
             Quaternion::XYZS(-0.499865230892, -0.500134732792, 0.500134732792, 0.499865230892),
             {0.000000000000, -0.001078007612, 0.000000000000},
             Frame::GCRF()},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({6999995.932648953050, 7546.051825722679, 0.000000000000}, Frame::GCRF()),
             Velocity::MetersPerSecond({-8.134701312198, 7546.048902632449, 0.000000000000}, Frame::GCRF()),
             Quaternion::XYZS(-0.499730425479, -0.500269429259, 0.500269429259, 0.499730425479),
             {0.000000000000, -0.001078007612, 0.000000000000},
             Frame::GCRF()}
        };

        const Array<State> states = profile_.getStatesAt(referenceInstants);

        EXPECT_EQ(referenceStates.getSize(), states.getSize());

        for (const auto stateTuple : ostk::core::container::iterator::Zip(referenceStates, states))
        {
            const State& referenceState = std::get<0>(stateTuple);
            const State& state = std::get<1>(stateTuple);

            EXPECT_EQ(state.getInstant(), referenceState.getInstant());
            EXPECT_TRUE(state.getPosition().getCoordinates().isNear(referenceState.getPosition().getCoordinates(), 1e-5)
            );
            EXPECT_TRUE(state.getVelocity().getCoordinates().isNear(referenceState.getVelocity().getCoordinates(), 1e-5)
            );
            EXPECT_TRUE(state.getAttitude().isNear(referenceState.getAttitude().toNormalized(), Angle::Degrees(1e-3)));
            EXPECT_TRUE(state.getAngularVelocity().isNear(referenceState.getAngularVelocity(), 1e-5));
            EXPECT_EQ(state.getFrame(), referenceState.getFrame());
        }
    }

    {
        EXPECT_ANY_THROW(Profile::Undefined().getStatesAt(
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC)}
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, GetBodyFrame)
{
    {
        EXPECT_NO_THROW(profile_.getBodyFrame("name"));
        EXPECT_THROW(
            profile_.getBodyFrame("name"), ostk::core::error::RuntimeError
        );  // Frame with same name already exists
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, Undefined)
{
    {
        EXPECT_NO_THROW(Profile::Undefined());
        EXPECT_FALSE(Profile::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, InertialPointing)
{
    {
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        const Profile profile = Profile::InertialPointing(orbit, Quaternion::Unit());

        const Real positionTolerance_m = 1e-3;
        const Real velocityTolerance_meterPerSec = 1e-6;
        const Real angularTolerance_asec = 0.0;
        const Real angularVelocityTolerance_radPerSec = 1e-10;

        // Reference data setup

        const File referenceDataFile =
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/InertialPointing/Satellite "
                                   "t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant_ref =
                Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC);

            const Vector3d x_BODY_GCRF_ref = {
                referenceRow["x (m)"].accessReal(),
                referenceRow["y (m)"].accessReal(),
                referenceRow["z (m)"].accessReal()
            };
            const Vector3d v_BODY_GCRF_in_GCRF_ref = {
                referenceRow["vx (m/sec)"].accessReal(),
                referenceRow["vy (m/sec)"].accessReal(),
                referenceRow["vz (m/sec)"].accessReal()
            };
            const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(
                                                   referenceRow["q1"].accessReal(),
                                                   referenceRow["q2"].accessReal(),
                                                   referenceRow["q3"].accessReal(),
                                                   referenceRow["q4"].accessReal()
            )
                                                   .normalize();
            const Vector3d w_BODY_GCRF_in_BODY_ref = {
                referenceRow["wx (rad/sec)"].accessReal(),
                referenceRow["wy (rad/sec)"].accessReal(),
                referenceRow["wz (rad/sec)"].accessReal()
            };

            const State state = profile.getStateAt(instant_ref);

            const Vector3d x_BODY_GCRF = state.getPosition().getCoordinates();
            const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity().getCoordinates();
            const Quaternion q_BODY_GCRF = state.getAttitude();
            const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity();

            ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format(
                "@ {}: {} - {} = {} [m]",
                instant_ref.toString(),
                x_BODY_GCRF.toString(),
                x_BODY_GCRF_ref.toString(),
                (x_BODY_GCRF - x_BODY_GCRF_ref).norm()
            );
            ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec))
                << String::Format(
                       "@ {}: {} - {} = {} [m/s]",
                       instant_ref.toString(),
                       v_BODY_GCRF_in_GCRF.toString(),
                       v_BODY_GCRF_in_GCRF_ref.toString(),
                       (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()
                   );
            ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec)))
                << String::Format(
                       "@ {}: {} / {} = {} [asec]",
                       instant_ref.toString(),
                       q_BODY_GCRF_ref.toString(),
                       q_BODY_GCRF.toString(),
                       q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()
                   );
            ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec))
                << String::Format(
                       "@ {}: {} - {} = {} [rad/s]",
                       instant_ref.toString(),
                       w_BODY_GCRF_in_BODY_ref.toString(),
                       w_BODY_GCRF_in_BODY.toString(),
                       (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()
                   );
        }
    }
}

// TBI: Should move these tests to the Transform test suite and only test interface here
TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, LocalOrbitalFramePointing_VVLH)
{
    // VVLH #1

    {
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        const Profile profile = Profile::LocalOrbitalFramePointing(orbit, Orbit::FrameType::VVLH);

        const Real positionTolerance_m = 1e-3;
        const Real velocityTolerance_meterPerSec = 1e-6;
        const Real angularTolerance_asec = 0.0;
        const Real angularVelocityTolerance_radPerSec = 1e-10;

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/LocalOrbitalFramePointing/VVLH/"
                        "Satellite_1 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")
        );

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant_ref =
                Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC);

            const Vector3d x_BODY_GCRF_ref = {
                referenceRow["x (m)"].accessReal(),
                referenceRow["y (m)"].accessReal(),
                referenceRow["z (m)"].accessReal()
            };
            const Vector3d v_BODY_GCRF_in_GCRF_ref = {
                referenceRow["vx (m/sec)"].accessReal(),
                referenceRow["vy (m/sec)"].accessReal(),
                referenceRow["vz (m/sec)"].accessReal()
            };
            const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(
                                                   referenceRow["q1"].accessReal(),
                                                   referenceRow["q2"].accessReal(),
                                                   referenceRow["q3"].accessReal(),
                                                   referenceRow["q4"].accessReal()
            )
                                                   .normalize();
            const Vector3d w_BODY_GCRF_in_BODY_ref = {
                referenceRow["wx (rad/sec)"].accessReal(),
                referenceRow["wy (rad/sec)"].accessReal(),
                referenceRow["wz (rad/sec)"].accessReal()
            };

            const State state = profile.getStateAt(instant_ref);

            const Vector3d x_BODY_GCRF = state.getPosition().getCoordinates();
            const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity().getCoordinates();
            const Quaternion q_BODY_GCRF = state.getAttitude();
            const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity();

            ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format(
                "@ {}: {} - {} = {} [m]",
                instant_ref.toString(),
                x_BODY_GCRF.toString(),
                x_BODY_GCRF_ref.toString(),
                (x_BODY_GCRF - x_BODY_GCRF_ref).norm()
            );
            ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec))
                << String::Format(
                       "@ {}: {} - {} = {} [m/s]",
                       instant_ref.toString(),
                       v_BODY_GCRF_in_GCRF.toString(),
                       v_BODY_GCRF_in_GCRF_ref.toString(),
                       (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()
                   );
            ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec)))
                << String::Format(
                       "@ {}: {} / {} = {} [asec]",
                       instant_ref.toString(),
                       q_BODY_GCRF_ref.toString(),
                       q_BODY_GCRF.toString(),
                       q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()
                   );
            ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec))
                << String::Format(
                       "@ {}: {} - {} = {} [rad/s]",
                       instant_ref.toString(),
                       w_BODY_GCRF_in_BODY_ref.toString(),
                       w_BODY_GCRF_in_BODY.toString(),
                       (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()
                   );
        }
    }

    // VVLH #2

    {
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        const Profile profile = Profile::LocalOrbitalFramePointing(orbit, Orbit::FrameType::VVLH);

        const Real positionTolerance_m = 1e-3;
        const Real velocityTolerance_meterPerSec = 1e-6;
        const Real angularTolerance_asec = 0.0;
        const Real angularVelocityTolerance_radPerSec = 1e-10;

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/LocalOrbitalFramePointing/VVLH/"
                        "Satellite_2 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")
        );

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant_ref =
                Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC);

            const Vector3d x_BODY_GCRF_ref = {
                referenceRow["x (m)"].accessReal(),
                referenceRow["y (m)"].accessReal(),
                referenceRow["z (m)"].accessReal()
            };
            const Vector3d v_BODY_GCRF_in_GCRF_ref = {
                referenceRow["vx (m/sec)"].accessReal(),
                referenceRow["vy (m/sec)"].accessReal(),
                referenceRow["vz (m/sec)"].accessReal()
            };
            const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(
                                                   referenceRow["q1"].accessReal(),
                                                   referenceRow["q2"].accessReal(),
                                                   referenceRow["q3"].accessReal(),
                                                   referenceRow["q4"].accessReal()
            )
                                                   .normalize();
            const Vector3d w_BODY_GCRF_in_BODY_ref = {
                referenceRow["wx (rad/sec)"].accessReal(),
                referenceRow["wy (rad/sec)"].accessReal(),
                referenceRow["wz (rad/sec)"].accessReal()
            };

            const State state = profile.getStateAt(instant_ref);

            const Vector3d x_BODY_GCRF = state.getPosition().getCoordinates();
            const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity().getCoordinates();
            const Quaternion q_BODY_GCRF = state.getAttitude();
            const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity();

            ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format(
                "@ {}: {} - {} = {} [m]",
                instant_ref.toString(),
                x_BODY_GCRF.toString(),
                x_BODY_GCRF_ref.toString(),
                (x_BODY_GCRF - x_BODY_GCRF_ref).norm()
            );
            ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec))
                << String::Format(
                       "@ {}: {} - {} = {} [m/s]",
                       instant_ref.toString(),
                       v_BODY_GCRF_in_GCRF.toString(),
                       v_BODY_GCRF_in_GCRF_ref.toString(),
                       (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()
                   );
            ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec)))
                << String::Format(
                       "@ {}: {} / {} = {} [asec]",
                       instant_ref.toString(),
                       q_BODY_GCRF_ref.toString(),
                       q_BODY_GCRF.toString(),
                       q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()
                   );
            ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec))
                << String::Format(
                       "@ {}: {} - {} = {} [rad/s]",
                       instant_ref.toString(),
                       w_BODY_GCRF_in_BODY_ref.toString(),
                       w_BODY_GCRF_in_BODY.toString(),
                       (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()
                   );
        }
    }

    // VVLH #3

    {
        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.05;
        const Angle inclination = Angle::Degrees(45.0);
        const Angle raan = Angle::Degrees(10.0);
        const Angle aop = Angle::Degrees(20.0);
        const Angle trueAnomaly = Angle::Degrees(30.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        const Profile profile = Profile::LocalOrbitalFramePointing(orbit, Orbit::FrameType::VVLH);

        const Real positionTolerance_m = 1e-3;
        const Real velocityTolerance_meterPerSec = 1e-6;
        const Real angularTolerance_asec = 0.0;
        const Real angularVelocityTolerance_radPerSec = 1e-7;

        // Reference data setup

        const File referenceDataFile = File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/LocalOrbitalFramePointing/VVLH/"
                        "Satellite_3 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")
        );

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant_ref =
                Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC);

            const Vector3d x_BODY_GCRF_ref = {
                referenceRow["x (m)"].accessReal(),
                referenceRow["y (m)"].accessReal(),
                referenceRow["z (m)"].accessReal()
            };
            const Vector3d v_BODY_GCRF_in_GCRF_ref = {
                referenceRow["vx (m/sec)"].accessReal(),
                referenceRow["vy (m/sec)"].accessReal(),
                referenceRow["vz (m/sec)"].accessReal()
            };
            const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(
                                                   referenceRow["q1"].accessReal(),
                                                   referenceRow["q2"].accessReal(),
                                                   referenceRow["q3"].accessReal(),
                                                   referenceRow["q4"].accessReal()
            )
                                                   .normalize();
            const Vector3d w_BODY_GCRF_in_BODY_ref = {
                referenceRow["wx (rad/sec)"].accessReal(),
                referenceRow["wy (rad/sec)"].accessReal(),
                referenceRow["wz (rad/sec)"].accessReal()
            };

            const State state = profile.getStateAt(instant_ref);

            const Vector3d x_BODY_GCRF = state.getPosition().getCoordinates();
            const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity().getCoordinates();
            const Quaternion q_BODY_GCRF = state.getAttitude();
            const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity();

            ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format(
                "@ {}: {} - {} = {} [m]",
                instant_ref.toString(),
                x_BODY_GCRF.toString(),
                x_BODY_GCRF_ref.toString(),
                (x_BODY_GCRF - x_BODY_GCRF_ref).norm()
            );
            ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec))
                << String::Format(
                       "@ {}: {} - {} = {} [m/s]",
                       instant_ref.toString(),
                       v_BODY_GCRF_in_GCRF.toString(),
                       v_BODY_GCRF_in_GCRF_ref.toString(),
                       (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()
                   );
            ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec)))
                << String::Format(
                       "@ {}: {} / {} = {} [asec]",
                       instant_ref.toString(),
                       q_BODY_GCRF_ref.toString(),
                       q_BODY_GCRF.toString(),
                       q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()
                   );
            ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec))
                << String::Format(
                       "@ {}: {} - {} = {} [rad/s]",
                       instant_ref.toString(),
                       w_BODY_GCRF_in_BODY_ref.toString(),
                       w_BODY_GCRF_in_BODY.toString(),
                       (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()
                   );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, Tabulated)
{
    {
        const Array<State> tabulatedStates = {
            {
                Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC),
                Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF()),
                Quaternion::RotationVector(RotationVector::X(Angle::Degrees(0.0))),
                {0.0, 0.0, 1.0},
                Frame::GCRF(),
            },
            {
                Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 0), Scale::UTC),
                Position::Meters({2.0, 3.0, 4.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({3.0, 4.0, 5.0}, Frame::GCRF()),
                Quaternion::RotationVector(RotationVector::X(Angle::Degrees(10.0))),
                {0.0, 0.0, 2.0},
                Frame::GCRF(),
            },
            {
                Instant::DateTime(DateTime(2020, 1, 1, 0, 2, 0), Scale::UTC),
                Position::Meters({4.0, 5.0, 6.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({1.0, 2.0, 3.0}, Frame::GCRF()),
                Quaternion::RotationVector(RotationVector::X(Angle::Degrees(30.0))),
                {0.0, 0.0, 4.0},
                Frame::GCRF(),
            },
        };

        const Tabulated tabulated = {tabulatedStates};

        const Profile profile = {tabulated};

        const Array<Instant> referenceInstants = {
            Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC),
            Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 30), Scale::UTC),
            Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 0), Scale::UTC),
            Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 30), Scale::UTC),
            Instant::DateTime(DateTime(2020, 1, 1, 0, 2, 0), Scale::UTC),
        };

        const Array<State> states = profile.getStatesAt(referenceInstants);

        const Array<State> referenceStates = {
            {
                Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC),
                Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF()),
                Quaternion::RotationVector(RotationVector::X(Angle::Degrees(0.0))),
                {0.0, 0.0, 1.0},
                Frame::GCRF(),
            },
            {
                Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 30), Scale::UTC),
                Position::Meters({1.5, 2.5, 3.5}, Frame::GCRF()),
                Velocity::MetersPerSecond({3.5, 4.5, 5.5}, Frame::GCRF()),
                Quaternion::RotationVector(RotationVector::X(Angle::Degrees(5.0))),
                {0.0, 0.0, 1.5},
                Frame::GCRF(),
            },
            {
                Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 0), Scale::UTC),
                Position::Meters({2.0, 3.0, 4.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({3.0, 4.0, 5.0}, Frame::GCRF()),
                Quaternion::RotationVector(RotationVector::X(Angle::Degrees(10.0))),
                {0.0, 0.0, 2.0},
                Frame::GCRF(),
            },
            {
                Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 30), Scale::UTC),
                Position::Meters({3.0, 4.0, 5.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({2.0, 3.0, 4.0}, Frame::GCRF()),
                Quaternion::RotationVector(RotationVector::X(Angle::Degrees(20.0))),
                {0.0, 0.0, 3.0},
                Frame::GCRF(),
            },
            {
                Instant::DateTime(DateTime(2020, 1, 1, 0, 2, 0), Scale::UTC),
                Position::Meters({4.0, 5.0, 6.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({1.0, 2.0, 3.0}, Frame::GCRF()),
                Quaternion::RotationVector(RotationVector::X(Angle::Degrees(30.0))),
                {0.0, 0.0, 4.0},
                Frame::GCRF(),
            },
        };

        EXPECT_EQ(referenceStates.getSize(), states.getSize());

        for (const auto stateTuple : ostk::core::container::iterator::Zip(referenceStates, states))
        {
            const State& referenceState = std::get<0>(stateTuple);
            const State& state = std::get<1>(stateTuple);

            EXPECT_EQ(state.getInstant(), referenceState.getInstant());
            EXPECT_TRUE(state.getPosition().getCoordinates().isNear(referenceState.getPosition().getCoordinates(), 1e-5)
            ) << state
              << referenceState;
            EXPECT_TRUE(state.getVelocity().getCoordinates().isNear(referenceState.getVelocity().getCoordinates(), 1e-5)
            );
            EXPECT_TRUE(state.getAttitude().isNear(referenceState.getAttitude().toNormalized(), Angle::Degrees(1e-3)));
            EXPECT_TRUE(state.getAngularVelocity().isNear(referenceState.getAngularVelocity(), 1e-5));
            EXPECT_EQ(state.getFrame(), referenceState.getFrame());
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, Target)
{
    {
        EXPECT_NO_THROW(Profile::Target(Profile::TargetType::GeocentricNadir, Profile::Axis::X));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, TrajectoryTarget)
{
    {
        EXPECT_THROW(
            Profile::TrajectoryTarget(Trajectory::Undefined(), Profile::Axis::X), ostk::core::error::runtime::Undefined
        );
    }

    {
        const Trajectory trajectory = Trajectory::Position(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()));
        EXPECT_NO_THROW(Profile::TrajectoryTarget(trajectory, Profile::Axis::X));
        EXPECT_NO_THROW(Profile::TrajectoryTarget(trajectory, Profile::Axis::X, true));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, CustomTarget)
{
    {
        std::function<Vector3d(const State&)> orientationGenerator = [](const State&) -> Vector3d
        {
            return Vector3d::X();
        };

        {
            EXPECT_NO_THROW(Profile::CustomTarget(orientationGenerator, Profile::Axis::X));
        }

        {
            EXPECT_NO_THROW(Profile::CustomTarget(orientationGenerator, Profile::Axis::X, true));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, OrientationProfileTarget)
{
    {
        EXPECT_THROW(Profile::OrientationProfileTarget({}, Profile::Axis::X), ostk::core::error::runtime::Undefined);
    }

    {
        const Array<Pair<Instant, Vector3d>> orientationProfile = {
            {Instant::J2000(), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(10.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(20.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(30.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(.0), Vector3d::X()},
        };
        EXPECT_NO_THROW(Profile::OrientationProfileTarget(orientationProfile, Profile::Axis::X));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, AlignAndConstrain)
{
    {
        EXPECT_THROW(
            Profile::AlignAndConstrain(
                std::make_shared<const Profile::Target>(Profile::TargetType::VelocityECEF, Profile::Axis::X),
                std::make_shared<const Profile::Target>(Profile::TargetType::GeocentricNadir, Profile::Axis::Y)
            ),
            ostk::core::error::runtime::ToBeImplemented
        );
    }

    {
        EXPECT_THROW(
            Profile::AlignAndConstrain(
                std::make_shared<const Profile::Target>(Profile::TargetType::GeocentricNadir, Profile::Axis::Y),
                std::make_shared<const Profile::Target>(Profile::TargetType::VelocityECEF, Profile::Axis::X)
            ),
            ostk::core::error::runtime::ToBeImplemented
        );
    }

    {
        const Instant epoch = Instant::J2000();

        const Shared<Earth> earthSPtr = std::make_shared<Earth>(Earth::Default());

        const Orbit orbit = Orbit::SunSynchronous(epoch, Length::Kilometers(500.0), Time(6, 0, 0), earthSPtr);

        const State state = orbit.getStateAt(epoch);

        // Trajectory
        {
            const Trajectory trajectory = Trajectory::Position(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()));
            const auto orientation = Profile::AlignAndConstrain(
                std::make_shared<Profile::TrajectoryTarget>(trajectory, Profile::Axis::X, false),
                std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Profile::Axis::Y)
            );

            const Quaternion q_B_GCRF = orientation(state);

            EXPECT_VECTORS_ALMOST_EQUAL(
                q_B_GCRF * -state.getPosition().getCoordinates().normalized(), Vector3d::X(), 1e-12
            );
        }

        // Orientation profile
        {
            const Array<Pair<Instant, Vector3d>> orientationProfile = {
                {Instant::J2000(), Vector3d::X()},
                {Instant::J2000() + Duration::Seconds(10.0), Vector3d::X()},
                {Instant::J2000() + Duration::Seconds(20.0), Vector3d::X()},
                {Instant::J2000() + Duration::Seconds(30.0), Vector3d::X()},
                {Instant::J2000() + Duration::Seconds(40.0), Vector3d::X()},
            };

            const auto orientation = Profile::AlignAndConstrain(
                std::make_shared<Profile::OrientationProfileTarget>(orientationProfile, Profile::Axis::X),
                std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Profile::Axis::Y)
            );

            const Quaternion q_B_GCRF = orientation(state);

            EXPECT_VECTORS_ALMOST_EQUAL(q_B_GCRF * Vector3d::X(), Vector3d::X(), 1e-12);
        }

        // Custom profile
        {
            std::function<Vector3d(const State&)> orientationGenerator = [](const State&) -> Vector3d
            {
                return Vector3d::X();
            };

            const auto orientation = Profile::AlignAndConstrain(
                std::make_shared<Profile::CustomTarget>(orientationGenerator, Profile::Axis::X, false),
                std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Profile::Axis::Y)
            );

            const Quaternion q_B_GCRF = orientation(state);

            EXPECT_VECTORS_ALMOST_EQUAL(q_B_GCRF * Vector3d::X(), Vector3d::X(), 1e-12);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, CustomPointing)
{
    const Instant epoch = Instant::J2000();

    const Shared<Earth> earthSPtr = std::make_shared<Earth>(Earth::Default());

    const Orbit orbit = Orbit::SunSynchronous(epoch, Length::Kilometers(500.0), Time(6, 0, 0), earthSPtr);

    const Shared<const Profile::Target> alignmentTargetSPtr =
        std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Profile::Axis::X);

    const Shared<const Profile::Target> clockingTargetSPtr =
        std::make_shared<Profile::Target>(Profile::TargetType::OrbitalMomentum, Profile::Axis::Y);

    {
        // VNC frame

        const auto orientation = Profile::AlignAndConstrain(alignmentTargetSPtr, clockingTargetSPtr);

        const Profile calculatedProfile = Profile::CustomPointing(orbit, orientation);

        // Compare against Orbit VNC frame which has been validated

        const Profile expectedProfile = Profile::LocalOrbitalFramePointing(orbit, Orbit::FrameType::VNC);

        for (const auto instant :
             Interval::Closed(epoch, epoch + Duration::Hours(1.0)).generateGrid(Duration::Minutes(5.0)))
        {
            const State calculatedState = calculatedProfile.getStateAt(instant);
            const State expectedState = expectedProfile.getStateAt(instant);

            EXPECT_VECTORS_ALMOST_EQUAL(
                calculatedState.getPosition().getCoordinates(), expectedState.getPosition().getCoordinates(), 1e-6
            );
            EXPECT_VECTORS_ALMOST_EQUAL(
                calculatedState.getVelocity().getCoordinates(), expectedState.getVelocity().getCoordinates(), 1e-6
            );
            EXPECT_TRUE(calculatedState.getAttitude().isNear(expectedState.getAttitude(), Angle::Arcseconds(1e-12)));
        }
    }

    // Self consistent test for the interface function
    {
        const auto orientation = Profile::AlignAndConstrain(alignmentTargetSPtr, clockingTargetSPtr);

        const Profile expectedProfile = Profile::CustomPointing(orbit, orientation);

        const Profile calculatedProfile = Profile::CustomPointing(orbit, alignmentTargetSPtr, clockingTargetSPtr);

        for (const auto instant :
             Interval::Closed(epoch, epoch + Duration::Hours(1.0)).generateGrid(Duration::Minutes(5.0)))
        {
            const State calculatedState = calculatedProfile.getStateAt(instant);
            const State expectedState = expectedProfile.getStateAt(instant);

            EXPECT_VECTORS_ALMOST_EQUAL(
                calculatedState.getPosition().getCoordinates(), expectedState.getPosition().getCoordinates(), 1e-15
            );
            EXPECT_VECTORS_ALMOST_EQUAL(
                calculatedState.getVelocity().getCoordinates(), expectedState.getVelocity().getCoordinates(), 1e-15
            );
            EXPECT_TRUE(calculatedState.getAttitude().isNear(expectedState.getAttitude(), Angle::Arcseconds(1e-15)));
        }
    }

    // Regression test for dangling orbit reference when a custom pointing profile
    {
        Profile profile = Profile::Undefined();
        {
            const Orbit anotherOrbit =
                Orbit::SunSynchronous(epoch, Length::Kilometers(500.0), Time(6, 0, 0), earthSPtr);

            profile = Profile::CustomPointing(anotherOrbit, alignmentTargetSPtr, clockingTargetSPtr);
        }

        EXPECT_NO_THROW(profile.getStateAt(epoch));
    }
}

class OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized
    : public ::testing::TestWithParam<
          std::tuple<Profile::Axis, Profile::Axis, Profile::TargetType, Profile::TargetType>>
{
};

TEST_P(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized, AlignAndConstrain)
{
    const auto param = GetParam();
    Profile::TargetType alignmentTargetType;
    Profile::TargetType clockingTargetType;
    Profile::Axis alignmentAxis;
    Profile::Axis clockingAxis;
    std::tie(alignmentAxis, clockingAxis, alignmentTargetType, clockingTargetType) = param;

    const Shared<const Profile::Target> alignmentTargetSPtr =
        std::make_shared<Profile::Target>(alignmentTargetType, alignmentAxis);

    const Shared<const Profile::Target> clockingTargetSPtr =
        std::make_shared<Profile::Target>(clockingTargetType, clockingAxis);

    // check failure conditions

    if (alignmentAxis == clockingAxis)
    {
        EXPECT_THROW(
            Profile::AlignAndConstrain(alignmentTargetSPtr, clockingTargetSPtr), ostk::core::error::RuntimeError
        );
        return;
    }

    if (alignmentTargetType == clockingTargetType)
    {
        EXPECT_THROW(
            Profile::AlignAndConstrain(alignmentTargetSPtr, clockingTargetSPtr), ostk::core::error::RuntimeError
        );
        return;
    }

    if (((alignmentTargetType == Profile::TargetType::GeocentricNadir) &&
         (clockingTargetType == Profile::TargetType::GeodeticNadir)) ||
        ((clockingTargetType == Profile::TargetType::GeocentricNadir) &&
         (alignmentTargetType == Profile::TargetType::GeodeticNadir)))
    {
        EXPECT_THROW(
            Profile::AlignAndConstrain(alignmentTargetSPtr, clockingTargetSPtr), ostk::core::error::RuntimeError
        );
        return;
    }

    const Instant epoch = Instant::J2000();

    const Shared<Earth> earthSPtr = std::make_shared<Earth>(Earth::Default());

    const Orbit orbit = Orbit::SunSynchronous(epoch, Length::Kilometers(500.0), Time(6, 0, 0), earthSPtr);

    const State state = orbit.getStateAt(epoch);

    static const Map<Profile::Axis, Vector3d> axisMap = {
        {Profile::Axis::X, {1.0, 0.0, 0.0}},
        {Profile::Axis::Y, {0.0, 1.0, 0.0}},
        {Profile::Axis::Z, {0.0, 0.0, 1.0}},
    };

    const std::function<Quaternion(const State&)> orientation =
        Profile::AlignAndConstrain(alignmentTargetSPtr, clockingTargetSPtr);

    const Quaternion q_B_GCRF = orientation(state);

    const auto getExpectedDirectionVector =
        [&earthSPtr](const Profile::TargetType& aTargetType, const State& aState) -> Vector3d
    {
        switch (aTargetType)
        {
            case Profile::TargetType::GeocentricNadir:
                return -aState.getPosition().getCoordinates().normalized();
                break;

            case Profile::TargetType::GeodeticNadir:
            {
                const LLA lla = LLA::Cartesian(
                    aState.inFrame(Frame::ITRF()).getPosition().getCoordinates(),
                    earthSPtr->getEquatorialRadius(),
                    earthSPtr->getFlattening()
                );
                const LLA llaOnSurface = LLA(lla.getLatitude(), lla.getLongitude(), Length::Meters(0.0));
                Vector3d positionOnSurface =
                    Position::Meters(
                        llaOnSurface.toCartesian(earthSPtr->getEquatorialRadius(), earthSPtr->getFlattening()),
                        Frame::ITRF()
                    )
                        .inFrame(Frame::GCRF(), aState.accessInstant())
                        .getCoordinates();
                return (positionOnSurface - aState.getPosition().getCoordinates()).normalized();
            }
            break;

            case Profile::TargetType::VelocityECI:
                return aState.getVelocity().getCoordinates().normalized();
                break;

            case Profile::TargetType::Sun:
            {
                const Sun sun = Sun::Default();
                return (sun.getPositionIn(Frame::GCRF(), aState.accessInstant()).getCoordinates() -
                        aState.getPosition().getCoordinates())
                    .normalized();
            }
            break;

            case Profile::TargetType::Moon:
            {
                const Moon moon = Moon::Default();
                return (moon.getPositionIn(Frame::GCRF(), aState.accessInstant()).getCoordinates() -
                        aState.getPosition().getCoordinates())
                    .normalized();
            }
            break;

            default:
                return Vector3d::Zero();
                break;
        }
    };

    // check alignment axis

    const Vector3d alignmentAxisVector = axisMap.at(alignmentAxis);

    const Vector3d estimatedAlignmentDirection = (q_B_GCRF.toConjugate() * alignmentAxisVector);

    const Vector3d expectedAlignmentDirection = getExpectedDirectionVector(alignmentTargetType, state);

    EXPECT_VECTORS_ALMOST_EQUAL(expectedAlignmentDirection, estimatedAlignmentDirection, 1e-12);

    // check clocking axis

    const Vector3d clockingAxisVector = axisMap.at(clockingAxis);

    const Vector3d estimatedClockingDirection = (q_B_GCRF.toConjugate() * clockingAxisVector);

    const Vector3d expectedClockingDirection = getExpectedDirectionVector(clockingTargetType, state);

    // TBI: These tests can be improved in the future
    if (alignmentTargetType == Profile::TargetType::VelocityECI && clockingTargetType == Profile::TargetType::Sun)
    {
        EXPECT_LT(Angle::Between(expectedClockingDirection, estimatedClockingDirection).inDegrees(), 16.0);
    }
    else if (clockingTargetType == Profile::TargetType::Sun)
    {
        EXPECT_LT(Angle::Between(expectedClockingDirection, estimatedClockingDirection).inDegrees(), 1.0);
    }
    else
    {
        EXPECT_LT(Angle::Between(expectedClockingDirection, estimatedClockingDirection).inDegrees(), 1e-3);
    }
}

std::string ParamNameGenerator(
    const ::testing::TestParamInfo<std::tuple<Profile::Axis, Profile::Axis, Profile::TargetType, Profile::TargetType>>&
        info
)
{
    std::string name = "Axis" + std::to_string(static_cast<int>(std::get<0>(info.param))) + "_Axis" +
                       std::to_string(static_cast<int>(std::get<1>(info.param))) + "_Target" +
                       std::to_string(static_cast<int>(std::get<2>(info.param))) + "_Target" +
                       std::to_string(static_cast<int>(std::get<3>(info.param)));
    return name;
}

INSTANTIATE_TEST_SUITE_P(
    OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_Values,
    OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized,
    ::testing::Combine(
        ::testing::Values(Profile::Axis::X),
        ::testing::Values(Profile::Axis::Y),
        ::testing::Values(
            Profile::TargetType::GeocentricNadir, Profile::TargetType::GeodeticNadir, Profile::TargetType::VelocityECI
        ),
        ::testing::Values(
            Profile::TargetType::GeocentricNadir,
            Profile::TargetType::GeodeticNadir,
            Profile::TargetType::VelocityECI,
            Profile::TargetType::Sun
        )
    ),
    ParamNameGenerator
);
