/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency__
#define __OpenSpaceToolkit_Astrodynamics_Estimator_ObservationFilter_MeanElementConsistency__

#include <ostream>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Matrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

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
using ostk::core::type::Size;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::VectorXi;

using ostk::physics::unit::Derived;

using ostk::astrodynamics::estimator::ObservationFilter;
using ostk::astrodynamics::trajectory::State;

using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

#define DEFAULT_MEAN_ELEMENT_CONSISTENCY_GATE_FACTOR Real(10.0)  // multiplier on the robust per-channel scale
#define DEFAULT_MEAN_ELEMENT_CONSISTENCY_NOISE_FLOOR Real(10.0)  // m — assumed observation noise floor

/// @brief Mean-orbital-element consistency observation filter.
///
/// @details Every genuine observation of one spacecraft lies on a single orbit. In mean orbital
/// element space that orbit is not a trajectory that must be propagated — it is a point drifting
/// slowly and smoothly: the semi-major axis, eccentricity vector, and inclination change at most
/// linearly over an arc (drag, luni-solar), the node precesses linearly (J2), and the along-track
/// phase advances at the known Kepler + J2 secular rate plus a slow drag quadratic. A bad
/// observation — a position/velocity glitch, a time-tag error, or a whole block on a mis-planed
/// orbit — cannot stay on that smooth curve in every element simultaneously, because the mapping
/// from state error to element error is a well-conditioned change of coordinates.
///
/// The filter therefore:
/// 1. converts each observation (position, velocity, time; any frame) to Brouwer-Lyddane mean
///    elements (closed form — this removes the J2 short- and long-period oscillations that would
///    otherwise mask small errors);
/// 2. forms six channels: semi-major axis, ex = e·cos(Ω+ω), ey = e·sin(Ω+ω), inclination,
///    node (with the J2 secular rate removed), and along-track phase
///    φ = wrap(Ω+ω+M − (Kepler + J2 secular rate)·t) — the secular rates come from the median
///    elements, in closed form;
/// 3. robustly fits each channel's slow trend: a Theil-Sen (median-of-pairwise-slopes) line,
///    refined by two least-squares refits that exclude only egregious outliers (beyond 3× the
///    gate); the phase channel uses a quadratic to absorb the drag decay;
/// 4. classifies: an observation is an outlier if any channel residual deviates from the channel
///    median by more than gateFactor × max(1.4826 × MAD, channel noise floor), where the channel
///    noise floors are the user's single position-noise floor (meters) mapped into each element.
///
/// No propagation, no environment or force model, no random sampling: the filter is O(N)
/// closed-form conversions plus medians, deterministic, and runs in milliseconds on
/// thousands of observations. There are exactly two tuning parameters, both physical: the gate
/// factor (how many robust sigmas is anomalous) and the observation noise floor (metres below
/// which deviations are never anomalous).
///
/// Detection limits follow from what the channels can see: deviations below the arc's unmodeled
/// smooth dynamics (a few hundred metres over multi-day LEO arcs, tens of metres over short arcs)
/// are absorbed by the trend fits and cannot be flagged. Near-equatorial orbits weaken the node
/// channel (the plane-offset signature then appears in inclination). Requires observations of a
/// single spacecraft on one orbit; a majority (> ~70%) of the observations must be good.
class MeanElementConsistency : public ObservationFilter
{
   public:
    /// @brief Number of element channels.
    static constexpr Size ChannelCount = 6;

    /// @brief Mean-element-consistency-specific analysis, extending the common
    /// @ref ObservationFilter::Analysis.
    class Analysis : public ObservationFilter::Analysis
    {
       public:
        /// @brief Constructor
        ///
        /// @param anInlierMask Integer mask aligned with the original observation array (1 = inlier, 0 = outlier).
        /// @param aChannelResidualMatrix (N × 6) per-observation channel residuals, in channel units:
        ///        [semi-major axis (m), ex (-), ey (-), inclination (rad), node (rad), phase (rad)].
        /// @param anAppliedChannelGateArray (6) gate actually applied to each channel, in channel units.
        Analysis(
            const VectorXi& anInlierMask,
            const MatrixXd& aChannelResidualMatrix,
            const VectorXd& anAppliedChannelGateArray
        );

        /// @brief Print analysis
        void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

        MatrixXd channelResiduals;     ///< (N × 6) channel residuals: [sma m, ex, ey, inclination rad, node rad,
                                       ///< phase rad].
        VectorXd appliedChannelGates;  ///< (6) gate applied per channel, same order and units as the residuals.
    };

    /// @brief Constructor
    ///
    /// @param aGateFactor Multiplier on the robust per-channel scale (max of 1.4826 × MAD and the
    ///        channel noise floor) beyond which an observation is classified as an outlier.
    /// @param aNoiseFloor Assumed observation position-noise floor (meters). Deviations below this
    ///        scale are never classified as outliers. It is mapped into each element channel
    ///        (e.g. δ/a for the angular channels, 2δ for the semi-major axis).
    /// @param aGravitationalParameter Gravitational parameter used for the mean element conversion
    ///        and the secular rates.
    MeanElementConsistency(
        const Real& aGateFactor = DEFAULT_MEAN_ELEMENT_CONSISTENCY_GATE_FACTOR,
        const Real& aNoiseFloor = DEFAULT_MEAN_ELEMENT_CONSISTENCY_NOISE_FLOOR,
        const Derived& aGravitationalParameter = EarthGravitationalModel::EGM2008.gravitationalParameter_
    );

    /// @brief Get the gate factor
    Real getGateFactor() const;

    /// @brief Get the observation noise floor (meters)
    Real getNoiseFloor() const;

    /// @brief Get the gravitational parameter
    Derived getGravitationalParameter() const;

    /// @brief Filter observations.
    ///
    /// @param anObservationStateArray Observations to filter. Each observation must contain
    ///        Cartesian position and velocity subsets (any frame; converted internally to GCRF).
    ///        The observations may be provided in any time order.
    ///
    /// @return Shared pointer whose dynamic type is @ref MeanElementConsistency::Analysis.
    Shared<const ObservationFilter::Analysis> filter(const Array<State>& anObservationStateArray) const override;

   private:
    Real gateFactor_;
    Real noiseFloor_;
    Derived gravitationalParameter_;
};

}  // namespace observationfilter
}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk

#endif
