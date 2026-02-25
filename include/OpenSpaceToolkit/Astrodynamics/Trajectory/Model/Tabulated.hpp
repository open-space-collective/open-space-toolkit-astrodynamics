/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Tabulated__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

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

using ostk::core::container::Array;
using ostk::core::container::Pair;
using ostk::core::filesystem::File;
using ostk::core::type::Index;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;

using ostk::astrodynamics::trajectory::Model;
using ostk::astrodynamics::trajectory::State;

#define DEFAULT_TABULATED_TRAJECTORY_INTERPOLATION_TYPE Interpolator::Type::Linear

/// @brief Tabulated trajectory model
///
///                      Interpolation is performed between states using the specified interpolation scheme.
///                      For now, linear, barycentric rational and cubic spline interpolation schemes are
///                      supported.
class Tabulated : public virtual Model
{
   public:
    /// @brief Constructor.
    ///
    /// @code{.cpp}
    ///     Array<State> states = { ... };
    ///     Tabulated tabulated(states, Interpolator::Type::Linear);
    /// @endcode
    ///
    /// @param aStateArray An array of states defining the tabulated trajectory.
    /// @param anInterpolationType The interpolation type to use between states.
    Tabulated(
        const Array<State>& aStateArray,
        const Interpolator::Type& anInterpolationType = DEFAULT_TABULATED_TRAJECTORY_INTERPOLATION_TYPE
    );

    /// @brief Clone the tabulated model.
    ///
    /// @code{.cpp}
    ///     Tabulated* cloned = tabulated.clone();
    /// @endcode
    ///
    /// @return A pointer to a cloned Tabulated instance.
    virtual Tabulated* clone() const override;

    /// @brief Equality operator.
    ///
    /// @code{.cpp}
    ///     bool equal = tabulatedA == tabulatedB;
    /// @endcode
    ///
    /// @param aTabulatedModel A tabulated model to compare against.
    /// @return True if the two tabulated models are equal.
    bool operator==(const Tabulated& aTabulatedModel) const;

    /// @brief Inequality operator.
    ///
    /// @code{.cpp}
    ///     bool notEqual = tabulatedA != tabulatedB;
    /// @endcode
    ///
    /// @param aTabulatedModel A tabulated model to compare against.
    /// @return True if the two tabulated models are not equal.
    bool operator!=(const Tabulated& aTabulatedModel) const;

    /// @brief Stream insertion operator.
    ///
    /// @code{.cpp}
    ///     std::cout << tabulated;
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param aTabulatedModel A tabulated model to stream.
    /// @return A reference to the output stream.
    friend std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aTabulatedModel);

    /// @brief Check if the tabulated model is defined.
    ///
    /// @code{.cpp}
    ///     bool defined = tabulated.isDefined();
    /// @endcode
    ///
    /// @return True if the tabulated model is defined.
    virtual bool isDefined() const override;

    /// @brief Get the time interval spanned by the tabulated states.
    ///
    /// @code{.cpp}
    ///     Interval interval = tabulated.getInterval();
    /// @endcode
    ///
    /// @return The time interval from the first to the last state.
    Interval getInterval() const;

    /// @brief Get the interpolation type used by the tabulated model.
    ///
    /// @code{.cpp}
    ///     Interpolator::Type type = tabulated.getInterpolationType();
    /// @endcode
    ///
    /// @return The interpolation type.
    Interpolator::Type getInterpolationType() const;

    /// @brief Get the first state in the tabulated trajectory.
    ///
    /// @code{.cpp}
    ///     State first = tabulated.getFirstState();
    /// @endcode
    ///
    /// @return The first state.
    State getFirstState() const;

    /// @brief Get the last state in the tabulated trajectory.
    ///
    /// @code{.cpp}
    ///     State last = tabulated.getLastState();
    /// @endcode
    ///
    /// @return The last state.
    State getLastState() const;

    /// @brief Calculate the state at a given instant.
    ///
    /// @code{.cpp}
    ///     State state = tabulated.calculateStateAt(instant);
    /// @endcode
    ///
    /// @param anInstant An instant at which to calculate the state.
    /// @return The interpolated state at the given instant.
    virtual State calculateStateAt(const Instant& anInstant) const override;

    /// @brief Calculate the states at a given array of instants.
    ///
    /// @code{.cpp}
    ///     Array<State> states = tabulated.calculateStatesAt(instants);
    /// @endcode
    ///
    /// @param anInstantArray An array of instants at which to calculate states.
    /// @return An array of interpolated states at the given instants.
    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const override;

    /// @brief Print the tabulated model to an output stream.
    ///
    /// @code{.cpp}
    ///     tabulated.print(std::cout, true);
    /// @endcode
    ///
    /// @param anOutputStream An output stream.
    /// @param displayDecorator If true, display a decorator around the output.
    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

    /// @brief Load a tabulated trajectory from a file.
    ///
    /// @code{.cpp}
    ///     Tabulated tabulated = Tabulated::Load(File::Path(Path::Parse("/path/to/file.csv")));
    /// @endcode
    ///
    /// @param aFile A file containing tabulated state data.
    /// @return A Tabulated trajectory model loaded from the file.
    static Tabulated Load(const File& aFile);

   protected:
    virtual bool operator==(const Model& aModel) const override;

    virtual bool operator!=(const Model& aModel) const override;

   private:
    State firstState_ = State::Undefined();
    State lastState_ = State::Undefined();
    Array<Shared<const Interpolator>> interpolators_;
};

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
