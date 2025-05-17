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
    Tabulated(
        const Array<State>& aStateArray,
        const Interpolator::Type& anInterpolationType = DEFAULT_TABULATED_TRAJECTORY_INTERPOLATION_TYPE
    );

    virtual Tabulated* clone() const override;

    bool operator==(const Tabulated& aTabulatedModel) const;

    bool operator!=(const Tabulated& aTabulatedModel) const;

    friend std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aTabulatedModel);

    virtual bool isDefined() const override;

    Interval getInterval() const;

    Interpolator::Type getInterpolationType() const;

    State getFirstState() const;

    State getLastState() const;

    virtual State calculateStateAt(const Instant& anInstant) const override;

    virtual Array<State> calculateStatesAt(const Array<Instant>& anInstantArray) const override;

    virtual void print(std::ostream& anOutputStream, bool displayDecorator = true) const override;

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
