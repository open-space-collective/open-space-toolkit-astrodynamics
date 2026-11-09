/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Spherical__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Spherical__

#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace hardbody
{

using ostk::core::type::Real;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::trajectory::State;

/// @brief Spherical hard body.
///
/// @details A sphere centered on the object position. Its cross section is a circle of the same radius,
/// whatever the state or frame.
class Spherical : public HardBody
{
   public:
    /// @brief Constructor
    ///
    /// @details Raises an Undefined error if the radius is undefined, and a RuntimeError if it is negative.
    ///
    /// @code{.cpp}
    ///              Spherical spherical = { 1.0 } ;
    /// @endcode
    ///
    /// @param aRadius The radius of the sphere [m]
    Spherical(const Real& aRadius);

    /// @brief Destructor
    virtual ~Spherical() override = default;

    /// @brief Get the radius of the sphere
    ///
    /// @return The radius of the sphere [m]
    Real getRadius() const;

    /// @brief Calculate the cross section of the sphere at a given state
    ///
    /// @details A circle of the sphere radius, i.e. a single vertex at the object position dilated by the radius.
    ///
    /// @param aState A state of the object
    /// @param aFrameSPtr A frame whose axes define the projection
    /// @return The cross section
    virtual HardBody::CrossSection calculateCrossSectionAt(const State& aState, const Shared<const Frame>& aFrameSPtr)
        const override;

   private:
    Real radius_;
};

}  // namespace hardbody
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
