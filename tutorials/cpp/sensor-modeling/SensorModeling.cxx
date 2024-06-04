/// Apache License 2.0

#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/2D/Object/Polygon.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/LineString.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Polygon.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Pyramid.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/RotationVector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Geometry.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

int main()
{
    using ostk::core::container::Array;
    using ostk::core::type::Real;
    using ostk::core::type::Shared;

    using ostk::mathematics::object::Vector3d;
    using Point2d = ostk::mathematics::geometry::d2::object::Point;
    using Polygon2d = ostk::mathematics::geometry::d2::object::Polygon;
    using Point3d = ostk::mathematics::geometry::d3::object::Point;
    using LineString3d = ostk::mathematics::geometry::d3::object::LineString;
    using Polygon3d = ostk::mathematics::geometry::d3::object::Polygon;
    using ostk::mathematics::geometry::d3::object::Pyramid;
    using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
    using ostk::mathematics::geometry::d3::transformation::rotation::RotationMatrix;

    using ostk::physics::coordinate::Frame;
    using ostk::physics::coordinate::frame::Provider;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::unit::Angle;
    using ostk::physics::unit::Derived;
    using ostk::physics::unit::Length;
    using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;
    using ostk::physics::coordinate::Position;
    using ostk::physics::coordinate::spherical::LLA;
    using ostk::physics::coordinate::Transform;
    using ostk::physics::coordinate::Velocity;
    using ostk::physics::Environment;
    using ostk::physics::environment::object::Celestial;
    using ostk::physics::environment::object::celestial::Earth;
    using ostk::physics::environment::object::Geometry;

    using ostk::astrodynamics::trajectory::Orbit;
    using ostk::astrodynamics::trajectory::orbit::model::Kepler;
    using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
    using ostk::astrodynamics::trajectory::State;

    // Environment

    const Environment environment = Environment::Default();
    const Shared<const Celestial> earthSPtr = environment.accessCelestialObjectWithName("Earth");

    // Orbital parameters

    const Length a = Length::Kilometers(7000.0);
    const Real e = 0.0;
    const Angle i = Angle::Degrees(0.0);
    const Angle raan = Angle::Degrees(0.0);
    const Angle aop = Angle::Degrees(0.0);
    const Angle nu = Angle::Degrees(0.0);

    const COE coe = {a, e, i, raan, aop, nu};

    std::cout << coe << std::endl;

    // Orbit

    const Instant epoch = Instant::DateTime(DateTime(2018, 9, 5, 0, 0, 0), Scale::UTC);

    const Kepler orbitalModel = {
        coe, epoch, *earthSPtr, Kepler::PerturbationType::None, true
    };  // True = COE expressed in ITRF frame

    const Orbit orbit = {orbitalModel, earthSPtr};

    std::cout << orbit << std::endl;

    // Target

    const Angle latitude = Angle::Degrees(0.0);
    const Angle longitude = Angle::Degrees(0.0);
    const Length altitude = Length::Meters(10.0);

    const LLA targetLla = {latitude, longitude, altitude};

    const Position targetPosition =
        Position::Meters(targetLla.toCartesian(Earth::EquatorialRadius, Earth::Flattening), Frame::ITRF());

    std::cout << targetPosition << std::endl;

    // Body frame definition

    const Shared<const Frame> orbitalFrame = orbit.getOrbitalFrame(Orbit::FrameType::NED);

    const auto calculateAttitude = [&targetPosition](const State& aState) -> Quaternion
    {
        const Vector3d sensorPosition_ITRF = aState.inFrame(Frame::ITRF()).getPosition().getCoordinates();
        // std::cout << "sensorPosition_ITRF = " << sensorPosition_ITRF.toString() << std::endl ;
        const Vector3d targetPosition_ITRF = targetPosition.getCoordinates();
        // std::cout << "targetPosition_ITRF = " << targetPosition_ITRF.toString() << std::endl ;

        const Vector3d sensorToTargetDirection_ITRF = (targetPosition_ITRF - sensorPosition_ITRF).normalized();
        // std::cout << "sensorToTargetDirection_ITRF = " << sensorToTargetDirection_ITRF.toString() << std::endl ;

        const Vector3d z_B_ITRF = sensorToTargetDirection_ITRF;
        const Vector3d x_B_ITRF = z_B_ITRF.cross(Vector3d::Y()).normalized();
        const Vector3d y_B_ITRF = z_B_ITRF.cross(x_B_ITRF);

        const RotationMatrix dcm_B_ITRF = RotationMatrix::Rows(x_B_ITRF, y_B_ITRF, z_B_ITRF);
        const Quaternion q_B_ITRF = Quaternion::RotationMatrix(dcm_B_ITRF).toNormalized();

        const Quaternion q_ITRF_GCRF =
            Frame::GCRF()->getTransformTo(Frame::ITRF(), aState.getInstant()).getOrientation();

        const Quaternion q_B_GCRF = (q_B_ITRF * q_ITRF_GCRF).toNormalized();

        return q_B_GCRF;
    };

    const auto bodyFrameTransformGenerator = [&orbit, calculateAttitude](const Instant& anInstant) -> Transform
    {
        const State state = orbit.getStateAt(anInstant);

        const Vector3d x_B_GCRF = state.getPosition().getCoordinates();
        const Quaternion q_B_GCRF = calculateAttitude(state);

        std::cout << "x_B_GCRF = " << x_B_GCRF.toString() << std::endl;

        return Transform::Passive(anInstant, -x_B_GCRF, Vector3d::Zero(), q_B_GCRF, Vector3d::Zero());
    };

    const Shared<const Provider> bodyFrameProvider = std::make_shared<DynamicProvider>(bodyFrameTransformGenerator);

    const Shared<const Frame> bodyFrameSPtr = Frame::Construct("Body", false, Frame::GCRF(), bodyFrameProvider);

    // Sensor geometry

    const Point3d apex_B = {0.0, 0.0, 0.0};
    const Polygon3d base_B = {
        Polygon2d({{-1.0, -1.0}, {+1.0, -1.0}, {+1.0, +1.0}, {-1.0, +1.0}}),
        apex_B + Vector3d(0.0, 0.0, 5.0),
        Vector3d(1.0, 0.0, 0.0),
        Vector3d(0.0, 1.0, 0.0)
    };

    const Pyramid pyramid_B = {base_B, apex_B};

    const Geometry sensorGeometry = {pyramid_B, bodyFrameSPtr};

    // Analysis

    const auto calculateSensorTrace = [earthSPtr, &targetLla, &sensorGeometry](const State& aState) -> Polygon2d
    {
        const Vector3d sensorPosition_ITRF = aState.inFrame(Frame::ITRF()).getPosition().getCoordinates();
        std::cout << "Sensor position [ITRF] = " << sensorPosition_ITRF.toString() << std::endl;

        const Shared<const Frame> nedFrameSPtr = earthSPtr->getFrameAt(targetLla, Celestial::FrameType::NED);

        // std::cout << "Earth [ITRF] = " << std::endl << earthSPtr->getGeometryIn(Frame::ITRF()) << std::endl ;
        std::cout << "Sensor geometry [B] = " << std::endl << sensorGeometry << std::endl;
        std::cout << "Sensor geometry [GCRF] = " << std::endl
                  << sensorGeometry.in(Frame::GCRF(), aState.getInstant()) << std::endl;
        std::cout << "Sensor geometry [ITRF] = " << std::endl
                  << sensorGeometry.in(Frame::ITRF(), aState.getInstant()) << std::endl;

        const Geometry intersectionGeometry = sensorGeometry.in(Frame::ITRF(), aState.getInstant())
                                                  .intersectionWith(earthSPtr->getGeometryIn(Frame::ITRF()));

        // std::cout << "intersectionGeometry = " << std::endl << intersectionGeometry << std::endl ;

        Array<Point2d> intersectionPoints_LL = Array<Point2d>::Empty();

        for (const auto& intersectionPoint_ITRF :
             intersectionGeometry.accessComposite().accessObjectAt(0).as<LineString3d>())
        {
            const LLA intersectionPoint_LLA =
                LLA::Cartesian(intersectionPoint_ITRF.asVector(), Earth::EquatorialRadius, Earth::Flattening);

            intersectionPoints_LL.add(Point2d(
                intersectionPoint_LLA.getLongitude().inDegrees(), intersectionPoint_LLA.getLatitude().inDegrees()
            ));
        }

        const Polygon2d intersectionPolygon_LL = {intersectionPoints_LL};

        return intersectionPolygon_LL;
    };

    const Instant analysisStartInstant = epoch;
    const Instant analysisEndInstant = analysisStartInstant + Duration::Minutes(1.0);

    const Duration analysisStep = Duration::Minutes(1.0);

    const Interval analysisInterval = Interval::Closed(analysisStartInstant, analysisEndInstant);

    const Array<State> orbitalStates = orbit.getStatesAt(analysisInterval.generateGrid(analysisStep));

    for (const auto& orbitalState : orbitalStates)
    {
        // std::cout << orbitalState << std::endl ;

        const Polygon2d sensorTrace_LL = calculateSensorTrace(orbitalState);

        // std::cout << sensorTrace_LL << std::endl ;
    }
}
