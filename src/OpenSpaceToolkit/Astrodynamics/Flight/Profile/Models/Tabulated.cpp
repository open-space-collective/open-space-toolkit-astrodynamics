/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformation/Rotation/Quaternion.hpp>

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

using ostk::core::types::Real;

using ostk::mathematics::object::Vector3d;
using ostk::mathematics::geometry::d3::transformation::rotation::Quaternion;

using ostk::physics::coordinate::Position;
using ostk::physics::coordinate::Velocity;
using ostk::physics::time::Duration;

Tabulated::Tabulated(const Array<State>& aStateArray, const Interpolator::Type& anInterpolatorType)
    : Model(),
      states_(aStateArray),
      stateBuilder_(aStateArray.accessFirst().accessFrame(), aStateArray.accessFirst().accessCoordinatesBroker())
{
    Array<State> stateArray = aStateArray;

    std::sort(
        stateArray.begin(),
        stateArray.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.getInstant() < rhs.getInstant();
        }
    );

    firstState_ = stateArray.accessFirst();
    lastState_ = stateArray.accessLast();

    VectorXd timestamps(stateArray.getSize());
    MatrixXd coordinates(stateArray.getSize(), 9);  // 3 Position, 3 Velocity, 3 Angular Velocity
    // TBI: Should we just interpolate Quaternions directly or should we use SLERP?

    for (Index i = 0; i < stateArray.getSize(); ++i)
    {
        timestamps(i) = (stateArray[i].accessInstant() - stateArray.accessFirst().accessInstant()).inSeconds();

        VectorXd coordinatesRow(9);
        coordinatesRow << stateArray[i].accessCoordinates()[0], stateArray[i].accessCoordinates()[1],
            stateArray[i].accessCoordinates()[2], stateArray[i].accessCoordinates()[3],
            stateArray[i].accessCoordinates()[4], stateArray[i].accessCoordinates()[5],
            stateArray[i].accessCoordinates()[10], stateArray[i].accessCoordinates()[11],
            stateArray[i].accessCoordinates()[12];
        coordinates.row(i) = coordinatesRow
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(Interpolator::GenerateInterpolator(anInterpolatorType, timestamps, coordinates.col(i)));
    }
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
    return !interpolators_.isEmpty() && firstState_.isDefined() && lastState_.isDefined();
}

Interval Tabulated::getInterval() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return Interval::Closed(firstState_.accessInstant(), lastState_.accessInstant());
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

    if (anInstant < firstState_.accessInstant() || anInstant > lastState_.accessInstant())
    {
        throw ostk::core::error::RuntimeError(String::Format(
            "Provided instant [{}] is outside of interpolation range [{}, {}].",
            anInstant.toString(),
            firstState_.accessInstant().toString(),
            lastState_.accessInstant().toString()
        ));
    }

    VectorXd interpolatedCoordinates(interpolators_.getSize());

    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        interpolatedCoordinates(i) = interpolators_[i]->evaluate((anInstant - firstState_.accessInstant()).inSeconds());
    }

    const Shared<const Frame>& frame = firstState_.accessFrame();
    const Shared<const CoordinatesBroker>& coordinatesBroker = firstState_.accessCoordinatesBroker();

    return State(anInstant, interpolatedCoordinates, frame, coordinatesBroker);

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

}  // namespace model
}  // namespace profile
}  // namespace flight
}  // namespace astrodynamics
}  // namespace ostk
