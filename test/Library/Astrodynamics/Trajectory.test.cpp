////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        Apache License 2.0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory/Models/Tabulated.hpp>
#include <Library/Astrodynamics/Trajectory.hpp>

#include <Library/Physics/Coordinate/Frame.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Trajectory, Constructor)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        EXPECT_TRUE(trajectory.isDefined()) ;

    }

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Trajectory trajectory = { states } ;

        EXPECT_TRUE(trajectory.isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, EqualToOperator)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        EXPECT_TRUE(trajectory == trajectory) ;

    }

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states_A =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model_A = { states_A } ;

        const Trajectory trajectory_A = { model_A } ;

        const Array<State> states_B =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model_B = { states_B } ;

        const Trajectory trajectory_B = { model_B } ;

        EXPECT_FALSE(trajectory_A == trajectory_B) ;

        EXPECT_FALSE(trajectory_A == Trajectory::Undefined()) ;
        EXPECT_FALSE(Trajectory::Undefined() == trajectory_B) ;

    }

    {

        EXPECT_FALSE(Trajectory::Undefined() == Trajectory::Undefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, NotEqualToOperator)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        EXPECT_FALSE(trajectory != trajectory) ;

    }

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states_A =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model_A = { states_A } ;

        const Trajectory trajectory_A = { model_A } ;

        const Array<State> states_B =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 0.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model_B = { states_B } ;

        const Trajectory trajectory_B = { model_B } ;

        EXPECT_TRUE(trajectory_A != trajectory_B) ;

        EXPECT_TRUE(trajectory_A != Trajectory::Undefined()) ;
        EXPECT_TRUE(Trajectory::Undefined() != trajectory_B) ;

    }

    {

        EXPECT_TRUE(Trajectory::Undefined() != Trajectory::Undefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, StreamOperator)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << trajectory << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, IsDefined)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        EXPECT_TRUE(trajectory.isDefined()) ;

    }

    {

        EXPECT_FALSE(Trajectory::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, AccessModel)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        EXPECT_TRUE(trajectory.accessModel().is<Tabulated>()) ;

    }

    {

        EXPECT_ANY_THROW(Trajectory::Undefined().accessModel()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, GetStateAt)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        EXPECT_EQ(states.at(0), trajectory.getStateAt(states.at(0).accessInstant())) ;
        EXPECT_EQ(states.at(1), trajectory.getStateAt(states.at(1).accessInstant())) ;

        EXPECT_EQ
        (
            State(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC), Position::Meters({ 0.5, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr)),
            trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC))
        ) ;

        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2017, 12, 31, 23, 59, 59), Scale::UTC))) ;
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2017, 12, 31, 23, 59, 58), Scale::UTC))) ;
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2017, 12, 31, 23, 59, 59), Scale::UTC))) ;
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC))) ;
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 3), Scale::UTC))) ;
        EXPECT_ANY_THROW(trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC))) ;

        EXPECT_EQ(states.at(0), trajectory.getStateAt(states.at(0).accessInstant())) ;
        EXPECT_EQ(states.at(1), trajectory.getStateAt(states.at(1).accessInstant())) ;

        EXPECT_EQ
        (
            State(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC), Position::Meters({ 0.5, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr)),
            trajectory.getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC))
        ) ;

    }

    {

        EXPECT_ANY_THROW(Trajectory::Undefined().getStateAt(Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC))) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, GetStatesAt)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        {

            const Array<Instant> referenceInstants =
            {
                states.at(0).accessInstant(),
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC),
                states.at(1).accessInstant()
            } ;

            const Array<State> referenceStates =
            {
                { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
                { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0, 500), Scale::UTC), Position::Meters({ 0.5, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
                { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
            } ;

            EXPECT_EQ(referenceStates, trajectory.getStatesAt(referenceInstants)) ;

        }

        {

            const Array<Instant> referenceInstants =
            {
                Instant::DateTime(DateTime(2017, 12, 31, 23, 59, 59), Scale::UTC),
                states.at(0).accessInstant(),
                states.at(1).accessInstant(),
                Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 2), Scale::UTC)
            } ;

            EXPECT_ANY_THROW(trajectory.getStatesAt(referenceInstants)) ;

        }

    }

    {

        EXPECT_ANY_THROW(Trajectory::Undefined().getStatesAt({ Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC) })) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, Print)
{

    using library::core::types::Shared ;
    using library::core::ctnr::Array ;

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::coord::Position ;
    using library::physics::coord::Velocity ;
    using library::physics::coord::Frame ;

    using library::astro::Trajectory ;
    using library::astro::trajectory::State ;
    using library::astro::trajectory::models::Tabulated ;

    {

        const Shared<const Frame> gcrfSPtr = Frame::GCRF() ;

        const Array<State> states =
        {
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), Position::Meters({ 0.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) },
            { Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), Position::Meters({ 1.0, 0.0, 0.0 }, gcrfSPtr), Velocity::MetersPerSecond({ 1.0, 0.0, 0.0 }, gcrfSPtr) }
        } ;

        const Tabulated model = { states } ;

        const Trajectory trajectory = { model } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(trajectory.print(std::cout, true)) ;
        EXPECT_NO_THROW(trajectory.print(std::cout, false)) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, Undefined)
{

    using library::astro::Trajectory ;

    {

        EXPECT_NO_THROW(Trajectory::Undefined()) ;
        EXPECT_FALSE(Trajectory::Undefined().isDefined()) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////