/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Dynamics.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Dynamics/PositionDerivative.hpp>

#include <Global.test.hpp>

using ostk::physics::time::Instant;

using ostk::astro::trajectory::Dynamics;
using ostk::astro::trajectory::dynamics::PositionDerivative;

class OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        startStateVector_.resize(6);
        startStateVector_[0] = 7000000.0;
        startStateVector_[1] = 0.0;
        startStateVector_[2] = 0.0;
        startStateVector_[3] = 5000.12345;
        startStateVector_[4] = 7546.05329;
        startStateVector_[5] = 8000.5737;
    }

    const PositionDerivative positionDerivative_;
    const Instant startInstant_ = Instant::J2000();

    Dynamics::StateVector startStateVector_;
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Flight_System_Dynamics_PositionDerivative, ApplyContribution)
{
    Dynamics::StateVector dxdt(6, 0.0);
    positionDerivative_.applyContribution(startStateVector_, dxdt, startInstant_);
    EXPECT_GT(1e-15, startStateVector_[3] - dxdt[0]);
    EXPECT_GT(1e-15, startStateVector_[4] - dxdt[1]);
    EXPECT_GT(1e-15, startStateVector_[5] - dxdt[2]);
    EXPECT_GT(1e-15, 0.0 - dxdt[3]);
    EXPECT_GT(1e-15, 0.0 - dxdt[4]);
    EXPECT_GT(1e-15, 0.0 - dxdt[5]);
}
