/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting__
#define __OpenSpaceToolkit_Astrodynamics_GuidanceLaw_ROETargeting__

#include <mutex>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Integer.hpp>
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
using ostk::core::type::Integer;
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
/// Burns may only happen in periodic windows: window k spans
///     [burnWindowEpoch + k * burnWindowPeriod, burnWindowEpoch + k * burnWindowPeriod + maximumBurnDuration]
/// and holds at most one burn, starting at the window start, at constant thrust, along a constant direction in the
/// spacecraft RTN frame.
///
/// At the start of every window, the burns of the next horizonWindowCount windows are optimized by solving
///     minimize   mean_j || W (x_j - x_target) ||^2  +  fuelWeight * sum_k || Δv_k || / Δv_max
///     subject to || Δv_k || <= Δv_max = thrustAcceleration * maximumBurnDuration
/// where x_j are the ROE predicted halfway through and at the end of each window period of the horizon, and W
/// divides each ROE component by its tolerance. The fuel term leaves errors that are not worth a burn uncorrected
/// (dead band): increase the fuel weight to save delta-v, decrease it to converge faster. Since the burn duration
/// changes the burn arc, the problem is solved by sequential convex programming: each convex sub-problem (solved with
/// ADMM) is re-linearized about the previous burn durations. Only the first burn is executed; the next window re-plans
/// from the then-current state (receding horizon).
///
/// Choosing the window period: a tangential burn also kicks the relative eccentricity vector along the burn
/// argument of latitude, and radial thrust cannot cancel that kick. With windows a whole number of days apart, the
/// burn argument of latitude barely moves from one window to the next, so recurring drag make-up burns build up a
/// relative eccentricity of a few tens of meters. A period of a half-integer number of orbits (e.g. 15.5 orbits,
/// about 24.7 hours, in LEO) alternates the burns between opposite sides of the orbit and cancels these kicks.
///
/// The plan of a window is computed at the first evaluation within that window, from the state at that evaluation
/// propagated back to the window start, and held until the end of the window. Evaluations before the start of the
/// window discard the held plan, so that a new propagation re-plans it. A new propagation starting inside a window
/// that was already planned reuses the held plan: use a new instance in that case.
///
/// Thrust switches on and off discontinuously. Embedded Runge-Kutta error estimates may not detect a switch inside
/// a step (Runge-Kutta-Fehlberg 7(8) only compares end-point stages), which mis-integrates the burn duration by
/// seconds. Either let a Segment detect the switches, or propagate each burn on its own, with the thruster, from the
/// window start (see getBurnWindowAt) to the burn end (see computePlan, which returns the held plan when given the
/// state at the window start), and coast without the thruster otherwise.
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
        /// @param aBurnWindowEpoch The start of the first burn window.
        /// @param aBurnWindowPeriod The time between the starts of two consecutive burn windows.
        /// @param aMaximumBurnDuration The maximum duration of a burn, which is also the duration of a burn window.
        /// @param aThrustAcceleration The thrust acceleration used for planning, in m/s^2.
        /// @param aHorizonWindowCount The number of burn windows optimized at each planning. Defaults to 5.
        /// @param aToleranceVector The tolerance on each ROE component [δa, δλ, δex, δey, δix, δiy], scaled by the
        /// reference semi-major axis, in m. Defaults to 10 m on every component.
        /// @param aFuelWeight The weight of the delta-v term of the cost, relative to the tracking error term.
        /// Defaults to 1.
        /// @param aMinimumBurnDuration Planned burns shorter than this duration are not executed. Defaults to 1 s.
        /// @param aRelativeSemiMajorAxisRate The rate of change of the semi-major axis of the spacecraft relative to
        /// the reference, not modeled by J2 (typically differential drag), in m/s. Defaults to 0.
        Parameters(
            const Instant& aBurnWindowEpoch,
            const Duration& aBurnWindowPeriod,
            const Duration& aMaximumBurnDuration,
            const Real& aThrustAcceleration,
            const Size& aHorizonWindowCount = 5,
            const Vector6d& aToleranceVector = Vector6d::Constant(10.0),
            const Real& aFuelWeight = 1.0,
            const Duration& aMinimumBurnDuration = Duration::Seconds(1.0),
            const Real& aRelativeSemiMajorAxisRate = 0.0
        );

        Instant burnWindowEpoch;
        Duration burnWindowPeriod;
        Duration maximumBurnDuration;
        Real thrustAcceleration;
        Size horizonWindowCount;
        Vector6d toleranceVector;
        Real fuelWeight;
        Duration minimumBurnDuration;
        Real relativeSemiMajorAxisRate;
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

        /// @brief The burn interval of each window of the horizon. Windows without a burn have a zero duration.
        Array<Interval> burnIntervals;

        /// @brief The delta-v of each window of the horizon, in the RTN frame of the spacecraft, in m/s.
        Array<Vector3d> deltaVelocities;

        /// @brief The instants at which the ROE are predicted.
        Array<Instant> predictionInstants;

        /// @brief The predicted ROE with respect to the reference, scaled by the reference semi-major axis, in m.
        Array<Vector6d> predictedRelativeOrbitalElements;

        /// @brief The number of sequential convex programming iterations.
        Size iterationCount;
    };

    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     const ROETargeting::Parameters parameters = {
    ///         Instant::J2000(), Duration::Days(1.0), Duration::Minutes(10.0), 7.5e-5
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

    /// @brief Get the burn window containing an instant, or the next one if the instant is between windows.
    ///
    /// @param anInstant An instant
    /// @return The burn window.
    Interval getBurnWindowAt(const Instant& anInstant) const;

    /// @brief Print the guidance law.
    ///
    /// @param anOutputStream An output stream
    /// @param displayDecorator If true, display decorators
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Compute the thrust acceleration.
    ///
    /// Within the planned burn interval of the current burn window (closed interval), thrust along the planned
    /// direction; zero otherwise.
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

    /// @brief Compute the ROE of a state with respect to the reference.
    ///
    /// @param aState A state, in an inertial frame
    /// @return The ROE [δa, δλ, δex, δey, δix, δiy], scaled by the reference semi-major axis, in m.
    Vector6d computeRelativeOrbitalElements(const State& aState) const;

    /// @brief Compute the burn plan from a state.
    ///
    /// The planning horizon starts with the first burn window starting at or after the state instant.
    ///
    /// @param aState A state, in an inertial frame
    /// @return The plan.
    Plan computePlan(const State& aState) const;

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

    mutable std::mutex burnCacheMutex_;
    mutable Integer cachedBurnWindowIndex_;
    mutable Interval cachedBurnInterval_;
    mutable Vector3d cachedBurnDirection_;

    Integer getBurnWindowIndexAt_(const Instant& anInstant) const;

    Vector6d computeMeanCOEVector_(const State& aState) const;

    Plan computePlan_(
        const Instant& anEpoch,
        const Vector6d& aReferenceMeanCOEVector,
        const Vector6d& aRelativeOrbitalElements,
        const Real& aMeanArgumentOfLatitude
    ) const;
};

}  // namespace guidancelaw
}  // namespace astrodynamics
}  // namespace ostk

#endif
