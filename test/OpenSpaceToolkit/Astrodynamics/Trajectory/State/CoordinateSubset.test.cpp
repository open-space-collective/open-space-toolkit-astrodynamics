/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

#include <Global.test.hpp>

using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::type::Size;
using ostk::core::container::Array;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Size defaultSize_ = 1;
    const CoordinateSubset defaultCoordinateSubset_ = CoordinateSubset(defaultName_, defaultSize_);

    const Array<Shared<const CoordinateSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<CoordinateSubset>(defaultCoordinateSubset_)
    };

    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, Constructor)
{
    {
        EXPECT_NO_THROW(CoordinateSubset("NAME", 1));
    }

    {
        EXPECT_NO_THROW(CoordinateSubset("R45", 4));
    }

    {
        EXPECT_ANY_THROW(CoordinateSubset("", 0));
        EXPECT_ANY_THROW(CoordinateSubset("NAME", 0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, EqualToOperator)
{
    {
        EXPECT_TRUE(defaultCoordinateSubset_ == defaultCoordinateSubset_);
    }

    {
        EXPECT_FALSE(defaultCoordinateSubset_ == CoordinateSubset("OTHER", 1));
        EXPECT_FALSE(defaultCoordinateSubset_ == CoordinateSubset("NAME", 2));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, NotEqualToOperator)
{
    {
        EXPECT_FALSE(defaultCoordinateSubset_ != defaultCoordinateSubset_);
    }

    {
        EXPECT_TRUE(defaultCoordinateSubset_ != CoordinateSubset("OTHER", 1));
        EXPECT_TRUE(defaultCoordinateSubset_ != CoordinateSubset("NAME", 2));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, Hash)
{
    {
        EXPECT_NO_THROW(defaultCoordinateSubset_.hash());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, Getters)
{
    {
        EXPECT_EQ(defaultName_, defaultCoordinateSubset_.getName());
        EXPECT_EQ(defaultSize_, defaultCoordinateSubset_.getSize());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, Add)
{
    const Instant instant = Instant::J2000();
    const Shared<const Frame> frame = Frame::GCRF();
    VectorXd input1(4);
    VectorXd input2(4);

    input1 << 1.0, 1.0, 2.0, 3.0;
    input2 << 2.0, 7.0, 3.0, 4.0;

    {
        EXPECT_ANY_THROW(defaultCoordinateSubset_.add(instant, input1, input2, frame, defaultCoordinateBroker_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, Subtract)
{
    const Instant instant = Instant::J2000();
    const Shared<const Frame> frame = Frame::GCRF();
    VectorXd input1(4);
    VectorXd input2(4);

    input1 << 1.0, 1.0, 2.0, 3.0;
    input2 << 2.0, 7.0, 3.0, 4.0;

    {
        EXPECT_ANY_THROW(defaultCoordinateSubset_.subtract(instant, input1, input2, frame, defaultCoordinateBroker_));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(3);
        fullCoordinatesVector << 1.0e7, -1e7, 5e6;

        VectorXd expected(1);
        expected << 1.0e7;

        VectorXd actual(1);
        actual = defaultCoordinateSubset_.inFrame(
            instant, fullCoordinatesVector, fromFrame, toFrame, defaultCoordinateBroker_
        );

        EXPECT_EQ(expected, actual);
    }

    // Attempt to transform a non-scalar subset without implementing the inFrame method
    {
        const Shared<const CoordinateSubset> nonScalarCoordinateSubset = std::make_shared<CoordinateSubset>("NAME", 3);
        const Shared<const CoordinateBroker> nonScalarCoordinateBroker =
            std::make_shared<CoordinateBroker>(Array<Shared<const CoordinateSubset>>({nonScalarCoordinateSubset}));

        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(3);
        fullCoordinatesVector << 1.0e7, -1e7, 5e6;

        EXPECT_THROW(
            {
                try
                {
                    nonScalarCoordinateSubset->inFrame(
                        instant, fullCoordinatesVector, fromFrame, toFrame, nonScalarCoordinateBroker
                    );
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(
                        "Cannot transform a non-scalar coordinate subset. Child classes must implement this method.",
                        e.getMessage()
                    );
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, Mass)
{
    {
        EXPECT_NO_THROW(CoordinateSubset::Mass());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, SurfaceArea)
{
    {
        EXPECT_NO_THROW(CoordinateSubset::SurfaceArea());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset, DragCoefficient)
{
    {
        EXPECT_NO_THROW(CoordinateSubset::DragCoefficient());
    }
}
