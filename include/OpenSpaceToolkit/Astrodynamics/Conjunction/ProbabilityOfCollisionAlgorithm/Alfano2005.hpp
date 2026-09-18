/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm.hpp>
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
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::physics::coordinate::Frame;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Alfano 2005 probability of collision algorithm as described in:
///
/// Alfano, Salvatore. (2005). A Numerical Implementation of Spherical Object Collision Probability. The Journal of the
/// Astronautical Sciences. 53. 103-109. 10.1007/BF03546397.
///
/// @details The two-dimensional probability density function of the relative position is integrated over the
/// combined hard-body circle in the encounter plane. The integral along the principal axis of the combined
/// covariance is computed with Simpson's rule, while the integral along the other principal axis is expressed with
/// error functions.
///
/// The algorithm relies on the following assumptions:
/// - High-relative velocity encounter
///
/// @ref https://doi.org/10.1007/BF03546397
class Alfano2005 : public ProbabilityOfCollisionAlgorithm
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     Alfano2005 algorithm = {} ;
    ///     Alfano2005 algorithm = { Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::GCRF() } ;
    /// @endcode
    ///
    /// @param aRelativeVelocityThreshold A relative velocity threshold below which the
    /// close approach is considered low-relative velocity and the algorithm is not applicable.
    /// Defaults to 100 m/s
    /// @param aFrameSPtr An inertial (or quasi-inertial) frame to use. Defaults to Frame::GCRF()
    Alfano2005(
        const Derived& aRelativeVelocityThreshold = Derived(100.0, Derived::Unit::MeterPerSecond()),
        const Shared<const Frame>& aFrameSPtr = Frame::GCRF()
    );

    /// @brief Destructor
    virtual ~Alfano2005() override = default;

    /// @brief Identify unsatisfied assumptions for a close approach
    ///
    /// @param aCloseApproach A close approach
    /// @return A non-exhaustive array of unsatisfied assumption descriptions
    virtual Array<String> identifyUnsatisfiedAssumptions(const CloseApproach& aCloseApproach) const override;

    /// @brief Compute the probability of collision
    ///
    /// @details The hard bodies are approximated by a combined hard-body radius, taken as the enclosing radius of
    /// their combined cross section in the encounter frame.
    ///
    /// @param aCloseApproach A close approach
    /// @param aHardBody1SPtr The hard body of Object 1
    /// @param aHardBody2SPtr The hard body of Object 2
    ///
    /// @return The probability of collision
    virtual Real computeProbabilityOfCollision(
        const CloseApproach& aCloseApproach,
        const Shared<const HardBody>& aHardBody1SPtr,
        const Shared<const HardBody>& aHardBody2SPtr
    ) const override;

   private:
    /// @brief Compute the probability of collision in the encounter plane
    ///
    /// @details The encounter plane coordinates are aligned with the principal axes of the combined covariance.
    ///
    /// @param aMissDistanceX The miss distance along the x principal axis [m]
    /// @param aMissDistanceY The miss distance along the y principal axis [m]
    /// @param aSigmaX The combined standard deviation along the x principal axis [m]
    /// @param aSigmaY The combined standard deviation along the y principal axis [m]
    /// @param aCombinedHardBodyRadius A combined hard-body radius [m]
    ///
    /// @return The probability of collision
    static Real ComputeProbabilityOfCollision(
        const Real& aMissDistanceX,
        const Real& aMissDistanceY,
        const Real& aSigmaX,
        const Real& aSigmaY,
        const Real& aCombinedHardBodyRadius
    );

    /// @brief Compute the number of Simpson's rule interval pairs (M)
    ///
    /// @details M = 5 * R / min(sigmaX, sigmaY, miss distance), bounded within
    /// [LowerIntervalPairCount, UpperIntervalPairCount].
    ///
    /// @param aMissDistanceX The miss distance along the x principal axis [m]
    /// @param aMissDistanceY The miss distance along the y principal axis [m]
    /// @param aSigmaX The combined standard deviation along the x principal axis [m]
    /// @param aSigmaY The combined standard deviation along the y principal axis [m]
    /// @param aCombinedHardBodyRadius A combined hard-body radius [m]
    ///
    /// @return The number of interval pairs
    static Size ComputeIntervalPairCount(
        const Real& aMissDistanceX,
        const Real& aMissDistanceY,
        const Real& aSigmaX,
        const Real& aSigmaY,
        const Real& aCombinedHardBodyRadius
    );

    /// @brief Compute the integrand of the Simpson's rule at a given abscissa
    ///
    /// @details The integrand combines the error function integral along the y principal axis over the circle chord at
    /// abscissa x, with the Gaussian density along the x principal axis evaluated at both x and -x (the chord being
    /// symmetric, the integral over [-R, R] is folded onto [-R, 0]).
    ///
    /// @param anAbscissa The abscissa x along the x principal axis [m]
    /// @param aMissDistanceX The miss distance along the x principal axis [m]
    /// @param aMissDistanceY The miss distance along the y principal axis [m]
    /// @param aSigmaX The combined standard deviation along the x principal axis [m]
    /// @param aSigmaY The combined standard deviation along the y principal axis [m]
    /// @param aCombinedHardBodyRadius A combined hard-body radius [m]
    ///
    /// @return The integrand value
    static Real ComputeIntegrand(
        const Real& anAbscissa,
        const Real& aMissDistanceX,
        const Real& aMissDistanceY,
        const Real& aSigmaX,
        const Real& aSigmaY,
        const Real& aCombinedHardBodyRadius
    );

    static const Shared<const CoordinateSubset> CartesianPositionSubsetSPtr;

    static const Size LowerIntervalPairCount;
    static const Size UpperIntervalPairCount;

    Derived relativeVelocityThreshold_;
    Shared<const Frame> frameSPtr_;
};

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
