/// Apache License 2.0

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Astrodynamics/CrossValidator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

using ostk::astro::validation::CrossValidator;
using ostk::astro::validation::Quantity;
using ostk::astro::validation::QuantityComparison;
using ostk::astro::validation::Tool;
using ostk::astro::validation::ToolComparison;

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::filesystem::Path;
using ostk::core::filesystem::File;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::VectorXd;

using ostk::physics::coord::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::time::DateTime;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::CoordinatesBroker;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianPosition;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Validation_CrossValidator : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        VectorXd initialCoordinates(7);
        initialCoordinates << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 10.0;
        defaultState_ = {Instant::J2000(), initialCoordinates, Frame::GCRF(), coordinatesBrokerSPtr_};

        referenceCoordinates_ = {
            initialCoordinates.segment(0, 3), initialCoordinates.segment(3, 3), initialCoordinates.segment(6, 1)
        };
    }

    const Table table_ = Table::Load(
        File::Path(Path::Parse({"/app/validation/OpenSpaceToolkit/Astrodynamics/data/test/test_table.csv"})),
        Table::Format::CSV,
        true
    );

    const ToolComparison toolComparisonGMAT_ = {
        Tool::GMAT,
        {
            {Quantity::CARTESIAN_POSITION_GCRF, 1.0e-0},
            {Quantity::CARTESIAN_VELOCITY_GCRF, 1.0e-3},
            {Quantity::CARTESIAN_ACCELERATION_GCRF, 1.0e-3},
            {Quantity::MASS, 1.0e-6},
            {Quantity::MANEUVER_ACCELERATION_J2000, 1.0e-3},
        },
    };
    const ToolComparison toolComparisonOREKIT_ = {
        Tool::OREKIT,
        {
            {Quantity::CARTESIAN_POSITION_GCRF, 1.0e-0},
            {Quantity::CARTESIAN_VELOCITY_GCRF, 1.0e-3},
            {Quantity::CARTESIAN_ACCELERATION_GCRF, 1.0e-3},
            {Quantity::MASS, 1.0e-6},
            {Quantity::MANEUVER_ACCELERATION_J2000, 1.0e-3},
        },
    };
    const ToolComparison toolComparisonOREKIT_PosVelOnly_ = {
        Tool::OREKIT,
        {
            {Quantity::CARTESIAN_POSITION_GCRF, 1.0e-0},
            {Quantity::CARTESIAN_VELOCITY_GCRF, 1.0e-3},
        },
    };

    const Shared<CoordinatesBroker> coordinatesBrokerSPtr_ = std::make_shared<CoordinatesBroker>(CoordinatesBroker({
        CartesianPosition::Default(),
        CartesianVelocity::Default(),
        CoordinatesSubset::Mass(),
    }));

    State defaultState_ = State::Undefined();
    Array<VectorXd> referenceCoordinates_ = Array<VectorXd>::Empty();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidator, IngestOutputQuantities)
{
    {
        const Array<Array<VectorXd>> groupedCoordinatesArray =
            CrossValidator::IngestOutputQuantities(table_, toolComparisonGMAT_);

        EXPECT_EQ(groupedCoordinatesArray.getSize(), 2);
        for (const auto& coordinatesArray : groupedCoordinatesArray)
        {
            EXPECT_EQ(coordinatesArray.getSize(), 5);
            EXPECT_EQ(coordinatesArray[0].size(), 3);
            EXPECT_EQ(coordinatesArray[1].size(), 3);
            EXPECT_EQ(coordinatesArray[2].size(), 3);
            EXPECT_EQ(coordinatesArray[3].size(), 1);
            EXPECT_EQ(coordinatesArray[4].size(), 3);

            EXPECT_EQ(coordinatesArray[0](0), 1000.0);
            EXPECT_EQ(coordinatesArray[0](1), 2000.0);
            EXPECT_EQ(coordinatesArray[0](2), 3000.0);
            EXPECT_EQ(coordinatesArray[1](0), 4000.0);
            EXPECT_EQ(coordinatesArray[1](1), 5000.0);
            EXPECT_EQ(coordinatesArray[1](2), 6000.0);
            EXPECT_EQ(coordinatesArray[2](0), 7000.0);
            EXPECT_EQ(coordinatesArray[2](1), 8000.0);
            EXPECT_EQ(coordinatesArray[2](2), 9000.0);
            EXPECT_EQ(coordinatesArray[3](0), 10.0);
            EXPECT_EQ(coordinatesArray[4](0), 11000.0);
            EXPECT_EQ(coordinatesArray[4](1), 12000.0);
            EXPECT_EQ(coordinatesArray[4](2), 13000.0);
        }
    }

    {
        const Array<Array<VectorXd>> groupedCoordinatesArray =
            CrossValidator::IngestOutputQuantities(table_, toolComparisonOREKIT_);

        EXPECT_EQ(groupedCoordinatesArray.getSize(), 2);
        for (const auto& coordinatesArray : groupedCoordinatesArray)
        {
            EXPECT_EQ(coordinatesArray.getSize(), 5);
            EXPECT_EQ(coordinatesArray[0].size(), 3);
            EXPECT_EQ(coordinatesArray[1].size(), 3);
            EXPECT_EQ(coordinatesArray[2].size(), 3);
            EXPECT_EQ(coordinatesArray[3].size(), 1);
            EXPECT_EQ(coordinatesArray[4].size(), 3);

            EXPECT_EQ(coordinatesArray[0](0), 1.0);
            EXPECT_EQ(coordinatesArray[0](1), 2.0);
            EXPECT_EQ(coordinatesArray[0](2), 3.0);
            EXPECT_EQ(coordinatesArray[1](0), 4.0);
            EXPECT_EQ(coordinatesArray[1](1), 5.0);
            EXPECT_EQ(coordinatesArray[1](2), 6.0);
            EXPECT_EQ(coordinatesArray[2](0), 7.0);
            EXPECT_EQ(coordinatesArray[2](1), 8.0);
            EXPECT_EQ(coordinatesArray[2](2), 9.0);
            EXPECT_EQ(coordinatesArray[3](0), 10.0);
            EXPECT_EQ(coordinatesArray[4](0), 11.0);
            EXPECT_EQ(coordinatesArray[4](1), 12.0);
            EXPECT_EQ(coordinatesArray[4](2), 13.0);
        }
    }

    {
        const Array<Array<VectorXd>> groupedCoordinatesArray =
            CrossValidator::IngestOutputQuantities(table_, toolComparisonOREKIT_PosVelOnly_);

        EXPECT_EQ(groupedCoordinatesArray.getSize(), 2);
        for (const auto& coordinatesArray : groupedCoordinatesArray)
        {
            EXPECT_EQ(coordinatesArray.getSize(), 2);
            EXPECT_EQ(coordinatesArray[0].size(), 3);
            EXPECT_EQ(coordinatesArray[1].size(), 3);

            EXPECT_EQ(coordinatesArray[0](0), 1.0);
            EXPECT_EQ(coordinatesArray[0](1), 2.0);
            EXPECT_EQ(coordinatesArray[0](2), 3.0);
            EXPECT_EQ(coordinatesArray[1](0), 4.0);
            EXPECT_EQ(coordinatesArray[1](1), 5.0);
            EXPECT_EQ(coordinatesArray[1](2), 6.0);
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidator, CompareOutputQuantities)
{
    {
        const VectorXd deltaWithTool = CrossValidator::CompareOutputQuantities(
            defaultState_, referenceCoordinates_, toolComparisonOREKIT_PosVelOnly_.quantityComparisons
        );

        EXPECT_EQ(deltaWithTool.size(), 2);
        EXPECT_EQ(deltaWithTool(0), 0.0);
        EXPECT_EQ(deltaWithTool(1), 0.0);
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    CrossValidator::CompareOutputQuantities(
                        defaultState_, referenceCoordinates_, toolComparisonOREKIT_.quantityComparisons
                    );
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Quantity} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );

        EXPECT_THROW(
            {
                try
                {
                    CrossValidator::CompareOutputQuantities(
                        defaultState_, referenceCoordinates_, toolComparisonGMAT_.quantityComparisons
                    );
                }
                catch (const ostk::core::error::runtime::Wrong& e)
                {
                    EXPECT_EQ("{Quantity} is wrong.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_CrossValidator, FindMaxDeltaIndex)
{
    {
        const Array<VectorXd> allDeltasWithTool = {
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
        };

        const Size maxDeltaIndex = CrossValidator::FindMaxDeltaIndex(allDeltasWithTool, 0);

        EXPECT_EQ(maxDeltaIndex, 0);
    }

    {
        const Array<VectorXd> allDeltasWithTool = {
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
        };

        const Size maxDeltaIndex = CrossValidator::FindMaxDeltaIndex(allDeltasWithTool, 1);

        EXPECT_EQ(maxDeltaIndex, 0);
    }

    {
        const Array<VectorXd> allDeltasWithTool = {
            VectorXd::Ones(2),
            VectorXd::Ones(2),
            VectorXd::Ones(2),
            VectorXd::Ones(2),
            VectorXd::Ones(2),
        };

        const Size maxDeltaIndex = CrossValidator::FindMaxDeltaIndex(allDeltasWithTool, 1);

        EXPECT_EQ(maxDeltaIndex, 0);
    }

    {
        const Array<VectorXd> allDeltasWithTool = {
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
            VectorXd::Zero(2),
        };

        EXPECT_THROW(CrossValidator::FindMaxDeltaIndex(allDeltasWithTool, 2), ostk::core::error::runtime::Wrong);
    }

    {
        VectorXd posDeltas(3);
        posDeltas << 1.0, 0.0, 0.5;
        VectorXd velDeltas(3);
        velDeltas << 0.0, 1.0, 0.5;
        VectorXd massDeltas(3);
        massDeltas << 1.0, 1.0, 1.5;

        const Array<VectorXd> allDeltasWithTool = {
            posDeltas,
            velDeltas,
            massDeltas,
        };

        EXPECT_EQ(CrossValidator::FindMaxDeltaIndex(allDeltasWithTool, 0), 0);
        EXPECT_EQ(CrossValidator::FindMaxDeltaIndex(allDeltasWithTool, 1), 1);
        EXPECT_EQ(CrossValidator::FindMaxDeltaIndex(allDeltasWithTool, 2), 2);
    }
}
