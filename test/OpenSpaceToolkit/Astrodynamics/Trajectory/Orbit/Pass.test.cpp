// pass.test.cpp

#include <optional>

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Pass.hpp>

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::trajectory::orbit::Pass;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass : public ::testing::Test
{
   protected:
    const int defaultRevolutionNumber_ = 1;
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
                    std::nullopt,
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
                    std::nullopt,
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
                    std::nullopt,
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
                    std::nullopt,
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
                    std::nullopt
                )
                    .getType(),
                Pass::Type::Partial
            );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, ConstructorWithOptionalRevolutionNumber)
{
    // No revolution number and no instant: partial, and never equal to another pass
    {
        const Pass pass = {
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
        };

        EXPECT_EQ(Pass::Type::Partial, pass.getType());
        EXPECT_FALSE(pass.getRevolutionNumber().has_value());
        EXPECT_FALSE(pass.getStartInstant().has_value());
        EXPECT_FALSE(pass.getEndInstant().has_value());
        EXPECT_FALSE(pass == pass);
    }

    // Revolution number and no instant: partial
    {
        const Pass pass = {
            defaultRevolutionNumber_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
        };

        EXPECT_EQ(Pass::Type::Partial, pass.getType());
        EXPECT_EQ(std::optional<int>(defaultRevolutionNumber_), pass.getRevolutionNumber());
        EXPECT_FALSE(pass.getStartInstant().has_value());
        EXPECT_FALSE(pass.getEndInstant().has_value());
    }

    // Revolution number and some instants: partial
    {
        const Pass pass = {
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
        };

        EXPECT_EQ(Pass::Type::Partial, pass.getType());
        EXPECT_EQ(std::optional<int>(defaultRevolutionNumber_), pass.getRevolutionNumber());
    }

    // Revolution number and all instants: complete
    {
        EXPECT_EQ(Pass::Type::Complete, defaultPass_.getType());
        EXPECT_EQ(std::optional<int>(defaultRevolutionNumber_), defaultPass_.getRevolutionNumber());
    }

    // No revolution number and all instants: complete, without a revolution number
    {
        const Pass pass = {
            std::nullopt,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            defaultInstantAtSouthPoint_,
            defaultInstantAtPassBreak_,
        };

        EXPECT_EQ(Pass::Type::Complete, pass.getType());
        EXPECT_FALSE(pass.getRevolutionNumber().has_value());

        // A pass without a revolution number is never equal to another pass
        EXPECT_FALSE(pass == pass);
        EXPECT_TRUE(pass != pass);
        EXPECT_FALSE(pass == defaultPass_);
        EXPECT_FALSE(defaultPass_ == pass);
    }

    // No revolution number and some instants: partial, without a revolution number
    {
        const Pass pass = {
            std::nullopt,
            defaultInstantAtAscendingNode_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
        };

        EXPECT_EQ(Pass::Type::Partial, pass.getType());
        EXPECT_FALSE(pass.getRevolutionNumber().has_value());
    }

    // Implicit conversion from int
    {
        const Pass pass = {
            2,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            defaultInstantAtSouthPoint_,
            defaultInstantAtPassBreak_,
        };

        EXPECT_EQ(std::optional<int>(2), pass.getRevolutionNumber());
        EXPECT_FALSE(pass == defaultPass_);
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
                std::nullopt,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                std::nullopt,
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
                std::nullopt,
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
                std::nullopt,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                std::nullopt,
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
                std::nullopt,
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
                std::nullopt,
                defaultInstantAtSouthPoint_,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                std::nullopt,
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
                std::nullopt,
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
                std::nullopt,
                defaultInstantAtPassBreak_,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                std::nullopt,
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
                std::nullopt,
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
                std::nullopt,
            };

            const Pass pass_2 = {
                defaultRevolutionNumber_,
                defaultInstantAtAscendingNode_,
                defaultInstantAtNorthPoint_,
                defaultInstantAtDescendingNode_,
                defaultInstantAtSouthPoint_,
                std::nullopt,
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
                std::nullopt,
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

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, IsComplete)
{
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
            std::nullopt,
        };
        EXPECT_FALSE(pass.isComplete());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetType)
{
    {
        EXPECT_EQ(defaultPass_.getType(), Pass::Type::Complete);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetRevolutionNumber)
{
    {
        EXPECT_EQ(defaultPass_.getRevolutionNumber(), defaultRevolutionNumber_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetDuration)
{
    {
        EXPECT_TRUE(defaultPass_.getDuration().has_value());
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            std::nullopt,
            defaultInstantAtPassBreak_,
        };
        EXPECT_FALSE(pass.getDuration().has_value());
    }

    {
        EXPECT_EQ(defaultPass_.getDuration().value(), defaultInstantAtPassBreak_ - defaultInstantAtAscendingNode_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetStartInstant)
{
    {
        EXPECT_EQ(defaultPass_.getStartInstant().value(), defaultInstantAtAscendingNode_);
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            std::nullopt,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            defaultInstantAtSouthPoint_,
            defaultInstantAtPassBreak_,
        };
        EXPECT_EQ(pass.getStartInstant().value(), defaultInstantAtNorthPoint_);
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            std::nullopt,
            std::nullopt,
            defaultInstantAtDescendingNode_,
            defaultInstantAtSouthPoint_,
            defaultInstantAtPassBreak_,
        };
        EXPECT_EQ(pass.getStartInstant().value(), defaultInstantAtDescendingNode_);
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            defaultInstantAtSouthPoint_,
            defaultInstantAtPassBreak_,
        };
        EXPECT_EQ(pass.getStartInstant().value(), defaultInstantAtSouthPoint_);
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            defaultInstantAtPassBreak_,
        };
        EXPECT_EQ(pass.getStartInstant().value(), defaultInstantAtPassBreak_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetEndInstant)
{
    {
        EXPECT_EQ(defaultPass_.getEndInstant().value(), defaultInstantAtPassBreak_);
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            defaultInstantAtSouthPoint_,
            std::nullopt,
        };
        EXPECT_EQ(pass.getEndInstant().value(), defaultInstantAtSouthPoint_);
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            std::nullopt,
            std::nullopt,
        };
        EXPECT_EQ(pass.getEndInstant().value(), defaultInstantAtDescendingNode_);
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
        };
        EXPECT_EQ(pass.getEndInstant().value(), defaultInstantAtNorthPoint_);
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
        };
        EXPECT_EQ(pass.getEndInstant().value(), defaultInstantAtAscendingNode_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetInterval)
{
    {
        EXPECT_TRUE(defaultPass_.getInterval().has_value());
    }

    {
        const Pass pass = {
            defaultRevolutionNumber_,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            std::nullopt,
            defaultInstantAtPassBreak_,
        };
        EXPECT_FALSE(pass.getInterval().has_value());
    }

    {
        EXPECT_EQ(
            defaultPass_.getInterval().value(),
            Interval::Closed(defaultInstantAtAscendingNode_, defaultInstantAtPassBreak_)
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetInstantAtAscendingNode)
{
    {
        EXPECT_EQ(defaultPass_.getInstantAtAscendingNode().value(), defaultInstantAtAscendingNode_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetInstantAtNorthPoint)
{
    {
        EXPECT_EQ(defaultPass_.getInstantAtNorthPoint().value(), defaultInstantAtNorthPoint_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetInstantAtDescendingNode)
{
    {
        EXPECT_EQ(defaultPass_.getInstantAtDescendingNode().value(), defaultInstantAtDescendingNode_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetInstantAtSouthPoint)
{
    {
        EXPECT_EQ(defaultPass_.getInstantAtSouthPoint().value(), defaultInstantAtSouthPoint_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, GetInstantAtPassBreak)
{
    {
        EXPECT_EQ(defaultPass_.getInstantAtPassBreak().value(), defaultInstantAtPassBreak_);
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

    {
        const Pass pass = {
            std::nullopt,
            defaultInstantAtAscendingNode_,
            defaultInstantAtNorthPoint_,
            defaultInstantAtDescendingNode_,
            defaultInstantAtSouthPoint_,
            defaultInstantAtPassBreak_,
        };

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(pass.print(std::cout, true));

        const std::string output = testing::internal::GetCapturedStdout();

        EXPECT_NE(std::string::npos, output.find("Undefined"));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, StringFromType)
{
    EXPECT_EQ(Pass::StringFromType(Pass::Type::Complete), "Complete");
    EXPECT_EQ(Pass::StringFromType(Pass::Type::Partial), "Partial");
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Pass, StringFromPhase)
{
    EXPECT_EQ(Pass::StringFromPhase(Pass::Phase::Ascending), "Ascending");
    EXPECT_EQ(Pass::StringFromPhase(Pass::Phase::Descending), "Descending");
}
