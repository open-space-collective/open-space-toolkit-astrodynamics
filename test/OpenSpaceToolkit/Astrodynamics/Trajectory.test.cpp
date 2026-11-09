/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/LLA.hpp>
#include <OpenSpaceToolkit/Physics/Data/Direction.hpp>
#include <OpenSpaceToolkit/Physics/Data/Provider/Nadir.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Static.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::Velocity;
using ostk::physics::data::Direction;
using ostk::physics::data::provider::Nadir;
using ostk::physics::Environment;
using ostk::physics::environment::object::celestial::Earth;

using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::Trajectory;
using ostk::astrodynamics::trajectory::model::Static;
using ostk::astrodynamics::trajectory::model::Tabulated;
using ostk::astrodynamics::trajectory::State;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, Constructor)
{
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        EXPECT_TRUE(trajectory.getModel().is<Tabulated>());
    }

    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC),
             Position::Meters({2.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 3), Scale::UTC),
             Position::Meters({3.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        // The default (barycentric rational) interpolation requires at least 4 states.
        const Trajectory trajectory = {states};

        EXPECT_TRUE(trajectory.getModel().is<Tabulated>());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, EqualToOperator)
{
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        EXPECT_TRUE(trajectory == trajectory);
    }

    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states_A = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model_A = {states_A, Interpolator::Type::Linear};

        const Trajectory trajectory_A = {model_A};

        const Array<State> states_B = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model_B = {states_B, Interpolator::Type::Linear};

        const Trajectory trajectory_B = {model_B};

        EXPECT_FALSE(trajectory_A == trajectory_B);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, NotEqualToOperator)
{
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        EXPECT_FALSE(trajectory != trajectory);
    }

    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states_A = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model_A = {states_A, Interpolator::Type::Linear};

        const Trajectory trajectory_A = {model_A};

        const Array<State> states_B = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model_B = {states_B, Interpolator::Type::Linear};

        const Trajectory trajectory_B = {model_B};

        EXPECT_TRUE(trajectory_A != trajectory_B);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, StreamOperator)
{
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << trajectory << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, IsDefined)
{
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        EXPECT_TRUE(trajectory.getModel().is<Tabulated>());
    }

    {
        EXPECT_THROW(Trajectory(Array<State>::Empty()), ostk::core::error::runtime::Wrong);
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, GetModel)
{
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        EXPECT_TRUE(trajectory.getModel().is<Tabulated>());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, GetStateAt)
{
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        EXPECT_EQ(states.at(0), trajectory.getStateAt(states.at(0).getInstant()));
        EXPECT_EQ(states.at(1), trajectory.getStateAt(states.at(1).getInstant()));

        EXPECT_EQ(
            State(
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                Position::Meters({0.5, 0.0, 0.0}, gcrfSPtr),
                Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)
            ),
            trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC))
        );

        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2017, 12, 31, 23, 59, 59), Scale::UTC)));
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2017, 12, 31, 23, 59, 58), Scale::UTC)));
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2017, 12, 31, 23, 59, 59), Scale::UTC)));
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC)));
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 3), Scale::UTC)));
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC)));

        EXPECT_EQ(states.at(0), trajectory.getStateAt(states.at(0).getInstant()));
        EXPECT_EQ(states.at(1), trajectory.getStateAt(states.at(1).getInstant()));

        EXPECT_EQ(
            State(
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                Position::Meters({0.5, 0.0, 0.0}, gcrfSPtr),
                Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)
            ),
            trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC))
        );
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, GetStatesAt)
{
    // Test correct handling of state array dimensions
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        {
            const Array<Instant> referenceInstants = {
                states.at(0).getInstant(),
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                states.at(1).getInstant()
            };

            const Array<State> referenceStates = {
                {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
                 Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
                 Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
                {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                 Position::Meters({0.5, 0.0, 0.0}, gcrfSPtr),
                 Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
                {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
                 Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
                 Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
            };

            EXPECT_EQ(referenceStates, trajectory.getStatesAt(referenceInstants));
        }

        {
            const Array<Instant> referenceInstants = {
                Instant::DateTime(DateTime(2017, 12, 31, 23, 59, 59), Scale::UTC),
                states.at(0).getInstant(),
                states.at(1).getInstant(),
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC)
            };

            EXPECT_ANY_THROW(trajectory.getStatesAt(referenceInstants));
        }
    }

    // Test correct handling of state array sorting and unsorting
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC),
             Position::Meters({2.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},

        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        {
            const Array<Instant> desiredInstants = {
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                states.at(1).getInstant(),
                states.at(0).getInstant(),
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1, 500), Scale::UTC),
            };

            const Array<State> referenceStates = {
                {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                 Position::Meters({0.5, 0.0, 0.0}, gcrfSPtr),
                 Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
                {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC),
                 Position::Meters({2.0, 0.0, 0.0}, gcrfSPtr),
                 Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
                {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
                 Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
                 Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
                {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1, 500), Scale::UTC),
                 Position::Meters({1.5, 0.0, 0.0}, gcrfSPtr),
                 Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
            };

            EXPECT_EQ(referenceStates, trajectory.getStatesAt(desiredInstants));
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, Print)
{
    {
        const Shared<const Frame> gcrfSPtr = Frame::GCRF();

        const Array<State> states = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model = {states, Interpolator::Type::Linear};

        const Trajectory trajectory = {model};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(trajectory.print(std::cout, true));
        EXPECT_NO_THROW(trajectory.print(std::cout, false));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, Position)
{
    {
        const Trajectory trajectory = Trajectory::Position(Position::Meters({2.0, 0.0, 0.0}, Frame::ITRF()));
        EXPECT_TRUE(trajectory.getModel().is<Static>());
    }

    {
        EXPECT_THROW(
            Trajectory::Position(Position::Meters({2.0, 0.0, 0.0}, Frame::GCRF())), ostk::core::error::runtime::Wrong
        );
    }

    {
        const Trajectory trajectory = Trajectory::Position(Position::Meters({2.0, 0.0, 0.0}, Frame::ITRF()));
        const State state = trajectory.getStateAt(Instant::J2000());
        EXPECT_TRUE(state.getFrame() == Frame::ITRF());
        EXPECT_EQ(state.getInstant(), Instant::J2000());
        EXPECT_EQ(state.getPosition(), Position::Meters({2.0, 0.0, 0.0}, Frame::ITRF()));
        EXPECT_EQ(state.getVelocity(), Velocity::MetersPerSecond({0.0, 0.0, 0.0}, Frame::ITRF()));
    }
}
