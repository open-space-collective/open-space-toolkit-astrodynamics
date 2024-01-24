/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Validation_Parser__
#define __OpenSpaceToolkit_Astrodynamics_Validation_Parser__

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Containers/Dictionary.hpp>
#include <OpenSpaceToolkit/Core/Containers/Object.hpp>
#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Types/Integer.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Types/Size.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Objects/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Objects/CelestialBodies/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Units/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/CrossValidator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/AtmosphericDrag.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/CentralBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/ThirdBodyGravity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/InstantCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition/RealCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/SatelliteSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw/ConstantThrust.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Segment.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Sequence.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astro
{
namespace validation
{

using ostk::astro::validation::Quantity;
using ostk::astro::validation::QuantityComparison;
using ostk::astro::validation::Tool;
using ostk::astro::validation::ToolComparison;
using ostk::astro::validation::CrossValidator;

using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using CntrObject = ostk::core::ctnr::Object;
using ostk::core::ctnr::Dictionary;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::filesystem::Directory;
using ostk::core::types::Integer;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::types::String;
using ostk::core::types::Size;

using ostk::math::geometry::d3::objects::Composite;
using ostk::math::geometry::d3::objects::Cuboid;
using ostk::math::object::Vector3d;
using ostk::math::object::Matrix3d;
using ostk::math::object::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;
using ostk::physics::Environment;
using EarthAtmosphericModel = ostk::physics::environment::atmospheric::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using EarthMagneticModel = ostk::physics::environment::magnetic::Earth;
using ostk::physics::environment::Object;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::environment::object::celestial::Moon;
using ostk::physics::environment::object::celestial::Sun;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::units::Mass;

using ostk::astro::Dynamics;
using ostk::astro::dynamics::AtmosphericDrag;
using ostk::astro::dynamics::CentralBodyGravity;
using ostk::astro::dynamics::PositionDerivative;
using ostk::astro::dynamics::ThirdBodyGravity;
using ostk::astro::dynamics::Thruster;
using ostk::astro::eventcondition::InstantCondition;
using ostk::astro::eventcondition::RealCondition;
using ostk::astro::flight::system::PropulsionSystem;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::guidancelaw::ConstantThrust;
using ostk::astro::trajectory::LocalOrbitalFrameDirection;
using ostk::astro::trajectory::LocalOrbitalFrameFactory;
using ostk::astro::trajectory::Propagator;
using ostk::astro::trajectory::Segment;
using ostk::astro::trajectory::Sequence;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;
using ostk::astro::trajectory::state::NumericalSolver;
using ostk::astro::trajectory::StateBuilder;

/// @brief Provide methods to parse a "Mission Sequence Scenario" defined as a YAML standardized format and turn it into
/// an OSTk mission sequence object.
class Parser
{
   public:
    /// @brief Parse the YAML output file of a tool and turn it into a dictionary.
    ///
    /// @param aPathToData The path to the data directory.
    /// @param aScenarioName The name of the scenario.
    /// @return A dictionary.
    static Dictionary ParseYaml(const String& aPathToData, const String& aScenarioName);

    /// @brief Parse the CSV output file of a tool and turn it into a table.
    ///
    /// @param aPathToData The path to the data directory.
    /// @param aScenarioName The name of the scenario.
    /// @param aTool The tool used to generate the CSV file.
    /// @return A table.
    static Table ParseCSV(const String& aPathToData, const String& aScenarioName, const Tool& aTool);

    /// @brief Create a satellite system from a dictionary.
    ///
    /// @param aDictionary A dictionary.
    /// @return A satellite system.
    static SatelliteSystem CreateSatelliteSystem(const Dictionary& aDictionary);

    /// @brief Create an initial state from a dictionary.
    ///
    /// @param aDictionary A dictionary.
    /// @param aSatelliteSystem A satellite system.
    /// @return An initial state.
    static State CreateInitialState(const Dictionary& aDictionary, const SatelliteSystem& aSatelliteSystem);

    /// @brief Create an environment from a dictionary.
    ///
    /// @param aDictionary A dictionary.
    /// @return An environment.
    static Environment CreateEnvironment(const Dictionary& aDictionary);

    /// @brief Create a segment from a dictionary and other OSTk objects.
    ///
    /// @param aDictionary A dictionary.
    /// @param aSatelliteSystem A satellite system.
    /// @param aDynamicsArray An array of shared dynamics.
    /// return A segment.
    static Sequence CreateSequence(
        const Dictionary& aDictionary,
        const SatelliteSystem& aSatelliteSystem,
        const Array<Shared<Dynamics>>& aDynamicsArray
    );

    /// @brief Create an array of evenly spaced instants at which to compare the output of two tools.
    ///
    /// @param aDictionary A dictionary.
    /// @param anInitialInstant The initial instant.
    /// @param aFinalInstant The final instant.
    /// @return An array of instants.
    static Array<Instant> CreateComparisonInstants(
        const Dictionary& aDictionary, const Instant& anInitialInstant, const Instant& aFinalInstant
    );

   private:
    Parser();
    ~Parser();

    static Segment CreateSegment(
        const Dictionary& aDictionary,
        const SatelliteSystem& aSatelliteSystem,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const NumericalSolver& aNumericalSolver
    );
};

}  // namespace validation
}  // namespace astro
}  // namespace ostk

#endif
