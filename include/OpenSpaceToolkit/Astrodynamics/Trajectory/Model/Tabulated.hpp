/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Tabulated__
#define __OpenSpaceToolkit_Astrodynamics_Trajectory_Model_Tabulated__

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Container/Pair.hpp>
#include <OpenSpaceToolkit/Core/FileSystem/File.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/CurveFitting/Interpolator.hpp>
#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Interval.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

using ostk::core::container::Array;
using ostk::core::container::Map;
using ostk::core::container::Pair;
using ostk::core::filesystem::File;
using ostk::core::type::Index;
using ostk::core::type::Shared;
using ostk::core::type::Size;

using ostk::mathematics::curvefitting::Interpolator;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::time::Instant;
using ostk::physics::time::Interval;
using ostk::physics::time::Scale;

using ostk::astrodynamics::trajectory::Model;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::CoordinateSubset;

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

    /// @brief Constructor with an explicit output frame.
    ///
    /// @code{.cpp}
    ///     Array<State> states = { ... };
    ///     Tabulated tabulated(states, Interpolator::Type::Linear, Frame::ITRF());
    /// @endcode
    ///
    /// @param aStateArray An array of states defining the tabulated trajectory.
    /// @param anInterpolationType The interpolation type to use between states.
    /// @param aFrameSPtr The reference frame in which the computed states are expressed. The provided states are
    /// converted to this frame and interpolation is performed in this frame.
    Tabulated(
        const Array<State>& aStateArray,
        const Interpolator::Type& anInterpolationType,
        const Shared<const Frame>& aFrameSPtr
    );

    /// @brief Constructor with per-coordinate-subset interpolation types.
    ///
    ///                      Each coordinate is interpolated using the interpolation type associated with the
    ///                      coordinate subset it belongs to.
    ///
    /// @code{.cpp}
    ///     Array<State> states = { ... };
    ///     Map<Shared<const CoordinateSubset>, Interpolator::Type> interpolationTypes = {
    ///         {CartesianPosition::Default(), Interpolator::Type::CubicSpline},
    ///         {CartesianVelocity::Default(), Interpolator::Type::Linear},
    ///     };
    ///     Tabulated tabulated(states, interpolationTypes);
    /// @endcode
    ///
    /// @param aStateArray An array of states defining the tabulated trajectory.
    /// @param anInterpolationTypeMap A mapping from coordinate subset to the interpolation type to use for that
    /// subset's coordinates. Every coordinate subset present in the states must have an entry in the map (an error is
    /// raised otherwise). Entries for coordinate subsets that are not present in the states are ignored.
    Tabulated(
        const Array<State>& aStateArray,
        const Map<Shared<const CoordinateSubset>, Interpolator::Type>& anInterpolationTypeMap
    );

    /// @brief Constructor with per-coordinate-subset interpolation types and an explicit output frame.
    ///
    /// @param aStateArray An array of states defining the tabulated trajectory.
    /// @param anInterpolationTypeMap A mapping from coordinate subset to the interpolation type to use for that
    /// subset's coordinates. Every coordinate subset present in the states must have an entry in the map (an error is
    /// raised otherwise). Entries for coordinate subsets that are not present in the states are ignored.
    /// @param aFrameSPtr The reference frame in which the computed states are expressed. The provided states are
    /// converted to this frame and interpolation is performed in this frame.
    Tabulated(
        const Array<State>& aStateArray,
        const Map<Shared<const CoordinateSubset>, Interpolator::Type>& anInterpolationTypeMap,
        const Shared<const Frame>& aFrameSPtr
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

    /// @brief Get the reference frame in which the computed states are expressed.
    ///
    /// @code{.cpp}
    ///     Shared<const Frame> frame = tabulated.getFrame();
    /// @endcode
    ///
    /// @return The output reference frame.
    Shared<const Frame> getFrame() const;

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
    /// @note When the model was constructed with per-coordinate-subset interpolation types, this returns the
    /// interpolation type of the first coordinate subset.
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

    /// @brief Construct a tabulated model using the default per-coordinate-subset interpolation types.
    ///
    ///                      Each coordinate subset present in the states is interpolated using the type given by
    ///                      DefaultInterpolationTypes(). The states may contain any subset of those coordinate subsets.
    ///
    /// @code{.cpp}
    ///     Array<State> states = { ... };
    ///     Tabulated tabulated = Tabulated::Default(states);
    /// @endcode
    ///
    /// @param aStateArray An array of states defining the tabulated trajectory.
    /// @return A Tabulated trajectory model using the default interpolation types.
    static Tabulated Default(const Array<State>& aStateArray);

    /// @brief Construct a tabulated model using the default per-coordinate-subset interpolation types and an explicit
    /// output frame.
    ///
    /// @param aStateArray An array of states defining the tabulated trajectory.
    /// @param aFrameSPtr The reference frame in which the computed states are expressed.
    /// @return A Tabulated trajectory model using the default interpolation types.
    static Tabulated Default(const Array<State>& aStateArray, const Shared<const Frame>& aFrameSPtr);

    /// @brief Get the default interpolation type to use for each well-known coordinate subset.
    ///
    /// @details Position, velocity, acceleration, attitude, angular velocity and mass use barycentric rational
    /// interpolation; drag coefficient, surface area, mass flow rate and ballistic coefficient use zero-order
    /// interpolation.
    ///
    /// @return A mapping from coordinate subset to its default interpolation type.
    static Map<Shared<const CoordinateSubset>, Interpolator::Type> DefaultInterpolationTypes();

   protected:
    virtual bool operator==(const Model& aModel) const override;

    virtual bool operator!=(const Model& aModel) const override;

   private:
    State firstState_ = State::Undefined();
    State lastState_ = State::Undefined();
    Array<Shared<const Interpolator>> interpolators_;
    Shared<const Frame> outputFrameSPtr_ = Frame::GCRF();

    /// @brief Sort the provided states by instant, cache the first and last states (in their native frame), and
    /// compute the interpolation timestamps and coordinate matrix shared by all constructors. The states are
    /// expressed in the output frame before their coordinates are extracted, so that interpolation is performed
    /// directly in the output frame.
    ///
    /// @param aStateArray An array of states defining the tabulated trajectory.
    /// @param aTimestampVector [out] The timestamps (in seconds, relative to the first state) of the sorted states.
    /// @param aCoordinateMatrix [out] The coordinates of the sorted states, expressed in the output frame (one row
    /// per state, one column per coordinate).
    /// @return True if the model could be defined (i.e. at least two states were provided), false otherwise.
    bool computeInterpolationData(
        const Array<State>& aStateArray, VectorXd& aTimestampVector, MatrixXd& aCoordinateMatrix
    );
};

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk

#endif
