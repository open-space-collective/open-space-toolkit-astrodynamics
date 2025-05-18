/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Size.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Dynamic.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/AttitudeQuaternion.hpp>

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
using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;

Tabulated::Tabulated(const Array<State>& aStateArray, const Interpolator::Type& anInterpolatorType)
    : Model(),
      stateArray_(aStateArray),
      stateBuilder_(StateBuilder::Undefined())
{
    setMembers(aStateArray, anInterpolatorType);
}

Tabulated::Tabulated(const Array<State>& aStateArray)
    : Model(),
      stateArray_(aStateArray),
      stateBuilder_(StateBuilder::Undefined())
{
    setMembers(aStateArray, Interpolator::Type::Linear);
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
        throw ostk::core::error::RuntimeError(String::Format(
            "Provided instant [{}] is outside of interpolation range [{}, {}].",
            anInstant.toString(),
            interval.accessStart().toString(),
            interval.accessEnd().toString()
        ));
    }

    VectorXd coordinates(stateBuilder_.getSize());

    // Interpolate the coordinates except for the attitude quaternion
    Size k = 0;
    for (Size i = 0; i < this->stateBuilder_.getCoordinateSubsets().getSize() - 1; ++i)
    {
        const auto coordinateSubset = this->stateBuilder_.getCoordinateSubsets().at(i);

        for (Size j = 0; j < coordinateSubset->getSize(); ++j)
        {
            const auto& interpolator = this->interpolators_.at(k);
            coordinates(k) = interpolator->evaluate((anInstant - interval.accessStart()).inSeconds());
            ++k;
        }
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

    coordinates.tail<4>() = qAtInstant.toNormalized().toVector(Quaternion::Format::XYZS);

    return stateBuilder_.build(anInstant, coordinates);
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

Shared<const Frame> Tabulated::getBodyFrame(const String& aFrameName) const
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

bool Tabulated::operator==(const Model& aModel) const
{
    const Tabulated* tabulatedModelPtr = dynamic_cast<const Tabulated*>(&aModel);

    return (tabulatedModelPtr != nullptr) && this->operator==(*tabulatedModelPtr);
}

bool Tabulated::operator!=(const Model& aModel) const
{
    return !((*this) == aModel);
}

void Tabulated::setMembers(const Array<State>& aStateArray, const Interpolator::Type& anInterpolatorType)
{
    if (aStateArray.getSize() < 2)
    {
        return;
    }

    const State& firstState = aStateArray.accessFirst();

    Array<Shared<const CoordinateSubset>> coordinateSubsets = firstState.getCoordinateSubsets();

    // ensure that the last coordinate subset is the attitude quaternion
    coordinateSubsets.remove(AttitudeQuaternion::Default());
    coordinateSubsets.add(AttitudeQuaternion::Default());

    stateBuilder_ = StateBuilder(aStateArray[0].accessFrame(), coordinateSubsets);

    Array<State> stateArray = aStateArray;

    std::sort(
        stateArray.begin(),
        stateArray.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.getInstant() < rhs.getInstant();
        }
    );

    stateArray_ = stateArray;

    VectorXd timestamps(stateArray.getSize());
    MatrixXd coordinates(stateArray.getSize(), firstState.getSize() - 4);  // Exclude quaternion

    for (Index i = 0; i < stateArray.getSize(); ++i)
    {
        timestamps(i) = (stateArray[i].accessInstant() - firstState.accessInstant()).inSeconds();

        VectorXd stateCoordinates(coordinates.cols());

        Index offset = 0;
        for (Index j = 0; j < coordinateSubsets.getSize() - 1; ++j)
        {
            const Size subsetSize = coordinateSubsets[j]->getSize();
            stateCoordinates.segment(offset, subsetSize) = stateArray[i].extractCoordinate(coordinateSubsets[j]);
            offset += subsetSize;
        }

        coordinates.row(i) = stateCoordinates;
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(Interpolator::GenerateInterpolator(anInterpolatorType, timestamps, coordinates.col(i)));
    }
}

}  // namespace model
}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
