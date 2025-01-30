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
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/TLESolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/LeastSquaresSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
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
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::estimator::TLESolver;
using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::StateBuilder;

Array<State> loadData(const String& aFileName)
{
    Array<State> observations;

    const Table referenceData = Table::Load(
        File::Path(Path::Parse(String::Format("/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/{}.csv", aFileName))),
        Table::Format::CSV,
        true
    );

    for (const auto& referenceRow : referenceData)
    {
        const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
        const Position position = Position::Meters(
            {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()}, Frame::ITRF()
        );
        const Velocity velocity = Velocity::MetersPerSecond(
            {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()}, Frame::ITRF()
        );

        const State state = State(instant, position, velocity);

        observations.add(state);
    }

    return observations;
}

Array<State> truncateObservations(const Array<State>& anObservations, const Duration& aDuration)
{
    const Instant observationsCutoffInstant = anObservations.accessLast().getInstant() - aDuration;
    return anObservations.getWhere([&observationsCutoffInstant](const auto& observation) { return observation.getInstant() >= observationsCutoffInstant; });
}

Array<State> truncatePredictions(const Array<State>& aPredictions, const Duration& aDuration)
{
    const Instant predictsCutoffInstant = aPredictions.accessFirst().getInstant() + aDuration;
    return aPredictions.getWhere([&predictsCutoffInstant](const auto& prediction) { return prediction.getInstant() <= predictsCutoffInstant; });
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

        const auto analysis = tleSolver_.estimateTLE(std::make_pair(observations[0], testBStar_), observations);

        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(analysis.solverAnalysis.iterationCount, tleSolver_.accessSolver().getMaxIterationCount());
        EXPECT_LT(analysis.solverAnalysis.rmsError, maximumRmsError);

        // Verify estimated TLE accuracy
        const TLE estimatedTLE = analysis.determinedTLE;
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
        std::make_tuple(Duration::Minutes(40.0), Duration::Hours(30.0), 30.0, 100.0, 0.5),
        std::make_tuple(Duration::Hours(2.0), Duration::Hours(30.0), 80.0, 300.0, 0.8),
        std::make_tuple(Duration::Hours(6.0), Duration::Hours(25.0), 400.0, 800.0, 1.2),
        std::make_tuple(Duration::Hours(12.0), Duration::Hours(21.0), 800.0, 1400.0, 1.5)
    )
);

TEST_P(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver_EstimateTLEWithoutBStar_Parameterized, EstimateTLE)
{
    {
        const auto& [fitSpanDuration, predictionDuration, maximumRmsError, maximumPositionError, maximumVelocityError] =
            GetParam();

        const Array<State> observations = truncateObservations(loadData("observations"), fitSpanDuration);
        const Array<State> predictions = truncatePredictions(loadData("predictions"), predictionDuration);

        const auto analysis = tleSolver_.estimateTLE(observations[0], observations);

        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(analysis.solverAnalysis.iterationCount, tleSolver_.accessSolver().getMaxIterationCount());
        EXPECT_LT(analysis.solverAnalysis.rmsError, maximumRmsError);

        // Verify estimated TLE accuracy
        const TLE estimatedTLE = analysis.determinedTLE;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, EstimateTLE_Failures)
{
    // Test invalid input type when fitting with BStar
    {
        const State cartesianState = State(
            Instant::DateTime(DateTime::Parse("2020-12-01T14:55:30.000"), Scale::UTC),
            Position::Meters({6514453.7323, 2256044.8405, -457775.3075}, Frame::GCRF()),
            Velocity::MetersPerSecond({798.3880, -779.1942, 7514.8828}, Frame::GCRF())
        );
        EXPECT_THROW(tleSolver_.estimateTLE(cartesianState, Array<State>::Empty()), ostk::core::error::RuntimeError);
    }
}

// TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, EstimateTLE_ReuseInitialGuess)
// {
//     {
//         // First estimation
//         Array<State> definitiveStates;
//         Array<State> predictiveStates;
//         GenerateObservations(Duration::Hours(12.0), definitiveStates, predictiveStates);

//         const auto analysis =
//             tleSolverWithBStar_.estimateTLE(std::make_pair(cartesianState_, testBStar_), definitiveStates);

//         // Second estimation using first result as initial guess
//         const auto analysisRerun = tleSolverWithBStar_.estimateTLE(analysis.determinedTLE, definitiveStates);

//         EXPECT_EQ(analysisRerun.solverAnalysis.terminationCriteria, "RMS Update Threshold");
//         EXPECT_LT(analysisRerun.solverAnalysis.rmsError, 510.0);

//         // Compare TLEs from both runs
//         const TLE& firstTLE = analysis.determinedTLE;
//         const TLE& secondTLE = analysisRerun.determinedTLE;

//         EXPECT_EQ(firstTLE, secondTLE);
//     }
// }

// TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, EstimateTLE_DifferentFitSpans)
// {
//     struct TestCase
//     {
//         Duration fitSpanDuration;
//         Duration predictDuration;
//         Real expectedRmsError;
//         Real expectedPositionDelta;
//         Real expectedVelocityDelta;
//     };

//     const Array<TestCase> testCases = {
//         {Duration::Hours(12.0), Duration::Hours(8.5), 510.0, 1100.0, 1.5},
//         {Duration::Hours(48.0), Duration::Hours(52.0), 680.0, 1400.0, 1.8},
//         {Duration::Hours(72.0), Duration::Hours(60.0), 680.0, 1400.0, 1.8},
//         {Duration::Hours(96.0), Duration::Hours(74.0), 800.0, 1800.0, 2.5}
//     };

//     for (const auto& testCase : testCases)
//     {
//         Array<State> definitiveStates;
//         Array<State> predictiveStates;
//         GenerateObservations(testCase.fitSpanDuration, definitiveStates, predictiveStates);

//         const auto analysis =
//             tleSolverWithBStar_.estimateTLE(std::make_pair(cartesianState_, testBStar_), definitiveStates);

//         EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
//         EXPECT_LT(analysis.solverAnalysis.rmsError, testCase.expectedRmsError);

//         const TLE estimatedTLE = analysis.determinedTLE;
//         const SGP4 sgp4(estimatedTLE);

//         // Check definitive span accuracy
//         for (const auto& refState : definitiveStates)
//         {
//             const State propagatedState = sgp4.calculateStateAt(refState.getInstant());
//             const Vector3d positionDelta =
//                 propagatedState.getPosition().getCoordinates() - refState.getPosition().getCoordinates();
//             const Vector3d velocityDelta =
//                 propagatedState.getVelocity().getCoordinates() - refState.getVelocity().getCoordinates();

//             EXPECT_LT(positionDelta.norm(), testCase.expectedPositionDelta);
//             EXPECT_LT(velocityDelta.norm(), testCase.expectedVelocityDelta);
//         }

//         // Check predictive span accuracy
//         for (const auto& refState : predictiveStates)
//         {
//             const State propagatedState = sgp4.calculateStateAt(refState.getInstant());
//             const Vector3d positionDelta =
//                 propagatedState.getPosition().getCoordinates() - refState.getPosition().getCoordinates();
//             const Vector3d velocityDelta =
//                 propagatedState.getVelocity().getCoordinates() - refState.getVelocity().getCoordinates();

//             EXPECT_LT(positionDelta.norm(), 10000.0);  // Looser bounds for predictive span
//             EXPECT_LT(velocityDelta.norm(), 12.0);
//         }
//     }
// }

class OpenSpaceToolkit_Astrodynamics_Solver_TLESolver_Analysis : public ::testing::Test
{
   protected:
    const Size observationCount_ = 10;
    const String terminationCriteria = "Test Criteria";
    const State solutionState_ = State(
        Instant::J2000(),
        Position::Meters({7.0e6, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({7.5e3, 0.0, 0.0}, Frame::GCRF())
    );
    const MatrixXd solutionCovariance_ = MatrixXd::Identity(6, 6);
    const MatrixXd solutionFrisbeeCovariance_ = MatrixXd::Identity(6, 6);
    const MatrixXd solutionResidualMatrix_ = MatrixXd::Ones(6, 6);
    const Array<LeastSquaresSolver::Step> steps_ = {
        LeastSquaresSolver::Step(2.0, VectorXd::Ones(6)), LeastSquaresSolver::Step(1.0, VectorXd::Ones(6))
    };

    const LeastSquaresSolver::Analysis solverAnalysis_ = LeastSquaresSolver::Analysis(
        observationCount_,
        terminationCriteria,
        solutionState_,
        solutionCovariance_,
        solutionFrisbeeCovariance_,
        solutionResidualMatrix_,
        steps_
    );

    const TLE determinedTLE_ = {
        "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927",
        "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537"
    };
    const TLESolver::Analysis analysis_ = {
        determinedTLE_,
        solverAnalysis_,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_TLESolver_Analysis, Analysis)
{
    {
        EXPECT_NO_THROW(TLESolver::Analysis(determinedTLE_, solverAnalysis_));
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
