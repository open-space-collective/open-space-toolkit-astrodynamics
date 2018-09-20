////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Flight/Profile.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Flight/Profile.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>

#include <Library/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <Library/Physics/Environment.hpp>
#include <Library/Physics/Time/DateTime.hpp>
#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Duration.hpp>
#include <Library/Physics/Time/Instant.hpp>
#include <Library/Physics/Time/Scale.hpp>
#include <Library/Physics/Units/Derived/Angle.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Length.hpp>

#include <Library/Mathematics/Objects/Vector.hpp>

#include <Library/Core/Containers/Table.hpp>
#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Shared.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Flight_Profile, Constructor)
{

    using library::core::types::Shared ;
    
    using library::physics::time::Instant ;
    using library::physics::coord::Transform ;
    using library::physics::coord::Frame ;
    using library::physics::coord::frame::provider::Dynamic ;

    using library::astro::flight::Profile ;

    {

        const Dynamic dynamicTransformProvider =
        {
            [] (const Instant& anInstant) -> Transform
            {
                return Transform::Identity(anInstant) ;
            }
        } ;

        const Shared<const Frame> frameSPtr = Frame::GCRF() ;

        EXPECT_NO_THROW(Profile profile(dynamicTransformProvider, frameSPtr)) ;

    }

}

TEST (Library_Astrodynamics_Flight_Profile, StreamOperator)
{

    using library::core::types::Shared ;
    
    using library::physics::time::Instant ;
    using library::physics::coord::Transform ;
    using library::physics::coord::Frame ;
    using library::physics::coord::frame::provider::Dynamic ;

    using library::astro::flight::Profile ;

    {

        const Dynamic dynamicTransformProvider =
        {
            [] (const Instant& anInstant) -> Transform
            {
                return Transform::Identity(anInstant) ;
            }
        } ;

        const Shared<const Frame> frameSPtr = Frame::GCRF() ;

        const Profile profile = { dynamicTransformProvider, frameSPtr } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << profile << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (Library_Astrodynamics_Flight_Profile, IsDefined)
{

    using library::core::types::Shared ;
    
    using library::physics::time::Instant ;
    using library::physics::coord::Transform ;
    using library::physics::coord::Frame ;
    using library::physics::coord::frame::provider::Dynamic ;

    using library::astro::flight::Profile ;

    {

        const Dynamic dynamicTransformProvider =
        {
            [] (const Instant& anInstant) -> Transform
            {
                return Transform::Identity(anInstant) ;
            }
        } ;

        const Shared<const Frame> frameSPtr = Frame::GCRF() ;

        const Profile profile = { dynamicTransformProvider, frameSPtr } ;

        EXPECT_TRUE(profile.isDefined()) ;

    }

    {

        EXPECT_FALSE(Profile::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Flight_Profile, GetStateAt)
{

    using library::core::types::Shared ;

    using library::math::obj::Vector3d ;
    using library::math::geom::d3::trf::rot::Quaternion ;
    
    using library::physics::time::Instant ;
    using library::physics::coord::Transform ;
    using library::physics::coord::Frame ;
    using library::physics::coord::frame::provider::Dynamic ;

    using library::astro::flight::Profile ;
    using library::astro::flight::profile::State ;

    {

        const Dynamic dynamicTransformProvider =
        {
            [] (const Instant& anInstant) -> Transform
            {
                return Transform::Identity(anInstant) ;
            }
        } ;

        const Shared<const Frame> frameSPtr = Frame::GCRF() ;

        const Profile profile = { dynamicTransformProvider, frameSPtr } ;

        const State state = profile.getStateAt(Instant::J2000()) ;

        EXPECT_EQ(Instant::J2000(), state.getInstant()) ;
        EXPECT_EQ(Vector3d::Zero(), state.getPosition()) ;
        EXPECT_EQ(Vector3d::Zero(), state.getVelocity()) ;
        EXPECT_EQ(Quaternion::Unit(), state.getAttitude()) ;
        EXPECT_EQ(Vector3d::Zero(), state.getAngularVelocity()) ;
        EXPECT_EQ(Frame::GCRF(), state.getFrame()) ;

    }

    {

        const Dynamic dynamicTransformProvider =
        {
            [] (const Instant& anInstant) -> Transform
            {
                return Transform::Identity(anInstant) ;
            }
        } ;

        const Shared<const Frame> frameSPtr = Frame::GCRF() ;

        const Profile profile = { dynamicTransformProvider, frameSPtr } ;

        EXPECT_ANY_THROW(Profile::Undefined().getStateAt(Instant::Undefined())) ;
        EXPECT_ANY_THROW(Profile::Undefined().getStateAt(Instant::J2000())) ;
        EXPECT_ANY_THROW(profile.getStateAt(Instant::Undefined())) ;

    }

}

TEST (Library_Astrodynamics_Flight_Profile, GetStatesAt)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::math::obj::Vector3d ;
    using library::math::geom::d3::trf::rot::Quaternion ;
    
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Transform ;
    using library::physics::coord::Frame ;
    using library::physics::coord::frame::provider::Dynamic ;

    using library::astro::flight::Profile ;
    using library::astro::flight::profile::State ;

    {

        const Dynamic dynamicTransformProvider =
        {
            [] (const Instant& anInstant) -> Transform
            {
                return Transform::Identity(anInstant) ;
            }
        } ;

        const Shared<const Frame> frameSPtr = Frame::GCRF() ;

        const Profile profile = { dynamicTransformProvider, frameSPtr } ;

        {

            const Array<Instant> referenceInstants =
            {
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 0), Scale::UTC),
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1, 0), Scale::UTC),
            } ;

            const Array<State> referenceStates =
            {
                { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Vector3d::Zero(), Vector3d::Zero(), Quaternion::Unit(), Vector3d::Zero(), Frame::GCRF() },
                { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC), Vector3d::Zero(), Vector3d::Zero(), Quaternion::Unit(), Vector3d::Zero(), Frame::GCRF() },
                { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Vector3d::Zero(), Vector3d::Zero(), Quaternion::Unit(), Vector3d::Zero(), Frame::GCRF() }
            } ;

            EXPECT_EQ(referenceStates, profile.getStatesAt(referenceInstants)) ;

        }

    }

    {

        EXPECT_ANY_THROW(Profile::Undefined().getStatesAt({ Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC) })) ;

    }

}

TEST (Library_Astrodynamics_Flight_Profile, Undefined)
{

    using library::astro::flight::Profile ;

    {

        EXPECT_NO_THROW(Profile::Undefined()) ;
        EXPECT_FALSE(Profile::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Flight_Profile, InertialPointing)
{

    using library::core::types::Shared ;
    using library::core::types::Real ;
    using library::core::types::String ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;
    using library::math::geom::d3::trf::rot::Quaternion ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::Environment ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;
    using library::astro::flight::Profile ;
    using library::astro::flight::profile::State ;

    {

        const Environment environment = Environment::Default() ;

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        const Profile profile = Profile::InertialPointing(orbit, Quaternion::Unit()) ;

        const Real positionTolerance_m = 1e-3 ;
        const Real velocityTolerance_meterPerSec = 1e-6 ;
        const Real angularTolerance_asec = 0.0 ;
        const Real angularVelocityTolerance_radPerSec = 0.0 ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("../test/Library/Astrodynamics/Flight/Profile/InertialPointing/Satellite t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

        for (const auto& referenceRow : referenceData)
        {

            const Instant instant_ref = Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC) ;

            const Vector3d x_BODY_GCRF_ref = { referenceRow["x (m)"].accessReal(), referenceRow["y (m)"].accessReal(), referenceRow["z (m)"].accessReal() } ;
            const Vector3d v_BODY_GCRF_in_GCRF_ref = { referenceRow["vx (m/sec)"].accessReal(), referenceRow["vy (m/sec)"].accessReal(), referenceRow["vz (m/sec)"].accessReal() } ;
            const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(referenceRow["q1"].accessReal(), referenceRow["q2"].accessReal(), referenceRow["q3"].accessReal(), referenceRow["q4"].accessReal()).normalize() ;
            const Vector3d w_BODY_GCRF_in_BODY_ref = { referenceRow["wx (rad/sec)"].accessReal(), referenceRow["wy (rad/sec)"].accessReal(), referenceRow["wz (rad/sec)"].accessReal() } ;

            const State state = profile.getStateAt(instant_ref) ;

            const Vector3d x_BODY_GCRF = state.getPosition() ;
            const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity() ;
            const Quaternion q_BODY_GCRF = state.getAttitude() ;
            const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity() ;

            ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format("@ {}: {} - {} = {} [m]", instant_ref.toString(), x_BODY_GCRF.toString(), x_BODY_GCRF_ref.toString(), (x_BODY_GCRF - x_BODY_GCRF_ref).norm()) ;
            ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec)) << String::Format("@ {}: {} - {} = {} [m/s]", instant_ref.toString(), v_BODY_GCRF_in_GCRF.toString(), v_BODY_GCRF_in_GCRF_ref.toString(), (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()) ;
            ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec))) << String::Format("@ {}: {} / {} = {} [asec]", instant_ref.toString(), q_BODY_GCRF_ref.toString(), q_BODY_GCRF.toString(), q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()) ;
            ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec)) << String::Format("@ {}: {} - {} = {} [rad/s]", instant_ref.toString(), w_BODY_GCRF_in_BODY_ref.toString(), w_BODY_GCRF_in_BODY.toString(), (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()) ;

        }

    }

}

TEST (Library_Astrodynamics_Flight_Profile, NadirPointing)
{

    using library::core::types::Shared ;
    using library::core::types::Real ;
    using library::core::types::String ;
    using library::core::ctnr::Array ;
    using library::core::ctnr::Table ;
    using library::core::fs::Path ;
    using library::core::fs::File ;

    using library::math::obj::Vector3d ;
    using library::math::geom::d3::trf::rot::Quaternion ;

    using library::physics::units::Length ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::Environment ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::orbit::models::Kepler ;
    using library::astro::trajectory::orbit::models::kepler::COE ;
    using library::astro::flight::Profile ;
    using library::astro::flight::profile::State ;

    // VVLH #1

    {

        const Environment environment = Environment::Default() ;

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(0.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH) ;

        const Real positionTolerance_m = 1e-3 ;
        const Real velocityTolerance_meterPerSec = 1e-6 ;
        const Real angularTolerance_asec = 0.0 ;
        const Real angularVelocityTolerance_radPerSec = 0.0 ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("../test/Library/Astrodynamics/Flight/Profile/NadirPointing/Satellite_1 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

        for (const auto& referenceRow : referenceData)
        {

            const Instant instant_ref = Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC) ;

            const Vector3d x_BODY_GCRF_ref = { referenceRow["x (m)"].accessReal(), referenceRow["y (m)"].accessReal(), referenceRow["z (m)"].accessReal() } ;
            const Vector3d v_BODY_GCRF_in_GCRF_ref = { referenceRow["vx (m/sec)"].accessReal(), referenceRow["vy (m/sec)"].accessReal(), referenceRow["vz (m/sec)"].accessReal() } ;
            const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(referenceRow["q1"].accessReal(), referenceRow["q2"].accessReal(), referenceRow["q3"].accessReal(), referenceRow["q4"].accessReal()).normalize() ;
            const Vector3d w_BODY_GCRF_in_BODY_ref = { referenceRow["wx (rad/sec)"].accessReal(), referenceRow["wy (rad/sec)"].accessReal(), referenceRow["wz (rad/sec)"].accessReal() } ;

            const State state = profile.getStateAt(instant_ref) ;

            const Vector3d x_BODY_GCRF = state.getPosition() ;
            const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity() ;
            const Quaternion q_BODY_GCRF = state.getAttitude() ;
            const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity() ;

            ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format("@ {}: {} - {} = {} [m]", instant_ref.toString(), x_BODY_GCRF.toString(), x_BODY_GCRF_ref.toString(), (x_BODY_GCRF - x_BODY_GCRF_ref).norm()) ;
            ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec)) << String::Format("@ {}: {} - {} = {} [m/s]", instant_ref.toString(), v_BODY_GCRF_in_GCRF.toString(), v_BODY_GCRF_in_GCRF_ref.toString(), (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()) ;
            ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec))) << String::Format("@ {}: {} / {} = {} [asec]", instant_ref.toString(), q_BODY_GCRF_ref.toString(), q_BODY_GCRF.toString(), q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()) ;
            // ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec)) << String::Format("@ {}: {} - {} = {} [rad/s]", instant_ref.toString(), w_BODY_GCRF_in_BODY_ref.toString(), w_BODY_GCRF_in_BODY.toString(), (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()) ;

        }

    }

    // VVLH #2

    {

        const Environment environment = Environment::Default() ;

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.0 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(0.0) ;
        const Angle aop = Angle::Degrees(0.0) ;
        const Angle trueAnomaly = Angle::Degrees(0.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH) ;

        const Real positionTolerance_m = 1e-3 ;
        const Real velocityTolerance_meterPerSec = 1e-6 ;
        const Real angularTolerance_asec = 0.0 ;
        const Real angularVelocityTolerance_radPerSec = 0.0 ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("../test/Library/Astrodynamics/Flight/Profile/NadirPointing/Satellite_2 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

        for (const auto& referenceRow : referenceData)
        {

            const Instant instant_ref = Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC) ;

            const Vector3d x_BODY_GCRF_ref = { referenceRow["x (m)"].accessReal(), referenceRow["y (m)"].accessReal(), referenceRow["z (m)"].accessReal() } ;
            const Vector3d v_BODY_GCRF_in_GCRF_ref = { referenceRow["vx (m/sec)"].accessReal(), referenceRow["vy (m/sec)"].accessReal(), referenceRow["vz (m/sec)"].accessReal() } ;
            const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(referenceRow["q1"].accessReal(), referenceRow["q2"].accessReal(), referenceRow["q3"].accessReal(), referenceRow["q4"].accessReal()).normalize() ;
            const Vector3d w_BODY_GCRF_in_BODY_ref = { referenceRow["wx (rad/sec)"].accessReal(), referenceRow["wy (rad/sec)"].accessReal(), referenceRow["wz (rad/sec)"].accessReal() } ;

            const State state = profile.getStateAt(instant_ref) ;

            const Vector3d x_BODY_GCRF = state.getPosition() ;
            const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity() ;
            const Quaternion q_BODY_GCRF = state.getAttitude() ;
            const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity() ;

            ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format("@ {}: {} - {} = {} [m]", instant_ref.toString(), x_BODY_GCRF.toString(), x_BODY_GCRF_ref.toString(), (x_BODY_GCRF - x_BODY_GCRF_ref).norm()) ;
            ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec)) << String::Format("@ {}: {} - {} = {} [m/s]", instant_ref.toString(), v_BODY_GCRF_in_GCRF.toString(), v_BODY_GCRF_in_GCRF_ref.toString(), (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()) ;
            ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec))) << String::Format("@ {}: {} / {} = {} [asec]", instant_ref.toString(), q_BODY_GCRF_ref.toString(), q_BODY_GCRF.toString(), q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()) ;
            // ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec)) << String::Format("@ {}: {} - {} = {} [rad/s]", instant_ref.toString(), w_BODY_GCRF_in_BODY_ref.toString(), w_BODY_GCRF_in_BODY.toString(), (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()) ;

        }

    }

    // VVLH #3

    {

        const Environment environment = Environment::Default() ;

        const Length semiMajorAxis = Length::Kilometers(7000.0) ;
        const Real eccentricity = 0.05 ;
        const Angle inclination = Angle::Degrees(45.0) ;
        const Angle raan = Angle::Degrees(10.0) ;
        const Angle aop = Angle::Degrees(20.0) ;
        const Angle trueAnomaly = Angle::Degrees(30.0) ;

        const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

        const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Derived gravitationalConstant = Earth::GravitationalConstant ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalConstant, equatorialRadius, J2, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH) ;

        const Real positionTolerance_m = 1e-3 ;
        const Real velocityTolerance_meterPerSec = 1e-6 ;
        const Real angularTolerance_asec = 0.0 ;
        const Real angularVelocityTolerance_radPerSec = 0.0 ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("../test/Library/Astrodynamics/Flight/Profile/NadirPointing/Satellite_3 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

        const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

        for (const auto& referenceRow : referenceData)
        {

            const Instant instant_ref = Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC) ;

            const Vector3d x_BODY_GCRF_ref = { referenceRow["x (m)"].accessReal(), referenceRow["y (m)"].accessReal(), referenceRow["z (m)"].accessReal() } ;
            const Vector3d v_BODY_GCRF_in_GCRF_ref = { referenceRow["vx (m/sec)"].accessReal(), referenceRow["vy (m/sec)"].accessReal(), referenceRow["vz (m/sec)"].accessReal() } ;
            const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(referenceRow["q1"].accessReal(), referenceRow["q2"].accessReal(), referenceRow["q3"].accessReal(), referenceRow["q4"].accessReal()).normalize() ;
            const Vector3d w_BODY_GCRF_in_BODY_ref = { referenceRow["wx (rad/sec)"].accessReal(), referenceRow["wy (rad/sec)"].accessReal(), referenceRow["wz (rad/sec)"].accessReal() } ;

            const State state = profile.getStateAt(instant_ref) ;

            const Vector3d x_BODY_GCRF = state.getPosition() ;
            const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity() ;
            const Quaternion q_BODY_GCRF = state.getAttitude() ;
            const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity() ;

            ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format("@ {}: {} - {} = {} [m]", instant_ref.toString(), x_BODY_GCRF.toString(), x_BODY_GCRF_ref.toString(), (x_BODY_GCRF - x_BODY_GCRF_ref).norm()) ;
            ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec)) << String::Format("@ {}: {} - {} = {} [m/s]", instant_ref.toString(), v_BODY_GCRF_in_GCRF.toString(), v_BODY_GCRF_in_GCRF_ref.toString(), (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()) ;
            ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec))) << String::Format("@ {}: {} / {} = {} [asec]", instant_ref.toString(), q_BODY_GCRF_ref.toString(), q_BODY_GCRF.toString(), q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()) ;
            // ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec)) << String::Format("@ {}: {} - {} = {} [rad/s]", instant_ref.toString(), w_BODY_GCRF_in_BODY_ref.toString(), w_BODY_GCRF_in_BODY.toString(), (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()) ;

        }

    }

    // VVLHGD

    // {

    //     [TBI]

    // }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////