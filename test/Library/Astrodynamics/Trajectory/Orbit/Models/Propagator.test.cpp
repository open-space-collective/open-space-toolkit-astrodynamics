////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Models/Propagator.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Models/Propagator.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/Solver.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/System.hpp>

#include <Library/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <Library/Physics/Environment.hpp>
#include <Library/Physics/Time/DateTime.hpp>
#include <Library/Physics/Time/Interval.hpp>
#include <Library/Physics/Time/Duration.hpp>
#include <Library/Physics/Time/Instant.hpp>
#include <Library/Physics/Time/Scale.hpp>
#include <Library/Physics/Units/Derived/Angle.hpp>
#include <Library/Physics/Units/Derived.hpp>
#include <Library/Physics/Units/Mass.hpp>
#include <Library/Physics/Units/Length.hpp>

#include <Library/Mathematics/Geometry/3D/Transformations/Rotations/RotationVector.hpp>
#include <Library/Mathematics/Geometry/3D/Transformations/Rotations/Quaternion.hpp>
#include <Library/Mathematics/Objects/Vector.hpp>

#include <Library/Core/Containers/Table.hpp>
#include <Library/Core/Containers/Array.hpp>
#include <Library/Core/Types/Real.hpp>
#include <Library/Core/Types/Shared.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Trajectory_Orbit_Models_Propagator, Test_1)
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
    using library::math::geom::d3::trf::rot::RotationVector ;

    using library::physics::units::Length ;
    using library::physics::units::Mass ;
    using library::physics::units::Angle ;
    using library::physics::units::Derived ;
    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::Duration ;
    using library::physics::time::Interval ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Frame ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::Environment ;
    using library::physics::env::obj::celest::Earth ;

    using library::astro::flight::dynamics::System ;
    using library::astro::flight::dynamics::Solver ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::Orbit ;
    using library::astro::trajectory::orbit::models::Propagator ;

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
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Equatorial Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Equatorial Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Circular Inclined Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Inclined Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Inclined Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Circular Polar Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Polar Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Circular Polar Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/CircularPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Elliptical Equatorial Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Equatorial Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Equatorial Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalEquatorialOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Elliptical Inclined Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Inclined Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Inclined Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalInclinedOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },

            {
                "Elliptical Polar Orbit - Runge-Kutta 4",
                Solver::RungeKutta4(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Polar Orbit - Runge-Kutta Dormand-Prince 5",
                Solver::RungeKuttaDormandPrince5(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
                1e-4, // [m]
                1e-7 // [m/s]
            },
            {
                "Elliptical Polar Orbit - Runge-Kutta Fehlberg 78",
                Solver::RungeKuttaFehlberg78(1e-16, 1e-16),
                File::Path(Path::Parse("../test/Library/Astrodynamics/Trajectory/Orbit/Models/Propagator/Test_1/EllipticalPolarOrbit t_UTC x_GCRF v_GCRF rev.csv")),
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
                System { Mass::Kilograms(50.0) },
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
