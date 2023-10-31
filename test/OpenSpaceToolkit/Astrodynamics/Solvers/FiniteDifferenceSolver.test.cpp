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

            const Real A = aState.accessCoordinates()(0);  // Initial position
            const Real omega = std::sqrt(1.0);             // Angular frequency, assuming unit mass and spring constant
            const Real phi = 0.0;                          // Phase angle, assumed to be zero for simplicity

            for (const Instant& instant : anInstantArray)
            {
                const Real t = (instant - aState.accessInstant()).inSeconds();
                const Real x = A * std::cos(omega * t + phi);
                const Real v = -A * omega * std::sin(omega * t + phi);

                VectorXd coordinates(2);
                coordinates << x, v;

                std::cout << coordinates.transpose() << std::endl;

                const State state = State(instant, coordinates, Frame::GCRF(), coordinateSubsets_);
                states.add(state);
            }

            return states;
        };

        getState = [this](const State& aState, const Instant& anInstant) -> State
        {
            const Real A = aState.accessCoordinates()(0);  // Initial position
            const Real omega = std::sqrt(1.0);             // Angular frequency, assuming unit mass and spring constant
            const Real phi = 0.0;                          // Phase angle, assumed to be zero for simplicity

            const Real t = (anInstant - aState.accessInstant()).inSeconds();  // Calculate the time elapsed
            const Real x = A * std::cos(omega * t + phi);                     // Analytical position
            const Real v = -A * omega * std::sin(omega * t + phi);            // Analytical velocity

            VectorXd coordinates(2);
            coordinates << x, v;

            State state = State(anInstant, coordinates, Frame::GCRF(), coordinateSubsets_);

            return state;
        };

        VectorXd initialCoordinates(2);
        initialCoordinates << 1.0, 0.0;

        initialState_ = {
            Instant::J2000(),
            initialCoordinates,
            Frame::GCRF(),
            coordinateSubsets_
        };
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
    std::function<State(const State&, const Instant&)> getState;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeJacobian)
{
    {
        MatrixXd jacobian = defaultSolver_.computeJacobian(initialState_, {Instant::J2000() + Duration::Seconds(100.0)}, getStates);
        std::cout << jacobian << std::endl;
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_FiniteDifferenceSolver, ComputeGradient)
{
    EXPECT_NO_THROW(defaultSolver_.computeGradient(initialState_, getState, Duration::Milliseconds(1.0)));
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
