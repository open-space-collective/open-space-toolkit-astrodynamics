/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationVector.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Models/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Models/Transform.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::astro::flight::Profile;
using ostk::astro::trajectory::Orbit;

class OpenSpaceToolkit_Astrodynamics_Flight_Profile : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        using ostk::core::types::Real;

        using ostk::physics::Environment;
        using ostk::physics::coord::Frame;
        using ostk::physics::environment::gravitational::Earth;
        using ostk::physics::time::DateTime;
        using ostk::physics::time::Instant;
        using ostk::physics::time::Scale;
        using ostk::physics::units::Angle;
        using ostk::physics::units::Derived;
        using ostk::physics::units::Length;

        using ostk::astro::flight::Profile;
        using ostk::astro::trajectory::orbit::models::Kepler;
        using ostk::astro::trajectory::orbit::models::kepler::COE;

        const Environment environment = Environment::Default();

        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(0.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);

        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        this->orbit_ = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        this->profile_ = Profile::NadirPointing(this->orbit_, Orbit::FrameType::VVLH);
    }

    Orbit orbit_ = Orbit::Undefined();
    Profile profile_ = Profile::Undefined();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, Constructor)
{
    using ostk::astro::flight::profile::models::Transform;

    {
        EXPECT_NO_THROW(Profile {Transform::Undefined()};);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, CopyConstructor)
{
    using ostk::astro::flight::profile::models::Transform;

    {
        EXPECT_NO_THROW(Profile profile(profile_););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, CopyAssignmentOperator)
{
    using ostk::astro::flight::profile::models::Transform;

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
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, IsDefined)
{
    using ostk::astro::flight::Profile;

    {
        EXPECT_TRUE(profile_.isDefined());
    }

    {
        EXPECT_FALSE(Profile::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, GetStateAt)
{
    using ostk::math::geom::d3::trf::rot::Quaternion;
    using ostk::math::obj::Vector3d;

    using ostk::physics::coord::Frame;
    using ostk::physics::time::Instant;
    using ostk::physics::units::Angle;

    using ostk::astro::flight::Profile;
    using ostk::astro::flight::profile::State;

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
    using ostk::core::ctnr::Array;

    using ostk::math::geom::d3::trf::rot::Quaternion;
    using ostk::math::obj::Vector3d;

    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;

    using ostk::astro::flight::Profile;
    using ostk::astro::flight::profile::State;

    {{const Array<Instant> referenceInstants = {
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
         Frame::GCRF()}};

    const Array<State> states = profile_.getStatesAt(referenceInstants);

    EXPECT_EQ(referenceStates.getSize(), states.getSize());

    for (const auto stateTuple : ostk::core::ctnr::iterators::Zip(referenceStates, states))
    {
        const State& referenceState = std::get<0>(stateTuple);
        const State& state = std::get<1>(stateTuple);

        EXPECT_EQ(state.getInstant(), referenceState.getInstant());
        EXPECT_TRUE(state.getPosition().getCoordinates().isNear(referenceState.getPosition().getCoordinates(), 1e-5));
        EXPECT_TRUE(state.getVelocity().getCoordinates().isNear(referenceState.getVelocity().getCoordinates(), 1e-5));
        EXPECT_TRUE(state.getAttitude().isNear(referenceState.getAttitude().toNormalized(), Angle::Degrees(1e-3)));
        EXPECT_TRUE(state.getAngularVelocity().isNear(referenceState.getAngularVelocity(), 1e-5));
        EXPECT_EQ(state.getFrame(), referenceState.getFrame());
    }
}
}

{
    EXPECT_ANY_THROW(Profile::Undefined().getStatesAt(
        {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
         Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC)}
    ));
}
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, Undefined)
{
    using ostk::astro::flight::Profile;

    {
        EXPECT_NO_THROW(Profile::Undefined());
        EXPECT_FALSE(Profile::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, InertialPointing)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;
    using ostk::core::types::String;

    using ostk::math::geom::d3::trf::rot::Quaternion;
    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::environment::gravitational::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::flight::Profile;
    using ostk::astro::flight::profile::State;
    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

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
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

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
                referenceRow["z (m)"].accessReal()};
            const Vector3d v_BODY_GCRF_in_GCRF_ref = {
                referenceRow["vx (m/sec)"].accessReal(),
                referenceRow["vy (m/sec)"].accessReal(),
                referenceRow["vz (m/sec)"].accessReal()};
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
                referenceRow["wz (rad/sec)"].accessReal()};

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_Profile, NadirPointing_VVLH)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;
    using ostk::core::types::String;

    using ostk::math::geom::d3::trf::rot::Quaternion;
    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::environment::gravitational::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::flight::Profile;
    using ostk::astro::flight::profile::State;
    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::orbit::models::Kepler;
    using ostk::astro::trajectory::orbit::models::kepler::COE;

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
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH);

        const Real positionTolerance_m = 1e-3;
        const Real velocityTolerance_meterPerSec = 1e-6;
        const Real angularTolerance_asec = 0.0;
        const Real angularVelocityTolerance_radPerSec = 1e-10;

        // Reference data setup

        const File referenceDataFile =
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/NadirPointing/VVLH/"
                                   "Satellite_1 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant_ref =
                Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC);

            const Vector3d x_BODY_GCRF_ref = {
                referenceRow["x (m)"].accessReal(),
                referenceRow["y (m)"].accessReal(),
                referenceRow["z (m)"].accessReal()};
            const Vector3d v_BODY_GCRF_in_GCRF_ref = {
                referenceRow["vx (m/sec)"].accessReal(),
                referenceRow["vy (m/sec)"].accessReal(),
                referenceRow["vz (m/sec)"].accessReal()};
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
                referenceRow["wz (rad/sec)"].accessReal()};

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
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH);

        const Real positionTolerance_m = 1e-3;
        const Real velocityTolerance_meterPerSec = 1e-6;
        const Real angularTolerance_asec = 0.0;
        const Real angularVelocityTolerance_radPerSec = 1e-10;

        // Reference data setup

        const File referenceDataFile =
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/NadirPointing/VVLH/"
                                   "Satellite_2 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant_ref =
                Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC);

            const Vector3d x_BODY_GCRF_ref = {
                referenceRow["x (m)"].accessReal(),
                referenceRow["y (m)"].accessReal(),
                referenceRow["z (m)"].accessReal()};
            const Vector3d v_BODY_GCRF_in_GCRF_ref = {
                referenceRow["vx (m/sec)"].accessReal(),
                referenceRow["vy (m/sec)"].accessReal(),
                referenceRow["vz (m/sec)"].accessReal()};
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
                referenceRow["wz (rad/sec)"].accessReal()};

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
        const Derived gravitationalParameter = Earth::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = Earth::EGM2008.equatorialRadius_;
        const Real J2 = Earth::EGM2008.J2_;
        const Real J4 = Earth::EGM2008.J4_;

        const Kepler keplerianModel = {
            coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None};

        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH);

        const Real positionTolerance_m = 1e-3;
        const Real velocityTolerance_meterPerSec = 1e-6;
        const Real angularTolerance_asec = 0.0;
        const Real angularVelocityTolerance_radPerSec = 1e-7;

        // Reference data setup

        const File referenceDataFile =
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/NadirPointing/VVLH/"
                                   "Satellite_3 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv"));

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true);

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant_ref =
                Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC);

            const Vector3d x_BODY_GCRF_ref = {
                referenceRow["x (m)"].accessReal(),
                referenceRow["y (m)"].accessReal(),
                referenceRow["z (m)"].accessReal()};
            const Vector3d v_BODY_GCRF_in_GCRF_ref = {
                referenceRow["vx (m/sec)"].accessReal(),
                referenceRow["vy (m/sec)"].accessReal(),
                referenceRow["vz (m/sec)"].accessReal()};
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
                referenceRow["wz (rad/sec)"].accessReal()};

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
    using ostk::core::ctnr::Array;

    using ostk::math::geom::d3::trf::rot::Quaternion;
    using ostk::math::geom::d3::trf::rot::RotationVector;
    using ostk::math::obj::Vector3d;

    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;

    using ostk::astro::flight::Profile;
    using ostk::astro::flight::profile::State;
    using ostk::astro::flight::profile::models::Tabulated;

    {
        const Array<State> tabulatedStates = {
            {Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF()),
             Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF()),
             Quaternion::RotationVector(RotationVector::X(Angle::Degrees(0.0))),
             {0.0, 0.0, 1.0},
             Frame::GCRF()},
            {Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 0), Scale::UTC),
             Position::Meters({2.0, 3.0, 4.0}, Frame::GCRF()),
             Velocity::MetersPerSecond({3.0, 4.0, 5.0}, Frame::GCRF()),
             Quaternion::RotationVector(RotationVector::X(Angle::Degrees(10.0))),
             {0.0, 0.0, 2.0},
             Frame::GCRF()},
            {Instant::DateTime(DateTime(2020, 1, 1, 0, 2, 0), Scale::UTC),
             Position::Meters({4.0, 5.0, 6.0}, Frame::GCRF()),
             Velocity::MetersPerSecond({1.0, 2.0, 3.0}, Frame::GCRF()),
             Quaternion::RotationVector(RotationVector::X(Angle::Degrees(30.0))),
             {0.0, 0.0, 4.0},
             Frame::GCRF()}};

        const Tabulated tabulated = {tabulatedStates};

        const Profile profile = {tabulated};

        const Array<Instant> referenceInstants = {
            Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC),
            Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 30), Scale::UTC),
            Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 0), Scale::UTC),
            Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 30), Scale::UTC),
            Instant::DateTime(DateTime(2020, 1, 1, 0, 2, 0), Scale::UTC)};

        const Array<State> states = profile.getStatesAt(referenceInstants);

        const Array<State> referenceStates = {
            {Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF()),
             Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF()),
             Quaternion::RotationVector(RotationVector::X(Angle::Degrees(0.0))),
             {0.0, 0.0, 1.0},
             Frame::GCRF()},
            {Instant::DateTime(DateTime(2020, 1, 1, 0, 0, 30), Scale::UTC),
             Position::Meters({1.5, 2.5, 3.5}, Frame::GCRF()),
             Velocity::MetersPerSecond({3.5, 4.5, 5.5}, Frame::GCRF()),
             Quaternion::RotationVector(RotationVector::X(Angle::Degrees(5.0))),
             {0.0, 0.0, 1.5},
             Frame::GCRF()},
            {Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 0), Scale::UTC),
             Position::Meters({2.0, 3.0, 4.0}, Frame::GCRF()),
             Velocity::MetersPerSecond({3.0, 4.0, 5.0}, Frame::GCRF()),
             Quaternion::RotationVector(RotationVector::X(Angle::Degrees(10.0))),
             {0.0, 0.0, 2.0},
             Frame::GCRF()},
            {Instant::DateTime(DateTime(2020, 1, 1, 0, 1, 30), Scale::UTC),
             Position::Meters({3.0, 4.0, 5.0}, Frame::GCRF()),
             Velocity::MetersPerSecond({2.0, 3.0, 4.0}, Frame::GCRF()),
             Quaternion::RotationVector(RotationVector::X(Angle::Degrees(20.0))),
             {0.0, 0.0, 3.0},
             Frame::GCRF()},
            {Instant::DateTime(DateTime(2020, 1, 1, 0, 2, 0), Scale::UTC),
             Position::Meters({4.0, 5.0, 6.0}, Frame::GCRF()),
             Velocity::MetersPerSecond({1.0, 2.0, 3.0}, Frame::GCRF()),
             Quaternion::RotationVector(RotationVector::X(Angle::Degrees(30.0))),
             {0.0, 0.0, 4.0},
             Frame::GCRF()}};

        EXPECT_EQ(referenceStates.getSize(), states.getSize());

        for (const auto stateTuple : ostk::core::ctnr::iterators::Zip(referenceStates, states))
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
