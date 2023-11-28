/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::object::Vector3d;

using ostk::physics::Environment;
using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::environment::object::celestial::Earth;
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
using ostk::astro::trajectory::orbit::models::Kepler;
using ostk::astro::trajectory::orbit::models::kepler::COE;

void testOrbit(
    const String& aScenarioIdentifier,
    const File& aReferenceDataFile,
    const Orbit& anOrbit,
    const Real& aPositionTolerance_GCRF,
    const Real& aVelocityTolerance_GCRF,
    const Real& aPositionTolerance_ITRF,
    const Real& aVelocityTolerance_ITRF
)
{
    // Reference data setup

    const Table referenceData = Table::Load(aReferenceDataFile, Table::Format::CSV, true);

    // Orbit test

    for (const auto& referenceRow : referenceData)
    {
        const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);

        const Vector3d referencePosition_GCRF = {
            referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()
        };
        const Vector3d referenceVelocity_GCRF = {
            referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()
        };

        const Vector3d referencePosition_ITRF = {
            referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal()
        };
        const Vector3d referenceVelocity_ITRF = {
            referenceRow[10].accessReal(), referenceRow[11].accessReal(), referenceRow[12].accessReal()
        };

        const Real referenceRevolutionNumber = referenceRow[13].accessReal();

        const State state_GCRF = anOrbit.getStateAt(instant);

        const Position position_GCRF = state_GCRF.getPosition();
        const Velocity velocity_GCRF = state_GCRF.getVelocity();

        ASSERT_EQ(Frame::GCRF(), position_GCRF.accessFrame());
        ASSERT_EQ(Frame::GCRF(), velocity_GCRF.accessFrame());

        ASSERT_GT(aPositionTolerance_GCRF, (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm())
            << String::Format(
                   "[{}] {}: {} ≉ {} Δ {} > {} [m]",
                   aScenarioIdentifier,
                   instant.toString(),
                   referencePosition_GCRF.toString(),
                   position_GCRF.accessCoordinates().toString(),
                   (position_GCRF.accessCoordinates() - referencePosition_GCRF).norm(),
                   aPositionTolerance_GCRF
               );
        ASSERT_GT(aVelocityTolerance_GCRF, (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm())
            << String::Format(
                   "[{}] {}: {} ≉ {} Δ {} > {} [m/s]",
                   aScenarioIdentifier,
                   instant.toString(),
                   referenceVelocity_GCRF.toString(),
                   velocity_GCRF.accessCoordinates().toString(),
                   (velocity_GCRF.accessCoordinates() - referenceVelocity_GCRF).norm(),
                   aVelocityTolerance_GCRF
               );

        const Shared<const Frame> itrfFrame = Frame::ITRF();

        const State state_ITRF = state_GCRF.inFrame(itrfFrame);

        const Position position_ITRF = state_ITRF.getPosition();
        const Velocity velocity_ITRF = state_ITRF.getVelocity();

        ASSERT_EQ(Frame::ITRF(), position_ITRF.accessFrame());
        ASSERT_EQ(Frame::ITRF(), velocity_ITRF.accessFrame());

        ASSERT_GT(aPositionTolerance_ITRF, (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm())
            << String::Format(
                   "[{}] {}: {} ≉ {} Δ {} > {} [m]",
                   aScenarioIdentifier,
                   instant.toString(),
                   referencePosition_ITRF.toString(),
                   position_ITRF.accessCoordinates().toString(),
                   (position_ITRF.accessCoordinates() - referencePosition_ITRF).norm(),
                   aPositionTolerance_ITRF
               );
        ASSERT_GT(aVelocityTolerance_ITRF, (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm())
            << String::Format(
                   "[{}] {}: {} ≉ {} Δ {} > {} [m/s]",
                   aScenarioIdentifier,
                   instant.toString(),
                   referenceVelocity_ITRF.toString(),
                   velocity_ITRF.accessCoordinates().toString(),
                   (velocity_ITRF.accessCoordinates() - referenceVelocity_ITRF).norm(),
                   aVelocityTolerance_ITRF
               );

        ASSERT_EQ(referenceRevolutionNumber.floor(), anOrbit.getRevolutionNumberAt(instant)) << String::Format(
            "[{}] {}: {} ≠ {}",
            aScenarioIdentifier,
            instant.toString(),
            referenceRevolutionNumber.floor(),
            anOrbit.getRevolutionNumberAt(instant)
        );
    }
}
