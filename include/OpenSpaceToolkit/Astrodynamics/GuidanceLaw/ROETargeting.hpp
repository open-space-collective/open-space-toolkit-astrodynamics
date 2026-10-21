/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/GuidanceLaw.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace guidancelaw
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::Matrix3d;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;
using Matrix6d = Eigen::Matrix<double, 6, 6>;
using Matrix63d = Eigen::Matrix<double, 6, 3>;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::GuidanceLaw;
using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::State;

/// @brief Full six-element orbit targeting with Relative Orbital Elements (ROE) model predictive control.
///
/// The spacecraft is steered towards a reference trajectory (optionally offset by a constant ROE vector), including
/// its along-track position (phasing), which Q-Law, a five-element law, cannot target.
///
/// The relative state is expressed with the quasi-nonsingular ROE of the spacecraft with respect to the reference,
/// computed from Brouwer-Lyddane mean elements:
///     [δa, δλ, δex, δey, δix, δiy]
/// Its free motion is predicted with the J2 state transition matrix of Koenig, Guffanti and D'Amico (2017),
/// augmented with an optional constant relative semi-major axis rate (typically differential drag). Burns enter the
/// model through the near-circular Gauss variational equations, averaged over the burn arc.
///
/// A plan optimizes horizonBurnCount burns at constant thrust, each along a constant direction in the spacecraft RTN
/// frame and lasting at most maximumBurnDuration, consecutive burns starting burnSpacing apart, by solving
///     minimize   mean_j || W (x_j - x_target) ||^2  +  fuelWeight * sum_k || Δv_k || / Δv_max
///     subject to || Δv_k || <= Δv_max = thrustAcceleration * maximumBurnDuration
/// where x_j are the ROE predicted halfway through and at the end of each burn spacing after the planning instant,
/// and W divides each ROE component by its tolerance. The fuel term leaves errors that are not worth a burn
/// uncorrected (dead band): increase the fuel weight to save delta-v, decrease it to converge faster. Since the burn
/// duration changes the burn arc, the problem is solved by sequential convex programming: each convex sub-problem
/// (solved with ADMM) is re-linearized about the previous burn durations.
///
/// Like the Q-Law, the guidance law decides at every evaluation whether burning now is worth it compared to burning
/// at other points of the orbit. It compares the cost J of:
///   - the plan whose first burn starts now,
///   - the plans whose first burn starts later, at burnStartCandidateCount start times evenly spread over one orbit
///     (one period of the mean argument of latitude), the following burns being delayed accordingly,
///   - the plan without the first burn,
/// and computes the effectivity of burning now, the fraction of the best improvement over not burning that burning
/// now achieves:
///     η = (J_none - J_now) / (J_none - J_best)
/// The spacecraft thrusts along the first burn direction of the plan starting now when η >= effectivityThreshold and
/// that burn lasts at least minimumBurnDuration.
///
/// The guidance law has no memory: the thrust only depends on the instant and the state. A burn should not be
/// re-evaluated along its course though: it removes the error that made it worth starting, so that the effectivity
/// drops below the threshold before the planned burn is complete, and the thrust would switch on and off at the
/// threshold. Instead, when a burn starts, the guidance law commits to it (see commitManeuverAt): a constant thrust
/// along the planned direction in the RTN (QSW) frame, for the planned duration. A maneuvering Segment follows the
/// committed burn, and enforces the constraints between burns (minimum separation between maneuvers, maximum maneuver
/// duration); burnSpacing only models them in the predictions (typically the minimum separation plus the maximum burn
/// duration).
///
/// Each evaluation solves up to burnStartCandidateCount + 2 small convex problems (a few milliseconds in total).
///
/// The model assumes a near-circular reference orbit around the Earth (EGM2008 constants) and separations small
/// with respect to the semi-major axis.
///
/// @ref https://doi.org/10.2514/1.G002409 (Koenig, Guffanti, D'Amico, New State Transition Matrices for Spacecraft
/// Relative Motion in Perturbed Orbits, JGCD 2017)
/// @ref https://mediatum.ub.tum.de/doc/1135045/document.pdf (D'Amico, Autonomous Formation Flying in Low Earth
/// Orbit, 2010)
class ROETargeting : public GuidanceLaw
{
   public:
    /// @brief Parameters of the ROE targeting guidance law.
    struct Parameters
    {
        /// @brief Constructor.
        ///
        /// @param aBurnSpacing The time between the starts of two consecutive burns assumed by the predictions,
        /// typically the minimum separation between maneuvers plus the maximum burn duration.
        /// @param aMaximumBurnDuration The maximum duration of a burn.
        /// @param aThrustAcceleration The thrust acceleration used for planning, in m/s^2.
        /// @param aHorizonBurnCount The number of burns optimized by each plan. Defaults to 5.
        /// @param aToleranceVector The tolerance on each ROE component [δa, δλ, δex, δey, δix, δiy], scaled by the
        /// reference semi-major axis, in m. Defaults to 10 m on every component.
        /// @param aFuelWeight The weight of the delta-v term of the cost, relative to the tracking error term.
        /// Defaults to 1.
        /// @param aMinimumBurnDuration Planned burns shorter than this duration are not executed. Defaults to 1 s.
        /// @param aRelativeSemiMajorAxisRate The rate of change of the semi-major axis of the spacecraft relative to
        /// the reference, not modeled by J2 (typically differential drag), in m/s. Defaults to 0.
        /// @param anEffectivityThreshold The effectivity of burning now above which the spacecraft burns, between 0
        /// and 1. Defaults to 0.9.
        /// @param aBurnStartCandidateCount The number of first burn start times, evenly spread over one orbit
        /// starting now, compared with each other. Defaults to 36.
        Parameters(
            const Duration& aBurnSpacing,
            const Duration& aMaximumBurnDuration,
            const Real& aThrustAcceleration,
            const Size& aHorizonBurnCount = 5,
            const Vector6d& aToleranceVector = Vector6d::Constant(10.0),
            const Real& aFuelWeight = 1.0,
            const Duration& aMinimumBurnDuration = Duration::Seconds(1.0),
            const Real& aRelativeSemiMajorAxisRate = 0.0,
            const Real& anEffectivityThreshold = 0.9,
            const Size& aBurnStartCandidateCount = 36
        );

        Duration burnSpacing;
        Duration maximumBurnDuration;
        Real thrustAcceleration;
        Size horizonBurnCount;
        Vector6d toleranceVector;
        Real fuelWeight;
        Duration minimumBurnDuration;
        Real relativeSemiMajorAxisRate;
        Real effectivityThreshold;
        Size burnStartCandidateCount;
    };

    /// @brief A burn plan over the planning horizon.
    struct Plan
    {
        /// @brief The planning epoch.
        Instant epoch;

        /// @brief The reference semi-major axis at the planning epoch, in m.
        Real referenceSemiMajorAxis;

        /// @brief The ROE of the spacecraft with respect to the reference at the planning epoch, scaled by the
        /// reference semi-major axis, in m.
        Vector6d relativeOrbitalElements;

        /// @brief The interval of each burn of the horizon. Burns that are not executed have a zero duration.
        Array<Interval> burnIntervals;

        /// @brief The delta-v of each burn of the horizon, in the RTN frame of the spacecraft, in m/s.
        Array<Vector3d> deltaVelocities;

        /// @brief The instants at which the ROE are predicted.
        Array<Instant> predictionInstants;

        /// @brief The predicted ROE with respect to the reference, scaled by the reference semi-major axis, in m.
        Array<Vector6d> predictedRelativeOrbitalElements;

        /// @brief The cost of the plan (tracking error term plus delta-v term).
        Real cost;

        /// @brief The number of sequential convex programming iterations.
        Size iterationCount;
    };

    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     const ROETargeting::Parameters parameters = {
    ///         Duration::Days(1.0) + Duration::Minutes(10.0), Duration::Minutes(10.0), 7.5e-5
    ///     };
    ///     ROETargeting guidanceLaw = {referenceOrbit, parameters};
    /// @endcode
    ///
    /// @param aReferenceTrajectory The reference trajectory. Must provide states in an inertial frame.
    /// @param aParameters The parameters.
    /// @param aTargetRelativeOrbitalElements The ROE to hold with respect to the reference, scaled by the reference
    /// semi-major axis, in m. Defaults to zero (fly the reference itself).
    ROETargeting(
        const Trajectory& aReferenceTrajectory,
        const Parameters& aParameters,
        const Vector6d& aTargetRelativeOrbitalElements = Vector6d::Zero()
    );

    /// @brief Destructor.
    virtual ~ROETargeting() override;

    /// @brief Output stream operator.
    ///
    /// @param anOutputStream An output stream
    /// @param aGuidanceLaw A guidance law
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const ROETargeting& aGuidanceLaw);

    /// @brief Access the reference trajectory.
    ///
    /// @return The reference trajectory.
    const Trajectory& accessReferenceTrajectory() const;

    /// @brief Get the parameters.
    ///
    /// @return The parameters.
    Parameters getParameters() const;

    /// @brief Get the target ROE.
    ///
    /// @return The target ROE, scaled by the reference semi-major axis, in m.
    Vector6d getTargetRelativeOrbitalElements() const;

    /// @brief Print the guidance law.
    ///
    /// @param anOutputStream An output stream
    /// @param displayDecorator If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Compute the thrust acceleration.
    ///
    /// Along the first burn direction of the plan starting now, if burning now is effective enough and that burn
    /// lasts at least the minimum burn duration; zero otherwise.
    ///
    /// @param anInstant An instant
    /// @param aPositionCoordinates The position coordinates, in an inertial frame
    /// @param aVelocityCoordinates The velocity coordinates, in an inertial frame
    /// @param aThrustAcceleration The thrust acceleration
    /// @param outputFrameSPtr The frame of the coordinates and of the returned acceleration
    /// @return The thrust acceleration.
    virtual Vector3d calculateThrustAccelerationAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Real& aThrustAcceleration,
        const Shared<const Frame>& outputFrameSPtr
    ) const override;

    /// @brief Commit to the burn starting at the provided coordinates.
    ///
    /// The first burn of the plan starting now: a constant thrust along its direction in the RTN (QSW) frame, for its
    /// duration.
    ///
    /// @param anInstant The start instant of the burn
    /// @param aPositionCoordinates The position coordinates at the start of the burn
    /// @param aVelocityCoordinates The velocity coordinates at the start of the burn
    /// @param aFrameSPtr The frame of the coordinates
    /// @return The committed burn, or nothing if the plan has no burn starting now.
    virtual std::optional<CommittedManeuver> commitManeuverAt(
        const Instant& anInstant,
        const Vector3d& aPositionCoordinates,
        const Vector3d& aVelocityCoordinates,
        const Shared<const Frame>& aFrameSPtr
    ) const override;

    /// @brief Compute the ROE of a state with respect to the reference.
    ///
    /// @param aState A state, in an inertial frame
    /// @return The ROE [δa, δλ, δex, δey, δix, δiy], scaled by the reference semi-major axis, in m.
    Vector6d computeRelativeOrbitalElements(const State& aState) const;

    /// @brief Compute the burn plan from a state.
    ///
    /// @param aState A state, in an inertial frame
    /// @param aFirstBurnDelay The delay of the first burn with respect to the state instant. Defaults to zero.
    /// @return The plan.
    Plan computePlan(const State& aState, const Duration& aFirstBurnDelay = Duration::Zero()) const;

    /// @brief Compute the effectivity of burning now.
    ///
    /// @param aState A state, in an inertial frame
    /// @return The effectivity, between 0 and 1 (0 if no burn of the next orbit improves on not burning).
    Real computeEffectivity(const State& aState) const;

    /// @brief Compute the quasi-nonsingular ROE between two sets of mean orbital elements.
    ///
    /// @param aCOE The mean orbital elements of the spacecraft
    /// @param aReferenceCOE The mean orbital elements of the reference
    /// @return The dimensionless ROE [δa, δλ, δex, δey, δix, δiy].
    static Vector6d ComputeRelativeOrbitalElements(const COE& aCOE, const COE& aReferenceCOE);

    /// @brief Compute the J2 state transition matrix of the quasi-nonsingular ROE (Koenig, Guffanti, D'Amico).
    ///
    /// @param aReferenceCOE The mean orbital elements of the reference at the start of the transition
    /// @param aDuration The transition duration
    /// @return The state transition matrix.
    static Matrix6d ComputeStateTransitionMatrix(const COE& aReferenceCOE, const Duration& aDuration);

    /// @brief Compute the control input matrix, mapping a delta-v in the RTN frame (m/s) to the dimensionless ROE,
    /// from the near-circular Gauss variational equations averaged over an arc of mean argument of latitude.
    ///
    /// @param aSemiMajorAxis The reference semi-major axis, in m
    /// @param aMeanArgumentOfLatitudeStart The mean argument of latitude at the start of the arc, in rad
    /// @param aMeanArgumentOfLatitudeEnd The mean argument of latitude at the end of the arc, in rad
    /// @return The control input matrix.
    static Matrix63d ComputeControlInputMatrix(
        const Real& aSemiMajorAxis, const Real& aMeanArgumentOfLatitudeStart, const Real& aMeanArgumentOfLatitudeEnd
    );

   private:
    const Trajectory referenceTrajectory_;
    const Parameters parameters_;
    const Vector6d targetRelativeOrbitalElements_;

    Vector6d computeMeanCOEVector_(const State& aState) const;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
