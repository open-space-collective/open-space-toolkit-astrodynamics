/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Velocity.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::VectorXi;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;

using ostk::astrodynamics::estimator::ObservationFilter;
using ostk::astrodynamics::trajectory::State;

// Minimal concrete filter, showing how a custom strategy plugs into the base class.
class KeepFirstHalf : public ObservationFilter
{
   public:
    Shared<const Analysis> filter(const Array<State>& anObservationStateArray) const override
    {
        VectorXi inlierMask = VectorXi::Zero(anObservationStateArray.getSize());

        for (Size i = 0; i < anObservationStateArray.getSize() / 2; ++i)
        {
            inlierMask(i) = 1;
        }

        return std::make_shared<Analysis>(inlierMask);
    }
};

class OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        for (Size i = 0; i < 10; ++i)
        {
            observations_.add(State(
                Instant::J2000() + Duration::Minutes(static_cast<double>(i)),
                Position::Meters({7.0e6 + static_cast<double>(i), 0.0, 0.0}, Frame::GCRF()),
                Velocity::MetersPerSecond({0.0, 7.5e3, 0.0}, Frame::GCRF())
            ));
        }

        inlierMask_ = VectorXi::Ones(10);
        inlierMask_(2) = 0;
        inlierMask_(7) = 0;
        inlierMask_(8) = 0;
    }

    Array<State> observations_ = Array<State>::Empty();
    VectorXi inlierMask_;
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter, Analysis_Constructor)
{
    {
        EXPECT_NO_THROW(ObservationFilter::Analysis analysis(inlierMask_););
    }

    {
        EXPECT_NO_THROW(ObservationFilter::Analysis analysis(VectorXi(0)););
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter, Analysis_Counts)
{
    {
        const ObservationFilter::Analysis analysis(inlierMask_);

        EXPECT_EQ(analysis.getInlierCount(), 7);
        EXPECT_EQ(analysis.getOutlierCount(), 3);
        EXPECT_EQ(analysis.inlierMask, inlierMask_);
    }

    {
        const ObservationFilter::Analysis analysis(VectorXi(0));

        EXPECT_EQ(analysis.getInlierCount(), 0);
        EXPECT_EQ(analysis.getOutlierCount(), 0);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter, Analysis_GetFilteredObservations)
{
    {
        const ObservationFilter::Analysis analysis(inlierMask_);

        const Array<State> filteredObservations = analysis.getFilteredObservations(observations_);

        ASSERT_EQ(filteredObservations.getSize(), 7);

        EXPECT_EQ(filteredObservations[0], observations_[0]);
        EXPECT_EQ(filteredObservations[1], observations_[1]);
        EXPECT_EQ(filteredObservations[2], observations_[3]);
        EXPECT_EQ(filteredObservations[6], observations_[9]);
    }

    {
        const ObservationFilter::Analysis analysis(VectorXi::Ones(3));

        EXPECT_THROW(analysis.getFilteredObservations(observations_), ostk::core::error::RuntimeError);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter, Analysis_Print)
{
    {
        const ObservationFilter::Analysis analysis(inlierMask_);

        testing::internal::CaptureStdout();

        EXPECT_NO_THROW(analysis.print(std::cout, true));
        EXPECT_NO_THROW(std::cout << analysis << std::endl);

        EXPECT_FALSE(testing::internal::GetCapturedStdout().empty());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter, Filter_DerivedClass)
{
    {
        const KeepFirstHalf filter;

        const Shared<const ObservationFilter::Analysis> analysis = filter.filter(observations_);

        EXPECT_EQ(analysis->getInlierCount(), 5);
        EXPECT_EQ(analysis->getOutlierCount(), 5);
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter, Apply)
{
    {
        const KeepFirstHalf filter;

        const Array<State> filteredObservations = filter.apply(observations_);

        ASSERT_EQ(filteredObservations.getSize(), 5);

        EXPECT_EQ(filteredObservations[0], observations_[0]);
        EXPECT_EQ(filteredObservations[4], observations_[4]);
    }

    {
        const KeepFirstHalf filter;

        EXPECT_EQ(filter.apply(Array<State>::Empty()).getSize(), 0);
    }
}
