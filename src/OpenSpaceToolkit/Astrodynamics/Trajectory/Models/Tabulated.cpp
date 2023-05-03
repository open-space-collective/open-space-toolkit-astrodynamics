// Copyright © Loft Orbital Solutions Inc.

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/Models/Tabulated.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{
namespace models
{

Tabulated::Tabulated(const Array<State>& aStateArray, const InterpolationType& anInterpolationType)
    : Model(), interpolationType_(anInterpolationType)
{
    using ostk::math::curvefitting::interp::BarycentricRational;
    using ostk::math::curvefitting::interp::CubicSpline;
    using ostk::math::curvefitting::interp::Linear;

    if (aStateArray.getSize() < 2)
    {
        return;
    }

    Array<State> stateArray = aStateArray;

    std::sort(stateArray.begin(), stateArray.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.getInstant() < rhs.getInstant();
    });

    firstState_ = aStateArray.accessFirst();
    lastState_ = aStateArray.accessLast();

    VectorXd timestamps(stateArray.getSize());
    MatrixXd coordinates(stateArray.getSize(), 6);

    for (Index i = 0; i < stateArray.getSize(); ++i)
    {
        timestamps(i) = (stateArray[i].accessInstant() - stateArray[0].accessInstant()).inSeconds();

        coordinates.row(i).segment<3>(0) = stateArray[i].accessPosition().accessCoordinates();
        coordinates.row(i).segment<3>(3) = stateArray[i].accessVelocity().accessCoordinates();
    }

    interpolators_.reserve(coordinates.cols());

    for (Index i = 0; i < Size(coordinates.cols()); ++i)
    {
        if (interpolationType_ == Tabulated::InterpolationType::CubicSpline)
        {
            interpolators_.add(std::make_shared<CubicSpline>(CubicSpline(timestamps, coordinates.col(i))));
        }
        else if (interpolationType_ == Tabulated::InterpolationType::BarycentricRational)
        {
            interpolators_.add(
                std::make_shared<BarycentricRational>(BarycentricRational(timestamps, coordinates.col(i))));
        }
        else if (interpolationType_ == Tabulated::InterpolationType::Linear)
        {
            interpolators_.add(std::make_shared<Linear>(Linear(timestamps, coordinates.col(i))));
        }
        else
        {
            throw ostk::core::error::runtime::Wrong("InterpolationType");
        }
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

    return interpolationType_ == aTabulatedModel.getInterpolationType() &&
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

Tabulated::InterpolationType Tabulated::getInterpolationType() const
{
    if (!this->isDefined())
    {
        throw ostk::core::error::runtime::Undefined("Tabulated");
    }

    return interpolationType_;
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
    using ostk::core::types::Index;
    using ostk::core::types::String;

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
            String::Format("Provided instant [{}] is outside of interpolation range [{}, {}].", anInstant.toString(),
                           firstState_.accessInstant().toString(), lastState_.accessInstant().toString()));
    }

    VectorXd interpolatedCoordinates(interpolators_.getSize());

    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        interpolatedCoordinates(i) = interpolators_[i]->evaluate((anInstant - firstState_.accessInstant()).inSeconds());
    }

    return State(
        anInstant, Position::Meters(interpolatedCoordinates.segment<3>(0), firstState_.accessPosition().accessFrame()),
        Velocity::MetersPerSecond(interpolatedCoordinates.segment<3>(3), firstState_.accessPosition().accessFrame()));
}

Array<State> Tabulated::calculateStatesAt(const Array<Instant>& anInstantArray) const
{
    using ostk::core::types::Index;

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
    using ostk::core::types::String;

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
            << (firstState.isDefined()
                    ? String::Format("{} - {} - {}", firstState.accessInstant().toString(),
                                     firstState.accessPosition().toString(), firstState.accessVelocity().toString())
                    : "Undefined");
    }

    {
        const State lastState =
            this->isDefined() ? this->calculateStateAt(this->getInterval().accessEnd()) : State::Undefined();

        ostk::core::utils::Print::Line(anOutputStream)
            << "Last state:"
            << (lastState.isDefined()
                    ? String::Format("{} - {} - {}", lastState.accessInstant().toString(),
                                     lastState.accessPosition().toString(), lastState.accessVelocity().toString())
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

}  // namespace models
}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
