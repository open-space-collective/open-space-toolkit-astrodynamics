/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated.hpp>

#include <Global.test.hpp>

using ostk::core::type::Integer;
using ostk::core::type::Size;
using ostk::core::type::Index;
using ostk::core::type::Real;
using ostk::core::type::String;
using ostk::core::container::Table;
using ostk::core::container::Array;
using ostk::core::container::Tuple;
using ostk::core::filesystem::Path;
using ostk::core::filesystem::File;

using ostk::mathematics::object::VectorXd;

using ostk::physics::Environment;
using ostk::physics::time::Instant;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Scale;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::Orbit;
using ostk::astrodynamics::trajectory::orbit::Model;
using ostk::astrodynamics::trajectory::orbit::model::Tabulated;

class OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        for (Size i = 0; i < 10; ++i)
        {
            states_.add(State(
                Instant::DateTime(DateTime::Parse("2018-01-01 00:00:00.000"), Scale::UTC) + Duration::Minutes(i),
                Position::Meters({1.0, 2.0, 3.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({4.0, 5.0, 6.0}, Frame::GCRF())
            ));
        }
    }

    void loadData()
    {
        states_.clear();
        referenceStates_.clear();

        const Table referenceData = Table::Load(
            File::Path(Path::Parse(
                "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/Tabulated/propagated_states.csv"
            )),
            Table::Format::CSV,
            true
        );

        referenceStates_.reserve(referenceData.getRowCount());
        states_.reserve(referenceData.getRowCount() / testStatesCount_);

        Size t = 0;
        for (const auto& row : referenceData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(row[0].accessString()), Scale::UTC);

            const Position position =
                Position::Meters({row[1].accessReal(), row[2].accessReal(), row[3].accessReal()}, Frame::GCRF());

            const Velocity velocity = Velocity::MetersPerSecond(
                {row[4].accessReal(), row[5].accessReal(), row[6].accessReal()}, Frame::GCRF()
            );

            State state = State(instant, position, velocity);

            referenceStates_.add(state);

            if ((t % testStatesCount_) == 0)
            {
                states_.add(state);
            }

            ++t;
        }
    }

    const Size testStatesCount_ = 20;
    Array<State> states_ = Array<State>::Empty();
    Array<State> referenceStates_ = Array<State>::Empty();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated, Constructor)
{
    const Tabulated tabulated(states_, 0, Tabulated::InterpolationType::Linear);

    Environment environment = Environment::Default();

    const Orbit orbit = {tabulated, environment.accessCelestialObjectWithName("Earth")};
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated, GetInterval)
{
    using ostk::physics::time::Interval;

    const Tabulated tabulated(states_, 0, Tabulated::InterpolationType::Linear);

    EXPECT_TRUE(tabulated.getInterval().isDefined());
    EXPECT_TRUE(
        tabulated.getInterval() ==
        Interval::Closed(states_.accessFirst().accessInstant(), states_.accessLast().accessInstant())
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated, EqualToOperator)
{
    const Tabulated tabulated(states_, 0, Tabulated::InterpolationType::Linear);
    const Tabulated anotherTabulated(states_, 0, Tabulated::InterpolationType::Linear);

    EXPECT_TRUE(tabulated == anotherTabulated);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated, NotEqualToOperator)
{
    const Tabulated tabulated(states_, 0, Tabulated::InterpolationType::CubicSpline);
    const Tabulated anotherTabulated(states_, 0, Tabulated::InterpolationType::Linear);

    EXPECT_TRUE(tabulated != anotherTabulated);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated, CalculateStateAt)
{
    loadData();

    const Array<Tuple<Tabulated::InterpolationType, Real>> testCases = {
        {Tabulated::InterpolationType::Linear, 420.0},
        {Tabulated::InterpolationType::BarycentricRational, 5e-2},
        {Tabulated::InterpolationType::CubicSpline, 5e-3},
    };

    for (const auto& testCase : testCases)
    {
        Tabulated::InterpolationType interpolationType = std::get<0>(testCase);
        Real tolerance = std::get<1>(testCase);

        const Tabulated tabulated(states_, 0, interpolationType);

        for (Size i = 0; i < referenceStates_.getSize() - testStatesCount_; ++i)
        {
            const Instant referenceInstant = referenceStates_[i].accessInstant();

            if (i == 0 || referenceInstant >= states_.accessLast().accessInstant())
            {
                continue;
            }

            State state = tabulated.calculateStateAt(referenceInstant);

            VectorXd residuals = (state.getCoordinates() - referenceStates_[i].getCoordinates()).array().abs();

            EXPECT_TRUE((residuals.array() < tolerance).all()) << String::Format("Residual: {}", residuals.maxCoeff());
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Trajectory_Orbit_Model_Tabulated, CalculateStatesAt)
{
    loadData();

    const Array<Tuple<Tabulated::InterpolationType, Real>> testCases = {
        {Tabulated::InterpolationType::Linear, 420.0},
        {Tabulated::InterpolationType::BarycentricRational, 5e-2},
        {Tabulated::InterpolationType::CubicSpline, 5e-3},
    };

    for (const auto& testCase : testCases)
    {
        Tabulated::InterpolationType interpolationType = std::get<0>(testCase);
        Real tolerance = std::get<1>(testCase);

        const Tabulated tabulated(states_, 0, interpolationType);

        Array<Instant> referenceInstants = Array<Instant>::Empty();
        Array<State> referenceStates = Array<State>::Empty();

        for (Index i = 0; i < referenceStates_.getSize() - testStatesCount_; ++i)
        {
            const Instant referenceInstant = referenceStates_[i].accessInstant();

            if (i == 0 || referenceInstant >= states_.accessLast().accessInstant())
            {
                continue;
            }

            referenceInstants.add(referenceInstant);
            referenceStates.add(referenceStates_[i]);
        }

        const Array<State> states = tabulated.calculateStatesAt(referenceInstants);

        for (Index i = 0; i < states.getSize(); ++i)
        {
            VectorXd residuals = (states[i].getCoordinates() - referenceStates[i].getCoordinates()).array().abs();

            EXPECT_TRUE((residuals.array() < tolerance).all()) << String::Format("Residual: {}", residuals.maxCoeff());
        }
    }
}
