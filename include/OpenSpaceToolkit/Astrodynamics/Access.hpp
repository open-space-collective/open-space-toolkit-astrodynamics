/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Access__
#define __OpenSpaceToolkit_Astrodynamics_Access__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Physics/Time/Duration.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived/Angle.hpp>

namespace ostk
{
namespace astrodynamics
{

using ostk::core::container::Array;
using ostk::core::type::String;

using ostk::physics::time::Duration;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::physics::unit::Angle;

/// @brief Object-to-object visibility.
///
/// @details Represents a visibility window between two objects, defined by an Acquisition of Signal (AOS),
/// a Time of Closest Approach (TCA), and a Loss of Signal (LOS). Also stores the maximum elevation
/// angle reached during the access and whether the access window is complete or partial (i.e., truncated
/// by the analysis interval boundaries).
class Access
{
   public:
    /// @brief Access type.
    enum class Type
    {

        Undefined,  ///< Undefined type.
        Complete,   ///< The access window is fully contained within the analysis interval.
        Partial     ///< The access window is truncated by the analysis interval boundaries.

    };

    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Access access = {
    ///         Access::Type::Complete,
    ///         acquisitionOfSignal,
    ///         timeOfClosestApproach,
    ///         lossOfSignal,
    ///         maxElevation
    ///     } ;
    /// @endcode
    ///
    /// @param aType An access type.
    /// @param anAcquisitionOfSignal An acquisition of signal instant.
    /// @param aTimeOfClosestApproach A time of closest approach instant.
    /// @param aLossOfSignal A loss of signal instant.
    /// @param aMaxElevation A maximum elevation angle.
    Access(
        const Access::Type& aType,
        const Instant& anAcquisitionOfSignal,
        const Instant& aTimeOfClosestApproach,
        const Instant& aLossOfSignal,
        const Angle& aMaxElevation
    );

    /// @brief Equality operator.
    ///
    /// @code{.cpp}
    ///     Access access1 = { ... } ;
    ///     Access access2 = { ... } ;
    ///     bool equal = access1 == access2 ;
    /// @endcode
    ///
    /// @param anAccess An access.
    /// @return True if accesses are equal.
    bool operator==(const Access& anAccess) const;

    /// @brief Inequality operator.
    ///
    /// @code{.cpp}
    ///     Access access1 = { ... } ;
    ///     Access access2 = { ... } ;
    ///     bool notEqual = access1 != access2 ;
    /// @endcode
    ///
    /// @param anAccess An access.
    /// @return True if accesses are not equal.
    bool operator!=(const Access& anAccess) const;

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     std::cout << access ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param anAccess An access.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Access& anAccess);

    /// @brief Check if the access is defined.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     bool defined = access.isDefined() ;
    /// @endcode
    ///
    /// @return True if the access is defined.
    bool isDefined() const;

    /// @brief Check if the access is complete.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     bool complete = access.isComplete() ;
    /// @endcode
    ///
    /// @return True if the access type is Complete.
    bool isComplete() const;

    /// @brief Get the access type.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     Access::Type type = access.getType() ;
    /// @endcode
    ///
    /// @return The access type.
    Access::Type getType() const;

    /// @brief Get the acquisition of signal instant.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     Instant aos = access.getAcquisitionOfSignal() ;
    /// @endcode
    ///
    /// @return The acquisition of signal instant.
    Instant getAcquisitionOfSignal() const;

    /// @brief Get the time of closest approach instant.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     Instant tca = access.getTimeOfClosestApproach() ;
    /// @endcode
    ///
    /// @return The time of closest approach instant.
    Instant getTimeOfClosestApproach() const;

    /// @brief Get the loss of signal instant.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     Instant los = access.getLossOfSignal() ;
    /// @endcode
    ///
    /// @return The loss of signal instant.
    Instant getLossOfSignal() const;

    /// @brief Get the time interval of the access window.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     Interval interval = access.getInterval() ;
    /// @endcode
    ///
    /// @return The interval from acquisition of signal to loss of signal.
    Interval getInterval() const;

    /// @brief Get the duration of the access window.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     Duration duration = access.getDuration() ;
    /// @endcode
    ///
    /// @return The duration from acquisition of signal to loss of signal.
    Duration getDuration() const;

    /// @brief Get the maximum elevation angle reached during the access window.
    ///
    /// @code{.cpp}
    ///     Access access = { ... } ;
    ///     Angle maxElevation = access.getMaxElevation() ;
    /// @endcode
    ///
    /// @return The maximum elevation angle.
    Angle getMaxElevation() const;

    /// @brief Construct an undefined access.
    ///
    /// @code{.cpp}
    ///     Access access = Access::Undefined() ;
    /// @endcode
    ///
    /// @return An undefined access.
    static Access Undefined();

    /// @brief Get a string representation of an access type.
    ///
    /// @code{.cpp}
    ///     String typeString = Access::StringFromType(Access::Type::Complete) ;
    /// @endcode
    ///
    /// @param aType An access type.
    /// @return A string representation of the access type.
    static String StringFromType(const Access::Type& aType);

   private:
    Access::Type type_;

    Instant acquisitionOfSignal_;
    Instant timeOfClosestApproach_;
    Instant lossOfSignal_;
    Angle maxElevation_;
};

}  // namespace astrodynamics
}  // namespace ostk

#endif
