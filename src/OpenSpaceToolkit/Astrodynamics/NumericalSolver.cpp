/// Apache License 2.0

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/external/eigen/eigen_algebra.hpp>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>

// TBI: Move this to eigen.hpp when we move this file to ostk mathematics
namespace boost::numeric::odeint
{
template <>
struct resize_impl<ostk::astro::NumericalSolver::StateVector, ostk::astro::NumericalSolver::StateVector>
{
    static void resize(
        ostk::astro::NumericalSolver::StateVector& x, const ostk::astro::NumericalSolver::StateVector& template_x
    )
    {
        x.resize(template_x.size());
    }
};

template <>
struct is_resizeable<ostk::astro::NumericalSolver::StateVector>
{
    typedef boost::true_type type;
    static const bool value = type::value;
};

template <>
struct same_size_impl<ostk::astro::NumericalSolver::StateVector, ostk::astro::NumericalSolver::StateVector>
{
    static bool same_size(
        const ostk::astro::NumericalSolver::StateVector& x, const ostk::astro::NumericalSolver::StateVector& template_x
    )
    {
        return x.rows() == template_x.rows();
    }
};

}  // namespace boost::numeric::odeint

namespace ostk
{
namespace astro
{

using namespace boost::numeric::odeint;

typedef runge_kutta4<NumericalSolver::StateVector, double, NumericalSolver::StateVector, double, vector_space_algebra>
    stepper_type_4;
typedef runge_kutta_cash_karp54<
    NumericalSolver::StateVector,
    double,
    NumericalSolver::StateVector,
    double,
    vector_space_algebra>
    error_stepper_type_54;
typedef runge_kutta_fehlberg78<
    NumericalSolver::StateVector,
    double,
    NumericalSolver::StateVector,
    double,
    vector_space_algebra>
    error_stepper_type_78;

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

Array<NumericalSolver::Solution> NumericalSolver::integrateDuration(
    const NumericalSolver::StateVector& anInitialStateVector,
    const Array<Real>& aDurationArray,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    return integrateTime(anInitialStateVector, 0.0, aDurationArray, aSystemOfEquations);
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
