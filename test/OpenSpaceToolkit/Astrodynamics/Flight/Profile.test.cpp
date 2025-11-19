/// Apache License 2.0

#include <unordered_set>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationVector.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Nadir.hpp>
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
using ostk::mathematics::object::Vector4d;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::Manager;
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
using ostk::astrodynamics::trajectory::model::Nadir;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, ConstructBodyFrame)
{
    {
        const String frameName = "test_construct_body_frame";

        // basic construction
        EXPECT_NO_THROW(profile_.constructBodyFrame(frameName));
        EXPECT_TRUE(Manager::Get().hasFrameWithName(frameName));

        // undefined constructor throws
        EXPECT_THROW(Profile::Undefined().constructBodyFrame(frameName), ostk::core::error::RuntimeError);

        // calling again without overwrite (default false) throws
        EXPECT_THROW(profile_.constructBodyFrame(frameName), ostk::core::error::RuntimeError);

        // calling with overwrite = false throws
        EXPECT_THROW(profile_.constructBodyFrame(frameName, false), ostk::core::error::RuntimeError);

        // calling with overwrite = true succeeds
        EXPECT_NO_THROW(profile_.constructBodyFrame(frameName, true));

        // frame still exists after overwrite
        EXPECT_TRUE(Manager::Get().hasFrameWithName(frameName));
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
    // Test constructor with Vector3d direction
    {
        EXPECT_NO_THROW(Profile::Target(Profile::TargetType::GeocentricNadir, Vector3d::X()));
    }

    // Test constructor with Axis
    {
        EXPECT_NO_THROW(Profile::Target(Profile::TargetType::GeocentricNadir, Profile::Axis::X));
        EXPECT_NO_THROW(Profile::Target(Profile::TargetType::GeocentricNadir, Profile::Axis::Y, true));
    }

    // Verify that direction is correctly set for Axis-based constructors
    {
        Profile::Target targetXPositive(Profile::TargetType::GeocentricNadir, Profile::Axis::X, false);
        EXPECT_TRUE(targetXPositive.direction.isNear(Vector3d::X(), 1e-12));

        Profile::Target targetXNegative(Profile::TargetType::GeocentricNadir, Profile::Axis::X, true);
        EXPECT_TRUE(targetXNegative.direction.isNear(-Vector3d::X(), 1e-12));

        Profile::Target targetYPositive(Profile::TargetType::GeocentricNadir, Profile::Axis::Y, false);
        EXPECT_TRUE(targetYPositive.direction.isNear(Vector3d::Y(), 1e-12));

        Profile::Target targetYNegative(Profile::TargetType::GeocentricNadir, Profile::Axis::Y, true);
        EXPECT_TRUE(targetYNegative.direction.isNear(-Vector3d::Y(), 1e-12));

        Profile::Target targetZPositive(Profile::TargetType::GeocentricNadir, Profile::Axis::Z, false);
        EXPECT_TRUE(targetZPositive.direction.isNear(Vector3d::Z(), 1e-12));

        Profile::Target targetZNegative(Profile::TargetType::GeocentricNadir, Profile::Axis::Z, true);
        EXPECT_TRUE(targetZNegative.direction.isNear(-Vector3d::Z(), 1e-12));
    }

    // Verify that direction is correctly set for Vector3d-based constructors
    {
        const Vector3d customDirection = Vector3d(1.0, 2.0, 3.0).normalized();
        Profile::Target target(Profile::TargetType::GeocentricNadir, customDirection);
        EXPECT_TRUE(target.direction.isNear(customDirection, 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, TrajectoryTarget)
{
    // Test undefined trajectory throws
    {
        EXPECT_THROW(
            Profile::TrajectoryTarget(Trajectory::Undefined(), Vector3d::X()), ostk::core::error::runtime::Undefined
        );
    }

    // Test undefined trajectory throws for Vector3d-based static factory methods
    {
        EXPECT_THROW(
            Profile::TrajectoryTarget::TargetPosition(Trajectory::Undefined(), Vector3d::X()),
            ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(
            Profile::TrajectoryTarget::TargetVelocity(Trajectory::Undefined(), Vector3d::X()),
            ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(
            Profile::TrajectoryTarget::TargetSlidingGroundVelocity(Trajectory::Undefined(), Vector3d::X()),
            ostk::core::error::runtime::Undefined
        );
    }

    // Test undefined trajectory throws for Axis-based static factory methods
    {
        EXPECT_THROW(
            Profile::TrajectoryTarget::TargetPosition(Trajectory::Undefined(), Profile::Axis::X),
            ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(
            Profile::TrajectoryTarget::TargetVelocity(Trajectory::Undefined(), Profile::Axis::X),
            ostk::core::error::runtime::Undefined
        );
        EXPECT_THROW(
            Profile::TrajectoryTarget::TargetSlidingGroundVelocity(Trajectory::Undefined(), Profile::Axis::X),
            ostk::core::error::runtime::Undefined
        );
    }

    // Test valid trajectory with Vector3d direction
    {
        const Trajectory trajectory = Trajectory::Position(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()));
        EXPECT_NO_THROW(Profile::TrajectoryTarget(trajectory, Vector3d::X()));

        // Testing static factory methods with Vector3d
        EXPECT_NO_THROW(Profile::TrajectoryTarget::TargetPosition(trajectory, Vector3d::X()));
        EXPECT_NO_THROW(Profile::TrajectoryTarget::TargetVelocity(trajectory, Vector3d::Y()));
        EXPECT_NO_THROW(Profile::TrajectoryTarget::TargetSlidingGroundVelocity(trajectory, Vector3d::Z()));

        // Testing static factory methods with Axis
        EXPECT_NO_THROW(Profile::TrajectoryTarget::TargetPosition(trajectory, Profile::Axis::X));
        EXPECT_NO_THROW(Profile::TrajectoryTarget::TargetPosition(trajectory, Profile::Axis::Y, true));
    }

    // Verify direction is correctly set for Vector3d-based methods
    {
        const Trajectory trajectory = Trajectory::Position(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()));
        const Vector3d customDirection = Vector3d(1.0, 2.0, 3.0).normalized();

        Profile::TrajectoryTarget targetPosition =
            Profile::TrajectoryTarget::TargetPosition(trajectory, customDirection);
        EXPECT_TRUE(targetPosition.direction.isNear(customDirection, 1e-12));
        EXPECT_EQ(targetPosition.type, Profile::TargetType::TargetPosition);

        Profile::TrajectoryTarget targetVelocity =
            Profile::TrajectoryTarget::TargetVelocity(trajectory, customDirection);
        EXPECT_TRUE(targetVelocity.direction.isNear(customDirection, 1e-12));
        EXPECT_EQ(targetVelocity.type, Profile::TargetType::TargetVelocity);

        Profile::TrajectoryTarget targetSlidingGroundVelocity =
            Profile::TrajectoryTarget::TargetSlidingGroundVelocity(trajectory, customDirection);
        EXPECT_TRUE(targetSlidingGroundVelocity.direction.isNear(customDirection, 1e-12));
        EXPECT_EQ(targetSlidingGroundVelocity.type, Profile::TargetType::TargetSlidingGroundVelocity);
    }

    // Verify direction is correctly set for Axis-based methods
    {
        const Trajectory trajectory = Trajectory::Position(Position::Meters({0.0, 0.0, 0.0}, Frame::ITRF()));

        Profile::TrajectoryTarget targetPositionX =
            Profile::TrajectoryTarget::TargetPosition(trajectory, Profile::Axis::X, false);
        EXPECT_TRUE(targetPositionX.direction.isNear(Vector3d::X(), 1e-12));

        Profile::TrajectoryTarget targetPositionXNeg =
            Profile::TrajectoryTarget::TargetPosition(trajectory, Profile::Axis::X, true);
        EXPECT_TRUE(targetPositionXNeg.direction.isNear(-Vector3d::X(), 1e-12));

        Profile::TrajectoryTarget targetVelocityY =
            Profile::TrajectoryTarget::TargetVelocity(trajectory, Profile::Axis::Y, false);
        EXPECT_TRUE(targetVelocityY.direction.isNear(Vector3d::Y(), 1e-12));

        Profile::TrajectoryTarget targetVelocityYNeg =
            Profile::TrajectoryTarget::TargetVelocity(trajectory, Profile::Axis::Y, true);
        EXPECT_TRUE(targetVelocityYNeg.direction.isNear(-Vector3d::Y(), 1e-12));

        Profile::TrajectoryTarget targetSlidingGroundVelocityZ =
            Profile::TrajectoryTarget::TargetSlidingGroundVelocity(trajectory, Profile::Axis::Z, false);
        EXPECT_TRUE(targetSlidingGroundVelocityZ.direction.isNear(Vector3d::Z(), 1e-12));

        Profile::TrajectoryTarget targetSlidingGroundVelocityZNeg =
            Profile::TrajectoryTarget::TargetSlidingGroundVelocity(trajectory, Profile::Axis::Z, true);
        EXPECT_TRUE(targetSlidingGroundVelocityZNeg.direction.isNear(-Vector3d::Z(), 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, CustomTarget)
{
    std::function<Vector3d(const State&)> orientationGenerator = [](const State&) -> Vector3d
    {
        return Vector3d::X();
    };

    // Test constructor with Vector3d direction
    {
        EXPECT_NO_THROW(Profile::CustomTarget(orientationGenerator, Vector3d::X()));
        EXPECT_NO_THROW(Profile::CustomTarget(orientationGenerator, Vector3d::Y()));
    }

    // Test constructor with Axis
    {
        EXPECT_NO_THROW(Profile::CustomTarget(orientationGenerator, Profile::Axis::X));
        EXPECT_NO_THROW(Profile::CustomTarget(orientationGenerator, Profile::Axis::Y, false));
    }

    // Verify direction is correctly set for Vector3d-based constructor
    {
        const Vector3d customDirection = Vector3d(1.0, 2.0, 3.0).normalized();
        const Profile::CustomTarget target = Profile::CustomTarget(orientationGenerator, customDirection);
        EXPECT_TRUE(target.direction.isNear(customDirection, 1e-12));
        EXPECT_EQ(target.type, Profile::TargetType::Custom);
    }

    // Verify direction is correctly set for Axis-based constructor
    {
        Profile::CustomTarget targetXPositive(orientationGenerator, Profile::Axis::X, false);
        EXPECT_TRUE(targetXPositive.direction.isNear(Vector3d::X(), 1e-12));

        Profile::CustomTarget targetXNegative(orientationGenerator, Profile::Axis::X, true);
        EXPECT_TRUE(targetXNegative.direction.isNear(-Vector3d::X(), 1e-12));

        Profile::CustomTarget targetYPositive(orientationGenerator, Profile::Axis::Y, false);
        EXPECT_TRUE(targetYPositive.direction.isNear(Vector3d::Y(), 1e-12));

        Profile::CustomTarget targetYNegative(orientationGenerator, Profile::Axis::Y, true);
        EXPECT_TRUE(targetYNegative.direction.isNear(-Vector3d::Y(), 1e-12));

        Profile::CustomTarget targetZPositive(orientationGenerator, Profile::Axis::Z, false);
        EXPECT_TRUE(targetZPositive.direction.isNear(Vector3d::Z(), 1e-12));

        Profile::CustomTarget targetZNegative(orientationGenerator, Profile::Axis::Z, true);
        EXPECT_TRUE(targetZNegative.direction.isNear(-Vector3d::Z(), 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, OrientationProfileTarget)
{
    // Test empty orientation profile throws with Vector3d
    {
        EXPECT_THROW(Profile::OrientationProfileTarget({}, Vector3d::X()), ostk::core::error::runtime::Undefined);
    }

    // Test empty orientation profile throws with Axis
    {
        EXPECT_THROW(Profile::OrientationProfileTarget({}, Profile::Axis::X), ostk::core::error::runtime::Undefined);
        EXPECT_THROW(
            Profile::OrientationProfileTarget({}, Profile::Axis::X, false), ostk::core::error::runtime::Undefined
        );
    }

    // Test valid orientation profile with Vector3d direction
    {
        const Array<Pair<Instant, Vector3d>> orientationProfile = {
            {Instant::J2000(), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(10.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(20.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(30.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(40.0), Vector3d::X()},
        };
        EXPECT_NO_THROW(Profile::OrientationProfileTarget(orientationProfile, Vector3d::X()));
    }

    // Test valid orientation profile with Axis
    {
        const Array<Pair<Instant, Vector3d>> orientationProfile = {
            {Instant::J2000(), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(10.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(20.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(30.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(40.0), Vector3d::X()},
        };
        EXPECT_NO_THROW(Profile::OrientationProfileTarget(orientationProfile, Profile::Axis::X));
        EXPECT_NO_THROW(Profile::OrientationProfileTarget(orientationProfile, Profile::Axis::X, false));
    }

    // Verify direction is correctly set for Vector3d-based constructor
    {
        const Array<Pair<Instant, Vector3d>> orientationProfile = {
            {Instant::J2000(), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(10.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(20.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(30.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(40.0), Vector3d::X()},
        };
        const Vector3d customDirection = Vector3d(1.0, 2.0, 3.0).normalized();
        Profile::OrientationProfileTarget target(orientationProfile, customDirection);
        EXPECT_TRUE(target.direction.isNear(customDirection, 1e-12));
        EXPECT_EQ(target.type, Profile::TargetType::OrientationProfile);
    }

    // Verify direction is correctly set for Axis-based constructor
    {
        const Array<Pair<Instant, Vector3d>> orientationProfile = {
            {Instant::J2000(), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(10.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(20.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(30.0), Vector3d::X()},
            {Instant::J2000() + Duration::Seconds(40.0), Vector3d::X()},
        };

        Profile::OrientationProfileTarget targetXPositive(orientationProfile, Profile::Axis::X, false);
        EXPECT_TRUE(targetXPositive.direction.isNear(Vector3d::X(), 1e-12));

        Profile::OrientationProfileTarget targetXNegative(orientationProfile, Profile::Axis::X, true);
        EXPECT_TRUE(targetXNegative.direction.isNear(-Vector3d::X(), 1e-12));

        Profile::OrientationProfileTarget targetYPositive(orientationProfile, Profile::Axis::Y, false);
        EXPECT_TRUE(targetYPositive.direction.isNear(Vector3d::Y(), 1e-12));

        Profile::OrientationProfileTarget targetYNegative(orientationProfile, Profile::Axis::Y, true);
        EXPECT_TRUE(targetYNegative.direction.isNear(-Vector3d::Y(), 1e-12));

        Profile::OrientationProfileTarget targetZPositive(orientationProfile, Profile::Axis::Z, false);
        EXPECT_TRUE(targetZPositive.direction.isNear(Vector3d::Z(), 1e-12));

        Profile::OrientationProfileTarget targetZNegative(orientationProfile, Profile::Axis::Z, true);
        EXPECT_TRUE(targetZNegative.direction.isNear(-Vector3d::Z(), 1e-12));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, YawCompensationOrekit)
{
    const Shared<const Earth> earthSPtr_ = std::make_shared<Earth>(Earth::WGS84());

    const Length semiMajorAxis = Length::Kilometers(7000.0);
    const Real eccentricity = 0.0;
    const Angle inclination = Angle::Degrees(98.0);
    const Angle raan = Angle::Degrees(0.0);
    const Angle aop = Angle::Degrees(0.0);
    const Angle trueAnomaly = Angle::Degrees(0.0);

    const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

    const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
    const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
    const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
    const Real J2 = EarthGravitationalModel::EGM2008.J2_;
    const Real J4 = EarthGravitationalModel::EGM2008.J4_;

    const Kepler keplerianModel = {
        coe, instant, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
    };
    const Orbit orbit = {keplerianModel, earthSPtr_};
    const Trajectory targetTrajectory = Trajectory(Nadir(orbit));

    std::cout.precision(16);

    // Yaw compensated clocking profile
    const Profile targetSlidingGroundVelocityClockedProfile = Profile::CustomPointing(
        orbit,
        std::make_shared<const Profile::TrajectoryTarget>(
            Profile::TrajectoryTarget::TargetPosition(targetTrajectory, Profile::Axis::Z)
        ),
        std::make_shared<const Profile::TrajectoryTarget>(
            Profile::TrajectoryTarget::TargetSlidingGroundVelocity(targetTrajectory, Profile::Axis::X)
        )
    );

    // Non-yaw compensated clocking profile
    const Profile targetVelocityClockedProfile = Profile::CustomPointing(
        orbit,
        std::make_shared<const Profile::TrajectoryTarget>(
            Profile::TrajectoryTarget::TargetPosition(targetTrajectory, Profile::Axis::Z)
        ),
        std::make_shared<const Profile::TrajectoryTarget>(
            Profile::TrajectoryTarget::TargetVelocity(targetTrajectory, Profile::Axis::X)
        )
    );

    const Profile satelliteVelocityClockedProfile = Profile::CustomPointing(
        orbit,
        std::make_shared<const Profile::TrajectoryTarget>(
            Profile::TrajectoryTarget::TargetPosition(targetTrajectory, Profile::Axis::Z)
        ),
        std::make_shared<const Profile::Target>(Profile::TargetType::VelocityECI, Profile::Axis::X)
    );

    const State targetSlidingGroundVelocityClockedState = targetSlidingGroundVelocityClockedProfile.getStateAt(instant);
    const State targetVelocityClockedProfileState = targetVelocityClockedProfile.getStateAt(instant);
    const State satelliteVelocityClockedState = satelliteVelocityClockedProfile.getStateAt(instant);

    // Values taken from Orekit for unit test
    const Vector4d expectedTargetSlidingGroundVelocityClockedQuaternion = {
        0.07270744620256138, -0.7033625816898471, -0.07270667666769966, 0.7033551344187611
    };
    const Vector4d expectedSatelliteVelocityClockedQuaternion = {
        0.049325536600717236, -0.7053880389500211, -0.04932501463561278, 0.7053805702431075
    };

    EXPECT_VECTORS_ALMOST_EQUAL(
        expectedTargetSlidingGroundVelocityClockedQuaternion,
        targetSlidingGroundVelocityClockedState.getAttitude().toNormalized().toVector(Quaternion::Format::XYZS),
        1e-8
    );
    EXPECT_VECTORS_ALMOST_EQUAL(
        expectedSatelliteVelocityClockedQuaternion,
        targetVelocityClockedProfileState.getAttitude().toNormalized().toVector(Quaternion::Format::XYZS),
        1e-3  // Not as close because this is targetVelocity clocked instead of satelliteVelocity clocked
    );
    EXPECT_VECTORS_ALMOST_EQUAL(
        expectedSatelliteVelocityClockedQuaternion,
        satelliteVelocityClockedState.getAttitude().toNormalized().toVector(Quaternion::Format::XYZS),
        1e-8
    );

    EXPECT_NEAR(
        targetSlidingGroundVelocityClockedState.getAttitude()
            .angularDifferenceWith(satelliteVelocityClockedState.getAttitude())
            .inDegrees(),
        3.803545407107513,
        1e-4
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, AlignAndConstrain)
{
    {
        EXPECT_THROW(
            Profile::AlignAndConstrain(
                std::make_shared<const Profile::Target>(Profile::TargetType::VelocityECEF, Vector3d::X()),
                std::make_shared<const Profile::Target>(Profile::TargetType::GeocentricNadir, Vector3d::Y())
            ),
            ostk::core::error::runtime::ToBeImplemented
        );
    }

    {
        EXPECT_THROW(
            Profile::AlignAndConstrain(
                std::make_shared<const Profile::Target>(Profile::TargetType::GeocentricNadir, Vector3d::Y()),
                std::make_shared<const Profile::Target>(Profile::TargetType::VelocityECEF, Vector3d::X())
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
                std::make_shared<Profile::TrajectoryTarget>(trajectory, Vector3d::X()),
                std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Vector3d::Y())
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
                std::make_shared<Profile::OrientationProfileTarget>(orientationProfile, Vector3d::X()),
                std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Vector3d::Y())
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
                std::make_shared<Profile::CustomTarget>(orientationGenerator, Vector3d::X()),
                std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Vector3d::Y())
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
        std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Vector3d::X());

    const Shared<const Profile::Target> clockingTargetSPtr =
        std::make_shared<Profile::Target>(Profile::TargetType::OrbitalMomentum, Vector3d::Y());

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

class OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_SimpleTargetTypes
    : public ::testing::TestWithParam<std::tuple<Vector3d, Vector3d, Profile::TargetType, Profile::TargetType>>
{
   protected:
    const Instant epoch_ = Instant::J2000();

    const Shared<Earth> earthSPtr_ = std::make_shared<Earth>(Earth::Default());

    const Orbit orbit_ = Orbit::SunSynchronous(epoch_, Length::Kilometers(500.0), Time(6, 0, 0), earthSPtr_);

    const State state_ = orbit_.getStateAt(epoch_);

    const std::function<Vector3d(const Profile::TargetType&, const State&)> getExpectedDirectionVector_ =
        [this](const Profile::TargetType& aTargetType, const State& aState) -> Vector3d
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
                    earthSPtr_->getEquatorialRadius(),
                    earthSPtr_->getFlattening()
                );
                const LLA llaOnSurface = LLA(lla.getLatitude(), lla.getLongitude(), Length::Meters(0.0));
                Vector3d positionOnSurface =
                    Position::Meters(
                        llaOnSurface.toCartesian(earthSPtr_->getEquatorialRadius(), earthSPtr_->getFlattening()),
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
};

TEST_P(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_SimpleTargetTypes, AlignAndConstrain)
{
    const auto& [alignmentDirection, clockingDirection, alignmentTargetType, clockingTargetType] = GetParam();

    const Shared<const Profile::Target> alignmentTargetSPtr =
        std::make_shared<Profile::Target>(alignmentTargetType, alignmentDirection);

    const Shared<const Profile::Target> clockingTargetSPtr =
        std::make_shared<Profile::Target>(clockingTargetType, clockingDirection);

    // check failure conditions

    if (alignmentDirection == clockingDirection)
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

    const std::function<Quaternion(const State&)> orientation =
        Profile::AlignAndConstrain(alignmentTargetSPtr, clockingTargetSPtr);

    const Quaternion q_B_GCRF = orientation(state_);

    // check alignment direction

    const Vector3d estimatedAlignmentDirection = (q_B_GCRF.toConjugate() * alignmentDirection);

    const Vector3d expectedAlignmentDirection = getExpectedDirectionVector_(alignmentTargetType, state_);

    EXPECT_VECTORS_ALMOST_EQUAL(expectedAlignmentDirection, estimatedAlignmentDirection, 1e-12);

    // check clocking direction

    const Vector3d estimatedClockingDirection = (q_B_GCRF.toConjugate() * clockingDirection);

    const Vector3d expectedClockingDirection = getExpectedDirectionVector_(clockingTargetType, state_);

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

INSTANTIATE_TEST_SUITE_P(
    OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_SimpleTargetTypes_Values,
    OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_SimpleTargetTypes,
    ::testing::Combine(
        // alignmentDirection
        ::testing::Values(Vector3d::X()),
        // clockingDirection
        ::testing::Values(Vector3d::Y()),
        // alignmentTargetType
        ::testing::Values(
            Profile::TargetType::GeocentricNadir, Profile::TargetType::GeodeticNadir, Profile::TargetType::VelocityECI
        ),
        // clockingTargetType
        ::testing::Values(
            Profile::TargetType::GeocentricNadir,
            Profile::TargetType::GeodeticNadir,
            Profile::TargetType::VelocityECI,
            Profile::TargetType::Sun
        )
    ),
    [](const ::testing::TestParamInfo<std::tuple<Vector3d, Vector3d, Profile::TargetType, Profile::TargetType>>&
           paramInfo)
    {
        // For Vector3d, print as string; for enums, print as int
        const auto& alignmentDirection = std::get<0>(paramInfo.param);
        const auto& clockingDirection = std::get<1>(paramInfo.param);
        const auto& alignmentTargetType = std::get<2>(paramInfo.param);
        const auto& clockingTargetType = std::get<3>(paramInfo.param);

        auto vecToStr = [](const Vector3d& v)
        {
            std::ostringstream oss;
            oss << (v.x() < 0 ? "m" : "") << std::abs(v.x()) << (v.y() < 0 ? "m" : "") << std::abs(v.y())
                << (v.z() < 0 ? "m" : "") << std::abs(v.z());
            return oss.str();
        };

        // Patch: GTest requires test name to match: ^[A-Za-z_][A-Za-z0-9_]*$
        // So, make a string that only uses alphanumerics and '_'
        return "Align_" + vecToStr(alignmentDirection) + "_Clock_" + vecToStr(clockingDirection) + "_AlignTarget" +
               std::to_string(static_cast<int>(alignmentTargetType)) + "_ClockTarget" +
               std::to_string(static_cast<int>(clockingTargetType));
    }
);

class OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_TrajectoryTargetTypes
    : public ::testing::TestWithParam<std::tuple<Angle, Angle, Vector3d, Vector3d, Vector3d>>
{
   protected:
    const Instant instant_ = Instant::J2000();
    const Shared<const Earth> earthSPtr_ = std::make_shared<Earth>(Earth::WGS84());

    // Simple perfectly polar keplerian orbit with epoch at the ascending node
    const Length semiMajorAxis_ = Length::Kilometers(7000.0);
    const Real eccentricity_ = 0.0;
    const Angle raan_ = Angle::Degrees(0.0);
    const Angle aop_ = Angle::Degrees(0.0);
};

TEST_P(OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_TrajectoryTargetTypes, AlignAndConstrain)
{
    const auto& [inclination, trueAnomaly, expectedTargetPositionDirection, expectedTargetVelocityDirection, expectedTargetSlidingGroundVelocityDirection] =
        GetParam();

    const COE coe = {semiMajorAxis_, eccentricity_, inclination, raan_, aop_, trueAnomaly};
    const Kepler keplerianModel = {coe, instant_, *earthSPtr_, Kepler::PerturbationType::None};
    const Orbit orbit = {keplerianModel, earthSPtr_};
    const State state = orbit.getStateAt(instant_);
    const Trajectory targetTrajectory = Trajectory(Nadir(orbit));

    const Shared<const Profile::TrajectoryTarget> targetPositionSPtr =
        std::make_shared<const Profile::TrajectoryTarget>(
            Profile::TrajectoryTarget::TargetPosition(targetTrajectory, Vector3d::X())
        );

    const Shared<const Profile::TrajectoryTarget> targetVelocitySPtr =
        std::make_shared<const Profile::TrajectoryTarget>(
            Profile::TrajectoryTarget::TargetVelocity(targetTrajectory, Vector3d::X())
        );
    const Shared<const Profile::TrajectoryTarget> targetSlidingGroundVelocitySPtr =
        std::make_shared<const Profile::TrajectoryTarget>(
            Profile::TrajectoryTarget::TargetSlidingGroundVelocity(targetTrajectory, Vector3d::X())
        );

    const Shared<const Profile::Target> clockingTargetSPtr =
        std::make_shared<Profile::Target>(Profile::TargetType::VelocityECI, Vector3d::Y());

    const std::function<Quaternion(const State&)> orientationTargetPositionAlignement =
        Profile::AlignAndConstrain(targetPositionSPtr, clockingTargetSPtr);
    const std::function<Quaternion(const State&)> orientationTargetVelocityAlignement =
        Profile::AlignAndConstrain(targetVelocitySPtr, clockingTargetSPtr);
    const std::function<Quaternion(const State&)> orientationTargetSlidingGroundVelocityAlignement =
        Profile::AlignAndConstrain(targetSlidingGroundVelocitySPtr, clockingTargetSPtr);

    const Vector3d targetPositionDirection =
        orientationTargetPositionAlignement(state).toInverse() * Vector3d(1.0, 0.0, 0.0);
    const Vector3d targetVelocityDirection =
        orientationTargetVelocityAlignement(state).toInverse() * Vector3d(1.0, 0.0, 0.0);
    const Vector3d targetSlidingGroundVelocityDirection =
        orientationTargetSlidingGroundVelocityAlignement(state).toInverse() * Vector3d(1.0, 0.0, 0.0);

    EXPECT_VECTORS_ALMOST_EQUAL(targetPositionDirection, expectedTargetPositionDirection, 1e-3);
    EXPECT_VECTORS_ALMOST_EQUAL(targetVelocityDirection, expectedTargetVelocityDirection, 1e-3);
    EXPECT_VECTORS_ALMOST_EQUAL(
        targetSlidingGroundVelocityDirection, expectedTargetSlidingGroundVelocityDirection, 1e-3
    );
}

INSTANTIATE_TEST_SUITE_P(
    OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_TrajectoryTargetTypes_Values,
    OpenSpaceToolkit_Astrodynamics_Flight_Profile_Parametrized_TrajectoryTargetTypes,
    ::testing::Values(
        // Ascending node
        std::make_tuple(
            Angle::Degrees(90.0),       // Inclination
            Angle::Degrees(0.0),        // TrueAnomaly
            Vector3d {-1.0, 0.0, 0.0},  // ExpectedTargetPositionDirection
            Vector3d {0.0, 0.0, 1.0},   // ExpectedTargetVelocityDirection
            Vector3d {0.0, -0.0675, 0.9977}
            // ExpectedTargetSlidingGroundVelocityDirection - pointing westwards to compensate for Earth rotation
        ),
        // North point
        std::make_tuple(
            Angle::Degrees(90.0),       // Inclination
            Angle::Degrees(90.0),       // TrueAnomaly
            Vector3d {0.0, 0.0, -1.0},  // ExpectedTargetPositionDirection
            Vector3d {-1.0, 0.0, 0.0},  // ExpectedTargetVelocityDirection
            Vector3d {-1.0, 0.0, 0.0}
            // ExpectedTargetSlidingGroundVelocityDirection - same as velocity direction because no rotation at pole
        ),
        // Descending node
        std::make_tuple(
            Angle::Degrees(90.0),       // Inclination
            Angle::Degrees(180.0),      // TrueAnomaly
            Vector3d {1.0, 0.0, 0.0},   // ExpectedTargetPositionDirection
            Vector3d {0.0, 0.0, -1.0},  // ExpectedTargetVelocityDirection
            Vector3d {0.0, 0.0675, -0.9977}
            // ExpectedTargetSlidingGroundVelocityDirection - pointing westwards to compensate for Earth rotation
        ),
        // South point
        std::make_tuple(
            Angle::Degrees(90.0),      // Inclination
            Angle::Degrees(270.0),     // TrueAnomaly
            Vector3d {0.0, 0.0, 1.0},  // ExpectedTargetPositionDirection
            Vector3d {1.0, 0.0, 0.0},  // ExpectedTargetVelocityDirection
            Vector3d {1.0, 0.0, 0.0}
            // ExpectedTargetSlidingGroundVelocityDirection - same as velocity direction because no rotation at pole
        )
    )
);
