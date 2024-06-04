/// Apache License 2.0

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::astrodynamics::solver::FiniteDifferenceSolver;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

class OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver : public ::testing::Test
{
    void SetUp() override
    {
        generateStatesCoordinates = [](const State& aState, const Array<Instant>& anInstantArray) -> MatrixXd
        {
            MatrixXd statesCoordinates(2, anInstantArray.size());

            const Real& x0 = aState.accessCoordinates()(0);  // Initial position
            const Real& v0 = aState.accessCoordinates()(1);  // Initial position
            const Real omega = std::sqrt(1.0);  // Angular frequency, assuming unit mass and spring constant

            Size i = 0;
            for (const Instant& instant : anInstantArray)
            {
                const Real t = (instant - aState.accessInstant()).inSeconds();
                const Real x = x0 * std::cos(omega * t) + v0 / omega * std::sin(omega * t);
                const Real v = -x0 * omega * std::sin(omega * t) + v0 * std::cos(omega * t);

                VectorXd coordinates(2);
                coordinates << x, v;

                statesCoordinates.col(i) = coordinates;
                ++i;
            }

            return statesCoordinates;
        };

        generateStateCoordinates = [](const State& aState, const Instant& anInstant) -> VectorXd
        {
            const Real& x0 = aState.accessCoordinates()(0);  // Initial position
            const Real& v0 = aState.accessCoordinates()(1);  // Initial position
            const Real omega = std::sqrt(1.0);  // Angular frequency, assuming unit mass and spring constant

            const Real t = (anInstant - aState.accessInstant()).inSeconds();
            const Real x = x0 * std::cos(omega * t) + v0 / omega * std::sin(omega * t);
            const Real v = -x0 * omega * std::sin(omega * t) + v0 * std::cos(omega * t);

            VectorXd coordinates(2, 1);
            coordinates << x, v;

            return coordinates;
        };

        VectorXd initialCoordinates(2);
        initialCoordinates << 1.0, 0.0;

        initialState_ = {Instant::J2000(), initialCoordinates, Frame::GCRF(), coordinateSubsets_};
    }

   protected:
    const FiniteDifferenceSolver::Type defaultType_ = FiniteDifferenceSolver::Type::Central;
    const Real defaultStepPercentage_ = 1e-3;
    const Duration defaultStepDuration_ = Duration::Milliseconds(1e-3);
    const FiniteDifferenceSolver defaultSolver_ = {
        defaultType_,
        defaultStepPercentage_,
        defaultStepDuration_,
    };

    const Array<Shared<const CoordinateSubset>> coordinateSubsets_ = {
        std::make_shared<const CoordinateSubset>(CoordinateSubset("Position", 1)),
        std::make_shared<const CoordinateSubset>(CoordinateSubset("Velocity", 1)),
    };

    State initialState_ = State::Undefined();

    std::function<MatrixXd(const State&, const Array<Instant>&)> generateStatesCoordinates;
    std::function<VectorXd(const State&, const Instant&)> generateStateCoordinates;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, Constructor)
{
    EXPECT_NO_THROW(FiniteDifferenceSolver solver(defaultType_, defaultStepPercentage_, defaultStepDuration_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << defaultSolver_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, Getters)
{
    {
        EXPECT_EQ(defaultType_, defaultSolver_.getType());
    }

    {
        EXPECT_EQ(defaultStepPercentage_, defaultSolver_.getStepPercentage());
    }

    {
        EXPECT_EQ(defaultStepDuration_, defaultSolver_.getStepDuration());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeJacobian)
{
    {
        const Array<Instant> instants = {
            Instant::J2000() + Duration::Seconds(100.0),
            Instant::J2000() + Duration::Seconds(200.0),
            Instant::J2000() + Duration::Seconds(300.0),
        };

        MatrixXd expectedJacobian(2 * instants.getSize(), 2);

        expectedJacobian << std::cos(100.0), std::sin(100.0), std::sin(-100.0), std::cos(100.0), std::cos(200.0),
            std::sin(200.0), std::sin(-200.0), std::cos(200.0), std::cos(300.0), std::sin(300.0), std::sin(-300.0),
            std::cos(300.0);

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Central,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian = solver.computeJacobian(initialState_, instants, generateStatesCoordinates, 2);

            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Forward,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian = solver.computeJacobian(initialState_, instants, generateStatesCoordinates, 2);
            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Backward,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian = solver.computeJacobian(initialState_, instants, generateStatesCoordinates, 2);
            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }
    }

    {
        const Instant instant = Instant::J2000() + Duration::Seconds(100.0);

        MatrixXd expectedJacobian(2, 2);
        expectedJacobian << std::cos(100.0), std::sin(100.0), std::sin(-100.0), std::cos(100.0);

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Central,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian = solver.computeJacobian(initialState_, instant, generateStateCoordinates, 2);

            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Forward,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian = solver.computeJacobian(initialState_, instant, generateStateCoordinates, 2);

            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Backward,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian = solver.computeJacobian(initialState_, instant, generateStateCoordinates, 2);

            EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-12));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeGradient)
{
    VectorXd expectedGradient(2);
    expectedGradient << 0.0, -1.0;

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Central,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(initialState_, generateStateCoordinates);

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Forward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(initialState_, generateStateCoordinates);

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Backward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(initialState_, generateStateCoordinates);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, Default)
{
    EXPECT_NO_THROW(FiniteDifferenceSolver::Default());
}
