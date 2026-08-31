/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_RANSAC__
#define __OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_RANSAC__

#include <ostream>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Propagator.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/NumericalSolver.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{
namespace observationfilter
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::VectorXi;

using ostk::physics::Environment;

using ostk::astrodynamics::estimator::ObservationFilter;
using ostk::astrodynamics::trajectory::Propagator;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::NumericalSolver;

#define DEFAULT_RANSAC_INLIER_THRESHOLD 1.0e3        // 1 km
#define DEFAULT_RANSAC_MAX_ITERATION_COUNT Size(50)  // 50 iterations
#define DEFAULT_RANSAC_MIN_INLIER_RATIO Real(0.5)    // 50% inliers triggers early termination
#define DEFAULT_RANSAC_RANDOM_SEED Size(0)           // 0 → seed from std::random_device

/// @brief RANSAC-based observation filter.
///
/// @details RANSAC (RANdom SAmple Consensus) sampling-based filter for trajectory observations.
/// Each iteration picks one observation at random, propagates it to every observation epoch using
/// the configured propagator, and classifies an observation as an inlier when its position
/// residual is below an inlier threshold. The iteration with the largest inlier set wins;
/// its inliers are returned as the filtered observation list.
///
/// This filter assumes each observation contains a full state (position + velocity) such that it
/// is a self-contained initial condition for the propagator.
class RANSAC : public ObservationFilter
{
   public:
    /// @brief RANSAC-specific analysis result, extending the common @ref ObservationFilter::Analysis.
    class Analysis : public ObservationFilter::Analysis
    {
       public:
        /// @brief Constructor
        ///
        /// @param anInlierMask Integer mask aligned with the original observation array (1 = inlier, 0 = outlier).
        /// @param anIterationCount Number of RANSAC iterations actually performed.
        /// @param isEarlyTermination True if the loop exited early after meeting the inlier ratio.
        /// @param aBestInlierCount Inlier count for the winning iteration.
        /// @param aSeedFailureCount Number of iterations where the seed propagation threw.
        Analysis(
            const VectorXi& anInlierMask,
            const Size& anIterationCount,
            const bool isEarlyTermination,
            const Size& aBestInlierCount,
            const Size& aSeedFailureCount
        );

        /// @brief Print RANSAC analysis
        void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

        Size iterationCount;    ///< Number of RANSAC iterations actually executed.
        bool earlyTermination;  ///< True if the loop terminated after reaching the inlier ratio target.
        Size bestInlierCount;   ///< Inlier count for the winning iteration.
        Size seedFailureCount;  ///< Number of iterations whose seed propagation threw.
    };

    /// @brief Constructor from a Propagator.
    ///
    /// @param aPropagator Propagator used to fit each sampled observation
    /// @param anInlierThreshold Position-residual threshold (meters) below which an observation
    ///         is considered an inlier
    /// @param aMaxIterationCount Maximum number of RANSAC iterations
    /// @param aMinInlierRatio If the best inlier set reaches this fraction of total observations,
    ///         iteration terminates early. Set to >= 1.0 to disable.
    /// @param aRandomSeed Seed for the RNG. Pass 0 to draw a seed from std::random_device.
    RANSAC(
        const Propagator& aPropagator,
        const Real& anInlierThreshold = DEFAULT_RANSAC_INLIER_THRESHOLD,
        const Size& aMaxIterationCount = DEFAULT_RANSAC_MAX_ITERATION_COUNT,
        const Real& aMinInlierRatio = DEFAULT_RANSAC_MIN_INLIER_RATIO,
        const Size& aRandomSeed = DEFAULT_RANSAC_RANDOM_SEED
    );

    /// @brief Convenience constructor that builds the Propagator from an Environment.
    ///
    /// @param anEnvironment Environment used to build the propagator
    /// @param aNumericalSolver Numerical solver used for propagation
    /// @param anInlierThreshold Position-residual threshold (meters)
    /// @param aMaxIterationCount Maximum number of RANSAC iterations
    /// @param aMinInlierRatio Inlier-fraction early-termination threshold
    /// @param aRandomSeed RNG seed (0 → random_device)
    RANSAC(
        const Environment& anEnvironment,
        const NumericalSolver& aNumericalSolver = NumericalSolver::Default(),
        const Real& anInlierThreshold = DEFAULT_RANSAC_INLIER_THRESHOLD,
        const Size& aMaxIterationCount = DEFAULT_RANSAC_MAX_ITERATION_COUNT,
        const Real& aMinInlierRatio = DEFAULT_RANSAC_MIN_INLIER_RATIO,
        const Size& aRandomSeed = DEFAULT_RANSAC_RANDOM_SEED
    );

    /// @brief Access the underlying Propagator
    const Propagator& accessPropagator() const;

    /// @brief Get the inlier threshold (meters)
    Real getInlierThreshold() const;

    /// @brief Get the maximum iteration count
    Size getMaxIterationCount() const;

    /// @brief Get the minimum inlier ratio for early termination
    Real getMinInlierRatio() const;

    /// @brief Get the RNG seed (0 indicates random_device-derived seed)
    Size getRandomSeed() const;

    /// @brief Filter observations.
    ///
    /// @param anObservationStateArray Observations to filter
    ///
    /// @return Shared pointer whose dynamic type is @ref RANSAC::Analysis.
    Shared<const ObservationFilter::Analysis> filter(const Array<State>& anObservationStateArray) const override;

   private:
    Propagator propagator_;
    Real inlierThreshold_;
    Size maxIterationCount_;
    Real minInlierRatio_;
    Size randomSeed_;
};

}  // namespace observationfilter
}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
