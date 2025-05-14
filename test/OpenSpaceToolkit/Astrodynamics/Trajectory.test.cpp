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
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Kepler/COE.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::mathematics::object::Vector3d;

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
using ostk::astrodynamics::trajectory::model::Tabulated;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::model::Kepler;
using ostk::astrodynamics::trajectory::orbit::model::kepler::COE;
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

        const Tabulated model = {states};

        const Trajectory trajectory = {model};

        EXPECT_TRUE(trajectory.isDefined());
    }

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

        const Trajectory trajectory = {states};

        EXPECT_TRUE(trajectory.isDefined());
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

        const Tabulated model = {states};

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

        const Tabulated model_A = {states_A};

        const Trajectory trajectory_A = {model_A};

        const Array<State> states_B = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model_B = {states_B};

        const Trajectory trajectory_B = {model_B};

        EXPECT_FALSE(trajectory_A == trajectory_B);

        EXPECT_FALSE(trajectory_A == Trajectory::Undefined());
        EXPECT_FALSE(Trajectory::Undefined() == trajectory_B);
    }

    {
        EXPECT_FALSE(Trajectory::Undefined() == Trajectory::Undefined());
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

        const Tabulated model = {states};

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

        const Tabulated model_A = {states_A};

        const Trajectory trajectory_A = {model_A};

        const Array<State> states_B = {
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Position::Meters({0.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)},
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC),
             Position::Meters({1.0, 0.0, 0.0}, gcrfSPtr),
             Velocity::MetersPerSecond({0.0, 0.0, 0.0}, gcrfSPtr)}
        };

        const Tabulated model_B = {states_B};

        const Trajectory trajectory_B = {model_B};

        EXPECT_TRUE(trajectory_A != trajectory_B);

        EXPECT_TRUE(trajectory_A != Trajectory::Undefined());
        EXPECT_TRUE(Trajectory::Undefined() != trajectory_B);
    }

    {
        EXPECT_TRUE(Trajectory::Undefined() != Trajectory::Undefined());
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

        const Tabulated model = {states};

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

        const Tabulated model = {states};

        const Trajectory trajectory = {model};

        EXPECT_TRUE(trajectory.isDefined());
    }

    {
        EXPECT_FALSE(Trajectory::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, AccessModel)
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

        const Tabulated model = {states};

        const Trajectory trajectory = {model};

        EXPECT_TRUE(trajectory.accessModel().is<Tabulated>());
    }

    {
        EXPECT_ANY_THROW(Trajectory::Undefined().accessModel());
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

        const Tabulated model = {states};

        const Trajectory trajectory = {model};

        EXPECT_EQ(states.at(0), trajectory.getStateAt(states.at(0).accessInstant()));
        EXPECT_EQ(states.at(1), trajectory.getStateAt(states.at(1).accessInstant()));

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

        EXPECT_EQ(states.at(0), trajectory.getStateAt(states.at(0).accessInstant()));
        EXPECT_EQ(states.at(1), trajectory.getStateAt(states.at(1).accessInstant()));

        EXPECT_EQ(
            State(
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                Position::Meters({0.5, 0.0, 0.0}, gcrfSPtr),
                Velocity::MetersPerSecond({1.0, 0.0, 0.0}, gcrfSPtr)
            ),
            trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC))
        );
    }

    {
        EXPECT_ANY_THROW(Trajectory::Undefined().getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC)
        ));
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

        const Tabulated model = {states};

        const Trajectory trajectory = {model};

        {
            const Array<Instant> referenceInstants = {
                states.at(0).accessInstant(),
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                states.at(1).accessInstant()
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
                states.at(0).accessInstant(),
                states.at(1).accessInstant(),
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

        const Tabulated model = {states};

        const Trajectory trajectory = {model};

        {
            const Array<Instant> desiredInstants = {
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                states.at(1).accessInstant(),
                states.at(0).accessInstant(),
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

    {
        EXPECT_ANY_THROW(Trajectory::Undefined().getStatesAt(
            {Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC),
             Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC)}
        ));
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

        const Tabulated model = {states};

        const Trajectory trajectory = {model};

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(trajectory.print(std::cout, true));
        EXPECT_NO_THROW(trajectory.print(std::cout, false));

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, Undefined)
{
    {
        EXPECT_NO_THROW(Trajectory::Undefined());
        EXPECT_FALSE(Trajectory::Undefined().isDefined());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, Position)
{
    {
        const Trajectory trajectory = Trajectory::Position(Position::Meters({2.0, 0.0, 0.0}, Frame::ITRF()));
        EXPECT_TRUE(trajectory.isDefined());
    }

    {
        EXPECT_THROW(
            Trajectory::Position(Position::Meters({2.0, 0.0, 0.0}, Frame::GCRF())), ostk::core::error::runtime::Wrong
        );
    }

    {
        const Trajectory trajectory = Trajectory::Position(Position::Meters({2.0, 0.0, 0.0}, Frame::ITRF()));
        const State state = trajectory.getStateAt(Instant::J2000());
        EXPECT_TRUE(state.accessFrame() == Frame::GCRF());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, GroundStrip)
{
    {
        const Earth earth = Earth::WGS84();
        const LLA startLLA = LLA::Vector({0.0, 0.0, 0.0});
        const LLA endLLA = LLA::Vector({0.0, 1.0, 0.0});
        {
            const Array<Instant> instants = {Instant::J2000(), Instant::J2000() + Duration::Seconds(10.0)};

            {
                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(LLA::Undefined(), endLLA, instants, earth),
                        ostk::core::error::RuntimeError
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(startLLA, LLA::Undefined(), instants, earth),
                        ostk::core::error::RuntimeError
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(LLA::Vector({0.0, 0.0, 1e-1}), endLLA, instants, earth),
                        ostk::core::error::RuntimeError
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(startLLA, LLA::Vector({0.0, 0.0, 1e-1}), instants, earth),
                        ostk::core::error::RuntimeError
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(startLLA, endLLA, {Instant::J2000()}, earth),
                        ostk::core::error::RuntimeError
                    );
                }
            }

            {
                {
                    const Trajectory trajectory = Trajectory::GroundStrip(startLLA, endLLA, instants, earth);

                    EXPECT_TRUE(trajectory.isDefined());
                }

                {
                    const Trajectory trajectory = Trajectory::GroundStrip(startLLA, endLLA, instants);

                    EXPECT_TRUE(trajectory.isDefined());
                }
            }
        }

        {
            const Derived groundSpeed = Derived(7000.0, Derived::Unit::MeterPerSecond());
            const Instant startInstant = Instant::J2000();

            {
                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(LLA::Undefined(), endLLA, groundSpeed, startInstant, earth),
                        ostk::core::error::RuntimeError
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(startLLA, LLA::Undefined(), groundSpeed, startInstant, earth),
                        ostk::core::error::RuntimeError
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(
                            LLA::Vector({0.0, 0.0, 1e-1}), endLLA, groundSpeed, startInstant, earth
                        ),
                        ostk::core::error::RuntimeError
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(
                            startLLA, LLA::Vector({0.0, 0.0, 1e-1}), groundSpeed, startInstant, earth
                        ),
                        ostk::core::error::RuntimeError
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(startLLA, endLLA, Derived::Undefined(), startInstant, earth),
                        ostk::core::error::runtime::Undefined
                    );
                }

                {
                    EXPECT_THROW(
                        Trajectory::GroundStrip(startLLA, endLLA, groundSpeed, Instant::Undefined(), earth),
                        ostk::core::error::runtime::Undefined
                    );
                }
            }

            {
                {
                    const Trajectory trajectory = Trajectory::GroundStrip(
                        startLLA, endLLA, groundSpeed, startInstant, earth, Duration::Seconds(1.0)
                    );

                    EXPECT_TRUE(trajectory.isDefined());

                    // Speed in ITRF frame should be approximately equal to ground speed
                    EXPECT_NEAR(
                        trajectory.getStateAt(startInstant)
                            .inFrame(Frame::ITRF())
                            .getVelocity()
                            .getCoordinates()
                            .norm(),
                        groundSpeed.in(Derived::Unit::MeterPerSecond()),
                        1.0
                    );
                }

                {
                    const Trajectory trajectory =
                        Trajectory::GroundStrip(startLLA, endLLA, groundSpeed, startInstant, earth);

                    EXPECT_TRUE(trajectory.isDefined());
                }

                {
                    const Trajectory trajectory = Trajectory::GroundStrip(startLLA, endLLA, groundSpeed, startInstant);

                    EXPECT_TRUE(trajectory.isDefined());
                }
            }

            {
                const Trajectory trajectory =
                    Trajectory::GroundStrip(startLLA, endLLA, groundSpeed, startInstant, earth);

                {
                    const State state = trajectory.getStateAt(startInstant).inFrame(Frame::ITRF());

                    EXPECT_TRUE(
                        LLA::Cartesian(
                            state.getPosition().getCoordinates(), earth.getEquatorialRadius(), earth.getFlattening()
                        )
                            .toVector()
                            .isNear(startLLA.toVector(), 1e-12)
                    );
                }

                {
                    const Length distance =
                        startLLA.calculateDistanceTo(endLLA, earth.getEquatorialRadius(), earth.getFlattening());
                    const Duration duration =
                        Duration::Seconds(distance.inMeters() / groundSpeed.in(Derived::Unit::MeterPerSecond()));
                    const State state = trajectory.getStateAt(startInstant + duration).inFrame(Frame::ITRF());

                    EXPECT_TRUE(
                        LLA::Cartesian(
                            state.getPosition().getCoordinates(), earth.getEquatorialRadius(), earth.getFlattening()
                        )
                            .toVector()
                            .isNear(endLLA.toVector(), 1e-8)
                    );
                }
            }
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

        const Trajectory trajectory = Trajectory::GroundStrip(startLLA, endLLA, instants);

        for (const auto& state : trajectory.getStatesAt(instants))
        {
            const Vector3d velocity = state.getVelocity().getCoordinates();
            EXPECT_NEAR(velocity.norm(), 970.3, 1.0);
        }
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory, GeodeticNadirGroundTrack)
{
    {
        EXPECT_THROW(
            Trajectory::GeodeticNadirGroundTrack(Orbit::Undefined()), ostk::core::error::RuntimeError
        );
    }

    {
        const Earth earth = Earth::WGS84();

        const Instant instant = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC);

        const Environment environment = Environment::Default(true);
    
        const Shared<Earth> earthSPtr = std::make_shared<Earth>(earth);
    
        const Length semiMajorAxis = Length::Kilometers(7000.0);
        const Real eccentricity = 0.0;
        const Angle inclination = Angle::Degrees(98.0);
        const Angle raan = Angle::Degrees(0.0);
        const Angle aop = Angle::Degrees(0.0);
        const Angle trueAnomaly = Angle::Degrees(0.0);
    
        const COE coe = {semiMajorAxis, eccentricity, inclination, raan, aop, trueAnomaly};
    
        const Derived gravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_;
        const Length equatorialRadius = EarthGravitationalModel::EGM2008.equatorialRadius_;
        const Real J2 = EarthGravitationalModel::EGM2008.J2_;
        const Real J4 = EarthGravitationalModel::EGM2008.J4_;
    
        const Kepler keplerianModel = {
            coe, instant, gravitationalParameter, equatorialRadius, J2, J4, Kepler::PerturbationType::None
        };
    
        const Orbit orbit = {keplerianModel, environment.accessCelestialObjectWithName("Earth")};

        EXPECT_NO_THROW(
            Trajectory::GeodeticNadirGroundTrack(orbit)
        );
    }
}
