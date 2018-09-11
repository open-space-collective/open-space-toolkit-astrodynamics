////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/Access.test.cpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Library/Astrodynamics/Access.hpp>

#include <Library/Physics/Time/DateTime.hpp>
#include <Library/Physics/Time/Instant.hpp>
#include <Library/Physics/Time/Scale.hpp>

#include <Global.test.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST (Library_Astrodynamics_Access, Constructor)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        EXPECT_NO_THROW(Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal) ;) ;

    }

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        EXPECT_ANY_THROW(Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal) ;) ;

    }

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;

        EXPECT_ANY_THROW(Access access(type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal) ;) ;

    }

}

TEST (Library_Astrodynamics_Access, EqualToOperator)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_TRUE(access == access) ;

    }

    {

        const Access::Type type = Access::Type::Partial ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_TRUE(access == access) ;

    }

    {

        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access firstAccess = { Access::Type::Complete, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;
        const Access secondAccess = { Access::Type::Partial, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_FALSE(firstAccess == secondAccess) ;

    }

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access firstAccess = { type, Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), timeOfClosestApproach, lossOfSignal } ;
        const Access secondAccess = { type, Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), timeOfClosestApproach, lossOfSignal } ;

        EXPECT_FALSE(firstAccess == secondAccess) ;

    }

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_FALSE(Access::Undefined() == Access::Undefined()) ;
        EXPECT_FALSE(Access::Undefined() == access) ;
        EXPECT_FALSE(access == Access::Undefined()) ;
        
    }

}

TEST (Library_Astrodynamics_Access, NotEqualToOperator)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_FALSE(access != access) ;

    }

    {

        const Access::Type type = Access::Type::Partial ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_FALSE(access != access) ;

    }

    {

        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access firstAccess = { Access::Type::Complete, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;
        const Access secondAccess = { Access::Type::Partial, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_TRUE(firstAccess != secondAccess) ;

    }

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access firstAccess = { type, Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC), timeOfClosestApproach, lossOfSignal } ;
        const Access secondAccess = { type, Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 1), Scale::UTC), timeOfClosestApproach, lossOfSignal } ;

        EXPECT_TRUE(firstAccess != secondAccess) ;

    }

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_TRUE(Access::Undefined() != Access::Undefined()) ;
        EXPECT_TRUE(Access::Undefined() != access) ;
        EXPECT_TRUE(access != Access::Undefined()) ;
        
    }

}

TEST (Library_Astrodynamics_Access, StreamOperator)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        testing::internal::CaptureStdout() ;

        EXPECT_NO_THROW(std::cout << access << std::endl) ;

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty()) ;

    }

}

TEST (Library_Astrodynamics_Access, IsDefined)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_TRUE(access.isDefined()) ;

    }

    {

        EXPECT_FALSE(Access::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Access, IsComplete)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_TRUE(access.isComplete()) ;

    }

    {

        const Access::Type type = Access::Type::Partial ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_FALSE(access.isComplete()) ;

    }

    {

        EXPECT_ANY_THROW(Access::Undefined().isComplete()) ;

    }

}

TEST (Library_Astrodynamics_Access, GetType)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_EQ(type, access.getType()) ;

    }

    {

        const Access::Type type = Access::Type::Partial ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_EQ(type, access.getType()) ;

    }

    {

        EXPECT_ANY_THROW(Access::Undefined().getType()) ;

    }

}

TEST (Library_Astrodynamics_Access, GetAcquisitionOfSignal)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_EQ(acquisitionOfSignal, access.getAcquisitionOfSignal()) ;

    }

    {

        EXPECT_ANY_THROW(Access::Undefined().getAcquisitionOfSignal()) ;

    }

}

TEST (Library_Astrodynamics_Access, GetTimeOfClosestApproach)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_EQ(timeOfClosestApproach, access.getTimeOfClosestApproach()) ;

    }

    {

        EXPECT_ANY_THROW(Access::Undefined().getTimeOfClosestApproach()) ;

    }

}

TEST (Library_Astrodynamics_Access, GetLossOfSignal)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_EQ(lossOfSignal, access.getLossOfSignal()) ;

    }

    {

        EXPECT_ANY_THROW(Access::Undefined().getLossOfSignal()) ;

    }

}

TEST (Library_Astrodynamics_Access, GetInterval)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::time::Interval ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_EQ(Interval::Closed(acquisitionOfSignal, lossOfSignal), access.getInterval()) ;

    }

    {

        EXPECT_ANY_THROW(Access::Undefined().getInterval()) ;

    }

}

TEST (Library_Astrodynamics_Access, GetDuration)
{

    using library::physics::time::Scale ;
    using library::physics::time::Instant ;
    using library::physics::time::DateTime ;
    using library::physics::time::Duration ;

    using library::astro::Access ;

    {

        const Access::Type type = Access::Type::Complete ;
        const Instant acquisitionOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 0, 0), Scale::UTC) ;
        const Instant timeOfClosestApproach = Instant::DateTime(DateTime(2018, 1, 1, 0, 1, 0), Scale::UTC) ;
        const Instant lossOfSignal = Instant::DateTime(DateTime(2018, 1, 1, 0, 2, 0), Scale::UTC) ;

        const Access access = { type, acquisitionOfSignal, timeOfClosestApproach, lossOfSignal } ;

        EXPECT_EQ(Duration::Minutes(2.0), access.getDuration()) ;

    }

    {

        EXPECT_ANY_THROW(Access::Undefined().getDuration()) ;

    }

}

TEST (Library_Astrodynamics_Access, Undefined)
{

    using library::astro::Access ;

    {

        EXPECT_NO_THROW(Access::Undefined()) ;

        EXPECT_FALSE(Access::Undefined().isDefined()) ;

    }

}

TEST (Library_Astrodynamics_Access, StringFromType)
{

    using library::astro::Access ;

    {

        EXPECT_EQ("Undefined", Access::StringFromType(Access::Type::Undefined)) ;
        EXPECT_EQ("Complete", Access::StringFromType(Access::Type::Complete)) ;
        EXPECT_EQ("Partial", Access::StringFromType(Access::Type::Partial)) ;

    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////