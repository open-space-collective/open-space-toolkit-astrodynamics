/// Apache License 2.0

#include <algorithm>

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/algebra/vector_space_algebra.hpp>
#include <boost/numeric/odeint/external/eigen/eigen.hpp>

#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator/CubicSpline.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Eigen/Core>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

using namespace boost::numeric::odeint;

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

namespace boost
{
namespace numeric
{
namespace odeint
{

struct eigen_pid_algebra : public vector_space_algebra
{
    template <class S1, class S2, class Op>
    static void for_each2(S1& s1, S2& s2, Op op)
    {
        for (Eigen::Index i = 0; i < s1.size(); ++i)
        {
            op(s1.coeffRef(i), s2.coeffRef(i));
        }
    }

    template <class S1, class S2, class S3, class S4, class Op>
    static void for_each4(S1& s1, S2& s2, S3& s3, S4& s4, Op op)
    {
        for (Eigen::Index i = 0; i < s1.size(); ++i)
        {
            op(s1.coeffRef(i), s2.coeffRef(i), s3.coeffRef(i), s4.coeffRef(i));
        }
    }
};

}  // namespace odeint
}  // namespace numeric
}  // namespace boost

using ostk::mathematics::curvefitting::interpolator::CubicSpline;

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

using namespace boost::numeric::odeint;

using ostk::core::container::Pair;
using ostk::core::type::Shared;

using ostk::physics::time::Duration;

using ostk::astrodynamics::RootSolver;
using ostk::astrodynamics::trajectory::StateBuilder;

using stepper_type_4 = runge_kutta4<NumericalSolver::StateVector>;

template <size_t Order>
auto make_controlled_adam_bashforth_moulton(double abs_tol, double rel_tol)
{
    using AdaptiveStepperType = adaptive_adams_bashforth_moulton<Order, NumericalSolver::StateVector>;

    using StepAdjusterType = detail::pid_step_adjuster<
        NumericalSolver::StateVector,
        double,
        NumericalSolver::StateVector,
        double,
        eigen_pid_algebra>;

    using ControlledStepperType = controlled_adams_bashforth_moulton<AdaptiveStepperType, StepAdjusterType>;

    return ControlledStepperType(StepAdjusterType(abs_tol, rel_tol));
}

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const RootSolver& aRootSolver
)
    : MathNumericalSolver(aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance),
      rootSolver_(aRootSolver),
      observedStates_(),
      stateLogger_(nullptr)
{
}

const Array<State>& NumericalSolver::accessObservedStates() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return observedStates_;
}

RootSolver NumericalSolver::getRootSolver() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return rootSolver_;
}

Array<State> NumericalSolver::getObservedStates() const
{
    return accessObservedStates();
}

Real NumericalSolver::getMaxStepSize() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return maxStepSize_;
}

void NumericalSolver::setMaxStepSize(const Real& aMaxStepSize)
{
    maxStepSize_ = aMaxStepSize;
}

Array<State> NumericalSolver::integrateTime(
    const State& aState,
    const Array<Instant>& anInstantArray,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    const Array<Real> durationArray = anInstantArray.map<Real>(
        [&aState](const Instant& anInstant) -> Real
        {
            return (anInstant - aState.accessInstant()).inSeconds();
        }
    );

    const Array<NumericalSolver::Solution> solutions =
        MathNumericalSolver::integrateDuration(aState.accessCoordinates(), durationArray, aSystemOfEquations);

    Array<State> states;
    states.reserve(solutions.getSize());
    for (Index i = 0; i < solutions.getSize(); ++i)
    {
        const NumericalSolver::Solution& solution = solutions.at(i);
        const State state = {
            anInstantArray[i],
            solution.first,
            aState.accessFrame(),
            aState.accessCoordinateBroker(),
        };
        states.add(state);
    }

    return states;
}

State NumericalSolver::integrateTime(
    const State& aState, const Instant& anEndTime, const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    observedStates_ = {};

    const StateBuilder stateBuilder = {aState};

    const NumericalSolver::Solution solution = MathNumericalSolver::integrateDuration(
        aState.accessCoordinates(), (anEndTime - aState.accessInstant()).inSeconds(), aSystemOfEquations
    );

    for (const auto& state : MathNumericalSolver::getObservedStateVectors())
    {
        observedStates_.add(stateBuilder.build(aState.accessInstant() + Duration::Seconds(state.second), state.first));
    }

    return stateBuilder.build(anEndTime, solution.first);
}

NumericalSolver::ConditionSolution NumericalSolver::integrateTime(
    const State& aState,
    const Instant& anInstant,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition
)
{
    // Multistep Adams-Bashforth-Moulton steppers are not supported with event conditions.
    // The conditional path samples the post-crossing bracket via `integrate_times` on a fresh
    // (multistep-history-cleared) copy of the stepper, which forces ABM to warm up from scratch
    // at every bracket. Combined with tight integration tolerances and discontinuous dynamics
    // (e.g. thrust events) this drives the accepted step size near zero and makes the run
    // effectively non-terminating. Callers should select a Runge-Kutta or Bulirsch-Stoer stepper.
    if (stepperType_ == StepperType::AdamsBashforthMoulton5 || stepperType_ == StepperType::AdamsBashforthMoulton8)
    {
        throw ostk::core::error::RuntimeError(
            "Conditional integration is not supported with Adams-Bashforth-Moulton steppers "
            "(AdamsBashforthMoulton5, AdamsBashforthMoulton8). Use a Runge-Kutta or Bulirsch-Stoer "
            "stepper instead."
        );
    }

    observedStates_ = {aState};

    const Real aDurationInSeconds = (anInstant - aState.accessInstant()).inSeconds();

    // Check trivial cases
    if (aDurationInSeconds.isZero())
    {
        return {
            aState,
            false,
            0,
            false,
        };
    }

    bool conditionSatisfied = anEventCondition.isSatisfied(aState, aState);
    if (conditionSatisfied)
    {
        return {
            aState,
            true,
            0,
            true,
        };
    }

    return integrateTimeWithControlledStepper(aState, anInstant, aSystemOfEquations, anEventCondition);
}

NumericalSolver NumericalSolver::Undefined()
{
    return {
        LogType::NoLog,
        StepperType::RungeKuttaCashKarp54,
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        RootSolver::Default(),
        nullptr,
    };
}

NumericalSolver NumericalSolver::Default()
{
    return {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
        nullptr,
    };
}

NumericalSolver NumericalSolver::FixedStepSize(const NumericalSolver::StepperType& aStepperType, const Real& aTimeStep)
{
    if (aStepperType != NumericalSolver::StepperType::RungeKutta4)
    {
        throw ostk::core::error::RuntimeError("Fixed step size is only supported with RungeKutta4 stepper type.");
    }

    return {
        NumericalSolver::LogType::NoLog,
        aStepperType,
        aTimeStep,
        1.0,
        1.0,
        RootSolver::Default(),
        nullptr,
    };
}

NumericalSolver NumericalSolver::DefaultConditional(const std::function<void(const State&)>& stateLogger)
{
    return {
        NumericalSolver::LogType::NoLog,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        5.0,
        1.0e-12,
        1.0e-12,
        RootSolver::Default(),
        stateLogger,
    };
}

NumericalSolver NumericalSolver::Conditional(
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const std::function<void(const State&)>& stateLogger
)
{
    const NumericalSolver::LogType logType =
        stateLogger != nullptr ? NumericalSolver::LogType::LogAdaptive : NumericalSolver::LogType::NoLog;

    return {
        logType,
        NumericalSolver::StepperType::RungeKuttaFehlberg78,
        aTimeStep,
        aRelativeTolerance,
        anAbsoluteTolerance,
        RootSolver::Default(),
        stateLogger,
    };
}

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const RootSolver& aRootSolver,
    const std::function<void(const State& aState)>& stateLogger
)
    : MathNumericalSolver(aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance),
      rootSolver_(aRootSolver),
      observedStates_(),
      stateLogger_(stateLogger)
{
}

void NumericalSolver::observeState(const State& aState)
{
    observedStates_.add(aState);

    if (stateLogger_ != nullptr && getLogType() != NumericalSolver::LogType::NoLog)
    {
        stateLogger_(aState);
    }
}

namespace
{

/// @brief Type trait to detect fixed-step steppers (e.g., RK4)
template <typename Stepper>
struct IsFixedStepStepper : std::false_type
{
};

template <>
struct IsFixedStepStepper<stepper_type_4> : std::true_type
{
};

/// @brief Perform a single integration step with a fixed-step stepper
template <typename Stepper, typename System>
inline typename std::enable_if<IsFixedStepStepper<Stepper>::value>::type doStep(
    Stepper& stepper, const System& system, NumericalSolver::StateVector& stateVector, double& currentTime, double& dt
)
{
    stepper.do_step(system, stateVector, currentTime, dt);
    currentTime += dt;
}

/// @brief Perform a single integration step with a controlled stepper (retries until accepted)
template <typename Stepper, typename System>
inline typename std::enable_if<!IsFixedStepStepper<Stepper>::value>::type doStep(
    Stepper& stepper, const System& system, NumericalSolver::StateVector& stateVector, double& currentTime, double& dt
)
{
    while (stepper.try_step(system, stateVector, currentTime, dt) ==
           boost::numeric::odeint::controlled_step_result::fail)
    {
    }
}

/// @brief Integrate adaptively to a target time. Used for the trim integration after the main
///        loop exits without finding the event, where direction is the same as the main loop and
///        the multistep history (if any) remains valid.
template <typename Stepper, typename System>
inline void integrateToTime_(
    Stepper& stepper,
    NumericalSolver::StateVector& stateVector,
    double startTime,
    double endTime,
    double stepSize,
    const System& system
)
{
    integrate_adaptive(stepper, system, stateVector, startTime, endTime, stepSize);
}

}  // namespace

template <typename Stepper>
NumericalSolver::ConditionSolution NumericalSolver::integrateTimeWithStepperImpl_(
    Stepper& aStepper,
    const State& aState,
    const Instant& anInstant,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition,
    double aSignedTimeStep
)
{
    observedStates_ = {aState};

    const StateBuilder stateBuilder = {aState};

    const double endTime = (anInstant - aState.accessInstant()).inSeconds();

    const auto createState = [&stateBuilder, &aState](const VectorXd& aStateVector, const double& aTime) -> State
    {
        return stateBuilder.build(aState.accessInstant() + Duration::Seconds(aTime), aStateVector);
    };

    const auto checkTimeLimit = [aSignedTimeStep, endTime](double aTime) -> bool
    {
        return aSignedTimeStep > 0.0 ? aTime < endTime : aTime > endTime;
    };

    NumericalSolver::StateVector currentStateVector = aState.accessCoordinates();
    NumericalSolver::StateVector previousStateVector = aState.accessCoordinates();

    double previousTime = 0.0;
    double currentTime = 0.0;
    double dt = aSignedTimeStep;
    State currentState = State::Undefined();
    State previousState = aState;
    bool conditionSatisfied = false;

    while (checkTimeLimit(currentTime) && !conditionSatisfied)
    {
        previousStateVector = currentStateVector;
        previousTime = currentTime;

        // Limit the step size to prevent massive overshoots past the target end time.
        // When a maxStepSize is specified, also enforce that as an upper bound. This is critical
        // for detecting discrete/step-function events (e.g., thrust toggles) that can be missed
        // entirely when the adaptive stepper grows its step too large.
        const double remaining = endTime - currentTime;
        double maxDt = std::abs(remaining) + std::abs(aSignedTimeStep);
        if (maxStepSize_.isDefined())
        {
            maxDt = std::min(maxDt, static_cast<double>(maxStepSize_));
        }
        dt = std::clamp(dt, -maxDt, maxDt);

        doStep(aStepper, aSystemOfEquations, currentStateVector, currentTime, dt);

        currentState = createState(currentStateVector, currentTime);

        observeState(currentState);

        conditionSatisfied = anEventCondition.isSatisfied(currentState, previousState);

        if (conditionSatisfied)
        {
            break;
        }

        previousState = currentState;
    }

    // Remove the state that triggered the condition (we'll find the exact crossing).
    observedStates_.pop_back();

    if (!conditionSatisfied)
    {
        // Use a relative tolerance when comparing currentTime to endTime: the running accumulator
        // can land epsilon-short of endTime, in which case a strict `!=` check would call
        // integrate_adaptive with a zero-length window and have boost odeint reject the request.
        if (std::abs(endTime - currentTime) > 1e-12)
        {
            // Compute step size with correct sign for the direction from currentTime to endTime.
            // This handles the case where we overshot endTime and need to integrate backwards.
            const double adjustmentStepSize = (endTime - currentTime) / 10.0;
            integrateToTime_<Stepper>(
                aStepper, currentStateVector, currentTime, endTime, adjustmentStepSize, aSystemOfEquations
            );
        }

        const State finalState = createState(currentStateVector, endTime);

        // Append finalState only if it preserves the strict monotonicity of observedStates_:
        // the trim integration above can produce a state at the exact same instant as the
        // last observed entry (e.g. when currentTime already coincided with endTime within
        // tolerance), and emitting a duplicate would silently regress monotonicity downstream.
        if (observedStates_.isEmpty() || finalState.accessInstant() != observedStates_.accessLast().accessInstant())
        {
            observeState(finalState);
        }

        return {
            finalState,
            false,
            0,
            false,
        };
    }

    const double stepSpan = currentTime - previousTime;

    // Fixed-step sampling + per-dimension CubicSpline interpolation.
    const double absStepSpan = std::abs(stepSpan);
    const bool isBackward = stepSpan < 0.0;

    // Cubic Spline requires a atleast 5 samples.
    const Size minIntervals = 5;
    const double maxDt = 20.0;
    const Size numIntervals =
        static_cast<Size>(std::max(static_cast<double>(minIntervals), std::ceil(absStepSpan / maxDt)));
    const Size numSamples = numIntervals + 1;
    const double signedSampleDt = stepSpan / static_cast<double>(numIntervals);
    const double absSampleDt = absStepSpan / static_cast<double>(numIntervals);

    VectorXd sampleTimes = VectorXd::LinSpaced(numSamples, previousTime, currentTime);

    const Size stateDim = previousStateVector.size();
    Eigen::MatrixXd sampleMatrix(numSamples, stateDim);

    Size sampleIdx = 0;
    const auto sampleObserver =
        [&sampleMatrix, &sampleIdx](const NumericalSolver::StateVector& aSampledState, const double& /*aTime*/)
    {
        sampleMatrix.row(sampleIdx++) = aSampledState.transpose();
    };

    const double initialDt = signedSampleDt / 10.0;
    integrate_times(
        aStepper,
        aSystemOfEquations,
        previousStateVector,
        sampleTimes.begin(),
        sampleTimes.end(),
        initialDt,
        sampleObserver
    );

    // CubicSpline (boost cardinal_cubic_b_spline) requires h > 0 and ascending x. For backward
    // integration the samples are in descending time order — reverse them so the spline sees an
    // ascending axis with positive spacing.
    const double splineX0 = isBackward ? currentTime : previousTime;
    if (isBackward)
    {
        sampleMatrix = sampleMatrix.colwise().reverse().eval();
    }

    Array<CubicSpline> splines;
    splines.reserve(stateDim);
    for (Index idx = 0; idx < stateDim; ++idx)
    {
        const ostk::mathematics::object::VectorXd yColumn = sampleMatrix.col(idx);
        splines.add(CubicSpline(yColumn, splineX0, absSampleDt));
    }

    NumericalSolver::StateVector interpolatedStateVector(previousStateVector.size());
    const auto stateGenerator = [&splines, stateDim](const double& aTime) -> NumericalSolver::StateVector
    {
        NumericalSolver::StateVector stateVectorAtTargetTime(stateDim);
        for (Index idx = 0; idx < stateDim; ++idx)
        {
            stateVectorAtTargetTime(idx) = splines[idx].evaluate(aTime);
        }
        return stateVectorAtTargetTime;
    };

    const auto checkCondition =
        [&anEventCondition, &createState, &previousState, &stateGenerator, &interpolatedStateVector](const double& aTime
        ) -> double
    {
        interpolatedStateVector = stateGenerator(aTime);
        const State interpolatedState = createState(interpolatedStateVector, aTime);
        const bool isSatisfied = anEventCondition.isSatisfied(interpolatedState, previousState);
        return isSatisfied ? 1.0 : -1.0;
    };

    // Condition at previousTime => False
    // Condition at currentTime => True
    // Search for the exact time of the condition change.
    //
    // When the adaptive stepper accepts an extremely small step that triggers the crossing,
    // re-evaluating the condition through `stateGenerator` (which re-interpolates the same
    // interval) may not reproduce the same crossing due to roundoff in the cartesian state and
    // the non-linear conversion to whatever quantity the event condition measures
    // (e.g. Brouwer-Lyddane mean SMA). In that case `f(previousTime)` and `f(currentTime)` end
    // up with the same sign and `boost::math::tools::bisect` throws `evaluation_error`. The
    // forward integration already detected the crossing, so accept `currentState` as the
    // solution rather than aborting.
    if (checkCondition(previousTime) * checkCondition(currentTime) > 0.0)
    {
        const State solutionState = createState(currentStateVector, currentTime);

        if (solutionState.accessInstant() != aState.accessInstant())
        {
            observeState(solutionState);
        }

        return {
            solutionState,
            true,
            0,
            false,
        };
    }

    const RootSolver::Solution solution = rootSolver_.bisection(checkCondition, previousTime, currentTime);

    // Ensure that the solution time has crossed the condition
    const double solutionTime = (aSignedTimeStep > 0.0) ? solution.upperBound : solution.lowerBound;

    const State solutionState = createState(interpolatedStateVector, solutionTime);

    // If the solution state is not the same as the initial state, add it to the observed states
    if (solutionState.accessInstant() != aState.accessInstant())
    {
        observeState(solutionState);
    }

    return {
        solutionState,
        true,
        solution.iterationCount,
        solution.hasConverged,
    };
}

NumericalSolver::ConditionSolution NumericalSolver::integrateTimeWithControlledStepper(
    const State& aState,
    const Instant& anInstant,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition
)
{
    const Real aDurationInSeconds = (anInstant - aState.accessInstant()).inSeconds();
    const double signedTimeStep = getSignedTimeStep(aDurationInSeconds);

    auto dispatch = [&](auto&& aStepper) -> NumericalSolver::ConditionSolution
    {
        using DispatchStepperType = std::decay_t<decltype(aStepper)>;
        return integrateTimeWithStepperImpl_<DispatchStepperType>(
            aStepper, aState, anInstant, aSystemOfEquations, anEventCondition, signedTimeStep
        );
    };

    switch (stepperType_)
    {
        case StepperType::RungeKutta4:
            return dispatch(stepper_type_4 {});
        case StepperType::RungeKuttaCashKarp54:
            return dispatch(make_controlled(
                absoluteTolerance_, relativeTolerance_, runge_kutta_cash_karp54<NumericalSolver::StateVector>()
            ));
        case StepperType::RungeKuttaFehlberg78:
            return dispatch(make_controlled(
                absoluteTolerance_, relativeTolerance_, runge_kutta_fehlberg78<NumericalSolver::StateVector>()
            ));
        case StepperType::RungeKuttaDopri5:
            return dispatch(make_controlled(
                absoluteTolerance_, relativeTolerance_, runge_kutta_dopri5<NumericalSolver::StateVector>()
            ));
        case StepperType::AdamsBashforthMoulton5:
            return dispatch(make_controlled_adam_bashforth_moulton<5>(absoluteTolerance_, relativeTolerance_));
        case StepperType::AdamsBashforthMoulton8:
            return dispatch(make_controlled_adam_bashforth_moulton<8>(absoluteTolerance_, relativeTolerance_));
        case StepperType::BulirschStoer:
            return dispatch(bulirsch_stoer<NumericalSolver::StateVector>(absoluteTolerance_, relativeTolerance_));
        default:
            throw ostk::core::error::runtime::Wrong("Stepper type");
    }
}

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
