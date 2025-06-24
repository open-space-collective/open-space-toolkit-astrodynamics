/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/TLESolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/LeastSquaresSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Table;
using ostk::core::container::Tuple;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;

using ostk::astrodynamics::estimator::TLESolver;
using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

Array<State> loadData(const String& aFileName)
{
    Array<State> observations;

    const Table observationData = Table::Load(
        File::Path(Path::Parse(
            String::Format("/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/TLESolverData/{}.csv", aFileName)
        )),
        Table::Format::CSV,
        true
    );

    for (const auto& observationRow : observationData)
    {
        const Instant instant = Instant::DateTime(DateTime::Parse(observationRow[0].accessString()), Scale::UTC);
        const Position position = Position::Meters(
            {observationRow[1].accessReal(), observationRow[2].accessReal(), observationRow[3].accessReal()},
            Frame::ITRF()
        );
        const Velocity velocity = Velocity::MetersPerSecond(
            {observationRow[4].accessReal(), observationRow[5].accessReal(), observationRow[6].accessReal()},
            Frame::ITRF()
        );

        const State state = State(instant, position, velocity);

        observations.add(state);
    }

    return observations;
}

Array<State> truncateObservations(const Array<State>& anObservations, const Duration& aDuration)
{
    const Instant observationsCutoffInstant = anObservations.accessLast().getInstant() - aDuration;
    return anObservations.getWhere(
        [&observationsCutoffInstant](const auto& observation)
        {
            return observation.getInstant() >= observationsCutoffInstant;
        }
    );
}

Array<State> truncatePredictions(const Array<State>& aPredictions, const Duration& aDuration)
{
    const Instant predictsCutoffInstant = aPredictions.accessFirst().getInstant() + aDuration;
    return aPredictions.getWhere(
        [&predictsCutoffInstant](const auto& prediction)
        {
            return prediction.getInstant() <= predictsCutoffInstant;
        }
    );
}

class OpenSpaceToolkit_Astrodynamics_Solver_TLESolver_Analysis : public ::testing::Test
{
   protected:
    const Size observationCount_ = 10;
    const String terminationCriteria = "Test Criteria";
    const State estimatedState_ = State(
        Instant::J2000(),
        Position::Meters({7.0e6, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({7.5e3, 0.0, 0.0}, Frame::GCRF())
    );
    const MatrixXd estimatedCovariance_ = MatrixXd::Identity(6, 6);
    const MatrixXd estimatedFrisbeeCovariance_ = MatrixXd::Identity(6, 6);
    const Array<State> computedObservationStates_ = {State(
        Instant::J2000(),
        Position::Meters({7.0e6, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({7.5e3, 0.0, 0.0}, Frame::GCRF())
    )};
    const Array<LeastSquaresSolver::Step> steps_ = {
        LeastSquaresSolver::Step(2.0, VectorXd::Ones(6)), LeastSquaresSolver::Step(1.0, VectorXd::Ones(6))
    };

    const LeastSquaresSolver::Analysis solverAnalysis_ = LeastSquaresSolver::Analysis(
        terminationCriteria,
        estimatedState_,
        estimatedCovariance_,
        estimatedFrisbeeCovariance_,
        computedObservationStates_,
        steps_
    );

    const TLE estimatedTLE_ = {
        "1 60504U 24149AN  25040.94027346  .00011269  00000-0  48620-3 0  9993",
        "2 60504  97.4240 120.0505 0003172 221.0946 139.0052 15.22694144 26919"
    };
    const TLESolver::Analysis analysis_ = {
        estimatedTLE_,
        solverAnalysis_,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_TLESolver_Analysis, Analysis)
{
    {
        EXPECT_NO_THROW(TLESolver::Analysis(estimatedTLE_, solverAnalysis_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_TLESolver_Analysis, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << analysis_ << std::endl);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_TLESolver_Analysis, Print)
{
    {
        testing::internal::CaptureStdout();

        analysis_.print(std::cout);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}

class OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver : public ::testing::Test
{
   protected:
    const LeastSquaresSolver leastSquaresSolver_ = LeastSquaresSolver::Default();
    const TLESolver tleSolver_ = {leastSquaresSolver_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, Constructor)
{
    {
        EXPECT_NO_THROW(TLESolver {leastSquaresSolver_});
        EXPECT_NO_THROW(TLESolver(leastSquaresSolver_, 0, "00001A", 0, true));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, Accessors)
{
    {
        EXPECT_NO_THROW(tleSolver_.accessSolver());
        EXPECT_EQ(tleSolver_.accessSatelliteNumber(), 0);
        EXPECT_EQ(tleSolver_.accessInternationalDesignator(), "00001A");
        EXPECT_EQ(tleSolver_.accessRevolutionNumber(), 0);
        EXPECT_EQ(tleSolver_.accessEstimateBStar(), true);
        EXPECT_EQ(tleSolver_.accessEstimationFrame(), Frame::GCRF());
        EXPECT_EQ(tleSolver_.accessDefaultBStar(), 0.0);
        EXPECT_EQ(tleSolver_.accessFirstDerivativeMeanMotionDividedBy2(), 0.0);
        EXPECT_EQ(tleSolver_.accessSecondDerivativeMeanMotionDividedBy6(), 0.0);
        EXPECT_EQ(tleSolver_.accessEphemerisType(), 0);
        EXPECT_EQ(tleSolver_.accessElementSetNumber(), 0);
        EXPECT_NO_THROW(tleSolver_.accessTLEStateBuilder());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, Estimate_InitialGuess_TLE)
{
    {
        const Array<State> observations = truncateObservations(loadData("observations"), Duration::Hours(12.0));

        const TLESolver::Analysis analysis = tleSolver_.estimate(std::make_pair(observations[0], 3.5e-4), observations);

        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(analysis.solverAnalysis.iterationCount, tleSolver_.accessSolver().getMaxIterationCount());
        EXPECT_LT(analysis.solverAnalysis.rmsError, 510.0);

        // Verify re-running with same TLE as initial guess
        const TLESolver::Analysis secondAnalysis = tleSolver_.estimate(analysis.estimatedTLE, observations);

        EXPECT_EQ(secondAnalysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(secondAnalysis.solverAnalysis.iterationCount, tleSolver_.accessSolver().getMaxIterationCount());
        EXPECT_LT(secondAnalysis.solverAnalysis.rmsError, 510.0);
    }
}

class OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver_EstimateTLEWithBStar_Parameterized
    : public ::testing::TestWithParam<Tuple<Duration, Duration, Real, Real, Real>>
{
   protected:
    const Real testBStar_ = 3.5e-4;
    const TLESolver tleSolver_ = {LeastSquaresSolver::Default(), 0, "00001A", 0, true};
};

INSTANTIATE_TEST_SUITE_P(
    EstimateTLETests,
    OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver_EstimateTLEWithBStar_Parameterized,
    ::testing::Values(
        // Fit span duration, prediction duration, maximum RMS error, maximum position error, maximum velocity error
        std::make_tuple(Duration::Hours(12.0), Duration::Hours(8.5), 510.0, 1100.0, 1.5),
        std::make_tuple(Duration::Hours(48.0), Duration::Hours(52.0), 680.0, 1400.0, 1.8),
        std::make_tuple(Duration::Hours(72.0), Duration::Hours(60.0), 680.0, 1400.0, 1.8),
        std::make_tuple(Duration::Hours(96.0), Duration::Hours(74.0), 800.0, 1800.0, 2.5)
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver_EstimateTLEWithBStar_Parameterized, EstimateTLE)
{
    {
        const auto& [fitSpanDuration, predictionDuration, maximumRmsError, maximumPositionError, maximumVelocityError] =
            GetParam();

        const Array<State> observations = truncateObservations(loadData("observations"), fitSpanDuration);
        const Array<State> predictions = truncatePredictions(loadData("predictions"), predictionDuration);

        const TLESolver::Analysis analysis =
            tleSolver_.estimate(std::make_pair(observations[0], testBStar_), observations);

        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(analysis.solverAnalysis.iterationCount, tleSolver_.accessSolver().getMaxIterationCount());
        EXPECT_LT(analysis.solverAnalysis.rmsError, maximumRmsError);

        // Verify estimated TLE accuracy
        const TLE estimatedTLE = analysis.estimatedTLE;
        const SGP4 sgp4(estimatedTLE);

        for (const auto& observation : observations)
        {
            const State propagatedState = sgp4.calculateStateAt(observation.getInstant());
            const Vector3d positionDelta = propagatedState.getPosition().getCoordinates() -
                                           observation.inFrame(Frame::GCRF()).getPosition().getCoordinates();
            const Vector3d velocityDelta = propagatedState.getVelocity().getCoordinates() -
                                           observation.inFrame(Frame::GCRF()).getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), maximumPositionError);
            EXPECT_LT(velocityDelta.norm(), maximumVelocityError);
        }

        // Verify prediction accuracy
        for (const auto& prediction : predictions)
        {
            const State propagatedState = sgp4.calculateStateAt(prediction.getInstant());
            const Vector3d positionDelta = propagatedState.getPosition().getCoordinates() -
                                           prediction.inFrame(Frame::GCRF()).getPosition().getCoordinates();
            const Vector3d velocityDelta = propagatedState.getVelocity().getCoordinates() -
                                           prediction.inFrame(Frame::GCRF()).getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), 10000.0);
            EXPECT_LT(velocityDelta.norm(), 12.0);
        }
    }
}

class OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver_EstimateTLEWithoutBStar_Parameterized
    : public ::testing::TestWithParam<Tuple<Duration, Duration, Real, Real, Real>>
{
   protected:
    const TLESolver tleSolver_ = {LeastSquaresSolver::Default(), 0, "00001A", 0, false};
};

INSTANTIATE_TEST_SUITE_P(
    EstimateTLETests,
    OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver_EstimateTLEWithoutBStar_Parameterized,
    ::testing::Values(
        // Fit span duration, prediction duration, maximum RMS error, maximum position error, maximum velocity error
        std::make_tuple(Duration::Minutes(40.0), Duration::Hours(30.0), 30.0, 100.0, 0.5),
        std::make_tuple(Duration::Hours(2.0), Duration::Hours(30.0), 80.0, 300.0, 0.8),
        std::make_tuple(Duration::Hours(6.0), Duration::Hours(25.0), 400.0, 800.0, 1.2),
        std::make_tuple(Duration::Hours(12.0), Duration::Hours(21.0), 800.0, 1400.0, 1.5)
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver_EstimateTLEWithoutBStar_Parameterized, Estimate)
{
    {
        const auto& [fitSpanDuration, predictionDuration, maximumRmsError, maximumPositionError, maximumVelocityError] =
            GetParam();

        const Array<State> observations = truncateObservations(loadData("observations"), fitSpanDuration);
        const Array<State> predictions = truncatePredictions(loadData("predictions"), predictionDuration);

        const TLESolver::Analysis analysis = tleSolver_.estimate(observations[0], observations);

        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(analysis.solverAnalysis.iterationCount, tleSolver_.accessSolver().getMaxIterationCount());
        EXPECT_LT(analysis.solverAnalysis.rmsError, maximumRmsError);

        // Verify estimated TLE accuracy
        const TLE estimatedTLE = analysis.estimatedTLE;
        const SGP4 sgp4(estimatedTLE);

        for (const auto& observation : observations)
        {
            const State propagatedState = sgp4.calculateStateAt(observation.getInstant());

            const Vector3d positionDelta = propagatedState.getPosition().getCoordinates() -
                                           observation.inFrame(Frame::GCRF()).getPosition().getCoordinates();
            const Vector3d velocityDelta = propagatedState.getVelocity().getCoordinates() -
                                           observation.inFrame(Frame::GCRF()).getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), maximumPositionError);
            EXPECT_LT(velocityDelta.norm(), maximumVelocityError);
        }

        // Verify prediction accuracy
        for (const auto& prediction : predictions)
        {
            const State propagatedState = sgp4.calculateStateAt(prediction.getInstant());

            const Vector3d positionDelta = propagatedState.getPosition().getCoordinates() -
                                           prediction.inFrame(Frame::GCRF()).getPosition().getCoordinates();
            const Vector3d velocityDelta = propagatedState.getVelocity().getCoordinates() -
                                           prediction.inFrame(Frame::GCRF()).getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), 10000.0);
            EXPECT_LT(velocityDelta.norm(), 12.0);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, Estimate_Failures)
{
    // Test invalid input type when estimating BStar
    {
        const State cartesianState = State(
            Instant::DateTime(DateTime::Parse("2020-12-01T14:55:30.000"), Scale::UTC),
            Position::Meters({6514453.7323, 2256044.8405, -457775.3075}, Frame::GCRF()),
            Velocity::MetersPerSecond({798.3880, -779.1942, 7514.8828}, Frame::GCRF())
        );
        EXPECT_THROW(tleSolver_.estimate(cartesianState, Array<State>::Empty()), ostk::core::error::RuntimeError);
    }
    // Test invalid input state (non-cartesian input state)
    {
        const VectorXd coordinates = VectorXd::Ones(6);
        const State nonCartesianState = State(
            Instant::DateTime(DateTime::Parse("2020-12-01T14:55:30.000"), Scale::UTC),
            coordinates,
            Frame::TEME(),
            {
                std::make_shared<CoordinateSubset>("INCLINATION", 1),
                std::make_shared<CoordinateSubset>("RAAN", 1),
                std::make_shared<CoordinateSubset>("ECCENTRICITY", 1),
                std::make_shared<CoordinateSubset>("AOP", 1),
                std::make_shared<CoordinateSubset>("MEAN_ANOMALY", 1),
                std::make_shared<CoordinateSubset>("MEAN_MOTION", 1),
            }
        );
        EXPECT_THROW(
            tleSolver_.estimate(std::make_pair(nonCartesianState, 0.0), Array<State>::Empty()),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, Estimate_NegativeEccentricity)
{
    const State initialState = State(
        Instant::DateTime(DateTime::Parse("2024-12-04T23:50:00"), Scale::UTC),
        Position::Meters({-130897.3295, 1348403.6866, -6841471.7888}, Frame::GCRF()),
        Velocity::MetersPerSecond({5678.9079, -4865.1520, -1075.1813}, Frame::GCRF())
    );

    const Shared<const Earth> earthSPtr = std::make_shared<Earth>(Earth::Spherical());

    const Environment environment = Environment(Instant::J2000(), {earthSPtr});

    const Propagator propagator = Propagator::Default(environment);

    const Interval interval =
        Interval::Closed(initialState.getInstant(), initialState.getInstant() + Duration::Days(1.0));

    const Array<Instant> instants = interval.generateGrid(Duration::Minutes(10.0));

    const Array<State> observations = propagator.calculateStatesAt(initialState, instants);

    const TLESolver tleSolver = {LeastSquaresSolver::Default(), 0, "00001A", 0, false};

    const TLESolver::Analysis analysis = tleSolver.estimate(observations[0], observations);

    EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, EstimateOrbit)
{
    {
        const Duration fitSpanDuration = Duration::Hours(12.0);
        const Duration predictionDuration = Duration::Hours(8.5);
        const Real maximumPositionError = 1100.0;
        const Real maximumVelocityError = 1.5;

        const Array<State> observations = truncateObservations(loadData("observations"), fitSpanDuration);
        const Array<State> predictions = truncatePredictions(loadData("predictions"), predictionDuration);

        const Orbit orbit = tleSolver_.estimateOrbit(std::make_pair(observations[0], 3.5e-4), observations);

        // Verify estimated TLE accuracy
        for (const auto& observation : observations)
        {
            const State propagatedState = orbit.getStateAt(observation.getInstant());

            const Vector3d positionDelta = propagatedState.getPosition().getCoordinates() -
                                           observation.inFrame(Frame::GCRF()).getPosition().getCoordinates();
            const Vector3d velocityDelta = propagatedState.getVelocity().getCoordinates() -
                                           observation.inFrame(Frame::GCRF()).getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), maximumPositionError);
            EXPECT_LT(velocityDelta.norm(), maximumVelocityError);
        }

        // Verify prediction accuracy
        for (const auto& prediction : predictions)
        {
            const State propagatedState = orbit.getStateAt(prediction.getInstant());

            const Vector3d positionDelta = propagatedState.getPosition().getCoordinates() -
                                           prediction.inFrame(Frame::GCRF()).getPosition().getCoordinates();
            const Vector3d velocityDelta = propagatedState.getVelocity().getCoordinates() -
                                           prediction.inFrame(Frame::GCRF()).getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), 10000.0);
            EXPECT_LT(velocityDelta.norm(), 12.0);
        }
    }
}
