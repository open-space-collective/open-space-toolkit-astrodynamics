/// Apache License 2.0

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Solver/TemporalConditionSolver.hpp>

using ostk::core::container::Array;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::solver::TemporalConditionSolver;

class OpenSpaceToolkit_Astrodynamics_Solvers_TemporalConditionSolver : public ::testing::Test
{
   protected:
    const Duration defaultTimeStep_ = Duration::Seconds(30.0);
    const Duration defaultTolerance_ = Duration::Milliseconds(1.0);
    const Size defaultMaximumIterationCount_ = 1234;
    const TemporalConditionSolver defaultSolver_ = {
        defaultTimeStep_,
        defaultTolerance_,
        defaultMaximumIterationCount_,
    };
    const Interval defaultInterval_ = Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Hours(2.0));
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_TemporalConditionSolver, Constructor)
{
    {
        EXPECT_NO_THROW(
            TemporalConditionSolver solver(defaultTimeStep_, defaultTolerance_, defaultMaximumIterationCount_);
        );
    }

    {
        EXPECT_NO_THROW(TemporalConditionSolver solver(defaultTimeStep_, defaultTolerance_););
    }

    {
        EXPECT_THROW(
            try {
                TemporalConditionSolver solver(Duration::Undefined(), defaultTolerance_);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Time step} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                TemporalConditionSolver solver(Duration::Zero(), defaultTolerance_);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    String::Format("Time step [{}] must be strictly positive.", Duration::Zero().toString()),
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );

        EXPECT_THROW(
            try {
                TemporalConditionSolver solver(Duration::Seconds(-30.0), defaultTolerance_);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    String::Format("Time step [{}] must be strictly positive.", Duration::Seconds(-30.0).toString()),
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            try {
                TemporalConditionSolver solver(defaultTimeStep_, Duration::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Tolerance} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );

        EXPECT_THROW(
            try {
                TemporalConditionSolver solver(defaultTimeStep_, Duration::Zero());
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    String::Format("Tolerance [{}] must be strictly positive.", Duration::Zero().toString()),
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );

        EXPECT_THROW(
            try {
                TemporalConditionSolver solver(defaultTimeStep_, Duration::Milliseconds(-1.0));
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    String::Format(
                        "Tolerance [{}] must be strictly positive.", Duration::Milliseconds(-1.0).toString()
                    ),
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            try {
                TemporalConditionSolver solver(Duration::Seconds(30.0), Duration::Seconds(30.0));
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    String::Format(
                        "Tolerance [{}] must be smaller than time step [{}].",
                        Duration::Seconds(30.0).toString(),
                        Duration::Seconds(30.0).toString()
                    ),
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );

        EXPECT_THROW(
            TemporalConditionSolver solver(Duration::Seconds(30.0), Duration::Minutes(1.0)),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_TemporalConditionSolver, Getters)
{
    {
        EXPECT_EQ(defaultTimeStep_, defaultSolver_.getTimeStep());
        EXPECT_EQ(defaultTolerance_, defaultSolver_.getTolerance());
        EXPECT_EQ(defaultMaximumIterationCount_, defaultSolver_.getMaximumIterationCount());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Solvers_TemporalConditionSolver, Solve)
{
    // Condition always met: the whole interval is returned
    {
        const Array<Interval> intervals = defaultSolver_.solve(
            [](const Instant&) -> bool
            {
                return true;
            },
            defaultInterval_
        );

        EXPECT_EQ(Array<Interval>({defaultInterval_}), intervals);
    }

    // Condition never met: no interval is returned
    {
        const Array<Interval> intervals = defaultSolver_.solve(
            [](const Instant&) -> bool
            {
                return false;
            },
            defaultInterval_
        );

        EXPECT_TRUE(intervals.isEmpty());
    }

    // Windows longer than the time step: switching instants are found within tolerance
    {
        const Array<Interval> expectedIntervals = {
            Interval::Closed(
                Instant::J2000() + Duration::Minutes(10.0) + Duration::Seconds(7.0),
                Instant::J2000() + Duration::Minutes(20.0) + Duration::Seconds(3.0)
            ),
            Interval::Closed(
                Instant::J2000() + Duration::Minutes(40.0) + Duration::Seconds(11.0),
                Instant::J2000() + Duration::Minutes(41.0) + Duration::Seconds(13.0)
            ),
        };

        const Array<Interval> intervals = defaultSolver_.solve(
            [&expectedIntervals](const Instant& anInstant) -> bool
            {
                return expectedIntervals[0].contains(anInstant) || expectedIntervals[1].contains(anInstant);
            },
            defaultInterval_
        );

        ASSERT_EQ(expectedIntervals.getSize(), intervals.getSize());

        for (Size i = 0; i < intervals.getSize(); ++i)
        {
            EXPECT_TRUE(intervals[i].getStart().isNear(expectedIntervals[i].getStart(), defaultTolerance_));
            EXPECT_TRUE(intervals[i].getEnd().isNear(expectedIntervals[i].getEnd(), defaultTolerance_));
        }
    }

    // Window shorter than the time step and falling between grid points: the window is missed
    {
        const Interval shortWindow = Interval::Closed(
            Instant::J2000() + Duration::Minutes(10.0) + Duration::Seconds(5.0),
            Instant::J2000() + Duration::Minutes(10.0) + Duration::Seconds(15.0)
        );

        const Array<Interval> intervals = defaultSolver_.solve(
            [&shortWindow](const Instant& anInstant) -> bool
            {
                return shortWindow.contains(anInstant);
            },
            defaultInterval_
        );

        EXPECT_TRUE(intervals.isEmpty());
    }

    // Multiple conditions, one never met: no interval is returned
    {
        const Array<Interval> intervals = defaultSolver_.solve(
            Array<TemporalConditionSolver::Condition>({
                [](const Instant&) -> bool
                {
                    return true;
                },
                [](const Instant&) -> bool
                {
                    return false;
                },
            }),
            defaultInterval_
        );

        EXPECT_TRUE(intervals.isEmpty());
    }

    // Interval duration equal to the time step: allowed
    {
        const Interval intervalEqualToTimeStep =
            Interval::Closed(Instant::J2000(), Instant::J2000() + defaultTimeStep_);

        const Array<Interval> intervals = defaultSolver_.solve(
            [](const Instant&) -> bool
            {
                return true;
            },
            intervalEqualToTimeStep
        );

        EXPECT_EQ(Array<Interval>({intervalEqualToTimeStep}), intervals);
    }

    // Interval duration shorter than the time step: warns but does not throw
    {
        const Interval shortInterval = Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Seconds(10.0));

        Array<Interval> intervals = Array<Interval>::Empty();

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(
            intervals = defaultSolver_.solve(
                [](const Instant&) -> bool
                {
                    return true;
                },
                shortInterval
            )
        );

        const std::string output = testing::internal::GetCapturedStdout();

        EXPECT_NE(
            std::string::npos,
            output.find(String::Format(
                "Time step [{}] is larger than interval duration [{}]: condition switches within the interval may be "
                "missed.",
                defaultTimeStep_.toString(),
                Duration::Seconds(10.0).toString()
            ))
        ) << output;

        EXPECT_EQ(Array<Interval>({shortInterval}), intervals);
    }

    // Interval duration shorter than the time step, condition never met: no interval is returned
    {
        const Interval shortInterval = Interval::Closed(Instant::J2000(), Instant::J2000() + Duration::Seconds(10.0));

        testing::internal::CaptureStdout();

        const Array<Interval> intervals = defaultSolver_.solve(
            [](const Instant&) -> bool
            {
                return false;
            },
            shortInterval
        );

        const std::string output = testing::internal::GetCapturedStdout();

        EXPECT_NE(
            std::string::npos,
            output.find(String::Format(
                "Time step [{}] is larger than interval duration [{}]: condition switches within the interval may be "
                "missed.",
                defaultTimeStep_.toString(),
                Duration::Seconds(10.0).toString()
            ))
        ) << output;

        EXPECT_TRUE(intervals.isEmpty());
    }

    // Undefined interval: throws
    {
        EXPECT_THROW(
            defaultSolver_.solve(
                [](const Instant&) -> bool
                {
                    return true;
                },
                Interval::Undefined()
            ),
            ostk::core::error::runtime::Undefined
        );
    }
}
