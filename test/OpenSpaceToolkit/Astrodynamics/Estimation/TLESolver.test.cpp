/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
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

#include <OpenSpaceToolkit/Astrodynamics/Solver/LeastSquaresSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Solver/TLESolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/TLE.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::type::Real;
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

using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::solver::TLESolver;
using ostk::astrodynamics::trajectory::orbit::model::SGP4;
using ostk::astrodynamics::trajectory::orbit::model::sgp4::TLE;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

class OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        // Setup TLEs
        tleWithNonzeroBStar_ =
            TLE("1     0U 00001A   20336.62187500  .00000000  00000-0  78991-2 0    03",
                "2     0  97.4983  18.8807 0010736 271.5508  84.7267 15.12617431    04");

        tleWithZeroBStar_ =
            TLE("1     0U 00001A   20336.62187500  .00000000  00000-0  00000-0 0    07",
                "2     0  97.4983  18.8807 0010736 271.5508  84.7267 15.12617431    04");

        // Setup test state and BStar
        cartesianState_ = State(
            Instant::DateTime(DateTime::Parse("2020-12-01T14:55:30.000"), Scale::UTC),
            Position::Meters({6514453.7323, 2256044.8405, -457775.3075}, Frame::GCRF()),
            Velocity::MetersPerSecond({798.3880, -779.1942, 7514.8828}, Frame::GCRF())
        );
        testBStar_ = 3.5e-4;

        // Setup solvers
        leastSquaresSolver_ = {20, 1.0};
        tleSolverWithBStar_ = {leastSquaresSolver_, 0, "00001A", 0, true};
        tleSolverWithoutBStar_ = {leastSquaresSolver_, 0, "00001A", 0, false};

        // Setup state subsets
        coordinateSubsets_ = {
            std::make_shared<CoordinateSubset>("INCLINATION", 1),
            std::make_shared<CoordinateSubset>("RAAN", 1),
            std::make_shared<CoordinateSubset>("ECCENTRICITY", 1),
            std::make_shared<CoordinateSubset>("AOP", 1),
            std::make_shared<CoordinateSubset>("MEAN_ANOMALY", 1),
            std::make_shared<CoordinateSubset>("MEAN_MOTION", 1),
            std::make_shared<CoordinateSubset>("B_STAR", 1)
        };

        // Setup reference values from fixture
        referenceValues_ = {1.7016, 0.3295, 0.0012, -1.5721, 7.7903, 15.1368, 3.5e-4};
    }

    TLE tleWithNonzeroBStar_;
    TLE tleWithZeroBStar_;
    State cartesianState_;
    Real testBStar_;
    LeastSquaresSolver leastSquaresSolver_;
    TLESolver tleSolverWithBStar_;
    TLESolver tleSolverWithoutBStar_;
    Array<Shared<const CoordinateSubset>> coordinateSubsets_;
    Array<Real> referenceValues_;

    void GenerateReferenceStates(
        const Duration& aFitSpanDuration, Array<State>& outDefinitiveStates, Array<State>& outPredictiveStates
    )
    {
        // Generate reference states for testing
        const Size stateCount = 100;
        const Instant startInstant = cartesianState_.getInstant();
        const Duration stepSize = Duration::Minutes(15.0);

        outDefinitiveStates.clear();
        outPredictiveStates.clear();

        for (Size i = 0; i < stateCount; ++i)
        {
            const Instant stateInstant = startInstant + stepSize * i;
            State state = cartesianState_;
            state.setInstant(stateInstant);

            if (stateInstant <= (startInstant + aFitSpanDuration))
            {
                outDefinitiveStates.add(state);
            }
            else
            {
                outPredictiveStates.add(state);
            }
        }
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, Constructor)
{
    {
        EXPECT_NO_THROW(TLESolver(leastSquaresSolver_));
        EXPECT_NO_THROW(TLESolver(leastSquaresSolver_, 0, "00001A", 0, true));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, TLEStateConversion)
{
    {
        // Test TLE to TLE state conversion with BStar
        const State tleState = tleSolverWithBStar_.TLEToTLEState(tleWithNonzeroBStar_);

        EXPECT_EQ(tleState.getInstant(), tleWithNonzeroBStar_.getEpoch());
        EXPECT_EQ(tleState.accessFrame(), Frame::TEME());
        EXPECT_EQ(tleState.getCoordinates().size(), 7);

        const TLE reconvertedTLE = tleSolverWithBStar_.TLEStateToTLE(tleState);
        EXPECT_EQ(reconvertedTLE, tleWithNonzeroBStar_);
    }

    {
        // Test TLE to TLE state conversion without BStar
        const State tleState = tleSolverWithoutBStar_.TLEToTLEState(tleWithZeroBStar_);

        EXPECT_EQ(tleState.getInstant(), tleWithZeroBStar_.getEpoch());
        EXPECT_EQ(tleState.accessFrame(), Frame::TEME());
        EXPECT_EQ(tleState.getCoordinates().size(), 6);

        const TLE reconvertedTLE = tleSolverWithoutBStar_.TLEStateToTLE(tleState);
        EXPECT_EQ(reconvertedTLE, tleWithZeroBStar_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, CartesianStateConversion)
{
    {
        // Test Cartesian to TLE state conversion with BStar
        const State tleState = tleSolverWithBStar_.CartesianStateAndBStarToTLEState(cartesianState_, &testBStar_);

        EXPECT_EQ(tleState.getInstant(), cartesianState_.getInstant());
        EXPECT_EQ(tleState.accessFrame(), Frame::TEME());
        EXPECT_EQ(tleState.getCoordinates().size(), 7);

        // Compare against reference values
        const VectorXd coordinates = tleState.getCoordinates();
        for (Size i = 0; i < coordinates.size(); ++i)
        {
            EXPECT_NEAR(coordinates(i), referenceValues_[i], 5e-2);
        }
    }

    {
        // Test without BStar
        const State tleState = tleSolverWithoutBStar_.CartesianStateAndBStarToTLEState(cartesianState_, nullptr);

        EXPECT_EQ(tleState.getInstant(), cartesianState_.getInstant());
        EXPECT_EQ(tleState.accessFrame(), Frame::TEME());
        EXPECT_EQ(tleState.getCoordinates().size(), 6);

        // Compare against reference values (excluding BStar)
        const VectorXd coordinates = tleState.getCoordinates();
        for (Size i = 0; i < coordinates.size(); ++i)
        {
            EXPECT_NEAR(coordinates(i), referenceValues_[i], 5e-2);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, EstimateTLE)
{
    {
        // Setup reference states
        Array<State> definitiveStates;
        Array<State> predictiveStates;
        GenerateReferenceStates(Duration::Hours(12.0), definitiveStates, predictiveStates);

        // Test TLE estimation with BStar fitting
        const auto analysis =
            tleSolverWithBStar_.estimateTLE(std::make_pair(cartesianState_, testBStar_), definitiveStates);

        EXPECT_EQ(analysis.getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysis.getIterationCount(), tleSolverWithBStar_.accessSolver().getMaxIterationCount());
        EXPECT_LT(analysis.getRmsError(), 510.0);

        // Verify estimated TLE accuracy
        const TLE estimatedTLE = analysis.getDeterminedTLE();
        const SGP4 sgp4(estimatedTLE);

        for (const auto& refState : definitiveStates)
        {
            const State propagatedState = sgp4.calculateStateAt(refState.getInstant());
            const Vector3d positionDelta =
                propagatedState.getPosition().getCoordinates() - refState.getPosition().getCoordinates();
            const Vector3d velocityDelta =
                propagatedState.getVelocity().getCoordinates() - refState.getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), 1100.0);
            EXPECT_LT(velocityDelta.norm(), 1.5);
        }
    }

    {
        // Test TLE estimation without BStar fitting
        Array<State> definitiveStates;
        Array<State> predictiveStates;
        GenerateReferenceStates(Duration::Minutes(40.0), definitiveStates, predictiveStates);

        const auto analysis = tleSolverWithoutBStar_.estimateTLE(cartesianState_, definitiveStates);

        EXPECT_EQ(analysis.getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysis.getRmsError(), 30.0);

        const TLE estimatedTLE = analysis.getDeterminedTLE();
        const SGP4 sgp4(estimatedTLE);

        for (const auto& refState : definitiveStates)
        {
            const State propagatedState = sgp4.calculateStateAt(refState.getInstant());
            const Vector3d positionDelta =
                propagatedState.getPosition().getCoordinates() - refState.getPosition().getCoordinates();
            const Vector3d velocityDelta =
                propagatedState.getVelocity().getCoordinates() - refState.getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), 100.0);
            EXPECT_LT(velocityDelta.norm(), 0.5);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, EstimateTLE_Failures)
{
    {
        // Test invalid input type when fitting with BStar
        EXPECT_THROW(
            tleSolverWithBStar_.estimateTLE(cartesianState_, Array<State>::Empty()), ostk::core::error::RuntimeError
        );
    }

    {
        // Test missing BStar when fitting with BStar
        EXPECT_THROW(
            tleSolverWithBStar_.CartesianStateAndBStarToTLEState(cartesianState_, nullptr),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, ComputeTLEResiduals)
{
    {
        Array<State> definitiveStates;
        Array<State> predictiveStates;
        GenerateReferenceStates(Duration::Hours(12.0), definitiveStates, predictiveStates);

        const auto residuals = TLESolver::computeTLEResiduals(tleWithNonzeroBStar_, definitiveStates);

        EXPECT_EQ(residuals.getSize(), definitiveStates.getSize());

        for (Size i = 0; i < residuals.getSize(); ++i)
        {
            const State& residual = residuals[i];
            const State& reference = definitiveStates[i];

            EXPECT_EQ(residual.getInstant(), reference.getInstant());

            const SGP4 sgp4(tleWithNonzeroBStar_);
            const State propagatedState = sgp4.calculateStateAt(reference.getInstant());

            const Vector3d expectedPositionDelta =
                reference.getPosition().getCoordinates() - propagatedState.getPosition().getCoordinates();
            const Vector3d expectedVelocityDelta =
                reference.getVelocity().getCoordinates() - propagatedState.getVelocity().getCoordinates();

            EXPECT_TRUE(residual.getPosition().getCoordinates().isApprox(expectedPositionDelta, 1e-10));
            EXPECT_TRUE(residual.getVelocity().getCoordinates().isApprox(expectedVelocityDelta, 1e-10));
        }
    }

    {
        // Test with undefined TLE
        EXPECT_THROW(
            TLESolver::computeTLEResiduals(TLE::Undefined(), Array<State>::Empty()),
            ostk::core::error::runtime::Undefined
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, EstimateTLE_ReuseInitialGuess)
{
    {
        // First estimation
        Array<State> definitiveStates;
        Array<State> predictiveStates;
        GenerateReferenceStates(Duration::Hours(12.0), definitiveStates, predictiveStates);

        const auto analysis =
            tleSolverWithBStar_.estimateTLE(std::make_pair(cartesianState_, testBStar_), definitiveStates);

        // Second estimation using first result as initial guess
        const auto analysisRerun = tleSolverWithBStar_.estimateTLE(analysis.getDeterminedTLE(), definitiveStates);

        EXPECT_EQ(analysisRerun.getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysisRerun.getRmsError(), 510.0);

        // Compare TLEs from both runs
        const TLE& firstTLE = analysis.getDeterminedTLE();
        const TLE& secondTLE = analysisRerun.getDeterminedTLE();

        EXPECT_EQ(firstTLE.getEpoch(), secondTLE.getEpoch());

        const State firstTLEState = tleSolverWithBStar_.TLEToTLEState(firstTLE);
        const State secondTLEState = tleSolverWithBStar_.TLEToTLEState(secondTLE);

        const VectorXd firstCoordinates = firstTLEState.getCoordinates();
        const VectorXd secondCoordinates = secondTLEState.getCoordinates();

        for (Size i = 0; i < firstCoordinates.size(); ++i)
        {
            EXPECT_NEAR(firstCoordinates[i], secondCoordinates[i], 1e-3);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, EstimateTLE_DifferentFitSpans)
{
    struct TestCase
    {
        Duration fitSpanDuration;
        Duration predictDuration;
        Real expectedRmsError;
        Real expectedPositionDelta;
        Real expectedVelocityDelta;
    };

    const Array<TestCase> testCases = {
        {Duration::Hours(12.0), Duration::Hours(8.5), 510.0, 1100.0, 1.5},
        {Duration::Hours(48.0), Duration::Hours(52.0), 680.0, 1400.0, 1.8},
        {Duration::Hours(72.0), Duration::Hours(60.0), 680.0, 1400.0, 1.8},
        {Duration::Hours(96.0), Duration::Hours(74.0), 800.0, 1800.0, 2.5}
    };

    for (const auto& testCase : testCases)
    {
        Array<State> definitiveStates;
        Array<State> predictiveStates;
        GenerateReferenceStates(testCase.fitSpanDuration, definitiveStates, predictiveStates);

        const auto analysis =
            tleSolverWithBStar_.estimateTLE(std::make_pair(cartesianState_, testBStar_), definitiveStates);

        EXPECT_EQ(analysis.getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysis.getRmsError(), testCase.expectedRmsError);

        const TLE estimatedTLE = analysis.getDeterminedTLE();
        const SGP4 sgp4(estimatedTLE);

        // Check definitive span accuracy
        for (const auto& refState : definitiveStates)
        {
            const State propagatedState = sgp4.calculateStateAt(refState.getInstant());
            const Vector3d positionDelta =
                propagatedState.getPosition().getCoordinates() - refState.getPosition().getCoordinates();
            const Vector3d velocityDelta =
                propagatedState.getVelocity().getCoordinates() - refState.getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), testCase.expectedPositionDelta);
            EXPECT_LT(velocityDelta.norm(), testCase.expectedVelocityDelta);
        }

        // Check predictive span accuracy
        for (const auto& refState : predictiveStates)
        {
            const State propagatedState = sgp4.calculateStateAt(refState.getInstant());
            const Vector3d positionDelta =
                propagatedState.getPosition().getCoordinates() - refState.getPosition().getCoordinates();
            const Vector3d velocityDelta =
                propagatedState.getVelocity().getCoordinates() - refState.getVelocity().getCoordinates();

            EXPECT_LT(positionDelta.norm(), 10000.0);  // Looser bounds for predictive span
            EXPECT_LT(velocityDelta.norm(), 12.0);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimation_TLESolver, Analysis)
{
    {
        const Real rmsError = 1.0;
        const Size iterationCount = 5;
        const String terminationCriteria = "Test Criteria";
        const MatrixXd covariance = MatrixXd::Identity(7, 7);
        const Array<LeastSquaresSolver::Step> steps = {
            LeastSquaresSolver::Step(2.0, VectorXd::Ones(7)), LeastSquaresSolver::Step(1.0, VectorXd::Ones(7))
        };

        const TLESolver::Analysis analysis(
            tleWithNonzeroBStar_, covariance, iterationCount, rmsError, terminationCriteria, steps
        );

        EXPECT_EQ(analysis.getRmsError(), rmsError);
        EXPECT_EQ(analysis.getIterationCount(), iterationCount);
        EXPECT_EQ(analysis.getTerminationCriteria(), terminationCriteria);
        EXPECT_EQ(analysis.getDeterminedTLE(), tleWithNonzeroBStar_);
        EXPECT_TRUE(analysis.getDeterminedTLECovariance().isApprox(covariance));
        EXPECT_EQ(analysis.getSteps().getSize(), steps.getSize());

        testing::internal::CaptureStdout();
        analysis.print(std::cout);
        const std::string output = testing::internal::GetCapturedStdout();
        EXPECT_TRUE(output.find("Analysis") != std::string::npos);
        EXPECT_TRUE(output.find("RMS Error") != std::string::npos);
        EXPECT_TRUE(output.find("Determined TLE") != std::string::npos);
    }
}
