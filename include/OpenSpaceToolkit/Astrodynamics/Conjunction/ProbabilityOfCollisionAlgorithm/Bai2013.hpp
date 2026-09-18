/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Bai2013__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Environment/Gravitational/Earth.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace probabilityofcollisionalgorithm
{

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using EarthGravitationalModel = ostk::physics::environment::gravitational::Earth;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Bai 2013 probability of collision algorithm as described in:
///
/// Bai, Xian-Zong & Chen, Lei & Tang, Guo-Jin. (2013). Explicit expression of collision probability in terms of RSW and
/// NTW components of relative position. Advances in Space Research. 52. 1078-1096. 10.1016/j.asr.2013.05.034.
///
/// @details The algorithm derives an explicit and a modified expression for the probability of collision under the
/// following assumptions:
/// - High-relative velocity encounter
/// - Diagonal position covariance matrix in the RSW frame
/// - Near-circular orbits (for the explicit formulation)
///
/// @ref
/// https://www.researchgate.net/publication/273848101_Explicit_expression_of_collision_probability_in_terms_of_RSW_and_NTW_components_of_relative_position
/// @ref https://pdfs.semanticscholar.org/dc1c/01515100462d289df5518556dec3bebf63d0.pdf
class Bai2013 : public ProbabilityOfCollisionAlgorithm
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     Bai2013 algorithm = {} ;
    ///     Bai2013 algorithm = {
    ///         Derived(100.0, Derived::Unit::MeterPerSecond()),
    ///         0.001,
    ///         Frame::GCRF(),
    ///         EarthGravitationalModel::EGM96.gravitationalParameter_
    ///     } ;
    /// @endcode
    ///
    /// @param aRelativeVelocityThreshold A relative velocity threshold below which the
    /// close approach is considered low-relative velocity and the algorithm is not applicable.
    /// Defaults to 100 m/s
    /// @param anEccentricityThreshold An eccentricity threshold below which the explicit
    /// formulation for near-circular orbits is used. Defaults to 0.001
    /// @param aFrameSPtr An inertial (or quasi-inertial) frame to use. Defaults to Frame::GCRF()
    /// @param aGravitationalParameter A gravitational parameter to use when computing the
    /// eccentricities. Defaults to the Earth EGM96 gravitational parameter
    Bai2013(
        const Derived& aRelativeVelocityThreshold = Derived(100.0, Derived::Unit::MeterPerSecond()),
        const Real& anEccentricityThreshold = 0.001,
        const Shared<const Frame>& aFrameSPtr = Frame::GCRF(),
        const Derived& aGravitationalParameter = EarthGravitationalModel::EGM96.gravitationalParameter_
    );

    /// @brief Destructor
    virtual ~Bai2013() override = default;

    /// @brief Get the name of the probability of collision algorithm
    ///
    /// @return "Bai 2013"
    virtual String getName() const override;

    /// @brief Check if the algorithm computes a maximum probability of collision
    ///
    /// @return False
    virtual bool isMaximumProbabilityOfCollisionAlgorithm() const override;

    /// @brief Identify unsatisfied assumptions for a close approach
    ///
    /// @param aCloseApproach A close approach
    /// @return A non-exhaustive array of unsatisfied assumption descriptions
    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override;

    /// @brief Compute the probability of collision
    ///
    /// @param aCloseApproach A close approach
    /// @param aCombinedHardBodyRadius A combined hard-body radius
    ///
    /// @return The probability of collision
    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach, const Length& aCombinedHardBodyRadius
    ) const override;

   private:
    /// @brief Compute the eccentricity of a state
    ///
    /// @details The state is first expressed in `frameSPtr_`. Eccentricity is then computed
    /// from the resulting Cartesian state using `gravitationalParameter_`.
    ///
    /// @param aState A state
    /// @return The eccentricity
    Real computeEccentricity(const State& aState) const;

    static const Shared<const CoordinateSubset> CartesianPositionSubsetSPtr;

    Derived relativeVelocityThreshold_;
    Real eccentricityThreshold_;
    Shared<const Frame> frameSPtr_;
    Derived gravitationalParameter_;
};

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
