////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory/Orbit/Determination/BatchLeastSquares.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Orbit/Determination/BatchLeastSquares.hpp>
#include <Library/Astrodynamics/Trajectory/Orbit/Models/Propagator.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/Solver.hpp>
#include <Library/Astrodynamics/Flight/Dynamics/System.hpp>

#include <Library/Physics/Units/Mass.hpp>

#include <Library/Core/Error.hpp>
#include <Library/Core/Utilities.hpp>

#include <nlopt.hpp>

#include <numeric>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace library
{
namespace astro
{
namespace trajectory
{
namespace orbit
{
namespace determination
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                BatchLeastSquares::BatchLeastSquares        (   const   Environment&                anEnvironment,
                                                                                const   BatchLeastSquares::Configuration& aConfiguration                        )
                                :   environment_(anEnvironment),
                                    configuration_(aConfiguration)
{

}

BatchLeastSquares::Output       BatchLeastSquares::estimateOrbit            (   const   Array<Pair<Instant, Position>>& aMeasurementArray                       ) const
{

    using library::math::obj::Vector3d ;

    using library::physics::units::Mass ;
    using library::physics::time::Duration ;

    using library::astro::flight::dynamics::System ;
    using library::astro::flight::dynamics::Solver ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::orbit::models::Propagator ;

    if (aMeasurementArray.getSize() < 2)
    {
        throw library::core::error::RuntimeError("At least 2 measurements must be provided.") ;
    }

    Environment environment = environment_ ;

    const Integer initialRevolutionNumber = 1 ;
    const System system = { Mass::Kilograms(50.0), environment } ; // [TBM] Param
    const Solver solver = Solver::RungeKutta4(1e-16, 1e-16) ;

    const auto [ initialInstant, initialPosition ] = aMeasurementArray.at(0) ;

    const auto referenceFrame = initialPosition.accessFrame() ;

    // Context setup

    struct Context
    {

        const Integer& initialRevolutionNumber ;
        const System& system ;
        const Solver& solver ;
        const Instant& initialInstant ;
        const Shared<const Frame>& referenceFrame ;
        const Array<Pair<Instant, Position>>& measurements ;
        const BatchLeastSquares::Configuration& configuration ;

    } ;

    Context context = { initialRevolutionNumber, system, solver, initialInstant, referenceFrame, aMeasurementArray, configuration_ } ;

    //

    const auto generateInitialStateVector =
    [&aMeasurementArray, &referenceFrame] () -> std::vector<double>
    {

        const auto [ instant, position ] = aMeasurementArray.at(0) ;
        const auto [ nextInstant, nextPosition ] = aMeasurementArray.at(1) ;

        const Velocity velocity = Velocity::MetersPerSecond((nextPosition.inUnit(Position::Unit::Meter).getCoordinates() - position.inUnit(Position::Unit::Meter).getCoordinates()) / Duration::Between(instant, nextInstant).inSeconds(), referenceFrame ) ;

        const Vector3d x_GCRF = position.inUnit(Position::Unit::Meter).getCoordinates() ;
        const Vector3d v_GCRF_in_GCRF = velocity.getCoordinates() ;

        // const Vector3d x_GCRF = { 8000000.000000000000, 0.000000000000, 0.000000000000 } ;
        // const Vector3d v_GCRF_in_GCRF = { 0.000000000000, 5467.635056630975, 5467.635056630975 } ;

        // const Vector3d x_GCRF = { 8000000.000000000000, 0.000000000000, 0.000000000000 } ;

        // const Vector3d x_GCRF = { 8000030.000000000000, 10.000000000000, 20.000000000000 } ;

        // const Vector3d v_GCRF_in_GCRF = { 0.000000000000, 5467.635056630975, 5467.635056630975 } ;

        // const Vector3d v_GCRF_in_GCRF = { 0.000000000000, 0.000000000000, 7733.403651194109 } ;
        // const Vector3d v_GCRF_in_GCRF = { 6.000000000000, -3.000000000000, 7733.403651194109 } ;

        return { x_GCRF.x(), x_GCRF.y(), x_GCRF.z(), v_GCRF_in_GCRF.x(), v_GCRF_in_GCRF.y(), v_GCRF_in_GCRF.z() } ;
        // return { x_GCRF.x(), x_GCRF.y(), x_GCRF.z() } ;

    } ;

    const auto stateFromVector =
    [&referenceFrame] (const Instant& anInstant, const std::vector<double>& aStateVector) -> State
    {

        const Vector3d v_GCRF_in_GCRF = { 0.000000000000, 5467.635056630975, 5467.635056630975 } ; // [TBR]

        const Position position = Position::Meters({ aStateVector.at(0), aStateVector.at(1), aStateVector.at(2) }, referenceFrame ) ;
        const Velocity velocity = Velocity::MetersPerSecond(v_GCRF_in_GCRF, referenceFrame ) ;

        return { anInstant, position, velocity } ;

    } ;

    const auto costFunction =
    [] (const std::vector<double>& aStateVector, std::vector<double>& aGradientVector, void* aContextPtr) -> double
    {

        (void) aGradientVector ;

        if (aContextPtr == nullptr)
        {
            throw library::core::error::runtime::Undefined("Context") ;
        }

        const Context* contextPtr = static_cast<const Context*>(aContextPtr) ;

        const Position position = Position::Meters({ aStateVector.at(0), aStateVector.at(1), aStateVector.at(2) }, contextPtr->referenceFrame ) ;
        const Velocity velocity = Velocity::MetersPerSecond({ aStateVector.at(3), aStateVector.at(4), aStateVector.at(5) }, contextPtr->referenceFrame ) ;
        // const Velocity velocity = Velocity::MetersPerSecond({ 0.000000000000, 5467.635056630975, 5467.635056630975 }, contextPtr->referenceFrame ) ; // [TBR]

        const State initialState = { contextPtr->initialInstant, position, velocity } ;

        const Propagator propagatorModel = { initialState, contextPtr->initialRevolutionNumber, contextPtr->system, contextPtr->solver } ;

        double localDeviationSum = 0.0 ;

        std::vector<double> localDeviations ;

        for (const auto& measurement : contextPtr->measurements)
        {

            const auto [ instant, measuredPosition ] = measurement ;

            const State estimatedState = propagatorModel.calculateStateAt(instant) ;

            const Vector3d errorVector = estimatedState.getPosition().accessCoordinates() - measuredPosition.accessCoordinates() ;

            const double localDeviation_m = errorVector.dot(errorVector) ;

            localDeviations.push_back(localDeviation_m) ;

            localDeviationSum += localDeviation_m ;

            // std::cout << String::Format("@ {} : Est.: {} / Meas.: {} -> {} [m]", instant.toString(), estimatedState.getPosition().accessCoordinates().toString(12), measuredPosition.accessCoordinates().toString(12), localDeviation_m) << std::endl ;

        }

        const auto [ minLocalDeviationIt, maxLocalDeviationIt ] = std::minmax_element(std::begin(localDeviations), std::end(localDeviations)) ;

        const double minLocalDeviation = (*minLocalDeviationIt) ;
        const double maxLocalDeviation = (*maxLocalDeviationIt) ;
        const double averageLocalDeviation = std::accumulate(localDeviations.begin(), localDeviations.end(), 0.0) / localDeviations.size() ;

        const double cost = localDeviationSum / 2.0 ;
        // const double cost = maxLocalDeviation ;

        if (contextPtr->configuration.verbose)
        {
            std::cout << String::Format("x: {} v: {} | c: [{} - {} ~ {}] -> {} [m]", position.getCoordinates().toString(12), velocity.getCoordinates().toString(12), minLocalDeviation, maxLocalDeviation, averageLocalDeviation, cost) << std::endl ;
        }

        return cost ;

    } ;

    nlopt::algorithm algorithm ;

    if (configuration_.algorithm == "LN_COBYLA")
    {
        algorithm = nlopt::LN_COBYLA ;
    }
    else if (configuration_.algorithm == "LN_BOBYQA")
    {
        algorithm = nlopt::LN_BOBYQA ;
    }
    else if (configuration_.algorithm == "LN_PRAXIS")
    {
        algorithm = nlopt::LN_PRAXIS ;
    }
    else if (configuration_.algorithm == "LN_NELDERMEAD")
    {
        algorithm = nlopt::LN_NELDERMEAD ;
    }
    else if (configuration_.algorithm == "LN_SBPLX")
    {
        algorithm = nlopt::LN_SBPLX ;
    }
    else
    {
        throw library::core::error::RuntimeError("Algorithm [{}] is wrong.", configuration_.algorithm) ;
    }


    nlopt::opt optimizer = { algorithm, 6 } ;
    // nlopt::opt optimizer = { algorithm, 3 } ;

    if (configuration_.stateLowerBound.isDefined())
    {
        optimizer.set_lower_bounds(std::vector<double> { configuration_.stateLowerBound.data(), configuration_.stateLowerBound.data() + configuration_.stateLowerBound.rows() * configuration_.stateLowerBound.cols() }) ;
    }

    if (configuration_.stateUpperBound.isDefined())
    {
        optimizer.set_upper_bounds(std::vector<double> { configuration_.stateUpperBound.data(), configuration_.stateUpperBound.data() + configuration_.stateUpperBound.rows() * configuration_.stateUpperBound.cols() }) ;
    }

    optimizer.set_min_objective(costFunction, &context) ;

    if (configuration_.initialState.isDefined())
    {
        optimizer.set_initial_step(std::vector<double> { configuration_.initialState.data(), configuration_.initialState.data() + configuration_.initialState.rows() * configuration_.initialState.cols() }) ;
    }

    if (configuration_.stoppingFunctionValue.isDefined())
    {
        optimizer.set_stopval(configuration_.stoppingFunctionValue) ;
    }

    if (configuration_.relativeStateTolerance.isDefined())
    {
        optimizer.set_xtol_rel(configuration_.relativeStateTolerance) ;
    }

    // optimizer.set_x_weights({ 1e0, 1e0, 1e0, 1e-3, 1e-3, 1e-3 }); // [TBM] Param
    // optimizer.set_xtol_abs({ 1e-3, 1e-3, 1e-3, 1e-6, 1e-6, 1e-6 }) ; // [TBM] Param

    if (configuration_.relativeFunctionTolerance.isDefined())
    {
        optimizer.set_ftol_rel(configuration_.relativeFunctionTolerance) ;
    }

    if (configuration_.absoluteFunctionTolerance.isDefined())
    {
        optimizer.set_ftol_abs(configuration_.absoluteFunctionTolerance) ;
    }

    if (configuration_.maximumEvaluationCount.isDefined())
    {
        optimizer.set_maxeval(configuration_.maximumEvaluationCount) ;
    }

    if (configuration_.maximumDuration.isDefined())
    {
        optimizer.set_maxtime(configuration_.maximumDuration.inSeconds()) ;
    }

    std::vector<double> stateVector = generateInitialStateVector() ;

    double minimumCost = -1.0 ;
    size_t evaluationCount = 0 ;
    String message = String::Empty() ;
    Orbit orbit = Orbit::Undefined() ;

    try
    {

        nlopt::result result = optimizer.optimize(stateVector, minimumCost) ;

        bool success = false ;

        switch (result)
        {

            case nlopt::STOPVAL_REACHED:
                success = true ;
                message = "STOPVAL_REACHED" ;
                break ;

            case nlopt::FTOL_REACHED:
                success = true ;
                message = "FTOL_REACHED" ;
                break ;

            case nlopt::XTOL_REACHED:
                success = true ;
                message = "XTOL_REACHED" ;
                break ;

            case nlopt::MAXEVAL_REACHED:
                success = true ;
                message = "MAXEVAL_REACHED" ;
                break ;

            case nlopt::MAXTIME_REACHED:
                success = true ;
                message = "MAXTIME_REACHED" ;
                break ;

            case nlopt::FAILURE:
            case nlopt::INVALID_ARGS:
            case nlopt::OUT_OF_MEMORY:
            case nlopt::ROUNDOFF_LIMITED:
            case nlopt::FORCED_STOP:
            default:
                message = "Cannot estimate orbit (solution did not converge)." ;
                // throw library::core::error::RuntimeError("Cannot estimate orbit (solution did not converge).") ;
                break ;

        }

        if (success)
        {

            const Propagator propagatorModel = { stateFromVector(initialInstant, stateVector), initialRevolutionNumber, system, solver } ;

            orbit = Orbit { propagatorModel, environment.accessCelestialObjectWithName("Earth") } ; // [TBM] environment is only accessible here...

        }

        evaluationCount = optimizer.get_numevals() ;

        // std::cout << String::Format("Solution ({} evaluations): {} [m]", evaluationCount, minimumCost) << std::endl ;

    }
    catch (const std::exception& anException)
    {
        // throw library::core::error::RuntimeError("Cannot estimate orbit (algorithm failed): [{}].", anException.what()) ;
        message = String::Format("Cannot estimate orbit (algorithm failed): [{}].", anException.what()) ;
    }

    if (configuration_.verbose)
    {

        std::cout << "Minimum cost: " << minimumCost << std::endl ;
        std::cout << "Evaluation count: " << evaluationCount << std::endl ;
        std::cout << "Message: " << message << std::endl ;

    }

    const BatchLeastSquares::Output output =
    {
        orbit,
        minimumCost,
        evaluationCount,
        message
    } ;

    return output ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
