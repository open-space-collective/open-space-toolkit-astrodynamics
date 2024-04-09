/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

#include <Global.test.hpp>

using ostk::core::type::Real;
using ostk::core::type::String;
using ostk::core::type::Shared;
using ostk::core::type::Index;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::container::Tuple;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

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
        Instant::DateTime(DateTime(2018, 01, 01, 0, 0, 5, 000), Scale::UTC),
        Instant::DateTime(DateTime(2018, 01, 01, 0, 0, 10, 000), Scale::UTC),
        Instant::DateTime(DateTime(2018, 01, 01, 0, 0, 15, 000), Scale::UTC),
    };
    const Array<Shared<const CoordinateSubset>> defaultWriteCoordinateSubsets_ = {CartesianVelocity::Default()};
    const Shared<const Frame> defaultFrameSPtr_ = Frame::GCRF();
    const Interpolator::Type defaultInterpolationType_ = Interpolator::Type::BarycentricRational;

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

            VectorXd row(contributionProfile.cols());
            for (Index j = 0; j < (Index)row.size(); ++j)
            {
                row[j] = referenceRow[j + 1].accessReal();
            }

            contributionProfile.row(i) = row.transpose();
            ++i;
        }

        return {instants, contributionProfile};
    }
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, Constructor)
{
    {
        EXPECT_NO_THROW(
            Tabulated(defaultInstants_, contributionProfile_, defaultWriteCoordinateSubsets_, defaultFrameSPtr_)
        );
    }

    {
        EXPECT_NO_THROW(Tabulated(
            defaultInstants_,
            contributionProfile_,
            defaultWriteCoordinateSubsets_,
            defaultFrameSPtr_,
            defaultInterpolationType_
        ));
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Tabulated({}, contributionProfile_, defaultWriteCoordinateSubsets_, defaultFrameSPtr_);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(
                        "Contribution profile must have the same number of rows as the number of instants.",
                        e.getMessage()
                    );
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Tabulated(defaultInstants_, contributionProfile_, {CoordinateSubset::Mass()}, defaultFrameSPtr_);
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ(
                        "Contribution profile must have the same number of columns as the sum of the sizes of the "
                        "write coordinate subsets.",
                        e.getMessage()
                    );
                    throw;
                }
            },
            ostk::core::error::RuntimeError
        );
    }

    {
        EXPECT_THROW(
            {
                try
                {
                    Tabulated(defaultInstants_, contributionProfile_, defaultWriteCoordinateSubsets_, Frame::ITRF());
                }
                catch (const ostk::core::error::RuntimeError& e)
                {
                    EXPECT_EQ("Contributions must be expressed in an inertial frame.", e.getMessage());
                    throw;
                }
            },
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
        defaultWriteCoordinateSubsets_,
        defaultFrameSPtr_,
        defaultInterpolationType_,
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
        defaultWriteCoordinateSubsets_,
        defaultFrameSPtr_,
        defaultInterpolationType_,
    };

    EXPECT_NO_THROW(tabulated.print(std::cout, true));
    EXPECT_NO_THROW(tabulated.print(std::cout, false));

    EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, Accessors)
{
    Tabulated tabulated = {
        defaultInstants_,
        contributionProfile_,
        defaultWriteCoordinateSubsets_,
        defaultFrameSPtr_,
    };

    {
        EXPECT_EQ(tabulated.accessInstants(), defaultInstants_);
    }

    {
        EXPECT_EQ(tabulated.accessContributionProfile(), contributionProfile_);
    }

    {
        EXPECT_EQ(tabulated.accessFrame(), defaultFrameSPtr_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, Getters)
{
    MatrixXd positionContributionsProfile_(defaultInstants_.getSize(), 3);
    MatrixXd velocityContributionsProfile_(defaultInstants_.getSize(), 3);
    MatrixXd massContributionsProfile_(defaultInstants_.getSize(), 1);
    positionContributionsProfile_ << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0;
    velocityContributionsProfile_ << -1.0, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0, -8.0, -9.0, -10.0, -11.0, -12.0;
    massContributionsProfile_ << 0.001, 0.002, 0.003, 0.004;

    MatrixXd extendedContributionProfile(defaultInstants_.getSize(), 7);
    extendedContributionProfile << positionContributionsProfile_, velocityContributionsProfile_,
        massContributionsProfile_;

    Tabulated tabulated = {
        defaultInstants_,
        extendedContributionProfile,
        {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()},
        defaultFrameSPtr_,
        defaultInterpolationType_,
    };

    EXPECT_THROW(
        {
            try
            {
                tabulated.getContributionProfileFromCoordinateSubsets({});
            }
            catch (const ostk::core::error::RuntimeError& e)
            {
                EXPECT_EQ("Specified Coordinate Subset array is empty.", e.getMessage());
                throw;
            }
        },
        ostk::core::error::RuntimeError
    );

    EXPECT_THROW(
        {
            try
            {
                tabulated.getContributionProfileFromCoordinateSubsets({CoordinateSubset::DragCoefficient()});
            }
            catch (const ostk::core::error::RuntimeError& e)
            {
                EXPECT_EQ("Coordinate subset not found in write coordinate subsets.", e.getMessage());
                throw;
            }
        },
        ostk::core::error::RuntimeError
    );

    EXPECT_THROW(
        {
            try
            {
                tabulated.getContributionProfileFromCoordinateSubsets(
                    {CartesianPosition::Default(), CoordinateSubset::DragCoefficient()}
                );
            }
            catch (const ostk::core::error::RuntimeError& e)
            {
                EXPECT_EQ("Coordinate subset not found in write coordinate subsets.", e.getMessage());
                throw;
            }
        },
        ostk::core::error::RuntimeError
    );

    // Test singular or correctly ordered subsets
    {
        EXPECT_EQ(
            tabulated.getContributionProfileFromCoordinateSubsets({CartesianPosition::Default()}),
            positionContributionsProfile_
        );

        EXPECT_EQ(
            tabulated.getContributionProfileFromCoordinateSubsets({CartesianVelocity::Default()}),
            velocityContributionsProfile_
        );

        EXPECT_EQ(
            tabulated.getContributionProfileFromCoordinateSubsets({CoordinateSubset::Mass()}), massContributionsProfile_
        );

        EXPECT_EQ(
            tabulated.getContributionProfileFromCoordinateSubsets(
                {CartesianPosition::Default(), CartesianVelocity::Default(), CoordinateSubset::Mass()}
            ),
            extendedContributionProfile
        );
    }

    // Test differently ordered subsets
    {
        {
            MatrixXd differentOrderContributionProfile(defaultInstants_.getSize(), 6);
            differentOrderContributionProfile << velocityContributionsProfile_, positionContributionsProfile_;

            EXPECT_EQ(
                tabulated.getContributionProfileFromCoordinateSubsets(
                    {CartesianVelocity::Default(), CartesianPosition::Default()}
                ),
                differentOrderContributionProfile
            );
        }

        {
            MatrixXd differentOrderContributionProfile(defaultInstants_.getSize(), 4);
            differentOrderContributionProfile << massContributionsProfile_, positionContributionsProfile_;

            EXPECT_EQ(
                tabulated.getContributionProfileFromCoordinateSubsets(
                    {CoordinateSubset::Mass(), CartesianPosition::Default()}
                ),
                differentOrderContributionProfile
            );
        }

        {
            MatrixXd differentOrderContributionProfile(defaultInstants_.getSize(), 4);
            differentOrderContributionProfile << massContributionsProfile_, velocityContributionsProfile_;

            EXPECT_EQ(
                tabulated.getContributionProfileFromCoordinateSubsets(
                    {CoordinateSubset::Mass(), CartesianVelocity::Default()}
                ),
                differentOrderContributionProfile
            );
        }
    }

    {
        EXPECT_EQ(tabulated.getInterpolationType(), defaultInterpolationType_);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Dynamics_Tabulated, ComputeContribution)
{
    const VectorXd x;  // Not used
    {
        Tabulated tabulated = {
            defaultInstants_,
            contributionProfile_,
            defaultWriteCoordinateSubsets_,
            defaultFrameSPtr_,
            defaultInterpolationType_,
        };

        // Wrong frame
        {
            EXPECT_THROW(
                {
                    try
                    {
                        tabulated.computeContribution(defaultInstants_[0], x, Frame::ITRF());
                    }
                    catch (const ostk::core::error::RuntimeError& e)
                    {
                        EXPECT_EQ("Contribution Frame conversion to non-inertial not yet supported.", e.getMessage());
                        throw;
                    }
                },
                ostk::core::error::RuntimeError
            );
        }

        // Outside bounds
        {
            EXPECT_EQ(
                tabulated.computeContribution(defaultInstants_[0] - Duration::Seconds(1.0), x, defaultFrameSPtr_),
                VectorXd::Zero(3)
            );
        }

        {
            EXPECT_EQ(
                tabulated.computeContribution(
                    defaultInstants_[defaultInstants_.getSize() - 1] + Duration::Seconds(1.0), x, defaultFrameSPtr_
                ),
                VectorXd::Zero(3)
            );
        }
    }

    {
        MatrixXd contributionProfile;
        Array<Instant> instants;
        std::tie(instants, contributionProfile) =
            loadData("/app/test/OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated_Earth_Gravity.csv");

        const Tabulated tabulated = {
            instants,
            contributionProfile,
            defaultWriteCoordinateSubsets_,
            defaultFrameSPtr_,
            defaultInterpolationType_,
        };

        MatrixXd expectedProfile;
        Array<Instant> expectedInstants;
        std::tie(expectedInstants, expectedProfile) =
            loadData("/app/test/OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated_Earth_Gravity_Truth.csv");

        for (Index i = 0; i < expectedInstants.getSize(); ++i)
        {
            const VectorXd computedContribution =
                tabulated.computeContribution(expectedInstants[i], x, defaultFrameSPtr_);
            const VectorXd expectedContribution = expectedProfile.row(i);

            for (Index j = 0; j < (Index)computedContribution.size(); ++j)
            {
                EXPECT_NEAR(computedContribution(j), expectedContribution(j), 1e-8);
            }
        }
    }
}
