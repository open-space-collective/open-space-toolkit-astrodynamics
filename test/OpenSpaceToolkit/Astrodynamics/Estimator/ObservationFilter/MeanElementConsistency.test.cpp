/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Table.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/Path.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter/MeanElementConsistency.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolver.hpp>
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

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::VectorXi;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::Environment;
using ostk::physics::environment::object::celestial::Earth;
using ostk::physics::time::DateTime;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Length;

using ostk::astrodynamics::estimator::ObservationFilter;
using ostk::astrodynamics::estimator::observationfilter::MeanElementConsistency;
using ostk::astrodynamics::estimator::OrbitDeterminationSolver;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

class OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency : public ::testing::Test
{
   protected:
    static Array<State> LoadObservations(const String& aPath, const Shared<const Frame>& aFrameSPtr)
    {
        const Table table = Table::Load(File::Path(Path::Parse(aPath)), Table::Format::CSV, true);

        Array<State> observations = Array<State>::Empty();

        for (const auto& row : table)
        {
            observations.add(State(
                Instant::DateTime(DateTime::Parse(row[0].accessString()), Scale::UTC),
                Position::Meters({row[1].accessReal(), row[2].accessReal(), row[3].accessReal()}, aFrameSPtr),
                Velocity::MetersPerSecond({row[4].accessReal(), row[5].accessReal(), row[6].accessReal()}, aFrameSPtr)
            ));
        }

        return observations;
    }

    // Shift the position of one observation radially by the given offset, keeping its velocity and frame.
    static Array<State> CorruptRadially(
        const Array<State>& anObservationArray, const Size& anIndex, const Real& anOffset
    )
    {
        Array<State> corrupted = anObservationArray;

        const State& state = anObservationArray[anIndex];
        const Vector3d position = state.getPosition().getCoordinates();

        corrupted[anIndex] = State(
            state.accessInstant(),
            Position::Meters(position + anOffset * position.normalized(), state.accessFrame()),
            state.getVelocity()
        );

        return corrupted;
    }

    // Every index in [aBegin, anEnd) must be an outlier, every other index an inlier.
    static void ExpectOutlierRange(const VectorXi& anInlierMask, const Size& aBegin, const Size& anEnd)
    {
        for (Size i = 0; i < static_cast<Size>(anInlierMask.size()); ++i)
        {
            const bool isBad = (i >= aBegin) && (i < anEnd);

            EXPECT_EQ(anInlierMask(i), isBad ? 0 : 1) << "at index " << i;
        }
    }

    void SetUp() override
    {
        const State initialState = State(
            Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC),
            Position::Meters({6878.0e3, 0.0, 0.0}, Frame::GCRF()),
            Velocity::MetersPerSecond({0.0, 1000.0, 7500.0}, Frame::GCRF())
        );

        Array<Instant> instants = Array<Instant>::Empty();

        for (Size i = 0; i < 60; ++i)
        {
            instants.add(initialState.accessInstant() + Duration::Seconds(60.0 * static_cast<double>(i)));
        }

        const Propagator propagator = Propagator::FromEnvironment(
            NumericalSolver::Default(), Environment(std::make_shared<Earth>(Earth::EGM2008(2, 0)), {})
        );

        cleanObservations_ = propagator.calculateStatesAt(initialState, instants);

        badObservations_ = LoadObservations(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilterData/bad_observations.csv",
            Frame::GCRF()
        );
        planeOffsetObservations_ = LoadObservations(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilterData/bad_gnss_plane_offset.csv",
            Frame::GCRF()
        );
    }

    const MeanElementConsistency defaultFilter_ = {};

    Array<State> cleanObservations_ = Array<State>::Empty();
    Array<State> badObservations_ = Array<State>::Empty();          // 1477 obs, 36 h, first 45 corrupted
    Array<State> planeOffsetObservations_ = Array<State>::Empty();  // 1749 obs, 25 h, obs 342-442 on a wrong plane
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Constructor)
{
    {
        EXPECT_NO_THROW(MeanElementConsistency filter;);
    }

    {
        EXPECT_NO_THROW(MeanElementConsistency filter(5.0, Length::Meters(500.0)););
    }

    {
        EXPECT_THROW(MeanElementConsistency filter(0.0), ostk::core::error::RuntimeError);
        EXPECT_THROW(MeanElementConsistency filter(-1.0), ostk::core::error::RuntimeError);
        EXPECT_THROW(MeanElementConsistency filter(Real::Undefined()), ostk::core::error::RuntimeError);
        EXPECT_THROW(MeanElementConsistency filter(10.0, Length::Undefined()), ostk::core::error::RuntimeError);
        EXPECT_THROW(MeanElementConsistency filter(10.0, Length::Meters(0.0)), ostk::core::error::RuntimeError);
        EXPECT_THROW(MeanElementConsistency filter(10.0, Length::Meters(-1.0)), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Getters)
{
    {
        EXPECT_EQ(defaultFilter_.getGateFactor(), 10.0);
        EXPECT_EQ(defaultFilter_.getMinimumDeviation(), Length::Kilometers(2.0));
    }

    {
        const MeanElementConsistency filter(5.0, Length::Meters(500.0));

        EXPECT_EQ(filter.getGateFactor(), 5.0);
        EXPECT_EQ(filter.getMinimumDeviation(), Length::Meters(500.0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, ChannelNames)
{
    {
        const Array<String> channelNames = MeanElementConsistency::ChannelNames();

        ASSERT_EQ(channelNames.getSize(), 6);
        EXPECT_EQ(channelNames[0], "semi_major_axis");
        EXPECT_EQ(channelNames[5], "phase");
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Analysis)
{
    {
        VectorXi inlierMask(2);
        inlierMask << 1, 0;

        const MeanElementConsistency::Analysis analysis(inlierMask, MatrixXd::Zero(2, 6), VectorXd::Ones(6));

        EXPECT_EQ(analysis.getInlierCount(), 1);
        EXPECT_EQ(analysis.getOutlierCount(), 1);
        EXPECT_EQ(analysis.deviations.rows(), 2);
        EXPECT_EQ(analysis.deviations.cols(), 6);
        EXPECT_EQ(analysis.gates.size(), 6);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(std::cout << analysis << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_Empty)
{
    {
        const Shared<const ObservationFilter::Analysis> analysis = defaultFilter_.filter(Array<State>::Empty());

        const Shared<const MeanElementConsistency::Analysis> meanElementAnalysis =
            std::dynamic_pointer_cast<const MeanElementConsistency::Analysis>(analysis);

        ASSERT_NE(meanElementAnalysis, nullptr);

        EXPECT_EQ(meanElementAnalysis->inlierMask.size(), 0);
        EXPECT_EQ(meanElementAnalysis->deviations.rows(), 0);
        EXPECT_EQ(meanElementAnalysis->deviations.cols(), 6);
        EXPECT_EQ(meanElementAnalysis->gates, VectorXd::Constant(6, 2000.0));
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_SingleObservation)
{
    {
        const Shared<const ObservationFilter::Analysis> analysis = defaultFilter_.filter({cleanObservations_[0]});

        EXPECT_EQ(analysis->getInlierCount(), 1);
        EXPECT_EQ(analysis->getOutlierCount(), 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_CleanObservations)
{
    {
        const Shared<const MeanElementConsistency::Analysis> analysis =
            std::dynamic_pointer_cast<const MeanElementConsistency::Analysis>(defaultFilter_.filter(cleanObservations_)
            );

        ASSERT_NE(analysis, nullptr);

        EXPECT_EQ(analysis->getInlierCount(), cleanObservations_.getSize());
        EXPECT_EQ(analysis->getOutlierCount(), 0);
        EXPECT_EQ(analysis->deviations.rows(), static_cast<int>(cleanObservations_.getSize()));
        EXPECT_TRUE(analysis->deviations.allFinite());

        for (int channel = 0; channel < 6; ++channel)
        {
            EXPECT_GE(analysis->gates(channel), 2000.0);
            EXPECT_LE(analysis->deviations.col(channel).cwiseAbs().maxCoeff(), analysis->gates(channel));
        }
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_CorruptedObservation)
{
    {
        const Array<State> observations = CorruptRadially(cleanObservations_, 30, 20.0e3);

        const Shared<const MeanElementConsistency::Analysis> analysis =
            std::dynamic_pointer_cast<const MeanElementConsistency::Analysis>(defaultFilter_.filter(observations));

        ASSERT_NE(analysis, nullptr);

        EXPECT_EQ(analysis->getOutlierCount(), 1);
        EXPECT_EQ(analysis->inlierMask(30), 0);
        EXPECT_GT(std::abs(analysis->deviations(30, 0)), 2000.0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_UndefinedObservation)
{
    {
        EXPECT_THROW(
            defaultFilter_.filter({cleanObservations_[0], State::Undefined()}), ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_BadObservationsDataset)
{
    {
        const Shared<const ObservationFilter::Analysis> analysis = defaultFilter_.filter(badObservations_);

        EXPECT_EQ(analysis->getOutlierCount(), 45);
        ExpectOutlierRange(analysis->inlierMask, 0, 45);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_PlaneOffsetDataset)
{
    {
        const Shared<const ObservationFilter::Analysis> analysis = defaultFilter_.filter(planeOffsetObservations_);

        EXPECT_EQ(analysis->getOutlierCount(), 101);
        ExpectOutlierRange(analysis->inlierMask, 342, 443);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Filter_TuningPlateau)
{
    // The classification does not depend on the exact tuning over a wide range.

    for (const MeanElementConsistency& filter : {
             MeanElementConsistency(3.0, Length::Kilometers(2.0)),
             MeanElementConsistency(30.0, Length::Kilometers(2.0)),
             MeanElementConsistency(10.0, Length::Kilometers(1.5)),
             MeanElementConsistency(10.0, Length::Kilometers(5.0)),
         })
    {
        ExpectOutlierRange(filter.filter(badObservations_)->inlierMask, 0, 45);
        ExpectOutlierRange(filter.filter(planeOffsetObservations_)->inlierMask, 342, 443);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, Apply)
{
    {
        const Array<State> filteredObservations = defaultFilter_.apply(badObservations_);

        ASSERT_EQ(filteredObservations.getSize(), 1432);
        EXPECT_EQ(filteredObservations[0], badObservations_[45]);
    }

    {
        EXPECT_EQ(defaultFilter_.apply(planeOffsetObservations_).getSize(), 1648);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency, OrbitDeterminationPreStep)
{
    // Observations in ITRF are converted internally. Filtering before orbit determination removes a 20 km
    // corrupted observation and improves the fit.

    const Array<State> observations = CorruptRadially(
        LoadObservations(
            "/app/test/OpenSpaceToolkit/Astrodynamics/Estimator/OrbitDeterminationSolverData/gnss_data.csv",
            Frame::ITRF()
        ),
        5,
        20.0e3
    );

    const Array<State> filteredObservations = defaultFilter_.apply(observations);

    ASSERT_EQ(filteredObservations.getSize(), observations.getSize() - 1);
    EXPECT_EQ(filteredObservations[5], observations[6]);

    const OrbitDeterminationSolver solver(Environment(std::make_shared<Earth>(Earth::EGM96(10, 10)), {}));

    const State initialGuess = filteredObservations[0].inFrame(Frame::GCRF());

    const OrbitDeterminationSolver::Analysis unfilteredAnalysis = solver.estimate(initialGuess, observations);
    const OrbitDeterminationSolver::Analysis filteredAnalysis = solver.estimate(initialGuess, filteredObservations);

    EXPECT_LT(filteredAnalysis.solverAnalysis.rmsError, unfilteredAnalysis.solverAnalysis.rmsError);
    EXPECT_LT(filteredAnalysis.solverAnalysis.rmsError, 100.0);
}
