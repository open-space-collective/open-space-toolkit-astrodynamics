/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/Dynamic.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Model/Tabulated.hpp>

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

using ostk::core::container::Unpack;
using ostk::core::type::Real;
using ostk::core::type::String;

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;
using ostk::mathematics::object::Vector3d;

using DynamicProvider = ostk::physics::coordinate::frame::provider::Dynamic;
using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Transform;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;

Tabulated::Tabulated(const Array<State>& aStateArray)
    : Model(),
      states_(aStateArray),
      stateIndex_(0)
{
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

    return this->states_ == aTabulatedModel.states_;
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
    return !this->states_.isEmpty();
}

Interval Tabulated::getInterval() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return Interval::Closed(this->states_.accessFirst().accessInstant(), this->states_.accessLast().accessInstant());
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

    const Pair<const State*, const State*> stateRange = this->accessStateRangeAt(anInstant);

    if ((stateRange.first != nullptr) && (stateRange.second != nullptr))
    {
        const State& previousState = *(stateRange.first);
        const State& nextState = *(stateRange.second);

        const Real ratio = Duration::Between(previousState.accessInstant(), anInstant).inSeconds() /
                           Duration::Between(previousState.accessInstant(), nextState.accessInstant()).inSeconds();

        return {
            anInstant,
            Position {
                previousState.getPosition().accessCoordinates() +
                    ratio *
                        (nextState.getPosition().accessCoordinates() - previousState.getPosition().accessCoordinates()),
                previousState.getPosition().getUnit(),
                previousState.getPosition().accessFrame()
            },
            Velocity {
                previousState.getVelocity().accessCoordinates() +
                    ratio *
                        (nextState.getVelocity().accessCoordinates() - previousState.getVelocity().accessCoordinates()),
                previousState.getVelocity().getUnit(),
                previousState.getVelocity().accessFrame()
            },
            Quaternion::SLERP(previousState.getAttitude(), nextState.getAttitude(), ratio),
            previousState.getAngularVelocity() +
                ratio * (nextState.getAngularVelocity() - previousState.getAngularVelocity()),
            previousState.getFrame()
        };
    }
    else if (stateRange.first != nullptr)
    {
        return *(stateRange.first);
    }
    else if (stateRange.second != nullptr)
    {
        return *(stateRange.second);
    }

    throw ostk::core::error::RuntimeError("Cannot calculate state at [{}].", anInstant.toString());

    return State::Undefined();
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
    const Quaternion q_GCRF_BODY = state.getAttitude().toConjugate();

    const Vector3d xAxis = q_GCRF_BODY * Vector3d::X();
    const Vector3d yAxis = q_GCRF_BODY * Vector3d::Y();
    const Vector3d zAxis = q_GCRF_BODY * Vector3d::Z();

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
        this->states_.accessFirst().accessFrame(),
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

Pair<const State*, const State*> Tabulated::accessStateRangeAt(const Instant& anInstant) const
{
    State const* previousStatePtr = nullptr;
    State const* nextStatePtr = nullptr;

    while (true)  // To be improved
    {
        Unpack(previousStatePtr, nextStatePtr) = this->accessStateRangeAtIndex(stateIndex_);  // Check index cache

        if ((previousStatePtr != nullptr) && (nextStatePtr != nullptr))
        {
            if ((previousStatePtr->accessInstant() <= anInstant) && (anInstant <= nextStatePtr->accessInstant()))
            {
                if (previousStatePtr->accessInstant() == anInstant)
                {
                    return {nullptr, previousStatePtr};
                }
                else if (nextStatePtr->accessInstant() == anInstant)
                {
                    return {nextStatePtr, nullptr};
                }

                return {previousStatePtr, nextStatePtr};
            }
            else
            {
                if (anInstant < previousStatePtr->accessInstant())
                {
                    if (stateIndex_ > 0)
                    {
                        stateIndex_--;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if ((states_.getSize() > 0) && (stateIndex_ < (states_.getSize() - 1)))
                    {
                        stateIndex_++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else if (previousStatePtr != nullptr)
        {
            if (previousStatePtr->accessInstant() == anInstant)
            {
                return {nullptr, previousStatePtr};
            }
            else
            {
                if (anInstant < previousStatePtr->accessInstant())
                {
                    if (stateIndex_ > 0)
                    {
                        stateIndex_--;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    return {nullptr, nullptr};
                }
            }
        }
        else
        {
            stateIndex_ = 0;

            break;
        }
    }

    return {nullptr, nullptr};
}

Pair<const State*, const State*> Tabulated::accessStateRangeAtIndex(const Index& anIndex) const
{
    const State* previousStatePtr = (anIndex < states_.getSize()) ? &(states_.at(anIndex)) : nullptr;
    const State* nextStatePtr = ((anIndex + 1) < states_.getSize()) ? &(states_.at(anIndex + 1)) : nullptr;

    return {previousStatePtr, nextStatePtr};
}

}  // namespace model
}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
