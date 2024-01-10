/// Apache License 2.0

#include <gmock/gmock.h>

#include <OpenSpaceToolkit/Core/Containers/Table.hpp>
#include <OpenSpaceToolkit/Core/Types/Real.hpp>
#include <OpenSpaceToolkit/Core/Types/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Objects/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinatesSubsets/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::types::Real;
using ostk::core::types::String;
using ostk::core::types::Shared;
using ostk::core::types::Index;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::ctnr::Array;
using ostk::core::ctnr::Table;
using ostk::core::ctnr::Tuple;

using ostk::math::object::MatrixXd;
using ostk::math::object::VectorXd;

using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::coord::Frame;

using ostk::astro::dynamics::Tabulated;
using ostk::astro::trajectory::state::CoordinatesSubset;
using ostk::astro::trajectory::state::coordinatessubsets::CartesianVelocity;

class OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated : public ::testing::Test
{
    void SetUp() override
    {
        contributionProfile_.resize(defaultInstants_.getSize(), 3);
        contributionProfile_ << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0;
    }

   protected:
    const Array<Instant> defaultInstants_ = {
        Instant::DateTime(DateTime(2018, 01, 01, 0, 0, 0, 000), Scale::UTC),
        Instant::DateTime(DateTime(2018, 01, 01, 0, 1, 0, 000), Scale::UTC),
        Instant::DateTime(DateTime(2018, 01, 01, 0, 2, 0, 000), Scale::UTC),
        Instant::DateTime(DateTime(2018, 01, 01, 0, 3, 0, 000), Scale::UTC),
    };
    const Array<Shared<const CoordinatesSubset>> defaultWriteCoordinatesSubsets_ = {CartesianVelocity::Default()};

    MatrixXd contributionProfile_;

    Tuple<Array<Instant>, MatrixXd> loadData(const String& aPath)
    {
        const File file = File::Path(Path::Parse(aPath));

        const Table referenceData = Table::Load(file, Table::Format::CSV, false);

        Array<Instant> instants = Array<Instant>::Empty();
        instants.reserve(referenceData.getRowCount());

        MatrixXd contributionProfile(referenceData.getRowCount(), referenceData.getColumnCount() - 1);

        Index i = 0;
        for (const auto& referenceRow : referenceData)
        {
            instants.add(Instant::DateTime(DateTime::Parse(referenceRow[0].accessString()), Scale::UTC));

            VectorXd row(referenceData.getColumnCount());
            for (Index j = 1; j < referenceData.getColumnCount(); ++j)
            {
                row[j - 1] = referenceRow[j].accessReal();
            }

            contributionProfile.row(i) = row;
            ++i;
        }

        return {instants, contributionProfile};
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, Constructor)
{
    {
        EXPECT_NO_THROW(Tabulated(defaultInstants_, contributionProfile_, defaultWriteCoordinatesSubsets_));
    }

    {
        EXPECT_THROW(
            Tabulated({}, contributionProfile_, defaultWriteCoordinatesSubsets_), ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            Tabulated(defaultInstants_, contributionProfile_, {CoordinatesSubset::Mass()}),
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, StreamOperator)
{
    testing::internal::CaptureStdout();

    Tabulated tabulated = {
        defaultInstants_,
        contributionProfile_,
        defaultWriteCoordinatesSubsets_,
    };

    EXPECT_NO_THROW(std::cout << tabulated << std::endl);

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, Print)
{
    testing::internal::CaptureStdout();

    Tabulated tabulated = {
        defaultInstants_,
        contributionProfile_,
        defaultWriteCoordinatesSubsets_,
    };

    EXPECT_NO_THROW(tabulated.print(std::cout, true));
    EXPECT_NO_THROW(tabulated.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, AccessInstants)
{
    Tabulated tabulated = {
        defaultInstants_,
        contributionProfile_,
        defaultWriteCoordinatesSubsets_,
    };
    EXPECT_EQ(tabulated.accessInstants(), defaultInstants_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, GetInstants)
{
    Tabulated tabulated = {
        defaultInstants_,
        contributionProfile_,
        defaultWriteCoordinatesSubsets_,
    };
    EXPECT_EQ(tabulated.getInstants(), defaultInstants_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, AccessContributionProfile)
{
    Tabulated tabulated = {
        defaultInstants_,
        contributionProfile_,
        defaultWriteCoordinatesSubsets_,
    };
    EXPECT_EQ(tabulated.accessContributionProfile(), contributionProfile_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, GetContributionProfile)
{
    Tabulated tabulated = {
        defaultInstants_,
        contributionProfile_,
        defaultWriteCoordinatesSubsets_,
    };
    EXPECT_EQ(tabulated.getContributionProfile(), contributionProfile_);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, ComputeContribution)
{
    {
        MatrixXd contributionProfile;
        Array<Instant> instants;
        std::tie(instants, contributionProfile) =
            loadData("/app/test/OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated_Earth_Gravity.csv");

        const Tabulated tabulated = {
            instants,
            contributionProfile,
            defaultWriteCoordinatesSubsets_,
        };

        MatrixXd expectedProfile;
        Array<Instant> expectedInstants;
        std::tie(expectedInstants, expectedProfile) =
            loadData("/app/test/OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated_Earth_Gravity_Truth.csv");

        const VectorXd x;  // Not used
        for (Index i = 0; i < expectedInstants.getSize(); ++i)
        {
            const VectorXd computedContribution = tabulated.computeContribution(expectedInstants[i], x, Frame::GCRF());
            const VectorXd expectedContribution = expectedProfile.row(i);

            for (Index j = 0; j < (Index)computedContribution.size(); ++j)
            {
                EXPECT_NEAR(computedContribution(j), expectedContribution(j), 1e-8);
            }
        }
    }
}
