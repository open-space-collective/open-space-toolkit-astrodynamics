////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           tutorials/cpp/sensor-modeling/SensorModeling.cxx
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Geometry.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Providers/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationVector.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Pyramid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Polygon.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/LineString.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Objects/Polygon.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Objects/Point.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int                             main                                        ( )
{

    using ostk::core::types::Shared ;
    using ostk::core::types::Real ;
    using ostk::core::ctnr::Array ;

    using ostk::math::obj::Vector3d ;
    using Point2d = ostk::math::geom::d2::objects::Point ;
    using Polygon2d = ostk::math::geom::d2::objects::Polygon ;
    using Point3d = ostk::math::geom::d3::objects::Point ;
    using LineString3d = ostk::math::geom::d3::objects::LineString ;
    using Polygon3d = ostk::math::geom::d3::objects::Polygon ;
    using ostk::math::geom::d3::objects::Pyramid ;
    using ostk::math::geom::d3::trf::rot::Quaternion ;
    using ostk::math::geom::d3::trf::rot::RotationMatrix ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::frame::Provider ;
    using DynamicProvider = ostk::physics::coord::frame::provider::Dynamic ;
    using ostk::physics::coord::Transform ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::coord::spherical::LLA ;
    using ostk::physics::Environment ;
    using ostk::physics::env::object::Geometry ;
    using ostk::physics::env::obj::Celestial ;
    using ostk::physics::env::obj::celest::Earth ;

    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::trajectory::orbit::models::Kepler ;
    using ostk::astro::trajectory::orbit::models::kepler::COE ;

    // Environment

    const Environment environment = Environment::Default() ;
    const Shared<const Celestial> earthSPtr = environment.accessCelestialObjectWithName("Earth") ;

    // Orbital parameters

    const Length a = Length::Kilometers(7000.0) ;
    const Real e = 0.0 ;
    const Angle i = Angle::Degrees(0.0) ;
    const Angle raan = Angle::Degrees(0.0) ;
    const Angle aop = Angle::Degrees(0.0) ;
    const Angle nu = Angle::Degrees(0.0) ;

    const COE coe = { a, e, i, raan, aop, nu } ;

    std::cout << coe << std::endl ;

    // Orbit

    const Instant epoch = Instant::DateTime(DateTime(2018, 9, 5, 0, 0, 0), Scale::UTC) ;

    const Kepler orbitalModel = { coe, epoch, *earthSPtr, Kepler::PerturbationType::None, true } ; // True = COE expressed in ITRF frame

    const Orbit orbit = { orbitalModel, earthSPtr } ;

    std::cout << orbit << std::endl ;

    // Target

    const Angle latitude = Angle::Degrees(0.0) ;
    const Angle longitude = Angle::Degrees(0.0) ;
    const Length altitude = Length::Meters(10.0) ;

    const LLA targetLla = { latitude, longitude, altitude } ;

    const Position targetPosition = Position::Meters(targetLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF()) ;

    std::cout << targetPosition << std::endl ;

    // Body frame definition

    const Shared<const Frame> orbitalFrame = orbit.getOrbitalFrame(Orbit::FrameType::NED) ;

    const auto calculateAttitude = [&targetPosition] (const State& aState) -> Quaternion
    {

        const Vector3d sensorPosition_ITRF = aState.inFrame(Frame::ITRF()).getPosition().getCoordinates() ;
        // std::cout << "sensorPosition_ITRF = " << sensorPosition_ITRF.toString() << std::endl ;
        const Vector3d targetPosition_ITRF = targetPosition.getCoordinates() ;
        // std::cout << "targetPosition_ITRF = " << targetPosition_ITRF.toString() << std::endl ;

        const Vector3d sensorToTargetDirection_ITRF = (targetPosition_ITRF - sensorPosition_ITRF).normalized() ;
        // std::cout << "sensorToTargetDirection_ITRF = " << sensorToTargetDirection_ITRF.toString() << std::endl ;

        const Vector3d z_B_ITRF = sensorToTargetDirection_ITRF ;
        const Vector3d x_B_ITRF = z_B_ITRF.cross(Vector3d::Y()).normalized() ;
        const Vector3d y_B_ITRF = z_B_ITRF.cross(x_B_ITRF) ;

        const RotationMatrix dcm_B_ITRF = RotationMatrix::Rows(x_B_ITRF, y_B_ITRF, z_B_ITRF) ;
        const Quaternion q_B_ITRF = Quaternion::RotationMatrix(dcm_B_ITRF).toNormalized() ;

        const Quaternion q_ITRF_GCRF = Frame::GCRF()->getTransformTo(Frame::ITRF(), aState.getInstant()).getOrientation() ;

        const Quaternion q_B_GCRF = (q_B_ITRF * q_ITRF_GCRF).toNormalized() ;

        return q_B_GCRF ;

    } ;

    const auto bodyFrameTransformGenerator = [&orbit, calculateAttitude] (const Instant& anInstant) -> Transform
    {

        const State state = orbit.getStateAt(anInstant) ;

        const Vector3d x_B_GCRF = state.getPosition().getCoordinates() ;
        const Quaternion q_B_GCRF = calculateAttitude(state) ;

        std::cout << "x_B_GCRF = " << x_B_GCRF.toString() << std::endl ;

        return Transform::Passive(anInstant, -x_B_GCRF, Vector3d::Zero(), q_B_GCRF, Vector3d::Zero()) ;

    } ;

    const Shared<const Provider> bodyFrameProvider = std::make_shared<DynamicProvider>(bodyFrameTransformGenerator) ;

    const Shared<const Frame> bodyFrameSPtr = Frame::Construct("Body", false, Frame::GCRF(), bodyFrameProvider) ;

    // Sensor geometry

    const Point3d apex_B = { 0.0, 0.0, 0.0 } ;
    const Polygon3d base_B = { Polygon2d({ { -1.0, -1.0 }, { +1.0, -1.0 }, { +1.0, +1.0 }, { -1.0, +1.0 } }), apex_B + Vector3d(0.0, 0.0, 5.0), Vector3d(1.0, 0.0, 0.0), Vector3d(0.0, 1.0, 0.0) } ;

    const Pyramid pyramid_B = { base_B, apex_B } ;

    const Geometry sensorGeometry = { pyramid_B, bodyFrameSPtr } ;

    // Analysis

    const auto calculateSensorTrace = [earthSPtr, &targetLla, &sensorGeometry] (const State& aState) -> Polygon2d
    {

        const Vector3d sensorPosition_ITRF = aState.inFrame(Frame::ITRF()).getPosition().getCoordinates() ;
        std::cout << "Sensor position [ITRF] = " << sensorPosition_ITRF.toString() << std::endl ;

        const Shared<const Frame> nedFrameSPtr = earthSPtr->getFrameAt(targetLla, Celestial::FrameType::NED) ;

        // std::cout << "Earth [ITRF] = " << std::endl << earthSPtr->getGeometryIn(Frame::ITRF()) << std::endl ;
        std::cout << "Sensor geometry [B] = " << std::endl << sensorGeometry << std::endl ;
        std::cout << "Sensor geometry [GCRF] = " << std::endl << sensorGeometry.in(Frame::GCRF(), aState.getInstant()) << std::endl ;
        std::cout << "Sensor geometry [ITRF] = " << std::endl << sensorGeometry.in(Frame::ITRF(), aState.getInstant()) << std::endl ;

        const Geometry intersectionGeometry = sensorGeometry.in(Frame::ITRF(), aState.getInstant()).intersectionWith(earthSPtr->getGeometryIn(Frame::ITRF())) ;

        // std::cout << "intersectionGeometry = " << std::endl << intersectionGeometry << std::endl ;

        Array<Point2d> intersectionPoints_LL = Array<Point2d>::Empty() ;

        for (const auto& intersectionPoint_ITRF : intersectionGeometry.accessComposite().accessObjectAt(0).as<LineString3d>())
        {

            const LLA intersectionPoint_LLA = LLA::Cartesian(intersectionPoint_ITRF.asVector(), Earth::EquatorialRadius, Earth::Flattening) ;

            intersectionPoints_LL.add(Point2d(intersectionPoint_LLA.getLongitude().inDegrees(), intersectionPoint_LLA.getLatitude().inDegrees())) ;

        }

        const Polygon2d intersectionPolygon_LL = { intersectionPoints_LL } ;

        return intersectionPolygon_LL ;

    } ;

    const Instant analysisStartInstant = epoch ;
    const Instant analysisEndInstant = analysisStartInstant + Duration::Minutes(1.0) ;

    const Duration analysisStep = Duration::Minutes(1.0) ;

    const Interval analysisInterval = Interval::Closed(analysisStartInstant, analysisEndInstant) ;

    const Array<State> orbitalStates = orbit.getStatesAt(analysisInterval.generateGrid(analysisStep)) ;

    for (const auto& orbitalState : orbitalStates)
    {

        // std::cout << orbitalState << std::endl ;

        const Polygon2d sensorTrace_LL = calculateSensorTrace(orbitalState) ;

        // std::cout << sensorTrace_LL << std::endl ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
