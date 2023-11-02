/// Apache License 2.0

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Core/Types/Size.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solvers/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

using ostk::core::types::Size;
using ostk::core::types::Real;
using ostk::core::types::Shared;
using ostk::core::ctnr::Array;

using ostk::physics::time::Instant;
using ostk::physics::time::Duration;
using ostk::physics::coord::Frame;

using ostk::math::obj::VectorXd;
using ostk::math::obj::MatrixXd;

using ostk::astro::solvers::FiniteDifferenceSolver;
using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesSubset;

class OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver : public ::testing::Test
{
    void SetUp() override
    {
        getStates = [this](const State& aState, const Array<Instant>& anInstantArray) -> Array<State>
        {
            Array<State> states;
            states.reserve(anInstantArray.size());

            const Real& x0 = aState.accessCoordinates()(0);  // Initial position
            const Real& v0 = aState.accessCoordinates()(1);  // Initial position
            const Real omega = std::sqrt(1.0);  // Angular frequency, assuming unit mass and spring constant

            for (const Instant& instant : anInstantArray)
            {
                const Real t = (instant - aState.accessInstant()).inSeconds();
                const Real x = x0 * std::cos(omega * t) + v0 / omega * std::sin(omega * t);
                const Real v = -x0 * omega * std::sin(omega * t) + v0 * std::cos(omega * t);

                VectorXd coordinates(2);
                coordinates << x, v;

                const State state = State(instant, coordinates, Frame::GCRF(), coordinateSubsets_);
                states.add(state);
            }

            return states;
        };

        generateState = [this](const State& aState, const Instant& anInstant) -> State
        {
            const Real& x0 = aState.accessCoordinates()(0);  // Initial position
            const Real& v0 = aState.accessCoordinates()(1);  // Initial position
            const Real omega = std::sqrt(1.0);  // Angular frequency, assuming unit mass and spring constant

            const Real t = (anInstant - aState.accessInstant()).inSeconds();
            const Real x = x0 * std::cos(omega * t) + v0 / omega * std::sin(omega * t);
            const Real v = -x0 * omega * std::sin(omega * t) + v0 * std::cos(omega * t);

            VectorXd coordinates(2);
            coordinates << x, v;

            const State state = State(anInstant, coordinates, Frame::GCRF(), coordinateSubsets_);

            return state;
        };

        VectorXd initialCoordinates(2);
        initialCoordinates << 1.0, 0.0;

        initialState_ = {Instant::J2000(), initialCoordinates, Frame::GCRF(), coordinateSubsets_};
    }

   protected:
    const FiniteDifferenceSolver::Type defaultType_ = FiniteDifferenceSolver::Type::Central;
    const FiniteDifferenceSolver defaultSolver_ = FiniteDifferenceSolver(defaultType_);

    const Array<Shared<const CoordinatesSubset>> coordinateSubsets_ = {
        std::make_shared<const CoordinatesSubset>(CoordinatesSubset("Position", 1)),
        std::make_shared<const CoordinatesSubset>(CoordinatesSubset("Velocity", 1)),
    };

    State initialState_ = State::Undefined();

    std::function<Array<State>(const State&, const Array<Instant>&)> getStates;
    std::function<State(const State&, const Instant&)> generateState;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, Constructor)
{
    EXPECT_NO_THROW(FiniteDifferenceSolver solver(defaultType_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultSolver_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeStateTransitionMatrix)
{
    {
        const Array<Instant> instants = {
            Instant::J2000() + Duration::Seconds(100.0),
            Instant::J2000() + Duration::Seconds(200.0),
        };

        MatrixXd expectedJacobian(2, 4);
        expectedJacobian << std::cos(100.0), std::sin(100.0), std::cos(200.0), std::sin(200.0), std::sin(-100.0),
            std::cos(100.0), std::sin(-200.0), std::cos(200.0);

        {
            const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Central};
            const MatrixXd jacobian = solver.computeStateTransitionMatrix(initialState_, instants, getStates);

            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Forward};
            const MatrixXd jacobian = solver.computeStateTransitionMatrix(initialState_, instants, getStates);
            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Backward};
            const MatrixXd jacobian = solver.computeStateTransitionMatrix(initialState_, instants, getStates);
            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }
    }

    {
        const Instant instant = Instant::J2000() + Duration::Seconds(100.0);

        MatrixXd expectedJacobian(2, 2);
        expectedJacobian << std::cos(100.0), std::sin(100.0), std::sin(-100.0), std::cos(100.0);

        {
            const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Central};
            const MatrixXd jacobian = solver.computeStateTransitionMatrix(initialState_, instant, generateState);

            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Forward};
            const MatrixXd jacobian = solver.computeStateTransitionMatrix(initialState_, instant, generateState);

            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Backward};
            const MatrixXd jacobian = solver.computeStateTransitionMatrix(initialState_, instant, generateState);

            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeGradient)
{
    VectorXd expectedGradient(2);
    expectedGradient << 0.0, -1.0;

    {
        const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Central};
        const VectorXd gradient = solver.computeGradient(initialState_, generateState, Duration::Milliseconds(1.0));

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Forward};
        const VectorXd gradient = solver.computeGradient(initialState_, generateState, Duration::Milliseconds(1e-3));

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {FiniteDifferenceSolver::Type::Backward};
        const VectorXd gradient = solver.computeGradient(initialState_, generateState, Duration::Milliseconds(1e-3));

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultSolver_.print(std::cout, true));
        EXPECT_NO_THROW(defaultSolver_.print(std::cout, false));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, StringFromType)
{
    EXPECT_EQ("Central", FiniteDifferenceSolver::StringFromType(FiniteDifferenceSolver::Type::Central));
    EXPECT_EQ("Forward", FiniteDifferenceSolver::StringFromType(FiniteDifferenceSolver::Type::Forward));
    EXPECT_EQ("Backward", FiniteDifferenceSolver::StringFromType(FiniteDifferenceSolver::Type::Backward));
}
