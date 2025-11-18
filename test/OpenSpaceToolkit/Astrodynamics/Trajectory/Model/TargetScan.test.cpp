/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/TargetScan.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::Vector6d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::trajectory::model::TargetScan;
using ostk::astrodynamics::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan : public ::testing::Test
{
   protected:
    const Earth earth_ = Earth::WGS84();
    const LLA startLLA_ = LLA::Vector({0.099457122261122202, -100.3695068214156, 0.0});
    const LLA endLLA_ = LLA::Vector({0.40715652013782405, -100.43336631325768, 0.0});
    const Instant startInstant_ = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);
    const Instant endInstant_ = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 5), Scale::UTC);
    const Duration stepSize_ = Duration::Seconds(1e-2);

    TargetScan targetScan_ = TargetScan(startLLA_, endLLA_, startInstant_, endInstant_, earth_, stepSize_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, Constructor)
{
    EXPECT_NO_THROW(TargetScan targetScan(startLLA_, endLLA_, startInstant_, endInstant_, earth_, stepSize_));

    EXPECT_NO_THROW(TargetScan targetScan(startLLA_, endLLA_, startInstant_, endInstant_, earth_));

    EXPECT_NO_THROW(TargetScan targetScan(startLLA_, endLLA_, startInstant_, endInstant_));
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, IsDefined)
{
    EXPECT_TRUE(targetScan_.isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, CalculateStateAt)
{
    // undefined cases

    {
        {
            EXPECT_THROW(targetScan_.calculateStateAt(Instant::Undefined()), ostk::core::error::runtime::Undefined);
        }

        {
            EXPECT_THROW(
                targetScan_.calculateStateAt(startInstant_ - Duration::Seconds(1.0)), ostk::core::error::RuntimeError
            );
        }

        {
            EXPECT_THROW(
                targetScan_.calculateStateAt(endInstant_ + Duration::Seconds(1.0)), ostk::core::error::RuntimeError
            );
        }

        {
            const TargetScan undefinedTargetScan(
                LLA::Undefined(), LLA::Undefined(), Instant::Undefined(), Instant::Undefined(), Earth::WGS84()
            );
            EXPECT_THROW(undefinedTargetScan.calculateStateAt(startInstant_), ostk::core::error::runtime::Undefined);
        }
    }

    // values obtained from Orekit by simulating a nadir pointing trajectory
    // within interval
    {
        const Instant instant = startInstant_ + (endInstant_ - startInstant_) / 2.0;

        const State state = targetScan_.calculateStateAt(instant);

        const Vector6d expectedCoordinates = {
            6378113.46765721,
            -2392.2706621550024,
            17005.12534605209,
            -18.702691992298014,
            -956.905990523886,
            6804.66697033946,
        };

        const VectorXd coordinates = state.getCoordinates();

        EXPECT_VECTORS_ALMOST_EQUAL(coordinates, expectedCoordinates, 1e-1);
    }

    // boundaries interval
    // Note: less accurate at boundaries due to polynomial interpolation
    {
        {
            const State state = targetScan_.calculateStateAt(startInstant_);

            const Vector6d expectedCoordinates = {
                6378136.936099239,
                2.6122573763132095E-4,
                -6.584354159011127,
                -0.07204923298218091,
                -956.9095709619337,
                6804.692417661864,
            };

            const VectorXd coordinates = state.getCoordinates();

            EXPECT_VECTORS_ALMOST_EQUAL(coordinates, expectedCoordinates, 1.5);
        }

        {
            const State state = targetScan_.calculateStateAt(endInstant_);

            const Vector6d expectedCoordinates = {
                6378043.422760435,
                -4784.523805314675,
                34016.70863685098,
                -37.33318936532419,
                -956.8953467933591,
                6804.591290206263,
            };

            const VectorXd coordinates = state.getCoordinates();

            EXPECT_VECTORS_ALMOST_EQUAL(coordinates, expectedCoordinates, 1.5);
        }
    }

    // Regression, discontinuities in velocity
    {
        const LLA startLLA = LLA::Vector({0.0, 0.0, 0.0});
        const LLA endLLA = LLA::Vector({0.5, 0.1, 0.0});

        const Interval interval = Interval::Closed(
            Instant::Parse("2024-01-12T02:33:25.366299241", Scale::UTC),
            Instant::Parse("2024-01-12T02:34:39.106651991", Scale::UTC)
        );

        const Array<Instant> instants = interval.generateGrid(Duration::Seconds(1.0));

        const TargetScan targetScan = TargetScan(startLLA, endLLA, startInstant_, endInstant_, earth_, stepSize_);

        for (const auto& state : targetScan.calculateStatesAt(instants))
        {
            const Vector3d velocity = state.getVelocity().getCoordinates();
            EXPECT_NEAR(velocity.norm(), 970.3, 1.0);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, Clone)
{
    TargetScan* clonedTargetScan = targetScan_.clone();

    EXPECT_TRUE(clonedTargetScan != nullptr);
    EXPECT_TRUE(*clonedTargetScan == targetScan_);

    delete clonedTargetScan;
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, EqualityOperator)
{
    EXPECT_TRUE(targetScan_ == targetScan_);

    const TargetScan targetScan2(LLA::Vector({1.0, 0.0, 0.0}), endLLA_, startInstant_, endInstant_, earth_);
    EXPECT_FALSE(targetScan_ == targetScan2);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, InequalityOperator)
{
    EXPECT_FALSE(targetScan_ != targetScan_);

    const TargetScan targetScan2(LLA::Vector({1.0, 0.0, 0.0}), endLLA_, startInstant_, endInstant_, earth_);
    EXPECT_TRUE(targetScan_ != targetScan2);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, Print)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(targetScan_.print(std::cout, true));
    EXPECT_NO_THROW(targetScan_.print(std::cout, false));

    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << targetScan_);

    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, Getters)
{
    EXPECT_EQ(targetScan_.getStartLLA(), startLLA_);
    EXPECT_EQ(targetScan_.getEndLLA(), endLLA_);
    EXPECT_EQ(targetScan_.getStartInstant(), startInstant_);
    EXPECT_EQ(targetScan_.getEndInstant(), endInstant_);
    EXPECT_EQ(targetScan_.getCelestial().getName(), earth_.getName());
    EXPECT_EQ(targetScan_.getStepSize(), stepSize_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_TargetScan, FromGroundSpeed)
{
    const Derived groundSpeed = Derived(1000.0, Derived::Unit::MeterPerSecond());
    const Duration stepSize = Duration::Seconds(1.0);

    EXPECT_NO_THROW(TargetScan::FromGroundSpeed(startLLA_, endLLA_, groundSpeed, startInstant_, earth_, stepSize));

    // undefined cases

    {
        EXPECT_THROW(
            TargetScan::FromGroundSpeed(LLA::Undefined(), endLLA_, groundSpeed, startInstant_, earth_, stepSize),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            TargetScan::FromGroundSpeed(startLLA_, LLA::Undefined(), groundSpeed, startInstant_, earth_, stepSize),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            TargetScan::FromGroundSpeed(startLLA_, endLLA_, Derived::Undefined(), startInstant_, earth_, stepSize),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            TargetScan::FromGroundSpeed(
                startLLA_, endLLA_, Derived(-1000.0, Derived::Unit::MeterPerSecond()), startInstant_, earth_, stepSize
            ),
            ostk::core::error::RuntimeError
        );

        EXPECT_THROW(
            TargetScan::FromGroundSpeed(startLLA_, endLLA_, groundSpeed, Instant::Undefined(), earth_, stepSize),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            TargetScan::FromGroundSpeed(startLLA_, endLLA_, groundSpeed, startInstant_, earth_, Duration::Undefined()),
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            TargetScan::FromGroundSpeed(
                startLLA_, endLLA_, groundSpeed, startInstant_, earth_, Duration::Seconds(-1.0)
            ),
            ostk::core::error::RuntimeError
        );
    }
}
