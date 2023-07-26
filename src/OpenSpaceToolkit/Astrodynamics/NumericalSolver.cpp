/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

namespace ostk
{
namespace astro
{

using namespace boost::numeric::odeint;

using ostk::core::types::Index;

typedef runge_kutta4<NumericalSolver::StateVector> stepper_type_4;
typedef runge_kutta_cash_karp54<NumericalSolver::StateVector> error_stepper_type_54;
typedef runge_kutta_fehlberg78<NumericalSolver::StateVector> error_stepper_type_78;
typedef runge_kutta_dopri5<NumericalSolver::StateVector> dense_stepper_type_5;

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance
)
    : logType_(aLogType),
      stepperType_(aStepperType),
      timeStep_(aTimeStep),
      relativeTolerance_(aRelativeTolerance),
      absoluteTolerance_(anAbsoluteTolerance),
      observedStates_()
{
}

NumericalSolver* NumericalSolver::clone() const
{
    return new NumericalSolver(*this);
}

bool NumericalSolver::operator==(const NumericalSolver& aNumericalSolver) const
{
    if ((!this->isDefined()) || (!aNumericalSolver.isDefined()))
    {
        return false;
    }

    return (logType_ == aNumericalSolver.logType_) && (stepperType_ == aNumericalSolver.stepperType_) &&
           (timeStep_ == aNumericalSolver.timeStep_) && (relativeTolerance_ == aNumericalSolver.relativeTolerance_) &&
           (absoluteTolerance_ == aNumericalSolver.absoluteTolerance_);
}

bool NumericalSolver::operator!=(const NumericalSolver& aNumericalSolver) const
{
    return !((*this) == aNumericalSolver);
}

std::ostream& operator<<(std::ostream& anOutputStream, const NumericalSolver& aNumericalSolver)
{
    aNumericalSolver.print(anOutputStream);

    return anOutputStream;
}

bool NumericalSolver::isDefined() const
{
    return timeStep_.isDefined() && relativeTolerance_.isDefined() && absoluteTolerance_.isDefined();
}

void NumericalSolver::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Numerical Solver") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Integration logging type:" << NumericalSolver::StringFromLogType(logType_);
    ostk::core::utils::Print::Line(anOutputStream)
        << "Integration stepper type:" << NumericalSolver::StringFromStepperType(stepperType_);
    ostk::core::utils::Print::Line(anOutputStream)
        << "Integration time step:" << (timeStep_.isDefined() ? timeStep_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Integration relative tolerance:"
        << (relativeTolerance_.isDefined() ? relativeTolerance_.toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "Integration absolute tolerance:"
        << (absoluteTolerance_.isDefined() ? absoluteTolerance_.toString() : "Undefined");

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

NumericalSolver::LogType NumericalSolver::getLogType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return logType_;
}

NumericalSolver::StepperType NumericalSolver::getStepperType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return stepperType_;
}

Real NumericalSolver::getTimeStep() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return timeStep_;
}

Real NumericalSolver::getRelativeTolerance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return relativeTolerance_;
}

Real NumericalSolver::getAbsoluteTolerance() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    return absoluteTolerance_;
}

Array<NumericalSolver::Solution> NumericalSolver::integrateTime(
    const StateVector& anInitialStateVector,
    const Real& aStartTime,
    const Array<Real>& aTimeArray,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    observedStates_.clear();

    NumericalSolver::StateVector aStateVector = anInitialStateVector;

    // Check if time array has zero length
    if (aTimeArray.size() == 0)
    {
        throw ostk::core::error::RuntimeError("Time Array is empty");
    }

    // Check if the incoming time array is the same as the start time if it has length 1
    if ((aTimeArray.size() == 1) && (aTimeArray[0] == aStartTime))
    {
        return {
            {anInitialStateVector, aStartTime},
        };
    }

    // Ensure integration starts in the correct direction with the initial time step guess
    const double adjustedTimeStep = getSignedTimeStep(aTimeArray.accessLast());

    // Add start time to the start of array
    Array<double> durationArray(aTimeArray.begin(), aTimeArray.end());
    durationArray.insert(durationArray.begin(), aStartTime);

    const auto observer = [this](const NumericalSolver::StateVector& x, double t) -> void
    {
        this->observeNumericalIntegration(x, t);
    };

    switch (stepperType_)
    {
        case NumericalSolver::StepperType::RungeKutta4:
        {
            integrate_times(
                stepper_type_4(), aSystemOfEquations, aStateVector, durationArray, adjustedTimeStep, observer
            );
            break;
        }

        case NumericalSolver::StepperType::RungeKuttaCashKarp54:
        {
            integrate_times(
                make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()),
                aSystemOfEquations,
                aStateVector,
                durationArray,
                adjustedTimeStep,
                observer
            );
            break;
        }

        case NumericalSolver::StepperType::RungeKuttaFehlberg78:
        {
            integrate_times(
                make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()),
                aSystemOfEquations,
                aStateVector,
                durationArray,
                adjustedTimeStep,
                observer
            );
            break;
        }

        default:
            throw ostk::core::error::runtime::Wrong("Stepper type");
    }

    // Return array of StateVectors excluding first element which is a repeat of the startState
    return Array<NumericalSolver::Solution>(observedStates_.begin() + 1, observedStates_.end());
}

NumericalSolver::Solution NumericalSolver::integrateDuration(
    const StateVector& anInitialStateVector,
    const Real& aDurationInSeconds,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    NumericalSolver::StateVector aStateVector = anInitialStateVector;

    observedStates_.clear();

    if (aDurationInSeconds.isZero())  // If integration duration is zero seconds long, skip integration
    {
        return {anInitialStateVector, 0.0};
    }

    // Ensure integration starts in the correct direction with the initial time step guess
    const double adjustedTimeStep = getSignedTimeStep(aDurationInSeconds);

    const auto observer = [this](const NumericalSolver::StateVector& x, double t) -> void
    {
        this->observeNumericalIntegration(x, t);
    };

    switch (stepperType_)
    {
        case NumericalSolver::StepperType::RungeKutta4:
        {
            // Integrate_adaptive uses constant step size under the hood
            // for a stepper without error control like RK4.
            // Therefore, just use integrate_const for simplicity.
            switch (logType_)
            {
                case NumericalSolver::LogType::NoLog:
                case NumericalSolver::LogType::LogAdaptive:
                case NumericalSolver::LogType::LogConstant:
                {
                    integrate_const(
                        stepper_type_4(),
                        aSystemOfEquations,
                        aStateVector,
                        (0.0),
                        (double)aDurationInSeconds,
                        adjustedTimeStep,
                        observer
                    );
                    return {aStateVector, aDurationInSeconds};
                }
                default:
                    throw ostk::core::error::runtime::Wrong("Log type");
            }
        }

        case NumericalSolver::StepperType::RungeKuttaCashKarp54:
        {
            switch (logType_)
            {
                case NumericalSolver::LogType::NoLog:
                case NumericalSolver::LogType::LogAdaptive:
                {
                    integrate_adaptive(
                        make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()),
                        aSystemOfEquations,
                        aStateVector,
                        (0.0),
                        (double)aDurationInSeconds,
                        adjustedTimeStep,
                        observer
                    );
                    return {aStateVector, aDurationInSeconds};
                }

                case NumericalSolver::LogType::LogConstant:
                {
                    integrate_const(
                        make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_54()),
                        aSystemOfEquations,
                        aStateVector,
                        (0.0),
                        (double)aDurationInSeconds,
                        adjustedTimeStep,
                        observer
                    );
                    return {aStateVector, aDurationInSeconds};
                }
                default:
                    throw ostk::core::error::runtime::Wrong("Log type");
            }
        }

        case NumericalSolver::StepperType::RungeKuttaFehlberg78:
        {
            switch (logType_)
            {
                case NumericalSolver::LogType::NoLog:
                case NumericalSolver::LogType::LogAdaptive:
                {
                    integrate_adaptive(
                        make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()),
                        aSystemOfEquations,
                        aStateVector,
                        (0.0),
                        (double)aDurationInSeconds,
                        adjustedTimeStep,
                        observer
                    );
                    return {aStateVector, aDurationInSeconds};
                }

                case NumericalSolver::LogType::LogConstant:
                {
                    integrate_const(
                        make_controlled(absoluteTolerance_, relativeTolerance_, error_stepper_type_78()),
                        aSystemOfEquations,
                        aStateVector,
                        (0.0),
                        (double)aDurationInSeconds,
                        adjustedTimeStep,
                        observer
                    );
                    return {aStateVector, aDurationInSeconds};
                }
                default:
                    throw ostk::core::error::runtime::Wrong("Log type");
            }
        }

        default:
            throw ostk::core::error::runtime::Wrong("Stepper type");
    }

    throw ostk::core::error::RuntimeError("No State Vector returned from Odeint.");

    return {anInitialStateVector, 0.0};
}

NumericalSolver::Solution NumericalSolver::integrateTime(
    const StateVector& anInitialStateVector,
    const Real& aStartTime,
    const Real& anEndTime,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    return this->integrateDuration(anInitialStateVector, (anEndTime - aStartTime), aSystemOfEquations);
}

NumericalSolver::Solution NumericalSolver::integrateTime(
    const NumericalSolver::StateVector& anInitialStateVector,
    const Real& aStartTime,
    const Real& anEndTime,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition
)
{
    NumericalSolver::Solution solution =
        integrateDuration(anInitialStateVector, anEndTime - aStartTime, aSystemOfEquations, anEventCondition);
    solution.second += aStartTime;

    return solution;
}

Array<NumericalSolver::Solution> NumericalSolver::integrateDuration(
    const NumericalSolver::StateVector& anInitialStateVector,
    const Array<Real>& aDurationArray,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    return integrateTime(anInitialStateVector, 0.0, aDurationArray, aSystemOfEquations);
}

NumericalSolver::Solution NumericalSolver::integrateDuration(
    const StateVector& anInitialStateVector,
    const Real& aDurationInSeconds,
    const SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition
)
{
    if (stepperType_ != NumericalSolver::StepperType::RungeKuttaDopri5)
    {
        throw ostk::core::error::runtime::ToBeImplemented(
            "Integrating with conditions is only supported with RungeKuttaDopri5 stepper type."
        );
    }

    if (aDurationInSeconds.isZero())
    {
        return {anInitialStateVector, 0.0};
    }

    NumericalSolver::StateVector aStateVector = anInitialStateVector;

    // Ensure that the time step is the correct sign
    const double signedTimeStep = getSignedTimeStep(aDurationInSeconds);

    // TBI: Adapt this to any dense stepper type
    auto stepper = make_dense_output(absoluteTolerance_, relativeTolerance_, dense_stepper_type_5());

    // initialize stepper
    double currentTime = 0.0;
    stepper.initialize(aStateVector, currentTime, signedTimeStep);

    // do first step
    double previousTime;
    std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
    observeNumericalIntegration(stepper.current_state(), stepper.current_time());

    bool conditionSatisfied = false;
    Real currentValue = Real::Undefined();

    Real previousValue = anEventCondition.evaluate(stepper.current_state(), stepper.current_time());
    NumericalSolver::StateVector currentState;

    // account for integration direction
    const auto checkTimeLimit = [&currentTime, &aDurationInSeconds]()
    {
        if (aDurationInSeconds > 0.0)
        {
            return currentTime < aDurationInSeconds;
        }
        return currentTime > aDurationInSeconds;
    };

    while (checkTimeLimit())
    {
        std::tie(previousTime, currentTime) = stepper.do_step(aSystemOfEquations);
        currentState = stepper.current_state();

        currentValue = anEventCondition.evaluate(currentState, currentTime);

        conditionSatisfied = anEventCondition.isSatisfied(currentValue, previousValue);

        if (conditionSatisfied)
        {
            break;
        }

        observeNumericalIntegration(currentState, currentTime);

        previousValue = currentValue;
    }

    // TBI: Share information upstream on if the condition was satisfied
    if (!conditionSatisfied)
    {
        stepper.calc_state(aDurationInSeconds, currentState);
        return {currentState, aDurationInSeconds};
    }

    // Condition at previousTime => False
    // Condition at currentTime => True
    // Search for the exact time of the condition change
    NumericalSolver::StateVector midState(currentState);
    double midTime;

    // TBI: Make this a parameter
    const Index maxIterationCount = 100;

    for (Index iterationCount = 0; iterationCount < maxIterationCount; ++iterationCount)
    {
        midTime = 0.5 * (previousTime + currentTime);
        stepper.calc_state(midTime, midState);

        const Real midValue = anEventCondition.evaluate(midState, midTime);

        if (anEventCondition.isSatisfied(midValue, previousValue))
        {
            // root lies between previousTime and midTime
            // update current -> mid
            currentTime = midTime;
            currentValue = midValue;
        }
        else
        {
            // root lies between midTime and currentTime
            // update previous -> mid
            previousTime = midTime;
            previousValue = midValue;
        }

        // TBI: Make tolerance a parameter
        if (std::abs(midValue) < 1e-6)
        {
            break;
        }
    }

    // TBI: Share information upstream on the number of iterations + success
    // if (iterationCount == maxIterationCount)
    // {
    //   do thing
    // }

    observeNumericalIntegration(midState, midTime);

    return {midState, midTime};
}

String NumericalSolver::StringFromLogType(const NumericalSolver::LogType& aLogType)
{
    switch (aLogType)
    {
        case NumericalSolver::LogType::NoLog:
            return "NoLog";

        case NumericalSolver::LogType::LogConstant:
            return "LogConstant";

        case NumericalSolver::LogType::LogAdaptive:
            return "LogAdaptive";

        default:
            throw ostk::core::error::runtime::Wrong("Log Type");
    }
}

String NumericalSolver::StringFromStepperType(const NumericalSolver::StepperType& aStepperType)
{
    switch (aStepperType)
    {
        case NumericalSolver::StepperType::RungeKutta4:
            return "RungeKutta4";

        case NumericalSolver::StepperType::RungeKuttaCashKarp54:
            return "RungeKuttaCashKarp54";

        case NumericalSolver::StepperType::RungeKuttaFehlberg78:
            return "RungeKuttaFehlberg78";

        case NumericalSolver::StepperType::RungeKuttaDopri5:
            return "RungeKuttaDopri5";

        default:
            throw ostk::core::error::runtime::Wrong("Stepper Type");
    }
}

NumericalSolver NumericalSolver::Undefined()
{
    return NumericalSolver(
        LogType::NoLog, StepperType::RungeKuttaCashKarp54, Real::Undefined(), Real::Undefined(), Real::Undefined()
    );
}

NumericalSolver NumericalSolver::Default()
{
    return NumericalSolver(
        NumericalSolver::LogType::NoLog, NumericalSolver::StepperType::RungeKuttaFehlberg78, 5.0, 1.0e-12, 1.0e-12
    );
}

void NumericalSolver::observeNumericalIntegration(const NumericalSolver::StateVector& x, const double t)
{
    observedStates_.push_back({x, t});

    switch (logType_)
    {
        case NumericalSolver::LogType::LogAdaptive:
        case NumericalSolver::LogType::LogConstant:
        {
            std::cout.precision(3);
            std::cout.setf(std::ios::fixed, std::ios::floatfield);

            std::cout << std::left << std::setw(15) << t;

            std::cout.precision(10);
            std::cout.setf(std::ios::scientific, std::ios::floatfield);
            for (Size i = 0; i < (Size)x.size(); i++)
            {
                std::cout << std::internal << std::setw(16) << x[i] << "     ";
            }
            std::cout << std::endl;

            std::cout.setf(std::ios::fixed, std::ios::floatfield);

            break;
        }

        default:
        {
            break;
        }
    }
}

double NumericalSolver::getSignedTimeStep(const Real& aReal) const
{
    const Real durationSign = (aReal > 0.0) - (aReal < 0.0);
    return timeStep_ * durationSign;
}

}  // namespace astro
}  // namespace ostk
