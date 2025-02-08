/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Directory.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::filesystem::Directory;
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
using ostk::physics::environment::Object;
using ostk::physics::environment::object::Celestial;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::estimator::OrbitDeterminationSolver;
using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver_Analysis : public ::testing::Test
{
   protected:
    const Real rmsError_ = 1.0;
    const Size iterationCount_ = 5;
    const String terminationCriteria_ = "Test Criteria";
    const State estimatedState_ = State(
        Instant::J2000(),
        Position::Meters({7.0e6, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({7.5e3, 0.0, 0.0}, Frame::GCRF())
    );
    const MatrixXd estimatedCovariance_ = MatrixXd::Identity(6, 6);
    const MatrixXd estimatedFrisbeeCovariance_ = MatrixXd::Identity(6, 6);
    const Array<LeastSquaresSolver::Step> steps_ = {
        LeastSquaresSolver::Step(2.0, VectorXd::Ones(6)), LeastSquaresSolver::Step(1.0, VectorXd::Ones(6))
    };
    const Array<State> computedObservationStates_ = {State(
        Instant::J2000(),
        Position::Meters({7.0e6, 0.0, 0.0}, Frame::GCRF()),
        Velocity::MetersPerSecond({7.5e3, 0.0, 0.0}, Frame::GCRF())
    )};

    const LeastSquaresSolver::Analysis leastSquaresAnalysis_ = {
        terminationCriteria_,
        estimatedState_,
        estimatedCovariance_,
        estimatedFrisbeeCovariance_,
        computedObservationStates_,
        steps_,
    };

    const OrbitDeterminationSolver::Analysis analysis_ = {estimatedState_, leastSquaresAnalysis_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver_Analysis, Accessors)
{
    {
        EXPECT_EQ(analysis_.estimatedState, estimatedState_);
        EXPECT_EQ(analysis_.solverAnalysis.terminationCriteria, terminationCriteria_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver_Analysis, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << analysis_ << std::endl);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver_Analysis, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << analysis_ << std::endl);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}

class OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Table observationData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/gnss_data.csv")),
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

            observationStates_.add(State(instant, position, velocity));
        }

        positionSigmas_ << 1.0e1, 1.0e1, 1.0e1;
        velocitySigmas_ << 1.0e-2, 1.0e-2, 1.0e-2;

        initialGuessSigmas_ = {
            {*CartesianPosition::Default(), positionSigmas_},
            {*CartesianVelocity::Default(), velocitySigmas_},
        };
        observationSigmas_ = {
            {*CartesianPosition::Default(), positionSigmas_},
            {*CartesianVelocity::Default(), velocitySigmas_},
        };
    }

    const Earth earth_ = Earth::EGM96(10, 10);

    Environment environment_ = Environment(std::make_shared<Earth>(earth_), {});
    NumericalSolver numericalSolver_ = NumericalSolver::Default();
    LeastSquaresSolver leastSquaresSolver_ = LeastSquaresSolver::Default();
    const Shared<const Frame> estimationFrame_ = Frame::GCRF();
    const OrbitDeterminationSolver odSolver_ = {environment_, numericalSolver_, leastSquaresSolver_, estimationFrame_};

    Array<State> observationStates_ = Array<State>::Empty();

    VectorXd positionSigmas_ {3};
    VectorXd velocitySigmas_ {3};

    std::unordered_map<CoordinateSubset, VectorXd> initialGuessSigmas_ = {};
    std::unordered_map<CoordinateSubset, VectorXd> observationSigmas_ = {};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver, Constructor)
{
    {
        EXPECT_NO_THROW(OrbitDeterminationSolver(environment_, numericalSolver_, leastSquaresSolver_, estimationFrame_)
        );
    }

    {
        EXPECT_NO_THROW(OrbitDeterminationSolver(environment_, numericalSolver_, leastSquaresSolver_));
    }

    {
        EXPECT_NO_THROW(OrbitDeterminationSolver(environment_, numericalSolver_));
    }

    {
        OrbitDeterminationSolver odSolver(environment_);  // Remove shadowed declaration warning
    }

    {
        EXPECT_NO_THROW(OrbitDeterminationSolver());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver, Accessors)
{
    {
        const OrbitDeterminationSolver odSolver(environment_, numericalSolver_);

        EXPECT_EQ(odSolver.accessEnvironment().accessCentralCelestialObject()->getName(), "Earth");
        EXPECT_TRUE(odSolver.accessPropagator().isDefined());
        EXPECT_NO_THROW(odSolver.accessSolver());
        EXPECT_EQ(odSolver.accessEstimationFrame()->getName(), "GCRF");
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver, Estimate)
{
    // Test basic state estimator
    {
        const OrbitDeterminationSolver::Analysis analysis =
            odSolver_.estimate(observationStates_[0], observationStates_);

        EXPECT_EQ(*estimationFrame_, *analysis.estimatedState.accessFrame());

        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(analysis.solverAnalysis.rmsError, 2.0);
    }

    // Test with estimator coordinate subsets
    {
        Array<Shared<const CoordinateSubset>> estimationSubsets = {CartesianPosition::Default()};

        const State initialGuessState = observationStates_[0];
        const OrbitDeterminationSolver::Analysis analysis =
            odSolver_.estimate(initialGuessState, observationStates_, estimationSubsets);

        EXPECT_EQ(*estimationFrame_, *analysis.estimatedState.accessFrame());
        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(
            analysis.solverAnalysis.rmsError, 18.0
        );  // Extra error because only position is estimated in this case
    }

    // Test with sigmas
    {
        const OrbitDeterminationSolver::Analysis analysis =
            odSolver_.estimate(observationStates_[0], observationStates_, {}, initialGuessSigmas_, observationSigmas_);

        EXPECT_EQ(*estimationFrame_, *analysis.estimatedState.accessFrame());
        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(analysis.solverAnalysis.rmsError, 5.0);
    }

    // Test with states in different frames
    {
        const Shared<const Frame> estimationFrame = Frame::ITRF();
        const OrbitDeterminationSolver odSolver = {
            environment_, numericalSolver_, leastSquaresSolver_, estimationFrame
        };

        const Array<State> observationStatesInTEME = observationStates_.map<State>(
            [](const State& aState) -> State
            {
                return aState.inFrame(Frame::TEME());
            }
        );

        const State initialGuessStateInGCRF = observationStates_[0].inFrame(Frame::GCRF());

        const OrbitDeterminationSolver::Analysis analysis =
            odSolver.estimate(initialGuessStateInGCRF, observationStatesInTEME);

        EXPECT_EQ(*estimationFrame, *analysis.estimatedState.accessFrame());
        EXPECT_EQ(analysis.solverAnalysis.terminationCriteria, "RMS Update Threshold");
        EXPECT_LT(analysis.solverAnalysis.rmsError, 2.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver, Estimate_Failures)
{
    // Test invalid estimator subsets
    {
        Array<Shared<const CoordinateSubset>> invalidSubsets = {CoordinateSubset::Mass()};

        EXPECT_THROW(
            odSolver_.estimate(observationStates_[0], observationStates_, invalidSubsets),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_OrbitDeterminationSolver, EstimateOrbit)
{
    {
        // Test basic orbit estimator
        const Orbit orbit = odSolver_.estimateOrbit(observationStates_[0], observationStates_);

        EXPECT_TRUE(orbit.isDefined());

        // Check propagated state matches initial condition
        const State orbitState = orbit.getStateAt(observationStates_[0].getInstant());
        EXPECT_TRUE(orbitState.isDefined());
    }

    {
        // Test orbit estimator with estimator subsets and sigmas
        Array<Shared<const CoordinateSubset>> estimationSubsets = {
            CartesianPosition::Default(), CartesianVelocity::Default()
        };

        const Orbit orbit = odSolver_.estimateOrbit(
            observationStates_[0], observationStates_, estimationSubsets, initialGuessSigmas_, observationSigmas_
        );

        EXPECT_TRUE(orbit.isDefined());
    }
}
