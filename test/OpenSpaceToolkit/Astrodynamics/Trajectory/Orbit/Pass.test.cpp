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
    const Integer defaultRevolutionNumber_ = 1;
    const Instant defaultInstantAtAscendingNode_ = Instant::J2000();
    const Instant defaultInstantAtNorthPoint_ = Instant::J2000() + Duration::Minutes(15.0);
    const Instant defaultInstantAtDescendingNode_ = Instant::J2000() + Duration::Minutes(30.0);
    const Instant defaultInstantAtSouthPoint_ = Instant::J2000() + Duration::Minutes(45.0);
    const Instant defaultInstantAtPassBreak_ = Instant::J2000() + Duration::Hours(1.0);

    const Pass defaultPass_ = {
        defaultRevolutionNumber_,
        defaultInstantAtAscendingNode_,
        defaultInstantAtNorthPoint_,
        defaultInstantAtDescendingNode_,
        defaultInstantAtSouthPoint_,
        defaultInstantAtPassBreak_,
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, Constructor)
{
    // Instant orders
    {
        // ascending node > north point
        EXPECT_THROW(
            Pass(
                defaultRevolutionNumber_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_
            ),
            ostk::core::error::RuntimeError
        );

        // north point > descending node
        EXPECT_THROW(
            Pass(
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_
            ),
            ostk::core::error::RuntimeError
        );

        // descending node > south point
        EXPECT_THROW(
            Pass(
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtPassBreak_
            ),
            ostk::core::error::RuntimeError
        );

        // south point > pass break
        EXPECT_THROW(
            Pass(
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtPassBreak_,
                defaultInstantAtSouthPoint_
            ),
            ostk::core::error::RuntimeError
        );
    }

    // complete
    {
        EXPECT_EQ(
            Pass(
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_
            )
                .getType(),
            Pass::Type::Complete
        );
    }

    // partial
    {
        {
            EXPECT_EQ(
                Pass(
                    defaultRevolutionNumber_,
                    Instant::Undefined(),
                    defaultInstantAtNorthPoint_,
                    defaultInstantAtDescendingNode_,
                    defaultInstantAtSouthPoint_,
                    defaultInstantAtPassBreak_
                )
                    .getType(),
                Pass::Type::Partial
            );
        }

        {
            EXPECT_EQ(
                Pass(
                    defaultRevolutionNumber_,
                    defaultInstantAtAscendingNode_,
                    Instant::Undefined(),
                    defaultInstantAtDescendingNode_,
                    defaultInstantAtSouthPoint_,
                    defaultInstantAtPassBreak_
                )
                    .getType(),
                Pass::Type::Partial
            );
        }

        {
            EXPECT_EQ(
                Pass(
                    defaultRevolutionNumber_,
                    defaultInstantAtAscendingNode_,
                    defaultInstantAtNorthPoint_,
                    Instant::Undefined(),
                    defaultInstantAtSouthPoint_,
                    defaultInstantAtPassBreak_
                )
                    .getType(),
                Pass::Type::Partial
            );
        }

        {
            EXPECT_EQ(
                Pass(
                    defaultRevolutionNumber_,
                    defaultInstantAtAscendingNode_,
                    defaultInstantAtNorthPoint_,
                    defaultInstantAtDescendingNode_,
                    Instant::Undefined(),
                    defaultInstantAtPassBreak_
                )
                    .getType(),
                Pass::Type::Partial
            );
        }

        {
            EXPECT_EQ(
                Pass(
                    defaultRevolutionNumber_,
                    defaultInstantAtAscendingNode_,
                    defaultInstantAtNorthPoint_,
                    defaultInstantAtDescendingNode_,
                    defaultInstantAtSouthPoint_,
                    Instant::Undefined()
                )
                    .getType(),
                Pass::Type::Partial
            );
        }
    }

    // undefined
    {
        const Pass pass = {
            defaultRevolutionNumber_,
            Instant::Undefined(),
            Instant::Undefined(),
            Instant::Undefined(),
            Instant::Undefined(),
            Instant::Undefined(),
        };
        EXPECT_FALSE(pass.isDefined());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultPass_ == defaultPass_);
    }

    // Ascending node
    {
        // undefined
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                Instant::Undefined(),
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                Instant::Undefined(),
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            EXPECT_TRUE(pass_1 == pass_2);
        }

        // Ascending point undefined in first pass, defined in second
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                Instant::Undefined(),
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            EXPECT_FALSE(pass_1 == pass_2);
        }
    }

    // north point
    {
        // undefined
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                Instant::Undefined(),
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                Instant::Undefined(),
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,

            };

            EXPECT_TRUE(pass_1 == pass_2);
        }

        // north point undefined in first pass, defined in second
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                Instant::Undefined(),
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            EXPECT_FALSE(pass_1 == pass_2);
        }
    }

    // descending node
    {
        // undefined
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                Instant::Undefined(),
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                Instant::Undefined(),
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            EXPECT_TRUE(pass_1 == pass_2);
        }

        // descending node undefined in first pass, defined in second
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                Instant::Undefined(),
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            EXPECT_FALSE(pass_1 == pass_2);
        }
    }

    // south point
    {
        // undefined
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                Instant::Undefined(),
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                Instant::Undefined(),
                defaultInstantAtPassBreak_,
            };

            EXPECT_TRUE(pass_1 == pass_2);
        }

        // south point undefined in first pass, defined in second
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                Instant::Undefined(),
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            EXPECT_FALSE(pass_1 == pass_2);
        }
    }

    // pass break
    {
        // undefined
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                Instant::Undefined(),
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                Instant::Undefined(),
            };

            EXPECT_TRUE(pass_1 == pass_2);
        }

        // pass break undefined in first pass, defined in second
        {
            const Pass pass_1 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                Instant::Undefined(),
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            EXPECT_FALSE(pass_1 == pass_2);
        }
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
            defaultRevolutionNumber_,
            Instant::Undefined(),
            Instant::Undefined(),
            Instant::Undefined(),
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
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            defaultInstantAtSouthPoint_,
            Instant::Undefined(),
        };
        EXPECT_FALSE(pass.isComplete());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetType)
{
    {
        EXPECT_THROW(Pass::Undefined().getType(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.getType(), Pass::Type::Complete);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetRevolutionNumber)
{
    {
        EXPECT_THROW(Pass::Undefined().getRevolutionNumber(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.getRevolutionNumber(), defaultRevolutionNumber_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetDuration)
{
    {
        EXPECT_THROW(Pass::Undefined().getDuration(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_TRUE(defaultPass_.getDuration().isDefined());
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            Instant::Undefined(),
            defaultInstantAtPassBreak_,
        };
        EXPECT_FALSE(pass.getDuration().isDefined());
    }

    {
        EXPECT_EQ(defaultPass_.getDuration(), defaultInstantAtPassBreak_ - defaultInstantAtAscendingNode_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, AccessInstantAtAscendingNode)
{
    {
        EXPECT_THROW(Pass::Undefined().accessInstantAtAscendingNode(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.accessInstantAtAscendingNode(), defaultInstantAtAscendingNode_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, AccessInstantAtNorthPoint)
{
    {
        EXPECT_THROW(Pass::Undefined().accessInstantAtNorthPoint(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.accessInstantAtNorthPoint(), defaultInstantAtNorthPoint_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, AccessInstantAtDescendingNode)
{
    {
        EXPECT_THROW(Pass::Undefined().accessInstantAtDescendingNode(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.accessInstantAtDescendingNode(), defaultInstantAtDescendingNode_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, AccessInstantAtSouthPoint)
{
    {
        EXPECT_THROW(Pass::Undefined().accessInstantAtSouthPoint(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.accessInstantAtSouthPoint(), defaultInstantAtSouthPoint_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, AccessInstantAtPassBreak)
{
    {
        EXPECT_THROW(Pass::Undefined().accessInstantAtPassBreak(), ostk::core::error::runtime::Undefined);
    }

    {
        EXPECT_EQ(defaultPass_.accessInstantAtPassBreak(), defaultInstantAtPassBreak_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, Print)
{
    {
        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(defaultPass_.print(std::cout, true));
        EXPECT_NO_THROW(defaultPass_.print(std::cout, false));
        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
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
