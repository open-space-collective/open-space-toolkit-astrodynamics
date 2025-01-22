/// Apache License 2.0

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/NewtonianAcceleration.hpp>

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
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::NewtonianAcceleration;

class OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_NewtonianAcceleration : public ::testing::Test
{
   protected:
    const String defaultName_ = "NAME";
    const Shared<const CartesianPosition> defaultCartesianPosition_ = CartesianPosition::Default();
    const NewtonianAcceleration defaultNewtonianAcceleration_ = {defaultCartesianPosition_, defaultName_};
    const Array<Shared<const CoordinateSubset>> defaultCoordinateSubsets_ = {
        std::make_shared<NewtonianAcceleration>(defaultNewtonianAcceleration_)
    };
    const Shared<const CoordinateBroker> defaultCoordinateBroker_ =
        std::make_shared<CoordinateBroker>(defaultCoordinateSubsets_);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_NewtonianAcceleration, Constructor)
{
    {
        EXPECT_NO_THROW(NewtonianAcceleration(defaultCartesianPosition_, "NAME"));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_NewtonianAcceleration, Getters)
{
    EXPECT_EQ(3, defaultNewtonianAcceleration_.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_NewtonianAcceleration, Add)
{
    {
        VectorXd firstCoordinates(3);
        firstCoordinates << 1.0, 2.0, 3.0;
        VectorXd secondCoordinates(3);
        secondCoordinates << 4.0, 5.0, 6.0;

        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        const VectorXd actual = defaultNewtonianAcceleration_.add(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinateBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_NewtonianAcceleration, Subtract)
{
    {
        VectorXd firstCoordinates(3);
        firstCoordinates << 1.0, 2.0, 3.0;
        VectorXd secondCoordinates(3);
        secondCoordinates << -4.0, -5.0, -6.0;

        VectorXd expected(3);
        expected << 5.0, 7.0, 9.0;

        const VectorXd actual = defaultNewtonianAcceleration_.subtract(
            Instant::Undefined(), firstCoordinates, secondCoordinates, Frame::Undefined(), defaultCoordinateBroker_
        );

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_NewtonianAcceleration, InFrame)
{
    {
        const Instant instant = Instant::J2000();
        const Shared<const Frame> fromFrame = Frame::GCRF();
        const Shared<const Frame> toFrame = Frame::TEME();
        VectorXd fullCoordinatesVector(3);
        fullCoordinatesVector << 1.0e7, -1e7, 5e6;
        const Shared<const CoordinateBroker> brokerkSPtr = defaultCoordinateBroker_;

        Vector3d expected = Position::Meters({1.0e7, -1e7, 5e6}, fromFrame).inFrame(toFrame, instant).getCoordinates();

        const VectorXd actual =
            defaultNewtonianAcceleration_.inFrame(instant, fullCoordinatesVector, fromFrame, toFrame, brokerkSPtr);

        EXPECT_EQ(expected, actual);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_State_CoordinateSubset_NewtonianAcceleration, Default)
{
    {
        EXPECT_NO_THROW(NewtonianAcceleration::Default());
    }
}
