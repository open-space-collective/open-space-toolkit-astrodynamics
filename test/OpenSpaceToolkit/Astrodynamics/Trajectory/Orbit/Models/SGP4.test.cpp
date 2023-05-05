// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>
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

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/TLE.hpp>

#include <Global.test.hpp>

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_SGP4, Test_1)
{
    using ostk::core::ctnr::Array;
    using ostk::core::ctnr::Table;
    using ostk::core::fs::File;
    using ostk::core::fs::Path;
    using ostk::core::types::Real;
    using ostk::core::types::Shared;

    using ostk::math::geom::d3::trf::rot::Quaternion;
    using ostk::math::geom::d3::trf::rot::RotationVector;
    using ostk::math::obj::Vector3d;

    using ostk::physics::Environment;
    using ostk::physics::coord::Frame;
    using ostk::physics::coord::Position;
    using ostk::physics::coord::Velocity;
    using ostk::physics::env::obj::celest::Earth;
    using ostk::physics::time::DateTime;
    using ostk::physics::time::Duration;
    using ostk::physics::time::Instant;
    using ostk::physics::time::Interval;
    using ostk::physics::time::Scale;
    using ostk::physics::units::Angle;
    using ostk::physics::units::Derived;
    using ostk::physics::units::Length;

    using ostk::astro::trajectory::Orbit;
    using ostk::astro::trajectory::State;
    using ostk::astro::trajectory::orbit::models::SGP4;
    using ostk::astro::trajectory::orbit::models::sgp4::TLE;

    {
        // Environment setup

        const Environment environment = Environment::Default();

        // Orbital model setup

        const TLE tle = TLE::Load(File::Path(
            Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/Test_1/Satellite.tle")
        ));

        const SGP4 sgp4Model = {tle};

        // Orbit setup

        const Orbit orbit = {sgp4Model, environment.accessCelestialObjectWithName("Earth")};

        // Reference data setup

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/SGP4/Test_1/Satellite Orbit.csv"
            )),
            Table::Format::CSV,
            true
        );

        // Orbit test

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

            const Vector3d referencePosition_TEME = {
                referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()};
            const Vector3d referenceVelocity_TEME = {
                referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()};

            const Vector3d referencePosition_GCRF = {
                referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()};
            const Vector3d referenceVelocity_GCRF = {
                referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()};

            const Vector3d referencePosition_ITRF = {
                referenceRow[13].accessReal(), referenceRow[14].accessReal(), referenceRow[15].accessReal()};
            const Vector3d referenceVelocity_ITRF = {
                referenceRow[16].accessReal(), referenceRow[17].accessReal(), referenceRow[18].accessReal()};

            // const Real referenceRevolutionNumber = referenceRow[19].accessReal() ;

            const State state_GCRF = orbit.getStateAt(instant);

            const Position position_GCRF = state_GCRF.accessPosition();
            const Velocity velocity_GCRF = state_GCRF.accessVelocity();

            EXPECT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame());
            EXPECT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame());

            EXPECT_GT(10.0, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm());
            EXPECT_GT(1e-2, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm());

            const Shared<const Frame> temeOfEpochFrame = Frame::TEMEOfEpoch(tle.getEpoch());

            const State state_TEME = state_GCRF.inFrame(temeOfEpochFrame);

            const Position position_TEME = state_TEME.accessPosition();
            const Velocity velocity_TEME = state_TEME.accessVelocity();

            // STK

            // const Quaternion ref_q_TEME_GCRF = Quaternion::XYZS(0.000013730697, 0.000889461232, -0.002075913109,
            // 0.999997449624).normalize() ;

            // OUT

            // const Quaternion q_TEME_GCRF = Frame::GCRF()->getTransformTo(Frame::TEME(), instant).getOrientation() ;

            // std::cout << "[REF] q_TEME_GCRF = " << ref_q_TEME_GCRF.toString(12) << std::endl ;

            // std::cout << "[REF] x_GCRF: " << referencePosition_GCRF.toString(12) << std::endl ;
            // std::cout << "[REF] x_TEME: " << referencePosition_TEME.toString(12) << std::endl ;

            // std::cout << "q_TEME_GCRF = " << q_TEME_GCRF.toString(12) << std::endl ;

            // std::cout << "x_GCRF: " << position_GCRF.accessCoordinates().toString(12) << std::endl ;
            // std::cout << "x_TEME: " << position_TEME.accessCoordinates().toString(12) << std::endl ;

            // std::cout << "dq = " << RotationVector::Quaternion((q_TEME_GCRF /
            // ref_q_TEME_GCRF).normalize().rectify()).getAngle().inArcseconds().toString(12) << " [asec]" << std::endl
            // ; std::cout << "dx_GCRF = " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " [m]" << std::endl ; std::cout << "dx_TEME = " <<
            // Real((position_TEME.accessCoordinates() - referencePosition_TEME).norm()).toString(12) << " [m]" <<
            // std::endl ;

            // std::cout << "x_GCRF A: " << (ref_q_TEME_GCRF.toConjugate() *
            // position_TEME.accessCoordinates()).toString(12) << " [m]" << std::endl ; std::cout << "x_GCRF B: " <<
            // (q_TEME_GCRF.toConjugate() * position_TEME.accessCoordinates()).toString(12) << " [m]" << std::endl ;

            // std::cout << "position_GCRF 1 = " << std::endl << position_GCRF << std::endl ;
            // std::cout << "position_GCRF 2 = " << std::endl <<
            // state_GCRF.inFrame(temeOfEpochFrame).inFrame(Frame::GCRF()).accessPosition() << std::endl ;

            // std::cout << "position_TEME 1 = " << std::endl << position_TEME << std::endl ;
            // std::cout << "position_TEME 2 = " << std::endl <<
            // state_TEME.inFrame(Frame::GCRF()).inFrame(temeOfEpochFrame).accessPosition() << std::endl ;

            EXPECT_EQ(*Frame::TEMEOfEpoch(tle.getEpoch()), *position_TEME.accessFrame());
            EXPECT_EQ(*Frame::TEMEOfEpoch(tle.getEpoch()), *velocity_TEME.accessFrame());

            EXPECT_GT(10.0, (position_TEME.accessCoordinates() - referencePosition_TEME).norm());
            EXPECT_GT(1e-2, (velocity_TEME.accessCoordinates() - referenceVelocity_TEME).norm());

            const Shared<const Frame> itrfFrame = Frame::ITRF();

            const State state_ITRF = state_GCRF.inFrame(itrfFrame);

            const Position position_ITRF = state_ITRF.accessPosition();
            const Velocity velocity_ITRF = state_ITRF.accessVelocity();

            EXPECT_EQ(*Frame::ITRF(), *position_ITRF.accessFrame());
            EXPECT_EQ(*Frame::ITRF(), *velocity_ITRF.accessFrame());

            EXPECT_GT(10.0, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm());
            EXPECT_GT(1e-2, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm());

            // EXPECT_EQ(referenceRevolutionNumber.floor(), orbit.getRevolutionNumberAt(instant)) ;

            // std::cout << "x @ GCRF = " << referencePosition_GCRF.toString(10) << " / " <<
            // position_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "x @ ITRF = " <<
            // referencePosition_ITRF.toString(10) << " / " << position_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dx = " << (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()
            // << " - " << (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm() << std::endl ;

            // std::cout << "v @ GCRF = " << referenceVelocity_GCRF.toString(10) << " / " <<
            // velocity_GCRF.accessCoordinates().toString(10) << std::endl ; std::cout << "v @ ITRF = " <<
            // referenceVelocity_ITRF.toString(10) << " / " << velocity_ITRF.accessCoordinates().toString(10) <<
            // std::endl ; std::cout << "dv = " << (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()
            // << " - " << (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm() << std::endl ;

            // std::cout   << "dx = " << Real((position_TEME.accessCoordinates() -
            // referencePosition_TEME).norm()).toString(12) << " - " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12)
            //    << " | " << "dv = " << Real((velocity_TEME.accessCoordinates() -
            //    referenceVelocity_TEME).norm()).toString(12) << " - " << Real((velocity_GCRF.accessCoordinates() -
            //    referenceVelocity_GCRF).norm()).toString(12) << std::endl ;

            // std::cout   << "dx = " << Real((position_TEME.accessCoordinates() -
            // referencePosition_TEME).norm()).toString(12) << " - " << Real((position_GCRF.accessCoordinates() -
            // referencePosition_GCRF).norm()).toString(12) << " - " << Real((position_ITRF.accessCoordinates() -
            // referencePosition_ITRF).norm()).toString(12)
            //    << " | " << "dv = " << Real((velocity_TEME.accessCoordinates() -
            //    referenceVelocity_TEME).norm()).toString(12) << " - " << Real((velocity_GCRF.accessCoordinates() -
            //    referenceVelocity_GCRF).norm()).toString(12) << " - " << Real((velocity_ITRF.accessCoordinates() -
            //    referenceVelocity_ITRF).norm()).toString(12) << std::endl ;

            // FAIL() ;
        }
    }
}
