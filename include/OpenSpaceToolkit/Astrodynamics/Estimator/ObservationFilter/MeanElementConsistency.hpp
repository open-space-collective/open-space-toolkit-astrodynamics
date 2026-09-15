/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency__
#define __OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency__

#include <ostream>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

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
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::VectorXi;

using ostk::physics::unit::Length;

using ostk::astrodynamics::estimator::ObservationFilter;
using ostk::astrodynamics::trajectory::State;

#define DEFAULT_MEAN_ELEMENT_GATE_FACTOR 10.0                           // Gate factor (robust sigmas)
#define DEFAULT_MEAN_ELEMENT_MINIMUM_DEVIATION Length::Kilometers(2.0)  // Minimum deviation flagged as outlier

/// @brief Observation filter based on the consistency of Brouwer-Lyddane mean orbital elements along the arc.
///
/// @details Good observations of a single spacecraft, once converted to mean orbital elements, trace slow
/// and smooth trends (a constant plus the J2 secular drift), while corrupted observations or coherent bad
/// segments (e.g. a wrong orbit plane) jump away from those trends. The filter needs no propagation, no
/// force model, no initial guess and no random numbers, and it runs in a fraction of a second for
/// thousands of observations.
///
/// Algorithm, after converting each observation to Brouwer-Lyddane Mean (Long) elements:
///  1. six channels expressed in metres: a, a·e·cos(Ω+ω), a·e·sin(Ω+ω), a·i, a·sin(i)·(Ω − Ω̇·t) and
///     a·(Ω+ω+M − λ̇·t), where the J2 secular rates Ω̇ and λ̇ come from the median elements and the two
///     angular channels are wrapped;
///  2. per channel, a robust linear trend is removed: Theil-Sen median slope over observation pairs separated
///     by at least a quarter of the arc span (so that dense high-rate bursts cannot dominate the slope), then
///     the residual is centred on its median;
///  3. an observation is an outlier when, in any channel, |residual| > max(gateFactor · σ, minimumDeviation),
///     where σ = 1.4826 · MAD is the robust scale of that channel's residuals.
///
/// Tuning. `gateFactor` (default 10): on the reference datasets any value in [3, 30] gives identical results.
/// `minimumDeviation` (default 2 km): any value in [1.5, 5] km gives identical results. The floor makes the
/// filter robust to arcs where most samples come from short high-rate bursts: the robust scale is then far
/// below the natural variation of mean elements between bursts (a few hundred metres to ~1.3 km for the
/// along-track channel) and only a physical floor prevents false rejections. The floor is also the smallest
/// error the filter can catch on short arcs (about minimumDeviation in a mean element, i.e. 1 to 1.5 times
/// that in position).
///
/// Limitations. Requires a majority (roughly 75 % or more) of good observations. Earth orbits only
/// (Brouwer-Lyddane). Observations must carry Cartesian position and velocity; they are converted to GCRF
/// internally. Errors smaller than the floor are not detected; leave those to the least squares residuals.
///
/// @code{.cpp}
///     const MeanElementConsistency filter = {};  // gateFactor = 10, minimumDeviation = 2 km
///     const Array<State> inliers = filter.apply(observations);
/// @endcode
class MeanElementConsistency : public ObservationFilter
{
   public:
    /// @brief Analysis results, extending the base analysis with per-channel deviations and gates.
    class Analysis : public ObservationFilter::Analysis
    {
       public:
        /// @brief Constructor
        ///
        /// @param anInlierMask Mask aligned with the observation array (1 = inlier, 0 = outlier)
        /// @param aDeviationMatrix [N x 6] detrended mean-element residuals in metres
        /// @param aGateVector [6] gate applied to each channel in metres
        Analysis(const VectorXi& anInlierMask, const MatrixXd& aDeviationMatrix, const VectorXd& aGateVector);

        /// @brief Print analysis
        ///
        /// @param anOutputStream An output stream
        /// @param displayDecorator If true, display decorator
        virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

        MatrixXd deviations;  ///< [N x 6] detrended residuals in metres, columns ordered as ChannelNames(). NaN when
                              ///< the observation could not be converted to mean elements.
        VectorXd gates;       ///< [6] gate applied to each channel in metres.
    };

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     MeanElementConsistency filter = {10.0, Length::Kilometers(2.0)};
    /// @endcode
    ///
    /// @param aGateFactor Number of robust sigmas beyond which a deviation is an outlier. Defaults to 10.
    /// @param aMinimumDeviation Deviation (in mean-element metres) below which an observation is never rejected.
    /// Defaults to 2 km.
    MeanElementConsistency(
        const Real& aGateFactor = DEFAULT_MEAN_ELEMENT_GATE_FACTOR,
        const Length& aMinimumDeviation = DEFAULT_MEAN_ELEMENT_MINIMUM_DEVIATION
    );

    /// @brief Get gate factor
    ///
    /// @return Gate factor
    Real getGateFactor() const;

    /// @brief Get minimum deviation
    ///
    /// @return Minimum deviation
    Length getMinimumDeviation() const;

    /// @brief Classify an array of observations
    ///
    /// @param anObservationStateArray Observations to classify (Cartesian position and velocity required)
    /// @return Analysis (dynamic type MeanElementConsistency::Analysis)
    virtual Shared<const ObservationFilter::Analysis> filter(const Array<State>& anObservationStateArray
    ) const override;

    /// @brief Names of the six channels, in the column order of Analysis::deviations
    ///
    /// @return Channel names
    static Array<String> ChannelNames();

   private:
    Real gateFactor_;
    Length minimumDeviation_;
};

}  // namespace observationfilter
}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
