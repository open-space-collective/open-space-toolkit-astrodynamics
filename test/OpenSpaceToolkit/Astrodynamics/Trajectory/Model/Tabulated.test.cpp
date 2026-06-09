/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::trajectory::model::Tabulated;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianAcceleration;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Tabulated : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Instant startInstant = Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00.000"), Scale::UTC);

        for (Size i = 0; i < 10; ++i)
        {
            states_.add(State(
                startInstant + Duration::Minutes(static_cast<double>(i)),
                Position::Meters({1.0 + i, 2.0 + 2.0 * i, 3.0 + 3.0 * i}, Frame::GCRF()),
                Velocity::MetersPerSecond({4.0 + i, 5.0 + i, 6.0 + i}, Frame::GCRF())
            ));
        }
    }

    Array<State> states_ = Array<State>::Empty();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Tabulated, Constructor)
{
    {
        const Map<Shared<const CoordinateSubset>, Interpolator::Type> interpolationTypes = {
            {CartesianPosition::Default(), Interpolator::Type::CubicSpline},
            {CartesianVelocity::Default(), Interpolator::Type::Linear},
        };

        const Tabulated tabulated(states_, interpolationTypes);

        EXPECT_TRUE(tabulated.isDefined());
        EXPECT_TRUE(tabulated.getInterval().isDefined());
    }

    // Coordinate subsets in the map that are not present in the states are ignored (no error).
    {
        const Map<Shared<const CoordinateSubset>, Interpolator::Type> interpolationTypes = {
            {CartesianPosition::Default(), Interpolator::Type::CubicSpline},
            {CartesianVelocity::Default(), Interpolator::Type::Linear},
            {CoordinateSubset::Mass(), Interpolator::Type::BarycentricRational},
            {CoordinateSubset::DragCoefficient(), Interpolator::Type::ZeroOrder},
        };

        const Tabulated tabulated(states_, interpolationTypes);

        EXPECT_TRUE(tabulated.isDefined());
    }

    // A per-subset map assigning the same type to every subset must behave identically to the single-type
    // constructor.
    {
        const Map<Shared<const CoordinateSubset>, Interpolator::Type> interpolationTypes = {
            {CartesianPosition::Default(), Interpolator::Type::CubicSpline},
            {CartesianVelocity::Default(), Interpolator::Type::CubicSpline},
        };

        const Tabulated tabulatedFromMap(states_, interpolationTypes);
        const Tabulated tabulatedFromType(states_, Interpolator::Type::CubicSpline);

        EXPECT_TRUE(tabulatedFromMap == tabulatedFromType);

        for (Size i = 0; i < states_.getSize() - 1; ++i)
        {
            const Instant instant = states_[i].accessInstant() + Duration::Seconds(13.0);

            const VectorXd fromMapCoordinates = tabulatedFromMap.calculateStateAt(instant).getCoordinates();
            const VectorXd fromTypeCoordinates = tabulatedFromType.calculateStateAt(instant).getCoordinates();

            EXPECT_TRUE(fromMapCoordinates.isApprox(fromTypeCoordinates, 1e-12));
        }
    }

    // The states contain CartesianVelocity but no interpolation type is provided for it.
    {
        const Map<Shared<const CoordinateSubset>, Interpolator::Type> interpolationTypes = {
            {CartesianPosition::Default(), Interpolator::Type::CubicSpline},
        };

        EXPECT_THROW({ const Tabulated tabulated(states_, interpolationTypes); }, ostk::core::error::RuntimeError);
    }

    // Distinct per-subset types must produce a model distinct from any single-type model.
    {
        const Map<Shared<const CoordinateSubset>, Interpolator::Type> interpolationTypes = {
            {CartesianPosition::Default(), Interpolator::Type::CubicSpline},
            {CartesianVelocity::Default(), Interpolator::Type::Linear},
        };

        const Tabulated tabulatedFromMap(states_, interpolationTypes);

        EXPECT_TRUE(tabulatedFromMap != Tabulated(states_, Interpolator::Type::CubicSpline));
        EXPECT_TRUE(tabulatedFromMap != Tabulated(states_, Interpolator::Type::Linear));

        const State state =
            tabulatedFromMap.calculateStateAt(states_.accessFirst().accessInstant() + Duration::Seconds(30.0));

        EXPECT_TRUE(state.isDefined());
        EXPECT_TRUE(state.getCoordinates().allFinite());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Tabulated, Default)
{
    const Tabulated tabulated = Tabulated::Default(states_);

    EXPECT_TRUE(tabulated.isDefined());
    // Position is interpolated using barycentric rational by default.
    EXPECT_EQ(tabulated.getInterpolationType(), Interpolator::Type::BarycentricRational);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Tabulated, DefaultInterpolationTypes)
{
    const Map<Shared<const CoordinateSubset>, Interpolator::Type> defaultTypes = Tabulated::DefaultInterpolationTypes();

    EXPECT_EQ(defaultTypes.size(), Size(9));

    EXPECT_EQ(defaultTypes.at(CartesianPosition::Default()), Interpolator::Type::BarycentricRational);
    EXPECT_EQ(defaultTypes.at(CartesianVelocity::Default()), Interpolator::Type::BarycentricRational);
    EXPECT_EQ(defaultTypes.at(CartesianAcceleration::Default()), Interpolator::Type::BarycentricRational);
    EXPECT_EQ(defaultTypes.at(AngularVelocity::Default()), Interpolator::Type::BarycentricRational);
    EXPECT_EQ(defaultTypes.at(CoordinateSubset::Mass()), Interpolator::Type::ZeroOrder);
    EXPECT_EQ(defaultTypes.at(CoordinateSubset::DragCoefficient()), Interpolator::Type::ZeroOrder);
    EXPECT_EQ(defaultTypes.at(CoordinateSubset::SurfaceArea()), Interpolator::Type::ZeroOrder);
    EXPECT_EQ(defaultTypes.at(CoordinateSubset::MassFlowRate()), Interpolator::Type::ZeroOrder);
    EXPECT_EQ(defaultTypes.at(CoordinateSubset::BallisticCoefficient()), Interpolator::Type::ZeroOrder);
}
