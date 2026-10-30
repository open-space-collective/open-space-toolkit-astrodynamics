/// Apache License 2.0

#include <cmath>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Uncertainty/Covariance.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::conjunction::hardbody::Spherical;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;
using ostk::astrodynamics::uncertainty::Covariance;

namespace
{

/// @brief Mock probability of collision algorithm that is always applicable (bogus probability of collision).
class AlwaysApplicableAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    AlwaysApplicableAlgorithm()
        : ProbabilityOfCollisionAlgorithm("Always Applicable", false)
    {
    }

    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override
    {
        (void)aCloseApproach;

        return Array<String>::Empty();
    }

    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override
    {
        (void)aCloseApproach;
        (void)aHardBody1SPtr;
        (void)aHardBody2SPtr;

        return 0.0;
    }
};

/// @brief Mock probability of collision algorithm that is never applicable (bogus probability of collision).
class NeverApplicableAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    NeverApplicableAlgorithm()
        : ProbabilityOfCollisionAlgorithm("Never Applicable", false)
    {
    }

    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override
    {
        (void)aCloseApproach;

        return {"Never applicable"};
    }

    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override
    {
        (void)aCloseApproach;
        (void)aHardBody1SPtr;
        (void)aHardBody2SPtr;

        return 0.0;
    }
};

/// @brief Mock probability of collision algorithm returning a constant probability of collision, whatever the close
/// approach (hence whatever the covariance scaling).
class ConstantProbabilityOfCollisionAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    ConstantProbabilityOfCollisionAlgorithm(const Real& aProbabilityOfCollision)
        : ProbabilityOfCollisionAlgorithm("Constant", false),
          probabilityOfCollision_(aProbabilityOfCollision)
    {
    }

    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override
    {
        (void)aCloseApproach;

        return Array<String>::Empty();
    }

    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override
    {
        (void)aCloseApproach;
        (void)aHardBody1SPtr;
        (void)aHardBody2SPtr;

        return probabilityOfCollision_;
    }

   private:
    Real probabilityOfCollision_;
};

/// @brief Mock probability of collision algorithm computing a maximum probability of collision (i.e. an upper bound),
/// returning a constant value whatever the close approach.
class MaximumProbabilityOfCollisionAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    MaximumProbabilityOfCollisionAlgorithm(const Real& aProbabilityOfCollision)
        : ProbabilityOfCollisionAlgorithm("Maximum", true),
          probabilityOfCollision_(aProbabilityOfCollision)
    {
    }

    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override
    {
        (void)aCloseApproach;

        return Array<String>::Empty();
    }

    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override
    {
        (void)aCloseApproach;
        (void)aHardBody1SPtr;
        (void)aHardBody2SPtr;

        return probabilityOfCollision_;
    }

   private:
    Real probabilityOfCollision_;
};

/// @brief Mock probability of collision algorithm returning a probability of collision that increases with the
/// covariance scaling: log(x), with x the square root of the first diagonal term of the Object 1 covariance.
///
/// @details The maximum probability of collision lies beyond the upper bound of the scaling interval (i.e. the search
/// interval is always "to the left" of the maximum), hence the close approach is in the robust region.
class IncreasingProbabilityOfCollisionAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    IncreasingProbabilityOfCollisionAlgorithm()
        : ProbabilityOfCollisionAlgorithm("Increasing", false)
    {
    }

    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override
    {
        (void)aCloseApproach;

        return Array<String>::Empty();
    }

    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override
    {
        (void)aHardBody1SPtr;
        (void)aHardBody2SPtr;

        return std::log(std::sqrt(aCloseApproach.getObject1State().getCovariance()->getCoordinates()(0, 0)));
    }
};

/// @brief Mock probability of collision algorithm returning a probability of collision that decreases with the
/// covariance scaling: 1 / x, with x the square root of the first diagonal term of the Object 1 covariance.
///
/// @details The maximum probability of collision lies below the lower bound of the scaling interval (i.e. the search
/// interval is always "to the right" of the maximum), hence the close approach is in the dilution region.
class DecreasingProbabilityOfCollisionAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    DecreasingProbabilityOfCollisionAlgorithm()
        : ProbabilityOfCollisionAlgorithm("Decreasing", false)
    {
    }

    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override
    {
        (void)aCloseApproach;

        return Array<String>::Empty();
    }

    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override
    {
        (void)aHardBody1SPtr;
        (void)aHardBody2SPtr;

        return 1.0 / std::sqrt(aCloseApproach.getObject1State().getCovariance()->getCoordinates()(0, 0));
    }
};

/// @brief Mock probability of collision algorithm returning a probability of collision with a single maximum, as a
/// function of x, the square root of the first diagonal term of the Object 1 covariance (i.e. a standard deviation):
/// - Linearly increasing from [0.0, 0.0] to [maximumProbabilityOfCollisionSigma, 0.05]
/// - Linearly decreasing from [maximumProbabilityOfCollisionSigma, 0.05] to [10000.0, 0.0]
class SingleMaximumProbabilityOfCollisionAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    SingleMaximumProbabilityOfCollisionAlgorithm(const Real& aMaximumProbabilityOfCollisionSigma)
        : ProbabilityOfCollisionAlgorithm("Single Maximum", false),
          maximumProbabilityOfCollisionSigma_(aMaximumProbabilityOfCollisionSigma)
    {
    }

    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override
    {
        (void)aCloseApproach;

        return Array<String>::Empty();
    }

    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override
    {
        (void)aHardBody1SPtr;
        (void)aHardBody2SPtr;

        const Real sigma = std::sqrt(aCloseApproach.getObject1State().getCovariance()->getCoordinates()(0, 0));

        if (sigma <= maximumProbabilityOfCollisionSigma_)
        {
            return MaximumProbabilityOfCollision * sigma / maximumProbabilityOfCollisionSigma_;
        }

        if (sigma <= ZeroProbabilityOfCollisionSigma)
        {
            return MaximumProbabilityOfCollision * (ZeroProbabilityOfCollisionSigma - sigma) /
                   (ZeroProbabilityOfCollisionSigma - maximumProbabilityOfCollisionSigma_);
        }

        return 0.0;
    }

    static constexpr double MaximumProbabilityOfCollision = 0.05;
    static constexpr double ZeroProbabilityOfCollisionSigma = 10000.0;

   private:
    Real maximumProbabilityOfCollisionSigma_;
};

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm : public ::testing::Test
{
   protected:
    // Helper function to build a state with a position covariance attached
    State buildState(
        const Instant& anInstant,
        const Shared<const Frame>& aFrameSPtr,
        const Vector3d& aPosition,
        const Vector3d& aVelocity,
        const Vector3d& aPositionSigmas
    ) const
    {
        const StateBuilder stateBuilder = {
            aFrameSPtr,
            {CartesianPosition::Default(), CartesianVelocity::Default()},
        };

        VectorXd coordinates(6);
        coordinates << aPosition, aVelocity;

        return stateBuilder.build(anInstant, coordinates)
            .withCovariance(Covariance::FromPositionSigmas(anInstant, aPositionSigmas, aFrameSPtr));
    }

    // Helper function to build a (head-on) close approach
    CloseApproach buildCloseApproach() const
    {
        const Instant instant = Instant::DateTime(DateTime(2024, 1, 1, 0, 0, 0), Scale::UTC);
        const Shared<const Frame> gcrfFrameSPtr = Frame::GCRF();

        const State object1State = buildState(
            instant, gcrfFrameSPtr, Vector3d(7.0e6, 0.0, 0.0), Vector3d(0.0, 7.5e3, 0.0), Vector3d(10.0, 100.0, 10.0)
        );
        const State object2State = buildState(
            instant, gcrfFrameSPtr, Vector3d(7.0e6, 0.0, 50.0), Vector3d(0.0, -7.5e3, 0.0), Vector3d(20.0, 200.0, 20.0)
        );

        return {object1State, object2State};
    }

    const Shared<const HardBody> hardBody1SPtr_ = std::make_shared<Spherical>(5.0);
    const Shared<const HardBody> hardBody2SPtr_ = std::make_shared<Spherical>(5.0);
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm, GetName)
{
    {
        EXPECT_EQ(AlwaysApplicableAlgorithm().getName(), "Always Applicable");
        EXPECT_EQ(NeverApplicableAlgorithm().getName(), "Never Applicable");
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm, IsMaximum)
{
    {
        EXPECT_FALSE(AlwaysApplicableAlgorithm().isMaximum());
        EXPECT_TRUE(MaximumProbabilityOfCollisionAlgorithm(0.01).isMaximum());
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm, Applicability)
{
    const CloseApproach closeApproach = buildCloseApproach();

    // Always applicable algorithm
    {
        const AlwaysApplicableAlgorithm algorithm = {};

        EXPECT_TRUE(algorithm.isApplicable(closeApproach));
        EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(closeApproach).isEmpty());
    }

    // Never applicable algorithm
    {
        const NeverApplicableAlgorithm algorithm = {};

        EXPECT_FALSE(algorithm.isApplicable(closeApproach));

        const Array<String> unsatisfiedAssumptions = algorithm.identifyUnsatisfiedAssumptions(closeApproach);

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_EQ(unsatisfiedAssumptions[0], "Never applicable");
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm, ComputeProbabilityAnalysisInputs)
{
    const Shared<const AlwaysApplicableAlgorithm> algorithmSPtr = std::make_shared<AlwaysApplicableAlgorithm>();
    const CloseApproach closeApproach = buildCloseApproach();

    // Undefined algorithm
    {
        EXPECT_THROW(
            try {
                ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                    nullptr, closeApproach, hardBody1SPtr_, hardBody2SPtr_
                );
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Probability of collision algorithm} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    // Algorithm computing a maximum probability of collision (i.e. an upper bound)
    {
        EXPECT_THROW(
            try {
                ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                    std::make_shared<MaximumProbabilityOfCollisionAlgorithm>(0.01),
                    closeApproach,
                    hardBody1SPtr_,
                    hardBody2SPtr_
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ(
                    "The probability of collision is already an upper bound: the analysis is not applicable.",
                    e.getMessage()
                );
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // No covariance to scale
    {
        EXPECT_THROW(
            try {
                ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                    algorithmSPtr, closeApproach, hardBody1SPtr_, hardBody2SPtr_, false, false
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("At least one covariance must be scalable to find the probability region.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm, ComputeProbabilityAnalysis)
{
    // Probability of collision without a maximum: the root solver cannot find a sign change of the slope, hence the
    // region is unknown and the maximum probability of collision is empty
    {
        // Constant probability of collision (zero slope): the bracketing never finds a sign change
        const Shared<const ProbabilityOfCollisionAlgorithm> constantAlgorithmSPtr =
            std::make_shared<ConstantProbabilityOfCollisionAlgorithm>(0.01);

        // Increasing probability of collision (positive slope): the bracketing walks upwards without end
        const Shared<const ProbabilityOfCollisionAlgorithm> increasingAlgorithmSPtr =
            std::make_shared<IncreasingProbabilityOfCollisionAlgorithm>();

        // Decreasing probability of collision (negative slope): the bracketing walks downwards without end
        const Shared<const ProbabilityOfCollisionAlgorithm> decreasingAlgorithmSPtr =
            std::make_shared<DecreasingProbabilityOfCollisionAlgorithm>();

        for (const auto& [algorithmSPtr, expectedProbabilityOfCollision] :
             std::vector<std::pair<Shared<const ProbabilityOfCollisionAlgorithm>, Real>> {
                 {constantAlgorithmSPtr, 0.01},
                 {increasingAlgorithmSPtr, std::log(10.0)},
                 {decreasingAlgorithmSPtr, 1.0 / 10.0},
             })
        {
            const ProbabilityOfCollisionAlgorithm::Analysis analysis =
                ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                    algorithmSPtr, buildCloseApproach(), hardBody1SPtr_, hardBody2SPtr_
                );

            EXPECT_EQ(analysis.region, ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Unknown)
                << algorithmSPtr->getName();
            EXPECT_DOUBLE_EQ(analysis.probabilityOfCollision, expectedProbabilityOfCollision)
                << algorithmSPtr->getName();
            EXPECT_FALSE(analysis.maximumProbabilityOfCollision.has_value()) << algorithmSPtr->getName();
            EXPECT_FALSE(analysis.maximumProbabilityOfCollisionScalingFactor.has_value()) << algorithmSPtr->getName();
            EXPECT_FALSE(analysis.maximumProbabilityOfCollisionSigmaScalingFactor.has_value())
                << algorithmSPtr->getName();
        }
    }

    // Single maximum to the right of the unscaled covariance (sigma of 10 m, maximum at a sigma of 100 m), hence the
    // close approach is in the robust region
    {
        const ProbabilityOfCollisionAlgorithm::Analysis analysis =
            ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                std::make_shared<SingleMaximumProbabilityOfCollisionAlgorithm>(100.0),
                buildCloseApproach(),
                hardBody1SPtr_,
                hardBody2SPtr_
            );

        EXPECT_EQ(analysis.region, ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust);
        EXPECT_DOUBLE_EQ(analysis.probabilityOfCollision, 0.05 * 10.0 / 100.0);
        EXPECT_NEAR(
            analysis.maximumProbabilityOfCollision.value(),
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision,
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision * 0.01
        );

        // Maximum at a sigma of 100 m: sigma scaling factor of 10.0 (i.e. covariance scaling factor of 10.0 squared)
        EXPECT_NEAR(analysis.maximumProbabilityOfCollisionSigmaScalingFactor.value(), 10.0, 10.0 * 0.01);
        EXPECT_DOUBLE_EQ(
            analysis.maximumProbabilityOfCollisionScalingFactor.value(),
            analysis.maximumProbabilityOfCollisionSigmaScalingFactor.value() *
                analysis.maximumProbabilityOfCollisionSigmaScalingFactor.value()
        );
    }

    // Single maximum to the left of the unscaled covariance (sigma of 10 m, maximum at a sigma of 1 m), hence the close
    // approach is in the dilution region
    {
        const ProbabilityOfCollisionAlgorithm::Analysis analysis =
            ProbabilityOfCollisionAlgorithm::ComputeProbabilityAnalysis(
                std::make_shared<SingleMaximumProbabilityOfCollisionAlgorithm>(1.0),
                buildCloseApproach(),
                hardBody1SPtr_,
                hardBody2SPtr_
            );

        EXPECT_EQ(analysis.region, ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted);
        EXPECT_DOUBLE_EQ(
            analysis.probabilityOfCollision,
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision *
                (SingleMaximumProbabilityOfCollisionAlgorithm::ZeroProbabilityOfCollisionSigma - 10.0) /
                (SingleMaximumProbabilityOfCollisionAlgorithm::ZeroProbabilityOfCollisionSigma - 1.0)
        );
        EXPECT_NEAR(
            analysis.maximumProbabilityOfCollision.value(),
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision,
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision * 0.01
        );

        // Maximum at a sigma of 1 m: sigma scaling factor of 0.1 (i.e. covariance scaling factor of 0.1 squared)
        EXPECT_NEAR(analysis.maximumProbabilityOfCollisionSigmaScalingFactor.value(), 0.1, 0.1 * 0.01);
        EXPECT_DOUBLE_EQ(
            analysis.maximumProbabilityOfCollisionScalingFactor.value(),
            analysis.maximumProbabilityOfCollisionSigmaScalingFactor.value() *
                analysis.maximumProbabilityOfCollisionSigmaScalingFactor.value()
        );
    }
}
