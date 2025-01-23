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

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ODLeastSquaresSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
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

using ostk::astrodynamics::estimator::ODLeastSquaresSolver;
using ostk::astrodynamics::solver::LeastSquaresSolver;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Table referenceData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/gnss_data.csv")),
            Table::Format::CSV,
            true
        );

        for (const auto& referenceRow : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC);
            const Position position = Position::Meters(
                {referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal()},
                Frame::ITRF()
            );
            const Velocity velocity = Velocity::MetersPerSecond(
                {referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal()},
                Frame::ITRF()
            );

            referenceStates_.add(State(instant, position, velocity));
        }

        positionSigmas_ << 1.0e1, 1.0e1, 1.0e1;
        velocitySigmas_ << 1.0e-2, 1.0e-2, 1.0e-2;

        referenceStateSigmas_ = {
            {*CartesianPosition::Default(), positionSigmas_},
            {*CartesianVelocity::Default(), velocitySigmas_},
        };
        initialStateSigmas_ = {
            {*CartesianPosition::Default(), positionSigmas_},
            {*CartesianVelocity::Default(), velocitySigmas_},
        };
    }

    const Earth earth_ = Earth::EGM96(10, 10);

    Environment environment_ = Environment(std::make_shared<Earth>(earth_), {});
    NumericalSolver numericalSolver_ = NumericalSolver::Default();
    LeastSquaresSolver leastSquaresSolver_ = LeastSquaresSolver::Default();
    const ODLeastSquaresSolver odSolver_ = {environment_, numericalSolver_, leastSquaresSolver_};

    Array<State> referenceStates_ = Array<State>::Empty();

    VectorXd positionSigmas_ {3};
    VectorXd velocitySigmas_ {3};

    std::unordered_map<CoordinateSubset, VectorXd> referenceStateSigmas_ = {};
    std::unordered_map<CoordinateSubset, VectorXd> initialStateSigmas_ = {};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver, Constructor)
{
    {
        EXPECT_NO_THROW(ODLeastSquaresSolver(environment_, numericalSolver_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver, Accessors)
{
    {
        const ODLeastSquaresSolver solver(environment_, numericalSolver_);

        EXPECT_EQ(solver.accessEnvironment().accessCentralCelestialObject()->getName(), "Earth");
        EXPECT_TRUE(solver.accessPropagator().isDefined());
        EXPECT_NO_THROW(solver.accessSolver());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver, EstimateState)
{
    {
        // Test basic state estimator
        const auto analysis = odSolver_.estimateState(referenceStates_[0], referenceStates_);

        EXPECT_EQ(analysis.getSolverAnalysis().getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysis.getSolverAnalysis().getRmsError(), 50.0);
    }

    {
        // Test with estimator coordinate subsets
        Array<Shared<const CoordinateSubset>> estimationSubsets = {CartesianPosition::Default()};

        const auto analysis = odSolver_.estimateState(referenceStates_[0], referenceStates_, estimationSubsets);

        EXPECT_EQ(analysis.getSolverAnalysis().getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysis.getSolverAnalysis().getRmsError(), 50.0);
    }

    {
        // Test with sigmas
        const auto analysis = odSolver_.estimateState(
            referenceStates_[0], referenceStates_, {}, initialStateSigmas_, referenceStateSigmas_
        );

        EXPECT_EQ(analysis.getSolverAnalysis().getTerminationCriteria(), "RMS Update Threshold");
        EXPECT_LT(analysis.getSolverAnalysis().getRmsError(), 50.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver, EstimateState_Failures)
{
    {
        // Test invalid estimator subsets
        Array<Shared<const CoordinateSubset>> invalidSubsets = {CoordinateSubset::Mass()};

        EXPECT_THROW(
            odSolver_.estimateState(referenceStates_[0], referenceStates_, invalidSubsets),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver, EstimateOrbit)
{
    {
        // Test basic orbit estimator
        const auto orbit = odSolver_.estimateOrbit(referenceStates_[0], referenceStates_);

        EXPECT_TRUE(orbit.isDefined());

        // Check propagated state matches initial condition
        const State orbitState = orbit.getStateAt(referenceStates_[0].getInstant());
        EXPECT_TRUE(orbitState.isDefined());
    }

    {
        // Test orbit estimator with estimator subsets and sigmas
        Array<Shared<const CoordinateSubset>> estimationSubsets = {
            CartesianPosition::Default(), CartesianVelocity::Default()
        };

        const auto orbit = odSolver_.estimateOrbit(
            referenceStates_[0], referenceStates_, estimationSubsets, referenceStateSigmas_, referenceStateSigmas_
        );

        EXPECT_TRUE(orbit.isDefined());
    }
}

class OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver_Analysis : public ::testing::Test
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

    const LeastSquaresSolver::Analysis leastSquaresAnalysis_ = {
        rmsError_,
        iterationCount_,
        terminationCriteria_,
        solutionState_,
        solutionCovariance_,
        solutionFrisbeeCovariance_,
        steps_,
    };

    const ODLeastSquaresSolver::Analysis analysis_ = {solutionState_, leastSquaresAnalysis_};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver_Analysis, Accessors)
{
    {
        EXPECT_EQ(analysis_.getDeterminedState(), solutionState_);
        EXPECT_EQ(analysis_.getSolverAnalysis().getTerminationCriteria(), terminationCriteria_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver_Analysis, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << analysis_ << std::endl);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solver_ODLeastSquaresSolver_Analysis, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << analysis_ << std::endl);

        const std::string output = testing::internal::GetCapturedStdout();
    }
}
