/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Containers/Array.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/Dynamics/PositionDerivative.hpp>
#include <OpenSpaceToolkit/Astrodynamics/NumericalSolver.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::ctnr::Array;
using ostk::core::types::Shared;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::NumericalSolver;
using ostk::astro::flight::system::Dynamics;
using ostk::astro::flight::system::dynamics::PositionDerivative;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        startStateVector_.resize(6);
        startStateVector_ << 7000000.0, 0.0, 0.0, 5000.12345, 7546.05329, 8000.5737;
    }

    const PositionDerivative positionDerivative_;
    const Instant startInstant_ = Instant::J2000();

    NumericalSolver::StateVector startStateVector_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative, Constructor)
{
    {
        EXPECT_NO_THROW(PositionDerivative positionDerivative());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative, IsDefined)
{
    EXPECT_TRUE(positionDerivative_.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative, StreamOperator)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << positionDerivative_ << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(positionDerivative_.print(std::cout, true));
        EXPECT_NO_THROW(positionDerivative_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative, GetReadCoordinatesSubsets)
{
    const PositionDerivative positionDerivative = PositionDerivative();

    const Array<Shared<const CoordinatesSubset>> subsets = positionDerivative.getReadCoordinatesSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianVelocity::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative, GetWriteCoordinatesSubsets)
{
    const PositionDerivative positionDerivative = PositionDerivative();

    const Array<Shared<const CoordinatesSubset>> subsets = positionDerivative.getWriteCoordinatesSubsets();

    EXPECT_EQ(1, subsets.size());
    EXPECT_EQ(CartesianPosition::Default(), subsets[0]);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative, ComputeContribution)
{
    const VectorXd contribution =
        positionDerivative_.computeContribution(startInstant_, startStateVector_.segment(3, 3), Frame::Undefined());

    EXPECT_EQ(3, contribution.size());
    EXPECT_EQ(startStateVector_[3], contribution[0]);
    EXPECT_EQ(startStateVector_[4], contribution[1]);
    EXPECT_EQ(startStateVector_[5], contribution[2]);
}
