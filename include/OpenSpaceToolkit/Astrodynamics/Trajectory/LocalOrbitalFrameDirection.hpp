/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection__

#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{

using ostk::core::type::Integer;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;

/// @brief Local Orbital Frame Direction.
///
/// @details A unit vector expressed in a local orbital frame, defined by a 3D direction vector and
/// a LocalOrbitalFrameFactory that determines how the frame is constructed from a given state.
class LocalOrbitalFrameDirection
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     LocalOrbitalFrameFactory factory = LocalOrbitalFrameFactory::TNW(Frame::GCRF()) ;
    ///     LocalOrbitalFrameDirection direction = { Vector3d::UnitX(), factory } ;
    /// @endcode
    ///
    /// @param aValue A vector value (will be normalized).
    /// @param aLocalOrbitalFrameFactory A local orbital frame factory.
    /// @return Local orbital frame direction.
    LocalOrbitalFrameDirection(const Vector3d& aValue, const LocalOrbitalFrameFactory& aLocalOrbitalFrameFactory);

    /// @brief Equal to operator
    ///
    /// @param aLocalOrbitalFrameDirection A local orbital frame direction
    ///
    /// @return True if local orbital frame directions are equal
    bool operator==(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const;

    /// @brief Not equal to operator
    ///
    /// @param aLocalOrbitalFrameDirection A local orbital frame direction
    ///
    /// @return True if local orbital frame directions are not equal
    bool operator!=(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aLocalOrbitalFrameDirection A local orbital frame direction
    /// @return A reference to output stream
    friend std::ostream& operator<<(
        std::ostream& anOutputStream, const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection
    );

    /// @brief Check if local orbital frame direction is defined.
    ///
    /// @code{.cpp}
    ///     LocalOrbitalFrameDirection direction = { ... } ;
    ///     bool defined = direction.isDefined() ;
    /// @endcode
    ///
    /// @return True if local orbital frame direction is defined.
    bool isDefined() const;

    /// @brief Print local orbital frame direction
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Access local orbital frame factory
    ///
    /// @return The local orbital frame factory
    const LocalOrbitalFrameFactory& accessLocalOrbitalFrameFactory() const;

    /// @brief Get the unit direction vector value.
    ///
    /// @code{.cpp}
    ///     LocalOrbitalFrameDirection direction = { ... } ;
    ///     Vector3d value = direction.getValue() ;
    /// @endcode
    ///
    /// @return The vector value.
    Vector3d getValue() const;

    /// @brief Get local orbital frame factory.
    ///
    /// @code{.cpp}
    ///     LocalOrbitalFrameDirection direction = { ... } ;
    ///     LocalOrbitalFrameFactory factory = direction.getLocalOrbitalFrameFactory() ;
    /// @endcode
    ///
    /// @return The local orbital frame factory.
    LocalOrbitalFrameFactory getLocalOrbitalFrameFactory() const;

    /// @brief Construct an undefined local orbital frame direction.
    ///
    /// @code{.cpp}
    ///     LocalOrbitalFrameDirection direction = LocalOrbitalFrameDirection::Undefined() ;
    /// @endcode
    ///
    /// @return An undefined local orbital frame direction.
    static LocalOrbitalFrameDirection Undefined();

   private:
    Vector3d value_;
    LocalOrbitalFrameFactory localOrbitalFrameFactory_;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
