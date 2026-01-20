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
#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
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

    /// @brief Maximum maneuver duration violation strategy
    /// Strategy to use when a maneuver exceeds the maximum duration constraint.
    ///
    /// Example:
    /// Maximum duration:  [------]
    /// Proposed maneuver: [---------------------------------]
    /// TruncateEnd:       [------]
    /// Center:                          [------]
    /// TruncateStart:                                [------]
    enum class MaximumManeuverDurationViolationStrategy
    {
        Fail,           ///< Will throw a RuntimeError if a maneuver exceeds the maximum duration.
        Skip,           ///< The maneuver will be skipped entirely.
        TruncateEnd,    ///< The maneuver will be shortened to the maximum duration, truncating the end.
        TruncateStart,  ///< The maneuver will be shortened to the maximum duration, truncating the start.
        Center,         ///< The maneuver will be shortened to the maximum duration, truncating the edges,
                        ///< keeping the centered part of the maneuver.
    };

    struct ManeuverConstraints
    {
        /// @brief Constructor
        /// @param aMinimumDuration The minimum duration for a maneuver. Defaults to Undefined.
        /// @param aMaximumDuration The maximum duration for a maneuver. Defaults to Undefined.
        /// @param aMinimumSeparation The minimum separation between maneuvers. Defaults to Undefined.
        /// @param aMaximumDurationStrategy The strategy when maximum duration is violated. Defaults to Fail.
        ManeuverConstraints(
            const Duration& aMinimumDuration = Duration::Undefined(),
            const Duration& aMaximumDuration = Duration::Undefined(),
            const Duration& aMinimumSeparation = Duration::Undefined(),
            const MaximumManeuverDurationViolationStrategy& aMaximumDurationStrategy =
                MaximumManeuverDurationViolationStrategy::Fail
        );

        Duration minimumDuration;
        Duration maximumDuration;
        Duration minimumSeparation;
        MaximumManeuverDurationViolationStrategy maximumDurationStrategy;

        /// @brief Check if the maneuver constraints are defined
        /// @return True if the maneuver constraints are defined
        bool isDefined() const;

        /// @brief Check if the minimum duration is valid
        /// @param aManeuverInterval The maneuver interval
        /// @return True if the minimum duration is valid
        bool intervalHasValidMinimumDuration(const Interval& aManeuverInterval) const;

        /// @brief Check if the maximum duration is valid
        /// @param aManeuverInterval The maneuver interval
        /// @return True if the maximum duration is valid
        bool intervalHasValidMaximumDuration(const Interval& aManeuverInterval) const;

        /// @brief Print the maneuver constraints
        /// @param anOutputStream An output stream
        /// @param displayDecorator If true, display decorators
        void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        /// @brief Output stream operator
        /// @param anOutputStream An output stream
        /// @param aManeuverConstraints A maneuver constraints
        /// @return An output stream
        friend std::ostream& operator<<(std::ostream& anOutputStream, const ManeuverConstraints& aManeuverConstraints);
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

        /// @brief Get thruster dynamics from the solution
        /// @return Thruster dynamics
        Shared<Thruster> getThrusterDynamics() const;

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

    /// @brief Get free dynamics array
    /// @return Free dynamics array
    Array<Shared<Dynamics>> getFreeDynamics() const;

    /// @brief Get numerical solver
    /// @return Numerical solver
    NumericalSolver getNumericalSolver() const;

    /// @brief Get thruster dynamics
    /// @return Thruster dynamics
    Shared<Thruster> getThrusterDynamics() const;

    /// @brief Get maneuver constraints
    /// @return Maneuver constraints
    ManeuverConstraints getManeuverConstraints() const;

    /// @brief Get type
    /// @return Type of segment
    Type getType() const;

    /// @brief Access event condition
    /// @return Event condition
    const Shared<EventCondition>& accessEventCondition() const;

    /// @brief Access numerical solver
    /// @return Numerical solver
    const NumericalSolver& accessNumericalSolver() const;

    /// @brief Solve the segment until its event condition is satisfied or the maximum propagation duration is reached.
    ///
    /// @param aState Initial state for the segment
    /// @param maximumPropagationDuration Maximum duration for propagation. Defaults to 30 days.
    /// @param previousManeuverInterval Last maneuver interval prior to this segment. Defaults to Undefined.
    /// @return A Solution representing the result of the solve
    Solution solve(
        const State& aState,
        const Duration& maximumPropagationDuration = Duration::Days(30.0),
        Interval previousManeuverInterval = Interval::Undefined()
    ) const;

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
    /// @param aManeuverConstraints Maneuver constraints
    /// @return A Segment for maneuvering
    static Segment Maneuver(
        const String& aName,
        const Shared<EventCondition>& anEventConditionSPtr,
        const Shared<Thruster>& aThrusterDynamics,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const NumericalSolver& aNumericalSolver,
        const ManeuverConstraints& aManeuverConstraints = ManeuverConstraints()
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
    /// @param aManeuverConstraints Maneuver constraints
    /// @return A Segment for maneuvering
    static Segment ConstantLocalOrbitalFrameDirectionManeuver(
        const String& aName,
        const Shared<EventCondition>& anEventConditionSPtr,
        const Shared<Thruster>& aThrusterDynamics,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const NumericalSolver& aNumericalSolver,
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactory,
        const Angle& aMaximumAllowedAngularOffset = Angle::Undefined(),
        const ManeuverConstraints& aManeuverConstraints = ManeuverConstraints()
    );

    // TBI: Remove this and replace with magic_enum based solution
    /// @brief String from maximum maneuver duration violation strategy
    /// @param aMaximumDurationStrategy The maximum maneuver duration violation strategy
    /// @return String from maximum maneuver duration violation strategy
    static String StringFromMaximumManeuverDurationViolationStrategy(
        const MaximumManeuverDurationViolationStrategy& aMaximumDurationStrategy
    );

   private:
    String name_;
    Type type_;
    Shared<EventCondition> eventCondition_;
    Array<Shared<Dynamics>> freeDynamicsArray_;
    Shared<Thruster> thrusterDynamicsSPtr_;
    NumericalSolver numericalSolver_;
    Shared<const LocalOrbitalFrameFactory> constantManeuverDirectionLocalOrbitalFrameFactory_;
    Angle constantManeuverDirectionMaximumAllowedAngularOffset_;
    ManeuverConstraints maneuverConstraints_;

    /// @brief Constructor
    ///
    /// @param aName The name of the segment
    /// @param aType The type of the segment
    /// @param anEventConditionSPtr The event condition
    /// @param aFreeDynamicsArray The free dynamics array
    /// @param aThrusterDynamics The thruster dynamics
    /// @param aNumericalSolver The numerical solver
    /// @param aLocalOrbitalFrameFactory The local orbital frame factory
    /// @param aMaximumAllowedAngularOffset The maximum allowed angular offset
    /// @param aManeuverConstraints Maneuver constraints
    /// @return A Segment
    Segment(
        const String& aName,
        const Type& aType,
        const Shared<EventCondition>& anEventConditionSPtr,
        const Array<Shared<Dynamics>>& aFreeDynamicsArray,
        const Shared<Thruster>& aThrusterDynamics,
        const NumericalSolver& aNumericalSolver,
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactory = nullptr,
        const Angle& aMaximumAllowedAngularOffset = Angle::Undefined(),
        const ManeuverConstraints& aManeuverConstraints = ManeuverConstraints()
    );

    /// @brief Solve the segment with the provided dynamics and event condition. This method is used to solve coasting
    /// and maneuvering segments.
    ///
    /// @param aState The initial state of the segment
    /// @param maximumPropagationDuration The maximum propagation duration
    /// @param aDynamicsArray The dynamics array
    /// @param anEventCondition The event condition
    /// @return The segment solution
    Segment::Solution solveWithDynamics_(
        const State& aState,
        const Duration& maximumPropagationDuration,
        const Array<Shared<Dynamics>>& aDynamicsArray,
        const Shared<EventCondition>& anEventCondition
    ) const;

    /// @brief For a given maneuver, construct a solution that is Local Orbital Frame (LOF) compliant.
    ///
    /// @param aState The initial state of the segment
    /// @param aManeuver The maneuver
    /// @return The segment solution
    Segment::Solution constructLOFCompliantManeuverSolution_(const State& aState, const flightManeuver& aManeuver)
        const;

    /// @brief Solve the coast segment, uses the internal free dynamics array and event condition of the segment.
    ///
    /// @param aState The initial state of the segment
    /// @param maximumPropagationDuration The maximum propagation duration
    /// @return The segment solution
    Segment::Solution solveCoast_(const State& aState, const Duration& maximumPropagationDuration) const;

    /// @brief Solve till the next maneuver ends. This maneuver is not necessarily constraint-compliant yet.
    ///
    /// @param aState The initial state of the segment
    /// @param maximumPropagationDuration The maximum propagation duration
    /// @param thrusterDynamics The thruster dynamics.
    /// @return The segment solution
    Segment::Solution solveNextManeuver_(
        const State& aState, const Duration& maximumPropagationDuration, const Shared<Thruster>& thrusterDynamics
    ) const;

    /// @brief Propagate the segment with the provided dynamics and event condition. This method is used to propagate
    /// the segment until a given instant, not based on an event condition.
    ///
    /// @param aState The initial state of the segment
    /// @param anEndInstant The end instant
    /// @param aDynamicsArray The dynamics array
    /// @return States
    Array<State> propagateWithDynamics_(
        const State& aState, const Instant& anEndInstant, const Array<Shared<Dynamics>>& aDynamicsArray
    ) const;

    /// @brief Solve the maneuver for a given interval. Coasts until the start of the maneuver interval and then solves
    /// the maneuver for the given interval. This guarantees that we have a state at the start and end of the maneuver
    /// interval.
    ///
    /// @param aState The initial state of the segment
    /// @param thrusterDynamics The thruster dynamics
    /// @param validManeuverInterval The valid maneuver interval
    /// @return The segment solution
    Segment::Solution solveManeuverForInterval_(
        const State& aState, const Shared<Thruster>& thrusterDynamics, const Interval& validManeuverInterval
    ) const;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif  // __OpenSpaceToolkit_Astrodynamics_Trajectory_Segment__
