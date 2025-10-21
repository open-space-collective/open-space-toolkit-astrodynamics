/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Segment__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Segment__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Thruster.hpp>
#include <OpenSpaceToolkit/Astrodynamics/EventCondition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/Maneuver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::type::Integer;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::Dynamics;
using ostk::astrodynamics::dynamics::Thruster;
using ostk::astrodynamics::EventCondition;
using flightManeuver = ostk::astrodynamics::flight::Maneuver;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

/// @brief Represent a propagation segment for astrodynamics purposes
class Segment
{
   public:
    enum class Type
    {
        Coast,    ///< Coast
        Maneuver  ///< Maneuver
    };

    /// @brief Once a segment is set up with an event condition, it can be solved, resulting in this segment's Solution.
    struct Solution
    {
       public:
        /// @brief Constructor
        ///
        /// @param aName Name of the segment
        /// @param aDynamicsArray Array of dynamics
        /// @param aStateArray Array of states for the segment
        /// @param aConditionIsSatisfied True if the event condition is satisfied
        /// @param aSegmentType Type of segment
        /// @return An instance of Solution
        Solution(
            const String& aName,
            const Array<Shared<Dynamics>>& aDynamicsArray,
            const Array<State>& aStateArray,
            const bool& aConditionIsSatisfied,
            const Segment::Type& aSegmentType
        );

        /// @brief Access Start Instant
        /// @return Start Instant
        const Instant& accessStartInstant() const;

        /// @brief Access end instant
        /// @return End Instant
        const Instant& accessEndInstant() const;

        /// @brief Get interval
        /// @return Interval
        Interval getInterval() const;

        /// @brief Get initial mass
        /// @return Initial mass
        Mass getInitialMass() const;

        /// @brief Get final mass
        /// @return Final mass
        Mass getFinalMass() const;

        /// @brief Get propagation duration
        /// @return Propagation duration
        Duration getPropagationDuration() const;

        /// @brief Compute delta V
        ///
        /// @param aSpecificImpulse Specific impulse
        /// @return Delta V
        Real computeDeltaV(const Real& aSpecificImpulse) const;

        /// @brief Compute delta mass
        /// @return Delta mass
        Mass computeDeltaMass() const;

        /// @brief Extract maneuvers from the (maneuvering) segment
        ///
        /// @param aFrameSPtr Frame
        /// @return Array of maneuvers
        Array<flightManeuver> extractManeuvers(const Shared<const Frame>& aFrameSPtr) const;

        /// @brief Calculate intermediate states at specified Instants using the provided Numerical Solver
        ///
        /// @param aNumericalSolver a numerical solver to use for the propagation between states
        /// @param anInstantArray an array of instants
        /// @return States at specified instants
        Array<State> calculateStatesAt(const Array<Instant>& anInstantArray, const NumericalSolver& aNumericalSolver)
            const;

        /// @brief Get dynamics contribution
        ///
        /// @param aDynamicsSPtr Dynamics
        /// @param aFrameSPtr Frame
        /// @param aCoordinateSubsetSPtrArray Array of coordinate subsets
        /// @return Dynamics contribution
        MatrixXd getDynamicsContribution(
            const Shared<Dynamics>& aDynamicsSPtr,
            const Shared<const Frame>& aFrameSPtr,
            const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetSPtrArray =
                Array<Shared<const CoordinateSubset>>::Empty()
        ) const;

        /// @brief Get dynamics acceleration contribution
        ///
        /// @param aDynamicsSPtr Dynamics
        /// @param aFrameSPtr Frame
        /// @return Dynamics acceleration contribution
        MatrixXd getDynamicsAccelerationContribution(
            const Shared<Dynamics>& aDynamicsSPtr, const Shared<const Frame>& aFrameSPtr
        ) const;

        /// @brief Get all segment dynamics contributions
        ///
        /// @param aFrameSPtr Frame
        /// @return All segment dynamics contributions
        Map<Shared<Dynamics>, MatrixXd> getAllDynamicsContributions(const Shared<const Frame>& aFrameSPtr) const;

        /// @brief Print the segment solution
        ///
        /// @param anOutputStream An output stream
        /// @param (optional) displayDecorators If true, display decorators
        void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        /// @brief Output stream operator
        ///
        /// @param anOutputStream An output stream
        /// @param aSolution A Solution
        /// @return An output stream
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Solution& aSolution);

        String name;                       // Name of the segment.
        Array<Shared<Dynamics>> dynamics;  // List of dynamics used.
        Array<State> states;               // Array of states for the segment.
        bool conditionIsSatisfied;         // True if the event condition is satisfied.
        Segment::Type segmentType;         // Type of segment.
    };

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aSegment A Segment
    /// @return An output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Segment& aSegment);

    /// @brief Get name
    /// @return Name of the segment
    String getName() const;

    /// @brief Get event condition
    /// @return Event condition
    Shared<EventCondition> getEventCondition() const;

    /// @brief Get dynamics
    /// @return Dynamics
    Array<Shared<Dynamics>> getDynamics() const;

    /// @brief Get numerical solver
    /// @return Numerical solver
    NumericalSolver getNumericalSolver() const;

    /// @brief Get type
    /// @return Type of segment
    Type getType() const;

    /// @brief Access event condition
    /// @return Event condition
    const Shared<EventCondition>& accessEventCondition() const;

    /// @brief Access dynamics
    /// @return Dynamics
    const Array<Shared<Dynamics>>& accessDynamics() const;

    /// @brief Access numerical solver
    /// @return Numerical solver
    const NumericalSolver& accessNumericalSolver() const;

    /// @brief Solve the segment
    ///
    /// @param aState Initial state for the segment
    /// @param maximumPropagationDuration Maximum duration for propagation. Defaults to 30 days
    /// @return A Solution representing the result of the solve
    Solution solve(const State& aState, const Duration& maximumPropagationDuration = Duration::Days(30.0)) const;

    /// @brief Print the segment
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Create a coasting segment
    ///
    /// @param aName A name
    /// @param anEventConditionSPtr An event condition
    /// @param aDynamicsArray Array of dynamics
    /// @param aNumericalSolver Numerical solver
    /// @return A Segment for coasting
    static Segment Coast(
        const String& aName,
        const Shared<EventCondition>& anEventConditionSPtr,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const NumericalSolver& aNumericalSolver
    );

    /// @brief Create a maneuvering segment
    ///
    /// @param aName A name
    /// @param anEventConditionSPtr An event condition
    /// @param aThrusterDynamics Dynamics for the thruster
    /// @param aDynamicsArray Array of dynamics
    /// @param aNumericalSolver Numerical solver
    /// @return A Segment for maneuvering
    static Segment Maneuver(
        const String& aName,
        const Shared<EventCondition>& anEventConditionSPtr,
        const Shared<Thruster>& aThrusterDynamics,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const NumericalSolver& aNumericalSolver
    );

    /// @brief Create a maneuvering segment that produces maneuvers with a constant direction in the local orbital
    /// frame.
    ///
    /// The provided thruster dynamics are used to solve the segment at first. The maneuvers produced by this segement
    /// solution are then used to create a new thruster dynamics with a constant direction in the local orbital frame.
    /// This new thruster dynamics is then used to actually solve the segment.
    ///
    /// If defined, a runtime error will be thrown if the maximum allowed angular offset between the original thruster
    /// dynamics and the mean thrust direction is violated.
    ///
    /// @param aName A name
    /// @param anEventConditionSPtr An event condition
    /// @param aThrusterDynamics Dynamics for the thruster
    /// @param aDynamicsArray Array of dynamics
    /// @param aNumericalSolver Numerical solver
    /// @param aLocalOrbitalFrameFactory A local orbital frame factory.
    /// @param aMaximumAllowedAngularOffset A maximum allowed angular offset to consider (if any). Defaults
    /// to Undefined.
    static Segment ConstantLocalOrbitalFrameDirectionManeuver(
        const String& aName,
        const Shared<EventCondition>& anEventConditionSPtr,
        const Shared<Thruster>& aThrusterDynamics,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const NumericalSolver& aNumericalSolver,
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactory,
        const Angle& aMaximumAllowedAngularOffset = Angle::Undefined()
    );

   private:
    String name_;
    Type type_;
    Shared<EventCondition> eventCondition_;
    Array<Shared<Dynamics>> dynamics_;
    NumericalSolver numericalSolver_;
    Shared<const LocalOrbitalFrameFactory> constantManeuverDirectionLocalOrbitalFrameFactory_;
    Angle constantManeuverDirectionMaximumAllowedAngularOffset_;

    /// @brief Constructor
    ///
    /// @param aName The name of the segment
    /// @param aType The type of the segment
    /// @param anEventConditionSPtr The event condition
    /// @param aDynamicsArray The dynamics array
    /// @param aNumericalSolver The numerical solver
    Segment(
        const String& aName,
        const Type& aType,
        const Shared<EventCondition>& anEventConditionSPtr,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const NumericalSolver& aNumericalSolver
    );

    /// @brief Solve the segment using the given dynamics and event condition
    ///
    /// @param aState The initial state of the segment
    /// @param maximumPropagationDuration The maximum propagation duration
    /// @param aDynamicsArray The dynamics array
    /// @param anEventConditionSPtr The event condition
    /// @return The segment solution
    Segment::Solution Solve_(
        const State& aState,
        const Duration& maximumPropagationDuration,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const Shared<EventCondition>& anEventConditionSPtr
    ) const;

    /// @brief Find the Thruster dynamics from an array of dynamics, throwing an error if none or multiple Thruster
    /// dynamics are found.
    ///
    /// @param aDynamicsArray Array of dynamics to search.
    /// @return Shared pointer to the Thruster dynamics.
    static const Shared<Thruster> FindThrusterDynamics(const Array<Shared<Dynamics>>& aDynamicsArray);
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif  // __OpenSpaceToolkit_Astrodynamics_Trajectory_Segment__
