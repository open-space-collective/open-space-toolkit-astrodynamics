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
#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

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
using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

namespace
{

/// @brief Mock probability of collision algorithm that is always applicable (bogus probability of collision).
class AlwaysApplicableAlgorithm : public ProbabilityOfCollisionAlgorithm
{
   public:
    AlwaysApplicableAlgorithm()
        : ProbabilityOfCollisionAlgorithm("Always Applicable")
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
        : ProbabilityOfCollisionAlgorithm("Never Applicable")
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
        : ProbabilityOfCollisionAlgorithm("Constant"),
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
        : ProbabilityOfCollisionAlgorithm("Increasing")
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

        return std::log(std::sqrt(aCloseApproach.getObject1CovarianceMatrix()->accessCoordinates()(0, 0)));
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
        : ProbabilityOfCollisionAlgorithm("Decreasing")
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

        return 1.0 / std::sqrt(aCloseApproach.getObject1CovarianceMatrix()->accessCoordinates()(0, 0));
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
        : ProbabilityOfCollisionAlgorithm("Single Maximum"),
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

        const Real sigma = std::sqrt(aCloseApproach.getObject1CovarianceMatrix()->accessCoordinates()(0, 0));

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
    // Helper function to build a state with a position covariance matrix attached
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

        State state = stateBuilder.build(anInstant, coordinates);
        state.setCovarianceMatrix(CovarianceMatrix::FromPositionSigmas(anInstant, aPositionSigmas, aFrameSPtr));

        return state;
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
    const AlwaysApplicableAlgorithm algorithm = {};
    const CloseApproach closeApproach = buildCloseApproach();

    // No covariance to scale
    {
        EXPECT_THROW(
            try {
                algorithm.computeProbabilityAnalysis(closeApproach, hardBody1SPtr_, hardBody2SPtr_, false, false);
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("At least one covariance must be scalable to find the probability region.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Sigma scaling factor lower bound equal to 0
    {
        EXPECT_THROW(
            try {
                algorithm.computeProbabilityAnalysis(
                    closeApproach, hardBody1SPtr_, hardBody2SPtr_, true, true, 0.0, 100.0
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Sigma scaling factor lower bound must be greater than 0.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Sigma scaling factor lower bound equal to 1
    {
        EXPECT_THROW(
            try {
                algorithm.computeProbabilityAnalysis(
                    closeApproach, hardBody1SPtr_, hardBody2SPtr_, true, true, 1.0, 100.0
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Sigma scaling factor lower bound must be lower than 1.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Sigma scaling factor upper bound equal to 1
    {
        EXPECT_THROW(
            try {
                algorithm.computeProbabilityAnalysis(
                    closeApproach, hardBody1SPtr_, hardBody2SPtr_, true, true, 0.01, 1.0
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Sigma scaling factor upper bound must be greater than 1.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Maximum point per pass count lower than 3
    {
        EXPECT_THROW(
            try {
                algorithm.computeProbabilityAnalysis(
                    closeApproach, hardBody1SPtr_, hardBody2SPtr_, true, true, 0.01, 100.0, 2
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Maximum point per pass count must be greater than or equal to 3.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Maximum pass count lower than 2
    {
        EXPECT_THROW(
            try {
                algorithm.computeProbabilityAnalysis(
                    closeApproach, hardBody1SPtr_, hardBody2SPtr_, true, true, 0.01, 100.0, 50, 1
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Maximum pass count must be greater than or equal to 2.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }

    // Convergence threshold equal to 0
    {
        EXPECT_THROW(
            try {
                algorithm.computeProbabilityAnalysis(
                    closeApproach, hardBody1SPtr_, hardBody2SPtr_, true, true, 0.01, 100.0, 50, 10, 0.0
                );
            } catch (const ostk::core::error::RuntimeError& e) {
                EXPECT_EQ("Convergence threshold must be greater than 0.", e.getMessage());
                throw;
            },
            ostk::core::error::RuntimeError
        );
    }
}

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm, ComputeProbabilityAnalysis)
{
    // Constant probability of collision: the probability of collision does not change with the covariance scaling
    {
        const ConstantProbabilityOfCollisionAlgorithm algorithm = {0.01};

        const ProbabilityOfCollisionAlgorithm::Analysis analysis =
            algorithm.computeProbabilityAnalysis(buildCloseApproach(), hardBody1SPtr_, hardBody2SPtr_);

        EXPECT_EQ(analysis.region, ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Maximum);
        EXPECT_EQ(analysis.probabilityOfCollision, 0.01);
        EXPECT_EQ(analysis.maximumProbabilityOfCollision, 0.01);
    }

    // Increasing probability of collision: the maximum lies beyond the upper bound of the scaling interval, hence the
    // close approach is in the robust region
    {
        const IncreasingProbabilityOfCollisionAlgorithm algorithm = {};

        const ProbabilityOfCollisionAlgorithm::Analysis analysis =
            algorithm.computeProbabilityAnalysis(buildCloseApproach(), hardBody1SPtr_, hardBody2SPtr_);

        EXPECT_EQ(analysis.region, ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust);
        EXPECT_DOUBLE_EQ(analysis.probabilityOfCollision, std::log(10.0));

        // Maximum at the upper bound of the scaling interval: sigma of 10 m x 100 (default sigma scaling factor upper
        // bound)
        EXPECT_NEAR(analysis.maximumProbabilityOfCollision, std::log(10.0 * 100.0), 1e-9);
    }

    // Decreasing probability of collision: the maximum lies below the lower bound of the scaling interval, hence the
    // close approach is in the dilution region
    {
        const DecreasingProbabilityOfCollisionAlgorithm algorithm = {};

        const ProbabilityOfCollisionAlgorithm::Analysis analysis =
            algorithm.computeProbabilityAnalysis(buildCloseApproach(), hardBody1SPtr_, hardBody2SPtr_);

        EXPECT_EQ(analysis.region, ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted);
        EXPECT_DOUBLE_EQ(analysis.probabilityOfCollision, 1.0 / 10.0);

        // Maximum at the lower bound of the scaling interval: sigma of 10 m x 0.01 (default sigma scaling factor lower
        // bound)
        EXPECT_NEAR(analysis.maximumProbabilityOfCollision, 1.0 / (10.0 * 0.01), 1e-9);
    }

    // Single maximum to the right of the unscaled covariance (sigma of 10 m, maximum at a sigma of 100 m), hence the
    // close approach is in the robust region
    {
        const SingleMaximumProbabilityOfCollisionAlgorithm algorithm = {100.0};

        const ProbabilityOfCollisionAlgorithm::Analysis analysis =
            algorithm.computeProbabilityAnalysis(buildCloseApproach(), hardBody1SPtr_, hardBody2SPtr_);

        EXPECT_EQ(analysis.region, ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Robust);
        EXPECT_DOUBLE_EQ(analysis.probabilityOfCollision, 0.05 * 10.0 / 100.0);
        EXPECT_NEAR(
            analysis.maximumProbabilityOfCollision,
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision,
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision * 0.01
        );
    }

    // Single maximum to the left of the unscaled covariance (sigma of 10 m, maximum at a sigma of 1 m), hence the close
    // approach is in the dilution region
    {
        const SingleMaximumProbabilityOfCollisionAlgorithm algorithm = {1.0};

        const ProbabilityOfCollisionAlgorithm::Analysis analysis =
            algorithm.computeProbabilityAnalysis(buildCloseApproach(), hardBody1SPtr_, hardBody2SPtr_);

        EXPECT_EQ(analysis.region, ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted);
        EXPECT_DOUBLE_EQ(
            analysis.probabilityOfCollision,
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision *
                (SingleMaximumProbabilityOfCollisionAlgorithm::ZeroProbabilityOfCollisionSigma - 10.0) /
                (SingleMaximumProbabilityOfCollisionAlgorithm::ZeroProbabilityOfCollisionSigma - 1.0)
        );
        EXPECT_NEAR(
            analysis.maximumProbabilityOfCollision,
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision,
            SingleMaximumProbabilityOfCollisionAlgorithm::MaximumProbabilityOfCollision * 0.01
        );
    }
}
