////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Open Space Toolkit ▸ Astrodynamics
/// @file           OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/Solver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Dynamics/System.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Units/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>
#include <OpenSpaceToolkit/Physics/Units/Length.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationVector.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Models_Propagator, Test_1)
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
    using ostk::math::geom::d3::trf::rot::RotationVector ;

    using ostk::physics::units::Length ;
    using ostk::physics::units::Mass ;
    using ostk::physics::units::Angle ;
    using ostk::physics::units::Derived ;
    using ostk::physics::time::Scale ;
    using ostk::physics::time::Instant ;
    using ostk::physics::time::Duration ;
    using ostk::physics::time::Interval ;
    using ostk::physics::time::DateTime ;
    using ostk::physics::coord::Frame ;
    using ostk::physics::coord::Position ;
    using ostk::physics::coord::Velocity ;
    using ostk::physics::Environment ;
    using ostk::physics::env::obj::celest::Earth ;

    using ostk::astro::flight::dynamics::System ;
    using ostk::astro::flight::dynamics::Solver ;
    using ostk::astro::trajectory::State ;
    using ostk::astro::trajectory::Orbit ;
    using ostk::astro::trajectory::orbit::models::Propagator ;

    {

        // Environment setup

        const Environment environment = Environment::Default() ;

        // Orbital model setup

        const Shared<const Frame> gcrf = Frame::GCRF() ;

        struct Scenario
        {

            String name ;
            Solver solver ;
            File referenceDataFile ;
            Real positionTolerance_m ;
            Real velocityTolerance_mps ;

        } ;

        const Array<Scenario> scenarios =
        {

            {
                "Circular Equatorial Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Equatorial Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Equatorial Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Circular Inclined Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Inclined Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Inclined Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Circular Polar Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Polar Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Polar Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Elliptical Equatorial Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Equatorial Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Equatorial Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Elliptical Inclined Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Inclined Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Inclined Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Elliptical Polar Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Polar Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Polar Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            }

        } ;

        for (const auto& scenario : scenarios)
        {

            // Reference data setup

            const Table referenceData = Table::Load(scenario.referenceDataFile, Table::Format::CSV, true) ;

            // Initial state setup

            const State initialState =
            {
                Instant::DateTime(DateTime::Parse(referenceData[0][0].accessString()), Scale::UTC),
                Position::Meters({ referenceData[0][1].accessReal(), referenceData[0][2].accessReal(), referenceData[0][3].accessReal() }, gcrf),
                Velocity::MetersPerSecond( { referenceData[0][4].accessReal(), referenceData[0][5].accessReal(), referenceData[0][6].accessReal() }, gcrf )
            } ;

            // Propagator model setup

            const Propagator propagatorModel =
            {
                initialState,
                1,
                System { Mass::Kilograms(50.0), environment },
                scenario.solver
            } ;

            // Orbit setup

            const Orbit orbit = { propagatorModel, environment.accessCelestialObjectWithName("Earth") } ;

            // Orbit test

            for (const auto& referenceRow : referenceData)
            {

                const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;

                const Vector3d referencePosition_GCRF = { referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal() } ;
                const Vector3d referenceVelocity_GCRF = { referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal() } ;

                // const Real referenceRevolutionNumber = referenceRow[7].accessReal() ;

                const State state_GCRF = orbit.getStateAt(instant) ;

                const Position position_GCRF = state_GCRF.accessPosition() ;
                const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

                ASSERT_EQ(*Frame::GCRF(), *position_GCRF.accessFrame()) ;
                ASSERT_EQ(*Frame::GCRF(), *velocity_GCRF.accessFrame()) ;

                const String positionReport = String::Format
                (
                    "({}) {}: {} ≈ {} Δ {} > {} [m]",
                    scenario.name,
                    instant.toString(),
                    position_GCRF.accessCoordinates().toString(12),
                    referencePosition_GCRF.toString(12),
                    Real((position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()).toString(12),
                    scenario.positionTolerance_m.toString(12)
                ) ;

                const String velocityReport = String::Format
                (
                    "({}) {}: {} ≈ {} Δ {} > {} [m/s]",
                    scenario.name,
                    instant.toString(),
                    velocity_GCRF.accessCoordinates().toString(12),
                    referenceVelocity_GCRF.toString(12),
                    Real((velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()).toString(12),
                    scenario.velocityTolerance_mps.toString(12)
                ) ;

                ASSERT_TRUE(position_GCRF.accessCoordinates().isNear(referencePosition_GCRF, scenario.positionTolerance_m)) << positionReport ;
                ASSERT_TRUE(velocity_GCRF.accessCoordinates().isNear(referenceVelocity_GCRF, scenario.velocityTolerance_mps)) << velocityReport ;

            }

            // [TBI] Test getStateAt invoked in random order

        }

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
