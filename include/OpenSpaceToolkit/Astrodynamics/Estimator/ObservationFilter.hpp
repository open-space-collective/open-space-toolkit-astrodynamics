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
/// @details An observation filter inspects an array of observation states and produces an
/// @ref Analysis describing which observations are inliers, which are outliers, and any
/// filter-specific metadata gathered during filtering. Concrete filter strategies derive
/// from this class and override @ref filter, returning a (possibly derived) Analysis.
class ObservationFilter
{
   public:
    /// @brief Common analysis results returned by any observation filter.
    ///
    /// @details Concrete filters may subclass @ref Analysis to expose filter-specific
    /// metrics. The base class carries an Eigen-backed integer mask aligned with the
    /// original observation array (1 = inlier, 0 = outlier). The filtered (inlier-only)
    /// observation array can be reconstructed by applying the mask to the original
    /// observation array via @ref getFilteredObservations.
    class Analysis
    {
       public:
        /// @brief Constructor
        ///
        /// @param anInlierMask Integer mask aligned with the original observation array
        ///         (1 = inlier, 0 = outlier).
        Analysis(const VectorXi& anInlierMask);

        /// @brief Virtual destructor
        virtual ~Analysis() = default;

        /// @brief Stream insertion operator
        friend std::ostream& operator<<(std::ostream& anOutputStream, const Analysis& anAnalysis);

        /// @brief Print analysis to an output stream
        ///
        /// @param anOutputStream Stream to write to
        /// @param displayDecorator Whether to print a header/footer decorator
        virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

        /// @brief Number of observations classified as inliers
        Size getInlierCount() const;

        /// @brief Number of observations classified as outliers (filtered out)
        Size getOutlierCount() const;

        /// @brief Apply the inlier mask to an observation array to obtain the filtered subset.
        ///
        /// @param anObservationStateArray The original observation array the mask was computed against.
        ///         Its size must match @ref inlierMask.
        ///
        /// @return The inlier subset of @p anObservationStateArray, in original order.
        Array<State> getFilteredObservations(const Array<State>& anObservationStateArray) const;

        VectorXi
            inlierMask;  ///< Eigen integer mask aligned with the original observation array; 1 = inlier, 0 = outlier.
    };

    /// @brief Virtual destructor
    virtual ~ObservationFilter() = default;

    /// @brief Filter an array of observation states.
    ///
    /// @param anObservationStateArray The observations to filter
    ///
    /// @return Analysis describing the inlier/outlier classification and filter-specific
    /// metrics. The dynamic type is the concrete filter's @ref Analysis subclass.
    virtual Shared<const Analysis> filter(const Array<State>& anObservationStateArray) const = 0;
};

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
