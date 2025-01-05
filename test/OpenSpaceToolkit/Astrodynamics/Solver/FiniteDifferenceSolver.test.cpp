/// Apache License 2.0

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/FiniteDifferenceSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::Environment;
using ostk::physics::environment::object::celestial::Earth;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::solver::FiniteDifferenceSolver;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver : public ::testing::Test
{
    void SetUp() override
    {
        generateStatesCoordinates_ = [](const State& aState, const Array<Instant>& anInstantArray) -> MatrixXd
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

        generateStateCoordinates_ = [](const State& aState, const Instant& anInstant) -> VectorXd
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
    const Duration defaultStepDuration_ = Duration::Microseconds(1.0);
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

    std::function<MatrixXd(const State&, const Array<Instant>&)> generateStatesCoordinates_;
    std::function<VectorXd(const State&, const Instant&)> generateStateCoordinates_;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, computeStateTransitionMatrix)
{
    {
        const Array<Instant> instants = {
            Instant::J2000() + Duration::Seconds(100.0),
            Instant::J2000() + Duration::Seconds(200.0),
            Instant::J2000() + Duration::Seconds(300.0),
        };

        MatrixXd expectedStateTransitionMatrix(2 * instants.getSize(), 2);

        expectedStateTransitionMatrix << std::cos(100.0), std::sin(100.0), std::sin(-100.0), std::cos(100.0),
            std::cos(200.0), std::sin(200.0), std::sin(-200.0), std::cos(200.0), std::cos(300.0), std::sin(300.0),
            std::sin(-300.0), std::cos(300.0);

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Central,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian =
                solver.computeStateTransitionMatrix(initialState_, instants, generateStatesCoordinates_, 2);

            EXPECT_TRUE(jacobian.isApprox(expectedStateTransitionMatrix, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Forward,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian =
                solver.computeStateTransitionMatrix(initialState_, instants, generateStatesCoordinates_, 2);
            EXPECT_TRUE(jacobian.isApprox(expectedStateTransitionMatrix, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Backward,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd jacobian =
                solver.computeStateTransitionMatrix(initialState_, instants, generateStatesCoordinates_, 2);
            EXPECT_TRUE(jacobian.isApprox(expectedStateTransitionMatrix, 1e-12));
        }
    }

    {
        const Instant instant = Instant::J2000() + Duration::Seconds(100.0);

        MatrixXd expectedStateTransitionMatrix(2, 2);
        expectedStateTransitionMatrix << std::cos(100.0), std::sin(100.0), std::sin(-100.0), std::cos(100.0);

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Central,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd stateTransitionMatrix =
                solver.computeStateTransitionMatrix(initialState_, instant, generateStateCoordinates_, 2);

            EXPECT_TRUE(stateTransitionMatrix.isApprox(expectedStateTransitionMatrix, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Forward,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd stateTransitionMatrix =
                solver.computeStateTransitionMatrix(initialState_, instant, generateStateCoordinates_, 2);

            EXPECT_TRUE(stateTransitionMatrix.isApprox(expectedStateTransitionMatrix, 1e-12));
        }

        {
            const FiniteDifferenceSolver solver = {
                FiniteDifferenceSolver::Type::Backward,
                defaultStepPercentage_,
                defaultStepDuration_,
            };
            const MatrixXd stateTransitionMatrix =
                solver.computeStateTransitionMatrix(initialState_, instant, generateStateCoordinates_, 2);

            EXPECT_TRUE(stateTransitionMatrix.isApprox(expectedStateTransitionMatrix, 1e-12));
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
        const VectorXd gradient = solver.computeGradient(initialState_, generateStateCoordinates_);

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Forward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(initialState_, generateStateCoordinates_);

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Backward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(initialState_, generateStateCoordinates_);

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeGradient_TwoBodyProblem)
{
    VectorXd coordinates(6);
    coordinates << 7000000.0, 0.0, 0.0, 0.0, 7546.0, 0.0;

    const State state = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };

    const Environment environment = Environment(Instant::J2000(), {std::make_shared<Earth>(Earth::Spherical())});

    const Propagator propagator = Propagator::Default(environment);

    const auto generateStateCoordinates = [&propagator](const State& aState, const Instant& anInstant) -> VectorXd
    {
        return propagator.calculateStateAt(aState, anInstant).accessCoordinates();
    };

    const Real mu =
        EarthGravitationalModel::EGM2008.gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared());

    const Vector3d positionCoordinates = state.getPosition().getCoordinates();
    const Real r = positionCoordinates.norm();
    const Real r3 = std::pow(r, 3);

    const Real x = positionCoordinates(0);
    const Real y = positionCoordinates(1);
    const Real z = positionCoordinates(2);

    VectorXd expectedGradient(6);
    expectedGradient << coordinates(3), coordinates(4), coordinates(5), -mu * x / r3, -mu * y / r3, -mu * z / r3;

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Central,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(state, generateStateCoordinates);

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Forward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(state, generateStateCoordinates);

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Backward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(state, generateStateCoordinates);

        EXPECT_TRUE(gradient.isApprox(expectedGradient, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Central,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const VectorXd gradient = solver.computeGradient(state, generateStateCoordinates);

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

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeJacobian)
{
    const State state = {
        Instant::J2000(),
        VectorXd::Zero(2),
        Frame::GCRF(),
        {std::make_shared<CoordinateSubset>("Position", 1), std::make_shared<CoordinateSubset>("Velocity", 1)},
    };

    const auto generateStateCoordinates = [](const State& aState, const Instant& anInstant) -> VectorXd
    {
        const Real& x0 = aState.accessCoordinates()(0);  // Initial position
        const Real& v0 = aState.accessCoordinates()(1);  // Initial position
        const Real omega = std::sqrt(1.0);               // Angular frequency, assuming unit mass and spring constant

        const Real t = (anInstant - aState.accessInstant()).inSeconds();
        const Real x = x0 * std::cos(omega * t) + v0 / omega * std::sin(omega * t);
        const Real v = -x0 * omega * std::sin(omega * t) + v0 * std::cos(omega * t);

        VectorXd coordinates(2, 1);
        coordinates << x, v;

        return coordinates;
    };

    MatrixXd expectedJacobian(2, 2);
    expectedJacobian << 0.0, 1.0, -1.0, 0.0;

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Central,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const MatrixXd jacobian = solver.computeJacobian(state, generateStateCoordinates);

        EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Forward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const MatrixXd jacobian = solver.computeJacobian(state, generateStateCoordinates);

        EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-6));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Backward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const MatrixXd jacobian = solver.computeJacobian(state, generateStateCoordinates);

        EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-6));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeJacobian_TwoBodyProblem)
{
    VectorXd coordinates(6);
    coordinates << -71095.1731570421, -4419328.15020176, -5283063.76725156, -1480.1545494292, 5721.69282426838,
        -4772.2461992043;

    const State state = {
        Instant::J2000(),
        coordinates,
        Frame::GCRF(),
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };

    const Environment environment = Environment(Instant::J2000(), {std::make_shared<Earth>(Earth::Spherical())});

    const Propagator propagator = Propagator::Default(environment);

    const auto generateStateCoordinates = [&propagator](const State& aState, const Instant& anInstant) -> VectorXd
    {
        return propagator.calculateStateAt(aState, anInstant).accessCoordinates();
    };

    const Real mu =
        EarthGravitationalModel::EGM2008.gravitationalParameter_.in(Derived::Unit::MeterCubedPerSecondSquared());

    const Vector3d positionCoordinates = state.getPosition().getCoordinates();
    const Real r = positionCoordinates.norm();
    const Real r3 = std::pow(r, 3);
    const Real r5 = std::pow(r, 5);

    const Real x = positionCoordinates(0);
    const Real y = positionCoordinates(1);
    const Real z = positionCoordinates(2);

    MatrixXd expectedJacobian(6, 6);
    expectedJacobian << 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,  // first row
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0,                  // second row
        0.0, 0.0, 0.0, 0.0, 0.0, 1.0,                  // third row
        (-mu / r3) + (3.0 * mu * x * x) / r5, 3.0 * mu * x * y / r5, 3.0 * mu * x * z / r5, 0.0, 0.0,
        0.0,  // fourth row
        3.0 * mu * x * y / r5, (-mu / r3) + (3.0 * mu * y * y / r5), 3.0 * mu * y * z / r5, 0.0, 0.0,
        0.0,  // fifth row
        3.0 * mu * x * z / r5, 3.0 * mu * y * z / r5, (-mu / r3) + (3.0 * mu * z * z / r5), 0.0, 0.0,
        0.0;  // sixth row

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Central,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const MatrixXd jacobian = solver.computeJacobian(state, generateStateCoordinates);

        EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-3));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Forward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const MatrixXd jacobian = solver.computeJacobian(state, generateStateCoordinates);

        EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-3));
    }

    {
        const FiniteDifferenceSolver solver = {
            FiniteDifferenceSolver::Type::Backward,
            defaultStepPercentage_,
            defaultStepDuration_,
        };
        const MatrixXd jacobian = solver.computeJacobian(state, generateStateCoordinates);

        EXPECT_TRUE(jacobian.isApprox(expectedJacobian, 1e-3));
    }
}
