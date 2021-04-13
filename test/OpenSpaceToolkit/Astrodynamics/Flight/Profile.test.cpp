////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Flight/Profile.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, Constructor)
{

    using ostk::core::types::Shared ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Transform ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::frame::provider::Dynamic ;

    using ostk::astro::flight::Profile ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, StreamOperator)
{

    using ostk::core::types::Shared ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Transform ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::frame::provider::Dynamic ;

    using ostk::astro::flight::Profile ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, IsDefined)
{

    using ostk::core::types::Shared ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Transform ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::frame::provider::Dynamic ;

    using ostk::astro::flight::Profile ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, GetStateAt)
{

    using ostk::core::types::Shared ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Instant ;
    using ostk::physics::coord::Transform ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::frame::provider::Dynamic ;

    using ostk::astro::flight::Profile ;
    using ostk::astro::flight::profile::State ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, GetStatesAt)
{

    using ostk::core::types::Shared ;
    using ostk::core::ctnr::Array ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Transform ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::frame::provider::Dynamic ;

    using ostk::astro::flight::Profile ;
    using ostk::astro::flight::profile::State ;

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

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, Undefined)
{

    using ostk::astro::flight::Profile ;

    {

        EXPECT_NO_THROW(Profile::Undefined()) ;
        EXPECT_FALSE(Profile::Undefined().isDefined()) ;

    }

}

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, InertialPointing)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::types::String ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::orbit::models::Kepler ;
    using ostk::astro::trajectory::orbit::models::kepler::COE ;
    using ostk::astro::flight::Profile ;
    using ostk::astro::flight::profile::State ;

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
        const Derived gravitationalParameter = Earth::GravitationalParameter ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;
        const Real J4 = Earth::J4 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        const Profile profile = Profile::InertialPointing(orbit, Quaternion::Unit()) ;

        const Real positionTolerance_m = 1e-3 ;
        const Real velocityTolerance_meterPerSec = 1e-6 ;
        const Real angularTolerance_asec = 0.0 ;
        const Real angularVelocityTolerance_radPerSec = 1e-10 ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/InertialPointing/Satellite t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

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

// TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, NadirPointing_LVLH)
// {

//     using ostk::core::types::Shared ;
//     using ostk::core::types::Real ;
//     using ostk::core::types::String ;
//     using ostk::core::ctnr::Array ;
//     using ostk::core::ctnr::Table ;
//     using ostk::core::fs::Path ;
//     using ostk::core::fs::File ;

//     using ostk::math::obj::Vector3d ;
//     using ostk::math::geom::d3::trf::rot::Quaternion ;

//     using ostk::physics::units::Length ;
//     using ostk::physics::units::Angle ;
//     using ostk::physics::units::Derived ;
//     using ostk::physics::time::Scale ;
//     using ostk::physics::time::Instant ;
//     using ostk::physics::time::Duration ;
//     using ostk::physics::time::Interval ;
//     using ostk::physics::time::DateTime ;
//     using ostk::physics::coord::Frame ;
//     using ostk::physics::Environment ;
//     using ostk::physics::env::obj::celest::Earth ;

//     using ostk::astro::trajectory::Orbit ;
//     using ostk::astro::trajectory::orbit::models::Kepler ;
//     using ostk::astro::trajectory::orbit::models::kepler::COE ;
//     using ostk::astro::flight::Profile ;
//     using ostk::astro::flight::profile::State ;

//     // LVLH #1

//     {

//         const Environment environment = Environment::Default() ;

//         const Length semiMajorAxis = Length::Kilometers(7000.0) ;
//         const Real eccentricity = 0.0 ;
//         const Angle inclination = Angle::Degrees(0.0) ;
//         const Angle raan = Angle::Degrees(0.0) ;
//         const Angle aop = Angle::Degrees(0.0) ;
//         const Angle trueAnomaly = Angle::Degrees(0.0) ;

//         const COE coe = { semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly } ;

//         const Instant epoch = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
//         const Derived gravitationalParameter = Earth::GravitationalParameter ;
//         const Length equatorialRadius = Earth::EquatorialRadius ;
//         const Real J2 = Earth::J2 ;
//         const Real J4 = Earth::J4 ;

//         const Kepler keplerianModel = { coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None } ;

//         const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

//         const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::LVLH) ;

//         const Real positionTolerance_m = 1e-3 ;
//         const Real velocityTolerance_meterPerSec = 1e-6 ;
//         const Real angularTolerance_asec = 0.0 ;
//         const Real angularVelocityTolerance_radPerSec = 1e-10 ;

//         // Reference data setup

//         const File referenceDataFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/NadirPointing/VVLH/Satellite_1 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

//         const Table referenceData = Table::Load(referenceDataFile, Table::Format::CSV, true) ;

//         for (const auto& referenceRow : referenceData)
//         {

//             const Instant instant_ref = Instant::DateTime(DateTime::Parse(referenceRow["Time (UTCG)"].accessString()), Scale::UTC) ;

//             const Vector3d x_BODY_GCRF_ref = { referenceRow["x (m)"].accessReal(), referenceRow["y (m)"].accessReal(), referenceRow["z (m)"].accessReal() } ;
//             const Vector3d v_BODY_GCRF_in_GCRF_ref = { referenceRow["vx (m/sec)"].accessReal(), referenceRow["vy (m/sec)"].accessReal(), referenceRow["vz (m/sec)"].accessReal() } ;
//             const Quaternion q_BODY_GCRF_ref = Quaternion::XYZS(referenceRow["q1"].accessReal(), referenceRow["q2"].accessReal(), referenceRow["q3"].accessReal(), referenceRow["q4"].accessReal()).normalize() ;
//             const Vector3d w_BODY_GCRF_in_BODY_ref = { referenceRow["wx (rad/sec)"].accessReal(), referenceRow["wy (rad/sec)"].accessReal(), referenceRow["wz (rad/sec)"].accessReal() } ;

//             const State state = profile.getStateAt(instant_ref) ;

//             const Vector3d x_BODY_GCRF = state.getPosition() ;
//             const Vector3d v_BODY_GCRF_in_GCRF = state.getVelocity() ;
//             const Quaternion q_BODY_GCRF = state.getAttitude() ;
//             const Vector3d w_BODY_GCRF_in_BODY = state.getAngularVelocity() ;

//             ASSERT_TRUE(x_BODY_GCRF.isNear(x_BODY_GCRF_ref, positionTolerance_m)) << String::Format("@ {}: {} - {} = {} [m]", instant_ref.toString(), x_BODY_GCRF.toString(), x_BODY_GCRF_ref.toString(), (x_BODY_GCRF - x_BODY_GCRF_ref).norm()) ;
//             ASSERT_TRUE(v_BODY_GCRF_in_GCRF.isNear(v_BODY_GCRF_in_GCRF_ref, velocityTolerance_meterPerSec)) << String::Format("@ {}: {} - {} = {} [m/s]", instant_ref.toString(), v_BODY_GCRF_in_GCRF.toString(), v_BODY_GCRF_in_GCRF_ref.toString(), (v_BODY_GCRF_in_GCRF - v_BODY_GCRF_in_GCRF_ref).norm()) ;
//             ASSERT_TRUE(q_BODY_GCRF.isNear(q_BODY_GCRF_ref, Angle::Arcseconds(angularTolerance_asec))) << String::Format("@ {}: {} / {} = {} [asec]", instant_ref.toString(), q_BODY_GCRF_ref.toString(), q_BODY_GCRF.toString(), q_BODY_GCRF.angularDifferenceWith(q_BODY_GCRF_ref).inArcseconds().toString()) ;
//             ASSERT_TRUE(w_BODY_GCRF_in_BODY.isNear(w_BODY_GCRF_in_BODY_ref, angularVelocityTolerance_radPerSec)) << String::Format("@ {}: {} - {} = {} [rad/s]", instant_ref.toString(), w_BODY_GCRF_in_BODY_ref.toString(), w_BODY_GCRF_in_BODY.toString(), (w_BODY_GCRF_in_BODY - w_BODY_GCRF_in_BODY_ref).norm()) ;

//         }

//     }

// }

TEST (OpenSpaceToolkit_Astrodynamics_Flight_Profile, NadirPointing_VVLH)
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::types::String ;
    using ostk::core::ctnr::Array ;
    using ostk::core::ctnr::Table ;
    using ostk::core::fs::Path ;
    using ostk::core::fs::File ;

    using ostk::math::obj::Vector3d ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::orbit::models::Kepler ;
    using ostk::astro::trajectory::orbit::models::kepler::COE ;
    using ostk::astro::flight::Profile ;
    using ostk::astro::flight::profile::State ;

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
        const Derived gravitationalParameter = Earth::GravitationalParameter ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;
        const Real J4 = Earth::J4 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH) ;

        const Real positionTolerance_m = 1e-3 ;
        const Real velocityTolerance_meterPerSec = 1e-6 ;
        const Real angularTolerance_asec = 0.0 ;
        const Real angularVelocityTolerance_radPerSec = 1e-10 ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/NadirPointing/Satellite_1 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

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
        const Derived gravitationalParameter = Earth::GravitationalParameter ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;
        const Real J4 = Earth::J4 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH) ;

        const Real positionTolerance_m = 1e-3 ;
        const Real velocityTolerance_meterPerSec = 1e-6 ;
        const Real angularTolerance_asec = 0.0 ;
        const Real angularVelocityTolerance_radPerSec = 1e-10 ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/NadirPointing/Satellite_2 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

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
        const Derived gravitationalParameter = Earth::GravitationalParameter ;
        const Length equatorialRadius = Earth::EquatorialRadius ;
        const Real J2 = Earth::J2 ;
        const Real J4 = Earth::J4 ;

        const Kepler keplerianModel = { coe, epoch, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None } ;

        const Orbit orbit = { keplerianModel, environment.accessCelestialObjectWithName("Earth") } ;

        const Profile profile = Profile::NadirPointing(orbit, Orbit::FrameType::VVLH) ;

        const Real positionTolerance_m = 1e-3 ;
        const Real velocityTolerance_meterPerSec = 1e-6 ;
        const Real angularTolerance_asec = 0.0 ;
        const Real angularVelocityTolerance_radPerSec = 1e-7 ;

        // Reference data setup

        const File referenceDataFile = File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Flight/Profile/NadirPointing/Satellite_3 t_UTC x_GCRF v_GCRF q_B_GCRF w_B_GCRF_in_GCRF.csv")) ;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
