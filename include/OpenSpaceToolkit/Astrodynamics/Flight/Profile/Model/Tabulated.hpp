/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Flight_Profile_Model_Tabulated__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace flight
{
namespace profile
{
namespace model
{

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::filesystem::File;
using ostk::core::type::Index;
using ostk::core::type::String;

using ostk::mathematics::curvefitting::Interpolator;

using ostk::physics::coordinate::Axes;
using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::flight::profile::Model;
using TabulatedTrajectory = ostk::astrodynamics::trajectory::model::Tabulated;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::StateBuilder;

/// @brief Tabulated profile model
///
/// Represents a tabulated profile model based on a set of states.
class Tabulated : public virtual Model
{
   public:
    /// @brief Constructor
    ///
    /// @param aStateArray An array of states
    /// @param anInterpolatorType  The type of interpolation used for all but the AttitudeQuaternion subset. Attitude
    /// quaternions will be interpolated using spherical linear interpolation (SLERP). Defaults to Barycentric Rational.
    Tabulated(
        const Array<State>& aStateArray,
        const Interpolator::Type& anInterpolatorType = Interpolator::Type::BarycentricRational
    );

    /// @brief Clone the tabulated model
    ///
    /// @return Pointer to the cloned tabulated model
    virtual Tabulated* clone() const override;

    /// @brief Equal to operator
    ///
    /// @param aTabulatedModel Another tabulated model
    /// @return True if models are equal
    bool operator==(const Tabulated& aTabulatedModel) const;

    /// @brief Not equal to operator
    ///
    /// @param aTabulatedModel Another tabulated model
    /// @return True if models are not equal
    bool operator!=(const Tabulated& aTabulatedModel) const;

    /// @brief Output stream operator
    ///
    /// @param anOutputStream An output stream
    /// @param aTabulatedModel A tabulated model
    /// @return A reference to the output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aTabulatedModel);

    /// @brief Check if the tabulated model is defined
    ///
    /// @return True if the model is defined
    virtual bool isDefined() const override;

    /// @brief Get the interval of the tabulated model
    ///
    /// @return Interval of the model
    Interval getInterval() const;

    /// @brief Get the interpolator type
    ///
    /// @return Interpolator type
    Interpolator::Type getInterpolatorType() const;

    /// @brief Calculate the state at a given instant
    ///
    /// @param anInstant An instant
    /// @return State at the given instant
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Get the axes at a given instant
    ///
    /// @param anInstant An instant
    /// @return Axes at the given instant
    virtual Axes getAxesAt(const Instant& anInstant) const override;

    /// @brief Get the body frame for a given frame name
    ///
    /// @param aFrameName Name of the frame
    /// @return Shared pointer to the frame
    virtual Shared<const Frame> constructBodyFrame(const String& aFrameName) const override;

    /// @brief Print the tabulated model to an output stream
    ///
    /// @param anOutputStream An output stream
    /// @param displayDecorator If true, display decorator
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Load a tabulated model from a file
    ///
    /// @param aFile A file
    /// @return Tabulated model
    static Tabulated Load(const File& aFile);

   protected:
    /// @brief Equal to operator for the base model
    ///
    /// @param aModel Another model
    /// @return True if models are equal
    virtual bool operator==(const Model& aModel) const override;

    /// @brief Not equal to operator for the base model
    ///
    /// @param aModel Another model
    /// @return True if models are not equal
    virtual bool operator!=(const Model& aModel) const override;

   private:
    Array<State> stateArray_;
    Interpolator::Type interpolatorType_;
    StateBuilder stateBuilder_;
    Array<Shared<const Interpolator>> interpolators_;

    StateBuilder reducedStateBuilder_;

    void setMembers(const Array<State>& aStateArray);
};

}  // namespace model
}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk

#endif
