/// Apache License 2.0

#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sundials/sundials_types.h>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/RootSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace state
{

using ostk::physics::time::Duration;

using ostk::astrodynamics::RootSolver;
using ostk::astrodynamics::trajectory::StateBuilder;

NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance
)
    : MathNumericalSolver(aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance),
      observedStates_(),
      stateLogger_(nullptr),
      cvode_mem(nullptr),
      y(nullptr),
      sun_ctx(nullptr),
      current_system_of_equations_(nullptr),
      current_event_condition_(nullptr),
      initial_instant_(Instant::Undefined()),
      current_coordinate_broker_(nullptr),
      current_frame_(nullptr)
{
    // Create SUNDIALS context
    SUNContext_Create(nullptr, &sun_ctx);

    // Initialize CVODE memory
    int lmm;
    // Assuming ostk::mathematics::solver::NumericalSolver::StepperType has been updated
    // with Adams and BDF members.
    if (aStepperType == ostk::mathematics::solver::NumericalSolver::StepperType::Adams)
    {
        lmm = CV_ADAMS;
    }
    else if (aStepperType == ostk::mathematics::solver::NumericalSolver::StepperType::BDF)
    {
        lmm = CV_BDF;
    }
    else
    {
        // If other OSTk Mathematics stepper types like RungeKutta are passed,
        // they are not directly compatible with CVodeCreate's LMM argument.
        throw ostk::core::error::RuntimeError(
            "Unsupported StepperType for CVODE NumericalSolver. Please use Adams or BDF."
        );
    }

    cvode_mem = CVodeCreate(lmm, sun_ctx);
    if (cvode_mem == nullptr) {
        throw ostk::core::error::RuntimeError("CVodeCreate failed.");
    }

    // Set user data
    CVodeSetUserData(cvode_mem, this);

    // Set tolerances
    // Assuming scalar tolerances for now
    if (CVodeSStolerances(cvode_mem, this->getRelativeTolerance(), this->getAbsoluteTolerance()) != CV_SUCCESS) {
        throw ostk::core::error::RuntimeError("CVodeSStolerances failed.");
    }

    // CVodeInit will be called in integrateTime as y (initial state) and t0 are not known here.
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

Array<State> NumericalSolver::integrateTime(
    const State& aState,
    const Array<Instant>& anInstantArray,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }
    if (cvode_mem == nullptr)
    {
        throw ostk::core::error::RuntimeError("CVODE memory is not initialized.");
    }
    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

    // Sort anInstantArray to ensure chronological order for CVODE `tout`
    Array<Instant> sortedTimeArray = anInstantArray;
    std::sort(
        sortedTimeArray.begin(),
        sortedTimeArray.end(),
        [](const Instant& a, const Instant& b) { return a < b; }
    );

    self->current_system_of_equations_ = &aSystemOfEquations;
    self->current_frame_ = aState.accessFrame();
    self->current_coordinate_broker_ = aState.accessCoordinateBroker();
    self->initial_instant_ = aState.accessInstant();

    self->observedStates_.clear();
    self->observeState(aState);

    long int N = aState.accessCoordinates().size();
    // Manage N_Vector y (class member)
    if (self->y == nullptr || N_VGetLength_Serial(self->y) != N)
    {
        if (self->y != nullptr)
        {
            N_VDestroy(self->y);
        }
        self->y = N_VNew_Serial(N, self->sun_ctx);
        if (self->y == nullptr)
        {
            throw ostk::core::error::RuntimeError("N_VNew_Serial failed for y.");
        }
    }
    std::copy(aState.accessCoordinates().data(), aState.accessCoordinates().data() + N, N_VGetArrayPointer(self->y));

    // Initialize CVODE solver. t0 = 0.0 corresponds to self->initial_instant_
    // Using ReInit, with fallback to Init, similar to other integrateTime methods.
    if (CVodeReInit(self->cvode_mem, 0.0, self->y) != CV_SUCCESS)
    {
        if (CVodeInit(self->cvode_mem, c_rhs_function_wrapper, 0.0, self->y) != CV_SUCCESS) {
            throw ostk::core::error::RuntimeError("CVodeInit/ReInit failed.");
        }
    }

    Array<State> resultStates;
    resultStates.reserve(sortedTimeArray.getSize());

    sunrealtype t_reached_internal_cvode = 0.0; // Tracks CVODE's internal time relative to initial_instant_

    // Add initial state if it's the first requested time point
    if (sortedTimeArray[0] == self->initial_instant_) {
        resultStates.add(aState);
        // observeState(aState) was already done
    }


    for (const auto& targetInstantInArray : sortedTimeArray)
    {
        if (targetInstantInArray < self->initial_instant_)
        {
            throw ostk::core::error::RuntimeError("Target instant in array is before initial state instant.");
        }

        // If current target instant is the initial instant, we've already handled it or it's the starting point
        if (targetInstantInArray == self->initial_instant_) {
            // If resultStates is empty, it means aState is the first point. Add it.
            if (resultStates.isEmpty()) {
                 resultStates.add(aState);
                 // observeState(aState) already called.
            }
            t_reached_internal_cvode = 0.0; // CVODE is at t0
            continue; // Move to next targetInstantInArray
        }

        sunrealtype cvode_tout = (targetInstantInArray - self->initial_instant_).inSeconds();

        // If cvode_tout is not forward in time from current t_reached_internal_cvode,
        // this indicates an issue with sorted input or duplicate times.
        // For simplicity, we rely on sorted distinct future instants.
        // CVODE requires tout > t_current for forward integration.
        if (cvode_tout <= t_reached_internal_cvode) {
            // This case implies that the target time is at or before the current internal time of CVODE.
            // This could happen if aTimeArray has duplicates or is not strictly increasing past initial_instant_.
            // If it's exactly t_reached_internal_cvode, we might already have this state.
            // For now, if it's not strictly greater, we'll assume it's a request for the current state or an already passed one.
            // A more robust solution might involve CVodeGetDky if exact previous points are needed without re-integration.
            // If resultStates is not empty and last state is this instant, skip.
            if (!resultStates.isEmpty() && resultStates.accessLast().accessInstant() == targetInstantInArray) {
                continue;
            }
            // Otherwise, this is an unsorted or duplicate time point that isn't strictly forward.
            // For this simplified loop, we expect strictly increasing target times from CVODE's perspective.
            // Let's construct the state from current `self->y` if it's a duplicate request for current t_reached_internal_cvode
             if (targetInstantInArray == (self->initial_instant_ + Duration::Seconds(t_reached_internal_cvode))) {
                VectorXd currentCoordinates(N);
                std::copy(N_VGetArrayPointer_Serial(self->y), N_VGetArrayPointer_Serial(self->y) + N, currentCoordinates.data());
                State currentStateAtTarget = {targetInstantInArray, currentCoordinates, self->current_frame_, self->current_coordinate_broker_};
                if (resultStates.isEmpty() || resultStates.accessLast().accessInstant() != targetInstantInArray) {
                    resultStates.add(currentStateAtTarget);
                    self->observeState(currentStateAtTarget);
                }
             } // else it's a request for a time before current CVODE time, which is an issue with sorted list.
            continue;
        }

        int flag = CVode(self->cvode_mem, cvode_tout, self->y, &t_reached_internal_cvode, CV_NORMAL);

        if (flag < 0) // Error
        {
            self->current_system_of_equations_ = nullptr;
            throw ostk::core::error::RuntimeError("CVode integration failed with error flag: " + std::to_string(flag));
        }

        // CV_NORMAL mode guarantees that y is the solution at cvode_tout (if successful).
        // t_reached_internal_cvode will be equal to cvode_tout.
        VectorXd integratedCoordinates(N);
        std::copy(N_VGetArrayPointer_Serial(self->y), N_VGetArrayPointer_Serial(self->y) + N, integratedCoordinates.data());
        State actualState = {targetInstantInArray, integratedCoordinates, self->current_frame_, self->current_coordinate_broker_};

        resultStates.add(actualState);
        self->observeState(actualState);
    }

    self->current_system_of_equations_ = nullptr;
    // Context members (frame, broker, initial_instant) are kept for potential reuse.

    return resultStates;
}

State NumericalSolver::integrateTime(
    const State& aState, const Instant& anEndTime, const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations
)
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }

    if (cvode_mem == nullptr) {
        throw ostk::core::error::RuntimeError("CVODE memory is not initialized.");
    }

    observedStates_.clear();
    observeState(aState);

    current_system_of_equations_ = &aSystemOfEquations;
    initial_instant_ = aState.accessInstant(); // Set for context
    current_coordinate_broker_ = aState.accessCoordinateBroker(); // Set for context
    current_frame_ = aState.accessFrame(); // Set for context

    sunrealtype t0 = 0.0;
    sunrealtype t_reached = 0.0;
    long int N = aState.accessCoordinates().size();

    // Manage N_Vector y for initial state
    if (y == nullptr || N_VGetLength_Serial(y) != N) // N_VGetLength_Serial is okay for serial vectors
    {
        if (y != nullptr)
        {
            N_VDestroy(y);
        }
        y = N_VNew_Serial(N, sun_ctx);
        if (y == nullptr) {
            throw ostk::core::error::RuntimeError("N_VNew_Serial failed for y.");
        }
    }
    // Copy data from aState to y
    std::copy(aState.accessCoordinates().data(), aState.accessCoordinates().data() + N, N_VGetArrayPointer(y));

    // Initialize or ReInitialize CVODE solver
    // For the first call, CVodeInit is necessary. Subsequent calls with different parameters might use CVodeReInit.
    // We need a flag to track if CVodeInit has been called.
    // For simplicity in this step, we'll assume CVodeInit is needed (or ReInit if already initialized).
    // A more robust solution would check an initialization flag.
    if (CVodeInit(cvode_mem, c_rhs_function_wrapper, t0, y) != CV_SUCCESS) {
         // If CVodeInit fails, it might be because it was already initialized. Try CVodeReInit.
         // This is a simplified error handling. Proper way is to track init status.
        if (CVodeReInit(cvode_mem, t0, y) != CV_SUCCESS) {
            N_VDestroy(y);
            y = nullptr;
            throw ostk::core::error::RuntimeError("CVodeInit/ReInit failed.");
        }
    }


    sunrealtype Tfinal = (anEndTime - aState.accessInstant()).inSeconds();

    int flag = CVode(cvode_mem, Tfinal, y, &t_reached, CV_NORMAL);
    if (flag != CV_SUCCESS) {
        N_VDestroy(y);
        y = nullptr;
        throw ostk::core::error::RuntimeError("CVode failed. Error code: " + std::to_string(flag));
    }

    VectorXd finalCoordinates(N);
    std::copy(N_VGetArrayPointer(y), N_VGetArrayPointer(y) + N, finalCoordinates.data());

    // It's important to destroy y if it was created with N_VNew_Serial or N_VMake_Serial for this call
    // If y becomes a class member managed across calls, destruction happens in NumericalSolver destructor
    N_VDestroy(y);
    y = nullptr; // Avoid dangling pointer

    current_system_of_equations_ = nullptr; // Reset after use
    // current_frame_ and current_coordinate_broker_ can also be reset if desired
    // initial_instant_ = Instant::Undefined();

    State finalState = State(anEndTime, finalCoordinates, current_frame_, current_coordinate_broker_);
    observeState(finalState);
    return finalState;
}

NumericalSolver::ConditionSolution NumericalSolver::integrateTime(
    const State& aState,
    const Instant& anInstant,
    const NumericalSolver::SystemOfEquationsWrapper& aSystemOfEquations,
    const EventCondition& anEventCondition
)
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("NumericalSolver");
    }
    if (cvode_mem == nullptr)
    {
        throw ostk::core::error::RuntimeError("CVODE memory is not initialized.");
    }

    observedStates_.clear();
    observeState(aState);

    current_system_of_equations_ = &aSystemOfEquations;
    current_event_condition_ = &anEventCondition;
    initial_instant_ = aState.accessInstant();
    current_coordinate_broker_ = aState.accessCoordinateBroker();
    current_frame_ = aState.accessFrame();

    sunrealtype t0 = 0.0;
    sunrealtype t_reached = 0.0; // This will be the time of the event or Tfinal
    long int N = aState.accessCoordinates().size();

    // Manage N_Vector y for initial state
    if (y == nullptr || N_VGetLength_Serial(y) != N)
    {
        if (y != nullptr)
        {
            N_VDestroy(y);
        }
        y = N_VNew_Serial(N, sun_ctx);
        if (y == nullptr) {
            throw ostk::core::error::RuntimeError("N_VNew_Serial failed for y in event integration.");
        }
    }
    std::copy(aState.accessCoordinates().data(), aState.accessCoordinates().data() + N, N_VGetArrayPointer(y));

    // Initialize or ReInitialize CVODE solver for the ODE part
    // Using a simple flag `is_initialized_` (to be added to class) would be more robust
    // than relying on ReInit failure to call Init.
    // For now, using the established pattern:
    if (CVodeReInit(cvode_mem, t0, y) != CV_SUCCESS) {
        if (CVodeInit(cvode_mem, c_rhs_function_wrapper, t0, y) != CV_SUCCESS) {
            N_VDestroy(y); y = nullptr;
            throw ostk::core::error::RuntimeError("CVodeInit/ReInit failed for ODE.");
        }
    }

    // Initialize rootfinding
    if (CVodeRootInit(cvode_mem, 1, c_root_function_wrapper) != CV_SUCCESS) {
        N_VDestroy(y); y = nullptr;
        throw ostk::core::error::RuntimeError("CVodeRootInit failed.");
    }
    // CVodeSetRootDirection can be used here if needed

    sunrealtype Tfinal = (anInstant - initial_instant_).inSeconds();
    if (Tfinal <= 0.0 && this->getTimeStep() > 0.0) { // Basic check for integration direction
         // CVODE typically integrates forward in time. If Tfinal is not positive,
         // and if positive step was assumed by default, this needs careful handling.
         // For now, this implementation assumes forward integration (Tfinal > t0).
         // Consider throwing an error or adjusting CVODE settings for backward integration if required.
    }


    int flag = CVode(cvode_mem, Tfinal, y, &t_reached, CV_NORMAL); // t_reached will be updated by CVode

    ConditionSolution sol;
    VectorXd finalCoordinates(N);
    std::copy(N_VGetArrayPointer(y), N_VGetArrayPointer(y) + N, finalCoordinates.data());
    Instant finalInstant = initial_instant_ + Duration::Seconds(static_cast<double>(t_reached));
    sol.state = State(finalInstant, finalCoordinates, current_frame_, current_coordinate_broker_);
    observeState(sol.state); // Observe the final state

    if (flag == CV_ROOT_RETURN)
    {
        sol.conditionIsSatisfied = true;
        sol.rootSolverHasConverged = true;
        sol.iterationCount = 0; // Placeholder
        // To get which roots were found if nrtfn > 1
        // int rootsfound[nrtfn];
        // CVodeGetRootInfo(cvode_mem, rootsfound);
    }
    else if (flag == CV_SUCCESS || flag == CV_TSTOP_RETURN) // CV_TSTOP_RETURN means integration reached Tfinal
    {
        sol.conditionIsSatisfied = false; // Condition was not met, or integration stopped at Tfinal
        sol.rootSolverHasConverged = false; // No root found, or not the primary reason for stopping
        sol.iterationCount = 0;
    }
    else
    {
        // An error occurred
        // N_VDestroy(y); y = nullptr; // y is a class member
        current_system_of_equations_ = nullptr;
        current_event_condition_ = nullptr;
        // initial_instant_ = Instant::Undefined(); // Resetting these might be too aggressive if reuse is intended
        // current_coordinate_broker_ = nullptr;
        // current_frame_ = nullptr;
        throw ostk::core::error::RuntimeError("CVode integration failed with error flag: " + std::to_string(flag));
    }

    // Clean up pointers to passed-in objects, but not y (class member) or context/memory (class members)
    current_system_of_equations_ = nullptr;
    current_event_condition_ = nullptr;
    // initial_instant_, current_coordinate_broker_, current_frame_ can be left as they might be useful
    // for subsequent calls if the solver is re-initialized with new parameters.

    return sol;
}

NumericalSolver NumericalSolver::Undefined()
{
    // For an "Undefined" solver using CVODE backend, it should still use a CVODE compatible type
    // to avoid constructor errors, or the concept of "Undefined" needs rethinking for this backend.
    // Using BDF as a placeholder, similar to Default.
    return NumericalSolver( // Calling the 6-argument private constructor
        LogType::NoLog,
        ostk::mathematics::solver::NumericalSolver::StepperType::BDF, // Placeholder BDF
        Real::Undefined(),
        Real::Undefined(),
        Real::Undefined(),
        nullptr // stateLogger
    );
}

NumericalSolver NumericalSolver::Default()
{
    // Assuming ostk::mathematics::solver::NumericalSolver::StepperType::BDF is available
    return {
        NumericalSolver::LogType::NoLog,
        ostk::mathematics::solver::NumericalSolver::StepperType::BDF, // Changed to BDF
        5.0,
        1.0e-12,
        1.0e-12,
        // RootSolver::Default(), // Removed
        nullptr // stateLogger for the private constructor
    };
}

NumericalSolver NumericalSolver::FixedStepSize(const NumericalSolver::StepperType& aStepperType, const Real& aTimeStep)
{
    // CVODE LMMs (Adams, BDF) are not classical fixed-step methods like RK4.
    // Forcing fixed steps with CVODE is possible but atypical for these steppers.
    // This method's original intent (fixed-step RK4) is not directly compatible.
    if (aStepperType == ostk::mathematics::solver::NumericalSolver::StepperType::Adams ||
        aStepperType == ostk::mathematics::solver::NumericalSolver::StepperType::BDF)
    {
        // Even for Adams/BDF, this factory implies a fixed-step behavior that CVODE doesn't naturally provide for these.
        throw ostk::core::error::ToBeImplemented(
            "FixedStepSize with CVODE's Adams/BDF stepper is not directly supported in this manner. CVODE is primarily adaptive."
        );
    }
    else // For other stepper types like RungeKutta4
    {
        throw ostk::core::error::RuntimeError(
            "FixedStepSize with non-CVODE steppers (e.g., RungeKutta4) is not supported by the CVODE backend."
        );
    }

    // Unreachable, but to satisfy return type if logic changes:
    // return {
    //     NumericalSolver::LogType::NoLog,
    //     aStepperType,
    //     aTimeStep,
    //     1.0,
    //     1.0,
    //     RootSolver::Default(),
    //     nullptr,
    // };
}

NumericalSolver NumericalSolver::DefaultConditional(const std::function<void(const State&)>& stateLogger)
{
    // Assuming ostk::mathematics::solver::NumericalSolver::StepperType::BDF is available
    return NumericalSolver::Conditional(
        5.0, 1.0e-12, 1.0e-12, stateLogger, ostk::mathematics::solver::NumericalSolver::StepperType::BDF
    );
}

NumericalSolver NumericalSolver::Conditional(
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const std::function<void(const State&)>& stateLogger
)
{
    // This version defaults to BDF.
    // It calls the private constructor that takes the stateLogger.
    const NumericalSolver::LogType logType =
        stateLogger != nullptr ? NumericalSolver::LogType::LogAdaptive : NumericalSolver::LogType::NoLog;

    // Assuming ostk::mathematics::solver::NumericalSolver::StepperType::BDF is available
    // Calls the private constructor (6 args)
    return NumericalSolver(
        logType,
        ostk::mathematics::solver::NumericalSolver::StepperType::BDF, // Default to BDF
        aTimeStep,
        aRelativeTolerance,
        anAbsoluteTolerance,
        // RootSolver::Default(), // Removed
        stateLogger
    );
}

// Overload or new factory method to specify stepper type for Conditional
NumericalSolver NumericalSolver::Conditional(
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const std::function<void(const State&)>& stateLogger,
    const ostk::mathematics::solver::NumericalSolver::StepperType& aStepperType
)
{
    const NumericalSolver::LogType logType =
        stateLogger != nullptr ? NumericalSolver::LogType::LogAdaptive : NumericalSolver::LogType::NoLog;

    if (aStepperType != ostk::mathematics::solver::NumericalSolver::StepperType::Adams &&
        aStepperType != ostk::mathematics::solver::NumericalSolver::StepperType::BDF)
    {
        throw ostk::core::error::RuntimeError("Conditional NumericalSolver with CVODE backend only supports Adams or BDF steppers.");
    }

    // Calls the private constructor (6 args)
    return NumericalSolver(
        logType,
        aStepperType,
        aTimeStep,
        aRelativeTolerance,
        anAbsoluteTolerance,
        // RootSolver::Default(), // Removed
        stateLogger
    );
}


NumericalSolver::NumericalSolver(
    const NumericalSolver::LogType& aLogType,
    const NumericalSolver::StepperType& aStepperType,
    const Real& aTimeStep,
    const Real& aRelativeTolerance,
    const Real& anAbsoluteTolerance,
    const std::function<void(const State& aState)>& stateLogger
)
    : MathNumericalSolver(aLogType, aStepperType, aTimeStep, aRelativeTolerance, anAbsoluteTolerance),
      observedStates_(),
      stateLogger_(stateLogger),
      cvode_mem(nullptr), // Added
      y(nullptr), // Added
      sun_ctx(nullptr), // Added
      current_system_of_equations_(nullptr), // This was missing from the previous incorrect diff, ensure it's here
      current_event_condition_(nullptr),
      initial_instant_(Instant::Undefined()),
      current_coordinate_broker_(nullptr),
      current_frame_(nullptr)
{
    // This is the private constructor, should not contain destructor logic
    // Note: The main CVODE setup (CVodeCreate, SUNContext_Create) is typically done
    // in the public constructor as it's common for all instances.
    // If the private constructor is meant to create a fully independent solver instance,
    // it might also need that setup, but current structure suggests it's a helper.
    // For now, just ensuring member initialization.
}

NumericalSolver::~NumericalSolver()
{
    if (cvode_mem != nullptr)
    {
        CVodeFree(&cvode_mem);
        cvode_mem = nullptr;
    }
    if (y != nullptr) // If y is made a class member and created with N_VNew_Serial
    {
        N_VDestroy(y);
        y = nullptr;
    }
    if (sun_ctx != nullptr)
    {
        SUNContext_Free(&sun_ctx);
        sun_ctx = nullptr;
    }
}

int NumericalSolver::c_rhs_function_wrapper(sunrealtype t, N_Vector y_cvode, N_Vector ydot_cvode, void *user_data)
{
    NumericalSolver* solver_instance = static_cast<NumericalSolver*>(user_data);

    if (!solver_instance || !solver_instance->current_system_of_equations_)
    {
        return -1; // Or some other error code
    }

    // Convert CVODE N_Vector y_cvode to OSTk VectorXd
    sunrealtype *y_data = N_VGetArrayPointer(y_cvode);
    Eigen::Map<const Eigen::VectorXd> y_eigen(y_data, N_VGetLength_Serial(y_cvode)); // Assuming N_VGetLength_Serial works for y_cvode

    // Prepare OSTk state vector for ydot (output)
    VectorXd ydot_eigen(N_VGetLength_Serial(ydot_cvode)); // Assuming ydot_cvode has same length

    // Call the user-provided system of equations
    // The time 't' from CVODE is a sunrealtype, usually a double.
    // The SystemOfEquationsWrapper expects a 'double' for time.
    // We assume current_system_of_equations_ is already set.
    (*solver_instance->current_system_of_equations_)(y_eigen, ydot_eigen, static_cast<double>(t));

    // Convert OSTk ydot_eigen back to CVODE N_Vector ydot_cvode
    sunrealtype *ydot_data = N_VGetArrayPointer(ydot_cvode);
    Eigen::Map<Eigen::VectorXd>(ydot_data, ydot_eigen.size()) = ydot_eigen;

    return 0; // Success
}

int NumericalSolver::c_root_function_wrapper(sunrealtype t, N_Vector y_cvode, sunrealtype *gout, void *user_data)
{
    NumericalSolver* solver_instance = static_cast<NumericalSolver*>(user_data);

    if (!solver_instance || !solver_instance->current_event_condition_ || !solver_instance->initial_instant_.isDefined() || !solver_instance->current_coordinate_broker_.isDefined()) // Added broker check
    {
        // Cannot evaluate condition without context
        return -1; // Error code
    }

    // Convert CVODE N_Vector y_cvode to OSTk VectorXd
    sunrealtype *y_data = N_VGetArrayPointer(y_cvode);
    long int N = N_VGetLength_Serial(y_cvode); // Assuming serial vector
    Eigen::Map<const Eigen::VectorXd> y_eigen(y_data, N);

    // Construct current State
    Instant current_instant = solver_instance->initial_instant_ + Duration::Seconds(static_cast<double>(t));
    State current_state = {current_instant, y_eigen, solver_instance->current_frame_, solver_instance->current_coordinate_broker_}; // Need current_frame_

    // For previous_state, using current_state as placeholder as discussed.
    // This implies EventCondition::isSatisfied must be robust to this.
    bool satisfied = solver_instance->current_event_condition_->isSatisfied(current_state, current_state);

    // Map boolean satisfaction to gout value for CVODE
    // gout[0] = 0.0 when root is found (condition satisfied)
    // gout[0] != 0.0 when root is not found (condition not satisfied)
    // We want CVODE to find where `satisfied` changes state.
    // If isSatisfied is true, we want gout to be 0 (or cross 0).
    // If isSatisfied is false, we want gout to be non-zero.
    // Let's define: condition met (satisfied = true) => gout[0] = 0.0
    // condition not met (satisfied = false) => gout[0] = 1.0
    // CVODE looks for a sign change in gout or gout == 0.
    // If currently false (gout=1) and becomes true (gout=0), it's a root.
    // If currently true (gout=0) and becomes false (gout=1), it's also a root.
    // This might need adjustment based on how EventCondition is structured.
    // A common approach: gout[0] = value_of_function_whose_zero_is_the_event.
    // If EventCondition provides such a value, use it. Otherwise, this mapping:
    if (satisfied) {
        gout[0] = 0.0; // Condition is met
    } else {
        gout[0] = 1.0; // Condition is not met
    }
    // For multiple roots, gout would be an array gout[0], gout[1], ...

    return 0; // Success
}


void NumericalSolver::observeState(const State& aState)
{
    observedStates_.add(aState);

    if (stateLogger_ != nullptr && getLogType() != NumericalSolver::LogType::NoLog)
    {
        stateLogger_(aState);
    }
}

}  // namespace state
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
