/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Model/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace trajectory
{
namespace model
{

Tabulated::Tabulated(const Array<State>& aStateArray, const Interpolator::Type& anInterpolationType)
    : Model()
{
    if (aStateArray.getSize() < 2)
    {
        return;
    }

    Array<State> stateArray = aStateArray;

    std::sort(
        stateArray.begin(),
        stateArray.end(),
        [](const auto& lhs, const auto& rhs)
        {
            return lhs.getInstant() < rhs.getInstant();
        }
    );

    firstState_ = aStateArray.accessFirst();
    lastState_ = aStateArray.accessLast();

    VectorXd timestamps(stateArray.getSize());
    MatrixXd coordinates(stateArray.getSize(), firstState_.getSize());

    for (Index i = 0; i < stateArray.getSize(); ++i)
    {
        timestamps(i) = (stateArray[i].accessInstant() - firstState_.accessInstant()).inSeconds();

        coordinates.row(i) = stateArray[i].accessCoordinates();
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        interpolators_.add(Interpolator::GenerateInterpolator(anInterpolationType, timestamps, coordinates.col(i)));
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

    return this->getInterpolationType() == aTabulatedModel.getInterpolationType() &&
           firstState_ == aTabulatedModel.getFirstState() && lastState_ == aTabulatedModel.getLastState();
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

Interpolator::Type Tabulated::getInterpolationType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    // Since all interpolators are of the same type, we can just return the type of the first one.
    return interpolators_[0]->getInterpolationType();
}

State Tabulated::getFirstState() const
{
    return firstState_;
}

State Tabulated::getLastState() const
{
    return lastState_;
}

State Tabulated::calculateStateAt(const Instant& anInstant) const
{
    using ostk::core::type::Index;
    using ostk::core::type::String;

    using ostk::astrodynamics::trajectory::state::CoordinateBroker;

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
        throw ostk::core::error::RuntimeError(
            String::Format(
                "Provided instant [{}] is outside of interpolation range [{}, {}].",
                anInstant.toString(),
                firstState_.accessInstant().toString(),
                lastState_.accessInstant().toString()
            )
        );
    }

    VectorXd interpolatedCoordinates(interpolators_.getSize());

    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        interpolatedCoordinates(i) = interpolators_[i]->evaluate((anInstant - firstState_.accessInstant()).inSeconds());
    }

    const Shared<const Frame>& frame = firstState_.accessFrame();
    const Shared<const CoordinateBroker>& coordinatesBroker = firstState_.accessCoordinateBroker();

    return State(anInstant, interpolatedCoordinates, frame, coordinatesBroker).inFrame(Frame::GCRF());
}

Array<State> Tabulated::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    using ostk::core::type::Index;

    if (anInstantArray.isEmpty())
    {
        return Array<State>::Empty();
    }

    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    Array<State> stateArray = Array<State>(anInstantArray.getSize(), State::Undefined());

    for (Index i = 0; i < anInstantArray.getSize(); ++i)
    {
        stateArray[i] = this->calculateStateAt(anInstantArray[i]);
    }

    return stateArray;
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

}  // namespace model
}  // namespace trajectory
}  // namespace astrodynamics
}  // namespace ostk
