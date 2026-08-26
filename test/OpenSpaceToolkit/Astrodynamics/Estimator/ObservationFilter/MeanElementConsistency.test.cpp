/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/MeanElementConsistency.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::container::Table;
using ostk::core::filesystem::File;
using ostk::core::filesystem::Path;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::estimator::ObservationFilter;
using ostk::astrodynamics::estimator::observationfilter::MeanElementConsistency;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;

namespace
{

Array<State> LoadObservations(const String& aPath, const Shared<const Frame>& aFrameSPtr)
{
    const Table observationData = Table::Load(File::Path(Path::Parse(aPath)), Table::Format::CSV, true);

    Array<State> observationStates = Array<State>::Empty();

    for (const auto& observationRow : observationData)
    {
        const Instant instant = Instant::DateTime(DateTime::Parse(observationRow[0].accessString()), Scale::UTC);
        const Position position = Position::Meters(
            {observationRow[1].accessReal(), observationRow[2].accessReal(), observationRow[3].accessReal()}, aFrameSPtr
        );
        const Velocity velocity = Velocity::MetersPerSecond(
            {observationRow[4].accessReal(), observationRow[5].accessReal(), observationRow[6].accessReal()}, aFrameSPtr
        );

        observationStates.add(State(instant, position, velocity));
    }

    return observationStates;
}

State withPositionOffset(const State& aState, const Vector3d& anOffset)
{
    return State(
        aState.accessInstant(),
        Position::Meters(aState.getPosition().getCoordinates() + anOffset, aState.accessFrame()),
        aState.getVelocity()
    );
}

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        observationStates_ = LoadObservations(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolverData/bad_observations.csv",
            Frame::GCRF()
        );
    }

    Array<State> observationStates_ = Array<State>::Empty();
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Constructor)
{
    {
        EXPECT_NO_THROW(MeanElementConsistency filter; (void)filter;);
    }

    {
        EXPECT_NO_THROW(MeanElementConsistency filter(15.0, 5.0); (void)filter;);
    }

    {
        // Non-positive gate factor
        EXPECT_THROW({ MeanElementConsistency filter(0.0); }, ostk::core::error::RuntimeError);
        EXPECT_THROW({ MeanElementConsistency filter(-1.0); }, ostk::core::error::RuntimeError);
    }

    {
        // Negative noise floor
        EXPECT_THROW(
            { MeanElementConsistency filter(DEFAULT_MEAN_ELEMENT_CONSISTENCY_GATE_FACTOR, -1.0); },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Accessors)
{
    const MeanElementConsistency filter(12.0, 5.0);

    EXPECT_DOUBLE_EQ(filter.getGateFactor(), 12.0);
    EXPECT_DOUBLE_EQ(filter.getNoiseFloor(), 5.0);
    EXPECT_TRUE(filter.getGravitationalParameter().isDefined());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, EmptyInput)
{
    const MeanElementConsistency filter;

    const Shared<const ObservationFilter::Analysis> analysisSPtr = filter.filter(Array<State>::Empty());

    ASSERT_NE(analysisSPtr, nullptr);
    EXPECT_EQ(analysisSPtr->inlierMask.size(), 0);
    EXPECT_EQ(analysisSPtr->getInlierCount(), 0u);
    EXPECT_EQ(analysisSPtr->getOutlierCount(), 0u);
    EXPECT_TRUE(analysisSPtr->getFilteredObservations(Array<State>::Empty()).isEmpty());

    const auto analysis = std::dynamic_pointer_cast<const MeanElementConsistency::Analysis>(analysisSPtr);
    ASSERT_NE(analysis, nullptr);
    EXPECT_EQ(analysis->channelResiduals.rows(), 0);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_BadObservations)
{
    // 36 h LEO arc; the first 45 observations are corrupted (tens of km off) — ground truth.
    ASSERT_EQ(observationStates_.getSize(), 1477u);

    const MeanElementConsistency filter;

    const Shared<const ObservationFilter::Analysis> analysisSPtr = filter.filter(observationStates_);

    ASSERT_NE(analysisSPtr, nullptr);
    EXPECT_EQ(static_cast<Size>(analysisSPtr->inlierMask.size()), observationStates_.getSize());
    EXPECT_EQ(analysisSPtr->getOutlierCount(), 45u);
    EXPECT_EQ(analysisSPtr->getInlierCount(), 1477u - 45u);

    // Exactly the corrupted block is rejected
    EXPECT_EQ(analysisSPtr->inlierMask.head(45).sum(), 0);
    EXPECT_EQ(analysisSPtr->inlierMask.tail(1477 - 45).sum(), 1477 - 45);

    const auto analysis = std::dynamic_pointer_cast<const MeanElementConsistency::Analysis>(analysisSPtr);
    ASSERT_NE(analysis, nullptr);
    EXPECT_EQ(static_cast<Size>(analysis->channelResiduals.rows()), observationStates_.getSize());
    EXPECT_EQ(static_cast<Size>(analysis->channelResiduals.cols()), MeanElementConsistency::ChannelCount);
    EXPECT_EQ(static_cast<Size>(analysis->appliedChannelGates.size()), MeanElementConsistency::ChannelCount);
    EXPECT_TRUE((analysis->appliedChannelGates.array() > 0.0).all());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_BadGNSSPlaneOffset)
{
    // A 25.5 h GNSS arc mixing 1 Hz bursts and 120 s sampling. Observations [342, 442] — a
    // contiguous 7.3 h block — lie on an orbital plane rotated by a constant ~0.073 deg from the
    // plane traced by the rest of the arc. The block is internally self-consistent and its
    // position residual against the true trajectory passes through zero twice per orbit — but its
    // orbit plane (the node channel) is offset at every single point, so the mean-element filter
    // rejects the whole block, including the zero crossings.
    const Array<State> gnssObservationStates = LoadObservations(
        "/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilterData/bad_gnss_plane_offset.csv",
        Frame::GCRF()
    );

    ASSERT_EQ(gnssObservationStates.getSize(), 1749u);

    const Eigen::Index corruptedBlockStart = 342;
    const Eigen::Index corruptedBlockSize = 101;

    const MeanElementConsistency filter;

    const Shared<const ObservationFilter::Analysis> analysisSPtr = filter.filter(gnssObservationStates);

    ASSERT_NE(analysisSPtr, nullptr);
    EXPECT_EQ(static_cast<Size>(analysisSPtr->inlierMask.size()), gnssObservationStates.getSize());
    EXPECT_EQ(analysisSPtr->getOutlierCount(), 101u);

    // Every rejected observation lies inside the corrupted block: no good observation is discarded.
    EXPECT_EQ(analysisSPtr->inlierMask.head(corruptedBlockStart).sum(), corruptedBlockStart);
    EXPECT_EQ(analysisSPtr->inlierMask.segment(corruptedBlockStart, corruptedBlockSize).sum(), 0);
    EXPECT_EQ(
        analysisSPtr->inlierMask.tail(analysisSPtr->inlierMask.size() - corruptedBlockStart - corruptedBlockSize).sum(),
        analysisSPtr->inlierMask.size() - corruptedBlockStart - corruptedBlockSize
    );
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_ShortArcGlitch)
{
    // 11 good observations at 60 s cadence (10 min arc)
    const Array<State> shortArc(observationStates_.begin() + 45, observationStates_.begin() + 56);
    ASSERT_EQ(shortArc.getSize(), 11u);

    const MeanElementConsistency filter;

    {
        // Clean arc: nothing is rejected
        const auto analysisSPtr = filter.filter(shortArc);
        EXPECT_EQ(analysisSPtr->getOutlierCount(), 0u);
    }

    {
        // 300 m along-track glitch on the middle observation: only that observation is rejected
        Array<State> glitchedArc = shortArc;
        const Vector3d alongTrack = glitchedArc[5].getVelocity().getCoordinates().normalized();
        glitchedArc[5] = withPositionOffset(glitchedArc[5], 300.0 * alongTrack);

        const auto analysisSPtr = filter.filter(glitchedArc);

        EXPECT_EQ(analysisSPtr->getOutlierCount(), 1u);
        EXPECT_EQ(analysisSPtr->inlierMask(5), 0);
    }

    {
        // 300 m cross-track glitch
        Array<State> glitchedArc = shortArc;
        const Vector3d crossTrack = glitchedArc[5]
                                        .getPosition()
                                        .getCoordinates()
                                        .cross(glitchedArc[5].getVelocity().getCoordinates())
                                        .normalized();
        glitchedArc[5] = withPositionOffset(glitchedArc[5], 300.0 * crossTrack);

        const auto analysisSPtr = filter.filter(glitchedArc);

        EXPECT_EQ(analysisSPtr->getOutlierCount(), 1u);
        EXPECT_EQ(analysisSPtr->inlierMask(5), 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_UnsortedInput)
{
    // The filter must not require time-ordered observations: the mask stays aligned with the input.
    Array<State> shortArc(observationStates_.begin() + 45, observationStates_.begin() + 56);
    const Vector3d alongTrack = shortArc[5].getVelocity().getCoordinates().normalized();
    shortArc[5] = withPositionOffset(shortArc[5], 1000.0 * alongTrack);

    Array<State> unsortedArc = shortArc;
    std::swap(unsortedArc[0], unsortedArc[5]);  // glitched observation now first

    const MeanElementConsistency filter;

    const auto analysisSPtr = filter.filter(unsortedArc);

    EXPECT_EQ(analysisSPtr->getOutlierCount(), 1u);
    EXPECT_EQ(analysisSPtr->inlierMask(0), 0);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_NonInertialInput)
{
    // Observations provided in a non-inertial frame are converted internally.
    const Array<State> itrfObservations = LoadObservations(
        "/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolverData/gnss_data.csv", Frame::ITRF()
    );

    const MeanElementConsistency filter;

    const auto analysisSPtr = filter.filter(itrfObservations);

    EXPECT_EQ(analysisSPtr->getOutlierCount(), 0u);
    EXPECT_EQ(analysisSPtr->getInlierCount(), itrfObservations.getSize());
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_MissingVelocitySubset)
{
    const MeanElementConsistency filter;

    VectorXd positionCoordinates(3);
    positionCoordinates << 7.0e6, 0.0, 0.0;

    const Array<State> positionOnlyObservations = {
        State(
            Instant::DateTime(DateTime::Parse("2026-05-15 12:14:22.000"), Scale::UTC),
            positionCoordinates,
            Frame::GCRF(),
            {CartesianPosition::Default()}
        ),
    };

    EXPECT_THROW({ filter.filter(positionOnlyObservations); }, ostk::core::error::RuntimeError);
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Print)
{
    const MeanElementConsistency filter;

    const Array<State> shortArc(observationStates_.begin() + 45, observationStates_.begin() + 56);
    const auto analysisSPtr = filter.filter(shortArc);

    testing::internal::CaptureStdout();
    EXPECT_NO_THROW(std::cout << *analysisSPtr << std::endl);
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}
