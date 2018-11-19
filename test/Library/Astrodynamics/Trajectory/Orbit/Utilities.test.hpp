////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Utilities.test.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
#include <Library/Core/Types/Integer.hpp>
#include <Library/Core/Types/Shared.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using library::core::types::Shared ;
using library::core::types::Real ;
using library::core::types::String ;
using library::core::ctnr::Array ;
using library::core::ctnr::Table ;
using library::core::fs::Path ;
using library::core::fs::File ;

using library::math::obj::Vector3d ;

using library::physics::units::Length ;
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

using library::astro::trajectory::Orbit ;
using library::astro::trajectory::State ;
using library::astro::trajectory::orbit::models::Kepler ;
using library::astro::trajectory::orbit::models::kepler::COE ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                            testOrbit                                   (   const   String&                     aScenarioIdentifier,
                                                                                const   File&                       aReferenceDataFile,
                                                                                const   Orbit&                      anOrbit,
                                                                                const   Real&                       aPositionTolerance_GCRF,
                                                                                const   Real&                       aVelocityTolerance_GCRF,
                                                                                const   Real&                       aPositionTolerance_ITRF,
                                                                                const   Real&                       aVelocityTolerance_ITRF                     )
{

    // Reference data setup

    const Table referenceData = Table::Load(aReferenceDataFile, Table::Format::CSV, true) ;

    // Orbit test

    for (const auto& referenceRow : referenceData)
    {

        const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC) ;

        const Vector3d referencePosition_GCRF = { referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal() } ;
        const Vector3d referenceVelocity_GCRF = { referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal() } ;

        const Vector3d referencePosition_ITRF = { referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal() } ;
        const Vector3d referenceVelocity_ITRF = { referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal() } ;

        const Real referenceRevolutionNumber = referenceRow[13].accessReal() ;

        const State state_GCRF = anOrbit.getStateAt(instant) ;

        const Position position_GCRF = state_GCRF.accessPosition() ;
        const Velocity velocity_GCRF = state_GCRF.accessVelocity() ;

        ASSERT_EQ(Frame::GCRF(), position_GCRF.accessFrame()) ;
        ASSERT_EQ(Frame::GCRF(), velocity_GCRF.accessFrame()) ;

        ASSERT_GT(aPositionTolerance_GCRF, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm()) << String::Format("[{}] {}: {} ≉ {} Δ {} > {} [m]", aScenarioIdentifier, instant.toString(), referencePosition_GCRF.toString(), position_GCRF.accessCoordinates().toString(), (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm(), aPositionTolerance_GCRF) ;
        ASSERT_GT(aVelocityTolerance_GCRF, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm()) << String::Format("[{}] {}: {} ≉ {} Δ {} > {} [m/s]", aScenarioIdentifier, instant.toString(), referenceVelocity_GCRF.toString(), velocity_GCRF.accessCoordinates().toString(), (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm(), aVelocityTolerance_GCRF) ;

        const Shared<const Frame> itrfFrame = Frame::ITRF() ;

        const State state_ITRF = state_GCRF.inFrame(itrfFrame) ;

        const Position position_ITRF = state_ITRF.accessPosition() ;
        const Velocity velocity_ITRF = state_ITRF.accessVelocity() ;

        ASSERT_EQ(Frame::ITRF(), position_ITRF.accessFrame()) ;
        ASSERT_EQ(Frame::ITRF(), velocity_ITRF.accessFrame()) ;

        ASSERT_GT(aPositionTolerance_ITRF, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm()) << String::Format("[{}] {}: {} ≉ {} Δ {} > {} [m]", aScenarioIdentifier, instant.toString(), referencePosition_ITRF.toString(), position_ITRF.accessCoordinates().toString(), (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm(), aPositionTolerance_ITRF) ;
        ASSERT_GT(aVelocityTolerance_ITRF, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm()) << String::Format("[{}] {}: {} ≉ {} Δ {} > {} [m/s]", aScenarioIdentifier, instant.toString(), referenceVelocity_ITRF.toString(), velocity_ITRF.accessCoordinates().toString(), (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm(), aVelocityTolerance_ITRF) ;

        ASSERT_EQ(referenceRevolutionNumber.floor(), anOrbit.getRevolutionNumberAt(instant)) << String::Format("[{}] {}: {} ≠ {}", aScenarioIdentifier, instant.toString(), referenceRevolutionNumber.floor(), anOrbit.getRevolutionNumberAt(instant)) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////