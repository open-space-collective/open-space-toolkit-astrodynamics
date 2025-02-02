/// Apache License 2.0

#include <random>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/LeastSquaresSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::solver::FiniteDifferenceSolver;
using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Step : public ::testing::Test
{
   protected:
    const VectorXd xHat_ = VectorXd::Ones(6);
    const Real rmsError_ = 1.0;
    const LeastSquaresSolver::Step step_ = LeastSquaresSolver::Step(rmsError_, xHat_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Step, Constructor)
{
    {
        EXPECT_NO_THROW(LeastSquaresSolver::Step(rmsError_, xHat_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Step, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << step_ << std::endl);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Step, Print)
{
    {
        testing::internal::CaptureStdout();

        step_.print(std::cout);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}

class OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis : public ::testing::Test
{
   protected:
    const Real rmsError_ = 1.0;
    const String terminationCriteria_ = "Test Criteria";
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

    const LeastSquaresSolver::Analysis analysis_ = LeastSquaresSolver::Analysis(
        rmsError_,
        terminationCriteria_,
        estimatedState_,
        estimatedCovariance_,
        estimatedFrisbeeCovariance_,
        computedObservationStates_,
        steps_
    );
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, Constructor)
{
    {
        EXPECT_EQ(analysis_.rmsError, rmsError_);
        EXPECT_EQ(analysis_.observationCount, computedObservationStates_.getSize());
        EXPECT_EQ(analysis_.terminationCriteria, terminationCriteria_);
        EXPECT_EQ(analysis_.estimatedState, estimatedState_);
        EXPECT_EQ(analysis_.estimatedCovariance, estimatedCovariance_);
        EXPECT_EQ(analysis_.estimatedFrisbeeCovariance, estimatedFrisbeeCovariance_);
        EXPECT_EQ(analysis_.computedObservationStates, computedObservationStates_);
        EXPECT_EQ(analysis_.steps.getSize(), steps_.getSize());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << analysis_ << std::endl);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, Print)
{
    {
        testing::internal::CaptureStdout();
        analysis_.print(std::cout);
        const std::string output = testing::internal::GetCapturedStdout();
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, computeResidualStates)
{
    // Test incorrect size
    {
        EXPECT_THROW(analysis_.computeResidualStates({}), ostk::core::error::RuntimeError);
    }

    {
        const Array<State> observationStates = {State(
            Instant::J2000(),
            Position::Meters({7.0e6, 0.0, 0.0}, Frame::GCRF()),
            Velocity::MetersPerSecond({7.5e3, 0.0, 0.0}, Frame::GCRF())
        )};
        const Array<State> residualStates = analysis_.computeResidualStates(observationStates);

        EXPECT_EQ(residualStates.getSize(), computedObservationStates_.getSize());
        EXPECT_EQ(residualStates.getSize(), observationStates.getSize());

        for (Size i = 0; i < residualStates.getSize(); ++i)
        {
            EXPECT_EQ(residualStates[i], computedObservationStates_[i] - observationStates[i]);
        }
    }
}

class OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        std::mt19937 rng(42);  // Fixed seed for reproducibility
        // Observations will be perturbed with Gaussian noise with standard deviation 0.01
        std::normal_distribution<double> distObservations(0.0, 0.01);

        // Define true initial state (position and velocity)
        const Vector3d trueInitialPositionCoordinates = {1.0, 0.0, 0.0};
        const Vector3d trueInitialVelocityCoordinates = {0.0, 0.0, 0.0};
        trueState_ = {
            Instant::J2000(),
            Position::Meters(trueInitialPositionCoordinates, Frame::GCRF()),
            Velocity::MetersPerSecond(trueInitialVelocityCoordinates, Frame::GCRF())
        };

        // Define very bad initial guess, to make sure the solver still converges (this problem is linear so it will)
        // converge to very close to the true state
        const Vector3d noiseInitialGuessPositionCoordinates = {10.0, 0.0, 0.0};
        const Vector3d noiseInitialGuessVelocityCoordinates = {5.0, 0.0, 0.0};
        initialGuessState_ =
            trueState_ +
            State(
                trueState_.getInstant(),
                Position::Meters(trueInitialPositionCoordinates + noiseInitialGuessPositionCoordinates, Frame::GCRF()),
                Velocity::MetersPerSecond(
                    trueInitialVelocityCoordinates + noiseInitialGuessVelocityCoordinates, Frame::GCRF()
                )
            );

        // Define observations (true states with noise)
        const Size observationCount = 100;
        const Real timeStep = 0.1;  // seconds

        for (Size i = 0; i < observationCount; ++i)
        {
            const Real time = timeStep * static_cast<Real>(i);
            const Real position = std::cos(time);
            const Real velocity = -std::sin(time);

            const Vector3d truePositionCoordinates = {position, 0.0, 0.0};
            const Vector3d trueVelocityCoordinates = {velocity, 0.0, 0.0};

            // Add noise to the true state
            const Vector3d noisePositionCoordinates = {distObservations(rng), 0.0, 0.0};
            const Vector3d noiseVelocityCoordinates = {distObservations(rng), 0.0, 0.0};

            const Position combinedPosition =
                Position::Meters(truePositionCoordinates + noisePositionCoordinates, Frame::GCRF());
            const Velocity combinedVelocity =
                Velocity::MetersPerSecond(trueVelocityCoordinates + noiseVelocityCoordinates, Frame::GCRF());

            observationStates_.add(State(Instant::J2000() + Duration::Seconds(time), combinedPosition, combinedVelocity)
            );
        }

        // Initial guess sigmas are set artifically small (much smaller than the actual deviations in the initial guess)
        // to test the apriori weighting
        initialGuessPositionSigmas_ << 0.01, 1.0e-5, 1.0e-5;  // Standard deviation of 0.01 m for position
        initialGuessVelocitySigmas_ << 0.01, 1.0e-5, 1.0e-5;  // Standard deviation of 0.01 m/s for velocity

        initialGuessSigmas_ = {
            {*CartesianPosition::Default(), initialGuessPositionSigmas_},
            {*CartesianVelocity::Default(), initialGuessVelocitySigmas_},
        };

        // Observation sigmas are set to the standard deviation of the noise added to the true states
        observationPositionSigmas_ << 0.01, 1.0e-5, 1.0e-5;  // Standard deviation of 0.01 m for position
        observationVelocitySigmas_ << 0.01, 1.0e-5, 1.0e-5;  // Standard deviation of 0.01 m/s for velocity

        observationSigmas_ = {
            {*CartesianPosition::Default(), observationPositionSigmas_},
            {*CartesianVelocity::Default(), observationVelocitySigmas_},
        };
    }

    State trueState_ = State::Undefined();
    State initialGuessState_ = State::Undefined();

    VectorXd initialGuessPositionSigmas_ = VectorXd(3);
    VectorXd initialGuessVelocitySigmas_ = VectorXd(3);
    VectorXd observationPositionSigmas_ = VectorXd(3);
    VectorXd observationVelocitySigmas_ = VectorXd(3);
    std::unordered_map<CoordinateSubset, VectorXd> initialGuessSigmas_;
    std::unordered_map<CoordinateSubset, VectorXd> observationSigmas_;

    Array<State> observationStates_;
    const Size maxIterationCount_ = 20;
    const Real rmsUpdateThreshold_ = 1e-2;
    const FiniteDifferenceSolver finiteDifferenceSolver_ = FiniteDifferenceSolver::Default();
    const LeastSquaresSolver solver_ = {maxIterationCount_, rmsUpdateThreshold_, finiteDifferenceSolver_};

    // Define state generator for harmonic oscillator
    static constexpr auto generateStates_ = [](const State& state, const Array<Instant>& instants) -> Array<State>
    {
        Array<State> states;
        states.reserve(instants.getSize());

        const Real initialPosition = state.getPosition().accessCoordinates().x();
        const Real initialVelocity = state.getVelocity().accessCoordinates().x();

        for (const auto& instant : instants)
        {
            const Real time = (instant - state.getInstant()).inSeconds();
            const Real position = initialPosition * std::cos(time) + initialVelocity * std::sin(time);
            const Real velocity = -initialPosition * std::sin(time) + initialVelocity * std::cos(time);

            const Position newPosition = Position::Meters({position, 0.0, 0.0}, Frame::GCRF());
            const Velocity newVelocity = Velocity::MetersPerSecond({velocity, 0.0, 0.0}, Frame::GCRF());

            states.add(State(instant, newPosition, newVelocity));
        }

        return states;
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Constructor)
{
    {
        EXPECT_NO_THROW(LeastSquaresSolver::Default());
        EXPECT_NO_THROW(LeastSquaresSolver(maxIterationCount_, rmsUpdateThreshold_));
        EXPECT_NO_THROW(LeastSquaresSolver(maxIterationCount_, rmsUpdateThreshold_, finiteDifferenceSolver_));
    }

    {
        EXPECT_THROW(LeastSquaresSolver(0, 1.0), ostk::core::error::RuntimeError);
        EXPECT_THROW(LeastSquaresSolver(20, 0.0), ostk::core::error::RuntimeError);
        EXPECT_THROW(LeastSquaresSolver(20, -1.0), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Getters)
{
    {
        EXPECT_EQ(solver_.getMaxIterationCount(), maxIterationCount_);
        EXPECT_EQ(solver_.getRmsUpdateThreshold(), rmsUpdateThreshold_);
        EXPECT_NO_THROW(solver_.getFiniteDifferenceSolver());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Solve_Success)
{
    // Test case 1: a priori is ignored and all observations weighted the same
    {
        const LeastSquaresSolver::Analysis analysis =
            solver_.solve(initialGuessState_, observationStates_, generateStates_, {}, {});

        EXPECT_EQ(analysis.observationCount, observationStates_.getSize());
        EXPECT_LT(analysis.iterationCount, solver_.getMaxIterationCount());
        EXPECT_EQ(analysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_EQ(analysis.computedObservationStates.getSize(), observationStates_.getSize());
        EXPECT_EQ(analysis.estimatedCovariance.rows(), 6);
        EXPECT_EQ(analysis.estimatedCovariance.cols(), 6);
        EXPECT_EQ(analysis.estimatedFrisbeeCovariance.rows(), 6);
        EXPECT_EQ(analysis.estimatedFrisbeeCovariance.cols(), 6);

        const VectorXd estimatedPosition = analysis.estimatedState.getPosition().getCoordinates();
        const VectorXd estimatedVelocity = analysis.estimatedState.getVelocity().getCoordinates();
        const VectorXd truePosition = trueState_.getPosition().getCoordinates();
        const VectorXd trueVelocity = trueState_.getVelocity().getCoordinates();

        EXPECT_LT(analysis.rmsError, 0.014);
        EXPECT_LT((estimatedPosition - truePosition).norm(), 0.004);
        EXPECT_LT((estimatedVelocity - trueVelocity).norm(), 0.002);
    }

    // Test case 2:  apriori is ignored and observations are weighted
    // Should produce almost the exact same answer as test case 2, because observations are weighted equally
    {
        const LeastSquaresSolver::Analysis analysis =
            solver_.solve(initialGuessState_, observationStates_, generateStates_, {}, observationSigmas_);

        EXPECT_EQ(analysis.observationCount, observationStates_.getSize());
        EXPECT_LT(analysis.iterationCount, solver_.getMaxIterationCount());
        EXPECT_EQ(analysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_EQ(analysis.computedObservationStates.getSize(), observationStates_.getSize());
        EXPECT_EQ(analysis.estimatedCovariance.rows(), 6);
        EXPECT_EQ(analysis.estimatedCovariance.cols(), 6);
        EXPECT_EQ(analysis.estimatedFrisbeeCovariance.rows(), 6);
        EXPECT_EQ(analysis.estimatedFrisbeeCovariance.cols(), 6);

        const VectorXd estimatedPosition = analysis.estimatedState.getPosition().getCoordinates();
        const VectorXd estimatedVelocity = analysis.estimatedState.getVelocity().getCoordinates();
        const VectorXd truePosition = trueState_.getPosition().getCoordinates();
        const VectorXd trueVelocity = trueState_.getVelocity().getCoordinates();

        EXPECT_LT(analysis.rmsError, 0.014);
        EXPECT_LT((estimatedPosition - truePosition).norm(), 0.004);
        EXPECT_LT((estimatedVelocity - trueVelocity).norm(), 0.002);
    }

    // Test case 3: apriori is used and weighted the same as observations
    // Estimate should be closer to initial guess than observations, due to apriori weighting
    {
        const LeastSquaresSolver::Analysis analysis = solver_.solve(
            initialGuessState_, observationStates_, generateStates_, initialGuessSigmas_, observationSigmas_
        );

        EXPECT_EQ(analysis.observationCount, observationStates_.getSize());
        EXPECT_LT(analysis.iterationCount, solver_.getMaxIterationCount());
        EXPECT_EQ(analysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_EQ(analysis.computedObservationStates.getSize(), observationStates_.getSize());
        EXPECT_EQ(analysis.estimatedCovariance.rows(), 6);
        EXPECT_EQ(analysis.estimatedCovariance.cols(), 6);
        EXPECT_EQ(analysis.estimatedFrisbeeCovariance.rows(), 6);
        EXPECT_EQ(analysis.estimatedFrisbeeCovariance.cols(), 6);

        const VectorXd estimatedPosition = analysis.estimatedState.getPosition().getCoordinates();
        const VectorXd estimatedVelocity = analysis.estimatedState.getVelocity().getCoordinates();
        const VectorXd truePosition = trueState_.getPosition().getCoordinates();
        const VectorXd trueVelocity = trueState_.getVelocity().getCoordinates();

        EXPECT_LT(analysis.rmsError, 0.13);
        EXPECT_LT((estimatedPosition - truePosition).norm(), 0.11);
        EXPECT_LT((estimatedVelocity - trueVelocity).norm(), 0.05);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Solve_Failures)
{
    // Test undefined initial guess state
    {
        EXPECT_THROW(
            solver_.solve(State::Undefined(), observationStates_, generateStates_),
            ostk::core::error::runtime::Undefined
        );
    }

    // Test empty observations
    {
        EXPECT_THROW(
            solver_.solve(initialGuessState_, Array<State>::Empty(), generateStates_),
            ostk::core::error::runtime::Undefined
        );
    }

    // Test non matching frames
    {
        EXPECT_THROW(
            solver_.solve(initialGuessState_.inFrame(Frame::ITRF()), observationStates_, generateStates_),
            ostk::core::error::RuntimeError
        );
    }

    // Test mismatched observations
    {
        Array<State> observations = observationStates_;
        VectorXd coordinates(3);
        coordinates << 0.1, 0.0, 0.0;
        observations.add(State(Instant::J2000(), coordinates, Frame::GCRF(), {CartesianPosition::Default()}));

        EXPECT_THROW(solver_.solve(initialGuessState_, observations, generateStates_), ostk::core::error::RuntimeError);
    }

    // Test not enough observations
    {
        EXPECT_THROW(
            solver_.solve(initialGuessState_, {observationStates_[0]}, generateStates_), ostk::core::error::RuntimeError
        );
    }

    // Test mismatched initial guess sigmas
    {
        std::unordered_map<CoordinateSubset, VectorXd> invalidSigmas = {
            {*CartesianPosition::Default(), initialGuessPositionSigmas_}  // Missing velocity sigmas
        };

        EXPECT_THROW(
            solver_.solve(initialGuessState_, observationStates_, generateStates_, invalidSigmas, observationSigmas_),
            ostk::core::error::RuntimeError
        );
    }

    // Test invalid sigma values
    {
        VectorXd invalidSigmaValues(3);
        invalidSigmaValues << -1.0, 1.0, 0.0;
        std::unordered_map<CoordinateSubset, VectorXd> invalidSigmas = {
            {*CartesianPosition::Default(), invalidSigmaValues},
            {*CartesianVelocity::Default(), initialGuessVelocitySigmas_}
        };

        EXPECT_THROW(
            solver_.solve(initialGuessState_, observationStates_, generateStates_, invalidSigmas, observationSigmas_),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Default)
{
    {
        EXPECT_NO_THROW(LeastSquaresSolver::Default());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, CalculateEmpiricalCovariance)
{
    {
        const LeastSquaresSolver::Analysis analysis =
            solver_.solve(initialGuessState_, observationStates_, generateStates_, {}, {});

        const State estimatedState = analysis.estimatedState;

        const Array<Instant> observationInstants = observationStates_.map<Instant>(
            [](const State& state) -> Instant
            {
                return state.getInstant();
            }
        );

        const Array<State> estimatedStates = generateStates_(estimatedState, observationInstants);

        Array<State> residuals = Array<State>::Empty();
        residuals.reserve(estimatedStates.getSize());

        for (Size i = 0; i < observationStates_.getSize(); ++i)
        {
            residuals.add(estimatedStates[i] - observationStates_[i]);
        }

        const MatrixXd covariance = LeastSquaresSolver::calculateEmpiricalCovariance(residuals);

        EXPECT_EQ(covariance.rows(), 6);
        EXPECT_EQ(covariance.cols(), 6);
        EXPECT_TRUE(covariance.isApprox(covariance.transpose(), 1e-10));

        // Check that position variances are close to observation sigmas
        for (Size i = 0; i < 3; ++i)
        {
            EXPECT_NEAR(std::sqrt(covariance(i, i)), observationSigmas_.at(*CartesianPosition::Default())[0], 1e-1);
        }

        // Check that velocity variances are close to observation sigmas
        for (Size i = 3; i < 6; ++i)
        {
            EXPECT_NEAR(std::sqrt(covariance(i, i)), observationSigmas_.at(*CartesianVelocity::Default())[0], 1e-1);
        }
    }

    {
        EXPECT_THROW(
            LeastSquaresSolver::calculateEmpiricalCovariance(Array<State>::Empty()),
            ostk::core::error::runtime::Undefined
        );
    }
}
