/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity__
#define __OpenSpaceToolkit_Astrodynamics_Dynamics_ThirdBodyGravity__

#include <OpenSpaceToolkit/Core/Type/Integer.hpp>

#include <OpenSpaceToolkit/Physics/Environment/Object/Celestial.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::type::Integer;
using ostk::core::type::String;

using ostk::physics::environment::object::Celestial;
using ostk::physics::time::Instant;

using ostk::astrodynamics::Dynamics;

/// @brief Third-body gravitational dynamics.
///
/// @details Defines the gravitational acceleration experienced by a spacecraft due to a third body
/// (i.e., a celestial object other than the central body). The acceleration is computed using the
/// third-body perturbation formulation, which accounts for both the direct attraction of the third
/// body on the spacecraft and the indirect effect from the attraction on the central body.
class ThirdBodyGravity : public Dynamics
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Shared<const Celestial> celestialSPtr = { ... } ;
    ///     ThirdBodyGravity thirdBodyGravity = { celestialSPtr } ;
    /// @endcode
    ///
    /// @param aCelestial A celestial object representing the third body.
    ThirdBodyGravity(const Shared<const Celestial>& aCelestial);

    /// @brief Constructor with name.
    ///
    /// @code{.cpp}
    ///     Shared<const Celestial> celestialSPtr = { ... } ;
    ///     ThirdBodyGravity thirdBodyGravity = { celestialSPtr, "Moon Gravity" } ;
    /// @endcode
    ///
    /// @param aCelestial A celestial object representing the third body.
    /// @param aName A name for the dynamics.
    ThirdBodyGravity(const Shared<const Celestial>& aCelestial, const String& aName);

    /// @brief Destructor.
    virtual ~ThirdBodyGravity() override;

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     ThirdBodyGravity thirdBodyGravity = { ... } ;
    ///     std::cout << thirdBodyGravity ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param aThirdBodyGravity A third-body gravity dynamics.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const ThirdBodyGravity& aThirdBodyGravity);

    /// @brief Check if the third-body gravity dynamics is defined.
    ///
    /// @code{.cpp}
    ///     ThirdBodyGravity thirdBodyGravity = { ... } ;
    ///     bool defined = thirdBodyGravity.isDefined() ;
    /// @endcode
    ///
    /// @return True if the third-body gravity dynamics is defined.
    virtual bool isDefined() const override;

    /// @brief Get the celestial object.
    ///
    /// @code{.cpp}
    ///     ThirdBodyGravity thirdBodyGravity = { ... } ;
    ///     Shared<const Celestial> celestial = thirdBodyGravity.getCelestial() ;
    /// @endcode
    ///
    /// @return A shared pointer to the celestial object.
    Shared<const Celestial> getCelestial() const;

    /// @brief Get the coordinate subsets that the instance reads from.
    ///
    /// @code{.cpp}
    ///     ThirdBodyGravity thirdBodyGravity = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> readSubsets = thirdBodyGravity.getReadCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance reads from.
    virtual Array<Shared<const CoordinateSubset>> getReadCoordinateSubsets() const override;

    /// @brief Get the coordinate subsets that the instance writes to.
    ///
    /// @code{.cpp}
    ///     ThirdBodyGravity thirdBodyGravity = { ... } ;
    ///     Array<Shared<const CoordinateSubset>> writeSubsets = thirdBodyGravity.getWriteCoordinateSubsets() ;
    /// @endcode
    ///
    /// @return The coordinate subsets that the instance writes to.
    virtual Array<Shared<const CoordinateSubset>> getWriteCoordinateSubsets() const override;

    /// @brief Compute the contribution to the state derivative.
    ///
    /// @code{.cpp}
    ///     ThirdBodyGravity thirdBodyGravity = { ... } ;
    ///     VectorXd contribution = thirdBodyGravity.computeContribution(anInstant, x, aFrameSPtr) ;
    /// @endcode
    ///
    /// @param anInstant An instant.
    /// @param x The reduced state vector (follows the structure determined by the read coordinate subsets).
    /// @param aFrameSPtr The frame in which the state vector is expressed.
    /// @return The reduced derivative state vector (follows the structure determined by the write coordinate subsets)
    /// expressed in the given frame.
    virtual VectorXd computeContribution(
        const Instant& anInstant, const VectorXd& x, const Shared<const Frame>& aFrameSPtr
    ) const override;

    /// @brief Print the third-body gravity dynamics.
    ///
    /// @code{.cpp}
    ///     ThirdBodyGravity thirdBodyGravity = { ... } ;
    ///     thirdBodyGravity.print(std::cout) ;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display decorators.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   private:
    Shared<const Celestial> celestialObjectSPtr_;
};

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk

#endif
