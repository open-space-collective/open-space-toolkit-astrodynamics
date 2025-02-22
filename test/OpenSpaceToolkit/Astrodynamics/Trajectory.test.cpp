/// Apache License 2.0

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::spherical::LLA;
using ostk::physics::coordinate::Velocity;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;

using ostk::astrodynamics::Trajectory;
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
    const Earth earth = Earth::WGS84();
    const LLA startLLA = LLA::Vector({0.0, 0.0, 0.0});
    const LLA endLLA = LLA::Vector({1.0, 0.0, 0.0});

    {
        const Array<Instant> instants = {Instant::J2000(), Instant::J2000() + Duration::Seconds(10.0)};

        {
            {
                EXPECT_THROW(
                    Trajectory::GroundStrip(LLA::Undefined(), endLLA, instants, earth), ostk::core::error::RuntimeError
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

        {
            const Trajectory trajectory = Trajectory::GroundStrip(startLLA, endLLA, instants, earth);

            {
                const State state = trajectory.getStateAt(instants.accessFirst()).inFrame(Frame::ITRF());

                EXPECT_TRUE(state.getVelocity().getCoordinates().isNear({0.0, 0.0, 0.0}, 1e-12));

                EXPECT_TRUE(LLA::Cartesian(
                                state.getPosition().getCoordinates(), earth.getEquatorialRadius(), earth.getFlattening()
                )
                                .toVector()
                                .isNear(startLLA.toVector(), 1e-12));
            }

            {
                const State state = trajectory.getStateAt(instants.accessLast()).inFrame(Frame::ITRF());

                EXPECT_TRUE(state.getVelocity().getCoordinates().isNear({0.0, 0.0, 0.0}, 1e-12));

                EXPECT_TRUE(LLA::Cartesian(
                                state.getPosition().getCoordinates(), earth.getEquatorialRadius(), earth.getFlattening()
                )
                                .toVector()
                                .isNear(endLLA.toVector(), 1e-12));
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
                    Trajectory::GroundStrip(LLA::Vector({0.0, 0.0, 1e-1}), endLLA, groundSpeed, startInstant, earth),
                    ostk::core::error::RuntimeError
                );
            }

            {
                EXPECT_THROW(
                    Trajectory::GroundStrip(startLLA, LLA::Vector({0.0, 0.0, 1e-1}), groundSpeed, startInstant, earth),
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
                const Trajectory trajectory =
                    Trajectory::GroundStrip(startLLA, endLLA, groundSpeed, startInstant, earth, Duration::Seconds(1.0));

                EXPECT_TRUE(trajectory.isDefined());
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
            const Trajectory trajectory = Trajectory::GroundStrip(startLLA, endLLA, groundSpeed, startInstant, earth);

            {
                const State state = trajectory.getStateAt(startInstant).inFrame(Frame::ITRF());

                EXPECT_TRUE(state.getVelocity().getCoordinates().isNear({0.0, 0.0, 0.0}, 1e-13));

                EXPECT_TRUE(LLA::Cartesian(
                                state.getPosition().getCoordinates(), earth.getEquatorialRadius(), earth.getFlattening()
                )
                                .toVector()
                                .isNear(startLLA.toVector(), 1e-12));
            }

            {
                const Length distance =
                    startLLA.calculateDistanceTo(endLLA, earth.getEquatorialRadius(), earth.getFlattening());
                const Duration duration =
                    Duration::Seconds(distance.inMeters() / groundSpeed.in(Derived::Unit::MeterPerSecond()));
                const State state = trajectory.getStateAt(startInstant + duration).inFrame(Frame::ITRF());

                EXPECT_TRUE(state.getVelocity().getCoordinates().isNear({0.0, 0.0, 0.0}, 1e-13));

                EXPECT_TRUE(LLA::Cartesian(
                                state.getPosition().getCoordinates(), earth.getEquatorialRadius(), earth.getFlattening()
                )
                                .toVector()
                                .isNear(endLLA.toVector(), 1e-8));
            }
        }
    }
}
