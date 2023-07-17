/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/CoordinatesSubsets/Invariant.hpp>

#include <Global.test.hpp>

using ostk::core::types::Shared;
using ostk::core::types::String;

using ostk::math::obj::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;

using ostk::astro::trajectory::CoordinatesBroker;
using ostk::astro::trajectory::coordinatessubsets::Invariant;

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_Invariant, Constructor)
{
    {
        EXPECT_NO_THROW(Invariant("NAME", 1));
    }

    {
        EXPECT_ANY_THROW(Invariant("", 0));
        EXPECT_ANY_THROW(Invariant("NAME", 0));
    }
}



TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_Invariant, EqualToOperator)
{
    {
        EXPECT_TRUE(Invariant("NAME", 1) == Invariant("NAME", 1));
    }
    
    {
        EXPECT_FALSE(Invariant("NAME", 1) == Invariant("OTHER", 1));
        EXPECT_FALSE(Invariant("NAME", 1) == Invariant("NAME", 2));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_Invariant, NotEqualToOperator)
{
    {
        EXPECT_FALSE(Invariant("NAME", 1) != Invariant("NAME", 1));
    }
    
    {
        EXPECT_TRUE(Invariant("NAME", 1) != Invariant("OTHER", 1));
        EXPECT_TRUE(Invariant("NAME", 1) != Invariant("NAME", 2));
    }
    
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_Invariant, Getters)
{
    {
        const Invariant invariant = Invariant("NAME", 1);

        EXPECT_EQ("NAME", invariant.getName());
        EXPECT_EQ(1, invariant.getSize());
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_Invariant, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> frame_1 = Frame::GCRF();
        const Shared<const Frame> frame_2 = Frame::TEME();
        VectorXd allCoordinates(3);
        allCoordinates << 0.0, 1.0, 2.0;
        const Invariant invariant_1 = Invariant("S1", 1);
        const Invariant invariant_2 = Invariant("S2", 2);
        CoordinatesBroker broker = CoordinatesBroker();
        broker.addSubset(invariant_1);
        broker.addSubset(invariant_2);
        const Shared<const CoordinatesBroker> brokerSPtr = std::make_shared<CoordinatesBroker>(broker);

        const VectorXd inFrame_1 = invariant_1.inFrame(instant, allCoordinates, frame_1, brokerSPtr, frame_2);
        EXPECT_EQ(1, inFrame_1.size());
        EXPECT_EQ(0.0, inFrame_1(0));

        const VectorXd inFrame_2 = invariant_2.inFrame(instant, allCoordinates, frame_1, brokerSPtr, frame_2);
        EXPECT_EQ(2, inFrame_2.size());
        EXPECT_EQ(1.0, inFrame_2(0));
        EXPECT_EQ(2.0, inFrame_2(1));
    }
}

TEST(OpenSpaceToolkit_Astrodynamics_Trajectory_CoordinatesSubsets_Invariant, Mass)
{
    {
        EXPECT_NO_THROW(Invariant::Mass());
    }

    {
        const Invariant mass = Invariant::Mass();

        EXPECT_EQ(Invariant::MASS_DEFAULT_NAME, mass.getName());
        EXPECT_EQ(1, mass.getSize());
    }
}

