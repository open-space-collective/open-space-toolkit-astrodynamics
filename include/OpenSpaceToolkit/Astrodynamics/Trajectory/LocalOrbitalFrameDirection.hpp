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

using ostk::core::type::Shared;
using ostk::core::type::Integer;
using ostk::core::type::String;

using ostk::mathematics::object::Vector3d;

using ostk::physics::time::Instant;

using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;

/// @brief Local Orbital Frame Direction
///
/// @note                       A unit vector, expressed with a Local Orbital Frame Factory.
class LocalOrbitalFrameDirection
{
   public:
    /// @brief Constructor
    ///
    /// @param aValue A vector value
    /// @param aLocalOrbitalFrameFactorySPtr A local orbital frame factory shared pointer
    ///
    /// @return Local orbital frame direction
    LocalOrbitalFrameDirection(
        const Vector3d& aValue, const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
    );

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

    /// @brief Check if local orbital frame direction is defined
    ///
    /// @return True if local orbital frame direction is defined
    bool isDefined() const;

    /// @brief Print local orbital frame direction
    ///
    /// @param anOutputStream An output stream
    /// @param (optional) displayDecorators If true, display decorators
    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief Access local orbital frame factory
    ///
    /// @return The local orbital frame factory
    const Shared<const LocalOrbitalFrameFactory>& accessLocalOrbitalFrameFactory() const;

    /// @brief Get vector value
    ///
    /// @return The vector value
    Vector3d getValue() const;

    /// @brief Get local orbital frame factory
    ///
    /// @return The local orbital frame factory
    Shared<const LocalOrbitalFrameFactory> getLocalOrbitalFrameFactory() const;

    /// @brief Undefined local orbital frame direction
    ///
    /// @return The undefined local orbital frame direction
    static LocalOrbitalFrameDirection Undefined();

   private:
    Vector3d value_;
    Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactorySPtr_;
};

}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
