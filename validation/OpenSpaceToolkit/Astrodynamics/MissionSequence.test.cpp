/// Apache License 2.0

#include <gtest/gtest.h>

#include <OpenSpaceToolkit/Astrodynamics/MissionSequence.hpp>

using ostk::astro::validation::Parser;
using ostk::astro::validation::ToolComparison;
using ostk::astro::validation::Tool;
using ostk::astro::validation::QuantityComparison;
using ostk::astro::validation::Quantity;
using ostk::astro::validation::MissionSequence;

using ostk::core::container::Array;
using ostk::core::container::Table;
using CntrObject = ostk::core::container::Object;
using ostk::core::container::Dictionary;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::filesystem::Directory;
using ostk::core::type::Integer;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;
using ostk::core::type::Size;

using ostk::mathematics::object::VectorXd;

using ostk::physics::Environment;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astro::Dynamics;
using ostk::astro::flight::system::SatelliteSystem;
using ostk::astro::trajectory::Sequence;
using ostk::astro::trajectory::State;

class OpenSpaceToolkit_Astrodynamics_Validation_MissionSequence : public ::testing::Test
{
   protected:
    const Dictionary defaultTree_ =
        CntrObject::Load(
            File::Path(Path::Parse("/app/validation/OpenSpaceToolkit/Astrodynamics/data/test/test_mission_sequence.yaml"
            )),
            CntrObject::Format::YAML
        )
            .accessDictionary();

    MissionSequence defaultMissionSequence_ = MissionSequence(defaultTree_);

    const ToolComparison toolComparisonOREKIT_PosVelOnly_ = {
        Tool::OREKIT,
        {
            {Quantity::CARTESIAN_POSITION_GCRF, 1.0e-0},
            {Quantity::CARTESIAN_VELOCITY_GCRF, 1.0e-3},
        },
    };
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_MissionSequence, Constructor)
{
    {
        EXPECT_NO_THROW(MissionSequence missionSequence(defaultTree_));
    }

    {
        EXPECT_NO_THROW(defaultMissionSequence_.accessSatelliteSystem());
        EXPECT_NO_THROW(defaultMissionSequence_.accessInitialState());
        EXPECT_NO_THROW(defaultMissionSequence_.accessEnvironment());
        EXPECT_NO_THROW(defaultMissionSequence_.accessDynamics());
        EXPECT_NO_THROW(defaultMissionSequence_.accessSequence());
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    defaultMissionSequence_.accessSolvedStates();
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("No solved states defined.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_MissionSequence, Run)
{
    {
        EXPECT_NO_THROW(defaultMissionSequence_.run());
    }

    {
        EXPECT_NO_THROW(defaultMissionSequence_.accessSatelliteSystem());
        EXPECT_NO_THROW(defaultMissionSequence_.accessInitialState());
        EXPECT_NO_THROW(defaultMissionSequence_.accessEnvironment());
        EXPECT_NO_THROW(defaultMissionSequence_.accessDynamics());
        EXPECT_NO_THROW(defaultMissionSequence_.accessSequence());
        EXPECT_NO_THROW(defaultMissionSequence_.accessSolvedStates());
    }

    {
        const Array<State> solvedStates = defaultMissionSequence_.accessSolvedStates();
        EXPECT_EQ(solvedStates.getSize(), 3);
        EXPECT_EQ(solvedStates[0].getInstant(), Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC));
        EXPECT_EQ(solvedStates[1].getInstant(), Instant::DateTime(DateTime(2023, 1, 1, 0, 2, 0), Scale::UTC));
        EXPECT_EQ(solvedStates[2].getInstant(), Instant::DateTime(DateTime(2023, 1, 1, 0, 4, 0), Scale::UTC));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Validation_MissionSequence, ComapreResults)
{
    const Table table = {
        {
            "Seconds since epoch",
            "Column A",
            "Column B",
            "Column C",
            "Column D",
            "Column E",
            "Column F",
        },
        {
            {
                CntrObject::Real(0.0),
                CntrObject::Real(1.0),
                CntrObject::Real(2.0),
                CntrObject::Real(3.0),
                CntrObject::Real(4.0),
                CntrObject::Real(5.0),
                CntrObject::Real(6.0),
            },
            {
                CntrObject::Real(0.0),
                CntrObject::Real(1.0),
                CntrObject::Real(2.0),
                CntrObject::Real(3.0),
                CntrObject::Real(4.0),
                CntrObject::Real(5.0),
                CntrObject::Real(6.0),
            },
            {
                CntrObject::Real(0.0),
                CntrObject::Real(1.0),
                CntrObject::Real(2.0),
                CntrObject::Real(3.0),
                CntrObject::Real(4.0),
                CntrObject::Real(5.0),
                CntrObject::Real(6.0),
            },
        }
    };

    const Table tableWrongLength = {
        {
            "Seconds since epoch",
            "Column A",
            "Column B",
            "Column C",
            "Column D",
            "Column E",
            "Column F",
        },
        {{
            CntrObject::Real(0.0),
            CntrObject::Real(1.0),
            CntrObject::Real(2.0),
            CntrObject::Real(3.0),
            CntrObject::Real(4.0),
            CntrObject::Real(5.0),
            CntrObject::Real(6.0),
        }}
    };

    // Need to run the mission sequence first to populate solvedStates_
    {
        EXPECT_THROW(
            {
                try
                {
                    defaultMissionSequence_.compareResults(table, toolComparisonOREKIT_PosVelOnly_);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("No solved states defined.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    defaultMissionSequence_.run();

    {
        EXPECT_THROW(
            {
                try
                {
                    defaultMissionSequence_.compareResults(tableWrongLength, toolComparisonOREKIT_PosVelOnly_);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Number of reference outputs does not match number of solved states.", e.getMessage());
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_NO_THROW(defaultMissionSequence_.compareResults(table, toolComparisonOREKIT_PosVelOnly_));
    }

    {
        const Array<VectorXd> allDeltasWithTool =
            defaultMissionSequence_.compareResults(table, toolComparisonOREKIT_PosVelOnly_);

        EXPECT_EQ(allDeltasWithTool.getSize(), defaultMissionSequence_.accessSolvedStates().getSize());

        for (const auto& deltaWithTool : allDeltasWithTool)
        {
            EXPECT_EQ(deltaWithTool.size(), 2);
        }
    }
}
