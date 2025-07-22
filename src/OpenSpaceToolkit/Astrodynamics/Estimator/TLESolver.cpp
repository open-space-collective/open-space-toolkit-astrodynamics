/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/TLESolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/ModifiedEquinoctial.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>

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
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::Instant;
using ostk::physics::unit::Length;

using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
using ostk::astrodynamics::trajectory::orbit::model::kepler::ModifiedEquinoctial;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;

const Shared<const CoordinateSubset> TLESolver::SemiLatusRectumSubset =
    std::make_shared<CoordinateSubset>("SEMI_LATUS_RECTUM", 1);
const Shared<const CoordinateSubset> TLESolver::EccentricityXSubset =
    std::make_shared<CoordinateSubset>("ECCENTRICITY_X", 1);
const Shared<const CoordinateSubset> TLESolver::EccentricityYSubset =
    std::make_shared<CoordinateSubset>("ECCENTRICITY_Y", 1);
const Shared<const CoordinateSubset> TLESolver::NodeXSubset = std::make_shared<CoordinateSubset>("NODE_X", 1);
const Shared<const CoordinateSubset> TLESolver::NodeYSubset = std::make_shared<CoordinateSubset>("NODE_Y", 1);
const Shared<const CoordinateSubset> TLESolver::TrueLongitudeSubset =
    std::make_shared<CoordinateSubset>("TRUE_LONGITUDE", 1);
const Shared<const CoordinateSubset> TLESolver::BStarSubset = std::make_shared<CoordinateSubset>("B_STAR", 1);

TLESolver::Analysis::Analysis(const TLE& aEstimatedTLE, const LeastSquaresSolver::Analysis& anAnalysis)
    : estimatedTLE(aEstimatedTLE),
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

    ostk::core::utils::Print::Separator(anOutputStream, "Estimated TLE");
    ostk::core::utils::Print::Line(anOutputStream) << estimatedTLE;

    ostk::core::utils::Print::Separator(anOutputStream, "Analysis");
    solverAnalysis.print(anOutputStream);

    ostk::core::utils::Print::Footer(anOutputStream);
}

TLESolver::TLESolver(
    const LeastSquaresSolver& aSolver,
    const Integer& aSatelliteNumber,
    const String& anInternationalDesignator,
    const Integer& aRevolutionNumber,
    const bool anEstimateBStar,
    const Shared<const Frame>& anEstimationFrameSPtr
)
    : solver_(aSolver),
      satelliteNumber_(aSatelliteNumber),
      internationalDesignator_(anInternationalDesignator),
      revolutionNumber_(aRevolutionNumber),
      estimateBStar_(anEstimateBStar),
      estimationFrameSPtr_(anEstimationFrameSPtr),
      defaultBStar_(0.0),
      firstDerivativeMeanMotionDividedBy2_(0.0),
      secondDerivativeMeanMotionDividedBy6_(0.0),
      ephemerisType_(0),
      elementSetNumber_(0),
      tleStateBuilder_(StateBuilder::Undefined())
{
    // Setup coordinate subsets for TLE state
    Array<Shared<const CoordinateSubset>> coordinateSubsets = {
        SemiLatusRectumSubset,
        EccentricityXSubset,
        EccentricityYSubset,
        NodeXSubset,
        NodeYSubset,
        TrueLongitudeSubset,
    };

    if (estimateBStar_)
    {
        coordinateSubsets.add(BStarSubset);
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

const bool& TLESolver::accessEstimateBStar() const
{
    return estimateBStar_;
}

const Shared<const Frame>& TLESolver::accessEstimationFrame() const
{
    return estimationFrameSPtr_;
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

const StateBuilder& TLESolver::accessTLEStateBuilder() const
{
    return tleStateBuilder_;
}

TLESolver::Analysis TLESolver::estimate(
    const std::variant<TLE, Pair<State, Real>, State>& anInitialGuess,
    const Array<State>& anObservationStateArray,
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
        if (estimateBStar_)
        {
            initialGuessTLEState = CartesianStateAndBStarToTLEState(pair->first, pair->second);
        }
        else
        {
            initialGuessTLEState = CartesianStateAndBStarToTLEState(pair->first);
            defaultBStar_ = pair->second;
        }
    }
    else if (const auto* state = std::get_if<State>(&anInitialGuess))
    {
        if (estimateBStar_)
        {
            throw ostk::core::error::RuntimeError("Initial guess must be a TLE or (State, B*) when also estimating B*."
            );
        }
        initialGuessTLEState = CartesianStateAndBStarToTLEState(*state);
    }

    const Array<State> observationsInEstimationFrame = anObservationStateArray.map<State>(
        [this](const State& aState) -> State
        {
            return aState.inFrame(estimationFrameSPtr_);
        }
    );

    const auto stateGenerator = [this](const State& aState, const Array<Instant>& anInstantArray) -> Array<State>
    {
        const TLE tle = TLEStateToTLE(aState);
        const SGP4 sgp4(tle);

        Array<State> states;
        states.reserve(anInstantArray.getSize());

        for (const Instant& instant : anInstantArray)
        {
            states.add(sgp4.calculateStateAt(instant).inFrame(estimationFrameSPtr_));
        }

        return states;
    };

    const LeastSquaresSolver::Analysis analysis = solver_.solve(
        initialGuessTLEState, observationsInEstimationFrame, stateGenerator, anInitialGuessSigmas, anObservationSigmas
    );

    // Convert solution state to TLE
    const TLE estimatedTLE = TLEStateToTLE(analysis.estimatedState);

    return Analysis(estimatedTLE, analysis);
}

Orbit TLESolver::estimateOrbit(
    const std::variant<TLE, Pair<State, Real>, State>& anInitialGuess,
    const Array<State>& anObservationStateArray,
    const std::unordered_map<CoordinateSubset, VectorXd>& anInitialGuessSigmas,
    const std::unordered_map<CoordinateSubset, VectorXd>& anObservationSigmas
) const
{
    const Analysis analysis =
        estimate(anInitialGuess, anObservationStateArray, anInitialGuessSigmas, anObservationSigmas);

    return Orbit(SGP4(analysis.estimatedTLE), std::make_shared<Earth>(Earth::Spherical()));
}

State TLESolver::TLEToTLEState(const TLE& aTLE) const
{
    const SGP4 sgp4(aTLE);
    const State state = sgp4.calculateStateAt(aTLE.getEpoch()).inFrame(Frame::TEME());

    const ModifiedEquinoctial modifiedEquinoctial = ModifiedEquinoctial::Cartesian(
        {state.getPosition(), state.getVelocity()}, EarthGravitationalModel::EGM2008.gravitationalParameter_
    );

    Array<double> coordinates = {
        modifiedEquinoctial.getSemiLatusRectum().inKilometers(),
        modifiedEquinoctial.getEccentricityX(),
        modifiedEquinoctial.getEccentricityY(),
        modifiedEquinoctial.getNodeX(),
        modifiedEquinoctial.getNodeY(),
        modifiedEquinoctial.getTrueLongitude().inRadians(),
    };

    if (estimateBStar_)
    {
        coordinates.add(aTLE.getBStarDragTerm());
    }

    return tleStateBuilder_.build(aTLE.getEpoch(), Eigen::Map<VectorXd>(coordinates.data(), coordinates.size()));
}

TLE TLESolver::TLEStateToTLE(const State& aTLEState) const
{
    const ModifiedEquinoctial modifiedEquinoctial = {
        Length::Kilometers(aTLEState.extractCoordinate(SemiLatusRectumSubset)[0]),
        aTLEState.extractCoordinate(EccentricityXSubset)[0],
        aTLEState.extractCoordinate(EccentricityYSubset)[0],
        aTLEState.extractCoordinate(NodeXSubset)[0],
        aTLEState.extractCoordinate(NodeYSubset)[0],
        Angle::Radians(aTLEState.extractCoordinate(TrueLongitudeSubset)[0]),
    };

    const COE coe = modifiedEquinoctial.toCOE(EarthGravitationalModel::EGM2008.gravitationalParameter_);

    return TLE::Construct(
        satelliteNumber_,
        "U",
        internationalDesignator_,
        aTLEState.getInstant(),
        firstDerivativeMeanMotionDividedBy2_,
        secondDerivativeMeanMotionDividedBy6_,
        estimateBStar_ ? Real(aTLEState.extractCoordinate(BStarSubset)[0]) : defaultBStar_,
        ephemerisType_,
        elementSetNumber_,
        coe.getInclination(),
        coe.getRaan(),
        coe.getEccentricity(),
        coe.getAop(),
        coe.getMeanAnomaly(),
        coe.getMeanMotion(EarthGravitationalModel::EGM2008.gravitationalParameter_),
        revolutionNumber_
    );
}

State TLESolver::CartesianStateAndBStarToTLEState(const State& aCartesianState, const Real& aBStar) const
{
    const State cartesianStateTEME = aCartesianState.inFrame(Frame::TEME());

    const ModifiedEquinoctial modifiedEquinoctial = ModifiedEquinoctial::Cartesian(
        {cartesianStateTEME.getPosition(), cartesianStateTEME.getVelocity()},
        EarthGravitationalModel::EGM2008.gravitationalParameter_
    );

    Array<double> coordinates = {
        modifiedEquinoctial.getSemiLatusRectum().inKilometers(),
        modifiedEquinoctial.getEccentricityX(),
        modifiedEquinoctial.getEccentricityY(),
        modifiedEquinoctial.getNodeX(),
        modifiedEquinoctial.getNodeY(),
        modifiedEquinoctial.getTrueLongitude().inRadians(0.0, Real::TwoPi()),
    };

    if (estimateBStar_)
    {
        coordinates.add(aBStar);
    }

    return tleStateBuilder_.build(
        cartesianStateTEME.getInstant(), Eigen::Map<VectorXd>(coordinates.data(), coordinates.size())
    );
}

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk
