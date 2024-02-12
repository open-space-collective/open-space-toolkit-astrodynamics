/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Flight/Profile/Models/Tabulated.hpp>

namespace ostk
{
namespace astro
{
namespace flight
{
namespace profile
{
namespace models
{

using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

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
    using ostk::core::type::Real;

    using ostk::mathematics::object::Vector3d;

    using ostk::physics::coordinate::Position;
    using ostk::physics::coordinate::Velocity;
    using ostk::physics::time::Duration;

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

    throw ostk::core::error::runtime::ToBeImplemented("Tabulated::getAxesAt");

    return Axes::Undefined();
}

Shared<const Frame> Tabulated::getBodyFrame(const String& aFrameName) const
{
    if (aFrameName.isEmpty())
    {
        throw ostk::core::error::runtime::Undefined("Body frame name");
    }

    throw ostk::core::error::runtime::ToBeImplemented("Tabulated::getBodyFrame");

    return nullptr;
}

void Tabulated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    using ostk::core::type::String;

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
    using ostk::core::container::Unpack;

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

}  // namespace models
}  // namespace profile
}  // namespace flight
}  // namespace astro
}  // namespace ostk
