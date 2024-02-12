/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Maneuver__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Maneuver__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Data/Scalar.hpp>
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Length.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Time.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Flight/System/PropulsionSystem.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;
using ostk::core::type::Real;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::curvefitting::Interpolator;

using ostk::physics::coordinate::Frame;
using ostk::physics::data::Scalar;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::Unit;
using ostk::physics::unit::Derived;
using ostk::physics::unit::Length;
using ostk::physics::unit::Mass;
using ostk::physics::unit::Time;

using TabulatedDynamics = ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::flight::system::PropulsionSystem;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

#define DEFAULT_MANEUVER_INTERPOLATION_TYPE Interpolator::Type::BarycentricRational

/// @brief Stores information and metadata about a maneuver as performed by a spacecraft.
class Maneuver
{
   public:
    static const Shared<const Frame> DefaultAccelFrameSPtr;

    /// @brief Constructor
    ///
    /// @param anInstantArray An array of instants, must be sorted
    /// @param anAccelerationProfile An acceleration profile of the maneuver, one Vector3d per instant
    /// @param aFrameSPtr A frame in which the acceleration profile is defined
    /// @param aMassFlowRateProfile A mass flow rate profile of the maneuver, one Real per instant
    Maneuver(
        const Array<Instant>& anInstantArray,
        const Array<Vector3d>& anAccelerationProfile,
        const Shared<const Frame>& aFrameSPtr,
        const Array<Real>& aMassFlowRateProfile
    );

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << Maneuver(...) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream
    /// @param aManeuver A Maneuver
    ///
    /// @return A reference to output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Maneuver& aManeuver);

    /// @brief Check if the maneuver is defined
    ///
    /// @return True if maneuver is defined (always returns true)
    bool isDefined() const;

    /// @brief Get the instants of the maneuver
    /// @return The instants
    Array<Instant> getInstants() const;

    /// @brief Get the acceleration profile of the maneuver
    ///
    /// @param aFrameSPtr A frame in which the acceleration profile is to be defined
    ///
    /// @return The acceleration profile
    Array<Vector3d> getAccelerationProfile(const Shared<const Frame>& aFrameSPtr = DefaultAccelFrameSPtr) const;

    /// @brief Get the mass flow rate profile of the maneuver
    /// @return The mass flow rate profile
    Array<Real> getMassFlowRateProfile() const;

    /// @brief Get the interval of the maneuver
    /// @return The interval
    Interval getInterval() const;

    /// @brief Calculate the deltaV (scalar metric) imparted during the maneuver
    /// @return The deltaV
    Scalar calculateDeltaV() const;

    /// @brief Calculate the delta Mass lost during the maneuver
    /// @return The delta Mass
    Mass calculateDeltaMass() const;

    /// @brief Calculate the average thrust imparted during the maneuver
    ///
    /// @param anInitialSpacecraftMass The initial mass of the spacecraft
    ///
    /// @return The average thrust
    Scalar calculateAverageThrust(const Mass& anInitialSpacecraftMass) const;

    /// @brief Calculate the average specific impulse produced during the maneuver
    ///
    /// @param anInitialSpacecraftMass The initial mass of the spacecraft
    ///
    /// @return The average specific impulse
    Scalar calculateAverageSpecificImpulse(const Mass& anInitialSpacecraftMass) const;

    /// @brief Calculate the (interpolated) acceleration at a given instant during the maneuver
    ///
    /// @param anInstant The instant at which to calculate the acceleration
    /// @param aFrameSPtr The frame in which the acceleration is to be defined
    /// @param anInterpolationType The interpolation type to use
    ///
    /// @return The acceleration
    Vector3d calculateAccelerationAt(
        const Instant& anInstant,
        const Shared<const Frame>& aFrameSPtr,
        const Interpolator::Type& anInterpolationType = DEFAULT_MANEUVER_INTERPOLATION_TYPE
    ) const;

    /// @brief Calculate the (interpolated) accelerations at given instants during the maneuver
    ///
    /// @param anInstantArray The instants at which to calculate the accelerations
    /// @param aFrameSPtr The frame in which the accelerations are to be defined
    /// @param anInterpolationType The interpolation type to use
    ///
    /// @return The accelerations
    Array<Vector3d> calculateAccelerationsAt(
        const Array<Instant>& anInstantArray,
        const Shared<const Frame>& aFrameSPtr,
        const Interpolator::Type& anInterpolationType = DEFAULT_MANEUVER_INTERPOLATION_TYPE
    ) const;

    /// @brief Calculate the (interpolated) mass flow rate at a given instant during the maneuver
    ///
    /// @param anInstant The instant at which to calculate the mass flow rate
    /// @param anInterpolationType The interpolation type to use
    ///
    /// @return The mass flow rate
    Real calculateMassFlowRateAt(
        const Instant& anInstant, const Interpolator::Type& anInterpolationType = DEFAULT_MANEUVER_INTERPOLATION_TYPE
    ) const;

    /// @brief Calculate the (interpolated) mass flow rates at given instants during the maneuver
    ///
    /// @param anInstantArray The instants at which to calculate the mass flow rates
    /// @param anInterpolationType The interpolation type to use
    ///
    /// @return The mass flow rates
    Array<Real> calculateMassFlowRatesAt(
        const Array<Instant>& anInstantArray,
        const Interpolator::Type& anInterpolationType = DEFAULT_MANEUVER_INTERPOLATION_TYPE
    ) const;

    /// @brief Convert the maneuver to a Tabulated Dynamics object
    ///
    /// @param aFrameSPtr The frame in which the acceleration profile is to be defined
    /// @param anInterpolationType The interpolation type to use in the Tabulated Dynamics object
    ///
    /// @return A shared pointer to the Tabulated Dynamics object
    Shared<TabulatedDynamics> toTabulatedDynamics(
        const Shared<const Frame>& aFrameSPtr = DefaultAccelFrameSPtr,
        const Interpolator::Type& anInterpolationType = DEFAULT_MANEUVER_INTERPOLATION_TYPE
    ) const;

    /// @brief Print Maneuver
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Create a maneuver from a constant mass flow rate profile
    ///
    /// @param anInstantArray An array of instants, must be sorted
    /// @param anAccelerationProfile An acceleration profile of the maneuver, one Vector3d per instant
    /// @param aFrameSPtr A frame in which the acceleration profile is defined
    /// @param aMassFlowRate A constant mass flow rate that will be used for all the instants in the maneuver
    static Maneuver FromConstantMassFlowRateProfile(
        const Array<Instant>& anInstantArray,
        const Array<Vector3d>& anAccelerationProfile,
        const Shared<const Frame>& aFrameSPtr,
        const Real& aMassFlowRate
    );

   private:
    Array<Instant> instants_;
    Array<Vector3d> accelerationProfileDefaultFrame_;
    Array<Real> massFlowRateProfile_;

    Array<Vector3d> convertAccelerationProfileFrame(const Shared<const Frame>& aFrameSPtr) const;

    Array<Vector3d> interpolateAccelerations(
        const Array<Instant>& anInstantArray,
        const Shared<const Frame>& aFrameSPtr,
        const Interpolator::Type& anInterpolationType
    ) const;

    Array<Real> interpolateMassFlowRates(
        const Array<Instant>& anInstantArray, const Interpolator::Type& anInterpolationType
    ) const;
};

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
