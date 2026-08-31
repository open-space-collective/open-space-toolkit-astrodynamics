/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/RANSAC.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::estimator::ObservationFilter;
using ostk::astrodynamics::estimator::observationfilter::RANSAC;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_RANSAC : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        const Table observationData = Table::Load(
            File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolverData/"
                                   "bad_observations.csv")),
            Table::Format::CSV,
            true
        );

        for (const auto& observationRow : observationData)
        {
            const Instant instant = Instant::DateTime(DateTime::Parse(observationRow[0].accessString()), Scale::UTC);
            const Position position = Position::Meters(
                {observationRow[1].accessReal(), observationRow[2].accessReal(), observationRow[3].accessReal()},
                Frame::GCRF()
            );
            const Velocity velocity = Velocity::MetersPerSecond(
                {observationRow[4].accessReal(), observationRow[5].accessReal(), observationRow[6].accessReal()},
                Frame::GCRF()
            );

            observationStates_.add(State(instant, position, velocity));
        }
    }

    Environment environment_ = Environment(std::make_shared<Earth>(Earth::EGM96(10, 10)), {});
    NumericalSolver numericalSolver_ = NumericalSolver::Default();

    Array<State> observationStates_ = Array<State>::Empty();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_RANSAC, Constructor)
{
    {
        EXPECT_NO_THROW(RANSAC ransac(environment_, numericalSolver_); (void)ransac;);
    }

    {
        EXPECT_NO_THROW(RANSAC ransac(environment_); (void)ransac;);
    }

    {
        EXPECT_THROW({ RANSAC ransac(environment_, numericalSolver_, -1.0); }, ostk::core::error::RuntimeError);
    }

    {
        EXPECT_THROW({ RANSAC ransac(environment_, numericalSolver_, 1.0e3, 0); }, ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_RANSAC, Accessors)
{
    const Real threshold = 1.0e3;
    const Size maxIter = 80;
    const Real minRatio = 0.6;
    const Size seed = 42;

    const RANSAC filter(environment_, numericalSolver_, threshold, maxIter, minRatio, seed);

    EXPECT_TRUE(filter.accessPropagator().isDefined());
    EXPECT_DOUBLE_EQ(filter.getInlierThreshold(), threshold);
    EXPECT_EQ(filter.getMaxIterationCount(), maxIter);
    EXPECT_DOUBLE_EQ(filter.getMinInlierRatio(), minRatio);
    EXPECT_EQ(filter.getRandomSeed(), seed);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_RANSAC, EmptyInput)
{
    const RANSAC filter(environment_, numericalSolver_);

    const Shared<const ObservationFilter::Analysis> analysisSPtr = filter.filter(Array<State>::Empty());

    ASSERT_NE(analysisSPtr, nullptr);
    EXPECT_EQ(analysisSPtr->inlierMask.size(), 0);
    EXPECT_EQ(analysisSPtr->getInlierCount(), 0u);
    EXPECT_EQ(analysisSPtr->getOutlierCount(), 0u);
    EXPECT_TRUE(analysisSPtr->getFilteredObservations(Array<State>::Empty()).isEmpty());

    const auto ransacAnalysisSPtr = std::dynamic_pointer_cast<const RANSAC::Analysis>(analysisSPtr);
    ASSERT_NE(ransacAnalysisSPtr, nullptr);
    EXPECT_EQ(ransacAnalysisSPtr->iterationCount, 0u);
    EXPECT_FALSE(ransacAnalysisSPtr->earlyTermination);
    EXPECT_EQ(ransacAnalysisSPtr->bestInlierCount, 0u);
    EXPECT_EQ(ransacAnalysisSPtr->seedFailureCount, 0u);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_RANSAC, Filter_BadObservations)
{
    ASSERT_EQ(observationStates_.getSize(), 1477u);

    const Real inlierThreshold = 5.0e3;  // 5 km — comfortably above propagator self-consistency error
    const Size maxIterationCount = 5;    // RANSAC iterations
    const Real minInlierRatio = 0.9;     // early termination at 90% inliers (well under the ~97% achievable)
    const Size randomSeed = 42;          // deterministic test

    const RANSAC filter(environment_, numericalSolver_, inlierThreshold, maxIterationCount, minInlierRatio, randomSeed);

    const Shared<const ObservationFilter::Analysis> analysisSPtr = filter.filter(observationStates_);

    ASSERT_NE(analysisSPtr, nullptr);
    EXPECT_EQ(static_cast<Size>(analysisSPtr->inlierMask.size()), observationStates_.getSize());
    EXPECT_EQ(analysisSPtr->getInlierCount(), 1477u - 45u);
    EXPECT_EQ(analysisSPtr->getOutlierCount(), 45u);

    const Array<State> filteredObservations = analysisSPtr->getFilteredObservations(observationStates_);
    EXPECT_EQ(filteredObservations.getSize(), 1477u - 45u);

    const auto ransacAnalysisSPtr = std::dynamic_pointer_cast<const RANSAC::Analysis>(analysisSPtr);
    ASSERT_NE(ransacAnalysisSPtr, nullptr);
    EXPECT_GE(ransacAnalysisSPtr->iterationCount, 1u);
    EXPECT_LE(ransacAnalysisSPtr->iterationCount, maxIterationCount);
    EXPECT_TRUE(ransacAnalysisSPtr->earlyTermination)
        << "Expected early termination at " << minInlierRatio << " inlier ratio";
    EXPECT_EQ(ransacAnalysisSPtr->bestInlierCount, 1477u - 45u);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_RANSAC, Print)
{
    const RANSAC filter(environment_, numericalSolver_, 5.0e3, 1, 0.0, 42);
    const Shared<const ObservationFilter::Analysis> analysisSPtr = filter.filter(observationStates_);

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(std::cout << *analysisSPtr << std::endl);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}
