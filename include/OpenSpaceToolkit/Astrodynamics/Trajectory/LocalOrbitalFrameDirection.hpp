/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_LocalOrbitalFrameDirection__

#include <OpenSpaceToolkit/Core/Types/Shared.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Shared;
using ostk::core::types::Integer;
using ostk::core::types::String;

using ostk::math::obj::Vector3d;

using ostk::physics::time::Instant;

using ostk::astro::trajectory::LocalOrbitalFrameFactory;

/// @brief                      Local Orbital Frame Direction
///
/// @note                       A unit vector, expressed with a Local Orbital Frame Factory.

class LocalOrbitalFrameDirection
{
   public:
    /// @brief                  Constructor
    ///
    /// @param                  [in] aValue A vector value
    /// @param                  [in] aLocalOrbitalFrameFactorySPtr A local orbital frame factory shared pointer
    ///
    /// @return                 Local orbital frame direction

    LocalOrbitalFrameDirection(
        const Vector3d& aValue, const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
    );

    /// @brief                  Equal to operator
    ///
    /// @param                  [in] aLocalOrbitalFrameDirection A local orbital frame direction
    ///
    /// @return                 True if local orbital frame directions are equal

    bool operator==(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const;

    /// @brief                  Not equal to operator
    ///
    /// @param                  [in] aLocalOrbitalFrameDirection A local orbital frame direction
    ///
    /// @return                 True if local orbital frame directions are not equal

    bool operator!=(const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection) const;

    /// @brief                  Output stream operator
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] aLocalOrbitalFrameDirection A local orbital frame direction
    /// @return                 A reference to output stream

    friend std::ostream& operator<<(std::ostream& anOutputStream, const LocalOrbitalFrameDirection& aLocalOrbitalFrameDirection);

    /// @brief                  Check if local orbital frame direction is defined
    ///
    /// @return                 True if local orbital frame direction is defined

    bool isDefined() const;

    /// @brief                  Print local orbital frame direction
    ///
    /// @param                  [in] anOutputStream An output stream
    /// @param                  [in] (optional) displayDecorators If true, display decorators

    void print(std::ostream& anOutputStream, bool displayDecorator = true) const;

    /// @brief                  Access local orbital frame factory
    ///
    /// @return                 The local orbital frame factory

    const Shared<const LocalOrbitalFrameFactory>& accessLocalOrbitalFrameFactory() const;

    /// @brief                  Get vector value
    ///
    /// @return                 The vector value

    Vector3d getValue() const;

    /// @brief                  Get local orbital frame factory
    ///
    /// @return                 The local orbital frame factory

    Shared<const LocalOrbitalFrameFactory> getLocalOrbitalFrameFactory() const;

    /// @brief                  Undefined local orbital frame direction
    ///
    /// @return                 The undefined local orbital frame direction

    static LocalOrbitalFrameDirection Undefined();

   private:
    Vector3d value_;
    Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactorySPtr_;
};

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk

#endif
