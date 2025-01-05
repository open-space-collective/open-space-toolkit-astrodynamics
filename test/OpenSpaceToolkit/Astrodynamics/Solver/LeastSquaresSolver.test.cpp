/// Apache License 2.0

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
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

class OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        // Define initial state (position and velocity)
        const Position initialPosition = Position::Meters({1.0, 0.0, 0.0}, Frame::GCRF());
        const Velocity initialVelocity = Velocity::MetersPerSecond({0.0, 1.0, 0.0}, Frame::GCRF());
        trueState_ = {Instant::J2000(), initialPosition, initialVelocity};

        // Define reference states (true states with noise)
        const Size referenceStateCount = 100;
        const Real timeStep = 0.1;  // seconds

        for (Size i = 0; i < referenceStateCount; ++i)
        {
            const Real time = timeStep * (Real)i;
            const Real position = std::cos(time);
            const Real velocity = -std::sin(time);

            const Position truePosition = Position::Meters({position, 0.0, 0.0}, Frame::GCRF());
            const Velocity trueVelocity = Velocity::MetersPerSecond({velocity, 0.0, 0.0}, Frame::GCRF());

            // Add noise to the true state
            const Position noisePosition =
                Position::Meters({0.01 * ((rand() % 200) - 100) / 100.0, 0.0, 0.0}, Frame::GCRF());
            const Velocity noiseVelocity =
                Velocity::MetersPerSecond({0.01 * ((rand() % 200) - 100) / 100.0, 0.0, 0.0}, Frame::GCRF());

            const Position combinedPosition =
                Position::Meters(truePosition.getCoordinates() + noisePosition.getCoordinates(), Frame::GCRF());
            const Velocity combinedVelocity = Velocity::MetersPerSecond(
                trueVelocity.getCoordinates() + noiseVelocity.getCoordinates(), Frame::GCRF()
            );

            referenceStates_.add(State(Instant::J2000() + Duration::Seconds(time), combinedPosition, combinedVelocity));
        }

        const Position perturbedPosition = Position::Meters({0.1, 0.0, 0.0}, Frame::GCRF());
        const Velocity perturbedVelocity = Velocity::MetersPerSecond({0.0, 0.1, 0.0}, Frame::GCRF());
        initialGuessState_ = trueState_ + State(trueState_.getInstant(), perturbedPosition, perturbedVelocity);
    }

    State trueState_ = State::Undefined();
    State initialGuessState_ = State::Undefined();
    std::unordered_map<CoordinateSubset, Real> referenceStateSigmas_ = {
        {*CartesianPosition::Default(), 0.01}, {*CartesianVelocity::Default(), 0.01}
    };
    std::unordered_map<CoordinateSubset, Real> initialStateSigmas_ = {
        {*CartesianPosition::Default(), 0.1}, {*CartesianVelocity::Default(), 0.1}
    };

    Array<State> referenceStates_;
    const Size maxIterationCount_ = 20;
    const Real rmsUpdateThreshold_ = 1e-2;
    const LeastSquaresSolver solver_ = {maxIterationCount_, rmsUpdateThreshold_};

    // Define generate states callback for harmonic oscillator
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

class OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Step : public ::testing::Test
{
   protected:
    const VectorXd xHat_ = VectorXd::Ones(6);
    const Real rmsError_ = 1.0;
    const LeastSquaresSolver::Step step_ = LeastSquaresSolver::Step(rmsError_, xHat_);
};

class OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis : public ::testing::Test
{
   protected:
    const Real rmsError_ = 1.0;
    const Size iterationCount_ = 5;
    const String terminationCriteria_ = "Test Criteria";
    const State solutionState_ = State(
        Instant::J2000(),
        Position::Meters({7.0e6, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({7.5e3, 0.0, 0.0}, Frame::GCRF())
    );
    const MatrixXd solutionCovariance_ = MatrixXd::Identity(6, 6);
    const MatrixXd solutionFrisbeeCovariance_ = MatrixXd::Identity(6, 6);
    const Array<LeastSquaresSolver::Step> steps_ = {
        LeastSquaresSolver::Step(2.0, VectorXd::Ones(6)), LeastSquaresSolver::Step(1.0, VectorXd::Ones(6))
    };

    const LeastSquaresSolver::Analysis analysis_ = LeastSquaresSolver::Analysis(
        rmsError_,
        iterationCount_,
        terminationCriteria_,
        solutionState_,
        solutionCovariance_,
        solutionFrisbeeCovariance_,
        steps_
    );
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Step, Constructor)
{
    {
        EXPECT_NO_THROW(LeastSquaresSolver::Step(rmsError_, xHat_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Step, GetRmsError)
{
    {
        EXPECT_EQ(step_.getRmsError(), rmsError_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Step, GetXHat)
{
    {
        EXPECT_TRUE(step_.getXHat().isApprox(xHat_));
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, Constructor)
{
    {
        EXPECT_NO_THROW(LeastSquaresSolver::Analysis(
            rmsError_,
            iterationCount_,
            terminationCriteria_,
            solutionState_,
            solutionCovariance_,
            solutionFrisbeeCovariance_,
            steps_
        ));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, GetRmsError)
{
    {
        EXPECT_EQ(analysis_.getRmsError(), rmsError_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, GetIterationCount)
{
    {
        EXPECT_EQ(analysis_.getIterationCount(), iterationCount_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, GetTerminationCriteria)
{
    {
        EXPECT_EQ(analysis_.getTerminationCriteria(), terminationCriteria_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, AccessSolutionState)
{
    {
        EXPECT_TRUE(analysis_.accessSolutionState().getCoordinates().isApprox(solutionState_.getCoordinates()));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, AccessSolutionCovariance)
{
    {
        EXPECT_TRUE(analysis_.accessSolutionCovariance().isApprox(solutionCovariance_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, AccessSolutionFrisbeeCovariance)
{
    {
        EXPECT_TRUE(analysis_.accessSolutionFrisbeeCovariance().isApprox(solutionFrisbeeCovariance_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver_Analysis, AccessSteps)
{
    {
        EXPECT_EQ(analysis_.accessSteps().getSize(), steps_.getSize());
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Constructor)
{
    {
        EXPECT_NO_THROW(LeastSquaresSolver::Default());
        EXPECT_NO_THROW(LeastSquaresSolver(maxIterationCount_, rmsUpdateThreshold_));
    }

    {
        EXPECT_THROW(LeastSquaresSolver(0, 1.0), ostk::core::error::RuntimeError);
        EXPECT_THROW(LeastSquaresSolver(20, 0.0), ostk::core::error::RuntimeError);
        EXPECT_THROW(LeastSquaresSolver(20, -1.0), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Accessors)
{
    {
        EXPECT_EQ(solver_.getMaxIterationCount(), maxIterationCount_);
        EXPECT_EQ(solver_.getRmsUpdateThreshold(), rmsUpdateThreshold_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Solve_Success)
{
    // Test case where a priori is ignored and all reference states weighted the same
    {
        const auto analysis = solver_.solve(initialGuessState_, referenceStates_, generateStates_, {}, {});

        // Check results
        EXPECT_EQ(analysis.getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysis.getRmsError(), 0.1);
        EXPECT_LT(analysis.getIterationCount(), solver_.getMaxIterationCount());

        const VectorXd estimatedPosition = analysis.accessSolutionState().getPosition().getCoordinates();
        const VectorXd estimatedVelocity = analysis.accessSolutionState().getVelocity().getCoordinates();

        const VectorXd truePosition = trueState_.getPosition().getCoordinates();
        const VectorXd trueVelocity = trueState_.getVelocity().getCoordinates();

        EXPECT_LT((estimatedPosition - truePosition).norm(), 0.15);
        EXPECT_LT((estimatedVelocity - trueVelocity).norm(), 0.15);
    }

    // Test case with weighted reference states
    {
        const auto analysis = solver_.solve(
            initialGuessState_, referenceStates_, generateStates_, initialStateSigmas_, referenceStateSigmas_
        );

        EXPECT_EQ(analysis.getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysis.getRmsError(), 20.0);
        EXPECT_LT(analysis.getIterationCount(), solver_.getMaxIterationCount());

        const VectorXd estimatedPosition = analysis.accessSolutionState().getPosition().getCoordinates();
        const VectorXd estimatedVelocity = analysis.accessSolutionState().getVelocity().getCoordinates();
        const VectorXd truePosition = trueState_.getPosition().getCoordinates();
        const VectorXd trueVelocity = trueState_.getVelocity().getCoordinates();

        EXPECT_LT((estimatedPosition - truePosition).norm(), 0.15);
        EXPECT_LT((estimatedVelocity - trueVelocity).norm(), 0.15);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, Solve_Failures)
{
    {
        // Test undefined initial guess state
        EXPECT_THROW(
            solver_.solve(State::Undefined(), referenceStates_, generateStates_), ostk::core::error::runtime::Undefined
        );
    }

    {
        // Test empty reference states
        EXPECT_THROW(
            solver_.solve(initialGuessState_, Array<State>::Empty(), generateStates_),
            ostk::core::error::runtime::Undefined
        );
    }

    {
        // Test mismatched initial guess sigmas
        std::unordered_map<CoordinateSubset, Real> invalidSigmas = {
            {*CartesianPosition::Default(), 1.0}  // Missing velocity sigmas
        };

        EXPECT_THROW(
            solver_.solve(initialGuessState_, referenceStates_, generateStates_, invalidSigmas, referenceStateSigmas_),
            ostk::core::error::RuntimeError
        );
    }

    {
        // Test invalid sigma values
        std::unordered_map<CoordinateSubset, Real> invalidSigmas = {
            {*CartesianPosition::Default(), -1.0}, {*CartesianVelocity::Default(), 1.0}
        };

        EXPECT_THROW(
            solver_.solve(initialGuessState_, referenceStates_, generateStates_, invalidSigmas, referenceStateSigmas_),
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

// TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_LeastSquaresSolver, CalculateEmpiricalCovariance)
// {
//     {
//         const Array<State> residuals = LeastSquaresSolver::computeStateResiduals(
//             referenceStates_,
//             [this](const Array<Instant>& instants)
//             {
//                 return generateStates_(this->trueState_, instants);
//             }
//         );

//         const MatrixXd covariance = LeastSquaresSolver::calculateEmpiricalCovariance(residuals);

//         EXPECT_EQ(covariance.rows(), 6);
//         EXPECT_EQ(covariance.cols(), 6);
//         EXPECT_TRUE(covariance.isApprox(covariance.transpose(), 1e-10));

//         // Check that position variances are close to reference sigmas
//         for (Size i = 0; i < 3; ++i)
//         {
//             EXPECT_NEAR(
//                 std::sqrt(covariance(i, i)),
//                 referenceStateSigmas_[CartesianPosition::Default()],
//                 referenceStateSigmas_[CartesianPosition::Default()] * 0.2
//             );
//         }

//         // Check that velocity variances are close to reference sigmas
//         for (Size i = 3; i < 6; ++i)
//         {
//             EXPECT_NEAR(
//                 std::sqrt(covariance(i, i)),
//                 referenceStateSigmas_[CartesianVelocity::Default()],
//                 referenceStateSigmas_[CartesianVelocity::Default()] * 0.2
//             );
//         }
//     }

//     {
//         EXPECT_THROW(
//             LeastSquaresSolver::calculateEmpiricalCovariance(Array<State>::Empty()),
//             ostk::core::error::runtime::Undefined
//         );
//     }
// }
