/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Validation_Parser__
#define __OpenSpaceToolkit_Astrodynamics_Validation_Parser__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Dictionary.hpp>
#include <OpenSpaceToolkit/Core/Container/Object.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Composite.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Cuboid.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Atmospheric/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Magnetic/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Moon.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Sun.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace validation
{

using ostk::astrodynamics::validation::Quantity;
using ostk::astrodynamics::validation::QuantityComparison;
using ostk::astrodynamics::validation::Tool;
using ostk::astrodynamics::validation::ToolComparison;
using ostk::astrodynamics::validation::CrossValidator;

using ostk::core::container::Array;
using ostk::core::container::Table;
using CntrObject = ostk::core::container::Object;
using ostk::core::container::Dictionary;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::filesystem::Directory;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::type::Size;

using ostk::mathematics::geometry::d3::object::Composite;
using ostk::mathematics::geometry::d3::object::Cuboid;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
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
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::AtmosphericDrag;
using ostk::astrodynamics::dynamics::CentralBodyGravity;
using ostk::astrodynamics::dynamics::PositionDerivative;
using ostk::astrodynamics::dynamics::ThirdBodyGravity;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::eventcondition::InstantCondition;
using ostk::astrodynamics::eventcondition::RealCondition;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::flight::system::SatelliteSystem;
using ostk::astrodynamics::guidancelaw::ConstantThrust;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::Segment;
using ostk::astrodynamics::trajectory::Sequence;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;
using ostk::astrodynamics::trajectory::StateBuilder;

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
}  // namespace astrodynamics
}  // namespace ostk

#endif
