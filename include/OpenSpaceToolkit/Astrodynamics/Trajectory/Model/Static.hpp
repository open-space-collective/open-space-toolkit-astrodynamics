/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Static__

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

using ostk::physics::coordinate::Position;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;

using ostk::astrodynamics::trajectory::Model;
using ostk::astrodynamics::trajectory::State;

/// @brief Static trajectory model to represent a fixed position on the surface of the earth
class Static : public virtual Model
{
   public:
    /// @brief Constructor
    ///
    /// @code{.cpp}
    ///              Position position = { ... } ;
    ///              Static staticModel(position) ;
    /// @endcode
    ///
    /// @param aPosition The position of the static model. Must be provided in the ITRF frame.
    Static(const Position& aPosition);

    /// @brief Clone the static model
    ///
    /// @code{.cpp}
    ///              Static staticModel = { ... } ;
    ///              Static* clonedModel = staticModel.clone() ;
    /// @endcode
    ///
    /// @return A pointer to the cloned static model
    virtual Static* clone() const override;

    /// @brief Equality operator
    ///
    /// @code{.cpp}
    ///              Static staticModel1 = { ... } ;
    ///              Static staticModel2 = { ... } ;
    ///              bool isEqual = (staticModel1 == staticModel2) ;
    /// @endcode
    ///
    /// @param aStaticModel The static model to compare with
    /// @return True if the models are equal, false otherwise
    bool operator==(const Static& aStaticModel) const;

    /// @brief Inequality operator
    ///
    /// @code{.cpp}
    ///              Static staticModel1 = { ... } ;
    ///              Static staticModel2 = { ... } ;
    ///              bool isNotEqual = (staticModel1 != staticModel2) ;
    /// @endcode
    ///
    /// @param aStaticModel The static model to compare with
    /// @return True if the models are not equal, false otherwise
    bool operator!=(const Static& aStaticModel) const;

    /// @brief Output stream operator
    ///
    /// @code{.cpp}
    ///              Static staticModel = { ... } ;
    ///              std::cout << staticModel ;
    /// @endcode
    ///
    /// @param anOutputStream The output stream
    /// @param aStaticModel The static model to output
    /// @return The output stream
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Static& aStaticModel);

    /// @brief Check if the static model is defined
    ///
    /// @code{.cpp}
    ///              Static staticModel = { ... } ;
    ///              bool isDefined = staticModel.isDefined() ;
    /// @endcode
    ///
    /// @return True if the model is defined, false otherwise
    virtual bool isDefined() const override;

    /// @brief Calculate the state at a given instant
    ///
    /// @code{.cpp}
    ///              Static staticModel = { ... } ;
    ///              Instant instant = { ... } ;
    ///              State state = staticModel.calculateStateAt(instant) ;
    /// @endcode
    ///
    /// @param anInstant The instant at which to calculate the state
    /// @return The state at the given instant
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Print the static model to an output stream
    ///
    /// @code{.cpp}
    ///              Static staticModel = { ... } ;
    ///              staticModel.print(std::cout, true) ;
    /// @endcode
    ///
    /// @param anOutputStream The output stream
    /// @param displayDecorator If true, display decorator
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

   protected:
    /// @brief Equality operator for Model base class
    ///
    /// @code{.cpp}
    ///              Static staticModel = { ... } ;
    ///              Model model = { ... } ;
    ///              bool isEqual = (staticModel == model) ;
    /// @endcode
    ///
    /// @param aModel The model to compare with
    /// @return True if the models are equal, false otherwise
    virtual bool operator==(const Model& aModel) const override;

    /// @brief Inequality operator for Model base class
    ///
    /// @code{.cpp}
    ///              Static staticModel = { ... } ;
    ///              Model model = { ... } ;
    ///              bool isNotEqual = (staticModel != model) ;
    /// @endcode
    ///
    /// @param aModel The model to compare with
    /// @return True if the models are not equal, false otherwise
    virtual bool operator!=(const Model& aModel) const override;

   private:
    Position position_;
};

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
