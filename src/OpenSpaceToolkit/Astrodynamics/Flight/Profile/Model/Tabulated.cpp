/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Dynamic.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AngularVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AttitudeQuaternion.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>

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

using ostk::core::type::Size;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;
using ostk::physics::coordinate::Transform;

using ostk::astrodynamics::trajectory::state::CoordinateSubset;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AngularVelocity;
using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;

Tabulated::Tabulated(const Array<State>& aStateArray, const Interpolator::Type& anInterpolatorType)
    : Model(),
      interpolatorType_(anInterpolatorType),
      stateBuilder_(StateBuilder::Undefined()),
      reducedStateBuilder_(StateBuilder::Undefined())
{
    setMembers(aStateArray);
}

Tabulated::Tabulated(
    const Array<State>& aStateArray,
    const Map<Shared<const CoordinateSubset>, Interpolator::Type>& anInterpolationTypeMap
)
    : Model(),
      interpolatorType_(Interpolator::Type::BarycentricRational),
      stateBuilder_(StateBuilder::Undefined()),
      reducedStateBuilder_(StateBuilder::Undefined())
{
    setMembers(aStateArray, anInterpolationTypeMap);
}

Tabulated* Tabulated::clone() const
{
    return new Tabulated(*this);
}

bool Tabulated::operator==(const Tabulated& aTabulatedModel) const
{
    if ((!this->isDefined()) || (!aTabulatedModel.isDefined()))
    {
        return false;
    }

    return this->stateArray_ == aTabulatedModel.stateArray_ && this->stateBuilder_ == aTabulatedModel.stateBuilder_ &&
           this->interpolators_ == aTabulatedModel.interpolators_;
}

bool Tabulated::operator!=(const Tabulated& aTabulatedModel) const
{
    return !((*this) == aTabulatedModel);
}

std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aTabulatedModel)
{
    aTabulatedModel.print(anOutputStream);

    return anOutputStream;
}

bool Tabulated::isDefined() const
{
    return !this->interpolators_.isEmpty() && !this->stateArray_.isEmpty() && this->stateBuilder_.isDefined();
}

Interval Tabulated::getInterval() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return Interval::Closed(
        this->stateArray_.accessFirst().accessInstant(), this->stateArray_.accessLast().accessInstant()
    );
}

Interpolator::Type Tabulated::getInterpolatorType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return this->interpolatorType_;
}

State Tabulated::calculateStateAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    const Interval interval = this->getInterval();

    if (anInstant < interval.accessStart() || anInstant > interval.accessEnd())
    {
        throw ostk::core::error::RuntimeError(
            String::Format(
                "Provided instant [{}] is outside of interpolation range [{}, {}].",
                anInstant.toString(),
                interval.accessStart().toString(),
                interval.accessEnd().toString()
            )
        );
    }

    VectorXd reducedCoordinates(reducedStateBuilder_.getSize());

    const double durationSeconds = (anInstant - interval.accessStart()).inSeconds();

    // Loop through all coordinate subsets except the last one (attitude quaternion)
    for (Size coordinateIndex = 0; coordinateIndex < interpolators_.getSize(); ++coordinateIndex)
    {
        const auto& interpolator = this->interpolators_.at(coordinateIndex);
        reducedCoordinates(coordinateIndex) = interpolator->evaluate(durationSeconds);
    }

    // Interpolate the attitude quaternion
    // find the state before and after the instant
    // for an std vector
    const auto it = std::lower_bound(
        this->stateArray_.begin(),
        this->stateArray_.end(),
        anInstant,
        [](const State& state, const Instant& instant)
        {
            return state.accessInstant() < instant;
        }
    );

    Quaternion qAtInstant = Quaternion::Undefined();

    if (it == this->stateArray_.begin())
    {
        qAtInstant = stateArray_.accessFirst().getAttitude();
    }
    else if (it == this->stateArray_.end())
    {
        qAtInstant = stateArray_.accessLast().getAttitude();
    }
    else
    {
        const State& stateBefore = *(it - 1);
        const State& stateAfter = *it;

        const Quaternion qBefore = stateBefore.getAttitude();
        const Quaternion qAfter = stateAfter.getAttitude();

        const double ratio = (anInstant - stateBefore.accessInstant()).inSeconds() /
                             (stateAfter.accessInstant() - stateBefore.accessInstant()).inSeconds();
        qAtInstant = Quaternion::SLERP(qBefore, qAfter, ratio);
    }

    const State reducedState = reducedStateBuilder_.build(anInstant, reducedCoordinates);

    const State attitudeQuaternionState = {
        anInstant,
        qAtInstant.toNormalized().toVector(Quaternion::Format::XYZS),
        reducedStateBuilder_.accessFrame(),
        {AttitudeQuaternion::Default()},
    };

    return stateBuilder_.expand(reducedState, attitudeQuaternionState);
}

Axes Tabulated::getAxesAt(const Instant& anInstant) const
{
    if (!anInstant.isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Instant");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    const State state = this->calculateStateAt(anInstant);
    const Quaternion q_GCRF_B = state.getAttitude().toConjugate();

    const Vector3d xAxis = q_GCRF_B * Vector3d::X();
    const Vector3d yAxis = q_GCRF_B * Vector3d::Y();
    const Vector3d zAxis = q_GCRF_B * Vector3d::Z();

    return {xAxis, yAxis, zAxis, state.accessFrame()};
}

Shared<const Frame> Tabulated::constructBodyFrame(const String& aFrameName) const
{
    if (aFrameName.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Body frame name");
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    const DynamicProvider dynamicTransformProvider = {
        [this](const Instant& anInstant) -> Transform
        {
            const State state = this->calculateStateAt(anInstant);

            return Transform::Passive(
                anInstant,
                -state.getPosition().getCoordinates(),
                -state.getVelocity().getCoordinates(),
                state.getAttitude().normalize(),
                state.getAngularVelocity()
            );
        }
    };

    return Frame::Construct(
        aFrameName,
        false,
        this->stateBuilder_.accessFrame(),
        std::make_shared<const DynamicProvider>(dynamicTransformProvider)
    );
}

void Tabulated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Tabulated") : void();

    ostk::core::utils::Print::Line(anOutputStream)
        << "Start instant:" << (this->isDefined() ? this->getInterval().accessStart().toString() : "Undefined");
    ostk::core::utils::Print::Line(anOutputStream)
        << "End instant:" << (this->isDefined() ? this->getInterval().accessEnd().toString() : "Undefined");

    ostk::core::utils::Print::Separator(anOutputStream);

    {
        const State firstState = this->isDefined() ? stateArray_.accessFirst() : State::Undefined();

        ostk::core::utils::Print::Line(anOutputStream)
            << "First state:"
            << (firstState.isDefined() ? String::Format(
                                             "{} - {} - {}",
                                             firstState.accessInstant().toString(),
                                             firstState.getPosition().toString(),
                                             firstState.getVelocity().toString()
                                         )
                                       : "Undefined");
    }

    {
        const State lastState = this->isDefined() ? stateArray_.accessLast() : State::Undefined();

        ostk::core::utils::Print::Line(anOutputStream)
            << "Last state:"
            << (lastState.isDefined() ? String::Format(
                                            "{} - {} - {}",
                                            lastState.accessInstant().toString(),
                                            lastState.getPosition().toString(),
                                            lastState.getVelocity().toString()
                                        )
                                      : "Undefined");
    }

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

Tabulated Tabulated::Default(const Array<State>& aStateArray)
{
    return Tabulated(
        aStateArray,
        {
            {CartesianPosition::Default(), Interpolator::Type::BarycentricRational},
            {CartesianVelocity::Default(), Interpolator::Type::BarycentricRational},
            {AngularVelocity::Default(), Interpolator::Type::BarycentricRational},
            // AttitudeQuaternion is always interpolated using SLERP
        }
    );
}

bool Tabulated::operator==(const Model& aModel) const
{
    const Tabulated* tabulatedModelPtr = dynamic_cast<const Tabulated*>(&aModel);

    return (tabulatedModelPtr != nullptr) && this->operator==(*tabulatedModelPtr);
}

bool Tabulated::operator!=(const Model& aModel) const
{
    return !((*this) == aModel);
}

void Tabulated::setMembers(const Array<State>& aStateArray)
{
    VectorXd timestamps;
    MatrixXd coordinates;

    this->computeReducedInterpolationData(aStateArray, timestamps, coordinates);

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(Interpolator::GenerateInterpolator(interpolatorType_, timestamps, coordinates.col(i)));
    }
}

void Tabulated::setMembers(
    const Array<State>& aStateArray,
    const Map<Shared<const CoordinateSubset>, Interpolator::Type>& anInterpolationTypeMap
)
{
    VectorXd timestamps;
    MatrixXd coordinates;

    this->computeReducedInterpolationData(aStateArray, timestamps, coordinates);

    // Index the requested interpolation types by coordinate subset id. Entries for coordinate subsets that are not
    // present in the (reduced) states are ignored.
    Map<String, Interpolator::Type> interpolationTypeBySubsetId;

    for (const auto& [coordinateSubsetSPtr, interpolationType] : anInterpolationTypeMap)
    {
        interpolationTypeBySubsetId[coordinateSubsetSPtr->getId()] = interpolationType;
    }

    // Resolve the interpolation type for each reduced coordinate. The attitude quaternion is excluded from the
    // reduced state and is always interpolated using SLERP, so any entry for it in the map is ignored.
    Array<Interpolator::Type> interpolationTypePerCoordinate = Array<Interpolator::Type>::Empty();
    interpolationTypePerCoordinate.reserve(coordinates.cols());

    for (const auto& coordinateSubsetSPtr : reducedStateBuilder_.getCoordinateSubsets())
    {
        const auto interpolationTypeIt = interpolationTypeBySubsetId.find(coordinateSubsetSPtr->getId());

        if (interpolationTypeIt == interpolationTypeBySubsetId.end())
        {
            throw ostk::core::error::RuntimeError(
                String::Format(
                    "No interpolation type was provided for the coordinate subset [{}].",
                    coordinateSubsetSPtr->getName()
                )
            );
        }

        for (Size i = 0; i < coordinateSubsetSPtr->getSize(); ++i)
        {
            interpolationTypePerCoordinate.add(interpolationTypeIt->second);
        }
    }

    // Report the interpolator type of the first reduced coordinate via getInterpolatorType().
    if (!interpolationTypePerCoordinate.isEmpty())
    {
        interpolatorType_ = interpolationTypePerCoordinate.accessFirst();
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(
            Interpolator::GenerateInterpolator(interpolationTypePerCoordinate[i], timestamps, coordinates.col(i))
        );
    }
}

void Tabulated::computeReducedInterpolationData(
    const Array<State>& aStateArray, VectorXd& aTimestampVector, MatrixXd& aReducedCoordinateMatrix
)
{
    if (aStateArray.getSize() < 2)
    {
        throw ostk::core::error::RuntimeError("State array must have at least length 2.");
    }

    const State& firstState = aStateArray.accessFirst();
    stateBuilder_ = StateBuilder(firstState);

    Array<Shared<const CoordinateSubset>> reducedCoordinateSubsets = firstState.getCoordinateSubsets();
    reducedCoordinateSubsets.remove(AttitudeQuaternion::Default());
    reducedStateBuilder_ = StateBuilder(firstState.accessFrame(), reducedCoordinateSubsets);

    // Ensure the states are sorted by instant
    stateArray_ = aStateArray;

    std::sort(
        stateArray_.begin(),
        stateArray_.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.getInstant() < rhs.getInstant();
        }
    );

    aTimestampVector.resize(stateArray_.getSize());
    aReducedCoordinateMatrix.resize(stateArray_.getSize(), reducedStateBuilder_.getSize());  // Exclude quaternion

    for (Index i = 0; i < stateArray_.getSize(); ++i)
    {
        aTimestampVector(i) = (stateArray_[i].accessInstant() - firstState.accessInstant()).inSeconds();

        aReducedCoordinateMatrix.row(i) = reducedStateBuilder_.reduce(stateArray_[i]).accessCoordinates();
    }
}

}  // namespace model
}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
