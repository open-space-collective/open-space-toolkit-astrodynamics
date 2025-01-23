/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianAcceleration.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Shared<const CartesianPosition> defaultCartesianPosition_ = CartesianPosition::Default();
    const Shared<const CartesianVelocity> defaultCartesianVelocity_ = CartesianVelocity::Default();
    const CartesianAcceleration defaultCartesianAcceleration_ = {
        defaultCartesianPosition_,
        defaultCartesianVelocity_,
        defaultName_,
    };
    const Array<Shared<const CoordinateSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<CartesianAcceleration>(defaultCartesianAcceleration_)
    };
    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration, Constructor)
{
    {
        EXPECT_NO_THROW(CartesianAcceleration(defaultCartesianPosition_, defaultCartesianVelocity_, "NAME"));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration, Getters)
{
    EXPECT_EQ(3, defaultCartesianAcceleration_.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration, Add)
{
    {
        VectorXd firstCoordinates(3);
        firstCoordinates << 1.0, 2.0, 3.0;
        VectorXd secondCoordinates(3);
        secondCoordinates << 4.0, 5.0, 6.0;

        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        const VectorXd actual = defaultCartesianAcceleration_.add(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinateBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration, Subtract)
{
    {
        VectorXd firstCoordinates(3);
        firstCoordinates << 1.0, 2.0, 3.0;
        VectorXd secondCoordinates(3);
        secondCoordinates << -4.0, -5.0, -6.0;

        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        const VectorXd actual = defaultCartesianAcceleration_.subtract(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinateBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration, InFrame)
{
    {
        VectorXd coordinates(3);
        coordinates << 1.0, 2.0, 3.0;
        const Shared<const CoordinateBroker> coordinateBroker =
            std::make_shared<CoordinateBroker>(defaultCoordinateSubsets_);

        {
            EXPECT_VECTORS_ALMOST_EQUAL(
                defaultCartesianAcceleration_.inFrame(
                    Instant::J2000(), coordinates, Frame::GCRF(), Frame::GCRF(), coordinateBroker
                ),
                coordinates,
                1e-15
            );
        }

        {
            EXPECT_THROW(
                defaultCartesianAcceleration_.inFrame(
                    Instant::J2000(), coordinates, Frame::GCRF(), Frame::ITRF(), coordinateBroker
                ),
                ostk::core::error::runtime::ToBeImplemented
            );
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration, Default)
{
    {
        EXPECT_NO_THROW(CartesianAcceleration::Default());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_CartesianAcceleration, ThrustAcceleration)
{
    {
        EXPECT_NO_THROW(CartesianAcceleration::ThrustAcceleration());
    }
}
