////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Trajectory.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Trajectory.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Trajectory, Constructor)
{

    using library::astro::Trajectory ;

    {

        // EXPECT_NO_THROW(Trajectory(states)) ;

        FAIL() ;

    }

}

TEST (Library_Astrodynamics_Trajectory, EqualToOperator)
{

    using library::astro::Trajectory ;

    {

        FAIL() ;

    }

}

TEST (Library_Astrodynamics_Trajectory, NotEqualToOperator)
{

    using library::astro::Trajectory ;

    {

        FAIL() ;

    }

}

TEST (Library_Astrodynamics_Trajectory, StreamOperator)
{

    using library::astro::Trajectory ;

    {

        FAIL() ;

        // const Trajectory trajectory = ... ;

        // testing::internal::CaptureStdout() ;

        // EXPECT_NO_THROW(std::cout << trajectory << std::endl) ;

        // EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, ConstIterator)
{

    using library::astro::Trajectory ;

    {

        FAIL() ;

    }

}

TEST (Library_Astrodynamics_Trajectory, IsDefined)
{

    using library::astro::Trajectory ;

    {

        FAIL() ;

    }

    {

        EXPECT_FALSE(Trajectory::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Trajectory, GetInterval)
{

    using library::astro::Trajectory ;

    {

        FAIL() ;

    }

}

TEST (Library_Astrodynamics_Trajectory, GetPositionAt)
{

    using library::astro::Trajectory ;

    {

        FAIL() ;

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