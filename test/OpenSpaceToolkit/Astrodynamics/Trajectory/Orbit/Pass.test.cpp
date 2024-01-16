// pass.test.cpp

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Core/Types/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

using ostk::core::types::Integer;

using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Duration;

using ostk::astro::trajectory::orbit::Pass;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass : public ::testing::Test
{
   protected:
    const Pass::Type defaultPassType_ = Pass::Type::Complete;
    const Integer defaultPassRevolutionNumber_ = 1;
    const Interval defaultPassInterval_ = Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0));
    const Instant defaultPassNorthPoint_ = Instant::J2000() + Duration::Minutes(15.0);
    const Instant defaultPassSouthPoint_ = Instant::J2000() + Duration::Minutes(45.0);

    Pass defaultPass_ = {
        defaultPassType_,
        defaultPassRevolutionNumber_,
        defaultPassInterval_,
        defaultPassNorthPoint_,
        defaultPassSouthPoint_,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, Constructor)
{
    {
        EXPECT_THROW(
            Pass(
                defaultPassType_,
                defaultPassRevolutionNumber_,
                defaultPassInterval_,
                Instant::Undefined(),
                defaultPassSouthPoint_
            ),
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            Pass(
                defaultPassType_,
                defaultPassRevolutionNumber_,
                defaultPassInterval_,
                defaultPassNorthPoint_,
                Instant::Undefined()
            ),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultPass_ == defaultPass_);
    }

    // north point undefined
    {
        const Pass pass_1 = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::Undefined(),
            Instant::J2000() + Duration::Minutes(45.0),
        };

        const Pass pass_2 = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::Undefined(),
            Instant::J2000() + Duration::Minutes(45.0),
        };

        EXPECT_TRUE(pass_1 == pass_2);
    }

    // south point undefined
    {
        const Pass pass_1 = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::J2000() + Duration::Minutes(15.0),
            Instant::Undefined(),
        };

        const Pass pass_2 = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::J2000() + Duration::Minutes(15.0),
            Instant::Undefined(),
        };

        EXPECT_TRUE(pass_1 == pass_2);
    }

    // north point undefined in first pass, defined in second
    {
        const Pass pass_1 = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::Undefined(),
            Instant::J2000() + Duration::Minutes(45.0),
        };

        const Pass pass_2 = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::J2000() + Duration::Minutes(15.0),
            Instant::J2000() + Duration::Minutes(45.0),
        };

        EXPECT_FALSE(pass_1 == pass_2);
    }

    // south point undefined in first pass, defined in second
    {
        const Pass pass_1 = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::J2000() + Duration::Minutes(15.0),
            Instant::Undefined(),
        };

        const Pass pass_2 = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::J2000() + Duration::Minutes(15.0),
            Instant::J2000() + Duration::Minutes(45.0),
        };

        EXPECT_FALSE(pass_1 == pass_2);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultPass_ != defaultPass_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, StreamOperator)
{
    testing::internal::CaptureStdout();

    EXPECT_NO_THROW(std::cout << defaultPass_ << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, IsDefined)
{
    {
        EXPECT_TRUE(defaultPass_.isDefined());
    }

    {
        const Pass pass = {
            Pass::Type::Undefined,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::Undefined(),
            Instant::Undefined(),
        };

        EXPECT_FALSE(pass.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, IsComplete)
{
    {
        EXPECT_THROW(Pass::Undefined().isComplete(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_TRUE(defaultPass_.isComplete());
    }

    {
        const Pass pass = {
            Pass::Type::Partial,
            1,
            Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(1.0)),
            Instant::Undefined(),
            Instant::J2000() + Duration::Minutes(45.0),
        };
        EXPECT_FALSE(pass.isComplete());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetType)
{
    {
        EXPECT_THROW(Pass::Undefined().isComplete(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.getType(), defaultPassType_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetRevolutionNumber)
{
    {
        EXPECT_THROW(Pass::Undefined().isComplete(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.getRevolutionNumber(), defaultPassRevolutionNumber_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetInterval)
{
    {
        EXPECT_THROW(Pass::Undefined().isComplete(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.getInterval(), defaultPassInterval_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, AccessNorthPoint)
{
    {
        EXPECT_THROW(Pass::Undefined().isComplete(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.accessNorthPoint(), defaultPassNorthPoint_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, AccessSouthPoint)
{
    {
        EXPECT_THROW(Pass::Undefined().isComplete(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.accessSouthPoint(), defaultPassSouthPoint_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, Undefined)
{
    {
        EXPECT_FALSE(Pass::Undefined().isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, StringFromType)
{
    EXPECT_EQ(Pass::StringFromType(Pass::Type::Undefined), "Undefined");
    EXPECT_EQ(Pass::StringFromType(Pass::Type::Complete), "Complete");
    EXPECT_EQ(Pass::StringFromType(Pass::Type::Partial), "Partial");
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, StringFromPhase)
{
    EXPECT_EQ(Pass::StringFromPhase(Pass::Phase::Undefined), "Undefined");
    EXPECT_EQ(Pass::StringFromPhase(Pass::Phase::Ascending), "Ascending");
    EXPECT_EQ(Pass::StringFromPhase(Pass::Phase::Descending), "Descending");
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, StringFromQuarter)
{
    EXPECT_EQ(Pass::StringFromQuarter(Pass::Quarter::Undefined), "Undefined");
    EXPECT_EQ(Pass::StringFromQuarter(Pass::Quarter::First), "First");
    EXPECT_EQ(Pass::StringFromQuarter(Pass::Quarter::Second), "Second");
    EXPECT_EQ(Pass::StringFromQuarter(Pass::Quarter::Third), "Third");
    EXPECT_EQ(Pass::StringFromQuarter(Pass::Quarter::Fourth), "Fourth");
}
