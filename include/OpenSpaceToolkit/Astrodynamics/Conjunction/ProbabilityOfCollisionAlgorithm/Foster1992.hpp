/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Foster1992__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Foster1992__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

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
using ostk::physics::unit::Angle;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

/// @brief Foster 1992 probability of collision algorithm as described in:
///
/// Foster, J.L., Estes, H.S. (1992). A Parametric Analysis of Orbital Debris Collision Probability and Maneuver Rate
/// for Space Vehicles. NASA/JSC-25898.
///
/// @details The two-dimensional probability density function of the relative position is integrated over the
/// combined hard-body circle in the encounter plane, in polar coordinates centered on the hard-body circle, as
/// described in (Equation 2):
///
/// Alfano, Salvatore. (2007). Review of Conjunction Probability Methods for Short-term Encounters. AAS 07-148.
///
/// The hard-body circle is sectioned into annular sectors (along its radius) and angular slices, and the probability
/// density function is evaluated at the center of each resulting cell (midpoint rule).
///
/// The algorithm relies on the following assumptions:
/// - High relative speed encounter
class Foster1992 : public ProbabilityOfCollisionAlgorithm
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///     Foster1992 algorithm = {} ;
    ///     Foster1992 algorithm = {
    ///         Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::GCRF(), Angle::Degrees(0.5), 12
    ///     } ;
    /// @endcode
    ///
    /// @note The default step sizes are those of Foster's numerical implementation, as reported by Alfano (2007): "the
    /// angle theta step size is 0.5 deg, and the radius r step size is OBJ/12" (OBJ being the combined hard-body
    /// radius). Alfano (2007) also notes that the accuracy degrades when the combined hard-body radius is smaller than
    /// the miss distance but larger than the smaller standard deviation, which smaller step sizes can remedy.
    ///
    /// @param aRelativeSpeedThreshold A relative speed threshold below which the
    /// close approach is considered low relative speed and the algorithm is not applicable.
    /// Defaults to 100 m/s
    /// @param aFrameSPtr An inertial (or quasi-inertial) frame to use. Defaults to Frame::GCRF()
    /// @param anAngularStepSize An angular step size of the polar integration, within (0, 360] deg. If it does not
    /// divide a full turn evenly, the number of angular slices is rounded up (and the step size reduced accordingly).
    /// Defaults to 0.5 deg
    /// @param aNumberOfAnnularSectors A number of annular sectors of the polar integration (i.e. the radial step size
    /// is the combined hard-body radius divided by this number), strictly positive. Defaults to 12
    Foster1992(
        const Derived& aRelativeSpeedThreshold = Derived(100.0, Derived::Unit::MeterPerSecond()),
        const Shared<const Frame>& aFrameSPtr = Frame::GCRF(),
        const Angle& anAngularStepSize = Angle::Degrees(0.5),
        const Size& aNumberOfAnnularSectors = 12
    );

    /// @brief Destructor
    virtual ~Foster1992() override = default;

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
    /// @brief Compute the probability of collision in the encounter plane (Alfano 2007, Equation 2)
    ///
    /// @details The encounter (U-W) plane coordinates are aligned with the principal axes of the combined covariance,
    /// centered on the combined covariance. The hard-body circle is centered at (R0 * sin(phi), R0 * cos(phi)).
    ///
    /// @param aR0 The distance from the origin to the center of the hard-body circle (i.e. the miss distance) [m]
    /// @param aPhi The angular position of the center of the hard-body circle, measured from the W axis
    /// @param aSigmaU The combined standard deviation along the U principal axis [m]
    /// @param aSigmaW The combined standard deviation along the W principal axis [m]
    /// @param anObj The combined hard-body radius (OBJ) [m]
    ///
    /// @return The probability of collision
    Real computeProbabilityOfCollision_(
        const Real& aR0, const Angle& aPhi, const Real& aSigmaU, const Real& aSigmaW, const Real& anObj
    ) const;

    static const Shared<const CoordinateSubset> CartesianPositionSubsetSPtr;

    Derived relativeSpeedThreshold_;
    Shared<const Frame> frameSPtr_;
    Size numberOfAnnularSectors_;
    Real angularStepSize_;               ///< Angular step size (dTheta), dividing a full turn evenly [rad]
    Array<double> angularSliceSines_;    ///< Sines of the angles at the center of the angular slices
    Array<double> angularSliceCosines_;  ///< Cosines of the angles at the center of the angular slices
};

}  // namespace probabilityofcollisionalgorithm
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
