/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Dynamic.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>
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

using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;
using ostk::physics::coordinate::Transform;

using ostk::astrodynamics::trajectory::state::coordinatesubset::AttitudeQuaternion;

Tabulated::Tabulated(const Array<State>& aStateArray, const Interpolator::Type& anInterpolatorType)
    : Model(),
      tabulated_(aStateArray, anInterpolatorType),
      stateBuilder_(StateBuilder::Undefined())
{
    if (aStateArray.getSize() < 2)
    {
        return;
    }

    stateBuilder_ = StateBuilder(aStateArray.accessFirst());
}

Tabulated::Tabulated(const Array<State>& aStateArray)
    : Model(),
      tabulated_(aStateArray, Interpolator::Type::Linear),
      stateBuilder_(StateBuilder::Undefined())
{
    if (aStateArray.getSize() < 2)
    {
        return;
    }

    stateBuilder_ = StateBuilder(aStateArray.accessFirst());
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

    return this->tabulated_ == aTabulatedModel.tabulated_;
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
    return this->tabulated_.isDefined() && this->stateBuilder_.isDefined();
}

Interval Tabulated::getInterval() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return this->tabulated_.getInterval();
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

    const State state = this->tabulated_.calculateStateAt(anInstant);

    VectorXd coordinates = state.getCoordinates();

    coordinates.segment(state.accessCoordinateBroker()->getSubsetIndex(AttitudeQuaternion::Default()), 4) = state.getAttitude().toNormalized().toVector(Quaternion::Format::XYZS);

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
                Vector3d::Zero()
            );
        }
    };

    return Frame::Construct(
        aFrameName,
        false,
        this->tabulated_.getFirstState().accessFrame(),
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
        const State firstState =
            this->isDefined() ? this->calculateStateAt(this->getInterval().accessStart()) : State::Undefined();

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
        const State lastState =
            this->isDefined() ? this->calculateStateAt(this->getInterval().accessEnd()) : State::Undefined();

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

}  // namespace model
}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
