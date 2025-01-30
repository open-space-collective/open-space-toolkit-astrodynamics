/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/TLESolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/BrouwerLyddaneMean/BrouwerLyddaneMeanLong.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{

using ostk::core::type::Shared;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::orbit::model::blm::BrouwerLyddaneMeanLong;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;

TLESolver::Analysis::Analysis(const TLE& aDeterminedTLE, const LeastSquaresSolver::Analysis& anAnalysis)
    : determinedTLE(aDeterminedTLE),
      solverAnalysis(anAnalysis)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const TLESolver::Analysis& anAnalysis)
{
    anAnalysis.print(anOutputStream);

    return anOutputStream;
}

void TLESolver::Analysis::print(std::ostream& anOutputStream) const
{
    ostk::core::utils::Print::Header(anOutputStream, "Analysis");

    ostk::core::utils::Print::Separator(anOutputStream, "Determined TLE");
    ostk::core::utils::Print::Line(anOutputStream) << determinedTLE;

    ostk::core::utils::Print::Separator(anOutputStream, "Analysis");
    solverAnalysis.print(anOutputStream);

    ostk::core::utils::Print::Footer(anOutputStream);
}

TLESolver::TLESolver(
    const LeastSquaresSolver& aSolver,
    const Integer& aSatelliteNumber,
    const String& anInternationalDesignator,
    const Integer& aRevolutionNumber,
    const bool aFitWithBStar,
    const Shared<const Frame>& anEstimationFrameSPtr
)
    : solver_(aSolver),
      satelliteNumber_(aSatelliteNumber),
      internationalDesignator_(anInternationalDesignator),
      revolutionNumber_(aRevolutionNumber),
      defaultBStar_(1e-4),
      firstDerivativeMeanMotionDividedBy2_(0.0),
      secondDerivativeMeanMotionDividedBy6_(0.0),
      ephemerisType_(0),
      elementSetNumber_(0),
      fitWithBStar_(aFitWithBStar),
      tleStateBuilder_(StateBuilder::Undefined()),
      estimationFrameSPtr_(anEstimationFrameSPtr)
{
    // Setup coordinate subsets for TLE state
    Array<Shared<const CoordinateSubset>> coordinateSubsets = {
        std::make_shared<CoordinateSubset>("INCLINATION", 1),
        std::make_shared<CoordinateSubset>("RAAN", 1),
        std::make_shared<CoordinateSubset>("ECCENTRICITY", 1),
        std::make_shared<CoordinateSubset>("AOP", 1),
        std::make_shared<CoordinateSubset>("MEAN_ANOMALY", 1),
        std::make_shared<CoordinateSubset>("MEAN_MOTION", 1)
    };

    if (fitWithBStar_)
    {
        coordinateSubsets.add(std::make_shared<CoordinateSubset>("B_STAR", 1));
    }

    tleStateBuilder_ = StateBuilder(Frame::TEME(), coordinateSubsets);
}

const LeastSquaresSolver& TLESolver::accessSolver() const
{
    return solver_;
}

const Integer& TLESolver::accessSatelliteNumber() const
{
    return satelliteNumber_;
}

const String& TLESolver::accessInternationalDesignator() const
{
    return internationalDesignator_;
}

const Integer& TLESolver::accessRevolutionNumber() const
{
    return revolutionNumber_;
}

const Real& TLESolver::accessDefaultBStar() const
{
    return defaultBStar_;
}

const Real& TLESolver::accessFirstDerivativeMeanMotionDividedBy2() const
{
    return firstDerivativeMeanMotionDividedBy2_;
}

const Real& TLESolver::accessSecondDerivativeMeanMotionDividedBy6() const
{
    return secondDerivativeMeanMotionDividedBy6_;
}

const Integer& TLESolver::accessEphemerisType() const
{
    return ephemerisType_;
}

const Integer& TLESolver::accessElementSetNumber() const
{
    return elementSetNumber_;
}

const bool& TLESolver::accessFitWithBStar() const
{
    return fitWithBStar_;
}

const StateBuilder& TLESolver::accessTLEStateBuilder() const
{
    return tleStateBuilder_;
}

TLESolver::Analysis TLESolver::estimateTLE(
    const std::variant<TLE, Pair<State, Real>, State>& anInitialGuess,
    const Array<State>& anObservationArray,
    const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas,
    const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas
) const
{
    // Convert initial guess to TLE state
    State initialGuessTLEState = State::Undefined();

    if (const auto* tle = std::get_if<TLE>(&anInitialGuess))
    {
        initialGuessTLEState = TLEToTLEState(*tle);
    }
    else if (const auto* pair = std::get_if<Pair<State, Real>>(&anInitialGuess))
    {
        if (fitWithBStar_)
        {
            initialGuessTLEState = CartesianStateAndBStarToTLEState(pair->first, pair->second);
        }
        else
        {
            defaultBStar_ = pair->second;
            initialGuessTLEState = CartesianStateAndBStarToTLEState(pair->first);
        }
    }
    else if (const auto* state = std::get_if<State>(&anInitialGuess))
    {
        if (fitWithBStar_)
        {
            throw ostk::core::error::RuntimeError("Initial guess must be a TLE or (State, B*) when fitting with B*.");
        }
        initialGuessTLEState = CartesianStateAndBStarToTLEState(*state);
    }

    // Convert inputs to an inertial frame for estimation
    const Array<State> observationsInEstimationFrame = anObservationArray.map<State>(
        [estimationFrameSPtr = estimationFrameSPtr_](const State& aState) -> State
        {
            return aState.inFrame(estimationFrameSPtr);
        }
    );

    // Define state generator
    const auto generateStates = [this](const State& aState, const Array<Instant>& anInstantArray) -> Array<State>
    {
        const TLE tle = TLEStateToTLE(aState);
        const SGP4 sgp4(tle);

        Array<State> states;
        states.reserve(anInstantArray.getSize());

        for (const auto& instant : anInstantArray)
        {
            states.add(sgp4.calculateStateAt(instant).inFrame(estimationFrameSPtr_));
        }

        return states;
    };

    // Solve least squares problem
    const LeastSquaresSolver::Analysis analysis = solver_.solve(
        initialGuessTLEState, observationsInEstimationFrame, generateStates, anInitialGuessSigmas, anObservationSigmas
    );

    // Convert solution state to TLE
    const TLE determinedTLE = TLEStateToTLE(analysis.solutionState);

    return Analysis(determinedTLE, analysis);
}

State TLESolver::TLEToTLEState(const TLE& aTLE) const
{
    Array<double> coordinates = {
        aTLE.getInclination().inRadians(),
        aTLE.getRaan().inRadians(),
        aTLE.getEccentricity(),
        aTLE.getAop().inRadians(),
        aTLE.getMeanAnomaly().inRadians(),
        aTLE.getMeanMotion().in(Derived::Unit::RevolutionPerDay())
    };

    if (fitWithBStar_)
    {
        coordinates.add(aTLE.getBStarDragTerm());
    }

    return tleStateBuilder_.build(aTLE.getEpoch(), Eigen::Map<VectorXd>(coordinates.data(), coordinates.size()));
}

TLE TLESolver::TLEStateToTLE(const State& aTLEState) const
{
    const VectorXd coordinates = aTLEState.getCoordinates();

    return TLE::Construct(
        satelliteNumber_,
        "U",
        internationalDesignator_,
        aTLEState.getInstant(),
        firstDerivativeMeanMotionDividedBy2_,
        secondDerivativeMeanMotionDividedBy6_,
        fitWithBStar_ ? Real(coordinates[6]) : defaultBStar_,
        ephemerisType_,
        elementSetNumber_,
        Angle::Radians(coordinates[0]),                              // inclination
        Angle::Radians(coordinates[1]),                              // RAAN
        coordinates[2],                                              // eccentricity
        Angle::Radians(coordinates[3]),                              // AOP
        Angle::Radians(coordinates[4]),                              // mean anomaly
        Derived(coordinates[5], Derived::Unit::RevolutionPerDay()),  // mean motion
        revolutionNumber_
    );
}

State TLESolver::CartesianStateAndBStarToTLEState(const State& aCartesianState, const Real& aBStar) const
{
    const State cartesianStateTEME = aCartesianState.inFrame(Frame::TEME());

    // Convert to Brouwer-Lyddane mean elements
    const BrouwerLyddaneMeanLong coe = BrouwerLyddaneMeanLong::Cartesian(
        {cartesianStateTEME.getPosition(), cartesianStateTEME.getVelocity()}, Earth::EGM2008.gravitationalParameter_
    );

    Array<double> coordinates = {
        coe.getInclination().inRadians(),
        coe.getRaan().inRadians(),
        coe.getEccentricity(),
        coe.getAop().inRadians(),
        coe.getMeanAnomaly().inRadians(),
        coe.getMeanMotion(Earth::EGM2008.gravitationalParameter_).in(Derived::Unit::RevolutionPerDay())
    };

    if (fitWithBStar_)
    {
        if (!aBStar.isDefined())
        {
            throw ostk::core::error::RuntimeError("B* must be provided when fitting with B*.");
        }

        coordinates.add(aBStar);
    }

    return tleStateBuilder_.build(
        cartesianStateTEME.getInstant(), Eigen::Map<VectorXd>(coordinates.data(), coordinates.size())
    );
}

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk
