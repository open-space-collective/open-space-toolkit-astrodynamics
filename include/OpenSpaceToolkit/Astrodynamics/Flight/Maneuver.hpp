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
#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Mass.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameDirection.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::unit::Angle;
using ostk::physics::unit::Mass;

using ostk::astrodynamics::dynamics::Tabulated;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameDirection;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

#define DEFAULT_MANEUVER_INTERPOLATION_TYPE Interpolator::Type::BarycentricRational

/// @brief Store an acceleration and mass flow rate profile of a spacecraft maneuver.
class Maneuver
{
   public:
    static const Shared<const Frame> DefaultAccelFrameSPtr;
    static const Shared<const CoordinateSubset> DefaultAccelerationCoordinateSubsetSPtr;
    static const Duration MinimumRecommendedDuration;
    static const Duration MaximumRecommendedInterpolationInterval;
    static const Array<Shared<const CoordinateSubset>> RequiredCoordinateSubsets;

    typedef Pair<LocalOrbitalFrameDirection, Angle> MeanDirectionAndMaximumAngularOffset;

    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///                  Maneuver maneuver = Maneuver(...);
    /// @endcode
    ///
    /// @param aStateArray An array of states, must be sorted, must contain Position, Velocity, Acceleration and Mass
    /// Flow Rate coordinate subsets.
    Maneuver(const Array<State>& aStateArray);

    /// @brief Equal to operator
    ///
    /// @param aManeuver A maneuver
    /// @return True if maneuvers are equal
    bool operator==(const Maneuver& aManeuver) const;

    /// @brief Not equal to operator
    ///
    /// @param aManeuver A maneuver
    /// @return True if maneuvers are not equal
    bool operator!=(const Maneuver& aManeuver) const;

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///                  std::cout << Maneuver(...);
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

    /// @brief Get the states of the maneuver
    ///
    /// @code{.cpp}
    ///                  Array<State> states = maneuver.getStates();
    /// @endcode
    ///
    /// @return The states
    Array<State> getStates() const;

    /// @brief Get the interval of the maneuver
    ///
    /// @code{.cpp}
    ///                  Interval interval = maneuver.getInterval();
    /// @endcode
    ///
    /// @return The interval
    Interval getInterval() const;

    /// @brief Calculate the deltaV magnitude imparted during the maneuver
    ///
    /// @code{.cpp}
    ///                  Real deltaV = maneuver.calculateDeltaV();
    /// @endcode
    ///
    /// @return The deltaV (m/s)
    Real calculateDeltaV() const;

    /// @brief Calculate the delta Mass lost during the maneuver
    ///
    /// @code{.cpp}
    ///                  Mass deltaMass = maneuver.calculateDeltaMass();
    /// @endcode
    ///
    /// @return The delta Mass (kg)
    Mass calculateDeltaMass() const;

    /// @brief Calculate the average thrust imparted during the maneuver
    ///
    /// @code{.cpp}
    ///                  Real averageThrust = maneuver.calculateAverageThrust(Mass(100.0, Mass::Unit::Kilogram));
    /// @endcode
    ///
    /// @param anInitialSpacecraftMass The initial mass of the spacecraft
    ///
    /// @return The average thrust (N)
    Real calculateAverageThrust(const Mass& anInitialSpacecraftMass) const;

    /// @brief Calculate the average specific impulse produced during the maneuver
    ///
    /// @code{.cpp}
    ///                  Real averageSpecificImpulse = maneuver.calculateAverageSpecificImpulse(Mass(100.0,
    ///                  Mass::Unit::Kilogram));
    /// @endcode
    ///
    /// @param anInitialSpacecraftMass The initial mass of the spacecraft
    ///
    /// @return The average specific impulse (s)
    Real calculateAverageSpecificImpulse(const Mass& anInitialSpacecraftMass) const;

    /// @brief Convert the maneuver to a Tabulated Dynamics object
    ///
    /// @code{.cpp}
    ///                  Shared<Tabulated> tabulatedDynamicsSPtr = maneuver.toTabulatedDynamics(Frame::GCRF(),
    ///                  Interpolator::Type::BarycentricRational);
    /// @endcode
    ///
    /// @param (optional) aFrameSPtr The frame in which the acceleration profile is to be defined
    /// @param (optional) anInterpolationType The interpolation type to use in the Tabulated Dynamics object
    ///
    /// @return A shared pointer to the Tabulated Dynamics object
    Shared<Tabulated> toTabulatedDynamics(
        const Shared<const Frame>& aFrameSPtr = DefaultAccelFrameSPtr,
        const Interpolator::Type& anInterpolationType = DEFAULT_MANEUVER_INTERPOLATION_TYPE
    ) const;

    /// @brief Calculate the mean thrust direction in the Local Orbital Frame and its maximum angular offset w.r.t. the
    /// maneuver's thrust acceleration directions.
    ///
    /// @param aLocalOrbitalFrameFactorySPtr A local orbital frame factory
    ///
    /// @return The mean thrust direction and its maximum angular offset
    Maneuver::MeanDirectionAndMaximumAngularOffset calculateMeanThrustDirectionAndMaximumAngularOffset(
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
    ) const;

    /// @brief Create a new maneuver with a constant thrust acceleration direction in the Local Orbital
    /// Frame.
    ///
    /// The new Maneuver contains the same states as the original Maneuver, but the thrust acceleration direction is
    /// constant in the Local Orbital Frame. Said direction is the mean direction of the thrust acceleration directions
    /// in the Local Orbital Frame of the original Maneuver. The thrust acceleration magnitude profile is the same as
    /// the original.
    ///
    /// If defined, a runtime error will be thrown if the maximum allowed angular offset between the original thrust
    /// acceleration direction and the mean thrust direction is violated.
    ///
    /// @code{.cpp}
    ///                  Maneuver newManeuver =
    ///                  maneuver.toConstantLocalOrbitalFrameDirectionManeuver(LocalOrbitalFrameFactory::TNW(Frame::GCRF()));
    /// @endcode
    ///
    /// @param aLocalOrbitalFrameFactorySPtr A local orbital frame factory
    /// @param aMaximumAllowedAngularOffset A maximum allowed angular offset to consider (if any). Defaults to
    /// Undefined.
    ///
    /// @return A new maneuver
    Maneuver toConstantLocalOrbitalFrameDirectionManeuver(
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr,
        const Angle& aMaximumAllowedAngularOffset = Angle::Undefined()
    ) const;

    /// @brief Print Maneuver
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Create a maneuver from a constant mass flow rate profile
    ///
    /// @code{.cpp}
    ///                  Maneuver maneuver = Maneuver::ConstantMassFlowRateProfile(...);
    /// @endcode
    ///
    /// @param aStateArray An array of states, must be sorted, must contain Position, Velocity and Acceleration
    /// coordinate subsets.
    /// @param aMassFlowRate A constant mass flow rate that will be used for all the instants in the maneuver in kg/s
    ///
    /// @return A maneuver
    static Maneuver ConstantMassFlowRateProfile(const Array<State>& aStatearray, const Real& aMassFlowRate);

   private:
    Array<State> states_;
};

}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
