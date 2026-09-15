/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter__
#define __OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter__

#include <ostream>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{

using ostk::core::container::Array;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::object::VectorXi;

using ostk::astrodynamics::trajectory::State;

/// @brief Abstract base class for observation filters.
///
/// @details An observation filter classifies each observation of an arc as an inlier or an outlier, so that
/// outliers can be removed before orbit determination. Derive from this class and implement @ref filter to
/// provide a custom strategy; the returned @ref Analysis may be a subclass carrying filter-specific metrics.
///
/// @code{.cpp}
///     const observationfilter::MeanElementConsistency filter = {};
///     const Array<State> inliers = filter.apply(observations);
///     const OrbitDeterminationSolver::Analysis analysis = solver.estimate(initialGuess, inliers);
/// @endcode
class ObservationFilter
{
   public:
    /// @brief Classification produced by an observation filter.
    class Analysis
    {
       public:
        /// @brief Constructor
        ///
        /// @param anInlierMask Mask aligned with the filtered observation array (1 = inlier, 0 = outlier).
        Analysis(const VectorXi& anInlierMask);

        /// @brief Virtual destructor
        virtual ~Analysis() = default;

        /// @brief Output stream operator
        ///
        /// @param anOutputStream An output stream
        /// @param anAnalysis An analysis
        /// @return A reference to output stream
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis
        ///
        /// @param anOutputStream An output stream
        /// @param displayDecorator If true, display decorator
        virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        /// @brief Get the number of observations classified as inliers
        ///
        /// @return Inlier count
        Size getInlierCount() const;

        /// @brief Get the number of observations classified as outliers
        ///
        /// @return Outlier count
        Size getOutlierCount() const;

        /// @brief Apply the inlier mask to the observation array the analysis was computed from
        ///
        /// @param anObservationStateArray The original observation array (same size as the mask)
        /// @return The inlier observations, in their original order
        Array<State> getFilteredObservations(const Array<State>& anObservationStateArray) const;

        VectorXi inlierMask;  ///< Mask aligned with the observation array (1 = inlier, 0 = outlier).
    };

    /// @brief Virtual destructor
    virtual ~ObservationFilter() = default;

    /// @brief Classify an array of observations
    ///
    /// @param anObservationStateArray Observations to classify
    /// @return Analysis holding the inlier mask (dynamic type may be a filter-specific subclass)
    virtual Shared<const Analysis> filter(const Array<State>& anObservationStateArray) const = 0;

    /// @brief Classify an array of observations and return the inliers
    ///
    /// @param anObservationStateArray Observations to filter
    /// @return The inlier observations, in their original order
    Array<State> apply(const Array<State>& anObservationStateArray) const;
};

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
